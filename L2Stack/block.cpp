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

Block::Block(tile_t tile)
	: tile(tile)
{
	flash = 0;
	green = 100;
	item = TILE_NULL;
}

Block::Block(tile_t tile, int flash)
	: tile(tile), flash(flash+1)
{
	green = 100;
	item = TILE_NULL;
}

void Block::Update()
{
	flash = std::max(flash-1, 0);
	green = std::max(green-1, 0);
}

void Block::Draw(Graphics &g, int x, int y) const
{
	g.DrawTile(tile, (float)x, (float)y);
	if (flash > 0) {
		glPushAttrib(GL_CURRENT_BIT);
		glColor4f(1, 1, 1, 0.67f);
		g.DrawTile(TILE_NULL, (float)x, (float)y);
		glPopAttrib();
	}
}

void Block::DrawScanline(Graphics &g, int x, int y,
		int scanline)
{
	if (scanline >= y-TILESIZE && scanline <= y) {
		green = 100;
	}

	if (green > 0) {
		glPushAttrib(GL_CURRENT_BIT);
		glColor4f(0, 1, 0, green / 100.f);
		g.DrawTile(TILE_NULL, (float)x, (float)y);
		glPopAttrib();
	}
	if (flash > 0) {
		glPushAttrib(GL_CURRENT_BIT);
		glColor4f(1, 1, 1, 0.67f);
		g.DrawTile(TILE_NULL, (float)x, (float)y);
		glPopAttrib();
	}
}

void Block::Draw(Graphics &g, tile_t tile, int x, int y)
{
	g.DrawTile(tile, (float)x, (float)y);
}

VanishingBlock::VanishingBlock(tile_t tile, int ttl)
: Block(tile)
{
	this->ttl = ttl;
}

void VanishingBlock::Update()
{
	ttl = std::max(ttl-1, 0);
}

void VanishingBlock::Draw(Graphics &g, int x, int y) const
{
	if (ttl > 0) {
		glPushAttrib(GL_CURRENT_BIT);
		if (ttl < 20) {
			glColor4f(1, 1, 1, ttl * 0.05f);
		}
		Block::Draw(g, tile, x, y);
		glPopAttrib();

	}
}

void InvisibleBlock::Draw(Graphics &g, int x, int y) const
{
	if (flash > 0) {
		g.DrawTile(tile, (float)x, (float)y);
		glPushAttrib(GL_CURRENT_BIT);
		glColor4f(1, 1, 1, 0.67f);
		g.DrawTile(TILE_NULL, (float)x, (float)y);
		glPopAttrib();
	}
}

GrayoutBlock::GrayoutBlock(tile_t tile, int ttl)
: Block(tile)
{
	this->ttl = ttl;
}

void GrayoutBlock::Update()
{
	ttl = std::max(ttl-1, 0);
}

void GrayoutBlock::Draw(Graphics &g, int x, int y) const
{
	if (ttl > 0) {
		Block::Draw(g, tile, x, y);
	} else {
		Block::Draw(g, TILE_GRAY, x, y);
	}
}
