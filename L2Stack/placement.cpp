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
#include "tet_incs.h"

Placement::Placement()
{
	p.tet = TET_NULL;
	state = TETS_NONE;
}

void Placement::Init(const rotsys_t *rs)
{
	rot_reset = rs->rot_reset == true;
	shift_reset = rs->shift_reset == true;
	drop_reset = rs->drop_reset == true;
}

void Placement::Spawn(const rotsys_t *rs, tetromino_t tet, int sw, int lw, int mw)
{
	p.tet = tet;
	p.x = rs->spawn_x[tet];
	p.y = rs->spawn_y[tet];
	p.rot = 0;
	p.state = rs->GetRotState(tet, p.rot);

	spawnwait = sw;
	lockwait = lw;
	mergewait = mw;
	hold_exhaust = false;

	SetState(TETS_SPAWNING);
}

void Placement::Swap(const rotsys_t *rs, tetromino_t tet)
{
	if (!hold_exhaust && state == TETS_MOVING) {
		p.state = rs->GetRotState(tet, 0);
		p.tet = tet;
		p.x = rs->spawn_x[tet];
		p.y = rs->spawn_y[tet];
		p.rot = 0;
		delay = lockwait;
		hold_exhaust = true;
	}
}

bool Placement::TestCollision(const well_t *well, int xoffs, int yoffs)
{
	return tet_TestCollision(well, p.state, p.x+xoffs, p.y+yoffs) == true;
}

bool Placement::RotateCW(const rotsys_t *rs, const well_t *well)
{
	if (rs->RotateCW(well, &p)) {
		p.state = rs->GetRotState(p.tet, p.rot);
		if (state == TETS_MOVING && rot_reset) delay = lockwait;
		UpdateTLS(well);
		UpdateRot();
		return true;
	}
	return false;
}

bool Placement::RotateCCW(const rotsys_t *rs, const well_t *well)
{
	if (rs->RotateCCW(well, &p)) {
		p.state = rs->GetRotState(p.tet, p.rot);
		if (state == TETS_MOVING && rot_reset) delay = lockwait;
		UpdateTLS(well);
		UpdateRot();
		return true;
	}
	return false;
}

bool Placement::ShiftLeft(const well_t *well)
{
	if (tet_ShiftLeft(well, &p)) {
		if (state == TETS_MOVING && shift_reset) delay = lockwait;
		UpdateTLS(well);
		return true;
	}
	return false;
}

bool Placement::ShiftRight(const well_t *well)
{
	if (tet_ShiftRight(well, &p)) {
		if (state == TETS_MOVING && shift_reset) delay = lockwait;
		UpdateTLS(well);
		return true;
	}
	return false;
}

bool Placement::Advance(const well_t *well)
{
	if (tet_Advance(well, &p)) {
		if (state == TETS_MOVING && drop_reset) delay = lockwait;
		return true;
	}
	return false;
}

int Placement::HardDrop(const well_t *well, bool locking)
{
	int	drop = tet_Drop(well, &p);

	if (state == TETS_MOVING) {
		if (locking) {
			SetState(TETS_LOCKED);
		} else if (drop && drop_reset) {
			delay = lockwait;
		}
	}
	return drop;
}

void Placement::SetState(tetstate_t news, bool locking)
{
	switch (news) {
	case TETS_SPAWNING:
		if (spawnwait == 0) {
			SetState(TETS_MOVING);
		} else {
			delay = spawnwait;
			state = news;
		}
		break;
	case TETS_MOVING:
		if (lockwait == 0 && locking) {
			SetState(TETS_LOCKED);
		} else {
			delay = lockwait;
			state = news;
		}
		break;
	case TETS_LOCKED:
		if (mergewait == 0) {
			SetState(TETS_MERGING);
		} else {
			delay = mergewait;
			state = news;
		}
		break;
	default:
		state = news;
	}
}

void Placement::Update(System &sys, const well_t *well, bool locking)
{
	//active += 1;
	switch (state) {
	case TETS_SPAWNING:
		if (delay > 0) {
			delay -= 1;
		} else {
			SetState(TETS_MOVING, TestCollision(well, 0, +1));
		}
		break;
	case TETS_MOVING:
		if (TestCollision(well, 0, +1)) {
			if (locking) {
				SetState(TETS_LOCKED);
			} else {
				if (delay == lockwait) {
					sys.GetSound().PlaySound("data/drop.wav");
				}
				if (delay > 0) {
					delay -= 1;
				} else {
					SetState(TETS_LOCKED);
				}
			}
		}
		break;
	case TETS_LOCKED:
		if (!TestCollision(well, 0, +1)) {
			state = TETS_MOVING;
		}
		if (delay > 0) {
			delay -= 1;
		} else {
			SetState(TETS_MERGING);
		}
		break;
	}
}

void Placement::UpdateTLS(const well_t *well)
{
	int	x = p.x;
	int	y = p.y;
	tls_y = GetMaxY(well, 0);
	tls_ym = GetMaxY(well, -1);
	tls_yp = GetMaxY(well, +1);

	tls_ym = std::min(tls_y, tls_ym);
	tls_yp = std::min(tls_y, tls_yp);
}

