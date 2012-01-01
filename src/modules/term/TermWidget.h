#ifndef _HELPWIDGET_H_
#define _HELPWIDGET_H_
//=============================================================================
//
//   File : TermWidget.h
//   Creation date : Wed Aug 30 2000 15:34:20 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#ifdef COMPILE_KDE_SUPPORT
	#include <QFrame>
	#include <QLabel>
	#include <QPushButton>
	#include "KviTalHBox.h"

	#include "kparts/part.h"

	class KviMainWindow;

	class TermWidget : public QFrame
	{
		Q_OBJECT
		Q_PROPERTY( int KviProperty_ChildFocusOwner READ dummy )
	public:
		TermWidget(QWidget * par,bool bIsStandalone = false);
		~TermWidget();
	private:
		KviTalHBox   * m_pHBox;
		QLabel       * m_pTitleLabel;
		QPushButton  * m_pCloseButton;
		KParts::Part * m_pKonsolePart;
		bool           m_bIsStandalone;
		QWidget      * m_pKonsoleWidget;
	protected:
		virtual void resizeEvent(QResizeEvent *e);
	protected slots:
		void closeClicked();
		void changeTitle(int i,const QString& str);
		void notifySize(int,int);
		void changeColumns(int);
	public:
		QWidget * konsoleWidget(){ return m_pKonsoleWidget ? m_pKonsoleWidget : this; };
		virtual QSize sizeHint() const;
		int dummy() const { return 0; };
	protected slots:
		void konsoleDestroyed();
		void autoClose();
	};
#endif

#endif //_HELPWIDGET_H_
