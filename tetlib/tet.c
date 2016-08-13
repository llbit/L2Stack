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

placement_t	*tet_NewPlacement(tetromino_t tet, const rotsys_t *rs)
{
	placement_t	*p = malloc(sizeof(placement_t));

	p->tet = tet;
	p->x = rs->spawn_x[tet];
	p->y = rs->spawn_y[tet];
	p->rot = 0;
	p->state = rs->GetRotState(tet, 0);

	return p;
}


void	tet_CopyPlacement(placement_t *dst, const placement_t *src)
{
	dst->tet = src->tet;
	dst->x = src->x;
	dst->y = src->y;
	dst->rot = src->rot;
	dst->state = src->state;
}

bool	tet_RotateCW(const rotsys_t *rs, const well_t *well, placement_t *p)
{
	if (rs->RotateCW(well, p)) {
		p->state = rs->GetRotState(p->tet, p->rot);
		return true;
	}
	return false;
}

bool	tet_RotateCCW(const rotsys_t *rs, const well_t *well, placement_t *p)
{
	if (rs->RotateCCW(well, p)) {
		p->state = rs->GetRotState(p->tet, p->rot);
		return true;
	}
	return false;
}

bool	tet_ShiftLeft(const well_t *well, placement_t *p)
{
	if (!tet_TestCollision(well, p->state, p->x-1, p->y)) {
		p->x -= 1;
		return true;
	}
	return false;
}

bool	tet_ShiftRight(const well_t *well, placement_t *p)
{
	if (!tet_TestCollision(well, p->state, p->x+1, p->y)) {
		p->x += 1;
		return true;
	}
	return false;
}

bool	tet_Advance(const well_t *well, placement_t *p)
{
	if (!tet_TestCollision(well, p->state, p->x, p->y+1)) {
		p->y += 1;
		return true;
	}
	return false;
}

int	tet_Drop(const well_t *well, placement_t *p)
{
	int	drop = 0;
	while (tet_Advance(well, p)) drop += 1;
	return drop;
}
