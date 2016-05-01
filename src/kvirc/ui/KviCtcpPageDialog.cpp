//=============================================================================
//
//   File : KviCtcpPageDialog.cpp
//   Creation date : Tue May 21 2002 22:09:45 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviCtcpPageDialog.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviIconManager.h"
#include "KviOptions.h"

#include <QDateTime>
#include <QLayout>
#include <QLabel>
#include <QPixmap>
#include <QDesktopWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QTabWidget>
#include <QLocale>

// KviApplication.cpp
extern KVIRC_API KviCtcpPageDialog * g_pCtcpPageDialog;

KviCtcpPageDialog::KviCtcpPageDialog()
    : QWidget(nullptr, Qt::WindowStaysOnTopHint | Qt::Tool | Qt::Dialog | Qt::Window)
{
	setObjectName("kvirc_ctcppage_dialog");
	setWindowTitle(__tr2qs("CTCP Page - KVIrc"));

	QGridLayout * g = new QGridLayout(this);
	m_pTabBar = new QTabWidget(this);
	m_pTabBar->setTabShape(QTabWidget::Triangular);
	g->addWidget(m_pTabBar, 0, 0);

	g->setRowStretch(0, 1);

	m_pCloseButton = new QPushButton(__tr2qs("Close"), this);
	connect(m_pCloseButton, SIGNAL(clicked()), this, SLOT(die()));
	g->addWidget(m_pCloseButton, 1, 0);

	setMinimumSize(300, 200);
	setMaximumSize(780, 580);
	setWindowIcon(*(g_pIconManager->getSmallIcon(KviIconManager::CtcpRequestReplied)));
}

KviCtcpPageDialog::~KviCtcpPageDialog()
{
	g_pCtcpPageDialog = nullptr;
}

void KviCtcpPageDialog::center()
{
	QRect rect = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());
	move((rect.width() - width()) / 2, (rect.height() - height()) / 2);
}

void KviCtcpPageDialog::die()
{
	delete this;
}

void KviCtcpPageDialog::addPage(const QString & szNick, const QString & szUser, const QString & szHost, const QString & szMsg)
{
	QLabel * l = new QLabel(this);
	QString szDate;
	QDateTime date = QDateTime::currentDateTime();
	switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
	{
		case 0:
			// this is the equivalent to an empty date.toString() call, but it's needed
			// to ensure qt4 will use the default() locale and not the system() one
			szDate = QLocale().toString(date, "ddd MMM d hh:mm:ss yyyy");
			break;
		case 1:
			szDate = date.toString(Qt::ISODate);
			break;
		case 2:
			szDate = date.toString(Qt::SystemLocaleShortDate);
			break;
	}

	QString tmp = "";
	tmp += __tr2qs("You have been paged by");
	tmp += ":<br><b>";
	tmp += szNick;
	tmp += " [";
	tmp += szUser;
	tmp += "@";
	tmp += szHost;
	tmp += "]</b><br><br><b>";
	tmp += szMsg;
	tmp += "</b><br><br>[";
	tmp += szDate;
	tmp += "]";

	l->setText(tmp);

	int id = m_pTabBar->addTab(l, szNick);
	m_pTabBar->setCurrentIndex(id);
}

void KviCtcpPageDialog::closeEvent(QCloseEvent *)
{
	deleteLater();
}

void KviCtcpPageDialog::popup()
{
	show();
	raise();
	activateWindow();
	m_pCloseButton->setFocus();
}

void KviCtcpPageDialog::showEvent(QShowEvent * e)
{
	QWidget::showEvent(e);
	center();
}
