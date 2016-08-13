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

Game::Game(System &sys, const string &mode, const tetlist_t &tets)
{
	memset(this->key_states, 0, sizeof(*(this->key_states))*NUM_KEYS);
	this->tets = tets;
	this->mode = mode;
}

void Game::SetUp(System& sys)
{
	randomizer = BuildRandomizer();
	rotsys = BuildRotSys();
	curr.Init(rotsys);
	rng_Seed(randomizer->rng, (uint32_t)std::time(NULL));
	credits = 0;
	well = new Well(sys);
	game_over = false;
	stepbuf = 0;
	dasbuf = 0;
	das = 0;
	are = 0;
	soft = 0;
	smooth_moves = sys.GetScript().ParseInt("profile.smooth_moves") > 0;
	ghost_type = sys.GetScript().ParseString("profile.ghost_type");
	ghost_disabled = boost::iequals(ghost_type, "disabled");
	ghost_hollow = boost::iequals(ghost_type, "hollow");
	dropeffects = sys.GetScript().ParseInt("profile.dropeffects") > 0;
	tetbuff = tets;
	hold = TET_NULL;
	first_tet = true;
	draw_lines = true;
	ntets = 0;
	n4 = 0;
	n3 = 0;
	n2 = 0;
	n1 = 0;
	sys.GetScript().SetStateNil("rank");
	sys.GetScript().SetStateBool("game_over", 0);
	sys.GetScript().SetStateBool("roll", 0);
	sys.GetScript().SetStateInt("credits", credits);
	sys.GetScript().SetStateInt("ntets", ntets);
	sys.GetScript().SetStateInt("n4", n4);
	sys.GetScript().SetStateInt("n3", n3);
	sys.GetScript().SetStateInt("n2", n2);
	sys.GetScript().SetStateInt("n1", n1);

	FillPreview();
}

void Game::TearDown()
{
	delete well;
	tet_FreeRand(&randomizer);
	tetbuff.clear();
	history.clear();
	garbageq.clear();
	hold = TET_NULL;
	tet_FreeRotSys(rotsys);
}

void Game::Restart(System &sys)
{
	this->TearDown();
	this->SetUp(sys);
}

Game::~Game()
{
	this->TearDown();
}

bool Game::GameOver()
{
	return game_over;
}

bool Game::PollKey(gamekey_t key)
{
	if (key < NUM_KEYS) {
		return key_states[key];
	}
	return false;
}

void Game::SetKey(gamekey_t key, bool down)
{
	if (key < NUM_KEYS) {
		key_states[key] = down;
	}
}


void Game::AddGarbageLine(System &sys, int hole)
{
	if (game_over) return;
	garbageq.push_back(hole);
}

void Game::Update(System& sys)
{
	Update(sys, true);
}

void Game::Update(System &sys, bool locking)
{
	game_over = sys.GetScript().GetStateBool("game_over");
	scanline = sys.GetScript().GetStateInt("scanline");

	well->Update(sys);
	if (game_over) {
		return;
	} else if (credits > 0) {
		--credits;
		sys.GetScript().SetStateInt("credits", credits);
		if (credits == 0) {
			return DoGameOver(sys);
		}
	}

	if ((int)garbageq.size() >= (tet_GetTopLine(well->GetVirtualWell())+2)) {
		while (!garbageq.empty()) {
			well->AddGarbageLine(garbageq.front());
			garbageq.pop_front();
		}
		DoGameOver(sys);
	}

	if (curr.state != TETS_SPAWNING) {
		if ((PollKey(KEY_LEFT) || PollKey(KEY_RIGHT)) && das > 0) {
			--das;
		}
	}

	if (!game_over) {
		curr.Update(sys, well->GetVirtualWell(), PollKey(KEY_SOFT) && GetRule(RULE_SOFTLOCK));
		if (curr.state == TETS_MOVING) {
			active += 1;
			DoMovement(sys);
			DoGravity(sys);
		} else if (curr.state == TETS_MERGING) {
			MergeBrick(sys);
		}
	}

	if (curr.state == TETS_NONE && !well->IsClearing() && !game_over) {
		if (are > 0) {
			--are;
		} else {
			SpawnNextBrick(sys);
		}
	}

	// GET CONTACT AREA
	if (curr.state == TETS_MOVING) {
		int		nfollow = (int)preview.size();
		tetromino_t	*follow = new tetromino_t[nfollow];
		tetlist_t::iterator iter = preview.begin();
		for (int i = 0; iter != preview.end(); ++iter, ++i) {
			follow[i] = *iter;
		}
		aiparams_t	params;
		params.area_weight = 1.f;
		aiplacement_t	*cp = tet_NewAIPlacement(curr.p.tet,
			well->GetVirtualWell(), rotsys, follow, nfollow);
		tet_CopyPlacement(cp->p, &curr.p);
		sys.GetScript().SetStateInt("area", (int)tet_GetContactScore(&params, cp));
		tet_FreeAIPlacement(cp);
	}
}

