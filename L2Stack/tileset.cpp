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

Tileset::Tileset()
{
	tile_width = 0;
}

Tileset::~Tileset()
{
}

void Tileset::Load(Archive &big, const string &fn)
{
	tile_width = Texture::LoadTileset(tiles, big, fn);
}

void Tileset::Draw2d(tile_t tile, float x, float y) const
{
	tiles[tile].Draw(x, y);
}

void Tileset::Draw3d(tile_t tile, float x, float y, float z, float rot) const
{
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tiles[tile].GLTex());

	float offset = -(float)tile_width/2.f;

	glPushMatrix();
	glTranslatef(x - offset, y - offset, z);
	if (rot > 0.f) {
		glRotatef(rot, 1.f, 0.f, 0.f);
	}
 
	glBegin(GL_QUADS);

	// back face
	glTexCoord2f(0.f, 0.f);
	glNormal3f(0, 0, 1);
	glVertex3f(offset, offset, offset);
	glTexCoord2f(1.f, 0.f);
	glNormal3f(0, 0, 1);
	glVertex3f(-offset, offset, offset);
	glTexCoord2f(1.f, 1.f);
	glNormal3f(0, 0, 1);
	glVertex3f(-offset, -offset, offset);
	glTexCoord2f(0.f, 1.f);
	glNormal3f(0, 0, 1);
	glVertex3f(offset, -offset, offset);

	// top face
	glTexCoord2f(0.f, 0.f);
	glNormal3f(0, 1, 0);
	glVertex3f(offset, offset, -offset);
	glTexCoord2f(1.f, 0.f);
	glNormal3f(0, 1, 0);
	glVertex3f(-offset, offset, -offset);
	glTexCoord2f(1.f, 1.f);
	glNormal3f(0, 1, 0);
	glVertex3f(-offset, offset, offset);
	glTexCoord2f(0.f, 1.f);
	glNormal3f(0, 1, 0);
	glVertex3f(offset, offset, offset);

	// bottom face
	glTexCoord2f(0.f, 0.f);
	glNormal3f(0, -1, 0);
	glVertex3f(offset, -offset, -offset);
	glTexCoord2f(1.f, 0.f);
	glNormal3f(0, -1, 0);
	glVertex3f(-offset, -offset, -offset);
	glTexCoord2f(1.f, 1.f);
	glNormal3f(0, -1, 0);
	glVertex3f(-offset, -offset, offset);
	glTexCoord2f(0.f, 1.f);
	glNormal3f(0, -1, 0);
	glVertex3f(offset, -offset, offset);

	// right face
	glTexCoord2f(0.f, 0.f);
	glNormal3f(1, 0, 0);
	glVertex3f(offset, -offset, -offset);
	glTexCoord2f(1.f, 0.f);
	glNormal3f(1, 0, 0);
	glVertex3f(offset, offset, -offset);
	glTexCoord2f(1.f, 1.f);
	glNormal3f(1, 0, 0);
	glVertex3f(offset, offset, offset);
	glTexCoord2f(0.f, 1.f);
	glNormal3f(1, 0, 0);
	glVertex3f(offset, -offset, offset);
	
	// left face
	glTexCoord2f(0.f, 0.f);
	glNormal3f(-1, 0, 0);
	glVertex3f(-offset, -offset, -offset);
	glTexCoord2f(1.f, 0.f);
	glNormal3f(-1, 0, 0);
	glVertex3f(-offset, offset, -offset);
	glTexCoord2f(1.f, 1.f);
	glNormal3f(-1, 0, 0);
	glVertex3f(-offset, offset, offset);
	glTexCoord2f(0.f, 1.f);
	glNormal3f(-1, 0, 0);
	glVertex3f(-offset, -offset, offset);

	// front face
	glTexCoord2f(0.f, 0.f);
	glNormal3f(0, 0, -1);
	glVertex3f(offset, offset, -offset);
	glTexCoord2f(1.f, 0.f);
	glNormal3f(0, 0, -1);
	glVertex3f(-offset, offset, -offset);
	glTexCoord2f(1.f, 1.f);
	glNormal3f(0, 0, -1);
	glVertex3f(-offset, -offset, -offset);
	glTexCoord2f(0.f, 1.f);
	glNormal3f(0, 0, -1);
	glVertex3f(offset, -offset, -offset);

	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glPopMatrix();
}
