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

int l2sl_atpanic(lua_State *L);
int l2sl_traceback(lua_State *L);
void l2sl_require(lua_State *L, int num);
void l2sl_expectuserdata(lua_State *L, int idx);
void l2sl_expecttable(lua_State *L, int idx);
void l2sl_expectnumber(lua_State *L, int idx);
void l2sl_expectstring(lua_State *L, int idx);

// PCRE functions
int re_test(lua_State *L);
int re_match(lua_State *L);

// System metamethods
int system_NewGame(lua_State *L);
int system_LoadReplay(lua_State *L);
int system_LoadScript(lua_State *L);
int system_LoadSettings(lua_State *L);
int system_SaveSettings(lua_State *L);
int system_WriteToLog(lua_State *L);
int system_GetTime(lua_State *L);
int system_GetKeyName(lua_State *L);
int system_LoadTexture(lua_State *L);
int system_LoadFont(lua_State *L);
int system_LoadSound(lua_State *L);
int system_DrawMetacube(lua_State *L);
int system_UpdateMetacube(lua_State *L);
int system_MetaMorph(lua_State *L);
int system_GetFileList(lua_State *L);
int system_PathExists(lua_State *L);
int system_CreateDirectory(lua_State *L);
int system_ListDisplayModes(lua_State *L);
int system_Exit(lua_State *L);
int system_AviOpen(lua_State *L);

// Object metamethods
int avi_WriteFrame(lua_State *L);
int avi_Close(lua_State *L);
int texture_Draw(lua_State *L);
int texture_DrawTiled(lua_State *L);
int texture_DrawRegion(lua_State *L);
int font_Width(lua_State *L);
int font_Draw(lua_State *L);
int font_DrawRegion(lua_State *L);
int sound_Play(lua_State *L);

int game_OnKeyDown(lua_State *L);
int game_OnKeyUp(lua_State *L);
int game_Update(lua_State *L);
int game_Draw(lua_State *L);
int game_PostRender(lua_State *L);
int game_Restart(lua_State *L);
int game_End(lua_State *L);
int game_SaveReplay(lua_State *L);
int game_AddOpponent(lua_State *L);
int game_AddGarbage(lua_State *L);
int game_GetNumTets(lua_State *L);


int gl_Color3(lua_State *L);
int gl_Color4(lua_State *L);
int gl_PushMatrix(lua_State *L);
int gl_PopMatrix(lua_State *L);
int gl_Rotate(lua_State *L);
int gl_Scale(lua_State *L);
int gl_Translate(lua_State *L);
int gl_BeginQuads(lua_State *L);
int gl_End(lua_State *L);
int gl_Vertex3(lua_State *L);
int gl_EnableClipPlane(lua_State *L);
int gl_DisableClipPlane(lua_State *L);
int gl_ClipPlane(lua_State *L);

#define l2slua_regSysFunc(f) \
	(lua_pushliteral(L, #f), \
	lua_pushcfunction(L, &system_##f), \
	lua_settable(L, -3))
#define l2slua_regGlFunc(f) \
	(lua_pushliteral(L, #f), \
	lua_pushcfunction(L, &gl_##f), \
	lua_settable(L, -3))

Script::Script()
{
	L = lua_open();
}

Script::~Script()
{
	lua_close(L);
}

void Script::SetUp(System &sys)
{
	lua_atpanic(L, &l2sl_atpanic);
	luaopen_base(L);
	luaopen_math(L);
	luaopen_string(L);

	// PCRE functions
	lua_pushliteral(L, "re_test");
	lua_pushcfunction(L, &re_test);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "re_match");
	lua_pushcfunction(L, &re_match);
	lua_settable(L, LUA_GLOBALSINDEX);

	// Build system table:
	void	*userdata = lua_newuserdata(L, sizeof(System *));
	*(System **)userdata = &sys;
	int	system = lua_gettop(L);
	lua_newtable(L);
	int	metatable = lua_gettop(L);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, metatable);
	lua_settable(L, metatable);
	lua_pushvalue(L, metatable);
	lua_setmetatable(L, system);
	lua_pushliteral(L, "system");
	lua_pushvalue(L, system);
	lua_settable(L, LUA_GLOBALSINDEX);

	// Register system metamethods
	l2slua_regSysFunc(NewGame);
	l2slua_regSysFunc(LoadReplay);
	l2slua_regSysFunc(LoadScript);
	l2slua_regSysFunc(LoadSettings);
	l2slua_regSysFunc(SaveSettings);
	l2slua_regSysFunc(WriteToLog);
	l2slua_regSysFunc(GetTime);
	l2slua_regSysFunc(GetKeyName);
	l2slua_regSysFunc(LoadTexture);
	l2slua_regSysFunc(LoadFont);
	l2slua_regSysFunc(LoadSound);
	l2slua_regSysFunc(DrawMetacube);
	l2slua_regSysFunc(UpdateMetacube);
	l2slua_regSysFunc(MetaMorph);
	l2slua_regSysFunc(GetFileList);
	l2slua_regSysFunc(PathExists);
	l2slua_regSysFunc(CreateDirectory);
	l2slua_regSysFunc(AviOpen);
	l2slua_regSysFunc(ListDisplayModes);
	l2slua_regSysFunc(Exit);

	// Register game keys
	lua_pushliteral(L, "GAMEKEY_HARD");
	lua_pushinteger(L, KEY_HARD);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "GAMEKEY_SOFT");
	lua_pushinteger(L, KEY_SOFT);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "GAMEKEY_LEFT");
	lua_pushinteger(L, KEY_LEFT);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "GAMEKEY_RIGHT");
	lua_pushinteger(L, KEY_RIGHT);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "GAMEKEY_AROT");
	lua_pushinteger(L, KEY_AROT);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "GAMEKEY_BROT");
	lua_pushinteger(L, KEY_BROT);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "GAMEKEY_CROT");
	lua_pushinteger(L, KEY_CROT);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "GAMEKEY_HOLD");
	lua_pushinteger(L, KEY_HOLD);
	lua_settable(L, LUA_GLOBALSINDEX);

	// Register Key constants
	lua_pushliteral(L, "KEY_UP");
	lua_pushinteger(L, DEV_KEYBOARD << 8 | SDLK_UP);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "KEY_DOWN");
	lua_pushinteger(L, DEV_KEYBOARD << 8 | SDLK_DOWN);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "KEY_LEFT");
	lua_pushinteger(L, DEV_KEYBOARD << 8 | SDLK_LEFT);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "KEY_RIGHT");
	lua_pushinteger(L, DEV_KEYBOARD << 8 | SDLK_RIGHT);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "KEY_RETURN");
	lua_pushinteger(L, DEV_KEYBOARD << 8 | SDLK_RETURN);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "KEY_TAB");
	lua_pushinteger(L, DEV_KEYBOARD << 8 | SDLK_TAB);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "KEY_ESCAPE");
	lua_pushinteger(L, DEV_KEYBOARD << 8 | SDLK_ESCAPE);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "KEY_BACKSPACE");
	lua_pushinteger(L, DEV_KEYBOARD << 8 | SDLK_BACKSPACE);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "MOUSEBUTTON1");
	lua_pushinteger(L, DEV_MOUSE << 8 | 1);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "MOUSEBUTTON2");
	lua_pushinteger(L, DEV_MOUSE << 8 | 2);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "MOUSEWHEELUP");
	lua_pushinteger(L, DEV_MOUSE << 8 | 4);
	lua_settable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "MOUSEWHEELDOWN");
	lua_pushinteger(L, DEV_MOUSE << 8 | 5);
	lua_settable(L, LUA_GLOBALSINDEX);

	// Register OpenGL functions
	lua_newtable(L);
	int	gl = lua_gettop(L);
	l2slua_regGlFunc(Color3);
	l2slua_regGlFunc(Color4);
	l2slua_regGlFunc(PushMatrix);
	l2slua_regGlFunc(PopMatrix);
	l2slua_regGlFunc(Rotate);
	l2slua_regGlFunc(Scale);
	l2slua_regGlFunc(Translate);
	l2slua_regGlFunc(BeginQuads);
	l2slua_regGlFunc(End);
	l2slua_regGlFunc(Vertex3);
	l2slua_regGlFunc(EnableClipPlane);
	l2slua_regGlFunc(DisableClipPlane);
	l2slua_regGlFunc(ClipPlane);
	lua_pushliteral(L, "gl");
	lua_pushvalue(L, gl);
	lua_settable(L, LUA_GLOBALSINDEX);
}