void Game::SpawnNextBrick(System &sys)
{
	int	ngarbage = (int)garbageq.size();
	while (!garbageq.empty()) {
		well->AddGarbageLine(garbageq.front());
		garbageq.pop_front();
	}
	OnGarbage(sys, ngarbage);

	ntets += 1;
	tetromino_t	tet;
	if (!preview.empty()) {
		tet = preview.front();
		preview.pop_front();
	} else {
		tet = GetNextTet();
	}
	FillPreview();

	curr.Spawn(rotsys, tet, GetWait(WAIT_SPAWN), GetWait(WAIT_LOCK), GetWait(WAIT_PRELOCK));
	curr.UpdateTLS(well->GetVirtualWell());
	curr.UpdateRot();
	sys.GetScript().SetStateInt("ntets", ntets);

	if (PollKey(KEY_HOLD)) {
		Hold(sys);
	} else if (curr.TestCollision(well->GetVirtualWell(), 0, 0)) {
		curr.state = TETS_MOVING;
		return DoGameOver(sys);
	} else
		DoIRS(sys);

	soft = sonic = active = 0;

	if (credits == 0) {
		OnSpawn(sys, tet, first_tet);
	}
	first_tet = false;

	switch (tet) {
	case TET_I:
		sys.GetSound().PlaySound("data/i.wav");
		break;
	case TET_J:
		sys.GetSound().PlaySound("data/j.wav");
		break;
	case TET_L:
		sys.GetSound().PlaySound("data/l.wav");
		break;
	case TET_O:
		sys.GetSound().PlaySound("data/o.wav");
		break;
	case TET_S:
		sys.GetSound().PlaySound("data/s.wav");
		break;
	case TET_T:
		sys.GetSound().PlaySound("data/t.wav");
		break;
	case TET_Z:
		sys.GetSound().PlaySound("data/z.wav");
		break;
	}
	DoGravity(sys);
}

void Game::DoIRS(System &sys)
{
	if (PollKey(KEY_AROT)) {
		if (curr.RotateCCW(rotsys, well->GetVirtualWell())) {
			sys.GetSound().PlaySound("data/irs.wav");
		}
	} else if (PollKey(KEY_BROT)) {
		if (curr.RotateCW(rotsys, well->GetVirtualWell())) {
			sys.GetSound().PlaySound("data/irs.wav");
		}
	}
}

tetromino_t Game::GetNextTet()
{
	tetromino_t	tet;
	if (tetbuff.size() > 0) {
		tet = tetbuff.front();
		tetbuff.pop_front();
	} else {
		tet = randomizer->GetNext(randomizer);
	}
	history.push_back(tet);
	return tet;
}

void Game::FillPreview()
{
	while ((int)preview.size() < GetRule(RULE_PREVIEW)) {
		preview.push_back(GetNextTet());
	}
}

