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
#ifndef tet_incs
#define tet_incs

#ifdef _DEBUG
#undef NDEBUG
#else
#define NDEBUG
#endif

// autoconf config.h
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <vector>
#include <map>
#include <assert.h>
#include <string.h>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <pcre.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_opengl.h>
#include <zlib.h>
#include <lua5.1/lua.hpp>
#ifdef _WIN32
#include <vfw.h>// AVIFile
#endif
//#include <SDL_net.h>

#include "../tetlib/tetlib.hpp"

#include "jsh_vector.h"
#include "jsh_color.h"
#include "tet_defs.h"
#include "tet_types.h"

#include "utility.h"
#include "regex.h"
#include "archive.h"
#include "rng.h"

#include "settings.h"
#include "texture.h"
#include "script.h"
#include "particles.h"
#include "sound.h"
//#include "randomizer.h"
//#include "randtap.h"
//#include "randtgm.h"
//#include "randbag.h"
//#include "randsequence.h"
#include "block.h"
#include "placement.h"
#include "tileset.h"
#include "font.h"
#include "well.h"
#include "virtualwell.h"
#include "state.h"
#include "metacube.h"
//#include "mpserver.h"
//#include "mpclient.h"
#include "game.h"
#include "mpgame.h"
#include "spdeath.h"
#include "spclassic.h"
#include "spmaster.h"
#include "spmroll.h"
#include "spsprint.h"
#include "spradar.h"
#include "aidemo.h"
#include "speasy.h"
#include "sprecorder.h"
#include "spplayback.h"
#include "graphics.h"
#include "system.h"

#endif
