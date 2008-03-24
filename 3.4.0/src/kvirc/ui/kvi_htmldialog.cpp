//=============================================================================
//
//   File : kvi_htmldialog.cpp
//   Created on Wed 03 Jan 2007 03:36:36 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007 Szymon Stefanek <pragma at kvirc dot net>
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
#define __KVIRC__

#include "kvi_htmldialog.h"
#include "kvi_locale.h"
#include "kvi_tal_textedit.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtextbrowser.h>

KviHtmlDialog::KviHtmlDialog(QWidget * pParent,KviHtmlDialogData * pData)
: QDialog(pParent)
{
	m_pData = pData;

	if(pData->szCaption.isEmpty())
		setCaption("KVIrc");
	else
		setCaption(pData->szCaption);
		
	if(!pData->pixIcon.isNull())
		setIcon(pData->pixIcon);

	QGridLayout * g = new QGridLayout(this,4,3,7,7);
	
	QLabel * l;
	QTextBrowser * te;
	QPushButton * pb;
	
	int iUp = 0;
	int iDown = 2;

	if(!pData->szUpperLabelText.isEmpty())
	{
		l = new QLabel(this);
		l->setText(pData->szUpperLabelText);
		g->addMultiCellWidget(l,0,0,0,2);
		iUp = 1;
	}

	if(!pData->szLowerLabelText.isEmpty())
	{
		l = new QLabel(this);
		l->setText(pData->szLowerLabelText);
		g->addMultiCellWidget(l,2,2,0,2);
		iDown = 1;
	}

	te = new QTextBrowser(this);
	te->setText(pData->szHtmlText);
	//te->setReadOnly(true);

	if(pData->iFlags & KviHtmlDialogData::ForceMinimumSize)
		te->setMinimumSize(pData->iMinimumWidth,pData->iMinimumHeight);

	//te->setReadOnly(true);
	g->addMultiCellWidget(te,iUp,iDown,0,2);

	int iButtons = pData->szButton3Text.isEmpty() ? (pData->szButton2Text.isEmpty() ? 1 : 2) : 3;
	if(pData->iCancelButton > iButtons)pData->iCancelButton = iButtons;
	if(pData->iDefaultButton > iButtons)pData->iDefaultButton = iButtons;

	pb = new QPushButton(this);
	pb->setText(pData->szButton1Text.isEmpty() ? __tr2qs("OK") : pData->szButton1Text);
	pb->setDefault(pData->iDefaultButton == 1);
	int iCoord = iButtons == 1 ? 1 : 0;
	g->addWidget(pb,3,iCoord);
	connect(pb,SIGNAL(clicked()),this,SLOT(button1Pressed()));

	if(!pData->szButton2Text.isEmpty())
	{
		pb = new QPushButton(this);
		pb->setText(pData->szButton2Text);
		pb->setDefault(pData->iDefaultButton == 2);
		iCoord = iButtons == 2 ? 2 : 1;
		g->addWidget(pb,3,iCoord);
		connect(pb,SIGNAL(clicked()),this,SLOT(button2Pressed()));

		if(!pData->szButton3Text.isEmpty())
		{
			pb = new QPushButton(this);
			pb->setText(pData->szButton3Text);
			pb->setDefault(pData->iDefaultButton == 3);
			g->addWidget(pb,3,2);
			connect(pb,SIGNAL(clicked()),this,SLOT(button3Pressed()));
		}
	}

	g->setRowStretch(1,1);

	m_pData->iSelectedButton = m_pData->iDefaultButton;
}

KviHtmlDialog::~KviHtmlDialog()
{
	
}

void KviHtmlDialog::button1Pressed()
{
	m_pData->iSelectedButton = 1;
	accept();
}

void KviHtmlDialog::button2Pressed()
{
	m_pData->iSelectedButton = 2;
	accept();
}

void KviHtmlDialog::button3Pressed()
{
	m_pData->iSelectedButton = 3;
	accept();
}

void KviHtmlDialog::reject()
{
	m_pData->iSelectedButton = m_pData->iCancelButton;
	QDialog::reject();
}

int KviHtmlDialog::display(QWidget * pParent,KviHtmlDialogData * pData)
{
	KviHtmlDialog * pDialog = new KviHtmlDialog(pParent,pData);
	pDialog->exec();
	delete pDialog;
	return pData->iSelectedButton;
}


