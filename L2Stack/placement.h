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
#ifndef tet_placement
#define tet_placement

class RotSys;

struct Placement {
	placement_t	p;
	tetstate_t	state;
	int		delay;
	int		spawnwait, lockwait, mergewait;
	bool		hold_exhaust;
	bool		rot_reset, shift_reset, drop_reset;
	bool		merge;
	int		tls_y, tls_yp, tls_ym;
	list<line_t>	contour;
	color3f		ccolor;

	Placement();

	void UpdateTLS(const well_t *);
	int GetMaxY(const well_t *, int xmod);
	color3f GetColor() const;
	void DrawTLS(const well_t *, Graphics &g, int nudgex, bool hollow);
	void UpdateRot();
	void Draw(const well_t *, Graphics &g, int nudgex, int nudgey);
	void DrawContour(Graphics &g, int x, int y, int nudgex, int nudgey);

	void DrawTet(Graphics &g, tile_t *p, int x, int y, int nudgex, int nudgey);

	void Init(const rotsys_t *);
	void Spawn(const rotsys_t *, tetromino_t, int spawnwait, int lockwait, int mergewait);
	void Swap(const rotsys_t *, tetromino_t);
	bool TestCollision(const well_t *, int xoffs = 0, int yoffs = 0);
	bool RotateCW(const rotsys_t *, const well_t *);
	bool RotateCCW(const rotsys_t *, const well_t *);
	bool ShiftLeft(const well_t *);
	bool ShiftRight(const well_t *);
	bool Advance(const well_t *);
	int HardDrop(const well_t *, bool locking);

	void SetState(tetstate_t news, bool locking = false);
	void Update(System &, const well_t *, bool locking);
};

#endif