void Script::LoadScript(Archive &archive, const string &fn)
{
	char*	data;
	u32	size;

	archive.ReadItem(fn, (void**)&data, &size);
	std::cerr << "Loading script: " << fn << "\n";
	LoadChunk(data, size, fn);
	delete[] data;
}

void Script::LoadScript(const string &s, const string &n)
{
	LoadChunk(s.c_str(), s.size(), n);
}

void Script::LoadChunk(const char *chunk, int size, const string &n)
{
	int	traceback;
	lua_pushcfunction(L, &l2sl_traceback);
	traceback = lua_gettop(L);
	if (luaL_loadbuffer(L, (char*)chunk, size, n.c_str())) {
		std::cerr << "Lua error while loading file " << n;
		std::cerr << ": " << lua_tostring(L, -1) << "\n";
		return;
	}
	if (lua_pcall(L, 0, LUA_MULTRET, traceback)) {
		std::cerr << "Lua Error: \n" << lua_tostring(L, -1) << "\n";
		OnError(string("Lua Error: ") + string(lua_tostring(L, -1)));
	}
}

void Script::SetStateInt(const string &n, int v)
{
	lua_pushliteral(L, "state");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, n.c_str());
	lua_pushnumber(L, (lua_Number)v);
	lua_settable(L, -3);
	lua_pop(L, -1);
}

void Script::SetStateFloat(const string &n, float v)
{
	lua_pushliteral(L, "state");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, n.c_str());
	lua_pushnumber(L, (lua_Number)v);
	lua_settable(L, -3);
	lua_pop(L, -1);
}

void Script::SetStateString(const string &n, const string &v)
{
	lua_pushliteral(L, "state");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, n.c_str());
	lua_pushstring(L, v.c_str());
	lua_settable(L, -3);
	lua_pop(L, -1);
}

void Script::SetStateBool(const string &n, int v)
{
	lua_pushliteral(L, "state");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, n.c_str());
	lua_pushboolean(L, v);
	lua_settable(L, -3);
	lua_pop(L, -1);
}

void Script::SetStateNil(const string &n)
{
	lua_pushliteral(L, "state");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, n.c_str());
	lua_pushnil(L);
	lua_settable(L, -3);
	lua_pop(L, -1);
}

int Script::GetStateInt(const string &n)
 {
	int	result;
	lua_pushliteral(L, "state");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, n.c_str());
	lua_gettable(L, -2);
	/*if (!lua_isnumber(L, -1))
 		result = 0;
	else*/
		result = (int)lua_tonumber(L, -1);
	lua_pop(L, -2);
	return result;
}

string Script::GetStateString(const string &n)
{
	string	result;
	lua_pushliteral(L, "state");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, n.c_str());
	lua_gettable(L, -2);
	/*if (!lua_isnumber(L, -1))
		result = string("");
	else*/
		result = string(lua_tostring(L, -1));
	lua_pop(L, -2);
	return result;
}

bool Script::GetStateBool(const string &n)
{
	bool	result;
	lua_pushliteral(L, "state");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, n.c_str());
	lua_gettable(L, -2);
	/*if (!lua_isboolean(L, -1))
		result = false;
	else*/
		result = lua_toboolean(L, -1) > 0;
	lua_pop(L, -2);
	return result;
}

int Script::ParseInt(const string& expr)
{
	string chunk = "return "+expr;
	LoadChunk(chunk.c_str(), chunk.length(), chunk);

	if (!lua_isnumber(L, -1)) {
		ostringstream	ostr;
		ostr << "Expected number, but received "
			<< lua_typename(L, lua_type(L, -1))
			<< " (" << expr << ")";
		lua_pushstring(L, ostr.str().c_str());
		lua_error(L);
	}

	return lua_tonumber(L, -1);
}

string Script::ParseString(const string& expr)
{
	string chunk = "return "+expr;
	LoadChunk(chunk.c_str(), chunk.length(), chunk);

	if (!lua_isstring(L, -1)) {
		ostringstream	ostr;
		ostr << "Expected string, but received "
			<< lua_typename(L, lua_type(L, -1));
		lua_pushstring(L, ostr.str().c_str());
		lua_error(L);
	}

	return string(lua_tostring(L, -1));
}

void Script::Update(unsigned int millis)
{
	int	gui, error_handler;
	lua_pushcfunction(L, &l2sl_traceback);
	error_handler = lua_gettop(L);
	lua_pushliteral(L, "GUI");
	lua_gettable(L, LUA_GLOBALSINDEX);
	gui = lua_gettop(L);
	lua_pushliteral(L, "Update");
	lua_gettable(L, gui);
	lua_pushvalue(L, gui);
	lua_pushnumber(L, (lua_Number)millis);
	if (lua_pcall(L, 2, 0, error_handler)) {
		OnError(string("Lua error: ") + string(lua_tostring(L, -1)));
	}
	lua_pop(L, 2);
}

