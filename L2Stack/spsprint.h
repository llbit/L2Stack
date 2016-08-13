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
#ifndef tet_spsprint
#define tet_spsprint

class SPSprint : public Game
{
	string		mode, rotsys, rand;
	int		level, goal;
	int		dasg, das, hardlock;

protected:
	rand_t* BuildRandomizer();
	rotsys_t* BuildRotSys();
	int GetRule(rule_t);
	int GetWait(wait_t wait);
	void OnLock(System &, int);
	void SetUp(System &);
	void TearDown();

public:
	SPSprint(System &, const string &mode, const tetlist_t &);
};

#endif
