//=============================================================================
//
//   File : channelsjoinwindow.cpp
//   Creation date : Thu Nov 6 2001 12:41:18 by Juanjo �varez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2003 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the linkss of the GNU General Public License
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

#include "channelsjoinwindow.h"

#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_selectors.h"
#include "kvi_app.h"
#include "kvi_string.h"
#include "kvi_iconmanager.h"
#include "kvi_console.h"
#include "kvi_regchan.h"
#include "kvi_kvs_script.h"
#include "kvi_tal_treewidget.h"
#include <kvi_tal_groupbox.h>

#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QEvent>
#include <QCloseEvent>
#include <QHeaderView>


extern KviChannelsJoinWindow * g_pChannelsWindow;
extern QRect                   g_rectChannelsJoinGeometry;
// kvi_app.cpp
extern KVIRC_API KviRegisteredChannelDataBase * g_pRegisteredChannelDataBase;


KviChannelsJoinWindow::KviChannelsJoinWindow(QWidget * par, const char * name) 
: QDialog(par)
{
	setObjectName(name);
	m_pConsole=0;
	setWindowTitle(__tr2qs("Join Channels"));

	setWindowIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)));

	QGridLayout * g = new QGridLayout(this);
	
	m_pListView = new KviTalTreeWidget(this);
	m_pListView->addColumn(__tr2qs("Channel"));
	m_pListView->setRootIsDecorated(true);
	m_pListView->setSelectionMode(QAbstractItemView::SingleSelection);
	g->addWidget(m_pListView,0,0,0,2);
	//g->addMultiCellWidget(m_pListView,0,0,0,1);
	connect(m_pListView,SIGNAL(clicked(KviTalTreeWidgetItem *)),this,SLOT(itemClicked(KviTalTreeWidgetItem *)));
	connect(m_pListView,SIGNAL(doubleClicked(KviTalTreeWidgetItem *)),this,SLOT(itemDoubleClicked(KviTalTreeWidgetItem *)));


	m_pGroupBox = new KviTalGroupBox(Qt::Horizontal,__tr2qs("Channel" ),this);
	QString szMsg = __tr2qs("Name");
	szMsg.append(":");
	QLabel * l = new QLabel(szMsg,m_pGroupBox);

	m_pChannelEdit = new QLineEdit(m_pGroupBox); 
	connect(m_pChannelEdit,SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));
	connect(m_pChannelEdit,SIGNAL(textChanged(const QString &)),this,SLOT(editTextChanged(const QString &)));

	szMsg = __tr2qs("Password");
	szMsg.append(":");
	l = new QLabel(szMsg,m_pGroupBox);

	m_pPass = new QLineEdit(m_pGroupBox);
	m_pPass->setEchoMode(QLineEdit::Password);

	g->addWidget(m_pGroupBox,1,1,0,1);
//	g->addMultiCellWidget(m_pGroupBox,1,1,0,1);

	m_pJoinButton = new QPushButton(__tr2qs("&Join"),this);
	// Join on return pressed
	m_pJoinButton->setDefault(true);
	connect(m_pJoinButton,SIGNAL(clicked()),this,SLOT(joinClicked()));

g->addWidget(m_pJoinButton,2,2,0,0,Qt::AlignHCenter);
//	g->addMultiCellWidget(m_pJoinButton,2,2,0,1,Qt::AlignHCenter);

	m_pShowAtStartupCheck = new KviStyledCheckBox(__tr2qs("Show this window after connecting"),this);
	m_pShowAtStartupCheck->setChecked(KVI_OPTION_BOOL(KviOption_boolShowChannelsJoinOnIrc));
	g->addWidget(m_pShowAtStartupCheck,3,0);


	QPushButton * cancelButton = new QPushButton(__tr2qs("Close"),this);
	connect(cancelButton,SIGNAL(clicked()),this,SLOT(cancelClicked()));

	g->addWidget(cancelButton,3,1);

/*
	KviTalHBox * hb = new KviTalHBox(this);
	hb->setSpacing(4);
	
	QPushButton * namesButton = new QPushButton("/names", hb);
	connect(namesButton,SIGNAL(clicked()),this,SLOT(namesClicked()));
	QPushButton * whoButton = new QPushButton("/who", hb);
	connect(whoButton,SIGNAL(clicked()),this,SLOT(whoClicked()));

	g->addWidget(hb,2,0);
*/

	g->setRowStretch(0,1);
	g->setColumnStretch(0,1);

	fillListView();

	if(g_rectChannelsJoinGeometry.y() < 5)g_rectChannelsJoinGeometry.setY(5);

	resize(g_rectChannelsJoinGeometry.width(),g_rectChannelsJoinGeometry.height());
	move(g_rectChannelsJoinGeometry.x(),g_rectChannelsJoinGeometry.y());

	enableJoin();
}

