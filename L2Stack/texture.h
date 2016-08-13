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
#ifndef tet_texture
#define tet_texture

class Texture {
	GLuint		gl_texture;
	int		image_w, image_h;
	int		surface_w, surface_h;
	float		u1, v1;

public:
	Texture() {}
	~Texture() {}

	static SDL_Surface *LoadSurface(Archive &big, const string &fn);
	static GLenum SurfaceFormat(const SDL_Surface *surface);
	static int TextureSize(int n);

	void LoadTexture(Archive &archive, const string &fn);
	static int LoadTileset(Texture *ts, Archive &archive, const string &fn);

	void Draw(float x, float y) const;
	void Draw(float x0, float y0, float x1, float y1) const;
	void DrawTiled(float x, float y, float w, float h) const;
	void DrawRegion(float x, float y, float u0, float v0, float u1, float v1) const;

	int Width() const {return image_w;}
	int Height() const {return image_h;}
	GLuint GLTex() const {return gl_texture;}
};

#endif
