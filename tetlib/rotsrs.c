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

bool srs_Kick(const well_t *well, placement_t *p, int rnew, int xoffs, int yoffs)
{
	tile_t*	snew;// snew sounds funny if you try to pronounce it in english...
       
	snew = srs_GetRotState(p->tet, rnew);
	
	if (!tet_TestCollision(well, snew, p->x+xoffs, p->y+yoffs)) {
		p->x += xoffs;
		p->y += yoffs;
		p->rot = rnew;
		return true;
	} else {
		return false;
	}
}

bool srs_RotateTo(const well_t *well, placement_t *p, int rnew)
{
	// Reference:
	// http://tetrisconcept.net/wiki/SRS 
	// first column omitted
	// O = 0
	// R = 1
	// 2 = 2
	// L = 3
	// example: L->2 = 3->2
	const int undef = 0;
	const int kickindex[4][4] = {
		{// O->?
			undef, 0, undef, 7
		},
		{// R->?
			1, undef, 2, undef
		},
		{// 2->?
			undef, 3, undef, 4
		},
		{
			6, undef, 5, undef
		}
	};
	const int kicks[8][4][2] = {
		{// O->R
			{-1,0}, {-1,+1}, {0,-2}, {-1,-2}
		},
		{// R->O
			{+1,0}, {+1,-1}, {0,+2}, {+1,+2}
		},
		{// R->2
			{+1,0}, {+1,-1}, {0,+2}, {+1,+2}
		},
		{// 2->R
			{-1,0}, {-1,+1}, {0,-2}, {-1,-2}
		},
		{// 2->L
			{+1,0}, {+1,+1}, {0,-2}, {+1,-2}
		},
		{// L->2
			{-1,0}, {-1,-1}, {0,+2}, {-1,+2}
		},
		{// L->O
			{-1,0}, {-1,-1}, {0,+2}, {-1,+2}
		},
		{// O->L
			{+1,0}, {+1,+1}, {0,-2}, {+1,-2}
		}
	};
	const int i_kicks[8][4][2]= {
		{// O->R
			{-2,0}, {+1,0}, {-2,-1}, {+1,+2}
		},
		{// R->O
			{+2,0}, {-1,0}, {+2,+1}, {-1,-2}
		},
		{// R->2
			{-1,0}, {+2,0}, {-1,+2}, {+2,-1}
		},
		{// 2->R
			{+1,0}, {-2,0}, {+1,-2}, {-2,+1}
		},
		{// 2->L
			{+2,0}, {-1,0}, {+2,+1}, {-1,-2}
		},
		{// L->2
			{-2,0}, {+1,0}, {-2,-1}, {+1,+2}
		},
		{// L->O
			{+1,0}, {-2,0}, {+1,-2}, {-2,+1}
		},
		{// O->L
			{-1,0}, {+2,0}, {-1,+2}, {+2,-1}
		}
	};
	const int t_kicks[8][4] = {
		{// T2->T1
			1, 1, 1, 1
		},
		{// T4->T1
			1, 1, 1, 1
		},
		{// T3->T2
			1, 0, 1, 1
		},
		{// T1->T2
			1, 1, 0, 1
		},
		{// T4->T3
			1, 1, 1, 1
		},
		{// T2->T3
			1, 1, 1, 1
		},
		{// T1->T4
			1, 1, 0, 1
		},
		{// T3->T4
			1, 0, 1, 1
		},
	};
	const int	(*kicktable)[4][2];
	int		index;
	int		i;

	index = kickindex[p->rot][rnew];

	if (p->tet == TET_O) {
		// return true to enable infinite "spin" for the O piece
		// This might seem silly, but at least Tetris DS does it this way.
		return true;
	} else if (srs_Kick(well, p, rnew, 0, 0)) {
		return true;
	} else if (p->tet == TET_I) {
		kicktable = &i_kicks[index];
	} else {
		kicktable = &kicks[index];
	}

	// Test1 is implicit (zero displacement test)
	for (i = 0; i < 4; ++i) {
		int xoffs = (*kicktable)[i][0];
		int yoffs = -(*kicktable)[i][1];// our y-cordinates increase downward
		if (srs_Kick(well, p, rnew, xoffs, yoffs)) {
			return true;
		}
	}
	
	return false;
}

bool srs_RotateCW(const well_t *well, placement_t *p)
{
	int rnew = p->rot + 1;
	if (rnew > 3) rnew = 0;

	return srs_RotateTo(well, p, rnew);
}

bool srs_RotateCCW(const well_t *well, placement_t *p)
{
	int rnew = p->rot - 1;
	if (rnew < 0) rnew = 3;
		
	return srs_RotateTo(well, p, rnew);
}

tile_t* srs_GetRotState(tetromino_t tet, int r)
{
	static int states[7][4][4][4] = {
		{{
			{0, 0, 1, 0},
			{1, 1, 1, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 1, 1, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{1, 1, 1, 0},
			{1, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{1, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 1, 0, 0},
			{0, 0, 0, 0}
		}},

		{{
			{2, 0, 0, 0},
			{2, 2, 2, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 2, 2, 0},
			{0, 2, 0, 0},
			{0, 2, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{2, 2, 2, 0},
			{0, 0, 2, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 2, 0, 0},
			{0, 2, 0, 0},
			{2, 2, 0, 0},
			{0, 0, 0, 0}
		}},

		{{
			{0, 4, 0, 0},
			{4, 4, 4, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 4, 0, 0},
			{0, 4, 4, 0},
			{0, 4, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{4, 4, 4, 0},
			{0, 4, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 4, 0, 0},
			{4, 4, 0, 0},
			{0, 4, 0, 0},
			{0, 0, 0, 0}
		}},

		{{
			{0, 0, 0, 0},
			{6, 6, 6, 6},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 6, 0},
			{0, 0, 6, 0},
			{0, 0, 6, 0},
			{0, 0, 6, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{6, 6, 6, 6},
			{0, 0, 0, 0}
		},
		{
			{0, 6, 0, 0},
			{0, 6, 0, 0},
			{0, 6, 0, 0},
			{0, 6, 0, 0}
		}},

		{{
			{0, 5, 5, 0},
			{5, 5, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 5, 0, 0},
			{0, 5, 5, 0},
			{0, 0, 5, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{0, 5, 5, 0},
			{5, 5, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{5, 0, 0, 0},
			{5, 5, 0, 0},
			{0, 5, 0, 0},
			{0, 0, 0, 0}
		}},

		{{
			{7, 7, 0, 0},
			{0, 7, 7, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 7, 0},
			{0, 7, 7, 0},
			{0, 7, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 0, 0, 0},
			{7, 7, 0, 0},
			{0, 7, 7, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 7, 0, 0},
			{7, 7, 0, 0},
			{7, 0, 0, 0},
			{0, 0, 0, 0}
		}},

		{{
			{0, 3, 3, 0},
			{0, 3, 3, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 3, 3, 0},
			{0, 3, 3, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 3, 3, 0},
			{0, 3, 3, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		},
		{
			{0, 3, 3, 0},
			{0, 3, 3, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0}
		}}};
	return (tile_t *)states[tet][(unsigned)r];
}
