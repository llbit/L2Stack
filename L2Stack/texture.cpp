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

SDL_Surface *Texture::LoadSurface(Archive &big, const string &fn)
{
	void		*data;
	u32		size;
	SDL_Surface	*surface;
	SDL_RWops	*rw;

	big.ReadItem(fn, &data, &size);

	if (data == NULL) {
		std::cerr << "File not found: " << fn << "\n";
		throw "fail";
	}

	rw = SDL_RWFromConstMem((void*)data, (int)size);
	surface = IMG_Load_RW(rw, 1);

	if (surface == NULL) {
		std::cerr << "While loading texture: " << fn << "\n";
		std::cerr << "Unable to load surface data: " << SDL_GetError() << "\n";
		throw "fail";
	}

	return surface;
}

GLenum Texture::SurfaceFormat(const SDL_Surface *surface)
{
	if (surface->format->BytesPerPixel == 4) {
		if (surface->format->Rmask == 0x000000ff) {
			return GL_RGBA;
		} else {
			return GL_BGRA;
		}
	} else if (surface->format->BytesPerPixel == 3) {
		if (surface->format->Rmask == 0x000000ff) {
			return GL_RGB;
		} else {
			return GL_BGR;
		}
	} else {
		std::cerr << "Unsupported color format.\n";
		throw "fail";
	}
}

int Texture::TextureSize(int n)
{
	// Round up to next highest power of two.
	--n;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	++n;
	return n;
}

void Texture::LoadTexture(Archive &big, const string &fn)
{
	SDL_Surface	*surface;
	i8		*data;
	GLuint		texture;
	GLenum		texture_format;

	surface = LoadSurface(big, fn);
	texture_format = SurfaceFormat(surface);
        
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	this->surface_w = TextureSize(surface->w);
	this->surface_h = TextureSize(surface->h);

	data = new i8[surface_w*surface_h * surface->format->BytesPerPixel];
	memset(data, 0, surface_w*surface_h * surface->format->BytesPerPixel);
	for (int y = 0; y < surface->h; ++y) {
		memcpy(
			&data[y*surface_w*surface->format->BytesPerPixel],
			&((i8 *)surface->pixels)[y*surface->pitch],
			surface->pitch);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, surface_w, surface_h, 0,
		texture_format, GL_UNSIGNED_BYTE, data);

	delete[] data;

	this->gl_texture = texture;
	this->image_w = surface->w;
	this->image_h = surface->h;
	this->u1 = (float)surface->w / (float)surface_w;
	this->v1 = (float)surface->h / (float)surface_h;

	SDL_FreeSurface(surface);
}

int Texture::LoadTileset(Texture *ts, Archive &big, const string &fn)
{
	SDL_Surface	*surface;
	i8		*data;
	GLenum		texture_format;
	int		tile_width;

	surface = LoadSurface(big, fn);
	texture_format = SurfaceFormat(surface);

	tile_width = surface->w;

	for (int i = 0; i < TILE_COUNT; ++i) {
		glGenTextures(1, &ts[i].gl_texture);
		glBindTexture(GL_TEXTURE_2D, ts[i].gl_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		ts[i].image_w = ts[i].surface_w = tile_width;
		ts[i].image_h = ts[i].surface_h = tile_width;

		data = new i8[tile_width*tile_width * surface->format->BytesPerPixel];
		for (int y = 0; y < tile_width; ++y) {
			memcpy(
				&data[y*tile_width * surface->format->BytesPerPixel],
				&((i8 *)surface->pixels)[(y+i*tile_width) * surface->pitch],
				surface->pitch);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, surface->format->BytesPerPixel, tile_width, tile_width, 0,
			texture_format, GL_UNSIGNED_BYTE, data);

		delete[] data;
	}

	SDL_FreeSurface(surface);

	return tile_width;
}

void Texture::Draw(float x, float y) const
{
	DrawRegion(x, y, 0, 0, 1, 1);
}

void Texture::Draw(float x0, float y0, float x1, float y1) const
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gl_texture);
 
	glBegin(GL_QUADS);

	glTexCoord2f(0.f, 0.f);
	glVertex2f(x0, y0);
	glTexCoord2f(u1, 0.f);
	glVertex2f(x1, y0);
	glTexCoord2f(u1, v1);
	glVertex2f(x1, y1);
	glTexCoord2f(0.f, v1);
	glVertex2f(x0, y1);
	
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void Texture::DrawTiled(float x, float y, float w, float h) const
{
	float	u0 = 0.f;
	float	u1 = w / (float)surface_w;
	float	v0 = 0.f;
	float	v1 = h / (float)surface_h;
	float	x0 = x;
	float	x1 = x + w;
	float	y0 = y;
	float	y1 = y + h;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gl_texture);
 
	glBegin(GL_QUADS);

	glTexCoord2f(u0, v0);
	glVertex2f(x0, y0);
	glTexCoord2f(u1, v0);
	glVertex2f(x1, y0);
	glTexCoord2f(u1, v1);
	glVertex2f(x1, y1);
	glTexCoord2f(u0, v1);
	glVertex2f(x0, y1);
	
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void Texture::DrawRegion(float x, float y, float u0, float v0, float u1, float v1) const
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gl_texture);

	float us = (float)image_w / surface_w;
	float vs = (float)image_h / surface_h;
 
	float su0 = u0 * us;
	float sv0 = v0 * vs;
	float su1 = u1 * us;
	float sv1 = v1 * vs;
	float x1 = x + (u1-u0) * image_w;
	float y1 = y + (v1-v0) * image_h;

	glBegin(GL_QUADS);
	glTexCoord2f(su0, sv0);
	glVertex2f(x, y);
	glTexCoord2f(su1, sv0);
	glVertex2f(x1, y);
	glTexCoord2f(su1, sv1);
	glVertex2f(x1, y1);
	glTexCoord2f(su0, sv1);
	glVertex2f(x, y1);	
	glEnd();

	glDisable(GL_TEXTURE_2D);
}
