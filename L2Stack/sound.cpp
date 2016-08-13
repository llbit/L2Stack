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

void mixer(void *context, Uint8 *stream, int len)
{
	SoundSystem	*soundsys = (SoundSystem *)context;
	soundsys->Mix(stream, len);
}


void mix8(Uint8 *stream, Uint8 *source, int len)
{
	for (int i = 0; i < len; ++i) {
		int	sum = (Sint8)source[i] + (Sint8)stream[i];
		if (sum > SCHAR_MAX) {
			stream[i] = SCHAR_MAX;
		} else if (sum < SCHAR_MIN) {
			stream[i] = SCHAR_MIN;
		} else {
			stream[i] = (Uint8)sum;
		}
	}
}

void mix16(Uint8 *stream, Uint8 *source, int len)
{
	for (int i = 0; i < len; i += 2) {
		int	sum = *((Sint16 *)&source[i]) + *((Sint16 *)&stream[i]);
		if (sum > SHRT_MAX) {
			*((Sint16 *)&stream[i]) = SHRT_MAX;
		} else if (sum < SHRT_MIN) {
			*((Sint16 *)&stream[i]) = SHRT_MIN;
		} else {
			*((Sint16 *)&stream[i]) = (Sint16)sum;
		}
	}
}

SoundSystem::SoundSystem()
{

}

SoundSystem::~SoundSystem()
{
	TearDown();
}

void SoundSystem::SetUp()
{
	SDL_AudioSpec	desired;
	desired.freq = 44100;
	desired.format = AUDIO_S16;
	desired.samples = desired.freq / 30;// only half frequency here to avoid starvation
	desired.channels = 1;
	desired.callback = mixer;
	desired.userdata = (void *)this;

	if (SDL_OpenAudio(&desired, &hwfmt) < 0) {
		std::cerr << "Failed to open audio device.\n";
		throw "fail";
	}

	std::cerr << "Opened sound device: " << hwfmt.freq << " Hz (" <<
		(int)hwfmt.channels << " channels)\n";

	SDL_PauseAudio(0);
}

void SoundSystem::TearDown()
{
	StopAll();
	SDL_CloseAudio();

	/*SDL_LockAudio();
	for (size_t i = 0; i < sounds.size(); ++i) {
		sample_t	&sound(sounds[i].second);
		if (sound.buff != NULL) {
			SDL_FreeWAV(sound.buff);
			sound.buff = NULL;
			sound.pos = sound.len = 0;
		}
	}
	sounds.clear();
	SDL_UnlockAudio();*/
}

int SoundSystem::LoadSound(Archive &big, const string &fn)
{
	int	i;
	for (i = 0; i < (int)sounds.size(); ++i) {
		if (sounds[i].first == fn) {
			return i;
		}
	}
	
	try {
		SDL_AudioSpec	wavfmt;
		Uint8		*wavbuff;
		Uint32		wavlen;
		void		*rwbuff;
		u32		rwsize;
		SDL_RWops	*rwsource;
		SDL_AudioCVT	cvt;

		big.ReadItem(fn, &rwbuff, &rwsize);
		rwsource = SDL_RWFromConstMem((void *)rwbuff, (int)rwsize);
		SDL_LoadWAV_RW(rwsource, 1, &wavfmt, &wavbuff, &wavlen);

		// Convert the audio data
		int res = SDL_BuildAudioCVT(&cvt, wavfmt.format, wavfmt.channels, wavfmt.freq,
			hwfmt.format, hwfmt.channels, hwfmt.freq);
		if (res < 0) {
			SDL_FreeWAV(wavbuff);
			std::cerr << "Could not convert audio for " << fn << ", reason: " << SDL_GetError() << "\n";
			throw "fail";
		}
		cvt.buf = new Uint8[wavlen * cvt.len_mult];
		memcpy(cvt.buf, wavbuff, wavlen);
		cvt.len = wavlen;
		SDL_ConvertAudio(&cvt);
		SDL_FreeWAV(wavbuff);

		SDL_LockAudio();
		sample_t	sound;
		sound.buff = cvt.buf;
		sound.len = cvt.len_cvt;
		sounds.push_back(pair<string, sample_t>(fn, sound));
		SDL_UnlockAudio();
	} catch (...) {
		SDL_LockAudio();
		sample_t	sound;
		sound.buff = NULL;
		sound.len = 0;
		sounds.push_back(pair<string, sample_t>(fn, sound));
		SDL_UnlockAudio();
	}
	return i;
}

void SoundSystem::PlaySound(const string &fn)
{
	for (int i = 0; i < (int)sounds.size(); ++i) {
		if (sounds[i].first == fn) {
			PlaySound(i);
			return;
		}
	}
	std::cerr << "Sound not preloaded: " << fn << "\n";
}

void SoundSystem::PlaySound(int i)
{	
	if (i < (int)sounds.size() && sounds[i].second.buff != NULL) {
		SDL_LockAudio();
		playlist.push_back(cursor_t());
		playlist.back().sample = i;
		playlist.back().pos = 0;
		SDL_UnlockAudio();
	}
}

void SoundSystem::StopAll()
{
	SDL_LockAudio();
	playlist.clear();
	SDL_UnlockAudio();
}

void SoundSystem::Mix(Uint8 *stream, int len)
{
	Uint32 amount;
	void (*mix_f)(Uint8 *, Uint8 *, int);

	if ((hwfmt.format & 0xFF) > 8) {
		mix_f = mix16;
	} else {
		mix_f = mix8;
	}

	SDL_LockAudio();
	list<cursor_t>::iterator	iter = playlist.begin();
	while (iter != playlist.end()) {
		cursor_t	&cursor(*iter);
		sample_t	&sound(sounds[cursor.sample].second);

		amount = (sound.len - cursor.pos);
		if (amount == 0) {
			iter = playlist.erase(iter);
			continue;
		} else if (amount > (unsigned)len) {
			amount = len;
		}
		mix_f(stream, &sound.buff[cursor.pos], amount);
		cursor.pos += amount;
		++iter;
	}
	SDL_UnlockAudio();
}
