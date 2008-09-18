//=============================================================================
//
//   File : channelsjoinwindow.h
//   Creation date : Thu Nov 06 2001 12:30:25 CEST by Juan Jos��varez
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#ifndef _CHANNELSJOIN_H_
#define _CHANNELSJOIN_H_
#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_tal_treewidget.h"

#include <QCheckBox>
#include <QDialog>

class QPushButton;
class QLineEdit;
class KviConsole;
class KviTalGroupBox;


class KviChannelsJoinWindow : public QDialog
{
	Q_OBJECT
public:
	KviChannelsJoinWindow(QWidget * par, const char * name);
	~KviChannelsJoinWindow();
protected:
	QLineEdit   * m_pChannelEdit; 
	KviTalTreeWidget   * m_pListView;
	KviTalGroupBox   * m_pGroupBox;
	QLineEdit   * m_pPass;
	QCheckBox   * m_pShowAtStartupCheck;
	QCheckBox   * m_pCloseAfterJoinCheck;
	QPushButton * m_pJoinButton;
	QPushButton * m_pRegButton;
	KviConsole  * m_pConsole;
public:
	virtual void closeEvent(QCloseEvent *e);
	void fillListView();
	void enableJoin();
	void setConsole(KviConsole  * pConsole);
protected slots:
	void editTextChanged(const QString &);
	void itemClicked(QTreeWidgetItem *it,int);
	void itemDoubleClicked(QTreeWidgetItem *it,int);
	void editReturnPressed();
	void cancelClicked();
	void joinClicked();
	void regClicked();
//	void whoClicked();
//	void namesClicked();
};

#endif //_CHANNELSJOINSWINDOW_H_
