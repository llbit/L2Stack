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

const int	easy_speed[][2] = {
//	{lvl,	speed}
	{0,	4},
	{8,	5},
	{19,	6},
	{35,	8},
	{40,	10},
	{50,	12},
	{60,	16},
	{70,	32},
	{80,	48},
	{90,	64},
	{100,	4},
	{108,	5},
	{119,	6},
	{125,	8},
	{131,	12},
	{139,	32},
	{149,	48},
	{156,	80},
	{164,	112},
	{174,	128},
	{180,	144},
	{200,	16},
	{212,	48},
	{221,	80},
	{232,	112},
	{244,	144},
	{256,	176},
	{267,	192},
	{277,	208},
	{287,	224},
	{295,	240},
	{300,	5120},
	{INT_MAX,0}
};

SPEasy::SPEasy(System & sys, const string &mode, const tetlist_t &tets)
	: Game(sys, mode, tets)
{
	this->mode = mode;
	this->SetUp(sys);
}

void SPEasy::SetUp(System &sys)
{
	Regex	re_sonic("^\\w+\\((.*,)?sonic=(\\d+).*\\)$");
	Regex	re_rot("^\\w+\\((.*,)?rot=(\\w+).*\\)$");
	
	level = 0;
	goal = 100;
	speed_offset = 0;
	sonic = 0;
	if (re_sonic.test(mode)) {
		sonic = boost::lexical_cast<int>(re_sonic.match(mode)[2]);
	}
	rotsys = string("ars");
	if (re_rot.test(mode)) {
		rotsys = re_rot.match(mode)[2];
	}

	Game::SetUp(sys);
	sys.GetScript().SetStateInt("level", level);
	sys.GetScript().SetStateInt("goal", 100);
}

void SPEasy::TearDown()
{
}

rotsys_t *SPEasy::BuildRotSys()
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

int SPEasy::GetWait(wait_t wait)
{
	switch (wait) {
		case WAIT_SPEED:
			while (level >= easy_speed[speed_offset+1][0]) {
				++speed_offset;
			}
			return easy_speed[speed_offset][1];
		case WAIT_DASG:
			return 256;
		case WAIT_ARE:
		case WAIT_LARE:
			return 25+2;// adjustment
		case WAIT_DAS:
			return 14;
		case WAIT_LOCK:
			return 30;
		case WAIT_CLEAR:
			return 40;
		case WAIT_FLASH:
			return 3;
		case WAIT_PRELOCK:
			return 1;
		default:
			return 0;
	}
}

int SPEasy::GetRule(rule_t rule)
{
	switch (rule) {
	case RULE_TLS:
		return level < 100 ? 1 : 0;
	case RULE_SOFTLOCK:
		return 1 - sonic;
	case RULE_HARDLOCK:
		return sonic;
	case RULE_HOLD:
		return 1;
	case RULE_HARD:
		return 1;
	case RULE_ROTRESET:
		return 0;
	case RULE_PREVIEW:
		return 3;
	case RULE_DROPRESET:
		return 0;
	default:
		return 0;
	}
}

void SPEasy::OnSpawn(System &sys, tetromino_t, bool first)
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

void SPEasy::OnLock(System &sys, int lines)
{
	int	next = std::min(level + lines, 300);
	
	if (goal - next <= 4 && goal - level > 4)
			sys.GetSound().PlaySound("data/sectionwarn.wav");
	if (next == 300) {
		Game::DoCreditRoll(sys);
		sys.GetScript().SetStateString("result", "Right On");
	}

	level = next;
	goal = std::min(100 + level - level % 100, 300);
	sys.GetScript().SetStateInt("level", level);
	sys.GetScript().SetStateInt("goal", goal);
}