void Script::Draw()
{
	lua_pushcfunction(L, &l2sl_traceback);
	lua_pushliteral(L, "GUI");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "Draw");
	lua_gettable(L, -2);
	lua_pushliteral(L, "GUI");
	lua_gettable(L, LUA_GLOBALSINDEX);
	if (lua_pcall(L, 1, 0, -4)) {
		OnError(string("Lua error: ") + string(lua_tostring(L, -1)));
	}
	lua_pop(L, 2);
}

void Script::OnKeyDown(devkey_t key)
{
	lua_pushcfunction(L, &l2sl_traceback);
	lua_pushliteral(L, "GUI");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "OnKeyDown");
	lua_gettable(L, -2);
	lua_pushliteral(L, "GUI");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushinteger(L, key);
	if (lua_pcall(L, 2, 0, -5)) {
		OnError(string("Lua error: ") + string(lua_tostring(L, -1)));
	}
	lua_pop(L, 2);
}

void Script::OnKeyUp(devkey_t key)
{
	int error_handler;
	lua_pushcfunction(L, &l2sl_traceback);
	error_handler = lua_gettop(L);
	lua_pushliteral(L, "GUI");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "OnKeyUp");
	lua_gettable(L, -2);
	lua_pushliteral(L, "GUI");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushinteger(L, key);
	if (lua_pcall(L, 2, 0, error_handler)) {
		OnError(string("Lua error: ") + string(lua_tostring(L, -1)));
	}
	lua_pop(L, 2);
}

void Script::OnText(Uint16 ch)
{
	string	chs = boost::lexical_cast<string>((char)(ch & 0xFF));
	lua_pushcfunction(L, &l2sl_traceback);
	int	error_handler = lua_gettop(L);
	lua_pushliteral(L, "GUI");
	lua_gettable(L, LUA_GLOBALSINDEX);
	int	gui = lua_gettop(L);
	lua_pushliteral(L, "OnText");
	lua_gettable(L, gui);
	lua_pushvalue(L, gui);
	lua_pushstring(L, chs.c_str());
	if (lua_pcall(L, 2, 0, error_handler)) {
		OnError(string("Lua error: ") + string(lua_tostring(L, -1)));
	}
	lua_pop(L, 2);
}

void Script::OnMouseMotion(int x, int y)
{
	int error_handler;
	lua_pushcfunction(L, &l2sl_traceback);
	error_handler = lua_gettop(L);
	lua_pushliteral(L, "GUI");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "OnMouseMotion");
	lua_gettable(L, -2);
	lua_pushliteral(L, "GUI");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushinteger(L, (lua_Integer)x);
	lua_pushinteger(L, (lua_Integer)y);
	if (lua_pcall(L, 3, 0, error_handler)) {
		OnError(string("Lua error: ") + string(lua_tostring(L, -1)));
	}
	lua_pop(L, 2);
}


void Script::OnError(const string &err)
{
	lua_pushcfunction(L, &l2sl_traceback);
	lua_pushliteral(L, "GUI");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushliteral(L, "OnError");
	lua_gettable(L, -2);
	lua_pushliteral(L, "GUI");
	lua_gettable(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, err.c_str());
	if (lua_pcall(L, 2, 0, -5)) {
		std::cerr << "Lua error while calling error handler:\n" << string(lua_tostring(L, -1));
		throw "fail";
	}
	lua_pop(L, 2);
}

int l2sl_atpanic(lua_State *L)
{
	l2sl_traceback(L);
	return 1;
}

int l2sl_traceback(lua_State *L)
{
	ostringstream	ostr;
	string		str;
	const char	*msg;

	assert(lua_isstring(L, -1));

	msg = lua_tostring(L, -1);

	ostr << msg << '\n';

//#ifndef NDEBUG
	lua_Debug	ar;
	for (int i = 2; true; ++i) {
		if (!lua_getstack(L, i, &ar)) {
			break;
		}
		if (!lua_getinfo(L, "Sl", &ar)) {
			break;
		}
		ostr << "called from " << ar.short_src << ":" << ar.currentline << "\n";
	}
//#endif

	str = ostr.str();
	std::cerr << str;
	lua_pushstring(L, str.c_str());
	return 1;
}

void l2sl_require(lua_State *L, int num)
{
	int	top = lua_gettop(L);
	if (num > top) {
		ostringstream	ostr;
		ostr << "Too few arguments given. Function requires " << num << " arguments; "
			<< top << " were given.";
		lua_pushstring(L, ostr.str().c_str());
		lua_error(L);
	}
}

void l2sl_expectuserdata(lua_State *L, int idx)
{
	if (lua_isuserdata(L, idx) != 1) {
		ostringstream	ostr;
		ostr << "Invalid argument type for argument number " <<
			idx << ". Function expects userdata; " <<
			lua_typename(L, lua_type(L, idx)) << " was given.";
		lua_pushstring(L, ostr.str().c_str());
		lua_error(L);
	}
}

void l2sl_expecttable(lua_State *L, int idx)
{
	if (lua_istable(L, idx) != 1) {
		ostringstream	ostr;
		ostr << "Invalid argument type for argument number " <<
			idx << ". Function expects table; " <<
			lua_typename(L, lua_type(L, idx)) << " was given.";
		lua_pushstring(L, ostr.str().c_str());
		lua_error(L);
	}
}

void l2sl_expectnumber(lua_State *L, int idx)
{
	if (lua_isnumber(L, idx) != 1) {
		ostringstream	ostr;
		ostr << "Invalid argument type for argument number " <<
			idx << ". Function expects number; " <<
			lua_typename(L, lua_type(L, idx)) << " was given.";
		lua_pushstring(L, ostr.str().c_str());
		lua_error(L);
	}
}

void l2sl_expectstring(lua_State *L, int idx)
{
	if (lua_isstring(L, idx) != 1) {
		ostringstream	ostr;
		ostr << "Invalid argument type for argument number " <<
			idx << ". Function expects string; " <<
			lua_typename(L, lua_type(L, idx)) << " was given.";
		lua_pushstring(L, ostr.str().c_str());
		lua_error(L);
	}
}

int re_test(lua_State *L)
{
	const char	*pattern, *subject;

	l2sl_require(L, 2);
	l2sl_expectstring(L, 1);
	l2sl_expectstring(L, 2);

	pattern = lua_tostring(L, 1);
	subject = lua_tostring(L, 2);
	Regex	regex(pattern);
	lua_pushboolean(L, regex.test(subject) ? true : false);
	return 1;
}

int re_match(lua_State *L)
{
	const char	*pattern, *subject;

	l2sl_require(L, 2);
	l2sl_expectstring(L, 1);
	l2sl_expectstring(L, 2);

	pattern = lua_tostring(L, 1);
	subject = lua_tostring(L, 2);
	lua_newtable(L);
	int		tbl = lua_gettop(L);
	Regex		regex(pattern);
	vector<string>	result = regex.match(subject);
	for (size_t i = 0; i < result.size(); ++i) {
		lua_pushnumber(L, (lua_Number)i);
		lua_pushstring(L, result[i].c_str());
		lua_settable(L, tbl);
	}
	return 1;
}

