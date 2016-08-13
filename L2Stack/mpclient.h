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
#ifndef tet_mpclient
#define tet_mpclient

class MPClient : public State
{
	list<event_t>	events;
	int			seed;

	rotation_t		rot_mode;
	Uint16			frame_offset;

	string		serveraddr;

	TCPsocket		server;
	int			netport;

	SDL_Thread*		thread;
	SDL_mutex*		mutex;
	bool			thread_error;
	string		error;

	MPGame*			localgame;
	MPGame*			servergame;

public:
	MPClient(System& sys, string ipaddr);
	~MPClient();

	bool Connect();
	void Disconnect();

	void ThreadErrorFlag() {thread_error = true;}
	void SetError(string str) {SDL_mutexP(mutex);thread_error = true;error = str;SDL_mutexV(mutex);}

	bool ReadSeed();
	void NetRead();

	void Update(System& sys);
	void Draw(Graphics& graphics);

	void OnKeyDown(System& sys, SDLKey sdlkey);
	void OnKeyUp(System& sys, SDLKey sdlkey);
};

#endif
