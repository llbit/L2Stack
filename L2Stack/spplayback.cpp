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

SPPlayback::SPPlayback(System & sys, const string & fn)
{
	filename = fn;
	this->SetUp(sys);
}

void SPPlayback::SetUp(System &sys)
{
	frame = 0;
	frame_offset = 0;
	game = NULL;

	LoadReplay(sys, filename);
}

void SPPlayback::TearDown()
{
	this->events.clear();
	if (game != NULL) {
		delete game;
		game = NULL;
	}
}

void SPPlayback::Restart(System &sys)
{
	this->TearDown();
	this->SetUp(sys);
}


SPPlayback::~SPPlayback()
{
	this->TearDown();
}

void SPPlayback::LoadReplay(System & sys, const string & fn)
{
	std::ifstream		file;
	Uint32			modelen;
	size_t			nevents, ntets;
	Uint32			nframes;
	tetlist_t	tets;

	size_t orig;
	size_t insize;

	// TODO: Better error detection.
	file.open(fn.c_str(), std::ios::binary);
	if (!file.good()) {
		std::cerr << "Could not open file: " << fn << "\n";
		throw "fail";
	}
	file.read((char *)&modelen, sizeof(modelen));
	char	*buff = new char[modelen];
	file.read(buff, modelen);
	mode = string(buff, modelen);
	delete[] buff;
	file.read((char *)&ntets, sizeof(ntets));
	file.read((char *)&nevents, sizeof(nevents));
	file.read((char *)&nframes, sizeof(nframes));

	this->max_frames = nframes;

	orig = file.tellg();
	file.seekg(0, std::ios_base::end);
	insize = file.tellg();
	insize -= orig;
	file.seekg(orig, std::ios_base::beg);
	Bytef* indata = new Bytef[insize];
	file.read((char*)indata, insize);

	uLong	buffsize = sizeof(tetromino_t)*ntets + sizeof(event_t)*nevents;
	Bytef	*buffer = new Bytef[buffsize];
	int	offset = 0;
	uncompress((Bytef *)buffer, &buffsize, indata, insize);

	for (size_t i = 0; i < ntets; ++i) {
		tetromino_t	tet;
		::memcpy((void *)&tet, (void *)&buffer[offset], sizeof(tet));
		offset += sizeof(tet);
		tets.push_back(tet);
	}
	for (size_t i = 0; i < nevents; ++i) {
		event_t	event;
		::memcpy((void *)&event, (void *)&buffer[offset], sizeof(event));
		offset += sizeof(event);
		events.push_back(event);
	}

	delete[] buffer;
	delete[] indata;

	file.close();

	Regex	re_classic("^classic.*$");
	Regex	re_death("^death.*$");
	Regex	re_master("^master.*$");
	Regex	re_sprint("^sprint.*$");
	Regex	re_easy("^easy.*$");
	Regex	re_mroll("^mroll.*$");

	if (re_classic.test(mode)) {
		game = new SPClassic(sys, mode, tets);
	} else if (re_death.test(mode)) {
		game = new SPDeath(sys, mode, tets);
	} else if (re_master.test(mode)) {
		game = new SPMaster(sys, mode, tets);
	} else if (re_sprint.test(mode)) {
		game = new SPSprint(sys, mode, tets);
	} else if (re_easy.test(mode)) {
		game = new SPEasy(sys, mode, tets);
	} else if (re_mroll.test(mode)) {
		game = new SPMroll(sys, mode, tets);
	} else {
		std::cerr << "Unknown game mode: " << mode << "\n";
		throw "fail";
	}
}

void SPPlayback::Update(System& sys)
{
	if (!events.empty()) {
		while (!events.empty() && frame_offset >= events.front().offset) {
			if (events.front().event == EVENT_KEYDOWN) {
				game->OnKeyDown(sys, (gamekey_t)events.front().data);
			} else if (events.front().event == EVENT_KEYUP) {
				game->OnKeyUp(sys, (gamekey_t)events.front().data);
			}
			events.pop_front();

			frame_offset = 0;
		}
	}

	if (frame < max_frames) {
		game->Update(sys);
		++frame_offset;
		++frame;
		if (frame == max_frames) {
			sys.GetScript().SetStateInt("game_over", 1);
		}
	}
}

void SPPlayback::Draw(Graphics& g)
{	
	game->Draw(g);
}

void SPPlayback::PostRender(Graphics & g) const
{
	game->PostRender(g);
}