int system_LoadScript(lua_State *L)
{
	System		*sys;
	const char	*fn;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);

	sys = *(System **)lua_touserdata(L, 1);
	fn = lua_tostring(L, 2);
	sys->GetScript().LoadScript(sys->GetArchive(), fn);
	return 0;
}

int system_NewGame(lua_State *L)
{
	System		*sys;
	const char	*mode;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);

	sys = *(System **)lua_touserdata(L, 1);
	mode = lua_tostring(L, 2);
	State	*new_state = new SPRecorder(*sys, mode);
	void	*userdata = lua_newuserdata(L, sizeof(new_state));
	*(State **)userdata = new_state;
	int	obj = lua_gettop(L);
	lua_newtable(L);
	int	metatable = lua_gettop(L);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, metatable);
	lua_settable(L, metatable);
	lua_pushliteral(L, "mode");
	lua_pushstring(L, mode);
	lua_settable(L, metatable);
	lua_pushliteral(L, "OnKeyDown");
	lua_pushcfunction(L, &game_OnKeyDown);
	lua_settable(L, metatable);
	lua_pushliteral(L, "OnKeyUp");
	lua_pushcfunction(L, &game_OnKeyUp);
	lua_settable(L, metatable);
	lua_pushliteral(L, "Update");
	lua_pushcfunction(L, &game_Update);
	lua_settable(L, metatable);
	lua_pushliteral(L, "Draw");
	lua_pushcfunction(L, &game_Draw);
	lua_settable(L, metatable);
	lua_pushliteral(L, "PostRender");
	lua_pushcfunction(L, &game_PostRender);
	lua_settable(L, metatable);
	lua_pushliteral(L, "SaveReplay");
	lua_pushcfunction(L, &game_SaveReplay);
	lua_settable(L, metatable);
	lua_pushliteral(L, "AddOpponent");
	lua_pushcfunction(L, &game_AddOpponent);
	lua_settable(L, metatable);
	lua_pushliteral(L, "AddGarbage");
	lua_pushcfunction(L, &game_AddGarbage);
	lua_settable(L, metatable);
	lua_pushliteral(L, "GetNumTets");
	lua_pushcfunction(L, &game_GetNumTets);
	lua_settable(L, metatable);
	lua_pushliteral(L, "Restart");
	lua_pushcfunction(L, &game_Restart);
	lua_settable(L, metatable);
	lua_pushliteral(L, "End");
	lua_pushcfunction(L, &game_End);
	lua_settable(L, metatable);
	lua_setmetatable(L, obj);
	return 1;
}

int system_LoadReplay(lua_State *L)
{
	System		*sys;
	const char	*fn;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);

	sys = *(System **)lua_touserdata(L, 1);
	fn = lua_tostring(L, 2);
	State	*new_state;
	try {
		new_state = new SPPlayback(*sys, fn);
	} catch (...) {
		sys->GetScript().OnError("Could not open the replay file.");
		lua_pushnil(L);
		return 1;
	}
	void	*userdata = lua_newuserdata(L, sizeof(new_state));
	*(State **)userdata = new_state;
	int	obj = lua_gettop(L);
	lua_newtable(L);
	int	metatable = lua_gettop(L);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, metatable);
	lua_settable(L, metatable);
	lua_pushliteral(L, "mode");
	lua_pushstring(L, ((SPPlayback *)new_state)->GetMode().c_str());
	lua_settable(L, metatable);
	lua_pushliteral(L, "Update");
	lua_pushcfunction(L, &game_Update);
	lua_settable(L, metatable);
	lua_pushliteral(L, "Draw");
	lua_pushcfunction(L, &game_Draw);
	lua_settable(L, metatable);
	lua_pushliteral(L, "PostRender");
	lua_pushcfunction(L, &game_PostRender);
	lua_settable(L, metatable);
	lua_pushliteral(L, "Restart");
	lua_pushcfunction(L, &game_Restart);
	lua_settable(L, metatable);
	lua_pushliteral(L, "End");
	lua_pushcfunction(L, &game_End);
	lua_settable(L, metatable);
	lua_setmetatable(L, obj);
	return 1;
}

int game_OnKeyDown(lua_State *L)
{
	System		*sys;
	State		*state;
	gamekey_t	key;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectnumber(L, 2);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	state = *(State **)lua_touserdata(L, 1);
	key = (gamekey_t)lua_tointeger(L, 2);
	state->OnKeyDown(*sys, key);
	return 0;
}

int game_OnKeyUp(lua_State *L)
{
	System		*sys;
	State		*state;
	gamekey_t	key;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectnumber(L, 2);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	state = *(State **)lua_touserdata(L, 1);
	key = (gamekey_t)lua_tointeger(L, 2);
	state->OnKeyUp(*sys, key);
	return 0;
}

int game_Update(lua_State *L)
{
	System	*sys;
	State	*state;

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	state = *(State **)lua_touserdata(L, 1);
	state->Update(*sys);
	return 0;
}

int game_Draw(lua_State *L)
{
	System	*sys;
	State	*state;

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	state = *(State **)lua_touserdata(L, 1);
	state->Draw(sys->GetGraphics());
	return 0;
}

int game_PostRender(lua_State *L)
{
	System	*sys;
	State	*state;

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	state = *(State **)lua_touserdata(L, 1);
	state->PostRender(sys->GetGraphics());
	return 0;
}

int game_Restart(lua_State *L)
{
	System	*sys;
	State	*state;

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	state = *(State **)lua_touserdata(L, 1);
	state->Restart(*sys);
	return 0;
}

int game_End(lua_State *L)
{
	System	*sys;
	State	*state;

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	state = *(State **)lua_touserdata(L, 1);
	delete state;
	return 0;
}

int game_SaveReplay(lua_State *L)
{
	System		*sys;
	SPRecorder	*recorder;
	const char	*fn;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	recorder = *(SPRecorder **)lua_touserdata(L, 1);
	fn = lua_tostring(L, 2);
	recorder->SaveReplay(*sys, fn);
	return 0;
}

int game_AddOpponent(lua_State *L)
{
	SPRecorder	*self, *opponent;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectuserdata(L, 2);

	self = *(SPRecorder **)lua_touserdata(L, 1);
	opponent = *(SPRecorder **)lua_touserdata(L, 2);
	self->GetGame()->AddOpponent(opponent->GetGame());
	return 0;
}

