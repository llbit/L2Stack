/*
    L2Stack
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

AIDemo::AIDemo(System & sys, const string &mode, const tetlist_t &tets)
	: Game(sys, mode, tets)
{
	this->mode = mode;
	this->SetUp(sys);
}

void AIDemo::SetUp(System &sys)
{
	Regex	re_delays("^\\w+\\((.*,)?delays=(\\d+).*\\)$");

	delays = 5;
	if (re_delays.test(mode)) {
		delays = boost::lexical_cast<int>(re_delays.match(mode)[2]);
	}

	level = 0;
	goal = 0;
	blocks = 0;
	timeout = 0;
	keys.clear();

	SetAggressive();

	sys.GetScript().SetStateInt("goal", goal);
	sys.GetScript().SetStateInt("level", level);

	Game::SetUp(sys);
}

void AIDemo::TearDown()
{
	if (randomizer != NULL) {
		delete randomizer;
		randomizer = NULL;
	}
}

void AIDemo::SetAIParams(const char *pstr)
{
	sscanf(pstr,
		"%f, %f, %f, %f, %f, %f, %f, %f, %f, %f",
		&params.depth_factor,
		&params.area_weight,
		&params.hole_weight,
		&params.depth_weight,
		&params.overhang_weight,
		&params.prep_weight,
		&params.single_weight,
		&params.double_weight,
		&params.triple_weight,
		&params.tetris_weight);
}

void AIDemo::SetAggressive()
{
	// 0.128164, 1.608390, 3.792242, 1.119142, 0.399051, 0.051953, -0.277731, -0.568346, 0.795139, -0.080115
	// 0.142493, 3.465303, 11.604479, 3.695079, 0.790772, 0.421025, -0.099433, 16.166727, 0.110210, -0.450229
	// -0.368764, 16.668798, 0.019274, 5.120242, 5.326426, -4.342116, -2.345706, 8.888012, -0.903848, -1.545342
	// -1.051188, 1.797791, 0.418447, 0.916232, 1.986126, -0.002133, -1.073969, 1.481097, 2.335227, -0.166434
	SetAIParams("-1.051188, 1.797791, 0.418447, 0.916232, 1.986126, -0.002133, -1.073969, 1.481097, 2.335227, -0.166434");
}

void AIDemo::SetBalanced()
{
	// 0.35, 5.81, 1.12, 3.28, 3.67, 3.18, 3.50, 0.50, 1.75, 2.24
	// 0.450552f, 6.40388f, 0.716028f, 3.48363f, 5.3118f, 3.17345f, 4.59771f, 1.18f, 1.98438f, 0.64657f
	// 0.448681, 1.053558, 1.110350, 1.032438, 2.325056, -0.537420, -3.032619, -1.827622, 0.825251, -1.179491
	SetAIParams("0.448681, 1.053558, 1.110350, 1.032438, 2.325056, -0.537420, -3.032619, -1.827622, 0.825251, -1.179491");
}

void AIDemo::SetDefensive()
{
	// -0.91, 1.91, 2.65, 4.69, 3.87, 3.08, 3.26, 2.35, 1.47, 5.14
	// -0.30, 0.61, 3.22, 5.84, 3.65, 5.20, 1.76, 2.54, 0.64, 6.43
	// 0.749308, 1.367139, 0.193878, 1.765027, 2.779387, -0.166094, 0.390988, -1.621448, 1.023724, 0.615388
	// 0.657219, 0.735792, 3.401177, 2.598906, 10.972251, 2.284941, -1.422771, 17.726280, -2.343821, 6.283345
	// 0.390455, 1.954027, 7.895727, 11.508776, 18.357460, 5.376618, 7.569025, 13.613381, 3.712212, 9.992501
	// 0.333253, 1.734988, 7.670566, 6.573225, 8.979251, 0.311121, 3.118781, 7.243810, 1.277643, 10.344501
	SetAIParams("0.333253, 1.734988, 7.670566, 6.573225, 8.979251, 0.311121, 3.118781, 7.243810, 1.277643, 10.344501");
}

rotsys_t *AIDemo::BuildRotSys()
{
	return tet_RotARS();
}

int AIDemo::GetWait(wait_t wait)
{
	switch (wait) {
	case WAIT_ARE:
	case WAIT_LARE:
		return 15;
	case WAIT_CLEAR:
		return 0;//20;
	case WAIT_LOCK:
		return 27;
	}
	return 0;
}

int AIDemo::GetRule(rule_t rule)
{
	switch (rule) {
	case RULE_TLS:
		return 0;
	case RULE_INSTANTSOFT:
		return 1;
	case RULE_SOFTLOCK:
		return 1;
	case RULE_HARDLOCK:
		return 0;
	case RULE_HOLD:
		return 1;
	case RULE_HARD:
		return 1;
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

void AIDemo::Update(System &sys)
{
	if (!keys.empty()) {
		if (timeout > 0) {
			timeout -= 1;
		} else {
			gamekey_t	key = keys.front();
			keys.pop_front();
			switch (key) {
				case KEY_LEFT:
					Game::MoveLeft(sys);
					break;
				case KEY_RIGHT:
					Game::MoveRight(sys);
					break;
				case KEY_AROT:
				case KEY_CROT:
					Game::RotateLeft(sys);
					break;
				case KEY_BROT:
					Game::RotateRight(sys);
					break;
				case KEY_SOFT:
					Game::SoftDrop(sys);
					break;
				case KEY_HARD:
					Game::HardDrop(sys);
					break;
			}
			timeout = 2;
		}
	}
	Game::Update(sys);
}

void AIDemo::OnSpawn(System &sys, tetromino_t, bool first)
{
	/*placement.fitness = -std::numeric_limits<float>::min();
	placement.moves.clear();
	ai.CalcMoves(placement);*/
	int		nfollow = (int)preview.size();
	tetromino_t	*follow = new tetromino_t[nfollow];
	tetlist_t::iterator iter = preview.begin();
	for (int i = 0; iter != preview.end(); ++iter, ++i) {
		follow[i] = *iter;
	}
	curr.p.state = rotsys->GetRotState(curr.p.tet, curr.p.rot);
	aiplacement_t	*cp = tet_NewAIPlacement(curr.p.tet,
		well->GetVirtualWell(), rotsys, follow, nfollow);
	aiplacement_t	*best = tet_Place(&params, cp);
	for (int i = 0; i < best->nactions; ++i) {
		keys.push_back(best->actions[i]);
	}
	timeout = 3;
	delete[] follow;
	tet_FreeAIPlacement(cp);
	tet_FreeAIPlacement(best);
}


void AIDemo::OnLock(System &sys, int lines)
{
	blocks += 4;
	blocks -= lines * 10;

	if (tet_GetTopLineHeight(well->GetVirtualWell()) >= 16) {
		SetDefensive();
	} else if (tet_GetTopLineHeight(well->GetVirtualWell()) >= 6) {
		SetDefensive();
	} else {
		SetAggressive();
	}


	// Send garbage
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

void AIDemo::OnGarbage(System &, int lines)
{
	blocks += lines * 10;

	if (blocks > 100) {
		SetDefensive();
	}
}
