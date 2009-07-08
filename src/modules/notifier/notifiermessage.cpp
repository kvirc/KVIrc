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

#include "kvi_mirccntrl.h"
#include "kvi_texticonmanager.h"
#include "kvi_options.h"

#include <QRect>
KviNotifierMessage::KviNotifierMessage(QPixmap * pPixmap, const QString &szText)
{
	m_szText=szText;
	m_pPixmap=pPixmap;

	m_pLabel0 = new QLabel();
	m_pLabel0->setFixedSize(16,16);
	if(m_pPixmap)
		m_pLabel0->setPixmap(*m_pPixmap);

	m_pLabel1 = new QLabel(KviNotifierMessage::convertToHtml(m_szText));
	m_pLabel1->setWordWrap(true);

	m_pHBox = new QHBoxLayout(this);
	m_pHBox->setSpacing(SPACING);
	m_pHBox->setMargin(SPACING);

	m_pHBox->addWidget(m_pLabel0);
	m_pHBox->addWidget(m_pLabel1);
}

KviNotifierMessage::~KviNotifierMessage()
{
	if(m_pLabel0)
		delete m_pLabel0;
	if(m_pLabel1)
		delete m_pLabel1;
	if(m_pHBox)
		delete m_pHBox;
}

//TODO merge this code with kvi_topicw.cpp's one
#define KVI_LABEL_DEF_BACK 100
#define KVI_LABEL_DEF_FORE 101

QString KviNotifierMessage::convertToHtml(const QString &text)
{
	QString result;
	bool curBold      = false;
	bool curUnderline = false;
	unsigned char curFore      = KVI_LABEL_DEF_FORE; //default fore
	unsigned char curBack      = KVI_LABEL_DEF_BACK; //default back

	unsigned int idx = 0;

	while(idx < (unsigned int)text.length())
	{
		unsigned short c = text[(int)idx].unicode();

		unsigned int start = idx;

		while(
				(c != KVI_TEXT_COLOR) &&
				(c != KVI_TEXT_BOLD) &&
				(c != KVI_TEXT_UNDERLINE) &&
				(c != KVI_TEXT_REVERSE) &&
				(c != KVI_TEXT_RESET) &&
				(c != KVI_TEXT_ICON)
			)
		{
			idx++;
			if(idx >= (unsigned int)text.length())break;
			else c = text[(int)idx].unicode();
		}

		int len = idx - start;

		if(len > 0)
		{
			bool bOpened = FALSE;
			QString szText = text.mid(start,len);

			if(curBold) result.append("<b>");
			if(curUnderline) result.append("<u>");

			if(curFore != KVI_LABEL_DEF_FORE)
			{
				result.append("<font color=\"");
				result.append(KVI_OPTION_MIRCCOLOR(curFore).name());
				result.append('"');
				bOpened = TRUE;
			}

/*			if(curBack != KVI_LABEL_DEF_BACK)
			{
				if(!bOpened)
					result.append("<font bgcolor=");
				else
					result.append(" bgcolor=");
				result.append(KVI_OPTION_MIRCCOLOR(curBack).name());
			}*/

			if(bOpened) result.append(">");

			result.append(text.mid(start,len));

			if( (curFore != KVI_LABEL_DEF_FORE) /*|| (curBack != KVI_LABEL_DEF_BACK)*/ )
				result.append("</font>");

			if(curUnderline) result.append("</u>");
			if(curBold) result.append("</b>");

		}

		switch(c)
		{
			case KVI_TEXT_BOLD: curBold = !curBold; ++idx; break;
			case KVI_TEXT_UNDERLINE: curUnderline = !curUnderline; ++idx; break;
			case KVI_TEXT_REVERSE:
				{
					char auxBack = curBack;
					curBack = curFore;
					curFore = auxBack;
				}
				++idx;
			break;
			case KVI_TEXT_RESET:
				curFore = KVI_LABEL_DEF_FORE;
				curBack = KVI_LABEL_DEF_BACK;
				curBold = false;
				curUnderline = false;
				++idx;
			break;
			case KVI_TEXT_COLOR:
			{
				++idx;
				unsigned char fore;
				unsigned char back;
				idx = getUnicodeColorBytes(text,idx,&fore,&back);
				if(fore != KVI_NOCHANGE)
				{
					curFore = fore;
					if(back != KVI_NOCHANGE)curBack = back;
				} else {
					// only a CTRL+K
					curBack = KVI_LABEL_DEF_BACK;
					curFore = KVI_LABEL_DEF_FORE;
				}
			}
			break;
			case KVI_TEXT_ICON:
			{
				++idx;

				unsigned int icoStart = idx;
				while((idx < (unsigned int)text.length()) && (text[(int)idx].unicode() > 32))idx++;

				KviStr lookupString = text.mid(icoStart,idx - icoStart);

				KviTextIcon * icon = g_pTextIconManager->lookupTextIcon(lookupString.ptr());
				if(icon)
				{
					//TODO: icons
/*					QPixmap * pigzmap = icon->pixmap();
					p->drawPixmap(curX,(baseline + 2) - pigzmap->height(),*(pigzmap));
					curX += pigzmap->width();*/
				} else {
					idx = icoStart;
				}
			}
			break;
		}
	}
	return result;
}

