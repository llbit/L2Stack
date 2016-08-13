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
#ifndef tet_regex
#define tet_regex

class Regex {
	pcre	*regex;

	int* alloc_ovector(int* size);
	void free_ovector(int* ovector);

	Regex() {}
	Regex(const Regex& r) {}
	Regex& operator=(const Regex& r) { return *this; }// never used
public:
	Regex(const string &str, int options = 0);
	~Regex();

	bool test(const string &str);
	vector<string> match(const string &str);
	vector<string> split(const string &str);
};

#endif