int Placement::GetMaxY(const well_t *well, int xmod)
{
	int max = 0;
	for (; !TestCollision(well, xmod, max+1); ++max);
	return max + p.y;
}

color3f Placement::GetColor() const
{
	tile_t	b = TILE_NULL;
	for (int i = 0; i < 16; ++i) {
		b = p.state[i];
		if (b != TILE_NULL) break;
	}

	return GetTileColor(b);
}

void Placement::UpdateRot()
{
	tile_t*	p = this->p.state;

	ccolor = GetColor();

	contour.clear();
	for (int j = 0; j < 4; ++j) {
		for (int i = 0; i < 4; ++i, ++p) {
			if (*p == TILE_NULL) {
				continue;
			}

			if (i == 0 || *(p-1) == TILE_NULL) {
				line_t new_line;
				new_line.x0 = i;
				new_line.x1 = i;
				new_line.y0 = j;
				new_line.y1 = (j+1);
				contour.push_back(new_line);
			}
			if (i == 3 || *(p+1) == TILE_NULL) {
				line_t new_line;
				new_line.x0 = (i+1);
				new_line.x1 = (i+1);
				new_line.y0 = j;
				new_line.y1 = (j+1);
				contour.push_back(new_line);
			}
			if (j == 0 || *(p-4) == TILE_NULL) {
				line_t new_line;
				new_line.x0 = i;
				new_line.x1 = (i+1);
				new_line.y0 = j;
				new_line.y1 = j;
				contour.push_back(new_line);
			}
			if (j == 3 || *(p+4) == TILE_NULL) {
				line_t new_line;
				new_line.x0 = i;
				new_line.x1 = (i+1);
				new_line.y0 = (j+1);
				new_line.y1 = (j+1);
				contour.push_back(new_line);
			}
		}
	}
}

void Placement::Draw(const well_t *well, Graphics &g, int nudgex, int nudgey)
{
	if ((nudgex > 0 && TestCollision(well, +1, 0)) ||
		(nudgex < 0 && TestCollision(well, -1, 0))) {
		nudgex = 0;
	}

	int	x = p.x;
	int	y = p.y;
	tile_t	*data = p.state;
	if (state == TETS_LOCKED) {// "prelock" dark flash
		glColor3f(0.4f, 0.4f, 0.4f);
		DrawTet(g, data, x, y, 0, 0);
		glColor3f(1.f, 1.f, 1.f);
	} else {
		if (!TestCollision(well, 0, +1)) {
			DrawTet(g, data, x, y, nudgex, nudgey);
		} else {
			float color = 0.55f + (float)delay / (float)lockwait * 0.45f;
			glColor3f(color, color, color);
			DrawTet(g, data, x, y, nudgex, 0);
			glColor3f(1.f, 1.f, 1.f);
		}
	}
}

void Placement::DrawTLS(const well_t *well, Graphics &g, int nudgex, bool hollow)
{
	if ((nudgex > 0 && TestCollision(well, +1, 0)) ||
		(nudgex < 0 && TestCollision(well, -1, 0))) {
		nudgex = 0;
	}
	int	x = p.x;
	int	y = p.y;
	int	my = y;
	if (nudgex == 0) my = tls_y;
	else if (nudgex < 0) my = tls_ym;
	else if (nudgex > 0) my = tls_yp;
	if (my-y > 0) {
		if (hollow) {
			glColor4f(ccolor.r, ccolor.g, ccolor.b, 0.68f);
			DrawContour(g, x, my, nudgex, 0);
			glColor4f(1, 1, 1, 1);
		} else {
			glColor4f(1, 1, 1, 0.47f);
			DrawTet(g, p.state, x, my, nudgex, 0);
			glColor4f(1, 1, 1, 1);
		}
	}
}

void Placement::DrawContour(Graphics &g, int x, int y, int nudgex, int nudgey)
{
	float	xf = nudgex/256.f;
	float	yf = nudgey/256.f;
	int	nx = (int)(xf*TILESIZE);
	int	ny = (int)(yf*TILESIZE);
	list<line_t>::iterator	it;

	for (it = contour.begin(); it != contour.end(); ++it) {
		line_t	line = *it;
		line.x0 = (line.x0 + x)*TILESIZE+nx;
		line.x1 = (line.x1 + x)*TILESIZE+nx;
		line.y0 = (line.y0 + y)*TILESIZE+ny;
		line.y1 = (line.y1 + y)*TILESIZE+ny;
		g.DrawLine(line);
	}
	
}

void Placement::DrawTet(Graphics &g, tile_t *p, int x, int y, int nudgex, int nudgey)
{
	float	xf = nudgex/256.f;
	float	yf = nudgey/256.f;
	int	nx = (int)(xf*TILESIZE);
	int	ny = (int)(yf*TILESIZE);
	for (int y1 = 0; y1 < 4; ++y1) {
		for (int x1 = 0; x1 < 4; ++x1) {
			tile_t	tile = p[x1 + y1*4];
			if (tile != TILE_NULL) {
				Block::Draw(g, tile,
					TILESIZE*(x+x1)+nx,
					TILESIZE*(y+y1)+ny);
			}
		}
	}
}

