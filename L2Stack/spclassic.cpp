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

const int	N_CLASSIC_GRADES = 19;
const char	*classic_grades[N_CLASSIC_GRADES] = {
	"9",
	"8",
	"7",
	"6",
	"5",
	"4",
	"3",
	"2",
	"1",
	"S1",
	"S2",
	"S3",
	"S4",
	"S5",
	"S6",
	"S7",
	"S8",
	"S9",
	"GM"
};
const int	classic_scores[N_CLASSIC_GRADES-1] = {
	400,
	800,
	1400,
	2000,
	3500,
	5500,
	8000,
	12000,
	16000,
	22000,
	30000,
	40000,
	52000,
	66000,
	82000,
	100000,
	120000,
	INT_MAX,
};

enum cspeed {
	S_LVL = 0,
	S_SPEED = 1
};

const int	classic_speed[][2] = {
//	{lvl,	speed}
	{0,	4},
	{30,	6},
	{35,	8},
	{40,	10},
	{50,	12},
	{60,	16},
	{70,	32},
	{80,	48},
	{90,	64},
	{100,	80},
	{120,	96},
	{140,	112},
	{160,	128},
	{170,	144},
	{200,	4},
	{220,	32},
	{230,	64},
	{233,	96},
	{236,	128},
	{239,	160},
	{243,	192},
	{247,	224},
	{251,	256},
	{300,	512},
	{330,	768},
	{360,	1024},
	{400,	1280},
	{420,	1024},
	{450,	768},
	{500,	5120},
	{INT_MAX,0}
};

SPClassic::SPClassic(System & sys, const string &mode, const tetlist_t &tets)
	: Game(sys, mode, tets)
{
	this->mode = mode;
	this->SetUp(sys);
}

void SPClassic::SetUp(System &sys)
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
	score = 0;
	grade = 0;
	checkpoint1 = false;
	checkpoint2 = false;
	fair_start = level == 0;
	speed_offset = 0;
	sectiontimes.resize(10, 0);

	Game::SetUp(sys);
	UpdateState(sys);
}

rotsys_t *SPClassic::BuildRotSys()
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

int SPClassic::GetWait(wait_t wait)
{
	switch (wait) {
		case WAIT_SPEED:
			while (level >= classic_speed[speed_offset + 1][S_LVL]) {
				++speed_offset;
			}
			return classic_speed[speed_offset][S_SPEED];
		case WAIT_DASG:
			return 256;
		case WAIT_FLASH:
			return 3;
		case WAIT_LOCK:
			return 30;
		case WAIT_CLEAR:
			return 41;
		case WAIT_DAS:
			return 14;
		case WAIT_ARE:
		case WAIT_LARE:
			return 30+2;// +2 adjustment
		case WAIT_SPAWN:
			return 1;
		case WAIT_PRELOCK:
			return 1;
		default:
			return 0;
	}
}

int SPClassic::GetRule(rule_t rule)
{
	switch (rule) {
	case RULE_TLS:
		return level < 100 ? 1 : 0;
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
	default:
		return 0;
	}
}

void SPClassic::UpdateState(System &sys)
{
	goal = std::min(100 + level - level % 100, 999);
	sys.GetScript().SetStateInt("level", level);
	sys.GetScript().SetStateInt("goal", goal);
	sys.GetScript().SetStateInt("score", score);
	sys.GetScript().SetStateString("rank", classic_grades[grade]);
}

void SPClassic::OnSpawn(System &sys, tetromino_t, bool first)
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

int SPClassic::GetNextLevel(System &sys, int lines)
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

void SPClassic::OnLock(System &sys, int lines)
{
	int	next = GetNextLevel(sys, lines);

	UpdateScore(sys, next, lines);
	UpdateGrade(sys, next, lines);

	level = next;
	UpdateState(sys);
}

void SPClassic::UpdateScore(System &sys, int next, int lines)
{
	static int	blocks = 0;
	static int	prev_combo = 1;
	int		bravo, combo;

	if (goal - next <= 4 && goal - level > 4)
		sys.GetSound().PlaySound("data/sectionwarn.wav");

	blocks += 4;
	if (lines) {
		blocks -= lines * 10;
		bravo = (blocks == 0) ? 4 : 1;
		combo = prev_combo + (2 * lines) - 2;
		score += ((level + lines + 2) / 4 + soft) * lines * combo * bravo;
		prev_combo = combo;
	} else {
		prev_combo = 1;
	}

	if (next >= goal) {
		char	diffbuf[64];
		int	section = level / 100;
		int	time = sys.GetScript().GetStateInt("time");
		int	diff = time - sectiontimes[std::max(section-1, 0)];
		sectiontimes[section] = time;
		snprintf(diffbuf, 64, "section%d - %02d:%02d:%02d",
			section+1,
			(diff / 60000) % 60,
			(diff / 1000) % 60,
			(diff / 10) % 100);
		sys.GetScript().SetStateString(string("st") + boost::lexical_cast<string>(section), diffbuf);
	}

	if (next == 999) {
		DoCreditRoll(sys);
		sys.GetScript().SetStateString("result", "Excellent");
	}

	sys.GetScript().SetStateInt("combo", prev_combo);
}

void SPClassic::UpdateGrade(System &sys, int next, int lines)
{
	// Update sub-GM grade
	for (int i = grade; i < N_CLASSIC_GRADES-1; ++i) {
		if (score < classic_scores[i]) {
			grade = i;
			break;
		}
	}

	// Check GM grade conditions
	if (level < 300 && next >= 300 && score >= 12000) {
		checkpoint1 = true;
	} else if (level < 500 && next >= 500 && score >= 40000) {
		checkpoint2 = true;
	}

	// Test GM grade
	if (fair_start &&
		next == 999 && grade == N_CLASSIC_GRADES-2 &&
		score >= 126000 &&
		checkpoint1 && checkpoint2 &&
		sys.GetScript().GetStateInt("time") <= (13 * 60 + 30) * 1000) {
			grade = N_CLASSIC_GRADES-1;
			sys.GetScript().SetStateString("result", "Awesome");
	}
}
