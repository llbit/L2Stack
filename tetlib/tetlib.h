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
#ifndef tetlib_tetlib_h
#define tetlib_tetlib_h

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

#define TET_WELL_H 21
// TET_WELL_W max 31 (32 bits per row)
#define TET_WELL_W 10

typedef enum {
	TILE_NULL = 0,
	TILE_ORANGE = 1,
	TILE_BLUE = 2,
	TILE_YELLOW = 3,
	TILE_PURPLE = 4,
	TILE_GREEN = 5,
	TILE_CYAN = 6,
	TILE_RED = 7,
	TILE_GRAY = 8,
	TILE_COUNT = 9
} tile_t;

typedef enum {
	TET_L = 0,
	TET_J = 1,
	TET_T = 2,
	TET_I = 3,
	TET_S = 4,
	TET_Z = 5,
	TET_O = 6,
	TET_COUNT = 7,
	TET_NULL = 8
} tetromino_t;
typedef tetromino_t tet_t;

typedef enum {
	KEY_LEFT = 0,
	KEY_RIGHT = 1,
	KEY_SOFT = 2,
	KEY_HARD = 3,
	KEY_AROT = 4,
	KEY_BROT = 5,
	KEY_CROT = 6,
	KEY_HOLD = 7,

	NUM_KEYS = 8
} gamekey_t;


typedef struct {
	tile_t*		state;
	tetromino_t	tet;
	int		x, y, rot;
} placement_t;

typedef struct {
	int32_t* well;
} well_t;

typedef struct {
	int*	preview_x;
	int*	preview_y;
	int*	spawn_x;
	int*	spawn_y;
	bool	shift_reset, rot_reset, drop_reset;
	bool	(*RotateCW)(const well_t*, placement_t*);
	bool	(*RotateCCW)(const well_t*, placement_t*);
	tile_t*	(*GetRotState)(tetromino_t, int);
} rotsys_t;

// General
rotsys_t*	tet_RotARS();
rotsys_t*	tet_RotSRS();
void		tet_FreeRotSys(rotsys_t*);
placement_t*	tet_NewPlacement(tetromino_t tet, const rotsys_t* rs);
well_t*		tet_NewWell();
void		tet_FreeWell(well_t*);
void		tet_CopyPlacement(placement_t*, const placement_t*);

// Placement
bool	tet_RotateCW(const rotsys_t*, const well_t*, placement_t*);
bool	tet_RotateCCW(const rotsys_t*, const well_t*, placement_t*);
bool	tet_ShiftLeft(const well_t*, placement_t*);
bool	tet_ShiftRight(const well_t*, placement_t*);
bool	tet_Advance(const well_t*, placement_t*);
int	tet_Drop(const well_t*, placement_t*);

// ARS
bool	ars_RotateCW(const well_t*, placement_t*);
bool	ars_RotateCCW(const well_t*, placement_t*);
tile_t*	ars_GetRotState(tetromino_t, int);

// SRS
bool	srs_RotateCW(const well_t*, placement_t*);
bool	srs_RotateCCW(const well_t*, placement_t*);
tile_t*	srs_GetRotState(tetromino_t, int);

// Well
bool	tet_TestBlock(const well_t*, int x, int y);
bool	tet_TestCollision(const well_t*, const tile_t*, int xoffs, int yoffs);
bool	tet_TestPlacement(const well_t*, const placement_t*);
void	tet_Merge(well_t*, const tile_t*, int xoffs, int yoffs);
void	tet_MergePlacement(well_t*, const placement_t*);
int	tet_CountLines(const well_t*, int y0, int y1);
int	tet_ClearLines(well_t*);
void	tet_CopyRow(well_t*, int ydst, int ysrc);
void	tet_ClearRow(well_t*, int y);
void	tet_AddGarbageLine(well_t*, int hole);
int	tet_GetTopLine(const well_t*);
int	tet_GetTopLineHeight(const well_t*);
bool	tet_IsLine(const well_t*, int y);
int	tet_GetLineDensity(const well_t*, int y);
void	tet_SetRow(well_t*, int y);
void	tet_ClearRow(well_t*, int y);
void	tet_SetBlock(well_t*, int x, int y);
void	tet_ClearBlock(well_t*, int x, int y);
int	tet_GetBlock(const well_t*, int x, int y);
void	tet_Clear(well_t*);
void	tet_CopyWell(well_t*, const well_t*);

// RNG
typedef struct rng_t rng_t;
struct rng_t {
	uint32_t low;
	uint32_t high;
};

rng_t*	tet_NewRNG();
void	rng_Seed(rng_t*, uint32_t);
int	rng_GenInt(rng_t*);
float	rng_GenFloat(rng_t*);
void	tet_FreeRNG(rng_t**);

// Randomizer
typedef struct randomizer_t rand_t;
struct randomizer_t {
	rng_t*	rng;
	tet_t*	state;
	int	auxint;
	tet_t	(*GetNext)(rand_t*);
};

rand_t*	tet_NewRandStateless();
rand_t*	tet_NewRandBag();
rand_t*	tet_NewRand4Retry();
rand_t*	tet_NewRand6Retry();
void	tet_FreeRand(rand_t**);

tet_t	rsl_GetNext(rand_t*);
tet_t	rbag_GetNext(rand_t*);
tet_t	r4retry_GetNext(rand_t*);
tet_t	r6retry_GetNext(rand_t*);

#endif
