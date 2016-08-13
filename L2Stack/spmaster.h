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
#ifndef tet_spmaster
#define tet_spmaster

class SPMaster : public Game
{
	string		mode, rotsys;
	int		level, speed_offset, wait_offset;
	int		igrade, igp;
	int		score, grade, goal, prev_combo;
	bool		fair_start;
	int		decaybuf;
	int		tetcount;
	int		sonic;
	vector<int>	sectiontimes;
	vector<int>	sectiontets;
	bool		mroll;

	void DoGameOver(System &sys);
	int GetNextLevel(System &, int lines);
	void UpdateScore(System &, int, int);
	void UpdateGrade(System &, int, int);
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
	SPMaster(System &, const string &mode, const tetlist_t &);

	Block *BuildBlock(tile_t t);
	Block *BuildDeathBlock(tile_t t, int row) {return new VanishingBlock(t, 20+10*row);}
	void Update(System &sys);
};

#endif
