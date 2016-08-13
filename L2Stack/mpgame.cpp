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

MPGame::MPGame(System & sys, const string &mode, const tetlist_t &tets)
	: Game(sys, mode, tets)
{
	this->mode = mode;
	this->SetUp(sys);
}

void MPGame::SetUp(System &sys)
{
	Regex	re_das("^\\w+\\((.*,)?das=(\\d+).*\\)$");
	Regex	re_dasg("^\\w+\\((.*,)?dasg=(\\d+).*\\)$");
	Regex	re_sonic("^\\w+\\((.*,)?sonic=(\\d+).*\\)$");
	Regex	re_rot("^\\w+\\((.*,)?rot=(\\w+).*\\)$");
	Regex	re_rand("^\\w+\\((.*,)?rand=(\\w+).*\\)$");

	dasg = 256;
	if (re_dasg.test(mode)) {
		dasg = boost::lexical_cast<int>(re_dasg.match(mode)[2]);
	}
	das = 12;
	if (re_das.test(mode)) {
		das = boost::lexical_cast<int>(re_das.match(mode)[2]);
	}
	hardlock = 1;
	if (re_sonic.test(mode)) {
		hardlock = boost::lexical_cast<int>(re_sonic.match(mode)[2]);
	}
	rotsys = string("ARS");
	if (re_rot.test(mode)) {
		rotsys = re_rot.match(mode)[2];
	}
	rand = string("TAP");
	if (re_rand.test(mode)) {
		rand = re_rand.match(mode)[2];
	}

	Game::SetUp(sys);
}

void MPGame::TearDown()
{
	if (randomizer != NULL) {
		delete randomizer;
		randomizer = NULL;
	}
}

rand_t* MPGame::BuildRandomizer()
{
	if (boost::iequals(this->rand, "TAP")) {
		return tet_NewRand6Retry();
	} else if (boost::iequals(this->rand, "TGM")) {
		return tet_NewRand4Retry();
	} else if (boost::iequals(this->rand, "BAG")) {
		return tet_NewRandBag();
	} else {
		std::cerr << "Unknown randomizer: " << this->rand << "\n";
		throw "fail";
	}
}

rotsys_t* MPGame::BuildRotSys()
{
	if (boost::iequals(this->rotsys, "ARS")) {
		return tet_RotARS();
	} else if (boost::iequals(this->rotsys, "SRS")) {
		return tet_RotSRS();
	} else {
		std::cerr << "Unknown rotation system: " << this->rotsys << "\n";
		throw "fail";
	}
}

int MPGame::GetWait(wait_t wait)
{
	switch (wait) {
		case WAIT_SPEED:
			return 0;
		case WAIT_DASG:
			return dasg;
		case WAIT_LOCK:
			return 30;
		case WAIT_DAS:
			return das;
		default:
			return 0;
	}
}

int MPGame::GetRule(rule_t rule)
{
	switch (rule) {
	case RULE_TLS:
		return 1;
	case RULE_INSTANTSOFT:
		return 1;
	case RULE_SOFTLOCK:
		return 1 - hardlock;
	case RULE_HARD:
		return 1;
	case RULE_HARDLOCK:
		return hardlock;
	case RULE_HOLD:
		return 1;
	case RULE_ROTRESET:
		return 0;
	case RULE_PREVIEW:
		return 3;
	case RULE_DROPRESET:
	case RULE_IRSRESET:
		return 1;
	default:
		return 0;
	}
}

void MPGame::OnLock(System &sys, int lines)
{
	if (lines > 1) {
		int	rows = lines - 1;
		list<Game *>::iterator	iter;
		for (iter = opponents.begin(); iter != opponents.end(); ++iter) {
			for (int i = 0; i < rows; ++i) {
				(*iter)->AddGarbageLine(sys, sys.GetRNG().GetNext()%10);
			}
		}
	}
}
