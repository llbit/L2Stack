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
#include "tet_incs.h"

enum dwaits {
	W_LVL = 0,
	W_ARE = 1,
	W_LARE = 2,
	W_DAS = 3,
	W_LOCK = 4,
	W_CLEAR = 5
};

const int	death_waits[][6] = {
//	{lvl,	are,	lare,	das,	lock,	clear}
	{99,	16,	12,	10,	30,	12},
	{199,	12,	6,	10,	26,	6},
	{299,	12,	6,	9,	22,	6},
	{399,	6,	6,	8,	18,	6},
	{499,	5,	5,	6,	15,	5},
	{999,	4,	4,	6,	15,	4},
};

const char*	death_grades[] = {
	"S1",
	"S2",
	"S3",
	"S4",
	"S5",
	"S6",
	"S7",
	"S8",
	"S9",
	"GM",
};

SPDeath::SPDeath(System & sys, const string &mode, const tetlist_t &tets)
	: Game(sys, mode, tets)
{
	this->mode = mode;
	this->SetUp(sys);
}

void SPDeath::SetUp(System &sys)
{
	Regex	re_level("^\\w+\\((.*,)?level=(\\d+).*\\)$");
	Regex	re_rot("^\\w+\\((.*,)?rot=(\\w+).*\\)$");
	
	level = 0;
	goal = 100;
	if (re_level.test(mode)) {
		level = boost::lexical_cast<int>(re_level.match(mode)[2]);
	}
	rotsys = string("ars");
	if (re_rot.test(mode)) {
		rotsys = re_rot.match(mode)[2];
	}
	fair_start = level == 0;
	score = 0;
	wait_offset = 0;

	Game::SetUp(sys);
	UpdateState(sys);
}

rotsys_t *SPDeath::BuildRotSys()
{
	if (boost::iequals(rotsys.c_str(), "ARS")) {
		return tet_RotARS();
	} else if (boost::iequals(rotsys.c_str(), "SRS")) {
		return tet_RotSRS();
	} else {
		std::cerr << "Unknown rotation system: " << rotsys << "\n";
		throw "fail";
	}
}

int SPDeath::GetWait(wait_t wait)
{
	while (level > death_waits[wait_offset][W_LVL]) {
		++wait_offset;
	}
	switch (wait) {
		case WAIT_SPEED:
			return 5120;
		case WAIT_DASG:
			return 256;
		case WAIT_FLASH:
			return 3;
		case WAIT_LOCK:
			return death_waits[wait_offset][W_LOCK];
		case WAIT_CLEAR:
			return death_waits[wait_offset][W_CLEAR];
		case WAIT_DAS:
			return death_waits[wait_offset][W_DAS];
		case WAIT_ARE:
			return death_waits[wait_offset][W_ARE] + 2;// +2 adjustment
		case WAIT_LARE:
			return death_waits[wait_offset][W_LARE] + 2;// +2 adjustment
		case WAIT_SPAWN:
			return 0;
		case WAIT_PRELOCK:
			return 1;
		default:
			return 0;
	}
}

int SPDeath::GetRule(rule_t rule)
{
	switch (rule) {
	case RULE_TLS:
		return 0;
	case RULE_SOFTLOCK:
		return 1;
	case RULE_HARDLOCK:
		return 0;
	case RULE_HOLD:
		return 0;
	case RULE_HARD:
		return 0;
	case RULE_ROTRESET:
		return 0;
	case RULE_PREVIEW:
		return 1;
	case RULE_DROPRESET:
		return 1;
	default:
		return 0;
	}
}

void SPDeath::UpdateState(System & sys)
{
	goal = std::min(100 + level - level % 100, 999);
	sys.GetScript().SetStateInt("level", level);
	sys.GetScript().SetStateInt("goal", goal);
	sys.GetScript().SetStateInt("score", score);
}

int SPDeath::GetNextLevel(System &sys, int lines)
{
	int	next = level;
	if (lines) {
		next = level + lines;
		if (goal - next <= 4 && goal - level > 4)
			sys.GetSound().PlaySound("data/sectionwarn.wav");
		if (next > 999) next = 999;
	}
	return next;
}

void SPDeath::OnSpawn(System &sys, tetromino_t, bool first)
{
	int	next = level + 1;

	if (first) return;
	if (next < goal) {
		if (goal - next <= 4 && goal - level > 4)
			sys.GetSound().PlaySound("data/sectionwarn.wav");
		level = next;
		sys.GetScript().SetStateInt("level", level);
	}
}

void SPDeath::OnLock(System &sys, int lines)
{
	static int	prev_combo = 1;
	static int	blocks = 0;
	int		combo;
	int		bravo;
	int		next_level = GetNextLevel(sys, lines);
	int		speed = GetWait(WAIT_LOCK) - active;

	if (speed < 0) speed = 0;
	blocks += 4;
	if (lines) {
		blocks -= lines * 10;
		combo = prev_combo + (2 * lines) - 2;
		bravo = (blocks == 0) ? 4 : 1;

		score += ((level + lines + 2) / 4 + soft + (2 * sonic)) * lines * combo * bravo +
			(next_level + 1) / 2 + speed * 7;
		prev_combo = combo;
	} else {
		prev_combo = 1;	
	}

	if (level < 500 && next_level >= 500) {
		if (fair_start && sys.GetScript().GetStateInt("time") <= (3 * 60 + 25) * 1000) {
			sys.GetScript().SetStateString("rank", "M");
			sys.GetScript().SetStateString("result", "Excellent");
		} else {
			Game::DoGameOver(sys);
		}
	} else if (next_level == 999) {
		sys.GetScript().SetStateString("rank", "GM");
		sys.GetScript().SetStateString("result", "Woah");
		Game::DoCreditRoll(sys);
	}
	level = next_level;
	UpdateState(sys);
	sys.GetScript().SetStateInt("combo", prev_combo);
}
