/*
    Copyright 2009 Jesper Öqvist (jesper@llbit.se)

    This file is part of L2Stack
*/
#include "tet_incs.h"

/**
 * Load an .ini file and write the options back to Lua.
 */
void settings_Load(System* sys, const string& fn, lua_State* L)
{
	u32		size;
	char*		data;
	int		tbl;

	try {
		sys->GetArchive().ReadItem(fn, (void**)&data, &size);
	} catch (...) {
		// nil return value signals faliure to the Lua scripts
		lua_pushnil(L);
		return;
	}

	Regex		rx_line("\n", PCRE_NEWLINE_ANY);
	Regex		rx_header("^\\s*(\\w+)\\s*=\\s*(.+)$");
	Regex		rx_record("^\\s*(\\w+)\\s*=\\s*(.+)$");
	Regex		rx_number("^(\\d*)$");
	Regex		rx_string("^\"(.*)\"$");
	vector<string>	lines;
	vector<string>	match;
	//string		header("");

	lua_newtable(L);
	tbl = lua_gettop(L);

	lines = rx_line.split(string(data, size));
	delete[] data;
	for (size_t i = 0; i < lines.size(); ++i) {
		match = rx_record.match(lines[i]);

		if (match.size() < 3) {
			// Assume this line is garbage
			//match = rx_header.match(lines[i]);
			//if (match.size == 2)
				//header = match[1];
		} else {
			string name = match[1];
			string data = match[2];

			if (rx_number.test(data)) {
				int num = boost::lexical_cast<int>(data);
				lua_pushstring(L, name.c_str());
				lua_pushnumber(L, num);
				lua_settable(L, tbl);
			} else {
				match = rx_string.match(data);
				string str = match[1];
				lua_pushstring(L, name.c_str());
				lua_pushstring(L, str.c_str());
				lua_settable(L, tbl);
			}
		}
	}
}

void settings_Save(System* sys, const string& fn, lua_State* L)
{
	ofstream	file;
	int		tbl;

	file.open(fn.c_str());
	if (file.fail()) {
		std::cerr << "Could not open file: " << fn << "\n";
		throw "fail";
	}

	tbl = lua_gettop(L);
	lua_pushnil(L);
	while (lua_next(L, tbl) != 0) {
		const int key = -2;
		const int value = -1;

		if (lua_isnumber(L, value)) {
			file << lua_tostring(L, key) << " = "
				<< lua_tonumber(L, value) << "\n";
		} else if (lua_isstring(L, value)) {
			file << lua_tostring(L, key) << " = \""
				<< lua_tostring(L, value) << "\"\n";
		}
		lua_pop(L, 1);
	}

	file.close();
}

