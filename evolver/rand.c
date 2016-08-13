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
#include "rand.h"
#include <limits.h>
#include <stdbool.h>

unsigned int	rand_seed_h, rand_seed_l;
tetromino_t	history[4];
bool		first;

unsigned int quickrand()
{
	rand_seed_h = (rand_seed_h << 16) + (rand_seed_h >> 16);
	rand_seed_h += rand_seed_l;
	rand_seed_l += rand_seed_h;
	return rand_seed_h;
}

void squickrand(unsigned int seed)
{
	rand_seed_h = seed;
	rand_seed_l = seed ^ 0x49616E42;
}

float randf()
{
	return (float)quickrand()/(float)UINT_MAX;
}

float randz()
{
	return -1.f + 2.f*(float)quickrand()/(float)UINT_MAX;
}

tetromino_t randt()
{
	int		i, j;
	tetromino_t	tet;

	do {
		for (i = 0; i < 6; ++i) {
			tet = (tetromino_t)(quickrand() % TET_COUNT);
			for (j = 0; j < 4; ++j) {
				if (tet == history[j]) {
					break;
				}
			}
			if (j == 4) {
				break;
			}
		}
	} while (first && (tet == TET_S || tet == TET_Z || tet == TET_O));

	first = false;

	for (i = 0; i < 3; ++i) {
		history[i] = history[i + 1];
	}
	history[3] = tet;
	return tet;
}

void initrandt()
{
	history[0] = TET_Z;
	history[1] = TET_S;
	history[2] = TET_Z;
	history[3] = TET_S;
	first = true;
}
