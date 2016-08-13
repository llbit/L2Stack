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
#ifndef tet_script
#define tet_script

class System;

class Script {
	lua_State	*L;

public:
	Script();
	~Script();

	void SetUp(System &sys);
	void LoadScript(Archive &archive, const string &fn);
	void LoadScript(const string &s, const string &n);
	void LoadChunk(const char *chunk, int size, const string &n);

	void SetStateInt(const string&, int);
	void SetStateFloat(const string&, float);
	void SetStateString(const string&, const string&);
	void SetStateBool(const string&, int);
	void SetStateNil(const string&);
	string GetStateString(const string&);
	bool GetStateBool(const string&);
	int GetStateInt(const string&);

	int ParseInt(const string&);
	string ParseString(const string&);

	void Update(unsigned int millis);
	void Draw();

	void OnKeyDown(devkey_t);
	void OnKeyUp(devkey_t);
	void OnText(Uint16);
	void OnMouseMotion(int x, int y);
	void OnError(const string &);

	lua_State *LUA() {return L;}
};

#endif
