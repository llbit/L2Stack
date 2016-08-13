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

int main(int argc, char *argv[])
{
	unsigned long int	t, next;
	unsigned int		fractions = 0, delay = 16;
	SDL_Event		event;
	std::ofstream		errfile;
	std::streambuf*		errbuf;
	
	errbuf = std::cerr.rdbuf();
	errfile.open("log.txt");
	if (!errfile.fail()) {
		std::cerr.rdbuf(errfile.rdbuf());
	}

	try {
		System sys;
		sys.SetUp();

		next = SDL_GetTicks() + delay;
		while (sys.Continue()) {
			while (SDL_PollEvent(&event)) {
				sys.OnEvent(event);
			}
			t = SDL_GetTicks();
			sys.Update(delay);
			sys.Draw();
			do {
				 t = SDL_GetTicks();
			} while (t < next);
			fractions += 67;
			delay = 16 + fractions / 100;
			fractions = fractions % 100;
			next = t + delay;
			sys.GetGraphics().SwapBuffers();
		}
	} catch (const char* fail) {
		std::cerr << "An unrecoverable error occurred. Exiting.\n";
		return 1;
	} catch (...) {
		std::cerr << "An unrecoverable error occurred. Exiting.\n";
		return 1;
	}

	// Restore cerr
	std::cerr.rdbuf(errbuf);
	errfile.close();
	return 0;
}
