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
#ifndef tet_game
#define tet_game

class Game : public State {
	string	mode, ghost_type;
	bool	ghost_disabled, ghost_hollow;
	bool	game_over;
	int	das, are;
	bool	smooth_moves, dropeffects;
	bool	key_states[NUM_KEYS];
	bool	first_tet;
	int	stepbuf, dasbuf;
	int	scanline;

	void FillPreview();

	void SpawnNextBrick(System&);
	tetromino_t GetNextTet();
	void DoMovement(System&);
	void DoGravity(System&);
	void DrawBrick(Graphics&);
	void ResetDas(System&);

	void DrawPreviews(Graphics&);
	void DrawPreview(Graphics&, tetromino_t);
	void DrawHold(Graphics&);

	void DoIRS(System&);

protected:
	tetlist_t	preview, tets, tetbuff, history;
	list<int>	garbageq;
	list<Game*>	opponents;
	rotsys_t*	rotsys;
	Well*		well;
	Placement	curr;
	tetromino_t	hold;
	int		soft, sonic, active;// Tetromino state variables
	rand_t*		randomizer;
	bool		draw_lines;
	int		credits;
	int		ntets, n4, n3, n2, n1;

	virtual void MergeBrick(System&);

	enum wait_t {
		WAIT_SPEED = 0,
		WAIT_LOCK = 1,
		WAIT_DAS = 2,
		WAIT_ARE = 3,
		WAIT_LARE = 4,
		WAIT_CLEAR = 5,
		WAIT_FLASH = 6,
		WAIT_SPAWN = 7,
		WAIT_PRELOCK = 8,
		WAIT_DASG = 9
	};

	enum rule_t {
		RULE_TLS = 0,
		RULE_SOFTLOCK = 1,
		RULE_HARDLOCK = 2,
		RULE_HOLD = 3,
		RULE_HARD = 4,
		RULE_ROTRESET = 5,
		RULE_PREVIEW = 6,
		RULE_DROPRESET = 7,
		RULE_IRSRESET = 8,
		RULE_INSTANTSOFT = 9
	};

	virtual rand_t* BuildRandomizer() = 0;
	virtual rotsys_t* BuildRotSys() = 0;
	virtual int GetRule(rule_t) = 0;
	virtual int GetWait(wait_t) = 0;
	virtual void OnLock(System &, int lines) {}
	virtual void OnSpawn(System &, tetromino_t, bool) {}
	virtual void OnGarbage(System &, int lines) {}

	virtual void DoGameOver(System &sys);
	void DoCreditRoll(System &sys, int roll = 1);
	bool GameOver();

	bool PollKey(gamekey_t);
	void SetKey(gamekey_t, bool);

	virtual void SetUp(System &);
	virtual void TearDown();

public:
	Game(System &, const string &mode, const tetlist_t &);
	virtual ~Game();

	string GetMode() {return mode;}
	void AddGarbageLine(System&, int);
	const tetlist_t & GetTetHistory() const;

	virtual void Update(System&);
	virtual void Update(System&, bool locking);
	virtual void Draw(Graphics&);
	virtual void PostRender(Graphics&) const;
	void Restart(System&);
	void AddOpponent(Game* opponent) {opponents.push_back(opponent);}

	virtual Block *BuildBlock(tile_t t) {return new Block(t, GetWait(WAIT_FLASH));}
	virtual Block *BuildDeathBlock(tile_t t, int row) {return new GrayoutBlock(t, row*5);}

	void HardDrop(System&);
	void SoftDrop(System&);
	void MoveLeft(System&);
	void MoveRight(System&);
	void RotateLeft(System&);
	void RotateRight(System&);
	void Hold(System&);

	int GetNumTets() const {return ntets;}
	int GetNumTetrises() const {return n4;}
	int GetNumTriples() const {return n3;}
	int GetNumDoubles() const {return n2;}
	int GetNumSingles() const {return n1;}

	virtual void OnKeyDown(System &, gamekey_t);
	virtual void OnKeyUp(System &, gamekey_t);
};

#endif
