#ifndef _KVI_TAL_LISTVIEW_H_
#define _KVI_TAL_LISTVIEW_H_

//=============================================================================
//
//   File : kvi_tal_listview.h
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================


//
// This is the only reasonable CROSS-QT3-QT4-COMPATIBLE implementation
// of QListView I've been able to find.
// Note that using macros for the items will NOT work since moc
// doesn't expand them. Note also that KviTalCheckListItem must
// be fully reimplemented and not be inherited from QCheckListItem
// to build up a consistent item object hierarchy. To complete
// the obscenity, we need TWO COMPLETE implementations: one for Qt3
// and one for Qt4... bleah :D
//
// The code for KviTalCheckListItem is adapted from qlistview.h/cpp
// present in qt 3.3.6 AND in qt 4.1.2.
//

#include "kvi_settings.h"

#ifdef COMPILE_USE_QT4
	#include "kvi_tal_listview_qt4.h"
#else
	#include "kvi_tal_listview_qt3.h"
#endif

#endif // _KVI_TAL_LISTVIEW_H_
