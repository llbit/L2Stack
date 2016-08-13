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
#ifndef tetris_well
#define tetris_well

class Well {
public:
	struct trail_t {
		vector3f	pos;
		float		alpha;
	};
	struct explosion_t {
		char		ttl;
		color3f		color;
		list<Particle>	particles;
	};
private:
	struct cube_t {
		tile_t	tile;
		float	x;
		float	y;
		float	z;
		float	x_vel;
		float	y_vel;
		float	z_vel;
		float	rot;
	};
	list<cube_t>		cubes;
	list<Particle>		particles;
	char			trail_ttl;
	list<trail_t>		trail;
	list<line_t>		contour;
	list<explosion_t>	explosions;
	well_t			*vwell;

	typedef vector<vector<Block *> > layer_t;
	layer_t	well_layer, temporary_layer;
	bool	clearing, game_over;
	char	clear;
	bool	lca_3d, lca_particle;
	string	lcanim;

	void AddAnim(tile_t, int, int);
	void AddCube(tile_t, float, float);
	void AddExplosion(tile_t, float, float);
	void UpdateAnim(System& sys);
	void DrawCubes(Graphics&) const;
	void DrawParticles(Graphics&) const;
	void DrawExplosions(Graphics&) const;
	void BuildContour(layer_t&);
	void DrawLayer(Graphics&, layer_t&);
	void DrawLayerScanline(Graphics&, layer_t&, int);
	int ClearLines(System&);
	void SetTet(layer_t&, Block*, int x, int y);
	void ClearLine(layer_t&, int);

	void TearDown();
	void SetUp();

public:
	Well(System &);
	~Well();

	void Reset();

	bool IsClearing() {return clearing;}

	void Update(System &);
	void SetTrail(const list<trail_t>&);
	void AddParticles(const list<vector3f>&);
	void Draw(Graphics&, bool, int scanline);
	void DrawLineClear(Graphics&) const;
	void OnGameOver(Game&);

	void AddGarbageLine(int);

	void Merge(Game&, const tile_t*, int, int);
	int Merge(Game&, System&, const placement_t&, int clear);

	const well_t* GetVirtualWell() const {return vwell;}
};

#endif
