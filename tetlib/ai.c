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
#include <float.h>
#include "tetai.h"

aiplacement_t *tet_NewAIPlacement(tetromino_t tet,
				  const well_t *well, const rotsys_t *rotsys,
				  const tetromino_t *follow, int nfollow)
{
	aiplacement_t	*p = malloc(sizeof(aiplacement_t));

	p->well = tet_NewWell();
	tet_CopyWell(p->well, well);
	
	p->rotsys = rotsys;
	
	p->actions = malloc(sizeof(gamekey_t) * TET_ACTIONBUFF_SIZE);
	p->nactions = 0;
	p->nfollow = nfollow;
	if (nfollow > 0) {
		p->follow = malloc(sizeof(tetromino_t) * nfollow);
		memcpy(p->follow, follow, sizeof(tetromino_t) * nfollow);
	}

	p->fitness = -FLT_MAX;

	p->p = tet_NewPlacement(tet, rotsys);
	p->p->tet = tet;
	p->p->x = rotsys->spawn_x[tet];
	p->p->y = rotsys->spawn_y[tet];
	p->p->rot = 0;
	p->p->state = rotsys->GetRotState(tet, 0);

	return p;

}

void tet_FreeAIPlacement(aiplacement_t *p)
{
	free(p->p);
	tet_FreeWell(p->well);
	free(p->actions);
	if (p->nfollow > 0) {
		free(p->follow);
	}
	free(p);
}

aiplacement_t *tet_CopyAIPlacement(const aiplacement_t *p)
{
	int		i;
	aiplacement_t	*pnew = tet_NewAIPlacement(p->p->tet,
		p->well, p->rotsys, p->follow, p->nfollow);

	pnew->nactions = p->nactions;
	for (i = 0; i < p->nactions; ++i) {
		pnew->actions[i] = p->actions[i];
	}

	pnew->fitness = p->fitness;

	tet_CopyPlacement(pnew->p, p->p);

	return pnew;
}

void tet_AppendAction(aiplacement_t *p, gamekey_t action)
{
	if (p->nactions < TET_ACTIONBUFF_SIZE) {
		p->actions[p->nactions] = action;
		p->nactions += 1;
	}
}

float tet_GetContactScore(const aiparams_t *params, const aiplacement_t *p)
{
	int	area = 0;
	int	i, j, x, y;
	tile_t	*data = p->p->state;

	for (i = 0, y = p->p->y; i < 4; ++y, ++i) {
		for (j = 0, x = p->p->x; j < 4; ++x, ++j) {
			if (data[j + i*4] != TILE_NULL) {
				if (tet_GetBlock(p->well, x-1, y)) {
					area += 1;
				}
				if (tet_GetBlock(p->well, x+1, y)) {
					area += 1;
				}
				if (tet_GetBlock(p->well, x, y-1)) {
					area += 1;
				}
				if (tet_GetBlock(p->well, x, y+1)) {
					area += 1;
				}
				if (y+1 >= TET_WELL_H) {
					area += 1;
				}
			}
		}
	}
	return params->area_weight * area;
}

float tet_GetCoverScore(const aiparams_t *params, const aiplacement_t *p)
{
	float	holes = 0;
	int	i, j, x, y, y2;
	float	mult;

	tile_t	*data = p->p->state;
	for (i = 0, x = p->p->x; i < 4; ++x, ++i) {
		for (j = 3, y = p->p->y+3; j >= 0; --y, --j) {
			if (data[i + j*4] != TILE_NULL && !tet_IsLine(p->well, y)) {
				mult = 1.f;
				for (y2 = y+1; y2 < TET_WELL_H; ++y2) {
					if (!tet_TestBlock(p->well, x, y2))
						holes += mult;
					mult *= params->depth_factor;
				}
				break;
			}
		}
	}
	return -holes * params->hole_weight;
}

float tet_GetDepthScore(const aiparams_t *params, const aiplacement_t *p)
{
	return params->depth_weight * p->p->y;
}

float tet_GetGapScore(const aiparams_t *params, const aiplacement_t *p)
{
	int ngaps;
	float score;
	float weight;
	bool gap;
	int i;
	int j;

	score = 0.f;
	weight = -1.f;

	for (i = 0; i < TET_WELL_H; ++i) {
		//ngaps = number of gaps;
		ngaps = 0;
		gap = false;

		for (j = 0; j < TET_WELL_W; ++j) {
			if (!tet_TestBlock(p->well, j, i)) {
				if (!gap)
					ngaps++;
				gap = true;
			} else {
				gap = false;
			}
		}

		score += ngaps * weight;
	}
	return 0.f;
}

