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
#include <sstream>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include <boost/filesystem.hpp>
#include "zlib.h"

using std::string;
using std::list;
using std::fstream;
using std::ifstream;
using namespace boost::filesystem;

typedef unsigned int u32;
typedef char i8;

#include "../L2Stack/archive.h"

int main(int argc, char** argv)
{
	std::cout << "Building data.big..." << std::endl;
	
	Archive bigpak;
	boost::filesystem::path	directory("data");
	if (exists(directory) && is_directory(directory)) {
		boost::filesystem::directory_iterator end_itr, itr(directory);
		for (int index = 1; itr != end_itr; ++itr) {
			if (!is_directory(*itr)) {
				bigpak.AddItem(itr->path().string());
			}
		}
	} else {
		std::cout << "error: could not find the source directory." << std::endl;
		return 1;
	}

	bigpak.OpenWrite("data.big");
	bigpak.WriteArchive();
	bigpak.Close();

	std::cout << "done" << std::endl;

	return 0;
}
