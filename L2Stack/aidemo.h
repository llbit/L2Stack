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
#ifndef tet_aidemo
#define tet_aidemo

class AIDemo : public Game
{
	string		mode;
	int		level, goal;
	int		blocks;
	int		delays;
	aiparams_t	params;
	list<gamekey_t>	keys;
	int		timeout;

	void SetAIParams(const char *);
	void SetAggressive();
	void SetBalanced();
	void SetDefensive();

protected:
	rand_t* BuildRandomizer() {return tet_NewRand6Retry();}
	rotsys_t *BuildRotSys();
	int GetRule(rule_t);
	int GetWait(wait_t wait);
	void OnLock(System &, int);
	void OnSpawn(System &, tetromino_t, bool);
	void OnGarbage(System &, int lines);
	void SetUp(System &);
	void TearDown();

public:
	AIDemo(System &, const string &mode, const tetlist_t &);

	void Update(System &);
	void OnKeyDown(System &, gamekey_t) {}
	void OnKeyUp(System &, gamekey_t) {}
};

#endif
