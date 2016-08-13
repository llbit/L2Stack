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
#ifndef tetlib_tetai_h
#define tetlib_tetai_h

#include "tetlib.h"

#define TET_ACTIONBUFF_SIZE 64

typedef struct {
	placement_t	*p;
	float		fitness;
	well_t		*well;
	const rotsys_t	*rotsys;
	tetromino_t	*follow;
	int		nfollow;
	gamekey_t	*actions;
	int		nactions;
} aiplacement_t;

typedef struct {
	float	hole_weight;
	float	depth_factor;
	float	depth_weight;
	float	overhang_weight;
	float	prep_weight;
	float	area_weight;
	float	single_weight;
	float	double_weight;
	float	triple_weight;
	float	tetris_weight;
	// ...
} aiparams_t;

aiplacement_t *tet_NewAIPlacement(tetromino_t tet,
				  const well_t *well, const rotsys_t *rotsys,
				  const tetromino_t *follow, int nfollow);
void tet_FreeAIPlacement(aiplacement_t *p);
aiplacement_t *tet_CopyAIPlacement(const aiplacement_t *);

float		tet_GetContactScore(const aiparams_t *params, const aiplacement_t *p);
float		tet_GetCoverScore(const aiparams_t *params, const aiplacement_t *p);
float		tet_GetDepthScore(const aiparams_t *params, const aiplacement_t *p);
float		tet_GetLineScore(const aiparams_t *params, const aiplacement_t *p);
float		tet_GetOverhangScore(const aiparams_t *params, const aiplacement_t *p);
float		tet_GetPrepScore(const aiparams_t *params, const aiplacement_t *p);
// Private:
//void		tet_MergeAIPlacement(const aiparams_t *params, aiplacement_t *p);
//aiplacement_t	*tet_Select(aiplacement_t *p1, aiplacement_t *p2);
//aiplacement_t	*tet_TopAdjustment(const aiparams_t *params, aiplacement_t *p);
//aiplacement_t	*tet_BottomAdjustment(const aiparams_t *params, aiplacement_t *p);
// ---
aiplacement_t	*tet_Place(const aiparams_t *params, aiplacement_t *p);

#endif
