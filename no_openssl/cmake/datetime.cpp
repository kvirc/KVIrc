//=============================================================================
//
//   File : datetime.cpp
//   Creation date : Fri 11 Apr 2008 22:46:15 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Szymon Stefanek <pragma at kvirc dot net>
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include <iostream>
#include <ctime>

int main(int,char **)
{
	std::time_t tNow = std::time(NULL);
	struct std::tm * tNowParts = std::gmtime(&tNow);
	if(!tNowParts)
		std::cout << "ERROR" << std::endl;
	else
		std::cout
			<< (tNowParts->tm_year + 1900)
			<< "-"
			<< ((tNowParts->tm_mon + 1) / 10)
			<< ((tNowParts->tm_mon + 1) % 10)
			<< "-"
			<< ((tNowParts->tm_mday) / 10)
			<< ((tNowParts->tm_mday) % 10)
			<< " "
			<< ((tNowParts->tm_hour) / 10)
			<< ((tNowParts->tm_hour) % 10)
			<< ":"
			<< ((tNowParts->tm_min) / 10)
			<< ((tNowParts->tm_min) % 10)
			<< ":"
			<< ((tNowParts->tm_sec) / 10)
			<< ((tNowParts->tm_sec) % 10)
			<< " UTC"
			<< std::endl;
	return 0;
}
