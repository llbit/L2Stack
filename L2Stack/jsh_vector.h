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
#ifndef jsh_vector
#define jsh_vector

namespace jsh {

template <class _t>
struct vector2
{
	_t x, y;

	vector2<_t>() {}
	vector2<_t>(_t x, _t y) {this->x = x;this->y = y;}
	vector2<_t>(const vector2<_t>& _a) {*this = _a;}

	_t dot_product(const vector2<_t>& _a) {
		return x * _a.x + y * _a.y;
	}

	_t length() const {
		return sqrt(x*x + y*y);
	}

	_t square_length() const {
		return (x*x + y*y);
	}

	void normalize() {
		*this = (*this) / length();
	}

	vector2<_t> operator*(const vector2<_t>& _a) const {
		vector2<_t> _b;
		_b.x = x * _a.x;
		_b.y = y * _a.y;
		return _b;
	}

	vector2<_t> operator*(const _t& _a) const {
		vector2<_t> _b;
		_b.x = x * _a;
		_b.y = y * _a;
		return _b;
	}
	vector2<_t> operator/(const _t& _a) const {
		vector2<_t> _b;
		_b.x = x / _a;
		_b.y = y / _a;
		return _b;
	}

	vector2<_t> operator+(const vector2<_t>& _a) const {
		vector2<_t> _b;
		_b.x = x + _a.x;
		_b.y = y + _a.y;
		return _b;
	}
	vector2<_t> operator-(const vector2<_t>& _a) const {
		vector2<_t> _b;
		_b.x = x - _a.x;
		_b.y = y - _a.y;
		return _b;
	}

	const vector2<_t>& operator=(const vector2<_t>& _a) {
		x = _a.x;
		y = _a.y;
		return *this;
	}

	const vector2<_t>& operator+=(const vector2<_t>& _a) {
		x += _a.x;
		y += _a.y;
		return *this;
	}

	const vector2<_t>& operator-=(const vector2<_t>& _a) {
		x -= _a.x;
		y -= _a.y;
		return *this;
	}

	const vector2<_t>& operator*=(const _t& _a) {
		x *= _a;
		y *= _a;
		return *this;
	}

	const vector2<_t>& operator/=(const _t& _a) {
		x /= _a;
		y /= _a;
		return *this;
	}

	bool operator==(const vector2<_t>& _a) const {
		return ((_a.x == x) && (_a.y == y));
	}
};

template <class _t>
struct vector3
{
	_t x, y, z;

	vector3<_t>() {}
	vector3<_t>(_t x, _t y, _t z) {this->x = x;this->y = y;this->z = z;}
	vector3<_t>(const vector3<_t>& _a) {*this = _a;}

	_t dot_product(const vector3<_t>& _a) {
		return x * _a.x + y * _a.y + z * _a.z;
	}

	vector3<_t> cross_product(const vector3<_t>& _a) {
		return vector3<_t>(y*_a.z - z*_a.y,
						   z*_a.x - x*_a.z,
						   x*_a.y - y*_a.x);
	}

	_t length() const {
		return sqrt(x*x + y*y + z*z);
	}

	_t square_length() const {
		return sqrt(x*x + y*y + z*z);
	}

	void normalize() {
		*this = (*this) / length();
	}

	vector3<_t> operator*(const vector3<_t>& _a) const {
		vector3<_t> _b;
		_b.x = x * _a.x;
		_b.y = y * _a.y;
		_b.z = z * _a.z;
		return _b;
	}

	vector3<_t> operator*(const _t& _a) const {
		vector3<_t> _b;
		_b.x = x * _a;
		_b.y = y * _a;
		_b.z = z * _a;
		return _b;
	}
	vector3<_t> operator/(const _t& _a) const {
		vector3<_t> _b;
		_b.x = x / _a;
		_b.y = y / _a;
		_b.z = z / _a;
		return _b;
	}

	vector3<_t> operator+(const vector3<_t>& _a) const {
		vector3<_t> _b;
		_b.x = x + _a.x;
		_b.y = y + _a.y;
		_b.z = z + _a.z;
		return _b;
	}
	vector3<_t> operator-(const vector3<_t>& _a) const {
		vector3<_t> _b;
		_b.x = x - _a.x;
		_b.y = y - _a.y;
		_b.z = z - _a.z;
		return _b;
	}

	vector3<_t>& operator=(const vector3<_t>& _a) {
		x = _a.x;
		y = _a.y;
		z = _a.z;
		return *this;
	}

	const vector3<_t>& operator+=(const vector3<_t>& _a) {
		x += _a.x;
		y += _a.y;
		z += _a.z;
		return *this;
	}

	const vector3<_t>& operator-=(const vector3<_t>& _a) {
		x -= _a.x;
		y -= _a.y;
		z -= _a.z;
		return *this;
	}

	const vector3<_t>& operator*=(const vector3<_t>& _a) {
		x *= _a.x;
		y *= _a.y;
		z *= _a.z;
		return *this;
	}

	const vector3<_t>& operator*=(const _t& _a) {
		x *= _a;
		y *= _a;
		z *= _a;
		return *this;
	}
};

};

typedef jsh::vector2<float> vector2f;
typedef jsh::vector2<int> vector2i;
typedef jsh::vector3<float> vector3f;

#endif
