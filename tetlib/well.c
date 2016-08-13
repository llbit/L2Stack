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

well_t	*tet_NewWell()
{
	well_t	*well = malloc(sizeof(well_t));
	well->well = malloc(sizeof(int)*TET_WELL_H);
	tet_Clear(well);
	return well;
}

void	tet_FreeWell(well_t *well)
{
	free(well->well);
	free(well);
}
bool	tet_TestBlock(const well_t *well, int x, int y)
{
	if (x < 0 || x >= TET_WELL_W)
		return true;
	if (y < 0 || y >= TET_WELL_H)
		return true;
	if (tet_GetBlock(well, x, y))
		return true;
	return false;
}

bool	tet_TestCollision(const well_t *well, const tile_t *state, int xoffs, int yoffs)
{
	int	x, y;
	int	x1, y1;
	for (y = 0, y1 = yoffs; y < 4; ++y, ++y1) {
		for (x = 0, x1 = xoffs; x < 4; ++x, ++x1) {
			if (state[x + y*4] != TILE_NULL) {
				if (x1 < 0 || x1 >= TET_WELL_W)
					return true;
				if (y1 < 0 || y1 >= TET_WELL_H)
					return true;
				if (tet_GetBlock(well, x1, y1))
					return true;
			}
		}
	}
	return false;
}

bool	tet_TestPlacement(const well_t *well, const placement_t *p)
{
	return !tet_TestCollision(well, p->state, p->x, p->y);
}

void	tet_Clear(well_t *well)
{
	memset(well->well, 0, sizeof(int)*TET_WELL_H);
}

int	tet_ClearLines(well_t *well)
{
	int	ncleared = 0;
	int	y, y2;
	for (y = TET_WELL_H-1; y >= 0; --y) {
		if (tet_IsLine(well, y)) {
			for (y2 = y-1; y2 >= 0; --y2) {
				tet_CopyRow(well, y2+1, y2);
			}
			tet_ClearRow(well, 0);
			y += 1;
			ncleared += 1;
		}
	}
	return ncleared;
}

void	tet_CopyRow(well_t *well, int ydst, int ysrc)
{
	well->well[ydst] = well->well[ysrc];
}

void	tet_CopyWell(well_t *dst, const well_t *src)
{
	memcpy(dst->well, src->well, sizeof(int)*TET_WELL_H);
}

void	tet_SetRow(well_t *well, int y)
{
	well->well[y] = (1 << TET_WELL_W) - 1;
}

void	tet_ClearRow(well_t *well, int y)
{
	well->well[y] = 0;
}

void	tet_SetBlock(well_t *well, int x, int y)
{
	well->well[y] = well->well[y] | (1 << x);
}

void	tet_ClearBlock(well_t *well, int x, int y)
{
	well->well[y] = well->well[y] & ~(1 << x);
}

int	tet_GetBlock(const well_t *well, int x, int y)
{
	if (x < 0 || x >= TET_WELL_W || y < 0 || y >= TET_WELL_H) {
		return 0;
	} else {
		return (well->well[y] >> x) & 1;
	}
}

void	tet_Merge(well_t *well, const tile_t *state, int xoffs, int yoffs)
{
	int	x, y;
	int	x1, y1;
	for (y = 0, y1 = yoffs; y < 4; ++y, ++y1) {
		for (x = 0, x1 = xoffs; x < 4; ++x, ++x1) {
			if (state[x + y*4] != TILE_NULL) {
				tet_SetBlock(well, x1, y1);
			}
		}
	}
}

void	tet_MergePlacement(well_t *well, const placement_t *p)
{
	tet_Merge(well, p->state, p->x, p->y);
}

int	tet_CountLines(const well_t *well, int y1, int y2)
{
	int	y;
	int	lines = 0;
	for (y = y1; y < y2; ++y) {
		lines += tet_IsLine(well, y) ? 1 : 0;
	}
	return lines;
}

bool	tet_IsLine(const well_t *well, int y)
{
	if (well->well[y] == (1 << TET_WELL_W) - 1) {
		return true;
	}
	return false;
}

void	tet_AddGarbageLine(well_t *well, int hole)
{
	int	y;
	for (y = 0; y < TET_WELL_H-1; ++y) {
		tet_CopyRow(well, y, y+1);
	}
	tet_SetRow(well, TET_WELL_H-1);
	tet_ClearBlock(well, hole, TET_WELL_H-1);
}

int	tet_GetLineDensity(const well_t *well, int y)
{
	// Count bits set (http://freeworld.thc.org/root/phun/unmaintain.html?repost=forPosterity)
	int	v = well->well[y], c;
	v = v - ((v >> 1) & 0x55555555);
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
	c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
	return c;
}

int	tet_GetTopLine(const well_t *well)
{
	int	y;
	for (y = 0; y < TET_WELL_H; ++y) {
		if (tet_GetLineDensity(well, y) > 0) {
			return y;
		}
	}
	return TET_WELL_H;
}

int	tet_GetTopLineHeight(const well_t *well)
{
	return TET_WELL_H - tet_GetTopLine(well);
}