float tet_GetLineScore(const aiparams_t *params, const aiplacement_t *p)
{
	int	lines = 0;
	int	y, ystart = p->p->y, ystop = p->p->y+4;

	if (ystart < 0) {
		ystart = 0;
	}
	if (ystop > TET_WELL_H) {
		ystop = TET_WELL_H;
	}

	for (y = ystart; y < ystop; ++y) {
		lines += (tet_IsLine(p->well, y) ? 1 : 0);
	}

	switch (lines) {
		case 1:
			return params->single_weight;
		case 2:
			return params->double_weight;
		case 3:
			return params->triple_weight;
		case 4:
			return params->tetris_weight;
	}
	return 0;
}

float tet_GetOverhangScore(const aiparams_t *params, const aiplacement_t *p)
{
	int	overhang = 0;
	int	x, y;
	int	mode;

	for (x = 0; x < TET_WELL_W; ++x) {
		mode = 0;
		for (y = 0; y < TET_WELL_H; ++y) {
			if (tet_IsLine(p->well, y))
				continue;
			if (tet_TestBlock(p->well, x, y))
				mode = 1;
			else overhang += mode;
		}
	}
	return -overhang * params->overhang_weight;
}

float tet_GetPrepScore(const aiparams_t *params, const aiplacement_t *p)
{
	int	illprepared = 0;
	int	y;
	int	density;

	for (y = 0; y < TET_WELL_H; ++y) {
		density = tet_GetLineDensity(p->well, y);
		if (density > 0 && density < 9) {
			illprepared += 1;
		}
	}
	return -illprepared * params->prep_weight;
}

void tet_MergeAIPlacement(const aiparams_t *params, aiplacement_t *p)
{
	p->fitness = tet_GetContactScore(params, p);
	tet_MergePlacement(p->well, p->p);
	p->fitness += tet_GetLineScore(params, p);

	if (p->nfollow > 0) {
		aiplacement_t	*p1 = tet_NewAIPlacement(
			p->follow[0],
			p->well,
			p->rotsys,
			p->nfollow > 1 ? &p->follow[1] : NULL,
			p->nfollow - 1);
		aiplacement_t	*best;

		tet_ClearLines(p1->well);
		best = tet_Place(params, p1);
		p->fitness += best->fitness;

		tet_FreeAIPlacement(best);
		tet_FreeAIPlacement(p1);
	} else {
		p->fitness += tet_GetCoverScore(params, p);
		p->fitness += tet_GetDepthScore(params, p);
		p->fitness += tet_GetOverhangScore(params, p);
		p->fitness += tet_GetPrepScore(params, p);
		p->fitness += tet_GetGapScore(params, p);
	}
}

aiplacement_t *tet_Select(aiplacement_t *p1, aiplacement_t *p2)
{
	if (p2->fitness > p1->fitness) {
		tet_FreeAIPlacement(p1);
		return p2;
	} else {
		tet_FreeAIPlacement(p2);
		return p1;
	}
}

