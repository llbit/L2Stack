/*
    Copyright 2009 Jesper Öqvist (jesper@llbit.se)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
	license.txt for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "tet_incs.h"

#ifdef L2STACK_NET
int MPClient_Thread(void* mpclass)
{
	MPClient*	client = (MPClient*)mpclass;

	if (!client->Connect()) {
		client->SetError(string(SDLNet_GetError()));
		return 1;
	}
	if (!client->ReadSeed()) {
		client->SetError(string(SDLNet_GetError()));
		return 1;
	}
	client->NetRead();
	return 0;
}

MPClient::MPClient(System& sys, string ipaddr)
{
	thread_error = false;
	serveraddr = ipaddr;
	rot_mode = sys.rot_mode;
	seed = 0;

	netport = sys.netport;

	server = NULL;
	mutex = NULL;
	thread = NULL;
	frame_offset = 0;

	servergame = NULL;
	localgame = NULL;

	mutex = SDL_CreateMutex();
	thread = SDL_CreateThread(MPClient_Thread, this);
}

MPClient::~MPClient()
{
	int	status;

	Disconnect();

	if (thread != NULL && !thread_error) {
		SDL_WaitThread(thread, &status);
		if (status != 0) {
			throw new Message(error, new MPMenu());
		}
	}

	SDL_DestroyMutex(mutex);

	if (servergame != NULL) {
		delete servergame;
	}
	if (localgame != NULL) {
		delete localgame;
	}
}

bool MPClient::Connect()
{
	if (SDLNet_Init() == -1) {
		return false;
	}

	IPaddress ip;
	if(SDLNet_ResolveHost(&ip, serveraddr.c_str(), netport) == -1) {
		return false;
	}

	server = SDLNet_TCP_Open(&ip);
	if(!server) {
		return false;
	}
	return true;
}

void MPClient::Disconnect()
{
	SDLNet_TCP_Close(server);
	SDLNet_Quit();
}

bool MPClient::ReadSeed()
{
	char	data[4];
	int	tot = 0;
	int	ret;
	
	while (tot < 4) {
		ret = SDLNet_TCP_Recv(server, &data[tot], 4-tot);
		if (ret <= 0) {
			return false;
		}
		tot += ret;
	}

	SDL_mutexP(mutex);
	seed = SDLNet_Read32(data);
	SDL_mutexV(mutex);

	// Send OK signal 'K'
	char	signal = 'K';
	SDLNet_TCP_Send(server, &signal, 1);
	return true;
}

void MPClient::NetRead()
{
	while (true) {
		event_t	new_event;
		char	data[4];
		int	tot;
		int	ret;

		if (server == NULL) {
			return;
		}

		ret = SDLNet_TCP_Recv(server, &new_event.event, 1);
		if (ret <= 0) {
			return;
		}

		ret = SDLNet_TCP_Recv(server, &new_event.data, 1);
		if (ret <= 0) {
			return;
		}

		for (tot = 0; tot < 2; ) {
			ret = SDLNet_TCP_Recv(server, &data[tot], 2-tot);
			if (ret <= 0) {
				return;
			}
			tot += ret;
		}
		new_event.offset = SDLNet_Read16(data);

		SDL_mutexP(mutex);
		events.push_back(new_event);
		SDL_mutexV(mutex);
	}
}

void MPClient::Update(System& sys)
{
	SDL_mutexP(mutex);

	if (thread_error) {
		SDL_mutexV(mutex);
		throw new Message(error, new MPMenu());
		return;
	}

	if (localgame == NULL && seed != 0) {
		servergame = new MPGame(rot_mode);
		localgame = new MPGame(rot_mode);
	} else if (servergame != NULL) {
		if (!events.empty()) {
			while (!events.empty()) {
				if (!servergame->LockPiece(sys,
					(tetromino_t)events.front().data,
					(char)(events.front().offset >> 8),
					(char)(events.front().offset & 0xFF),
					(char)events.front().event)) {
						break;
				} else {
					events.pop_front();
				}
			}
		}

		int gqueue = localgame->GetGarbageQueue();
		if (gqueue > 0) {
			servergame->AddGarbage(gqueue, randserv, 2);
		}

		gqueue = servergame->GetGarbageQueue();
		if (gqueue > 0) {
			localgame->AddGarbage(gqueue, randserv, 3);
		}

		servergame->Update(sys, false);
		localgame->Update(sys, true);

		while (!localgame->history.empty()) {
			event_t	new_event;
			char	data[4];

			new_event.event = localgame->history.front().GetRot();
			new_event.data = localgame->history.front().GetTet();
			new_event.offset = (Uint8)(localgame->history.front().GetX()) << 8 |
				(Uint8)(localgame->history.front().GetY());
			
			SDLNet_TCP_Send(server, &new_event.event, 1);
			SDLNet_TCP_Send(server, &new_event.data, 1);
			SDLNet_Write16(new_event.offset, data);
			SDLNet_TCP_Send(server, data, 2);

			localgame->history.pop_front();
		}
	}

	SDL_mutexV(mutex);
}

void MPClient::Draw(Graphics& graphics)
{
	
	
	if (localgame != NULL && servergame != NULL) {
		glPushMatrix();
		glTranslatef(75.f, 0.f, 0.f);
		localgame->Draw(sys);
		glPopMatrix();

		glPushMatrix();
		glTranslatef((float)(System::WIDTH - 12*TILESIZE-75), 0.f, 0.f);
		servergame->Draw(sys);
		glPopMatrix();
	} else if (server == NULL) {
		glPushMatrix();
		glTranslatef(200.f, 150.f, 0.f);
		sys.DrawString(0, 20, "Connecting...", 1);
		glPopMatrix();
	} else {
		glPushMatrix();
		glTranslatef(200.f, 150.f, 0.f);
		sys.DrawString(0, 20, "Handshaking...", 1);
		glPopMatrix();
	}

	
}

void MPClient::OnKeyDown(System& sys, SDLKey sdlkey)
{
	gamekey_t key = sys.GetKey(sdlkey);

	if (sdlkey == SDLK_ESCAPE) {
		sys.SetState(new MPMenu());
		return;
	} else if (key < NUM_KEYS) {
		localgame->OnKeyDown(sys, key);

		/*if (server != NULL) {
			event_t	new_event;
			char	data[4];

			new_event.event = EVENT_KEYDOWN;
			new_event.data = key;
			new_event.offset = frame_offset;
			frame_offset = 0;
			SDLNet_TCP_Send(server, &new_event.event, 1);
			SDLNet_TCP_Send(server, &new_event.data, 1);
			SDLNet_Write16(new_event.offset, data);
			SDLNet_TCP_Send(server, data, 2);
		}*/
	}
}

void MPClient::OnKeyUp(System& sys, SDLKey sdlkey)
{
	gamekey_t key = sys.GetKey(sdlkey);

	if (key < NUM_KEYS) {
		localgame->OnKeyUp(sys, key);

		/*if (server != NULL) {
			event_t	new_event;
			char	data[4];

			new_event.event = EVENT_KEYUP;
			new_event.data = key;
			new_event.offset = frame_offset;
			frame_offset = 0;
			SDLNet_TCP_Send(server, &new_event.event, 1);
			SDLNet_TCP_Send(server, &new_event.data, 1);
			SDLNet_Write16(new_event.offset, data);
			SDLNet_TCP_Send(server, data, 2);
		}*/
	}
}
#endif
