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
#ifndef tet_graphics
#define tet_graphics

class Graphics {
	SDL_Surface	*screen;
	float		aspect;
	Tileset		*tileset;
	int		width, height;
	bool		tile3d;

	vector<pair<string, Texture*> >	textures;
	vector<pair<string, Font*> >	fonts;

	void SetUpProjection(float w, float h);
	Tileset *LoadTileset(Archive &, const string &);
public:
	Graphics();
	~Graphics();

	void Update(System &);

	void SetUp(System &);
	void TearDown();

	int GetWidth() {return width;}
	int GetHeight() {return height;}

	int LoadTexture(Archive &bigpak, const string &fn);
	int TextureWidth(int tx) const;
	int TextureHeight(int tx) const;
	void DrawTexture(int tx, float, float) const;
	void DrawTexture(int tx, float, float, float, float) const;
	void DrawTextureTiled(int tx, float, float, float, float) const;
	void DrawTextureRegion(int tx, float, float, float, float, float, float) const;

	int LoadFont(Archive &bigpak, const string &fn);
	int TextWidth(int ft, const string &text) const;
	int TextHeight(int ft) const;
	void DrawText(int ft, const string &text, float x, float y) const;
	void DrawTextBox(int ft, const string&, float, float, float, float) const;

	void DrawTile(tile_t tile, float x, float y) const;
	void DrawTile3d(tile_t tile, float x, float y, float z, float r) const;

	void DrawLine(line_t line) const;

	void ClearBuffer();
	void SwapBuffers();
};

#endif
