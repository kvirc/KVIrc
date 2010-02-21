//=============================================================================
//
//   File : kvi_tal_toolbar.cpp
//   Creation date : Mon Aug 13 05:05:45 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_tal_toolbar.h"

#ifdef COMPILE_KDE_SUPPORT
	KviTalToolBar::KviTalToolBar(const QString & szLabel, QMainWindow * pWnd, Qt::ToolBarArea type, bool bNewLine, const char * pcName)
	: KToolBar(pcName,pWnd,type,bNewLine)
	{
		setWindowTitle(szLabel);
		//toolbars are locked by default
		setToolBarsLocked(false);
	}
#else
	KviTalToolBar::KviTalToolBar(const QString & szLabel, QMainWindow * pWnd, Qt::ToolBarArea type, bool bNewLine, const char * pcName)
	: QToolBar(szLabel,pWnd)
	{
		//setFrameStyle(QFrame::NoFrame);
		setObjectName(pcName);
		if(!layout())
			this->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
		pWnd->addToolBar(type,this);
	}

	KviTalToolBar::KviTalToolBar(QMainWindow * pWnd, const char * pcName)
	: QToolBar(pWnd)
	{
		//setFrameStyle(QFrame::NoFrame);
		setObjectName(pcName);
		if(!layout())
			this->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
		pWnd->addToolBar(this);
	}

	QBoxLayout * KviTalToolBar::boxLayout()
	{
		return (QBoxLayout*)this->layout();
	}

	void KviTalToolBar::setBoxLayout(QBoxLayout * pLayout)
	{
		this->setLayout(pLayout);
	}
#endif // COMPILE_KDE_SUPPORT

	bool KviTalToolBar::usesBigPixmaps()
	{
		return (iconSize().width() > 40);
	}

	void KviTalToolBar::setUsesBigPixmaps(bool bUse)
	{
		if(bUse)
			setIconSize(QSize(48,48));
		else
			setIconSize(QSize(22,22));
	}

KviTalToolBar::~KviTalToolBar()
{
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_toolbar.moc"
#endif // COMPILE_USE_STANDALONE_MOC_SOURCES
