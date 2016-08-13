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

Graphics::Graphics()
{
	tile3d = false;
	screen = NULL;
	tileset = NULL;
}

Graphics::~Graphics()
{
	TearDown();
}

void Graphics::Update(System &sys)
{
	tile3d = sys.GetScript().ParseInt("profile.blocks_3d") > 0;
}

int Graphics::LoadTexture(Archive& bigpak, const string &fn)
{
	int	i;
	for (i = 0; i < (int)textures.size(); ++i) {
		if (textures[i].first == fn) {
			return i;
		}
	}
	Texture	*tx = new Texture();
	tx->LoadTexture(bigpak, fn);
	textures.push_back(pair<string, Texture*>(fn, tx));
	return i;
}

int Graphics::TextureWidth(int i) const
{
	if (i >= (int)textures.size() ||
		textures[i].second == NULL) {
		return 0;
	} else {
		return textures[i].second->Width();
	}
}

int Graphics::TextureHeight(int i) const
{
	if (i >= (int)textures.size() ||
		textures[i].second == NULL) {
		return 0;
	} else {
		return textures[i].second->Height();
	}
}

void Graphics::DrawTexture(int tx, float x, float y) const
{
	if (!(tx >= (int)textures.size() ||
		textures[tx].second == NULL)) {
		return textures[tx].second->Draw(x, y);
	}
}

void Graphics::DrawTexture(int tx, float x0, float y0, float x1, float y1) const
{
	if (!(tx >= (int)textures.size() ||
		textures[tx].second == NULL)) {
		return textures[tx].second->Draw(x0, y0, x1, y1);
	}
}

void Graphics::DrawTextureTiled(int tx, float x, float y, float w, float h) const
{
	if (!(tx >= (int)textures.size() ||
		textures[tx].second == NULL)) {
		return textures[tx].second->DrawTiled(x, y, w, h);
	}
}

void Graphics::DrawTextureRegion(int tx, float x, float y, float u0, float v0, float u1, float v1) const
{
	if (!(tx >= (int)textures.size() ||
		textures[tx].second == NULL)) {
		return textures[tx].second->DrawRegion(x, y, u0, v0, u1, v1);
	}
}

int Graphics::LoadFont(Archive &bigpak, const string &fn)
{
	int	i;
	for (i = 0; i < (int)fonts.size(); ++i) {
		if (fonts[i].first == fn) {
			return i;
		}
	}
	Font	*ft = new Font();
	ft->Load(bigpak, fn);
	fonts.push_back(pair<string, Font*>(fn, ft));
	return i;
}

int Graphics::TextWidth(int i, const string &text) const
{
	if (i >= (int)fonts.size() ||
		fonts[i].second == NULL) {
		return 0;
	} else {
		return fonts[i].second->TextWidth(text);
	}
}

int Graphics::TextHeight(int i) const
{
	if (i >= (int)fonts.size() ||
		fonts[i].second == NULL) {
		return 0;
	} else {
		return fonts[i].second->TextHeight();
	}
}

void Graphics::DrawText(int i, const string &text, float x, float y) const
{
	if (!(i >= (int)fonts.size() ||
		fonts[i].second == NULL)) {
		fonts[i].second->DrawText(text, x, y);
	}
}

void Graphics::DrawTextBox(int ft, const string& text, float x, float y, float w, float h) const
{
	if (!(ft >= (int)fonts.size() ||
		fonts[ft].second == NULL)) {
		fonts[ft].second->DrawTextBox(text, x, y, w, h);
	}
}


Tileset *Graphics::LoadTileset(Archive &big, const string &fn)
{
	Tileset	*ts = new Tileset();
	ts->Load(big, fn);
	return ts;
}

void Graphics::DrawTile(tile_t tile, float x, float y) const
{
	static float zoffset = TILESIZE/2.f;

	assert(tileset != NULL);
	if (tile3d) {
		tileset->Draw3d(tile, x, y, zoffset, 0);
	} else {
		tileset->Draw2d(tile, x, y);
	}
}

void Graphics::DrawTile3d(tile_t tile, float x, float y, float z, float r) const
{
	assert(tileset != NULL);
	tileset->Draw3d(tile, x, y, z, r);
}

void Graphics::DrawLine(line_t line) const
{
	glDisable(GL_TEXTURE_2D);
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex3i(line.x0, line.y0, 0);
	glVertex3i(line.x1, line.y1, 0);
	glEnd();
}

void Graphics::SetUp(System& sys)
{
	bool fs = sys.GetScript().ParseInt("profile.fullscreen") != 0;
	width = sys.GetScript().ParseInt("profile.width");
	height = sys.GetScript().ParseInt("profile.height");
	screen = SDL_SetVideoMode(width, height, 32,
		SDL_OPENGL | (fs ? SDL_FULLSCREEN : 0));
	if (screen == NULL) {
		std::cerr << "Unable to set video mode: " << SDL_GetError() << "\n";
		throw "fail";
	}
	glViewport(0, 0, width, height);
	SetUpProjection((float)width, (float)height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH);
	glClearColor(0.f, 0.f, 0.f, 0.f);

	// Lights setup
	glShadeModel(GL_FLAT);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_RESCALE_NORMAL);// compensate for modelview scaling
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	GLfloat glblambient[] = {0.2f, 0.2f, 0.2f, 1.f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, glblambient);

	GLfloat ambient[] = {0, 0, 0, 1};
	GLfloat diffuse[] = {0.9f, 0.9f, 0.9f, 1};
	GLfloat specular[] = {0, 0, 0, 1};
	GLfloat position[] = {0, 0, 5.f, 0};
	GLfloat n = 0;
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_LIGHT0);

	// Load Tetris Tiles
	tileset = LoadTileset(sys.GetArchive(), "data/tiles.png");
}

void Graphics::TearDown()
{
	while (!textures.empty()) {
		Texture *tx = textures.back().second;
		if (tx != NULL) {
			delete tx;
		}
		textures.pop_back();
	}
	while (!fonts.empty()) {
		Font	*ft = fonts.back().second;
		if (ft != NULL) {
			delete ft;
		}
		fonts.pop_back();
	}
	if (tileset != NULL) {
		delete tileset;
		tileset = NULL;
	}
}

void Graphics::SetUpProjection(float w, float h)
{
	aspect = (float)w/(float)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float	aspect = w/h;
	float	fovy = 3.1415f * 45.f / 180.f;
	float	alpha = 1.f / tan(fovy / 2.f);
	float	d = h / (2 * tan(fovy / 2.f));
	float	n = d - 200, f = d + 50;
	float	za = (f + n) / (n - f);
	float	zb = (2 * f * n) / (n - f);
	float	projection[] = {alpha / aspect, 0, 0, 0,
				0, alpha, 0, 0,
				0, 0, za, -1,
				0, 0, zb, 0};
	glMultMatrixf(projection);
	glScalef(1, -1, -1);
	glTranslatef(-w / 2, -h / 2, d);
}

void Graphics::ClearBuffer()
{
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::SwapBuffers()
{
	glFlush();
	SDL_GL_SwapBuffers();
}
