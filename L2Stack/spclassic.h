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
#ifndef tet_spclassic
#define tet_spclassic

class SPClassic : public Game
{
	string		mode, rotsys;
	int		level, score, grade, goal;
	int		speed_offset;
	bool		fair_start;
	bool		checkpoint1;
	bool		checkpoint2;
	vector<int>	sectiontimes;

	int GetNextLevel(System &, int lines);
	void UpdateScore(System &, int, int);
	void UpdateGrade(System &, int, int);
	void UpdateState(System &);

protected:
	rand_t* BuildRandomizer() {return tet_NewRand4Retry();}
	rotsys_t* BuildRotSys();
	int GetRule(rule_t);
	int GetWait(wait_t wait);
	void OnLock(System &, int);
	void OnSpawn(System &, tetromino_t, bool);
	void SetUp(System &);

public:
	SPClassic(System &, const string &mode, const tetlist_t &);
};

#endif
