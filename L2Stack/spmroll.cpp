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

SPMroll::SPMroll(System & sys, const string &mode, const tetlist_t &tets)
	: Game(sys, mode, tets)
{
	this->mode = mode;
	this->SetUp(sys);
}

void SPMroll::SetUp(System &sys)
{
	Regex	re_rot("^\\w+\\((.*,)?rot=(\\w+).*\\)$");
	
	rotsys = string("ars");
	if (re_rot.test(mode)) {
		rotsys = re_rot.match(mode)[2];
	}
	
	sys.GetScript().SetStateInt("level", 0);
	sys.GetScript().SetStateInt("goal", 0);
	sys.GetScript().SetStateInt("score", 0);
	sys.GetScript().SetStateString("rank", "S9");
	Game::SetUp(sys);
	DoCreditRoll(sys, 0);
	draw_lines = false;
}

rotsys_t *SPMroll::BuildRotSys()
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

int SPMroll::GetWait(wait_t wait)
{
	switch (wait) {
	case WAIT_SPEED:
		return 5120;
	case WAIT_DASG:
		return 256;
	case WAIT_FLASH:
		return 3;
	case WAIT_LOCK:
		return 17;
	case WAIT_CLEAR:
		return 6;
	case WAIT_DAS:
		return 6;
	case WAIT_ARE:
		return 12 + 2;// +2 adjustment
	case WAIT_LARE:
		return 6 + 2;// +2 adjustment
	case WAIT_SPAWN:
		return 0;
	case WAIT_PRELOCK:
		return 1;
	default:
		return 0;
	}
}

int SPMroll::GetRule(rule_t rule)
{
	switch (rule) {
	case RULE_TLS:
		return 0;
	case RULE_SOFTLOCK:
		return 1;
	case RULE_HARDLOCK:
		return 1;
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

void SPMroll::DoGameOver(System &sys)
{
	if (credits == 0) {
		if (boost::iequals(rotsys, "SRS") && ntets < 50) {
			sys.GetScript().SetStateString("result", "...");
		} else {
			sys.GetScript().SetStateString("rank", "GM");
			sys.GetScript().SetStateString("result", "Incredible!");
		}
	} else {
		sys.GetScript().SetStateString("rank", "M");
		sys.GetScript().SetStateString("result", "Nice Try");
	}

	Game::DoGameOver(sys);
}
