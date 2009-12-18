//=============================================================================
//
//   File : kvi_ctcppagedialog.cpp
//   Creation date : Tue May 21 2002 22:09:45 by Szymon Stefanek
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================


#include "kvi_ctcppagedialog.h"
#include "kvi_locale.h"
#include "kvi_app.h"
#include "kvi_iconmanager.h"
#include "kvi_options.h"

#include <QDateTime>
#include <QLayout>
#include <QLabel>
#include <QPixmap>
#include <QDesktopWidget>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QPushButton>
#include <QTabWidget>

// kvi_app.cpp
extern KVIRC_API KviCtcpPageDialog * g_pCtcpPageDialog;

KviCtcpPageDialog::KviCtcpPageDialog()
: QWidget(0,Qt::WindowStaysOnTopHint | Qt::Tool | Qt::Dialog | Qt::Window)
{
	setObjectName("kvirc_ctcppage_dialog");
	setWindowTitle(__tr2qs("CTCP Page - KVIrc"));

	QGridLayout * g = new QGridLayout(this);
	m_pTabBar = new QTabWidget(this);
	m_pTabBar->setTabShape(QTabWidget::Triangular);
	g->addWidget(m_pTabBar,1,0);

	g->setRowStretch(0,1);
	g->addItem(new QSpacerItem(0, 15), 2, 0);

	m_pCloseButton = new QPushButton(__tr2qs("Close"),this);
	connect(m_pCloseButton,SIGNAL(clicked()),this,SLOT(die()));
	g->addWidget(m_pCloseButton,3,0);

	setMinimumSize(300,200);
	setMaximumSize(780,580);
	setWindowIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CTCPREQUESTREPLIED)));
}

KviCtcpPageDialog::~KviCtcpPageDialog()
{
	g_pCtcpPageDialog = 0;
}

void KviCtcpPageDialog::center()
{
	QRect rect = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());
	move((rect.width() - width())/2,(rect.height() - height())/2);
}

void KviCtcpPageDialog::die()
{
	delete this;
}

void KviCtcpPageDialog::addPage(const QString &szNick,const QString &szUser,const QString &szHost,const QString &szMsg)
{
	QLabel * l = new QLabel(this);
	l->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
	//l->setMaximumWidth(600);
	QString szDate;
	QDateTime date = QDateTime::currentDateTime();
	switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
	{
		case 0:
			szDate = date.toString();
			break;
		case 1:
			szDate = date.toString(Qt::ISODate);
			break;
		case 2:
			szDate = date.toString(Qt::SystemLocaleDate);
			break;
	}

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
	tmp += szDate;
	tmp += "]</center>";

	l->setText(tmp);

	int id = m_pTabBar->addTab(l, szNick);
	m_pTabBar->setCurrentIndex(id);
}

void KviCtcpPageDialog::closeEvent(QCloseEvent *)
{
	delete this;
}

void KviCtcpPageDialog::popup()
{
	show();
	raise();
	activateWindow();
	m_pCloseButton->setFocus();
}

void KviCtcpPageDialog::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);
	center();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_ctcppagedialog.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
