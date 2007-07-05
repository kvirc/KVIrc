//
//   File : kvi_ctcppagedialog.cpp
//   Creation date : Tue May 21 2002 22:09:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVIRC__

#include "kvi_ctcppagedialog.h"

#include "kvi_locale.h"
#include "kvi_app.h"

#include "kvi_iconmanager.h"

#include <qdatetime.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpixmap.h>

#ifdef COMPILE_USE_QT4
	#include <qdesktopwidget.h>
#endif

// kvi_app.cpp
extern KVIRC_API KviCtcpPageDialog * g_pCtcpPageDialog;

KviCtcpPageDialog::KviCtcpPageDialog()
: QWidget(0,"kvirc_ctcppage_dialog",
#ifdef COMPILE_USE_QT4
		Qt::WindowStaysOnTopHint | Qt::Tool | Qt::Dialog | Qt::Window)
#else
		  WStyle_StaysOnTop | WStyle_Tool | WType_Dialog | WType_TopLevel)
#endif
{
	QGridLayout * g = new QGridLayout(this,4,1,6,0);
	m_pWidgetStack = new KviTalWidgetStack(this);
	g->addWidget(m_pWidgetStack,0,0);
	m_pTabBar = new QTabBar(this);
	m_pTabBar->setShape(QTabBar::TriangularBelow);
	connect(m_pTabBar,SIGNAL(selected(int)),this,SLOT(tabSelected(int)));
	g->addWidget(m_pTabBar,1,0);

	g->setRowStretch(0,1);

	g->addRowSpacing(2,15);

	m_pCloseButton = new QPushButton(__tr2qs("Close"),this);
	connect(m_pCloseButton,SIGNAL(clicked()),this,SLOT(die()));
	g->addWidget(m_pCloseButton,3,0);

	setMinimumSize(300,200);
	setMaximumSize(780,580);
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CTCPREQUESTREPLIED)));

	setCaption(__tr2qs("CTCP Page - KVIrc"));
}

KviCtcpPageDialog::~KviCtcpPageDialog()
{
	g_pCtcpPageDialog = 0;
}

void KviCtcpPageDialog::center()
{
	move((g_pApp->desktop()->width() - width()) >> 1,
		(g_pApp->desktop()->height() - height()) >> 1);
}

void KviCtcpPageDialog::die()
{
	delete this;
}

void KviCtcpPageDialog::tabSelected(int id)
{
	m_pWidgetStack->raiseWidget(id);
}

void KviCtcpPageDialog::addPage(const QString &szNick,const QString &szUser,const QString &szHost,const QString &szMsg)
{
#ifdef COMPILE_USE_QT4
	int id = m_pTabBar->addTab(szNick);
#else
	int id = m_pTabBar->addTab(new QTab(szNick));
#endif
	QLabel * l = new QLabel(this);
	l->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
	//l->setMaximumWidth(600);
	QString date = QDateTime::currentDateTime().toString();

	QString tmp = "<center>";
	tmp += __tr2qs("You have been paged by");
	tmp += "<br><b>";
	tmp += szNick;
	tmp += " [";
	tmp += szUser;
	tmp += "@";
	tmp += szHost;
	tmp += "]</b>:<br><br><b>";
	tmp += szMsg;
	tmp += "</b><br><br>[";
	tmp += date;
	tmp += "]</center>";

	l->setText(tmp);
	m_pWidgetStack->addWidget(l,id);
	m_pWidgetStack->raiseWidget(l);
	m_pTabBar->setCurrentTab(id);
}

void KviCtcpPageDialog::closeEvent(QCloseEvent *)
{
	delete this;
}

void KviCtcpPageDialog::popup()
{
	show();
//	raise();
//	setActiveWindow();
	m_pCloseButton->setFocus();
}

void KviCtcpPageDialog::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);
	center();
}