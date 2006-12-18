//
//   File : logviewwidget.cpp
//   Creation date : Thu Apr 23 2002 17:42:12 by Juanjo �varez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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

#include "logviewwidget.h"

#include <qsplitter.h>
#include <qlayout.h>
#include <qdir.h>

//#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_app.h"

#include "kvi_qstring.h"
#ifdef COMPILE_ZLIB_SUPPORT
	#include <zlib.h>
#endif
#include <qtextcodec.h>

const QPixmap * KviLogListViewItemType::pixmap(int col) const 
{
    if(KviQString::equalCI(m_szTypeName,"channel"))
        return g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL);
    else if(KviQString::equalCI(m_szTypeName,"query"))
        return g_pIconManager->getSmallIcon(KVI_SMALLICON_QUERY);
    else if(KviQString::equalCI(m_szTypeName,"dcc"))
        return g_pIconManager->getSmallIcon(KVI_SMALLICON_DCCMSG);
    else 
        return g_pIconManager->getSmallIcon(KVI_SMALLICON_CONSOLE);
}

#include "logviewwidget.moc"
