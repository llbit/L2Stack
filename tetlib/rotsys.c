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

rotsys_t *tet_RotARS()
{
	int i;
	rotsys_t	*rs = malloc(sizeof(rotsys_t));

	rs->drop_reset = true;
	rs->rot_reset = false;
	rs->shift_reset = false;
	rs->preview_x = malloc(sizeof(int)*TET_COUNT);
	rs->preview_y = malloc(sizeof(int)*TET_COUNT);
	rs->spawn_x = malloc(sizeof(int)*TET_COUNT);
	rs->spawn_y = malloc(sizeof(int)*TET_COUNT);
	rs->RotateCW = &ars_RotateCW;
	rs->RotateCCW = &ars_RotateCCW;
	rs->GetRotState = &ars_GetRotState;
	for (i = 0; i < TET_COUNT; ++i) {
		rs->preview_x[i] = 0;
		rs->preview_y[i] = -2;
		rs->spawn_x[i] = 3;
		rs->spawn_y[i] = -1;
	}
	rs->spawn_y[TET_I] += 1;
	rs->preview_y[TET_I] += 1;
	return rs;
}

rotsys_t *tet_RotSRS()
{
	int i;
	rotsys_t	*rs = malloc(sizeof(rotsys_t));

	rs->drop_reset = true;
	rs->rot_reset = true;
	rs->shift_reset = true;
	rs->preview_x = malloc(sizeof(int)*TET_COUNT);
	rs->preview_y = malloc(sizeof(int)*TET_COUNT);
	rs->spawn_x = malloc(sizeof(int)*TET_COUNT);
	rs->spawn_y = malloc(sizeof(int)*TET_COUNT);
	rs->RotateCW = &srs_RotateCW;
	rs->RotateCCW = &srs_RotateCCW;
	rs->GetRotState = &srs_GetRotState;
	for (i = 0; i < TET_COUNT; ++i) {
		rs->preview_x[i] = 0;
		rs->preview_y[i] = 0;
		rs->spawn_x[i] = 3;
		rs->spawn_y[i] = 1;
	}
	rs->spawn_y[TET_I] -= 1;
	//rs->preview_y[TET_I] += 1;
	return rs;
}

void tet_FreeRotSys(rotsys_t *rs)
{
	free(rs->preview_x);
	free(rs->preview_y);
	free(rs->spawn_x);
	free(rs->spawn_y);
	free(rs);
}
