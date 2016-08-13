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
#ifndef tet_mpserver
#define tet_mpserver

class MPGame;

class MPServer : public State
{
	list<event_t>	events;
	int			seed;

	rotation_t		rot_mode;
	Uint16			frame_offset;

	int			netport;
	TCPsocket		tcpsock;
	TCPsocket		client;

	SDL_Thread*		thread;
	bool			thread_error;
	bool			exit;
	string		error;
	SDL_mutex*		mutex;

	bool			ready;

	MPGame*			localgame;
	MPGame*			clientgame;

public:
	MPServer(System& sys);
	~MPServer();

	bool Connect();
	void Disconnect();
	bool WaitConnect();
	void SetError(string str) {SDL_mutexP(mutex);thread_error = true;error = str;SDL_mutexV(mutex);}

	bool SendSeed();
	bool WaitOK();
	void NetRead();

	void Update(System& sys);
	void Draw(Graphics& graphics);

	void OnKeyDown(System& sys, SDLKey sdlkey);
	void OnKeyUp(System& sys, SDLKey sdlkey);
};

#endif
