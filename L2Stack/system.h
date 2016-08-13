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
#ifndef tet_sys
#define tet_sys

class System {
	SoundSystem	soundsys;
	Graphics	graphics;
	Archive		bigpak;
	Script		lua;
	RNG		rng;
	Metacube	metacube;

	bool		exit;

	void SetUpSDL();

public:
	System();
	~System();

	Graphics& GetGraphics() {return graphics;}
	SoundSystem& GetSound() {return soundsys;}
	Archive& GetArchive() {return bigpak;}
	Script& GetScript() {return lua;}
	RNG& GetRNG() {return rng;}
	Metacube& GetMetacube() {return metacube;}

	void SetUp();

	void Update(unsigned int millis);
	void Draw();
	void OnError(const string &);
	void OnEvent(SDL_Event &);

	bool Continue/*?*/() {return exit == false;}
	void Exit() {exit = true;}
};

#endif
