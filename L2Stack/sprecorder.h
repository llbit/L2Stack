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
#ifndef tet_sprecorder
#define tet_sprecorder

class SPRecorder : public State {
	Game		*game;
	string	mode;
	list<event_t>	events;
	Uint16		frame_offset;
	Uint32		frame;
	Uint32		max_frames;

	void SetUp(System &);
	void TearDown();

public:
	SPRecorder(System &, const string& mode);
	~SPRecorder();

	void SaveReplay(System &, const string &);

	void Update(System &);
	void Draw(Graphics &);
	void PostRender(Graphics &) const;
	void Restart(System &);
	Game *GetGame() {return game;}

	void OnKeyDown(System &, gamekey_t);
	void OnKeyUp(System &, gamekey_t);
};

#endif