int game_AddGarbage(lua_State *L)
{
	System		*sys;
	SPRecorder	*self;
	Game		*game;
	int		nlines;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectnumber(L, 2);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	self = *(SPRecorder **)lua_touserdata(L, 1);
	nlines = (int)lua_tonumber(L, 2);
	game = self->GetGame();
	for (int i = 0; i < nlines; ++i) {
		game->AddGarbageLine(*sys, sys->GetRNG().GetNext()%10);
	}
	return 0;
}

int game_GetNumTets(lua_State *L)
{
	SPRecorder	*self;

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);

	self = *(SPRecorder **)lua_touserdata(L, 1);
	lua_pushnumber(L, (lua_Number) self->GetGame()->GetNumTets());
	return 1;
}

int system_LoadSettings(lua_State *L)
{
	System*		sys;
	const char*	fn;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);

	sys = *(System**)lua_touserdata(L, 1);
	fn = lua_tostring(L, 2);

	settings_Load(sys, fn, L);
	return 1;
}

int system_SaveSettings(lua_State *L)
{
	System*		sys;

	l2sl_require(L, 3);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);
	l2sl_expecttable(L, 3);

	sys = *(System**)lua_touserdata(L, 1);
	string fn(lua_tostring(L, 2));

	if (!boost::filesystem::exists("profiles"))
		boost::filesystem::create_directory("profiles");
	fn = "profiles/"+fn;
	fn += ".ini";

	lua_pushvalue(L, 3);/* make sure the settings table
			       is on the top of the stack
			       */
	settings_Save(sys, fn, L);
	lua_pop(L, 1);
	return 0;
}

int system_WriteToLog(lua_State *L)
{
	const char	*text;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);

	text = lua_tostring(L, 2);
	std::cerr << text << "\n";
	return 0;
}

int system_GetTime(lua_State *L)
{
	Uint32 ticks = SDL_GetTicks();

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);

	lua_pushnumber(L, (lua_Number)ticks);
	return 1;
}

int system_GetKeyName(lua_State *L)
{
	devkey_t	key;
	string		name;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectnumber(L, 2);

	key = (devkey_t)lua_tonumber(L, 2);
	switch (key >> 8) {
		case DEV_KEYBOARD:
			name = string(SDL_GetKeyName((SDLKey)(key & 0xFF)));
			break;
		case DEV_MOUSE:
			name = string("mouse_") + boost::lexical_cast<string>(key & 0xFF);
			break;
		case DEV_JOY1:
			name = string("joy1_") + boost::lexical_cast<string>(key & 0xFF);
			break;
		case DEV_JOY2:
			name = string("joy2_") + boost::lexical_cast<string>(key & 0xFF);
			break;
		case DEV_JOY3:
			name = string("joy3_") + boost::lexical_cast<string>(key & 0xFF);
			break;
		case DEV_JOY4:
			name = string("joy4_") + boost::lexical_cast<string>(key & 0xFF);
			break;
		default:
			name = string("unknown key");
	}

	lua_pushstring(L, name.c_str());
	return 1;
}

int system_LoadTexture(lua_State *L)
{
	System		*sys;
	const char	*fn;
	int		tx, obj, metatable;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);

	sys = *(System **)lua_touserdata(L, 1);
	fn = lua_tostring(L, 2);
	tx = sys->GetGraphics().LoadTexture(sys->GetArchive(), string(fn));
	void	*userdata = lua_newuserdata(L, sizeof(tx));
	*(int *)userdata = tx;
	obj = lua_gettop(L);
	lua_newtable(L);
	metatable = lua_gettop(L);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, metatable);
	lua_settable(L, metatable);
	lua_pushliteral(L, "height");
	lua_pushnumber(L, (lua_Number)sys->GetGraphics().TextureHeight(tx));
	lua_settable(L, metatable);
	lua_pushliteral(L, "width");
	lua_pushnumber(L, (lua_Number)sys->GetGraphics().TextureWidth(tx));
	lua_settable(L, metatable);
	lua_pushliteral(L, "Draw");
	lua_pushcfunction(L, &texture_Draw);
	lua_settable(L, metatable);
	lua_pushliteral(L, "DrawTiled");
	lua_pushcfunction(L, &texture_DrawTiled);
	lua_settable(L, metatable);
	lua_pushliteral(L, "DrawRegion");
	lua_pushcfunction(L, &texture_DrawRegion);
	lua_settable(L, metatable);
	lua_setmetatable(L, obj);
	return 1;
}

int texture_Draw(lua_State *L)
{
	int		tx;
	float		x, y, x1, y1;
	System		*sys;

	l2sl_require(L, 3);
	l2sl_expectuserdata(L, 1);
	l2sl_expectnumber(L, 2);
	l2sl_expectnumber(L, 3);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	tx = *(int *)lua_touserdata(L, 1);
	x = (float)lua_tonumber(L, 2);
	y = (float)lua_tonumber(L, 3);
	if (lua_isnumber(L, 4) && lua_isnumber(L, 5)) {
		x1 = (float)lua_tonumber(L, 4);
		y1 = (float)lua_tonumber(L, 5);
		sys->GetGraphics().DrawTexture(tx, x, y, x1, y1);
	} else {
		sys->GetGraphics().DrawTexture(tx, x, y);
	}
	return 0;
}

int texture_DrawTiled(lua_State *L)
{
	int		tx;
	float		x, y, w, h;
	System		*sys;

	l2sl_require(L, 5);
	l2sl_expectuserdata(L, 1);
	l2sl_expectnumber(L, 2);
	l2sl_expectnumber(L, 3);
	l2sl_expectnumber(L, 4);
	l2sl_expectnumber(L, 5);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);

	tx = *(int *)lua_touserdata(L, 1);
	x = (float)lua_tonumber(L, 2);
	y = (float)lua_tonumber(L, 3);
	w = (float)lua_tonumber(L, 4);
	h = (float)lua_tonumber(L, 5);
	
	sys->GetGraphics().DrawTextureTiled(tx, x, y, w, h);
	return 0;
}

int texture_DrawRegion(lua_State *L)
{
	int		tx;
	float		x, y, u0, v0, u1, v1;
	System		*sys;

	l2sl_require(L, 7);
	l2sl_expectuserdata(L, 1);
	l2sl_expectnumber(L, 2);
	l2sl_expectnumber(L, 3);
	l2sl_expectnumber(L, 4);
	l2sl_expectnumber(L, 5);
	l2sl_expectnumber(L, 6);
	l2sl_expectnumber(L, 7);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);

	tx = *(int *)lua_touserdata(L, 1);
	x = (float)lua_tonumber(L, 2);
	y = (float)lua_tonumber(L, 3);
	u0 = (float)lua_tonumber(L, 4);
	v0 = (float)lua_tonumber(L, 5);
	u1 = (float)lua_tonumber(L, 6);
	v1 = (float)lua_tonumber(L, 7);
	
	sys->GetGraphics().DrawTextureRegion(tx, x, y, u0, v0, u1, v1);
	return 0;
}

