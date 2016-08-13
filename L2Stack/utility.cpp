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

color3f GetTileColor(tile_t t)
{
	switch (t) {
	case TILE_NULL:
		return color3f(255, 255, 225);
	case TILE_BLUE:
		return color3f(81, 86, 225);
	case TILE_RED:
		return color3f(233, 53, 49);
	case TILE_CYAN:
		return color3f(10, 203, 208);
	case TILE_GRAY:
		return color3f(120, 120, 120);
	case TILE_GREEN:
		return color3f(22, 233, 17);
	case TILE_ORANGE:
		return color3f(238, 201, 52);
	case TILE_PURPLE:
		return color3f(201, 76, 234);
	case TILE_YELLOW:
		return color3f(236, 240, 42);
	default:
		std::cerr << "Unknown brick color!\n";
		throw "fail";
	}
}
