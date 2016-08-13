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
#ifndef tet_rng
#define tet_rng

class RNG {
	unsigned int	high, low;

public:
	RNG() : high(1), low(0x49616E43) {}

	void Seed(unsigned int seed) {high = seed; low = seed ^ 0x49616E42;}
	unsigned int GetNext() {
		high = (high << 16) + (high >> 16);
		high += low;
		low += high;
		return high;
	}

	float GetFloat() {
		return (float)GetNext() / (float)GetMax(); 
	}

	unsigned int GetMax() {return UINT_MAX;}
};

#endif
