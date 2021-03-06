/*
    Copyright 2009 Jesper �qvist (jesper@llbit.se)

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
#ifndef evolver_rand_h
#define evolver_rand_h

#include "../tetlib/tetlib.h"

unsigned int quickrand();
void squickrand(unsigned int seed);
float randf();// uniform random number between 0.f and 1.f
float randz();// uniform random number between -1.f and 1.f
tetromino_t randt();// random tetromino
void initrandt();// initialize randomizer

#endif
