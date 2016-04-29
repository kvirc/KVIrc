#ifndef _KVI_MSGBOX_H_
#define _KVI_MSGBOX_H_
//=============================================================================
//
//   File : KviMessageBox.h
//   Creation date : Sun Jan 17 1999 13:55:41 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
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

#include "kvi_settings.h"
#include "KviQString.h"
/*
class KviMessageBox : public KviTalMessageBox
{
	enum Result { Yes, No, Ok, Cancel };
	enum ButtonResult { Button1, Button2, Button3 };

	KVIRC_API void warning(const char * fmt,...);
	KVIRC_API void information(const char * fmt,...);

	KVIRC_API Result yesNo(const char * fmt,...);
	KVIRC_API Result yesNoCancel(const char * fmt,...);

	KVIRC_API ButtonResult oneButton(const char * btnTxt,const char * fmt,...);
	KVIRC_API ButtonResult twoButtons(const char * btnTxt1,const char * btnTxt2,const char * fmt,...);
	KVIRC_API ButtonResult threeButtons(const char * btnTxt1,const char * btnTxt2,const char * btnTxt3,const char * fmt,...);
};
*/

namespace KviMessageBox
{
	KVIRC_API void warning(QString fmt, ...);
	KVIRC_API void information(QString fmt, ...);
	KVIRC_API bool yesNo(const QString & caption, QString fmt, ...);
}

#endif //!_KVI_MSGBOX_H_
