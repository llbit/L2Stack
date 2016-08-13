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

void VirtualWell::Allocate()
{
	well = new int[TET_WELL_W * TET_WELL_H];
	Clear();
}

void VirtualWell::Clear()
{
	memset(well, 0, sizeof(int)*TET_WELL_W*TET_WELL_H);
}

void VirtualWell::ClearLines()
{
	for (int y = TET_WELL_H-1; y >= 0; --y) {
		if (IsLine(y)) {
			for (int y2 = y-1; y2 >= 0; --y2) {
				CopyRow(y2+1, y2);
			}
			SetRow(0, 0);
			y += 1;
		}
	}
}

void VirtualWell::Free()
{
	if (well != NULL) {
		delete[] well;
		well = NULL;
	}
}

void VirtualWell::CopyRow(int ydst, int ysrc)
{
	memcpy(&well[ydst*TET_WELL_W], &well[ysrc*TET_WELL_W], sizeof(int)*TET_WELL_W);
}

void VirtualWell::SetRow(int b, int y)
{
	memset(&well[y*TET_WELL_W], b, sizeof(int)*TET_WELL_W);
}

void VirtualWell::SetBlock(int b, int x, int y)
{
	well[x + y*TET_WELL_W] = b;
}

int VirtualWell::GetBlock(int x, int y) const
{
	if (x < 0 || x >= TET_WELL_W || y < 0 || y >= TET_WELL_H) {
		return 0;
	}
	return well[x + y*TET_WELL_W];
}

bool VirtualWell::TestCollision(int x, int y) const
{
	if (x < 0 || x >= TET_WELL_W || y <0 || y >= TET_WELL_H) {
		return true;
	}
	if (well[x + y*TET_WELL_W] > 0) {
		return true;
	}
	return false;
}

int VirtualWell::CountLines(int y1, int y2) const
{
	int lines = 0;
	for (int y = y1; y < y2; ++y) {
		lines += IsLine(y) ? 1 : 0;
	}
	return lines;
}

bool VirtualWell::IsLine(int y) const
{
	for (int x = 0; x < TET_WELL_W; ++x) {
		if (GetBlock(x, y) == 0)
			return false;
	}
	return true;
}

void VirtualWell::AddGarbageLine(int hole)
{
	for (int y = 0; y < TET_WELL_H-1; ++y) {
		CopyRow(y, y+1);
	}
	for (int x = 0; x < TET_WELL_W; ++x) {
		SetBlock(x != hole ? 1 : 0, x, TET_WELL_H-1);
	}
}

int VirtualWell::GetLineDensity(int y) const
{
	int	density = 0;
	for (int x = 0; x < TET_WELL_W; ++x) {
		density += GetBlock(x, y);
	}
	return density;
}

int VirtualWell::GetTopLine() const
{
	for (int y = 0; y < TET_WELL_H; ++y) {
		if (GetLineDensity(y) > 0) {
			return y;
		}
	}
	return TET_WELL_H;
}

void VirtualWell::PrintState()
{
	std::cerr << "-----------\n";
	for (int y = 1; y < TET_WELL_H; ++y) {
		for (int x = 0; x < TET_WELL_W; ++x) {
			std::cerr << GetBlock(x, y);
		}
		std::cerr << "\n";
	}
}
