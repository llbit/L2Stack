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
#ifndef tet_archive
#define tet_archive

class Archive {
	struct item_t {
		string	name;
		u32	data_offset, name_offset, raw_size, size;
	};

	list<item_t>	items;
	fstream		stream;

	u32 MaxCodeLength(u32 input_length);

public:
	Archive();
	~Archive();

	void OpenWrite(const string &);
	void OpenRead(const string &);
	void Close();

	void AddItem(const string &fn);
	void ReadItem(const string &fn, void** data, u32* size);
	void ReadIndex();
	void WriteArchive();
};

#endif
