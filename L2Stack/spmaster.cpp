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

const int	N_MASTER_GRADES = 20;
const char	*master_grades[N_MASTER_GRADES] = {
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
	"M",
	"GM"
};

enum mspeed {
	S_LVL = 0,
	S_SPEED = 1
};

const int	master_speed[][2] = {
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

enum mwaits {
	W_LVL = 0,
	W_ARE = 1,
	W_LARE = 2,
	W_DAS = 3,
	W_LOCK = 4,
	W_CLEAR = 5
};

const int	master_waits[][6] = {
//	{lvl,	are,	l_are,	das,	lock, clear}
	{499,	25,	25,	14,	30,	40},
	{599,	25,	25,	8,	30,	25},
	{699,	25,	16,	8,	30,	16},
	{799,	16,	12,	8,	30,	12},
	{899,	12,	6,	8,	30,	8},
	{999,	12,	6,	6,	17,	6}
};

enum migrade {
	IG_GRADE = 0,
	IG_DECAY = 1,
	IG_SINGLE = 2,
	IG_DOUBLE = 3,
	IG_TRIPLE = 4,
	IG_TETRIS = 5
};

const int	master_igrades[][7] = {
//	{grade,	decay,	single,	double,	triple,	tetris}
	{0,	125,	10,	20,	40,	50},
	{1,	80,	10,	20,	30,	40},
	{2,	80,	10,	20,	30,	40},
	{3,	50,	10,	15,	30,	40}, 
	{4,	45,	10,	15,	20,	40},
	{5,	45,	5,	15,	20,	30},
	{5,	45,	5,	10,	20,	30},
	{6,	40,	5,	10,	15,	30},
	{6,	40,	5,	10,	15,	30},
	{7,	40,	5,	10,	15,	30},
	{7,	40,	2,	12,	13,	30},
	{7,	40,	2,	12,	13,	30},
	{8,	30,	2,	12,	13,	30},
	{8,	30,	2,	12,	13,	30},
	{8,	30,	2,	12,	13,	30},
	{9,	20,	2,	12,	13,	30},
	{9,	20,	2,	12,	13,	30},
	{9,	20,	2,	12,	13,	30},
	{10,	20,	2,	12,	13,	30},
	{11,	20,	2,	12,	13,	30},
	{12,	15,	2,	12,	13,	30},
	{12,	15,	2,	12,	13,	30},
	{12,	15,	2,	12,	13,	30},
	{13,	15,	2,	12,	13,	30},
	{13,	15,	2,	12,	13,	30},
	{14,	15,	2,	12,	13,	30},
	{14,	15,	2,	12,	13,	30},
	{15,	15,	2,	12,	13,	30},
	{15,	15,	2,	12,	13,	30},
	{16,	15,	2,	12,	13,	30},
	{16,	10,	2,	12,	13,	30},
	{17,	10,	2,	12,	13,	30},
};

SPMaster::SPMaster(System & sys, const string &mode, const tetlist_t &tets)
	: Game(sys, mode, tets)
{
	this->mode = mode;
	this->SetUp(sys);
}

void SPMaster::SetUp(System &sys)
{
	Regex	re_level("^\\w+\\((.*,)?level=(\\d+).*\\)$");
	Regex	re_sonic("^\\w+\\((.*,)?sonic=(\\d+).*\\)$");
	Regex	re_rot("^\\w+\\((.*,)?rot=(\\w+).*\\)$");
	
	level = 0;
	goal = 100;
	if (re_level.test(mode)) {
		level = boost::lexical_cast<int>(re_level.match(mode)[2]);
	}
	sonic = 0;
	if (re_sonic.test(mode)) {
		sonic = boost::lexical_cast<int>(re_sonic.match(mode)[2]);
	}
	rotsys = string("ars");
	if (re_rot.test(mode)) {
		rotsys = re_rot.match(mode)[2];
	}
	fair_start = level == 0;
	score = 0;
	prev_combo = 1;
	speed_offset = 0;
	wait_offset = 0;
	igp = 0;
	igrade = 0;
	grade = 0;
	decaybuf = 0;
	tetcount = 0;
	sectiontimes.resize(10, 0);
	sectiontets.resize(10, 0);
	mroll = false;

	Game::SetUp(sys);
	UpdateState(sys);
}

rotsys_t *SPMaster::BuildRotSys()
{
	if (boost::iequals(rotsys, "ARS")) {
		return tet_RotARS();
	} else if (boost::iequals(rotsys, "SRS")) {
		return tet_RotSRS();
	} else {
		std::cerr << "Unknown rotation system: " << rotsys << "\n";
		throw "fail";
	}
}

Block *SPMaster::BuildBlock(tile_t t)
{
	if (mroll) {
		return new InvisibleBlock(t, GetWait(WAIT_FLASH));
	} else {
		return new Block(t, GetWait(WAIT_FLASH));
	}
}

int SPMaster::GetWait(wait_t wait)
{
	while (level >= master_speed[speed_offset + 1][S_LVL]) {
		++speed_offset;
	}
	while (level > master_waits[wait_offset][W_LVL]) {
		++wait_offset;
	}
	switch (wait) {
		case WAIT_SPEED:
			return master_speed[speed_offset][S_SPEED];
		case WAIT_DASG:
			return 256;
		case WAIT_FLASH:
			return 3;
		case WAIT_LOCK:
			return master_waits[wait_offset][W_LOCK];
		case WAIT_CLEAR:
			return master_waits[wait_offset][W_CLEAR];
		case WAIT_DAS:
			return master_waits[wait_offset][W_DAS];
		case WAIT_ARE:
			return master_waits[wait_offset][W_ARE] + 2;// +2 adjustment
		case WAIT_LARE:
			return master_waits[wait_offset][W_LARE] + 2;// +2 adjustment
		case WAIT_SPAWN:
			return 0;
		case WAIT_PRELOCK:
			return 1;
		default:
			return 0;
	}
}

int SPMaster::GetRule(rule_t rule)
{
	switch (rule) {
		case RULE_TLS:
			return level < 100 ? 1 : 0;
		case RULE_SOFTLOCK:
			return 1 - sonic;
		case RULE_HARDLOCK:
			return sonic;
		case RULE_HOLD:
			return 0;
		case RULE_HARD:
			return 1;
		case RULE_ROTRESET:
			return 0;
		case RULE_PREVIEW:
			return 1;
		case RULE_DROPRESET:
			return 0;
		default:
			return 0;
	}
}

void SPMaster::Update(System &sys)
{
	if (prev_combo == 1) {
		int	decayrate = master_igrades[std::min(igrade, 31)][IG_DECAY];
		decaybuf += 1;

		while (decaybuf >= decayrate) {
			igp = std::max(igp-1, 0);
			decaybuf -= decayrate;
		}
	}
	Game::Update(sys);
}

void SPMaster::DoGameOver(System &sys)
{
	if (credits == 0 && mroll) {
		grade = N_MASTER_GRADES - 1;
		sys.GetScript().SetStateString("rank", master_grades[grade]);
		sys.GetScript().SetStateString("result", "Incredible");
	}

	Game::DoGameOver(sys);
}

void SPMaster::UpdateState(System & sys)
{
	goal = std::min(100 + level - level % 100, 999);
	sys.GetScript().SetStateInt("level", level);
	sys.GetScript().SetStateInt("goal", goal);
	sys.GetScript().SetStateInt("score", score);
	sys.GetScript().SetStateString("rank", master_grades[grade]);
}

void SPMaster::OnSpawn(System &sys, tetromino_t, bool first)
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

int SPMaster::GetNextLevel(System & sys, int lines)
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

void SPMaster::OnLock(System &sys, int lines)
{
	int	next = GetNextLevel(sys, lines);

	UpdateScore(sys, next, lines);
	UpdateGrade(sys, next, lines);

	level = next;
	UpdateState(sys);
}

void SPMaster::UpdateScore(System &sys, int next, int lines)
{
	static int	blocks = 0;
	int		combo;
	int		bravo;
	int		speed = GetWait(WAIT_LOCK) - active;

	if (speed < 0) speed = 0;
	blocks += 4;
	if (lines) {
		blocks -= lines * 10;
		combo = prev_combo + (2 * lines) - 2;
		bravo = (blocks == 0) ? 4 : 1;

		score += ((level + lines + 2) / 4 + soft + (2 * sonic)) * lines * combo * bravo +
			(next + 1) / 2 + speed * 7;
		prev_combo = combo;
	} else {
		prev_combo = 1;	
	}

	if (lines == 4) {
		tetcount += 1;
	}

	if (next >= goal) {
		char	diffbuf[64];
		int	section = level / 100;
		int	time = sys.GetScript().GetStateInt("time") / 10;
		int	diff = time - sectiontimes[std::max(section-1, 0)];
		sectiontimes[section] = time;
		sectiontets[section] = tetcount;
		tetcount = 0;
		snprintf(diffbuf, 64, "section%d - %02d:%02d:%02d",
			section+1,
			(diff / 6000) % 60,
			(diff / 100) % 60,
			diff % 100);
		sys.GetScript().SetStateString(string("st") + boost::lexical_cast<string>(section), diffbuf);
	}

	if (next == 999) {
		int	avg5s;
		avg5s = (sectiontimes[0] +
			sectiontimes[1] +
			sectiontimes[2] +
			sectiontimes[3] +
			sectiontimes[4]) / 5;
		mroll = grade == N_MASTER_GRADES - 3 &&
			sys.GetScript().GetStateInt("state.time") / 10 <= 52500 &&
			sectiontimes[0] <= 6500 &&
			sectiontimes[1] <= 6500 &&
			sectiontimes[2] <= 6500 &&
			sectiontimes[3] <= 6500 &&
			sectiontimes[4] <= 6500 &&
			sectiontimes[5] <= avg5s + 200 &&
			sectiontimes[6] <= sectiontimes[5] + 200 &&
			sectiontimes[7] <= sectiontimes[6] + 200 &&
			sectiontimes[8] <= sectiontimes[7] + 200 &&
			sectiontimes[9] <= sectiontimes[8] + 200 &&
			sectiontets[0] >= 2 &&
			sectiontets[1] >= 2 &&
			sectiontets[2] >= 2 &&
			sectiontets[3] >= 2 &&
			sectiontets[4] >= 2 &&
			sectiontets[5] >= 1 &&
			sectiontets[6] >= 1 &&
			sectiontets[7] >= 1 &&
			sectiontets[8] >= 1;
		draw_lines = !mroll;
		DoCreditRoll(sys);
		if (mroll) {
			grade = N_MASTER_GRADES - 2;
			sys.GetScript().SetStateString("result", "Wicked");
		} else {
			sys.GetScript().SetStateString("result", "Smooth");
		}
	}
	
	sys.GetScript().SetStateInt("combo", prev_combo);
}

void SPMaster::UpdateGrade(System &sys, int next, int lines)
{
	static float	combo_mult[][4] = {
		{1,	1,	1,	1},
		{1.2f,	1.4f,	1.5f,	1},
		{1.2f,	1.5f,	1.8f,	1},
		{1.4f,	1.6f,	2,	1},
		{1.4f,	1.7f,	2.2f,	1},
		{1.4f,	1.8f,	2.3f,	1},
		{1.4f,	1.9f,	2.4f,	1},
		{1.5f,	2,	2.5f,	1},
		{1.5f,	2.1f,	2.6f,	1},
		{2,	2.5f,	3,	1}
	};
	static int	combo_size = 1;
	int		points;
	int		ig = std::min(igrade, 31);
	int		lvlmult = 1 + level / 250;

	if (lines == 0) {
		combo_size = 1;
	} else {
		points = master_igrades[std::min(igrade, 31)][IG_SINGLE-1+lines];
		points = (int)(points * combo_mult[combo_size][lines-1] + 0.5f);
		igp += points;
		if (igp >= 100) {
			igp = 0;
			igrade += 1;
			grade = master_igrades[std::min(igrade, 31)][IG_GRADE];
		}
		combo_size += lines - 1;
	}
}
