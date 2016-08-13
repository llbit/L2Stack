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
#ifndef tet_mpsetup
#define tet_mpsetup

class MPSetup : public State
{
	string	host_port;
	bool		syntaxerror;

public:
	MPSetup();

	void Update(System& sys);
	void Draw(Graphics& graphics);

	void OnKeyDown(System& sys, SDLKey sdlkey);
	void OnKeyUp(System& sys, SDLKey sdlkey);
};

#endif
