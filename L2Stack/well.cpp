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

Well::Well(System &sys)
{
	int well_size = TET_WELL_W*TET_WELL_H;
	for (int i = 0; i < TET_WELL_H; ++i) {
		well_layer.push_back(vector<Block *>());
		for (int j = 0; j < TET_WELL_W; ++j) {
			well_layer[i].push_back(NULL);
		}
	}
	temporary_layer = well_layer;
	lcanim = sys.GetScript().ParseString("profile.lcanim");
	lca_3d = lcanim == "3D";
	lca_particle = lcanim == "Particle";
	vwell = tet_NewWell();

	SetUp();
}

Well::~Well()
{
	for (int i = 0; i < TET_WELL_H; ++i) {
		for (int j = 0; j < TET_WELL_W; ++j) {
			SetTet(well_layer, NULL, j, i);
		}
	}
	tet_FreeWell(vwell);
}

void Well::SetUp()
{
	cubes.clear();
	particles.clear();
	trail.clear();
	explosions.clear();
	trail_ttl = 0;
	clearing = false;
	clear = 0;
	game_over = false;
}

void Well::TearDown()
{
	contour.clear();
}

void Well::Reset()
{
	TearDown();
	SetUp();
}

void Well::AddAnim(tile_t tet, int x, int y)
{
	if (lca_3d) AddCube(tet, (float)x, (float)y);
	else if (lca_particle) AddExplosion(tet, (float)x, (float)y);
}

void Well::AddCube(tile_t tile, float x, float y)
{
	cube_t	new_cube;

	new_cube.tile = tile;
	new_cube.x = x;
	new_cube.y = y;
	new_cube.z = -3.f;
	new_cube.x_vel = (x / (float)(10*TILESIZE) - 0.5f)*6.f;
	new_cube.y_vel = -6.f;
	new_cube.z_vel = -1.4f - fabs(new_cube.x_vel)*0.3f;
	new_cube.rot = 0.f;

	cubes.push_back(new_cube);
}

void Well::AddExplosion(tile_t tile, float x, float y)
{
	const int	nparticles = 15;
	explosions.push_back(explosion_t());
	explosion_t	&explosion = explosions.back();
	explosion.ttl = 35;

	// Set up particles
	for (int i = 0; i < nparticles; ++i) {
		float x1 = (float)rand() / RAND_MAX;// Todo: use custom rng
		float y1 = (float)rand() / RAND_MAX;
		Particle p;
		p.p = vector3f(x + x1*TILESIZE, y + y1*TILESIZE, 0.f);
		p.v = vector3f((x1-0.5f)*300.f, (y1-0.5f)*300.f, 0.f);
		p.timetolive = rand() % 15 + 20;
		explosion.particles.push_back(p);
	}

	explosion.color = GetTileColor(tile);
}

void Well::SetTrail(const list<trail_t> &t)
{
	this->trail_ttl = 8;
	this->trail = t;
}

void Well::AddParticles(const list<vector3f> &t)
{
	list<vector3f>::const_iterator	it = t.begin();
	for ( ;	it != t.end(); ++it) {
		Particle p;
		p.p = *it;
		p.timetolive = rand() % 15 + 20;
		particles.push_back(p);
	}
}

void Well::UpdateAnim(System &sys)
{
	list<cube_t>::iterator	it;

	for (it = cubes.begin(); it != cubes.end(); ) {
		it->rot += 23.f;
		it->x += it->x_vel;
		it->y += it->y_vel;
		it->z += it->z_vel;

		it->y_vel += 0.8f;
		if (it->y > 800) {
			cubes.erase(it++);
		} else {
			++it;
		}
	}

	static vector3f	gravity(0.f, 800.f, 0.f);
	ParticleSystem::Update(particles, 0.9f, gravity);

	list<explosion_t>::iterator	iter = this->explosions.begin();
	while (iter != this->explosions.end()) {
		if (--(iter->ttl) < 0) {
			iter = this->explosions.erase(iter);
			continue;
		}

		ParticleSystem::Update(iter->particles, 0.8f, gravity);
		++iter;
	}
}

