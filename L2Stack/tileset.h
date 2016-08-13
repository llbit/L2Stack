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
#ifndef tet_tileset
#define tet_tileset

class Tileset {
	Texture	tiles[TILE_COUNT];
	int	tile_width;

public:
	Tileset();
	~Tileset();

	void Load(Archive &big, const string &fn);

	void Draw2d(tile_t tile, float x, float y) const;
	void Draw3d(tile_t tile, float x, float y, float z, float rot) const;
};


#endif
