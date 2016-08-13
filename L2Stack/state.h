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
#ifndef tetris_state
#define tetris_state

class System;

class State
{
public:
	virtual ~State() {}

	virtual void Update(System &) = 0;
	virtual void Draw(Graphics &) = 0;
	virtual void PostRender(Graphics &) const = 0;
	virtual void Restart(System &) = 0;

	virtual void OnKeyDown(System &, gamekey_t) {}
	virtual void OnKeyUp(System &, gamekey_t) {}
};

#endif