void Well::DrawCubes(Graphics &g) const
{
	glColor4f(1.f, 1.f, 1.f, 0.8f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	for (list<cube_t>::const_iterator it = cubes.begin();
		it != cubes.end(); ++it) {
			g.DrawTile3d(it->tile, it->x, it->y, it->z, it->rot);
	}
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glColor4f(1.f, 1.f, 1.f, 1.f);
}

void Well::DrawParticles(Graphics &g) const
{
	list<Particle>::const_iterator	iter = this->particles.begin();
	for ( ; iter != particles.end(); ++iter) {
			float s = iter->timetolive / 7.f;
			glPointSize(s);
			glBegin(GL_POINTS);
			glVertex3f(iter->p.x, iter->p.y, 0.f);
			glEnd();
	}
}

void Well::OnGameOver(Game &builder)
{
	game_over = true;
	if (clearing) {
		clearing = false;
		for (int i = 0; i < TET_WELL_H; ++i) {
			well_layer[i] = temporary_layer[i];
		}
	}

	for (int i = 0; i < TET_WELL_H; ++i) {
		for (int j = 0; j < TET_WELL_W; ++j) {
			temporary_layer[i][j] = NULL;
			if (well_layer[i][j] != NULL) {
				tile_t	tile = well_layer[i][j]->GetTile();
				delete well_layer[i][j];
				well_layer[i][j] = builder.BuildDeathBlock(tile, TET_WELL_H - i);
			}
		}
	}

}

void Well::Update(System &sys)
{
	UpdateAnim(sys);

	for (int i = 0; i < TET_WELL_H; ++i) {
		for (int j = 0; j < TET_WELL_W; ++j) {
			if (well_layer[i][j] != NULL) {
				well_layer[i][j]->Update();
			}
			if (temporary_layer[i][j] != NULL) {
				temporary_layer[i][j]->Update();
			}
		}
	}

	if (trail_ttl > 0) {
		trail_ttl -= 1;
	}

	if (game_over) return;

	if (clearing) {
		if (clear > 0) {
			--clear;
		} else {
			sys.GetSound().PlaySound("data/impact.wav");
			BuildContour(well_layer);
			clearing = false;
		}
	}
}

void Well::Draw(Graphics& g, bool draw_lines, int scanline)
{
	// Draw trail
	if (trail_ttl > 0) {
		float	alpha = trail_ttl * 1/16.f;
		glBegin(GL_QUADS);
		list<trail_t>::iterator	iter = trail.begin();
		for ( ; iter != trail.end(); ++iter) {
				glColor4f(1, 1, 1, alpha);
				glVertex3f(iter->pos.x, iter->pos.y, 0);
		}
		glColor4f(1, 1, 1, 1);
		glEnd();
	}

	// Draw particles
	if (particles.size() > 0) {
		DrawParticles(g);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glColor3f(0.7f, 0.7f, 0.7f);
	if (clearing) {
		if (scanline >= 0)
			DrawLayerScanline(g, temporary_layer, scanline);
		else
			DrawLayer(g, temporary_layer);
	} else {
		if (scanline >= 0)
			DrawLayerScanline(g, well_layer, scanline);
		else
			DrawLayer(g, well_layer);
	}
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	if (draw_lines) {
		glColor3f(0.75f, 0.75f, 0.75f);
		list<line_t>::iterator it = contour.begin();
		for (; it != contour.end(); ++it) {
			g.DrawLine(*it);
		}
	}
	glColor3f(1.f, 1.f, 1.f);
}

void Well::DrawLineClear(Graphics &g) const
{
	if (cubes.size() > 0) {
		DrawCubes(g);
	}
	if (explosions.size() > 0) {
		DrawExplosions(g);
	}
}

void Well::DrawExplosions(Graphics &g) const
{
	list<explosion_t>::const_iterator	iter = this->explosions.begin();
	for ( ; iter != this->explosions.end(); ++iter) {
		glColor3f(iter->color.r, iter->color.g, iter->color.b);
		glPointSize(iter->ttl / 7.f);
		glBegin(GL_POINTS);
		list<Particle>::const_iterator	piter = iter->particles.begin();
		for ( ;	piter != iter->particles.end(); ++piter) {
			glVertex3f(piter->p.x, piter->p.y, 0.f);
		}
		glEnd();
	}
	glColor3f(1, 1, 1);
}

void Well::DrawLayer(Graphics& g, layer_t& layer)
{
	for (int i = 0; i < TET_WELL_H; ++i) {
		for (int j = 0; j < TET_WELL_W; ++j) {
			if (layer[i][j] != NULL) {
				layer[i][j]->Draw(g, j*TILESIZE, i*TILESIZE);
			}
		}
	}
}

void Well::DrawLayerScanline(Graphics& g, layer_t& layer,
		int scanline)
{
	for (int i = 0; i < TET_WELL_H; ++i) {
		for (int j = 0; j < TET_WELL_W; ++j) {
			if (layer[i][j] != NULL) {
				layer[i][j]->DrawScanline(g, j*TILESIZE, i*TILESIZE, scanline);
			}
		}
	}
}

void Well::AddGarbageLine(int hole)
{
	for (int x = 0; x < TET_WELL_W; ++x) {// clear row
		SetTet(well_layer, NULL, x, 0);
	}
	for (int y = 0; y < TET_WELL_H-1; ++y) {
		well_layer[y] = well_layer[y+1];
	}
	for (int x = 0; x < TET_WELL_W; ++x) {
		Block	*block = NULL;
		if (x != hole) {
			block = new Block(TILE_GRAY);
		}
		well_layer[TET_WELL_H-1][x] = block;
	}

	tet_AddGarbageLine(vwell, hole);
	
	BuildContour(well_layer);
}

void Well::SetTet(layer_t &layer, Block *block, int x, int y)
{
	if (layer[y][x] != NULL) {
		delete layer[y][x];
	}
	layer[y][x] = block;
}

void Well::ClearLine(layer_t &layer, int y)
{
	// Add clear animation
	for (int x = 0; x < TET_WELL_W; ++x) {
		if (well_layer[y][x] != NULL)
			AddAnim(well_layer[y][x]->GetTile(),
				x*TILESIZE, y*TILESIZE);
	}
	// Clear row
	for (int x = 0; x < TET_WELL_W; ++x) {
		SetTet(layer, NULL, x, y);
	}
	temporary_layer[y] = well_layer[y];
	// Collapse
	for (int i = y-1; i >= 0; --i) {
		well_layer[i+1] = well_layer[i];
	}
	// Clear top row
	for (int i = 0; i < TET_WELL_W; ++i) {
		well_layer[0][i] = NULL;
	}
}

void Well::Merge(Game &builder, const tile_t *state, int xoffs, int yoffs)
{
	// TODO: shit
	for (int y = 0, y1 = yoffs; y < 4; ++y, ++y1) {
		for (int x = 0, x1 = xoffs; x < 4; ++x, ++x1) {
			tile_t	tile = state[x + y*4];
			if (tile != TILE_NULL) {
				SetTet(well_layer, builder.BuildBlock(tile), x1, y1);
			}
		}
	}
	tet_Merge(vwell, state, xoffs, yoffs);
}

int Well::Merge(Game &builder, System &sys, const placement_t &p, int clear)
{
	int	cleared;
	
	// Merge
	Merge(builder, p.state, p.x, p.y);

	// Clear
	cleared = ClearLines(sys);

	if (cleared > 0) {
		tet_ClearLines(vwell);
		if (clear > 0) {
			this->clear = clear;
			clearing = true;
		}
		sys.GetSound().PlaySound("data/clear.wav");
	} else {
		BuildContour(well_layer);
		sys.GetSound().PlaySound("data/lock.wav");
	}

	if (clearing) BuildContour(temporary_layer);
	else BuildContour(well_layer);

	return cleared;
}

int Well::ClearLines(System &sys)
{
	int	x, y, cleared = 0;

	for (y = 0; y < TET_WELL_H; ++y) {
		for (x = 0; x < TET_WELL_W; ++x) {
			if (well_layer[y][x] == NULL) {
				break;
			}
		}
		if (x == TET_WELL_W) {
			ClearLine(well_layer, y);
			++cleared;
		} else temporary_layer[y] = well_layer[y];
	}
	return cleared;
}

void Well::BuildContour(layer_t &layer)
{
	contour.clear();
	for (int y = 0; y < TET_WELL_H; ++y) {
		for (int x = 0; x < TET_WELL_W; ++x) {
			if (layer[y][x] == NULL) continue;

			if (x == 0 || layer[y][x-1] == NULL) {
				line_t new_line;
				new_line.x0 = (x)*TILESIZE;
				new_line.x1 = (x)*TILESIZE;
				new_line.y0 = (y)*TILESIZE;
				new_line.y1 = (y+1)*TILESIZE;
				contour.push_back(new_line);
			}
			if (x == TET_WELL_W - 1 || layer[y][x+1] == NULL) {
				line_t new_line;
				new_line.x0 = (x+1)*TILESIZE;
				new_line.x1 = (x+1)*TILESIZE;
				new_line.y0 = (y)*TILESIZE;
				new_line.y1 = (y+1)*TILESIZE;
				contour.push_back(new_line);
			}
			if (y == 0 || layer[y-1][x] == NULL) {
				line_t new_line;
				new_line.x0 = (x)*TILESIZE;
				new_line.x1 = (x+1)*TILESIZE;
				new_line.y0 = (y)*TILESIZE;
				new_line.y1 = (y)*TILESIZE;
				contour.push_back(new_line);
			}
			if (y == TET_WELL_H - 1 || layer[y+1][x] == NULL) {
				line_t new_line;
				new_line.x0 = (x)*TILESIZE;
				new_line.x1 = (x+1)*TILESIZE;
				new_line.y0 = (y+1)*TILESIZE;
				new_line.y1 = (y+1)*TILESIZE;
				contour.push_back(new_line);
			}
		}
	}
}
