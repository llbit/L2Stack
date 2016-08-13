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
MPStart::MPStart()
{
	syntaxerror = false;
}

void MPStart::Update(System& sys)
{
}

void MPStart::Draw(Graphics& graphics)
{
	glPushMatrix();
	glTranslatef(200.f, 150.f, 0.f);

	

	sys.DrawString(0.f, 40.f, "Enter host IP: ", 1);
	sys.DrawString(0.f, 60.f, host_ip.c_str(), 0);
	sys.DrawString(0.f, 60.f, "_", 0);
	
	if (syntaxerror) {
		sys.DrawString(0.f, 90.f, "Bad Syntax.", 2);
	}

	

	glPopMatrix();
}

void MPStart::OnKeyDown(System& sys, SDLKey sdlkey)
{
	if (sdlkey == SDLK_BACKSPACE) {
		if (host_ip.size() > 0) {
			host_ip = host_ip.substr(0, host_ip.size()-1);
			syntaxerror = false;
		}
	} else if (sdlkey == SDLK_RETURN) {
		Regex	regex("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
		if (!regex.test(host_ip)) {
			syntaxerror = true;
			return;
		}
		pcre_free(regex);
		sys.SetState(new MPClient(sys, host_ip));
		return;
	} else if (sdlkey == SDLK_ESCAPE) {
		sys.SetState(new MPMenu());
		return;
	} else if (sdlkey >= SDLK_0 && sdlkey <= SDLK_9) {
		host_ip += (char)('0' + sdlkey - SDLK_0);
		syntaxerror = false;
	} else if (sdlkey == SDLK_PERIOD) {
		host_ip += '.';
		syntaxerror = false;
	}
}

void MPStart::OnKeyUp(System& sys, SDLKey sdlkey)
{
}
#endif
