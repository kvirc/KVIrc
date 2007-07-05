//
//   File : optw_ircview.cpp
//   Creation date : Sat Aug 11 2001 03:29:52 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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
#include "optw_ircview.h"

#include <qlayout.h>
#include "kvi_options.h"
#include "kvi_locale.h"

//#warning "Info tips"

KviIrcViewThemeOptionsWidget::KviIrcViewThemeOptionsWidget(QWidget * pParent)
: KviOptionsWidget(pParent)
{
}

KviIrcViewThemeOptionsWidget::~KviIrcViewThemeOptionsWidget()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviIrcViewLookOptionsWidget::KviIrcViewLookOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"ircviewlook_options_widget")
{
	createLayout(6,2);

	addFontSelector(0,0,1,0,__tr2qs_ctx("Font:","options"),KviOption_fontIrcView);
	addColorSelector(0,1,1,1,__tr2qs_ctx("Background color:","options"),KviOption_colorIrcViewBackground);
	
	addPixmapSelector(0,2,1,2,__tr2qs_ctx("Background image:","options"),KviOption_pixmapIrcViewBackground);

	addLabel(0,3,0,3,__tr2qs_ctx("Horizontal align:","options"));
	m_pHorizontalAlign=new QComboBox(this);
	addWidgetToLayout(m_pHorizontalAlign,1,3,1,3);
	
	addLabel(0,4,0,4,__tr2qs_ctx("Vertical align:","options"));
	m_pVerticalAlign=new QComboBox(this);
	addWidgetToLayout(m_pVerticalAlign,1,4,1,4);
		
	addColorSelector(0,5,1,5,__tr2qs_ctx("Mark Line:","options"),KviOption_colorIrcViewMarkLine);
	
	m_pHorizontalAlign->insertItem(__tr2qs_ctx("Tile","options"));
	m_pHorizontalAlign->insertItem(__tr2qs_ctx("Left","options"));
	m_pHorizontalAlign->insertItem(__tr2qs_ctx("Right","options"));
	m_pHorizontalAlign->insertItem(__tr2qs_ctx("Center","options"));
	
	m_pVerticalAlign->insertItem(__tr2qs_ctx("Tile","options"));
	m_pVerticalAlign->insertItem(__tr2qs_ctx("Top","options"));
	m_pVerticalAlign->insertItem(__tr2qs_ctx("Bottom","options"));
	m_pVerticalAlign->insertItem(__tr2qs_ctx("Center","options"));
	
	switch( KVI_OPTION_UINT(KviOption_uintIrcViewPixmapAlign) & Qt::AlignHorizontal_Mask)
	{
		case Qt::AlignLeft:
			m_pHorizontalAlign->setCurrentItem(1);
			break;
		case Qt::AlignRight:
			m_pHorizontalAlign->setCurrentItem(2);
			break;
		case Qt::AlignHCenter:
			m_pHorizontalAlign->setCurrentItem(3);
			break;
		default:
			m_pHorizontalAlign->setCurrentItem(0);
	}
	
	switch( KVI_OPTION_UINT(KviOption_uintIrcViewPixmapAlign) & Qt::AlignVertical_Mask)
	{
		case Qt::AlignTop:
			m_pVerticalAlign->setCurrentItem(1);
			break;
		case Qt::AlignBottom:
			m_pVerticalAlign->setCurrentItem(2);
			break;
		case Qt::AlignVCenter:
			m_pVerticalAlign->setCurrentItem(3);
			break;
		default:
			m_pVerticalAlign->setCurrentItem(0);
	}
	
	layout()->setRowStretch(2,1);
}

KviIrcViewLookOptionsWidget::~KviIrcViewLookOptionsWidget()
{
}

void KviIrcViewLookOptionsWidget::commit()
{
	int iFlags=0;
	switch(m_pHorizontalAlign->currentItem())
	{
		case 1:
			iFlags|=Qt::AlignLeft;
			break;
		case 2:
			iFlags|=Qt::AlignRight;
			break;
		case 3:
			iFlags|=Qt::AlignHCenter;
			break;
	}
	switch(m_pVerticalAlign->currentItem())
	{
		case 1:
			iFlags|=Qt::AlignTop;
			break;
		case 2:
			iFlags|=Qt::AlignBottom;
			break;
		case 3:
			iFlags|=Qt::AlignVCenter;
			break;
	}
	
	KVI_OPTION_UINT(KviOption_uintIrcViewPixmapAlign)=iFlags;
	KviOptionsWidget::commit();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviIrcViewFeaturesOptionsWidget::KviIrcViewFeaturesOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"ircviewfeatures_options_widget")
{
	createLayout(14,1);

	

	addBoolSelector(0,7,0,7,__tr2qs_ctx("Enable URL highlighting","options"),KviOption_boolIrcViewUrlHighlighting);
	addBoolSelector(0,8,0,8,__tr2qs_ctx("Use line wrap margin","options"),KviOption_boolIrcViewWrapMargin);
	KviUIntSelector * s = addUIntSelector(0,9,0,9,__tr2qs_ctx("Maximum buffer size:","options"),KviOption_uintIrcViewMaxBufferSize,32,32767,2048);
	s->setSuffix(__tr2qs_ctx(" lines","options"));
	s = addUIntSelector(0,10,0,10,__tr2qs_ctx("Link tooltip show delay:","options"),KviOption_uintIrcViewToolTipTimeoutInMsec,256,10000,1800);
	s->setSuffix(__tr2qs_ctx(" msec","options"));
	s = addUIntSelector(0,11,0,11,__tr2qs_ctx("Link tooltip hide delay:","options"),KviOption_uintIrcViewToolTipHideTimeoutInMsec,256,10000,12000);
	s->setSuffix(__tr2qs_ctx(" msec","options"));
	addBoolSelector(0,12,0,12,__tr2qs_ctx("Track last read text line","options"),KviOption_boolTrackLastReadTextViewLine);
	addRowSpacer(0,13,0,13);

}

KviIrcViewFeaturesOptionsWidget::~KviIrcViewFeaturesOptionsWidget()
{
}