int system_LoadFont(lua_State *L)
{
	System		*sys;
	const char	*fn;
	int		ft, obj, metatable;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);

	sys = *(System **)lua_touserdata(L, 1);
	fn = lua_tostring(L, 2);
	ft = sys->GetGraphics().LoadFont(sys->GetArchive(), string(fn));
	void	*userdata = lua_newuserdata(L, sizeof(ft));
	*(int *)userdata = ft;
	obj = lua_gettop(L);
	lua_newtable(L);
	metatable = lua_gettop(L);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, metatable);
	lua_settable(L, metatable);
	lua_pushliteral(L, "height");
	lua_pushnumber(L, (lua_Number)sys->GetGraphics().TextHeight(ft));
	lua_settable(L, metatable);
	lua_pushliteral(L, "__mul");
	lua_pushcfunction(L, &font_Width);
	lua_settable(L, metatable);
	lua_pushliteral(L, "Draw");
	lua_pushcfunction(L, &font_Draw);
	lua_settable(L, metatable);
	lua_pushliteral(L, "DrawRegion");
	lua_pushcfunction(L, &font_DrawRegion);
	lua_settable(L, metatable);
	lua_setmetatable(L, obj);
	return 1;
}

int font_Width(lua_State *L)
{
	int		ft;
	const char	*text;
	System		*sys;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	ft = *(int *)lua_touserdata(L, 1);
	text = lua_tostring(L, 2);

	lua_pushnumber(L, (lua_Number)sys->GetGraphics().TextWidth(ft, text));
	return 1;
}

int font_Draw(lua_State *L)
{
	int		ft;
	const char	*text;
	float		x, y;
	System		*sys;

	l2sl_require(L, 4);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);
	l2sl_expectnumber(L, 3);
	l2sl_expectnumber(L, 4);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);

	ft = *(int *)lua_touserdata(L, 1);
	text = lua_tostring(L, 2);
	x = (float)lua_tonumber(L, 3);
	y = (float)lua_tonumber(L, 4);

	sys->GetGraphics().DrawText(ft, text, x, y);
	return 0;
}

int font_DrawRegion(lua_State *L)
{
	System		*sys;
	const char	*text;
	float		x, y, w, h;
	int		ft;

	l2sl_require(L, 6);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);
	l2sl_expectnumber(L, 3);
	l2sl_expectnumber(L, 4);
	l2sl_expectnumber(L, 5);
	l2sl_expectnumber(L, 6);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	ft = *(int *)lua_touserdata(L, 1);
	text = lua_tostring(L, 2);
	x = (float)lua_tonumber(L, 3);
	y = (float)lua_tonumber(L, 4);
	w = (float)lua_tonumber(L, 5);
	h = (float)lua_tonumber(L, 6);
	
	sys->GetGraphics().DrawTextBox(ft, string(text), x, y, w, h);
	return 0;
}

int system_LoadSound(lua_State *L)
{
	System		*sys;
	const char	*fn;
	void		*userdata;
	int		sd, obj, metatable;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);

	sys = *(System **)lua_touserdata(L, 1);
	fn = lua_tostring(L, 2);

	sd = sys->GetSound().LoadSound(sys->GetArchive(), string(fn));
	userdata = lua_newuserdata(L, sizeof(sd));
	*(int *)userdata = sd;
	obj = lua_gettop(L);
	lua_newtable(L);
	metatable = lua_gettop(L);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, metatable);
	lua_settable(L, metatable);
	lua_pushliteral(L, "Play");
	lua_pushcfunction(L, &sound_Play);
	lua_settable(L, metatable);
	lua_setmetatable(L, obj);
	return 1;
}

int sound_Play(lua_State *L)
{
	System	*sys;
	int	sd;

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);

	lua_pushliteral(L, "system");
	lua_gettable(L, LUA_GLOBALSINDEX);
	sys = *(System **)lua_touserdata(L, -1);
	sd = *(int *)lua_touserdata(L, 1);
	sys->GetSound().PlaySound(sd);
	return 0;
}

int system_UpdateMetacube(lua_State *L)
{
	System		*sys;

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);

	sys = *(System **)lua_touserdata(L, 1);
	sys->GetMetacube().Update(*sys);
	return 0;
}

int system_DrawMetacube(lua_State *L)
{
	System		*sys;

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);

	sys = *(System **)lua_touserdata(L, 1);
	sys->GetMetacube().Draw(sys->GetGraphics());
	return 0;
}

int system_MetaMorph(lua_State *L)
{
	static int	prev = 0;
	int		next;
	System		*sys;

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);

	sys = *(System **)lua_touserdata(L, 1);

	next = sys->GetRNG().GetNext() % 4;
	if (next == prev) next = (next+1) % 4;
	prev = next;
	Metacube::t_morph	morph;
	switch (next) {
	case 0:
		morph = Metacube::MORPH_DISTRIBUTION;
		break;
	case 1:
		morph = Metacube::MORPH_PLANE;
		break;
	case 2:
		morph = Metacube::MORPH_TORUS;
		break;
	case 3:
		morph = Metacube::MORPH_SPHERE;
		break;
	}
	sys->GetMetacube().Morph(morph);
	return 0;
}

/* function system:GetFileList(directory, pattern)
	returns all files in directory matching the regular expression pattern.
*/
int system_GetFileList(lua_State *L)
{
	const char*	path;
	
	l2sl_require(L, 2);
	l2sl_expectstring(L, 1);
	l2sl_expectstring(L, 2);

	Regex pattern(string(lua_tostring(L, 2)));
	path = lua_tostring(L, 1);

	lua_newtable(L);

	boost::filesystem::path	directory(path);
	if (exists(directory) && is_directory(directory)) {
		boost::filesystem::directory_iterator end_itr, itr(directory);
		for (int index = 1; itr != end_itr; ++itr) {
			if (!is_directory(*itr)) {
				vector<string>	result = pattern.match(itr->path().leaf().string());
				if (result.size() > 0) {
					lua_pushnumber(L, (lua_Number)index++);
					lua_pushstring(L, result[1].c_str());
					lua_settable(L, -3);
				}
			}
		}
	}

	return 1;
}

int system_PathExists(lua_State *L)
{
	const char*	path;
	
	l2sl_require(L, 1);
	l2sl_expectstring(L, 1);

	path = lua_tostring(L, 1);

	lua_pushboolean(L, boost::filesystem::exists(path));

	return 1;
}

int system_CreateDirectory(lua_State *L)
{
	const char*	path;
	
	l2sl_require(L, 1);
	l2sl_expectstring(L, 1);

	path = lua_tostring(L, 1);

	boost::filesystem::create_directory(path);

	return 0;
}

