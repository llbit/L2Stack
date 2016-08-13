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
#ifndef tet_defs
#define tet_defs

class System;
class Game;
class Well;
class Graphics;

typedef Uint16 devkey_t;

enum event_type_t {
	EVENT_KEYDOWN = 0,
	EVENT_KEYUP = 1,
	EVENT_LOCK = 2
};

struct event_t {
	Uint8	event;// keydown/keyup/etc.
	Uint8	data;// gamekey/tet/etc.
	Uint16	offset;// frame offset
};

enum idevice_t {
	DEV_KEYBOARD = 0,
	DEV_MOUSE = 1,
	DEV_JOY1 = 2,
	DEV_JOY2 = 3,
	DEV_JOY3 = 4,
	DEV_JOY4 = 5
};

enum tetstate_t {
	TETS_NONE,
	TETS_SPAWNING,
	TETS_MOVING,
	TETS_LOCKED,
	TETS_MERGING
};

enum rotation_t {
	ROT_ARS = 0,
	ROT_SRS = 1,

	ROT_UNDEF = 3
};

typedef struct line_t {
	int x0, y0, x1, y1;
} line_t;

const int	TILESIZE = 32;

#endif
