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

void Font::Load(Archive &big, const string &fn)
{
	SDL_Surface	*surface;
	i8		*data;
	GLenum		texture_format;

	surface = Texture::LoadSurface(big, fn);
	texture_format = Texture::SurfaceFormat(surface);

	// The character width must be less than or equal to the line height
	int	glyph_w = surface->h - 1;
	font_height = glyph_w;
	int	size = glyph_w * (int)sqrt(surface->w / (float)glyph_w);
	int	surface_h, surface_w;
	surface_h = surface_w = Texture::TextureSize(size);

	for (int i = 0; i < 256; ++i) {
		glyphs[i].width = 0;
		glyphs[i].u0 = 0.f;
		glyphs[i].u1 = 0.f;
		glyphs[i].v0 = 0.f;
		glyphs[i].v1 = 0.f;
	}

	data = new i8[surface_w*surface_h * surface->format->BytesPerPixel];
	memset(data, 0, surface_w*surface_h * surface->format->BytesPerPixel);
	int	start = 0, bpp = surface->format->BytesPerPixel, sx = 0, sy = 0;
	i8	*pixels = (i8 *)surface->pixels;
	int	character = 33;
	for (int x = 1; x < surface->w; ++x) {
		if (!memcmp((void *)pixels, (void *)&pixels[x*bpp], bpp)) {
			int	width = x-start;
			if (sx+width >= surface_w) {
				sx = 0;
				sy += glyph_w;
				if (sy >= surface_h) {
					std::cerr << "Font format is messed up.\n";
					throw "fail";
				}
			}
			for (int y = 1; y < surface->h; ++y) {
				memcpy(&data[((sy + y-1)*surface_w + sx)*bpp],
					&pixels[y*surface->pitch + start*bpp],
					width*bpp);
			}
			glyphs[character].width = width;
			glyphs[character].u0 = sx / (float)surface_w;
			glyphs[character].u1 = (sx + width) / (float)surface_w;
			glyphs[character].v0 = sy / (float)surface_h;
			glyphs[character].v1 = (sy + glyph_w) / (float)surface_h;
			character += 1;
			start = x;
			sx += width;
		}
	}
	glyphs[32].width = glyphs['i'].width;

	texture_format = GL_RGBA;// TODO this should not be hard-coded!!
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, texture_format, surface_w, surface_h, 0,
		texture_format, GL_UNSIGNED_BYTE, data);

	delete[] data;
	SDL_FreeSurface(surface);
}

void Font::DrawText(const string &text, float x, float y) const
{
	string::const_iterator it = text.begin(); 
	while (it != text.end()) {
		int character = *(it++);
		DrawCharacter(x, y, character);
		x += glyphs[character].width;
	}
}

void Font::DrawTextBox(const string &text, float x, float y, float w, float h) const
{
	Regex		regex("\\s");
	vector<string>	words = regex.split(text);
	float		line_w = 0;
	float		line_h = 0;

	for (size_t i = 0; i < words.size(); ++i) {
		float word_w = (float)TextWidth(words[i]);
		if (line_w + word_w >= w) {
			line_h += TextHeight()*1.5f;
			line_w = 0;
		}
		if (line_h >= h) {
			break;
		}
		DrawText(words[i], x+line_w, y+line_h);
		line_w += word_w + TextHeight();
	}
}

void Font::DrawCharacter(float x, float y, int character) const
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glTexCoord2f(glyphs[character].u0, glyphs[character].v0);
	glVertex2f(x, y);
	glTexCoord2f(glyphs[character].u1, glyphs[character].v0);
	glVertex2f(x+glyphs[character].width, y);
	glTexCoord2f(glyphs[character].u1, glyphs[character].v1);
	glVertex2f(x+glyphs[character].width, y+font_height);
	glTexCoord2f(glyphs[character].u0, glyphs[character].v1);
	glVertex2f(x, y+font_height);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

int Font::TextWidth(const string &text) const
{
	int	x = 0;
	string::const_iterator it = text.begin();
	while (it != text.end()) {
		int character = *(it++);
		x += glyphs[character].width;
	}
	return x;
}

int Font::TextHeight() const
{
	return font_height;
}
