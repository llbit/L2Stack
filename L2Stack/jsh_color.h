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
#ifndef jsh_color
#define jsh_color

namespace jsh {

struct color3f
{
	float r, g, b;

	color3f() : r(0), g(0), b(0) {}
	color3f(float r, float g, float b) : r(r), g(g), b(b) {}
	color3f(int r, int g, int b) : r(r/256.f), g(g/256.f), b(b/256.f) {}
	color3f(const color3f &_a) {*this = _a;}

	const color3f &operator=(const color3f &_a) {
		r = _a.r;
		g = _a.g;
		b = _a.b;
		return *this;
	}

	color3f operator+(const color3f &_a) {
		color3f _b;
		_b.r = r + _a.r;
		_b.g = g + _a.g;
		_b.b = b + _a.b;
		return _b;
	}

	color3f operator-(const color3f &_a) {
		color3f _b;
		_b.r = r - _a.r;
		_b.g = g - _a.g;
		_b.b = b - _a.b;
		return _b;
	}

	color3f operator*(float _a) {
		color3f _b;
		_b.r = r * _a;
		_b.g = g * _a;
		_b.b = b * _a;
		return _b;
	}
};

};

typedef jsh::color3f color3f;

#endif