void Game::DoMovement(System &sys)
{
	if (PollKey(KEY_LEFT) && das == 0) {
		dasbuf -= GetWait(WAIT_DASG);

		while (dasbuf <= -128) {
			if (!curr.ShiftLeft(well->GetVirtualWell())) {
				dasbuf = 0;
				break;
			} else{
				dasbuf += 256;
			}
		}
	} else if (PollKey(KEY_RIGHT) && das == 0) {
		dasbuf += GetWait(WAIT_DASG);

		while (dasbuf >= 128) {
			if (!curr.ShiftRight(well->GetVirtualWell())) {
				dasbuf = 0;
				break;
			} else {
				dasbuf -= 256;
			}
		}
	}
	
	if (PollKey(KEY_SOFT)) {
		SoftDrop(sys);
	} else if (PollKey(KEY_HARD) && GetRule(RULE_HARD)) {
		HardDrop(sys);
	}
}

void Game::DoGravity(System &sys)
{
	int	stepspertick = GetWait(WAIT_SPEED);

	stepbuf += stepspertick;
	while (stepbuf >= 256) {
		if (!curr.Advance(well->GetVirtualWell())) {
			stepbuf = 0;
			break;
		} else {
			stepbuf -= 256;
		}
	}
}

void Game::Draw(Graphics& g)
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	DrawPreviews(g);
	DrawHold(g);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glPushMatrix();
	glTranslatef(0.f, -(float)TILESIZE, 0.f);
	well->Draw(g, scanline < 0 ? draw_lines : false,
			scanline);

	DrawBrick(g);
	glPopMatrix();
}

void Game::DrawPreviews(Graphics &g)
{
	float	xoffset = 3, yoffset = -3;
	tetlist_t::iterator	iter = preview.begin();
	for (int i = 0; iter != preview.end(); ++iter, ++i) {
		glPushMatrix();
		glTranslatef(xoffset*TILESIZE, yoffset*TILESIZE, 0);
		if (i > 0) {
			glScalef(0.5f, 0.5f, 0.5f);
			xoffset += 2.5f;
		} else {
			xoffset += 5;
		}
		DrawPreview(g, *iter);
		glPopMatrix();
	}

}

void Game::DrawPreview(Graphics &g, tetromino_t tet)
{
	int	x = rotsys->preview_x[tet];
	int	y = rotsys->preview_y[tet];
	int	rot = 0;
	curr.DrawTet(g, rotsys->GetRotState(tet, rot), x, y, 0, 0);
}

void Game::DrawHold(Graphics &g)
{
	if (hold != TET_NULL) {
		glPushMatrix();
		glTranslatef(0.5f*TILESIZE, -3.f*TILESIZE, 0);
		glScalef(0.5f, 0.5f, 0.5f);
		DrawPreview(g, hold);
		glPopMatrix();
	}
}

void Game::PostRender(Graphics &g) const
{
	glPushMatrix();
	glTranslatef(0.f, -(float)TILESIZE, 0.f);
	well->DrawLineClear(g);
	glPopMatrix();
}

