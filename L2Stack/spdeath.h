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
#ifndef tet_spdeath
#define tet_spdeath

class SPDeath : public Game
{
	string	mode, rotsys;
	int	level, score, goal, wait_offset;
	bool	fair_start;

	int GetNextLevel(System &, int lines);

	void UpdateState(System &);

protected:
	rand_t* BuildRandomizer() {return tet_NewRand6Retry();}
	rotsys_t* BuildRotSys();
	int GetRule(rule_t);
	int GetWait(wait_t);
	void OnLock(System &, int);
	void OnSpawn(System &, tetromino_t, bool);
	void SetUp(System &);

public:
	SPDeath(System &, const string &mode, const tetlist_t &);

	Block *BuildDeathBlock(tile_t t, int row) {return new VanishingBlock(t, 20+10*row);}
};

#endif