KviChannelsJoinWindow::~KviChannelsJoinWindow()
{
	KVI_OPTION_BOOL(KviOption_boolShowChannelsJoinOnIrc) = m_pShowAtStartupCheck->isChecked();

	g_rectChannelsJoinGeometry = QRect(pos().x(),pos().y(),size().width(),size().height());
	g_pChannelsWindow = 0;
}

void KviChannelsJoinWindow::setConsole(KviConsole  * pConsole)
{
	m_pConsole=pConsole;
	fillListView();
}

void KviChannelsJoinWindow::fillListView()
{
	m_pListView->clear();

	m_pListView->header()->hide();

	KviTalTreeWidgetItem * par = new KviTalTreeWidgetItem(m_pListView,__tr2qs("Recent Channels"));
	par->setExpanded(true);
	KviTalTreeWidgetItem * chld;
	
	if(m_pConsole)
	{
	
		QStringList* pList=g_pApp->getRecentChannels(m_pConsole->currentNetworkName());
		if(pList)
		{
			for(QStringList::Iterator it = pList->begin(); it != pList->end(); ++it)
			{
				chld = new KviTalTreeWidgetItem(par,*it);
				chld->setIcon(0,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)));
			}
		}
	}

	// FIXME: Registered channels go here!
	par = new KviTalTreeWidgetItem(m_pListView,__tr2qs("Registered Channels"));
	par->setExpanded(true);

	KviPointerHashTable<const char *,KviRegisteredChannelList> * d = g_pRegisteredChannelDataBase->channelDict();
	if(d)
	{
		KviPointerHashTableIterator<const char *,KviRegisteredChannelList> it(*d);
		while(it.current())
		{
			chld = new KviTalTreeWidgetItem(par,it.currentKey());
			chld->setIcon(0,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)));
			++it;
		}
	}
}

void KviChannelsJoinWindow::itemClicked(KviTalTreeWidgetItem * it) 
{
	if(!it)return;
	if(!it->parent())return;
	QString tmp = it->text(0);
	m_pChannelEdit->setText(tmp);
	enableJoin();
}

void KviChannelsJoinWindow::itemDoubleClicked(KviTalTreeWidgetItem * it) 
{
	if(!it)return;
	if(!it->parent())return;
	KviStr tmp = it->text(0);
	m_pChannelEdit->setText(tmp.ptr());
	enableJoin();
	joinClicked();
}

void KviChannelsJoinWindow::editTextChanged(const QString &)
{
	enableJoin();
}

void KviChannelsJoinWindow::enableJoin()
{
	KviStr tmp = m_pChannelEdit->text();
	KviConsole * c = g_pApp->topmostConnectedConsole();
	if(c)
	{
		if(tmp.isEmpty())
			m_pJoinButton->setEnabled(false);
		else
			m_pJoinButton->setEnabled(true);
	} else {
		m_pListView->setEnabled(false);
		m_pGroupBox->setEnabled(false);
		m_pJoinButton->setEnabled(false);
	}
}

void KviChannelsJoinWindow::cancelClicked()
{
	delete this;
}

void KviChannelsJoinWindow::joinClicked()
{
	KviStr pass = m_pPass->text();
	KviStr tmp = m_pChannelEdit->text();

	if(tmp.isEmpty())return;

	KviStr command(KviStr::Format,"join %s %s",tmp.ptr(),pass.ptr());

	KviConsole * c = g_pApp->topmostConnectedConsole();
	if(!c)return; // no connection
	KviWindow * w = g_pActiveWindow;
	if(w->console() != c)w = c;
	KviKvsScript::run(command.ptr(),w);

	m_pChannelEdit->setText("");
	m_pPass->setText("");
}

/*
void KviChannelsJoinWindow::whoClicked()
{
	KviStr tmp = m_pChannelEdit->text();
	if(!tmp.isEmpty())doCmd("who", tmp.ptr());
}

void KviChannelsJoinWindow::namesClicked()
{
	KviStr tmp = m_pChannelEdit->text();
	//FIXME: I must be a nice guy and implement /names in the core...
	if(!tmp.isEmpty())doCmd("raw names", tmp.ptr());
}

void KviChannelsJoinWindow::itemDoubleClicked(KviTalListBoxItem * it)
{
	if (it == 0)return;
	KviStr tmp = it->text();
	doCmd("join", tmp.ptr());
//	if(KVI_OPTION_BOOL(KviOption_boolCloseChannelsJoinAfterJoin))
//		g_pApp->collectGarbage(this);
}
*/

void KviChannelsJoinWindow::editReturnPressed()
{
	joinClicked();
}

void KviChannelsJoinWindow::closeEvent(QCloseEvent *e)
{
	e->ignore();
	delete this;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_channelsjoinwindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