aiplacement_t *tet_BottomAdjustment(const aiparams_t *params, aiplacement_t *p)
{
	int		x0 = p->p->x, y0 = p->p->y, r0 = p->p->rot;
	aiplacement_t	*p1, *best;
	
	best = tet_CopyAIPlacement(p);
	tet_MergeAIPlacement(params, best);

	if (tet_ShiftLeft(p->well, p->p) &&
		tet_TestCollision(p->well, p->p->state, p->p->x, p->p->y-1) &&
		tet_TestCollision(p->well, p->p->state, p->p->x, p->p->y+1)) {
			p1 = tet_CopyAIPlacement(p);
			tet_MergeAIPlacement(params, p1);
			tet_AppendAction(p1, KEY_LEFT);
			best = tet_Select(best, p1);
	}
	p->p->x = x0;

	if (tet_ShiftRight(p->well, p->p) &&
		tet_TestCollision(p->well, p->p->state, p->p->x, p->p->y-1) &&
		tet_TestCollision(p->well, p->p->state, p->p->x, p->p->y+1)) {
			p1 = tet_CopyAIPlacement(p);
			tet_MergeAIPlacement(params, p1);
			tet_AppendAction(p1, KEY_RIGHT);
			best = tet_Select(best, p1);
	}
	p->p->x = x0;

	if (tet_RotateCCW(p->rotsys, p->well, p->p) &&
		tet_TestCollision(p->well, p->p->state, p->p->x, p->p->y-1) &&
		tet_TestCollision(p->well, p->p->state, p->p->x, p->p->y+1)) {
			p1 = tet_CopyAIPlacement(p);
			tet_MergeAIPlacement(params, p1);
			tet_AppendAction(p1, KEY_AROT);
			best = tet_Select(best, p1);

			if (tet_RotateCCW(p->rotsys, p->well, p->p) &&
				tet_TestCollision(p->well, p->p->state, p->p->x, p->p->y-1) &&
				tet_TestCollision(p->well, p->p->state, p->p->x, p->p->y+1)) {
					p1 = tet_CopyAIPlacement(p);
					tet_MergeAIPlacement(params, p1);
					tet_AppendAction(p1, KEY_AROT);
					tet_AppendAction(p1, KEY_AROT);
					best = tet_Select(best, p1);
			}
	}
	p->p->x = x0;
	p->p->y = y0;
	p->p->rot = r0;
	p->p->state = p->rotsys->GetRotState(p->p->tet, r0);

	if (tet_RotateCW(p->rotsys, p->well, p->p) &&
		tet_TestCollision(p->well, p->p->state, p->p->x, p->p->y-1) &&
		tet_TestCollision(p->well, p->p->state, p->p->x, p->p->y+1)) {
			p1 = tet_CopyAIPlacement(p);
			tet_MergeAIPlacement(params, p1);
			tet_AppendAction(p1, KEY_BROT);
			best = tet_Select(best, p1);

			if (tet_RotateCW(p->rotsys, p->well, p->p) &&
				tet_TestCollision(p->well, p->p->state, p->p->x, p->p->y-1) &&
				tet_TestCollision(p->well, p->p->state, p->p->x, p->p->y+1)) {
					p1 = tet_CopyAIPlacement(p);
					tet_MergeAIPlacement(params, p1);
					tet_AppendAction(p1, KEY_BROT);
					tet_AppendAction(p1, KEY_BROT);
					best = tet_Select(best, p1);
			}
	}
	p->p->x = x0;
	p->p->y = y0;
	p->p->rot = r0;
	p->p->state = p->rotsys->GetRotState(p->p->tet, r0);

	tet_AppendAction(best, KEY_SOFT);
	return best;
}

aiplacement_t *tet_TopAdjustment(const aiparams_t *params, aiplacement_t *p)
{
	int		x0 = p->p->x, y0 = p->p->y, n0 = p->nactions;
	aiplacement_t	*best;
	
	tet_Drop(p->well, p->p);
	tet_AppendAction(p, KEY_HARD);
	best = tet_BottomAdjustment(params, p);
	p->nactions -= 1;
	p->p->y = y0;

	while (tet_ShiftRight(p->well, p->p)) {
		tet_AppendAction(p, KEY_RIGHT);
		tet_Drop(p->well, p->p);
		tet_AppendAction(p, KEY_HARD);
		best = tet_Select(best, tet_BottomAdjustment(params, p));
		p->nactions -= 1;
		p->p->y = y0;
	}
	p->nactions = n0;
	p->p->x = x0;

	while (tet_ShiftLeft(p->well, p->p)) {
		tet_AppendAction(p, KEY_LEFT);
		tet_Drop(p->well, p->p);
		tet_AppendAction(p, KEY_HARD);
		best = tet_Select(best, tet_BottomAdjustment(params, p));
		p->nactions -= 1;
		p->p->y = y0;
	}
	p->nactions = n0;
	p->p->x = x0;
	
	return best;
}

aiplacement_t *tet_Place(const aiparams_t *params, aiplacement_t *p)
{
	int		r0 = p->p->rot, n0 = p->nactions;
	tetromino_t	tet = p->p->tet;
	aiplacement_t	*best;

	p->p->state = p->rotsys->GetRotState(p->p->tet, p->p->rot);

	if (!tet_TestPlacement(p->well, p->p)) {
		best = tet_CopyAIPlacement(p);
		return best;
	}
	
	best = tet_TopAdjustment(params, p);
	if (tet == TET_O) {
		return best;
	}

	if (tet_RotateCCW(p->rotsys, p->well, p->p)) {
		tet_AppendAction(p, KEY_AROT);
		best = tet_Select(best, tet_TopAdjustment(params, p));
		if (tet == TET_I || tet == TET_S || tet == TET_Z) {
			return best;
		}
		if (tet_RotateCCW(p->rotsys, p->well, p->p)) {
			tet_AppendAction(p, KEY_AROT);
			best = tet_Select(best, tet_TopAdjustment(params, p));
		}
		p->p->rot = r0;
		p->p->state = p->rotsys->GetRotState(p->p->tet, r0);
		p->nactions = n0;
	}
	
	if (tet_RotateCW(p->rotsys, p->well, p->p)) {
		tet_AppendAction(p, KEY_BROT);
		best = tet_Select(best, tet_TopAdjustment(params, p));
		p->p->rot = r0;
		p->p->state = p->rotsys->GetRotState(p->p->tet, r0);
		p->nactions = n0;
	}

	return best;
}
