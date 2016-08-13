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
#ifndef tetris_sound
#define tetris_sound

#define NUM_SOUNDS 2

class SoundSystem
{
	struct sample_t {
		Uint8	*buff;
		Uint32	len;
	};
	struct cursor_t {
		int	sample;
		Uint32	pos;
	};

	SDL_AudioSpec			hwfmt;
	vector<pair<string, sample_t> >	sounds;
	list<cursor_t>			playlist;

public:
	SoundSystem();
	~SoundSystem();

	void SetUp();
	void TearDown();
	
	int LoadSound(Archive &big, const string &fn);
	void PlaySound(const string &sd);
	void PlaySound(int sd);
	SDL_AudioSpec Fmt() {return hwfmt;}

	void StopAll();
	void Mix(Uint8 *stream, int len);
};

#endif
