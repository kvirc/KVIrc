//=============================================================================
//
//   File : notifiermessage.cpp
//   Creation date : Mar 02 Nov 2004 02:41:18 by Iacopo Palazzi
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004-2008 Iacopo <iakko@siena.linux.it>
//   Copyright (C) 2009 Fabio Bas < ctrlaltca at gmail dot com >
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

#include "notifiermessage.h"
#include "notifiersettings.h"

#include "kvi_htmlgenerator.h"
#include "kvi_options.h"

#include <QRect>
#include <QResizeEvent>

KviNotifierMessage::KviNotifierMessage(QPixmap * pPixmap, const QString &szText)
{
	m_pLabel0 = 0;
	m_pLabel1 = 0;
	
	m_szText=szText;
	m_pPixmap=pPixmap;

	//QByteArray utf8 = szText.toUtf8();
	//if(utf8.data())
	//	qDebug("NOTIFIER TEXT MESSAGE: \n%s\n",utf8.data());

	m_pHBox = new QHBoxLayout(this);
	m_pHBox->setSpacing(SPACING);
	m_pHBox->setMargin(SPACING);

	updateGui();
}

KviNotifierMessage::~KviNotifierMessage()
{
	if(m_pLabel0)
		m_pLabel0->deleteLater();
	if(m_pLabel1)
		m_pLabel1->deleteLater();
	if(m_pHBox)
		m_pHBox->deleteLater();
}

void KviNotifierMessage::updateGui()
{
	bool bShowImages=KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages);

	if(m_pLabel0)
		delete m_pLabel0;

	if(m_pLabel1)
		delete m_pLabel1;

	if(bShowImages)
	{
		m_pLabel0 = new QLabel(this);
		m_pLabel0->setFixedSize(16,16);
		if(m_pPixmap)
			m_pLabel0->setPixmap(*m_pPixmap);
	} else {
		m_pLabel0 = 0;
	}

	m_pLabel1 = new QLabel(this);
	m_pLabel1->setTextFormat(Qt::RichText);
	m_pLabel1->setText(KviHtmlGenerator::convertToHtml(m_szText));
	m_pLabel1->setWordWrap(true);
	m_pLabel1->setFont(KVI_OPTION_FONT(KviOption_fontNotifier));
	QPalette pal = m_pLabel1->palette();
	pal.setColor(QPalette::WindowText, KVI_OPTION_COLOR(KviOption_colorNotifierForeground));
	m_pLabel1->setPalette(pal);	
	
	if(bShowImages)
	{
		m_pHBox->setStretch(1,99);
		m_pHBox->addWidget(m_pLabel0);
	}
	m_pHBox->addWidget(m_pLabel1);

}