void Game::DrawBrick(Graphics &g)
{
	if (curr.state == TETS_SPAWNING || curr.state == TETS_NONE) return;

	int	nudgex = dasbuf, nudgey = stepbuf;
	if (!smooth_moves) {
		nudgex = nudgey = 0;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	if (GetRule(RULE_TLS) && !ghost_disabled) {
		glBlendFunc(GL_ZERO, GL_ONE);
		curr.DrawTLS(well->GetVirtualWell(), g, nudgex, ghost_hollow);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		curr.DrawTLS(well->GetVirtualWell(), g, nudgex, ghost_hollow);
	}
	curr.Draw(well->GetVirtualWell(), g, nudgex, nudgey);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
}

void Game::OnKeyDown(System& sys, gamekey_t gamekey)
{
	SetKey(gamekey, true);

	switch (gamekey) {
	case KEY_LEFT:
		MoveLeft(sys);
		break;
	case KEY_RIGHT:
		MoveRight(sys);
		break;
	case KEY_AROT:
	case KEY_CROT:
		RotateLeft(sys);
		break;
	case KEY_BROT:
		RotateRight(sys);
		break;
	case KEY_HARD:
		HardDrop(sys);
		break;
	case KEY_SOFT:
		SoftDrop(sys);
		break;
	case KEY_HOLD:
		Hold(sys);
		break;
	default:
		break;
	}
}

void Game::OnKeyUp(System &sys, gamekey_t gamekey)
{
	SetKey(gamekey, false);

	switch (gamekey) {
	case KEY_LEFT:
	case KEY_RIGHT:
		ResetDas(sys);
		break;
	}
}

void Game::MergeBrick(System &sys)
{
	int		lines;

	if (GetRule(RULE_DROPRESET) > 0) {
		SetKey(KEY_SOFT, false);
		SetKey(KEY_HARD, false);
	}

	if (GetRule(RULE_IRSRESET) > 0) {
		SetKey(KEY_AROT, false);
		SetKey(KEY_BROT, false);
		SetKey(KEY_CROT, false);
	}

	lines = well->Merge(*this, sys, curr.p, GetWait(WAIT_CLEAR));
	curr.state = TETS_NONE;

	are = (lines > 0) ? GetWait(WAIT_LARE) : GetWait(WAIT_ARE);
	ResetDas(sys);
	stepbuf = 0;

	switch (lines) {
	case 4:
		sys.GetSound().PlaySound("data/tetris.wav");
		n4 += 1;
		sys.GetScript().SetStateInt("n4", n4);
		break;
	case 3:
		sys.GetSound().PlaySound("data/triple.wav");
		n3 += 1;
		sys.GetScript().SetStateInt("n3", n3);
		break;
	case 2:
		sys.GetSound().PlaySound("data/double.wav");
		n2 += 1;
		sys.GetScript().SetStateInt("n2", n2);
		break;
	case 1:
		sys.GetSound().PlaySound("data/single.wav");
		n1 += 1;
		sys.GetScript().SetStateInt("n1", n1);
		break;
	}

	if (this->credits == 0) {
		OnLock(sys, lines);
	}
}

void Game::ResetDas(System &sys)
{
	das = GetWait(WAIT_DAS);
	dasbuf = 0;
}

void Game::DoGameOver(System &sys)
{
	if (curr.state != TETS_NONE && curr.state != TETS_SPAWNING) {
		well->Merge(*this, sys, curr.p, GetWait(WAIT_CLEAR));
	}
	well->OnGameOver(*this);
	credits = 0;
	game_over = true;
	draw_lines = false;
	sys.GetScript().SetStateInt("game_over", 1);
	sys.GetScript().SetStateInt("credits", credits);
}

void Game::DoCreditRoll(System &sys, int roll)
{
	credits = 3690;
	sys.GetScript().SetStateBool("roll", roll);
	sys.GetScript().SetStateInt("credits", credits);
}

void Game::HardDrop(System &sys)
{
	if (!GetRule(RULE_HARD) ||
		curr.state != TETS_MOVING ||
		game_over ) {
		return;
	}

	int	drop = curr.HardDrop(well->GetVirtualWell(), GetRule(RULE_HARDLOCK) > 0);
	if (drop > 0) {
		sys.GetSound().PlaySound("data/hard.wav");
		if (curr.state == TETS_LOCKED) {
			sys.GetSound().PlaySound("data/lock.wav");
		}
		stepbuf = 0;
		if (drop > sonic) sonic = drop;

		if (dropeffects) {// Update trail
			int			x = curr.p.x;
			int			y = curr.p.y;
			list<Well::trail_t>	trail;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					tile_t	tile = curr.p.state[i + j*4];
					if (tile != TILE_NULL) {
						Well::trail_t	t;
						float	x1 = (float)(TILESIZE*(x + i));
						float	x2 = x1 + TILESIZE;
						float	y1 = (float)(TILESIZE*(y - drop + j));
						float	y2 = (float)(TILESIZE*(y + j));
						float	a1 = 0.f;
						float	a2 = 0.3f + j * 0.3f / drop;
						t.alpha = a1;
						t.pos = vector3f(x1, y1, 0);
						trail.push_back(t);
						t.pos = vector3f(x2, y1, 0);
						trail.push_back(t);
						t.alpha = a2;
						t.pos = vector3f(x2, y2, 0);
						trail.push_back(t);
						t.pos = vector3f(x1, y2, 0);
						trail.push_back(t);
						break;
					}
				}
			}
			well->SetTrail(trail);
		}
	}
}

