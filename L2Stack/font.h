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
#ifndef tetris_font
#define tetris_font

class Font {
	struct glyph_t {
		float	u0, u1, v0, v1;
		int	width;
	};

	GLuint		texture;
	int		font_height;
	glyph_t		glyphs[256];

	void DrawCharacter(float x, float y, int character) const;
public:

	void Load(Archive &big, const string &fn);

	void DrawText(const string &text, float x, float y) const;
	void DrawTextBox(const string &text, float x, float y, float w, float h) const;

	int TextWidth(const string &text) const;
	int TextHeight() const;
};

#endif
