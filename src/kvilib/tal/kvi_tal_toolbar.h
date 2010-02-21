#ifndef _KVI_TAL_TOOLBAR_H_
#define _KVI_TAL_TOOLBAR_H_
//=============================================================================
//
//   File : kvi_tal_toolbar.h
//   Creation date : Mon Aug 13 05:05:44 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

/**
* \file kvi_tal_toolbar.h
* \author Szymon Stefanek
* \brief Toolkit Abstraction Layer class
*/

#include "kvi_settings.h"

/**
* \class KviTalToolBar
* \brief Toolkit Abstraction Layer: toolbar class
*/

#ifdef COMPILE_KDE_SUPPORT

#include <ktoolbar.h>

class KVILIB_API KviTalToolBar : public KToolBar

#else // COMPILE_KDE_SUPPORT

#include <QToolBar>
#include <QMainWindow>
#include <QBoxLayout>

class KVILIB_API KviTalToolBar : public QToolBar

#endif // COMPILE_KDE_SUPPORT

{
	Q_OBJECT
public:
	/**
	* \brief Constructs a toolbar object
	* \param szLabel The label of the toolbar
	* \param pWnd The mainwindow where to attach the toolbar
	* \param type The type of the toolbar
	* \param bNewLine Whether to add a newline
	* \param pcName The name of the toolbar
	* \return KviTalToolBar
	*/
	KviTalToolBar(const QString & szLabel, QMainWindow * pWnd, Qt::ToolBarArea type = Qt::TopToolBarArea, bool bNewLine = false, const char * pcName = 0);

	/**
	* \brief Destroys a toolbar object
	*/
	~KviTalToolBar();
#ifndef COMPILE_KDE_SUPPORT
public:
	/**
	* \brief Constructs a toolbar object
	* \param pWnd The mainwindow where to attach the toolbar
	* \param pcName The name of the toolbar
	* \return KviTalToolBar
	*/
	KviTalToolBar(QMainWindow * pWnd, const char * pcName = 0);
public:
	/**
	* \brief Returns the box layout object
	* \return QBoxLayout *
	*/
	QBoxLayout * boxLayout();

	/**
	* \brief Sets the box layout
	* \param pLayout The box layout
	* \return void
	*/
	void setBoxLayout(QBoxLayout * pLayout);
#endif // COMPILE_KDE_SUPPORT
	/**
	* \brief Returns true if the toolbar uses big pixmaps
	* \return bool
	*/
	bool usesBigPixmaps();

	/**
	* \brief Sets the use of big pixmaps
	* \param bUse Whether to use big pixmaps
	* \return void
	*/
	void setUsesBigPixmaps(bool bUse);
};

#endif //_KVI_TAL_TOOLBAR_H_
