//
//   File : dialogs.cpp
//   Creation date : Tue Sep 19 09 2000 15:23:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
#include "dialogs.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_app.h"

#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QStringList>
#include <QDesktopWidget>
#include <QEvent>
#include <QCloseEvent>
#include <QShowEvent>

KviDccBox::KviDccBox(KviDccBroker * br,KviDccDescriptor * dcc)
{
	m_pDescriptor = dcc;
	m_pBroker = br;
}

KviDccBox::~KviDccBox()
{
	if(m_pDescriptor)delete m_pDescriptor;
	m_pDescriptor = 0;
	m_pBroker->unregisterDccBox(this);
}

void KviDccBox::forgetDescriptor()
{
	m_pDescriptor = 0;
}

KviDccAcceptBox::KviDccAcceptBox(KviDccBroker * br,KviDccDescriptor * dcc,const QString &text,const QString &capt)
: QWidget(0,"dcc_accept_box") , KviDccBox(br,dcc)
{
	QVBoxLayout * vb = new QVBoxLayout(this,4,4);
	QLabel * l = new QLabel(text,this);
	l->setWordWrap(true);
	vb->addWidget(l);

	QHBoxLayout *hb = new QHBoxLayout(4);
	vb->addLayout(hb,Qt::AlignCenter);
	QPushButton * btn = new QPushButton(__tr2qs_ctx("&Accept","dcc"),this);
	btn->setDefault(true);
	//btn->setFocus();
	hb->addWidget(btn);
	connect(btn,SIGNAL(clicked()),this,SLOT(acceptClicked()));
	btn = new QPushButton(__tr2qs_ctx("&Reject","dcc"),this);
	connect(btn,SIGNAL(clicked()),this,SLOT(rejectClicked()));
	hb->addWidget(btn);

	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DCCMSG)));
	setWindowTitle(capt);

	l->setActiveWindow();
	l->setFocus();
}

KviDccAcceptBox::~KviDccAcceptBox()
{
}

void KviDccAcceptBox::acceptClicked()
{
	hide();
	emit accepted(this,m_pDescriptor);
	g_pApp->collectGarbage(this);
}

void KviDccAcceptBox::rejectClicked()
{
	hide();
	emit rejected(this,m_pDescriptor);
	g_pApp->collectGarbage(this);
}

void KviDccAcceptBox::closeEvent(QCloseEvent *e)
{
	hide();
	e->ignore();
	emit rejected(this,m_pDescriptor);
	g_pApp->collectGarbage(this);
}

void KviDccAcceptBox::showEvent(QShowEvent *e)
{
	move((g_pApp->desktop()->width() - width()) >> 1,
		(g_pApp->desktop()->height() - height()) >> 1);
	QWidget::showEvent(e);
}


KviDccRenameBox::KviDccRenameBox(KviDccBroker * br,KviDccDescriptor * dcc,const QString &text,bool bDisableResume)
: QWidget(0,"dcc_rename_box") , KviDccBox(br,dcc)
{
	QVBoxLayout * vb = new QVBoxLayout(this,4,4);
	QLabel * l = new QLabel(text,this);
	l->setWordWrap(true);
	vb->addWidget(l);

	QHBoxLayout *hb = new QHBoxLayout(4);
	vb->addLayout(hb,Qt::AlignCenter);	

	QPushButton * btn = new QPushButton(__tr2qs_ctx("&Rename","dcc"),this);
	hb->addWidget(btn);
	connect(btn,SIGNAL(clicked()),this,SLOT(renameClicked()));

	btn = new QPushButton(__tr2qs_ctx("Over&write","dcc"),this);
	hb->addWidget(btn);
	connect(btn,SIGNAL(clicked()),this,SLOT(overwriteClicked()));

	btn = new QPushButton(__tr2qs_ctx("Re&sume","dcc"),this);
	hb->addWidget(btn);
	connect(btn,SIGNAL(clicked()),this,SLOT(resumeClicked()));
	if(bDisableResume)btn->setEnabled(false);

	btn = new QPushButton(__tr2qs_ctx("Cancel","dcc"),this);
	hb->addWidget(btn);
	connect(btn,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	btn->setDefault(true);
	//btn->setFocus();

	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DCCMSG)));
	setWindowTitle(__tr2qs_ctx("File Already Exists - KVIrc","dcc"));
}

KviDccRenameBox::~KviDccRenameBox()
{
}

void KviDccRenameBox::closeEvent(QCloseEvent *e)
{
	hide();
	e->ignore();
	if(m_pDescriptor)
	{
		emit cancelSelected(this,m_pDescriptor);
		g_pApp->collectGarbage(this);
	}
}

void KviDccRenameBox::showEvent(QShowEvent *e)
{
	move((g_pApp->desktop()->width() - width()) >> 1,
		(g_pApp->desktop()->height() - height()) >> 1);
	QWidget::showEvent(e);
}

void KviDccRenameBox::renameClicked()
{
	hide();
	emit renameSelected(this,m_pDescriptor);
	g_pApp->collectGarbage(this);
}

void KviDccRenameBox::overwriteClicked()
{
	hide();
	emit overwriteSelected(this,m_pDescriptor);
	g_pApp->collectGarbage(this);
}

void KviDccRenameBox::resumeClicked()
{
	hide();
	m_pDescriptor->bResume = true;
	emit overwriteSelected(this,m_pDescriptor);
	g_pApp->collectGarbage(this);
}

void KviDccRenameBox::cancelClicked()
{
	hide();
	emit cancelSelected(this,m_pDescriptor);
	g_pApp->collectGarbage(this);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_dialogs.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
