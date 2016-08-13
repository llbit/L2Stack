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

SPRecorder::SPRecorder(System &sys, const string& mode)
{
	this->mode = mode;
	this->SetUp(sys);
}

void SPRecorder::SetUp(System &sys)
{
	Regex	re_classic("^classic.*$");
	Regex	re_death("^death.*$");
	Regex	re_master("^master.*$");
	Regex	re_sprint("^sprint.*$");
	Regex	re_easy("^easy.*$");
	Regex	re_radar("^radar.*$");
	Regex	re_evolution("^evolution.*$");
	Regex	re_aidemo("^aidemo.*$");
	Regex	re_mroll("^mroll.*$");
	Regex	re_mpgame("^mpgame.*$");

	max_frames = 0;
	frame_offset = 0;
	frame = 0;

	tetlist_t	tets;
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
	} else if (re_radar.test(mode)) {
		game = new SPRadar(sys, mode, tets);
	} else if (re_aidemo.test(mode)) {
		game = new AIDemo(sys, mode, tets);
	} else if (re_mroll.test(mode)) {
		game = new SPMroll(sys, mode, tets);
	} else if (re_mpgame.test(mode)) {
		game = new MPGame(sys, mode, tets);
	} else {
		std::cerr << "Unknown game mode: " << mode << "\n";
		throw "fail";
	}
}

void SPRecorder::TearDown()
{
	this->events.clear();
	if (game != NULL) {
		delete game;
		game = NULL;
	}
}

void SPRecorder::Restart(System &sys)
{
	this->TearDown();
	this->SetUp(sys);
}

SPRecorder::~SPRecorder()
{
	this->TearDown();
}

void SPRecorder::SaveReplay(System & sys, const string & fn)
{
	std::ofstream	file;
	string		mode = game->GetMode();
	Uint32		modelen = (Uint32)mode.size();
	tetlist_t	tets = game->GetTetHistory();
	size_t		nevents = events.size(), ntets = tets.size();
	Uint32		nframes = frame;

	try {
		if (!boost::filesystem::exists("replays"))
			boost::filesystem::create_directory("replays");
		file.open(fn.c_str(), std::ios_base::binary);
		file.write((char *)&modelen, sizeof(modelen));
		file.write(mode.c_str(), mode.size());
		file.write((char *)&ntets, sizeof(ntets));
		file.write((char *)&nevents, sizeof(nevents));
		file.write((char *)&nframes, sizeof(nframes));

		uLong	buffsize = sizeof(tetromino_t)*ntets + sizeof(event_t)*nevents;
		Bytef	*buffer = new Bytef[buffsize];
		int	offset = 0;
		
		for (size_t i = 0; i < ntets; ++i) {
			tetromino_t	tet = tets.front();
			tets.pop_front();
			::memcpy(&buffer[offset], (void *)&tet, sizeof(tet));
			offset += sizeof(tet);
		}
		for (size_t i = 0; i < nevents; ++i) {
			event_t	event = events.front();
			events.pop_front();
			::memcpy(&buffer[offset], (void *)&event, sizeof(event));
			offset += sizeof(event);
		}

		uLong	outsize = (uLong)(buffsize * 1.2f) + 12;
		Bytef	*outdata = new Bytef[outsize];
		compress(outdata, &outsize, (Bytef *)buffer, buffsize);
		file.write((char *)outdata, outsize);

		delete[] buffer;
		delete[] outdata;

		file.close();
	} catch (string errormsg) {
		sys.OnError(string("Error while saving replay: ") + errormsg);
	}
}

void SPRecorder::Update(System& sys)
{
	game->Update(sys);
	++frame_offset;
	++frame;
}

void SPRecorder::Draw(Graphics& g)
{
	game->Draw(g);
}

void SPRecorder::PostRender(Graphics & g) const
{
	game->PostRender(g);
}

void SPRecorder::OnKeyDown(System& sys, gamekey_t key)
{
	if (key < NUM_KEYS) {
		game->OnKeyDown(sys, key);

		event_t new_event;
		new_event.event = EVENT_KEYDOWN;
		new_event.data = key;
		new_event.offset = frame_offset;
		events.push_back(new_event);

		frame_offset = 0;
	}
}

void SPRecorder::OnKeyUp(System& sys, gamekey_t key)
{
	if (key < NUM_KEYS) {
		game->OnKeyUp(sys, key);

		event_t new_event;
		new_event.event = EVENT_KEYUP;
		new_event.data = key;
		new_event.offset = frame_offset;
		events.push_back(new_event);

		frame_offset = 0;
	}
}
