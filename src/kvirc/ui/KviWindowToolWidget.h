#ifndef _KVI_TOOLWINDOWS_CONTAINER_H_
#define _KVI_TOOLWINDOWS_CONTAINER_H_
//=============================================================================
//
//   File : KviWindowToolWidget.h
//   Creation date : Tue Aug 1 2000 21:03:41 by Szymon Stefanek
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

#include "KviHeapObject.h"
#include "KviIconManager.h"

#include <QToolButton>

class KviWindowToolWidget;
class KviWindowToolPageButton;

class KVIRC_API KviWindowToolWidget : public QWidget
{
	Q_OBJECT
public:
	KviWindowToolWidget(QWidget * pParent, KviWindowToolPageButton * pButton);
	~KviWindowToolWidget();

protected:
	KviWindowToolPageButton * m_pButton;
};

class KVIRC_API KviWindowToolPageButton : public QToolButton
{
	Q_OBJECT
public:
	KviWindowToolPageButton(KviIconManager::SmallIcon eIconOff, KviIconManager::SmallIcon eIconOn, const QString & szText, QWidget * pParent, bool bOn = false);
	~KviWindowToolPageButton();
};

#endif //_KVI_TOOLWINDOWS_CONTAINER_H_