#define SBUFSZ 64
int system_ListDisplayModes(lua_State *L)
{
	SDL_Rect	**modes;
	char		strbuff[SBUFSZ];

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);
	
	modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
	lua_newtable(L);
	if (modes == (SDL_Rect **)0 || modes == (SDL_Rect **)-1) {
		return 1;
	} else {
		for (int i = 0; modes[i]; ++i) {
			snprintf(strbuff, SBUFSZ, "%dx%d", modes[i]->w, modes[i]->h);
			lua_pushnumber(L, (lua_Number)(i+1));
			lua_pushstring(L, strbuff);
			lua_settable(L, -3);
		}
	}
	return 1;
}

int system_Exit(lua_State *L)
{
	System		*sys;

	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);

	sys = *(System **)lua_touserdata(L, 1);
	sys->Exit();
	return 0;
}

int gl_PushMatrix(lua_State *L)
{
	glPushMatrix();
	return 0;
}

int gl_PopMatrix(lua_State *L)
{
	glPopMatrix();
	return 0;
}

int gl_Rotate(lua_State *L)
{
	double	angle, x, y, z;

	l2sl_require(L, 3);
	l2sl_expectnumber(L, 1);
	l2sl_expectnumber(L, 2);
	l2sl_expectnumber(L, 3);

	angle = lua_tonumber(L, 1);
	x = lua_tonumber(L, 2);
	y = lua_tonumber(L, 3);
	z = lua_tonumber(L, 4);
	glRotated(angle, x, y, z);
	return 0;
}

int gl_Scale(lua_State *L)
{
	double	x, y, z;

	l2sl_require(L, 3);
	l2sl_expectnumber(L, 1);
	l2sl_expectnumber(L, 2);
	l2sl_expectnumber(L, 3);

	x = lua_tonumber(L, 1);
	y = lua_tonumber(L, 2);
	z = lua_tonumber(L, 3);
	glScaled(x, y, z);
	return 0;
}

int gl_Translate(lua_State *L)
{
	double	x, y, z;

	l2sl_require(L, 3);
	l2sl_expectnumber(L, 1);
	l2sl_expectnumber(L, 2);
	l2sl_expectnumber(L, 3);

	x = lua_tonumber(L, 1);
	y = lua_tonumber(L, 2);
	z = lua_tonumber(L, 3);
	glTranslated(x, y, z);
	return 0;
}

int gl_Color3(lua_State *L)
{
	double	r, g, b;

	l2sl_require(L, 3);
	l2sl_expectnumber(L, 1);
	l2sl_expectnumber(L, 2);
	l2sl_expectnumber(L, 3);

	r = lua_tonumber(L, 1);
	g = lua_tonumber(L, 2);
	b = lua_tonumber(L, 3);
	glColor3d(r, g, b);
	return 0;
}

int gl_Color4(lua_State *L)
{
	double	r, g, b, a;

	l2sl_require(L, 4);
	l2sl_expectnumber(L, 1);
	l2sl_expectnumber(L, 2);
	l2sl_expectnumber(L, 3);
	l2sl_expectnumber(L, 4);

	r = lua_tonumber(L, 1);
	g = lua_tonumber(L, 2);
	b = lua_tonumber(L, 3);
	a = lua_tonumber(L, 4);
	glColor4d(r, g, b, a);
	return 0;
}

int gl_BeginQuads(lua_State *L)
{
	glBegin(GL_QUADS);
	return 0;
}

int gl_End(lua_State *L)
{
	glEnd();
	return 0;
}

int gl_Vertex3(lua_State *L)
{
	double	x, y, z;

	l2sl_require(L, 3);
	l2sl_expectnumber(L, 1);
	l2sl_expectnumber(L, 2);
	l2sl_expectnumber(L, 3);

	x = lua_tonumber(L, 1);
	y = lua_tonumber(L, 2);
	z = lua_tonumber(L, 3);
	glVertex3d(x, y, z);
	return 0;
}

#ifdef WINDOWS
struct aviinfo_t {
	PAVIFILE		avifile;
	AVISTREAMINFO		audioinfo;
	AVISTREAMINFO		videoinfo;
	PAVISTREAM		videostream;
	PAVISTREAM		audiostream;
	PAVISTREAM		vidc, audioc;// Compressed streams
	AVICOMPRESSOPTIONS	compressopts;
	int			width, height, bpp, frame, auframe;
};
#endif

int system_AviOpen(lua_State *L)
{
#ifndef WINDOWS
	//UNIMPLEMENTED
	return 0;
#else
	aviinfo_t	*avi;
	const char	*fn;
	int		obj, metatable;
	HRESULT		hr;

	l2sl_require(L, 2);
	l2sl_expectuserdata(L, 1);
	l2sl_expectstring(L, 2);

	System		*sys;
	sys = *(System **)lua_touserdata(L, 1);
	fn = lua_tostring(L, 2);

	avi = (aviinfo_t *)lua_newuserdata(L, sizeof(aviinfo_t));
	memset((void *)avi, 0, sizeof(aviinfo_t));
	avi->width = sys->GetGraphics().GetWidth();
	avi->height = sys->GetGraphics().GetHeight();
	avi->bpp = 24;

	AVIFileInit();
	// Open AVI File
	if (AVIFileOpen(&avi->avifile, fn, OF_CREATE, NULL) != 0) {
		std::cerr << "Could not open the AVI file:" << fn << "\n";
		lua_pushnil(L);
		return 1;
	}

	// Create Video Stream
	avi->videoinfo.fccType = streamtypeVIDEO;
	avi->videoinfo.dwScale = 1;
	avi->videoinfo.dwRate = 60;
	avi->videoinfo.dwSuggestedBufferSize = avi->width*avi->height*avi->bpp/8;
	avi->videoinfo.dwQuality = -1;
	SetRect(&avi->videoinfo.rcFrame, 0, 0, avi->width, avi->height);
	hr = AVIFileCreateStream(avi->avifile, &avi->videostream, &avi->videoinfo);
	if (hr != AVIERR_OK) {
		std::cerr << "Could not create video stream.\n";
		lua_pushnil(L);
		return 1;
	}

	// Create Audio Stream
	SDL_AudioSpec	wavfmt = sys->GetSound().Fmt();
	WAVEFORMATEX	wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = wavfmt.channels;
	wfx.wBitsPerSample = wavfmt.format & 0xFF;
	wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample >> 3);
	wfx.nSamplesPerSec = wavfmt.freq;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize = sizeof(wfx);

	avi->audioinfo.fccType = streamtypeAUDIO;
	avi->audioinfo.dwScale = wfx.nBlockAlign;
	avi->audioinfo.dwRate = wfx.nAvgBytesPerSec;
	avi->audioinfo.dwSampleSize = wfx.nBlockAlign;
	hr = AVIFileCreateStream(avi->avifile, &avi->audiostream, &avi->audioinfo);
	if (hr != AVIERR_OK) {
		std::cerr << "Could not create audio stream.\n";
		lua_pushnil(L);
		return 1;
	}
	hr = AVIStreamSetFormat(avi->audiostream, 0, &wfx, wfx.cbSize);
	if (hr != AVIERR_OK) {
		std::cerr << "Could not set audio stream format.\n";
		lua_pushnil(L);
		return 1;
	}

	// Prompt for compression type
	AVICOMPRESSOPTIONS *aopts[1];
	aopts[0] = &avi->compressopts;
	if (!AVISaveOptions(NULL, 0, 1, &avi->videostream, aopts)) {
		AVISaveOptionsFree(1, aopts);
		lua_pushnil(L);
		return 1;
	}

	// Create Compressed Stream
	hr = AVIMakeCompressedStream(&avi->vidc, avi->videostream,
		&avi->compressopts, NULL);
	AVISaveOptionsFree(1, aopts);
	if (hr != AVIERR_OK) {
			std::cerr << "Could not create a compressed video stream.\n";
			lua_pushnil(L);
			return 1;
	}

	// Set video stream format
	BITMAPINFOHEADER	format;
	memset((void *)&format, 0, sizeof(format));
	format.biSize = sizeof(format);
	format.biWidth = avi->width;
	format.biHeight = avi->height;
	format.biPlanes = 1;
	format.biBitCount = avi->bpp;
	format.biCompression = BI_RGB;
	hr = AVIStreamSetFormat(avi->vidc, 0, (void *)&format, format.biSize);
	if (hr != AVIERR_OK) {
		std::cerr << "Could not set the video stream format.\n";
		lua_pushnil(L);
		return 1;
	}

	obj = lua_gettop(L);
	lua_newtable(L);
	metatable = lua_gettop(L);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, metatable);
	lua_settable(L, metatable);
	lua_pushliteral(L, "WriteFrame");
	lua_pushvalue(L, 1);// sys pointer
	lua_pushcclosure(L, &avi_WriteFrame, 1);
	lua_settable(L, metatable);
	lua_pushliteral(L, "Close");
	lua_pushvalue(L, 1);// sys pointer
	lua_pushcclosure(L, &avi_Close, 1);
	lua_settable(L, metatable);
	lua_setmetatable(L, obj);

	// Stop Audio
	SDL_PauseAudio(1);
	sys->GetSound().StopAll();
	return 1;
