#ifndef _KVINOTIFIERWINDOWTAB_H_
#define _KVINOTIFIERWINDOWTAB_H_
//=============================================================================
//
//   File : NotifierWindowTab.h
//   Creation date : Tue 07 Jul 2009 10:28 by Fabio Bas
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2009 Fabio Bas < ctrlaltca at gmail dot com >
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

/**
* \file NotifierWindowTab.h
* \author Fabio Bas
* \brief Defines the tab page for the notifier object
*/

#include "NotifierMessage.h"

#include "kvi_settings.h"

#include <QScrollArea>

class KviWindow;
class QPainter;
class QTabWidget;
class QVBoxLayout;
class QWidget;

/**
* \class NotifierWindowTab
* \brief Defines an object for every single tab about the tabs area
*/
class NotifierWindowTab : public QScrollArea
{
	Q_OBJECT
public:
	/**
	* \brief Creates the tab object
	* \param pWnd The name of the window represented by the tab
	* \param pParent The parent area container
	* \return NotifierWindowTab
	*/
	NotifierWindowTab(KviWindow * pWnd, QTabWidget * pParent);

	/**
	* \brief Destroys the tab object
	*/
	~NotifierWindowTab();

private:
	QString m_szLabel;
	KviWindow * m_pWnd = nullptr;
	QTabWidget * m_pParent = nullptr;
	QVBoxLayout * m_pVBox = nullptr;
	QWidget * m_pVWidget = nullptr;

public:
	/**
	* \brief Appends the given message to the window
	* \param pMessage The message received to append
	* \return void
	*/
	void appendMessage(NotifierMessage * pMessage);

	/**
	* \brief Updates the GUI
	* \return void
	*/
	void updateGui();

	/**
	* \brief Returns the name of the current window
	* \return QString
	*/
	QString label() const { return m_szLabel; }

	/**
	* \brief Returns the pointer of the current window
	* \return KviWindow *
	*/
	KviWindow * wnd() const { return m_pWnd; }
protected:
	void mouseDoubleClickEvent(QMouseEvent * e) override;
	void resizeEvent(QResizeEvent * e) override;
	void paintEvent(QPaintEvent * e) override;
private slots:
	/**
	* \brief Emitted when the scrollbar range is changed
	* \return void
	*/
	void scrollRangeChanged(int, int);

	/**
	* \brief Emitted when the window changes its name
	* \return void
	*/
	void labelChanged();

	/**
	* \brief Emitted when the window is being destroyed
	* \return void
	*/
	void closeMe();
};

#endif //!_KVINOTIFIERWINDOWTAB_H_
