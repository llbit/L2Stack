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
int MPServer_Thread(void* mpclass)
{
	MPServer*	server = (MPServer*)mpclass;

	if (!server->Connect()) {
		server->SetError(string(SDLNet_GetError()));
		return 1;
	}

	if (!server->WaitConnect()) {
		return 1;
	}

	if (!server->SendSeed()) {
		return 1;
	}
	if (!server->WaitOK()) {
		server->SetError(string("Could not verify connection from client."));
		return 1;
	}
	server->NetRead();
	return 0;
}

MPServer::MPServer(System& sys)
{
	thread_error = false;
	exit = false;
	ready = false;
	randserv = NULL;
	rot_mode = sys.rot_mode;

	netport = sys.netport;

	client = NULL;
	mutex = NULL;
	thread = NULL;
	frame_offset = 0;

	seed = (int)::time(NULL);

	clientgame = NULL;
	localgame = NULL;

	mutex = SDL_CreateMutex();
	thread = SDL_CreateThread(MPServer_Thread, this);
}

MPServer::~MPServer()
{
	int	status;

	Disconnect();

	if (thread != NULL) {
		SDL_WaitThread(thread, &status);
	}

	SDL_DestroyMutex(mutex);

	if (clientgame != NULL) {
		delete clientgame;
	}
	if (localgame != NULL) {
		delete localgame;
	}

	if (randserv != NULL) {
		delete randserv;
	}
}

bool MPServer::Connect()
{
	if (SDLNet_Init() == -1) {
		return false;
	}

	IPaddress ip;
	if(SDLNet_ResolveHost(&ip, NULL, netport) == -1) {
		return false;
	}

	tcpsock = SDLNet_TCP_Open(&ip);
	if(!tcpsock) {
		return false;
	}
	return true;
}

void MPServer::Disconnect()
{
	if (client != NULL) {
		SDLNet_TCP_Close(client);
		client = NULL;
	}
	SDLNet_TCP_Close(tcpsock);
	SDLNet_Quit();
}

bool MPServer::SendSeed()
{
	char	data[4];

	SDLNet_Write32(seed, data);
	SDLNet_TCP_Send(client, data, 4);
	return true;
}

bool MPServer::WaitOK()
{
	char	data;
	int	ret;
	
	ret = SDLNet_TCP_Recv(client, &data, 1);
	if (ret <= 0 || data != 'K') {
		return false;
	}

	SDL_mutexP(mutex);
	ready = true;
	SDL_mutexV(mutex);
	return true;
}

bool MPServer::WaitConnect()
{
	while (!exit) {
		SDL_mutexP(mutex);
		client = SDLNet_TCP_Accept(tcpsock);
		if (client != NULL) {
			SDL_mutexV(mutex);
			return true;
		}
		SDL_mutexV(mutex);
	}
	return false;
}

void MPServer::NetRead()
{
	while (true) {
		event_t	new_event;
		char	data[4];
		int	tot;
		int	ret;

		if (client == NULL) {
			return;
		}

		ret = SDLNet_TCP_Recv(client, &new_event.event, 1);
		if (ret <= 0) {
			return;
		}

		ret = SDLNet_TCP_Recv(client, &new_event.data, 1);
		if (ret <= 0) {
			return;
		}

		for (tot = 0; tot < 2; ) {
			ret = SDLNet_TCP_Recv(client, &data[tot], 2-tot);
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

void MPServer::Update(System& sys)
{
	SDL_mutexP(mutex);

	if (thread_error) {
		SDL_mutexV(mutex);
		throw new Message(error, new MPMenu());
		return;
	}

	if (clientgame == NULL && ready) {
		randserv = new RandServer(seed, 4);
		localgame = new MPGame(rot_mode, System::MakeRandomizer(rand_mode, randserv, 0));
		clientgame = new MPGame(rot_mode, System::MakeRandomizer(rand_mode, randserv, 1));
	} else if (clientgame != NULL) {
		if (!events.empty()) {
			while (!events.empty()) {
				if (!clientgame->LockPiece(sys,
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

		int gqueue = clientgame->GetGarbageQueue();
		if (gqueue > 0) {
			localgame->AddGarbage(gqueue, randserv, 2);
		}

		gqueue = localgame->GetGarbageQueue();
		if (gqueue > 0) {
			clientgame->AddGarbage(gqueue, randserv, 3);
		}

		clientgame->Update(sys, false);
		localgame->Update(sys, true);

		while (!localgame->history.empty()) {
			event_t	new_event;
			char	data[4];

			new_event.event = localgame->history.front().GetRot();
			new_event.data = localgame->history.front().GetTet();
			new_event.offset = (Uint8)(localgame->history.front().GetX()) << 8 |
				(Uint8)(localgame->history.front().GetY());
			
			SDLNet_TCP_Send(client, &new_event.event, 1);
			SDLNet_TCP_Send(client, &new_event.data, 1);
			SDLNet_Write16(new_event.offset, data);
			SDLNet_TCP_Send(client, data, 2);

			localgame->history.pop_front();
		}
	}

	SDL_mutexV(mutex);
}

void MPServer::Draw(Graphics& graphics)
{
	SDL_mutexP(mutex);
	if (ready) {
		glPushMatrix();
		glTranslatef(75.f, 0.f, 0.f);
		localgame->Draw(sys);
		glPopMatrix();

		glPushMatrix();
		glTranslatef((float)(System::WIDTH - 12*TILESIZE-75), 0.f, 0.f);
		clientgame->Draw(sys);
		glPopMatrix();
	} else if (client != NULL) {
		glPushMatrix();
		glTranslatef(200.f, 150.f, 0.f);
		sys.DrawString(0.f, 20.f, "Handshaking...", 1);
		glPopMatrix();
	} else {
		glPushMatrix();
		glTranslatef(200.f, 150.f, 0.f);
		sys.DrawString(0.f, 20.f, "Waiting for connection...", 1);
		glPopMatrix();
	}
	SDL_mutexV(mutex);
}

void MPServer::OnKeyDown(System& sys, SDLKey sdlkey)
{
	gamekey_t key = sys.GetKey(sdlkey);

	if (sdlkey == SDLK_ESCAPE) {
		exit = true;
		sys.SetState(new MPMenu());
		return;
	} else if (key < NUM_KEYS && localgame != NULL) {
		localgame->OnKeyDown(sys, key);

		/*if (client != NULL) {
			event_t	new_event;
			char	data[4];

			new_event.event = EVENT_KEYDOWN;
			new_event.data = key;
			new_event.offset = frame_offset;
			frame_offset = 0;
			SDLNet_TCP_Send(client, &new_event.event, 1);
			SDLNet_TCP_Send(client, &new_event.data, 1);
			SDLNet_Write16(new_event.offset, data);
			SDLNet_TCP_Send(client, data, 2);
		}*/
	}
}

void MPServer::OnKeyUp(System& sys, SDLKey sdlkey)
{
	gamekey_t key = sys.GetKey(sdlkey);

	if (key < NUM_KEYS && localgame != NULL) {
		localgame->OnKeyUp(sys, key);

		/*if (client != NULL) {
			event_t	new_event;
			char	data[4];

			new_event.event = EVENT_KEYUP;
			new_event.data = key;
			new_event.offset = frame_offset;
			frame_offset = 0;
			SDLNet_TCP_Send(client, &new_event.event, 1);
			SDLNet_TCP_Send(client, &new_event.data, 1);
			SDLNet_Write16(new_event.offset, data);
			SDLNet_TCP_Send(client, data, 2);
		}*/
	}
}
#endif
