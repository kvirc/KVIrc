//=============================================================================
//
//   File : kvi_tal_tabdialog.cpp
//   Creation date : Tue Feb 06 2007 14:35:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#define __KVILIB__
#include "kvi_tal_tabdialog.h"
#include "kvi_tal_hbox.h"

#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>

KviTalTabDialog::KviTalTabDialog(QWidget * pParent,const char * name,bool bModal)
: QDialog(pParent)
{
	setObjectName(name);
	setModal(bModal);

	m_pLayout = new QVBoxLayout(this);
	setLayout(m_pLayout);

	m_pTabWidget = new QTabWidget(this);
	m_pLayout->addWidget(m_pTabWidget);

	m_pButtons = new KviTalHBox(this);
	m_pButtons->setAlignment(Qt::AlignRight);
	m_pLayout->addWidget(m_pButtons);
}

KviTalTabDialog::~KviTalTabDialog()
{
}

void KviTalTabDialog::addTab(QWidget * name, const QString & label)
{
	m_pTabWidget->addTab(name,label);
}

void KviTalTabDialog::setOkButton(const QString & text)
{
	QPushButton * pBtnOk = new QPushButton(text,(QWidget *)m_pButtons);
	pBtnOk->setFixedSize(100,30);
	connect(pBtnOk,SIGNAL(released()),this,SLOT(accept()));
}

void KviTalTabDialog::setCancelButton(const QString & text)
{
	QPushButton * pBtnCancel = new QPushButton(text,(QWidget *)m_pButtons);
	pBtnCancel->setFixedSize(100,30);
	connect(pBtnCancel,SIGNAL(released()),this,SLOT(reject()));
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_tabdialog.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