#endif
}

// TODO: C closure instead of table lookups?
int avi_WriteFrame(lua_State *L)
{
#ifndef WINDOWS
	//UNIMPLEMENTED
	return 0;
#else
	System		*sys;
	aviinfo_t	*avi;
	Uint8		*framebuff;
	int		framesize;
	HRESULT		hr;

	l2sl_expectuserdata(L, lua_upvalueindex(1));
	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);
	
	sys = *(System **)lua_touserdata(L, lua_upvalueindex(1));
	avi = (aviinfo_t *)lua_touserdata(L, 1);
	
	framesize = avi->width*avi->height*avi->bpp/8;
	framebuff = new Uint8[framesize];
	glFlush();
	glReadPixels(0, 0, avi->width, avi->height, GL_BGR_EXT, GL_UNSIGNED_BYTE, (void *)framebuff);
	hr = AVIStreamWrite(avi->vidc, avi->frame, 1, (void *)framebuff,
		framesize, 0, NULL, NULL);
	delete[] framebuff;
	if (hr != AVIERR_OK) {
			std::cerr << "Error while writing to AVI video stream.\n";
			throw "fail";
	}
	avi->frame += 1;

	// Add audio frames
	SDL_AudioSpec	wavfmt = sys->GetSound().Fmt();
	int		nsamples = wavfmt.channels * (wavfmt.freq / 60);
	int		wavlen = nsamples * ((wavfmt.format & 0xFF) >> 3);
	Uint8		*wavbuff = new Uint8[wavlen];

	memset(wavbuff, 0, wavlen);
	sys->GetSound().Mix(wavbuff, wavlen);
	hr = AVIStreamWrite(avi->audiostream, avi->auframe, nsamples,
		wavbuff, wavlen, 0, NULL, NULL);
	delete[] wavbuff;
	if (hr != AVIERR_OK) {
			std::cerr << "Error while writing to AVI audio stream.\n";
			throw "fail";
	}
	avi->auframe += nsamples;
	return 0;
#endif
}

int avi_Close(lua_State *L)
{
#ifndef WINDOWS
	//UNIMPLEMENTED
	return 0;
#else
	System		*sys;
	aviinfo_t	*avi;

	l2sl_expectuserdata(L, lua_upvalueindex(1));
	l2sl_require(L, 1);
	l2sl_expectuserdata(L, 1);
	
	sys = *(System **)lua_touserdata(L, lua_upvalueindex(1));
	avi = (aviinfo_t *)lua_touserdata(L, 1);

	if (avi->audiostream != 0) {
		AVIStreamRelease(avi->audiostream);
		avi->audiostream = 0;
	}
	if (avi->vidc != 0) {
		AVIStreamRelease(avi->vidc);
		avi->vidc = 0;
	}
	if (avi->videostream != 0) {
		AVIStreamRelease(avi->videostream);
		avi->videostream = 0;
	}
	if (avi->avifile != 0) {
		AVIFileRelease(avi->avifile);
		avi->avifile = 0;
	}
	AVIFileExit();
	SDL_PauseAudio(0);
	return 0;
#endif
}

int gl_EnableClipPlane(lua_State *L)
{
	int	id;

	l2sl_require(L, 1);
	l2sl_expectnumber(L, 1);

	id = (int) lua_tonumber(L, 1);
	glEnable(GL_CLIP_PLANE0+id);
	return 0;
}

int gl_DisableClipPlane(lua_State *L)
{
	int	id;

	l2sl_require(L, 1);
	l2sl_expectnumber(L, 1);

	id = (int) lua_tonumber(L, 1);
	glDisable(GL_CLIP_PLANE0+id);
	return 0;
}

int gl_ClipPlane(lua_State *L)
{
	double	eq[4];
	int	id;

	l2sl_require(L, 5);
	l2sl_expectnumber(L, 1);
	l2sl_expectnumber(L, 2);
	l2sl_expectnumber(L, 3);
	l2sl_expectnumber(L, 4);
	l2sl_expectnumber(L, 5);

	id = (int) lua_tonumber(L, 1);
	eq[0] = lua_tonumber(L, 2);
	eq[1] = lua_tonumber(L, 3);
	eq[2] = lua_tonumber(L, 4);
	eq[3] = lua_tonumber(L, 5);

	glClipPlane(GL_CLIP_PLANE0+id, eq);
	return 0;
}
