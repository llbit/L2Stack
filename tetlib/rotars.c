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
#include "tetlib.h"

bool ars_TestImpossible(const well_t *well, const placement_t *p, int rnew)
{
	switch (p->tet) {
	case TET_O:
		return true;
	case TET_L:
		if (p->rot == 0 && tet_TestBlock(well, p->x+1, p->y+3) &&
			!(rnew == 1 && tet_TestBlock(well, p->x, p->y+1))) {
				return true;
		} else if (p->rot == 0 && tet_TestBlock(well, p->x+1, p->y+1)) {
			return true;
		} else if (p->rot == 2 && tet_TestBlock(well, p->x+1, p->y+1)) {
			return true;
		} else if (p->rot == 2 && tet_TestBlock(well, p->x+1, p->y+2)) {
			return true;
		}
		break;
	case TET_J:
		if (p->rot == 0 && tet_TestBlock(well, p->x+1, p->y+3) &&
			!(rnew == 3 && tet_TestBlock(well, p->x+2, p->y+1))) {
				return true;
		} else if (p->rot == 0 && tet_TestBlock(well, p->x+1, p->y+1)) {
			return true;
		} else if (p->rot == 2 && tet_TestBlock(well, p->x+1, p->y+1)) {
			return true;
		} else if (p->rot == 2 && tet_TestBlock(well, p->x+1, p->y+2)) {
			return true;
		}
		break;
	case TET_T:
		if (p->rot == 2 && tet_TestBlock(well, p->x+1, p->y+1)) {
			return true;
		} else if (p->rot == 0 && tet_TestBlock(well, p->x+1, p->y+1)) {
			return true;
		}
	}

	return false;
}


bool ars_RotateTo(const well_t *well, placement_t *p, int rnew)
{
	tile_t	*snew;
	int	x, y;

	if (ars_TestImpossible(well, p, rnew)) {
		return false;
	}

	x = p->x;
	y = p->y;
	snew = ars_GetRotState(p->tet, rnew);

	if (!tet_TestCollision(well, snew, x, y)) {
		p->rot = rnew;
		return true;
	} else if (p->tet == TET_I) {
		return false;
	} else if (!tet_TestCollision(well, snew, x+1, y)) {
		p->x += 1;
		p->rot = rnew;
		return true;
	} else if (!tet_TestCollision(well, snew, x-1, y)) {
		p->x -= 1;
		p->rot = rnew;
		return true;
	} else {
		return false;
	}
}

bool ars_RotateCW(const well_t *well, placement_t *p)
{
	int rnew = p->rot + 1;
	if (rnew > 3) rnew = 0;

	return ars_RotateTo(well, p, rnew);
}

bool ars_RotateCCW(const well_t *well, placement_t *p)
{
	int rnew = p->rot - 1;
	if (rnew < 0) rnew = 3;
		
	return ars_RotateTo(well, p, rnew);
}

tile_t* ars_GetRotState(tetromino_t tet, int r)
{
	static int states[7][4][4][4] = {
		{{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{1, 1, 1, 0},
			{1, 0, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{1, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 1, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 1, 0},
			{1, 1, 1, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 1, 1, 0}
		}},

		{{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{2, 2, 2, 0},
			{0, 0, 2, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 2, 0, 0},
			{0, 2, 0, 0},
			{2, 2, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{2, 0, 0, 0},
			{2, 2, 2, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 2, 2, 0},
			{0, 2, 0, 0},
			{0, 2, 0, 0}
		}},

		{{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{6, 6, 6, 0},
			{0, 6, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 6, 0, 0},
			{6, 6, 0, 0},
			{0, 6, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 6, 0, 0},
			{6, 6, 6, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 6, 0, 0},
			{0, 6, 6, 0},
			{0, 6, 0, 0}
		}},

		{{
			{0, 0, 0, 0},
			{7, 7, 7, 7},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 7, 0},
			{0, 0, 7, 0},
			{0, 0, 7, 0},
			{0, 0, 7, 0}
		},
		{
			{0, 0, 0, 0},
			{7, 7, 7, 7},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 7, 0},
			{0, 0, 7, 0},
			{0, 0, 7, 0},
			{0, 0, 7, 0}
		}},

		{{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 4, 4, 0},
			{4, 4, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{4, 0, 0, 0},
			{4, 4, 0, 0},
			{0, 4, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 4, 4, 0},
			{4, 4, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{4, 0, 0, 0},
			{4, 4, 0, 0},
			{0, 4, 0, 0}
		}},

		{{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{5, 5, 0, 0},
			{0, 5, 5, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 5, 0},
			{0, 5, 5, 0},
			{0, 5, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{5, 5, 0, 0},
			{0, 5, 5, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 5, 0},
			{0, 5, 5, 0},
			{0, 5, 0, 0}
		}},

		{{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 3, 3, 0},
			{0, 3, 3, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 3, 3, 0},
			{0, 3, 3, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 3, 3, 0},
			{0, 3, 3, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 3, 3, 0},
			{0, 3, 3, 0}
		}}};
	return (tile_t *)states[tet][r];
}