void Game::SoftDrop(System &sys)
{
	if (curr.state != TETS_MOVING || game_over ) {
		return;
	}

	if (GetRule(RULE_SOFTLOCK)) {
		++soft;
	}

	if (GetRule(RULE_INSTANTSOFT)) {
		int drop = curr.HardDrop(well->GetVirtualWell(), GetRule(RULE_SOFTLOCK) > 0);

		if (drop > 0) {
			sys.GetSound().PlaySound("data/hard.wav");
			if (curr.state == TETS_LOCKED) {
				sys.GetSound().PlaySound("data/lock.wav");
				MergeBrick(sys);
			}
			stepbuf = 0;
			if (drop > sonic) sonic = drop;
		}
	} else {
		bool	result = curr.Advance(well->GetVirtualWell());

		if (result && dropeffects) {
			list<vector3f>	trail;
			tile_t		*p = curr.p.state;
			for (int y = 0; y < 4; ++y) {
				for (int x = 0; x < 4; ++x) {
					if (p[x + y*4] != TILE_NULL) {
						float	x1 = (float)(TILESIZE*(x + curr.p.x) + rand() % TILESIZE);
						float	y1 = (float)(TILESIZE*(y + curr.p.y) + rand() % TILESIZE);
						trail.push_back(vector3f(x1, y1, 0));
					}
				}
			}
			well->AddParticles(trail);
		}
	}
}

void Game::MoveLeft(System &sys)
{
	ResetDas(sys);

	if (curr.state != TETS_MOVING || game_over) {
		return;
	}

	if (curr.ShiftLeft(well->GetVirtualWell())) {
		sys.GetSound().PlaySound("data/shift.wav");
	}
}

void Game::MoveRight(System &sys)
{
	ResetDas(sys);

	if (curr.state != TETS_MOVING || game_over) {
		return;
	}

	if (curr.ShiftRight(well->GetVirtualWell())) {
		sys.GetSound().PlaySound("data/shift.wav");
	}
}

void Game::RotateLeft(System &sys)
{
	if (curr.state != TETS_MOVING || game_over) {
		return;
	}

	if (curr.RotateCCW(rotsys, well->GetVirtualWell())) {
		sys.GetSound().PlaySound("data/rotate.wav");
	}
}

void Game::RotateRight(System &sys)
{
	if (curr.state != TETS_MOVING || game_over) {
		return;
	}

	if (curr.RotateCW(rotsys, well->GetVirtualWell())) {
		sys.GetSound().PlaySound("data/rotate.wav");
	}
}

void Game::Hold(System &sys)
{
	if (GetRule(RULE_HOLD) == 0) {
		return;
	}
	if (curr.state != TETS_MOVING || game_over) {
		return;
	}

	if (!curr.hold_exhaust) {
		tetromino_t	next;
		if (hold == TET_NULL) {
			next = preview.front();
			preview.pop_front();
			FillPreview();
		} else {
			next = hold;
		}
		hold = curr.p.tet;
		curr.Swap(rotsys, next);
		curr.UpdateTLS(well->GetVirtualWell());
		curr.UpdateRot();
		sys.GetSound().PlaySound("data/hold.wav");
		DoIRS(sys);
	}
}

const tetlist_t & Game::GetTetHistory() const
{
	return history;
}
