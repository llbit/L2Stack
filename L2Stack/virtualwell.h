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
#ifndef tet_virtualwell
#define tet_virtualwell

class VirtualWell {
	int	*well;

	void Allocate();
	void Free();

public:
	VirtualWell(const VirtualWell &w) : well(NULL) {Allocate();*this = w;}
	VirtualWell() : well(NULL) {Allocate();}
	~VirtualWell() {Free();}

	void CopyRow(int ydst, int ysrc);
	void SetRow(int b, int y);
	void SetBlock(int b, int x, int y);
	int GetBlock(int x, int y) const;
	bool TestCollision(int x, int y) const;
	int CountLines(int y1, int y2) const;
	bool IsLine(int y) const;
	int GetLineDensity(int y) const;
	int GetTopLine() const;
	void AddGarbageLine(int hole);
	void Clear();
	void ClearLines();
	void PrintState();

	VirtualWell& operator=(const VirtualWell &w) {
		memcpy(well, w.well, sizeof(int)*TET_WELL_W*TET_WELL_H);
		return *this;
	}
};

#endif
