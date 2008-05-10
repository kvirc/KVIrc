#ifndef _LANGDETECTOR_H_
#define _LANGDETECTOR_H_
//
//   File : langdetector.h
//   Creation date : Sun May 11 2008 00:07:20 CET by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "kvi_string.h"
#include "kvi_kvs_array.h"

#include "detector.h"

class KviLangDetetector
{
public:
	KviLangDetetector(QString doc);
	~KviLangDetetector();
};

#endif //_LANGDETECTOR_H_
