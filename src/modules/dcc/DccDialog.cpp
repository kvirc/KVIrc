//=============================================================================
//
//   File : DccDialog.cpp
//   Creation date : Tue Sep 19 09 2000 15:23:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "DccDialog.h"

#include "KviLocale.h"
#include "KviIconManager.h"
#include "KviApplication.h"
#include "KviMainWindow.h"

#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QStringList>
#include <QDesktopWidget>
#include <QEvent>
#include <QCloseEvent>
#include <QShowEvent>
#include <QIcon>

DccDialog::DccDialog(DccBroker * br, DccDescriptor * dcc)
{
	m_pDescriptor = dcc;
	m_pBroker = br;
}

DccDialog::~DccDialog()
{
	if(m_pDescriptor)
		delete m_pDescriptor;
	m_pDescriptor = nullptr;
	m_pBroker->unregisterDccBox(this);
}

void DccDialog::forgetDescriptor()
{
	m_pDescriptor = nullptr;
}

DccAcceptDialog::DccAcceptDialog(DccBroker * br, DccDescriptor * dcc, const QString & text, const QString & capt)
    : QWidget(nullptr), DccDialog(br, dcc)
{
	setObjectName("dcc_accept_box");
	//QVBoxLayout * vb = new QVBoxLayout(this,4,4);
	QVBoxLayout * vb = new QVBoxLayout(this);
	vb->setMargin(4);
	vb->setSpacing(4);
	QLabel * l = new QLabel(text, this);
	l->setWordWrap(true);
	vb->addWidget(l);

	//QHBoxLayout *hb = new QHBoxLayout(4);
	QHBoxLayout * hb = new QHBoxLayout();
	hb->setSpacing(4);
	vb->addLayout(hb, Qt::AlignCenter);
	QPushButton * btn = new QPushButton(__tr2qs_ctx("&Accept", "dcc"), this);
	btn->setDefault(true);
	//btn->setFocus();
	hb->addWidget(btn);
	connect(btn, SIGNAL(clicked()), this, SLOT(acceptClicked()));
	btn = new QPushButton(__tr2qs_ctx("&Reject", "dcc"), this);
	connect(btn, SIGNAL(clicked()), this, SLOT(rejectClicked()));
	hb->addWidget(btn);

	setWindowIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::DccMsg))));
	setWindowTitle(capt);

	l->activateWindow();
	l->setFocus();
}

DccAcceptDialog::~DccAcceptDialog()
    = default;

void DccAcceptDialog::acceptClicked()
{
	hide();
	emit accepted(this, m_pDescriptor);
	this->deleteLater();
	//g_pApp->collectGarbage(this);
}

void DccAcceptDialog::rejectClicked()
{
	hide();
	emit rejected(this, m_pDescriptor);
	this->deleteLater();
	//g_pApp->collectGarbage(this);
}

void DccAcceptDialog::closeEvent(QCloseEvent * e)
{
	hide();
	e->ignore();
	emit rejected(this, m_pDescriptor);
	this->deleteLater();
	//g_pApp->collectGarbage(this);
}

void DccAcceptDialog::showEvent(QShowEvent * e)
{
	int iScreen = g_pApp->desktop()->screenNumber(g_pMainWindow);
	if(iScreen < 0)
		iScreen = g_pApp->desktop()->primaryScreen();
	QRect rect = g_pApp->desktop()->screenGeometry(iScreen);
	move(rect.x() + ((rect.width() - width()) / 2),rect.y() + ((rect.height() - height()) / 2));
	QWidget::showEvent(e);
}

DccRenameDialog::DccRenameDialog(DccBroker * br, DccDescriptor * dcc, const QString & text, bool bDisableResume)
    : QWidget(nullptr), DccDialog(br, dcc)
{
	setObjectName("dcc_rename_box");
	//QVBoxLayout * vb = new QVBoxLayout(this,4,4);
	QVBoxLayout * vb = new QVBoxLayout(this);
	vb->setMargin(4);
	vb->setSpacing(4);
	QLabel * l = new QLabel(text, this);
	l->setWordWrap(true);
	vb->addWidget(l);

	//QHBoxLayout *hb = new QHBoxLayout(4);
	QHBoxLayout * hb = new QHBoxLayout();
	hb->setSpacing(4);
	vb->addLayout(hb, Qt::AlignCenter);

	QPushButton * btn = new QPushButton(__tr2qs_ctx("&Rename", "dcc"), this);
	hb->addWidget(btn);
	connect(btn, SIGNAL(clicked()), this, SLOT(renameClicked()));

	btn = new QPushButton(__tr2qs_ctx("Over&write", "dcc"), this);
	hb->addWidget(btn);
	connect(btn, SIGNAL(clicked()), this, SLOT(overwriteClicked()));

	btn = new QPushButton(__tr2qs_ctx("Re&sume", "dcc"), this);
	hb->addWidget(btn);
	connect(btn, SIGNAL(clicked()), this, SLOT(resumeClicked()));
	if(bDisableResume)
		btn->setEnabled(false);

	btn = new QPushButton(__tr2qs_ctx("Cancel", "dcc"), this);
	hb->addWidget(btn);
	connect(btn, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	btn->setDefault(true);
	//btn->setFocus();

	setWindowIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::DccMsg))));
	setWindowTitle(__tr2qs_ctx("File Already Exists - KVIrc", "dcc"));
}

DccRenameDialog::~DccRenameDialog()
    = default;

void DccRenameDialog::closeEvent(QCloseEvent * e)
{
	hide();
	e->ignore();
	if(m_pDescriptor)
	{
		emit cancelSelected(this, m_pDescriptor);
		//g_pApp->collectGarbage(this);
		this->deleteLater();
	}
}

void DccRenameDialog::showEvent(QShowEvent * e)
{
	int iScreen = g_pApp->desktop()->screenNumber(g_pMainWindow);
	if(iScreen < 0)
		iScreen = g_pApp->desktop()->primaryScreen();
	QRect rect = g_pApp->desktop()->screenGeometry(iScreen);
	move(rect.x() + ((rect.width() - width()) / 2),rect.y() + ((rect.height() - height()) / 2));
	QWidget::showEvent(e);
}

void DccRenameDialog::renameClicked()
{
	hide();
	emit renameSelected(this, m_pDescriptor);
	//g_pApp->collectGarbage(this);
	this->deleteLater();
}

void DccRenameDialog::overwriteClicked()
{
	hide();
	emit overwriteSelected(this, m_pDescriptor);
	//g_pApp->collectGarbage(this);
	this->deleteLater();
}

void DccRenameDialog::resumeClicked()
{
	hide();
	m_pDescriptor->bResume = true;
	emit overwriteSelected(this, m_pDescriptor);
	this->deleteLater();
	//g_pApp->collectGarbage(this);
}

void DccRenameDialog::cancelClicked()
{
	hide();
	emit cancelSelected(this, m_pDescriptor);
	this->deleteLater();
	//g_pApp->collectGarbage(this);
}
