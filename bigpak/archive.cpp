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
#include <string.h>
#include "zlib.h"

using std::string;
using std::list;
using std::fstream;
using std::ifstream;

typedef unsigned int u32;
typedef char i8;

#include "../L2Stack/archive.h"

Archive::Archive()
{
}

Archive::~Archive()
{
	if (stream.is_open()) {
		stream.close();
	}
}

u32 Archive::MaxCodeLength(u32 input_length)
{
	return (u32)(input_length * 1.2f) + 12;
}

void Archive::OpenWrite(const string &fn)
{
	stream.open(fn.c_str(), std::ios_base::out | std::ios_base::binary);
}

void Archive::OpenRead(const string &fn)
{
	stream.open(fn.c_str(), std::ios_base::in | std::ios_base::binary);
}

void Archive::ReadIndex()
{
	u32	index_offset;
	u32	raw_size, size;
	i8	*raw_data, *index;

	stream.read((char *)&index_offset, sizeof(index_offset));
	stream.seekg(index_offset, std::ios_base::beg);
	stream.read((char *)&size, sizeof(size));
	stream.read((char *)&raw_size, sizeof(raw_size));

	raw_data = new i8[raw_size];
	index = new i8[size];
	stream.read(raw_data, raw_size);
	uncompress((Bytef *)index, (uLongf *)&size, (Bytef *)raw_data, (uLong)raw_size);
	delete[] raw_data;

	// parse index
	size_t	nitems;
	memcpy(&nitems, &index[0], 4);
	for (u32 i = 0; i < nitems; ++i) {
		item_t	new_item;

		memcpy(&new_item.data_offset, &index[4 + 16*i], 4);
		memcpy(&new_item.name_offset, &index[8 + 16*i], 4);
		memcpy(&new_item.raw_size, &index[12 + 16*i], 4);
		memcpy(&new_item.size, &index[16 + 16*i], 4);
		new_item.name = string((char *)&index[new_item.name_offset]);
		
		items.push_back(new_item);
	}
}

void Archive::Close()
{
	stream.close();
}

void Archive::WriteArchive()
{
	list<item_t>::iterator	itr;
	u32			index_offset = 0;

	// leave room for index offset
	stream.write((char *)&index_offset, sizeof(index_offset));

	// write files
	u32	index_size = 4;
	for (itr = items.begin(); itr != items.end(); ++itr) {
		std::cout << "archiving file: " << itr->name << "\n";
		itr->data_offset = stream.tellp();
		index_size += 16 + (itr->name.size()+1);
		
		ifstream	file;
		i8		*data, *raw_data;

		file.open(itr->name.c_str(), std::ios_base::binary);
		if (file.fail()) {
			throw string("Could not open the file ") + itr->name;
		}

		file.seekg(0, std::ios_base::end);
		itr->size = file.tellg();
		file.seekg(0, std::ios_base::beg);
		data = new i8[itr->size];
		file.read(data, itr->size);
		file.close();

		itr->raw_size = MaxCodeLength(itr->size);
		raw_data = new i8[itr->raw_size];
		compress((Bytef *)raw_data, (uLongf *)&itr->raw_size, (Bytef *)data, (uLong)itr->size);
		delete[] data;

		stream.write(raw_data, itr->raw_size);
		delete[] raw_data;
	}

	// build index
	i8	*index = new i8[index_size];
	u32	nitems = items.size();
	u32	name_offset = 4 + 16*nitems;
	memcpy(&index[0], &nitems, 4);
	int	item = 0;
	for (itr = items.begin(); itr != items.end(); ++itr, ++item) {
		memcpy(&index[4 + 16*item], &itr->data_offset, 4);
		memcpy(&index[8 + 16*item], &name_offset, 4);
		memcpy(&index[12 + 16*item], &itr->raw_size, 4);
		memcpy(&index[16 + 16*item], &itr->size, 4);
		memcpy(&index[name_offset], itr->name.c_str(), itr->name.size()+1);
		name_offset += itr->name.size()+1;
	}

	// code index
	u32	index_raw_size = MaxCodeLength(index_size);
	i8	*raw_index = new i8[index_raw_size];
	compress((Bytef *)raw_index, (uLongf *)&index_raw_size, (Bytef *)index, (uLong)index_size);

	// write index
	index_offset = stream.tellp();
	stream.seekp(std::ios_base::beg);
	stream.write((char *)&index_offset, 4);
	stream.seekp(index_offset, std::ios_base::beg);
	stream.write((char *)&index_size, 4);
	stream.write((char *)&index_raw_size, 4);
	stream.write(raw_index, index_raw_size);
	delete[] index;
	delete[] raw_index;
}

void Archive::AddItem(const string &fn)
{
	item_t	new_item;

	new_item.name = fn;
	items.push_back(new_item);
}

void Archive::ReadItem(const string &fn, void** data, u32* size)
{
	list<item_t>::iterator	it;
	ifstream		file;

	// First try opening the file from disk
	file.open(fn.c_str(), std::ios_base::binary);
	if (!file.fail()) {
		file.seekg(0, std::ios_base::end);
		*size = file.tellg();
		file.seekg(0, std::ios_base::beg);

		*data = new i8[*size];

		file.read((char *)*data, *size);
		file.close();
		return;
	} else {
		for (it = items.begin(); it != items.end(); ++it) {

			if (it->name == fn) {
				*data = new i8[it->size];

				stream.seekg(it->data_offset, std::ios_base::beg);
				i8	*code = new i8[it->raw_size];
				stream.read((char *)code, it->raw_size);
				uncompress((Bytef*)*data, (uLongf *)size, (Bytef *)code, it->raw_size);
				delete[] code;

				return;
			}
		}
	}

	*data = NULL;
	*size = 0;

	throw string("File not found: ") + fn;
}
