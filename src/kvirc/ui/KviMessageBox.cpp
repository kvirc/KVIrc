//=============================================================================
//
//   File : KviMessageBox.cpp
//   Creation date : Tue Jun 20 2000 12:49:41 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviMessageBox.h"
#include "KviMemory.h"
#include "KviCString.h"
#include "KviLocale.h"

// FIXME: #warning "This should go into TAL as layer ...then maybe a wrapper in KviApplication!"
#ifdef COMPILE_KDE_SUPPORT
#include <kmessagebox.h>
#else
#include <QMessageBox>
#endif //!COMPILE_WITH_KDE

// FIXME: #warning "This could go into KviApplication"

namespace KviMessageBox
{
	void warning(QString fmt, ...)
	{
		kvi_va_list list;
		kvi_va_start(list, fmt);
		QString s;
		KviQString::vsprintf(s, fmt, list);
		kvi_va_end(list);
#ifdef COMPILE_KDE_SUPPORT
		KMessageBox::error(nullptr, s, "KVIrc");
#else
		QMessageBox::warning(0, "KVIrc", s);
#endif
	}

	void information(QString fmt, ...)
	{
		kvi_va_list list;
		kvi_va_start(list, fmt);
		QString s;
		KviQString::vsprintf(s, fmt, list);
		kvi_va_end(list);
#ifdef COMPILE_KDE_SUPPORT
		KMessageBox::information(nullptr, s, "KVIrc");
#else
		QMessageBox::information(0, "KVIrc", s);
#endif
	}

	bool yesNo(const QString & caption, QString fmt, ...)
	{
		kvi_va_list list;
		kvi_va_start(list, fmt);
		QString s;
		KviQString::vsprintf(s, fmt, list);
		kvi_va_end(list);
		bool bRet;
#ifdef COMPILE_KDE_SUPPORT
		bRet = (KMessageBox::questionYesNo(nullptr, s, caption) == KMessageBox::Yes);
#else
		bRet = (QMessageBox::information(0, caption, s,
		            QMessageBox::Yes | QMessageBox::Default,
		            QMessageBox::No | QMessageBox::Escape)
		    == QMessageBox::Yes);
#endif
		return bRet;
	}
}
