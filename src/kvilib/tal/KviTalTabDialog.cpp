//=============================================================================
//
//   File : KviTalTabDialog.cpp
//   Creation date : Tue Feb 06 2007 14:35:08 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#include "KviTalTabDialog.h"

#include <QDialog>
#include <QTabWidget>
#include <QGridLayout>
#include <QPushButton>

KviTalTabDialog::KviTalTabDialog(QWidget * pParent, const char * name, bool bModal)
    : QDialog(pParent)
{
	setObjectName(name);
	setModal(bModal);

	m_pLayout = new QGridLayout(this);
	m_pLayout->setColumnStretch(0, 10);
	setLayout(m_pLayout);

	m_pTabWidget = new QTabWidget(this);
	m_pLayout->addWidget(m_pTabWidget, 0, 0, 1, 3);
}

KviTalTabDialog::~KviTalTabDialog()
    = default;

void KviTalTabDialog::addTab(QWidget * widget, const QString & label)
{
	m_pTabWidget->addTab(widget, label);
}

void KviTalTabDialog::setOkButton(const QString & text)
{
	QPushButton * pBtnOk = new QPushButton(text, this);
	m_pLayout->addWidget(pBtnOk, 1, 1);
	connect(pBtnOk, SIGNAL(clicked()), this, SLOT(okPressed()));
}

void KviTalTabDialog::setCancelButton(const QString & text)
{
	QPushButton * pBtnCancel = new QPushButton(text, this);
	m_pLayout->addWidget(pBtnCancel, 1, 2);
	connect(pBtnCancel, SIGNAL(clicked()), this, SLOT(cancelPressed()));
}

void KviTalTabDialog::okPressed()
{
	emit applyButtonPressed();
}

void KviTalTabDialog::cancelPressed()
{
	emit cancelButtonPressed();
}
