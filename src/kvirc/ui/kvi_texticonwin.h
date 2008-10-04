#ifndef _KVI_TEXTICONWIN_H_
#define _KVI_TEXTICONWIN_H_
//=============================================================================
//
//   File : kvi_texticonwin.h
//   Creation date : Fri May 17 2002 02:33:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

/**
* \file kvi_texticonwin.h
* \author Szymon Stefanek
* \brief Text icon window
*
* \def KVI_TEXTICON_WIN_WIDTH The width of the window
* \def KVI_TEXTICON_WIN_HEIGHT The height of the window
*/

#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_tal_iconview.h"
#include "kvi_iconmanager.h"

#include <QPainter>

#define KVI_TEXTICON_WIN_WIDTH 230
#define KVI_TEXTICON_WIN_HEIGHT 200

/**
* \class KviTextIconWindow
* \brief Text icon window class
*/
class KVIRC_API KviTextIconWindow : public KviTalIconView
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the text icon window objet
	* \param pParent The parent widget
	* \return KviTextIconWindow
	*/
	KviTextIconWindow(QWidget * pParent);

	/**
	* \brief Destroys the text icon window objet
	*/
	~KviTextIconWindow();
private:
	QWidget            * m_pOwner;
	KviTalIconViewItem * m_pItem;
	QWidget            * m_pParent;
	QString              m_szTypedSeq;
	QString              m_szCurFullSeq;
	bool                 m_bAltMode; // in alt mode the itemSelected() string contains also the CTRL+I escape code
	int                  m_iTimerId;
public:
	/**
	* \brief Shows the popup
	* \param pOwner The owner of the widget
	* \param bAltMode Whether to use the alternative mode, prepend the text
	* \return void
	*/
	void popup(QWidget * pOwner, bool bAltMode = false);
private:
	/**
	* \brief Hides the parent and set the focus to the owner
	* \return void
	*/
	void doHide();

	/**
	* \brief Shows the window
	* \return void
	*/
	virtual void show();

	/**
	* \brief Searches for an icon in the char sequence typed
	* \return bool
	*/
	bool findTypedSeq(); // returns true if it is a complete word
private:
	virtual void keyPressEvent(QKeyEvent * e);
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void timerEvent(QTimerEvent * e);
public slots:
	/**
	* \brief Fills in the text icon window
	* \return void
	*/
	void fill();

	/**
	* \brief Called when the owner is destroyed. Then it calls doHide()
	* \return void
	*/
	void ownerDead();

	/**
	* \brief Called when the current item is changed
	* \param pItem The selected item
	* \return void
	*/
	void currentItemChanged(KviTalIconViewItem * pItem, KviTalIconViewItem *);

	/**
	* \brief Called when an item is selected
	* \param pItem The selected item
	* \return void
	*/
	void itemSelected(KviTalIconViewItem * pItem);
};

/**
* \class KviTextIconWindowWidget
* \brief Text icon window widget class
*/
class KVIRC_API KviTextIconWindowWidget : public QWidget
{
	Q_OBJECT
public:
	KviTextIconWindow * m_pWindow;
public:
	/**
	* \brief Constructs the text icon window widget objet
	* \return KviTextIconWindowWidget
	*/
	KviTextIconWindowWidget();

	/**
	* \brief Destroys the text icon window widget objet
	*/
	~KviTextIconWindowWidget();
public:
	/**
	* \brief Shows the popup
	* \param pOwner The owner of the widget
	* \param bAltMode Whether to use the alternative mode, prepend the text
	* \return void
	*/
	void popup(QWidget * pOwner, bool bAltMode = false);
};

#endif //_KVI_TEXTICONWIN_H_
