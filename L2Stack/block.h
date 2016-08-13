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
#ifndef tet_block
#define tet_block

class Block {
protected:
	char	flash;
	int	green;
	tile_t	tile;
	tile_t	item;

public:
	Block(tile_t tile);
	Block(tile_t tile, int flash);

	tile_t GetTile() {return tile;}

	virtual void Update();
	virtual void Draw(Graphics&, int, int) const;
	void DrawScanline(Graphics&, int, int, int);

	static void Draw(Graphics&, tile_t, int, int);
};

class VanishingBlock : public Block {
	int	ttl;

public:
	VanishingBlock(tile_t, int ttl);

	void Update();
	void Draw(Graphics &, int, int) const;
};

class InvisibleBlock : public Block {
public:
	InvisibleBlock(tile_t tile, int flash) : Block(tile, flash) {}

	void Draw(Graphics &, int, int) const;
};

class GrayoutBlock : public Block {
	int	ttl;

public:
	GrayoutBlock(tile_t, int ttl);

	void Update();
	void Draw(Graphics &, int, int) const;
};

#endif
