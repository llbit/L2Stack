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

Regex::Regex(const string &str, int options)
{
	const char	*error;
	int		erroffset;

	regex = pcre_compile(str.c_str(), 0, &error, &erroffset, NULL);
	if (error != NULL) {
		std::cerr << "PCRE Error: " << error << "\n";
		throw "fail";
	}
}

Regex::~Regex()
{
	pcre_free(regex);
}

int* Regex::alloc_ovector(int *size)
{
	pcre_fullinfo(regex, NULL, PCRE_INFO_CAPTURECOUNT, (void*)size);
	*size = ((*size) + 1) * 3;
	return new int[*size];
}

void Regex::free_ovector(int *ovec)
{
	delete[] ovec;
}

bool Regex::test(const string& str)
{
	int	*ovec;
	int	ovec_size, rc;

	ovec = alloc_ovector(&ovec_size);
	rc = pcre_exec(regex, NULL, str.c_str(), str.size(), 0, 0, ovec, ovec_size);
	free_ovector(ovec);
	if (rc <= 0) {
		return false;
	}
	return true;
}

vector<string> Regex::match(const string& str)
{
	int		*ovec;
	int		ovec_size, rc;
	vector<string>	result;

	ovec = alloc_ovector(&ovec_size);
	rc = pcre_exec(regex, NULL, str.c_str(), str.size(), 0, 0, ovec, ovec_size);
	for (int i = 0; i < rc; ++i) {
		int start = ovec[i*2];
		int count = ovec[i*2+1]-start;
		if (start >= 0) {
			result.push_back(str.substr(start, count));
		} else {
			result.push_back(string(""));
		}
	}
	free_ovector(ovec);
	return result;
}

vector<string> Regex::split(const string& str)
{
	int		*ovec;
	int		ovec_size, offset = 0;
	vector<string>	result;

	ovec = alloc_ovector(&ovec_size);
	while (pcre_exec(regex, NULL, str.c_str(), str.size(), offset, 0, ovec, ovec_size) > 0) {
		int count = ovec[0]-offset;
		result.push_back(str.substr(offset, count));
		offset = ovec[1];

		if (offset >= (int)str.size()) break;
	}
	if (offset < (int)str.size()) {
		int count = (int)str.size()-offset;
		result.push_back(str.substr(offset, count));
	}
	free_ovector(ovec);
	return result;
}
