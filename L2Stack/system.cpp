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

System::System()
{
	exit = false;
}

System::~System()
{
	bigpak.Close();
}

void System::SetUp()
{
	srand((unsigned int)::time(NULL));
	rng.Seed((unsigned int)::time(NULL));

	bigpak.OpenRead("data.big");
	bigpak.ReadIndex();

	lua.SetUp(*this);
	lua.LoadScript(bigpak, "data/defaults.lua");

	SetUpSDL();

	graphics.SetUp(*this);

	soundsys.SetUp();
	metacube.Init(*this);

	lua.SetStateString("version", PACKAGE_VERSION);
	lua.LoadScript(bigpak, "data/gui.lua");
}

void System::SetUpSDL()
{
	atexit(SDL_Quit);
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cerr << "Unable to initialize SDL: " << SDL_GetError() << "\n";
		throw "fail";
	}
	SDL_WM_SetCaption("L2Stack", NULL);
	SDL_EnableUNICODE(1);
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) == 0) {
		int	njoys = std::min(4, SDL_NumJoysticks());
		for (int i = 0; i < njoys; ++i) {
			SDL_JoystickOpen(i);
		}
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_ShowCursor(0);
}

void System::Update(unsigned int millis)
{
	try {
		lua.Update(millis);
		graphics.Update(*this);
	} catch (string err) {
		this->OnError(err);
	}
}

void System::Draw()
{
	try {
		graphics.ClearBuffer();
		lua.Draw();
	} catch (string err) {
		this->OnError(err);
	}
}

void System::OnError(const string &error)
{
	lua.OnError(error);
}

void System::OnEvent(SDL_Event &event)
{
	Uint16 device = 0;
	switch (event.type) {
	case SDL_QUIT:
		Exit();
		break;
	case SDL_KEYDOWN:
		device = DEV_KEYBOARD;
		lua.OnKeyDown(device << 8 | event.key.keysym.sym);
		if (event.key.keysym.unicode >= ' ') {
			lua.OnText(event.key.keysym.unicode);
		}
		break;
	case SDL_KEYUP:
		device = DEV_KEYBOARD;
		lua.OnKeyUp(device << 8 | event.key.keysym.sym);
		break;
	case SDL_MOUSEMOTION:
		lua.OnMouseMotion(event.motion.x, event.motion.y);
		break;
	case SDL_JOYBUTTONDOWN:
		device = DEV_JOY1 + event.jbutton.which;
		lua.OnKeyDown(device << 8 | event.jbutton.button);
		break;
	case SDL_JOYBUTTONUP:
		device = DEV_JOY1 + event.jbutton.which;
		lua.OnKeyUp(device << 8 | event.jbutton.button);
		break;
	case SDL_MOUSEBUTTONDOWN:
		device = DEV_MOUSE;
		lua.OnKeyDown(device << 8 | event.button.button);
		break;
	case SDL_MOUSEBUTTONUP:
		device = DEV_MOUSE;
		lua.OnKeyUp(device << 8 | event.button.button);
		break;
	}
}
