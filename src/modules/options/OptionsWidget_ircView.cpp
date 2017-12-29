//=============================================================================
//
//   File : OptionsWidget_ircView.cpp
//   Creation date : Sat Aug 11 2001 03:29:52 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidget_ircView.h"

#include "KviOptions.h"
#include "KviLocale.h"

#include <QLayout>

OptionsWidget_ircViewTheme::OptionsWidget_ircViewTheme(QWidget * pParent)
    : KviOptionsWidget(pParent)
{
}

OptionsWidget_ircViewTheme::~OptionsWidget_ircViewTheme()
    = default;

OptionsWidget_ircViewLook::OptionsWidget_ircViewLook(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("ircviewlook_options_widget");
	createLayout();

	addFontSelector(0, 0, 1, 0, __tr2qs_ctx("Font:", "options"), KviOption_fontIrcView);
	addColorSelector(0, 1, 1, 1, __tr2qs_ctx("Background color:", "options"), KviOption_colorIrcViewBackground);

	addPixmapSelector(0, 2, 1, 2, __tr2qs_ctx("Background image:", "options"), KviOption_pixmapIrcViewBackground);

	addLabel(0, 3, 0, 3, __tr2qs_ctx("Horizontal align:", "options"));
	m_pHorizontalAlign = new QComboBox(this);
	addWidgetToLayout(m_pHorizontalAlign, 1, 3, 1, 3);

	addLabel(0, 4, 0, 4, __tr2qs_ctx("Vertical align:", "options"));
	m_pVerticalAlign = new QComboBox(this);
	addWidgetToLayout(m_pVerticalAlign, 1, 4, 1, 4);

	m_pHorizontalAlign->addItem(__tr2qs_ctx("Tile", "options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Left", "options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Right", "options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Center", "options"));

	m_pVerticalAlign->addItem(__tr2qs_ctx("Tile", "options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Top", "options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Bottom", "options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Center", "options"));

	switch(KVI_OPTION_UINT(KviOption_uintIrcViewPixmapAlign) & Qt::AlignHorizontal_Mask)
	{
		case Qt::AlignLeft:
			m_pHorizontalAlign->setCurrentIndex(1);
			break;
		case Qt::AlignRight:
			m_pHorizontalAlign->setCurrentIndex(2);
			break;
		case Qt::AlignHCenter:
			m_pHorizontalAlign->setCurrentIndex(3);
			break;
		default:
			m_pHorizontalAlign->setCurrentIndex(0);
	}

	switch(KVI_OPTION_UINT(KviOption_uintIrcViewPixmapAlign) & Qt::AlignVertical_Mask)
	{
		case Qt::AlignTop:
			m_pVerticalAlign->setCurrentIndex(1);
			break;
		case Qt::AlignBottom:
			m_pVerticalAlign->setCurrentIndex(2);
			break;
		case Qt::AlignVCenter:
			m_pVerticalAlign->setCurrentIndex(3);
			break;
		default:
			m_pVerticalAlign->setCurrentIndex(0);
	}

	layout()->setRowStretch(2, 1);
}

OptionsWidget_ircViewLook::~OptionsWidget_ircViewLook()
    = default;

void OptionsWidget_ircViewLook::commit()
{
	int iFlags = 0;
	switch(m_pHorizontalAlign->currentIndex())
	{
		case 1:
			iFlags |= Qt::AlignLeft;
			break;
		case 2:
			iFlags |= Qt::AlignRight;
			break;
		case 3:
			iFlags |= Qt::AlignHCenter;
			break;
	}
	switch(m_pVerticalAlign->currentIndex())
	{
		case 1:
			iFlags |= Qt::AlignTop;
			break;
		case 2:
			iFlags |= Qt::AlignBottom;
			break;
		case 3:
			iFlags |= Qt::AlignVCenter;
			break;
	}

	KVI_OPTION_UINT(KviOption_uintIrcViewPixmapAlign) = iFlags;
	KviOptionsWidget::commit();
}

OptionsWidget_ircViewFeatures::OptionsWidget_ircViewFeatures(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("ircviewfeatures_options_widget");
	createLayout();

	addBoolSelector(0, 7, 0, 7, __tr2qs_ctx("Enable URL highlighting", "options"), KviOption_boolIrcViewUrlHighlighting);
	addBoolSelector(0, 8, 0, 8, __tr2qs_ctx("Use line wrap margin", "options"), KviOption_boolIrcViewWrapMargin);
	KviUIntSelector * s = addUIntSelector(0, 9, 0, 9, __tr2qs_ctx("Maximum buffer size:", "options"), KviOption_uintIrcViewMaxBufferSize, 32, 32767, 2048);
	s->setSuffix(__tr2qs_ctx(" lines", "options"));
	s = addUIntSelector(0, 10, 0, 10, __tr2qs_ctx("Link tooltip show delay:", "options"), KviOption_uintIrcViewToolTipTimeoutInMsec, 256, 10000, 1800);
	s->setSuffix(__tr2qs_ctx(" msec", "options"));
	s = addUIntSelector(0, 11, 0, 11, __tr2qs_ctx("Link tooltip hide delay:", "options"), KviOption_uintIrcViewToolTipHideTimeoutInMsec, 256, 10000, 12000);
	s->setSuffix(__tr2qs_ctx(" msec", "options"));
	addBoolSelector(0, 12, 0, 12, __tr2qs_ctx("Enable animated smiles", "options"), KviOption_boolEnableAnimatedSmiles);

	KviTalGroupBox * pGroup = addGroupBox(0, 13, 0, 13, Qt::Horizontal, __tr2qs_ctx("Enable Tooltips for", "options"));
	addBoolSelector(pGroup, __tr2qs_ctx("URL links", "options"), KviOption_boolEnableUrlLinkToolTip);
	addBoolSelector(pGroup, __tr2qs_ctx("Host links", "options"), KviOption_boolEnableHostLinkToolTip);
	addBoolSelector(pGroup, __tr2qs_ctx("Server links", "options"), KviOption_boolEnableServerLinkToolTip);
	addBoolSelector(pGroup, __tr2qs_ctx("Mode links", "options"), KviOption_boolEnableModeLinkToolTip);
	addBoolSelector(pGroup, __tr2qs_ctx("Nick links", "options"), KviOption_boolEnableNickLinkToolTip);
	addBoolSelector(pGroup, __tr2qs_ctx("Channel links", "options"), KviOption_boolEnableChannelLinkToolTip);
	addBoolSelector(pGroup, __tr2qs_ctx("Escape sequences", "options"), KviOption_boolEnableEscapeLinkToolTip);

	addRowSpacer(0, 14, 0, 14);
}

OptionsWidget_ircViewFeatures::~OptionsWidget_ircViewFeatures()
    = default;

OptionsWidget_ircViewMarker::OptionsWidget_ircViewMarker(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("ircviewmarker_options_widget");
	createLayout();

	addBoolSelector(0, 0, 1, 0, __tr2qs_ctx("Track last read text line", "options"), KviOption_boolTrackLastReadTextViewLine);
	addColorSelector(0, 1, 1, 1, __tr2qs_ctx("Marker color:", "options"), KviOption_colorIrcViewMarkLine);

	KviUIntSelector * s = addUIntSelector(0, 2, 1, 2, __tr2qs_ctx("Marker size:", "options"), KviOption_uintIrcViewMarkerSize, 1, 5, 1);
	s->setSuffix(__tr2qs_ctx(" pixels", "options"));

	addLabel(0, 3, 0, 3, __tr2qs_ctx("Marker style:", "options"));
	m_pMarkerStyle = new QComboBox(this);
	addWidgetToLayout(m_pMarkerStyle, 1, 3, 1, 3);

	addRowSpacer(0, 4, 0, 4);

	m_pMarkerStyle->addItem(__tr2qs_ctx("DotLine", "options"));
	m_pMarkerStyle->addItem(__tr2qs_ctx("DashLine", "options"));
	m_pMarkerStyle->addItem(__tr2qs_ctx("SolidLine", "options"));
	m_pMarkerStyle->addItem(__tr2qs_ctx("DashDotLine", "options"));
	m_pMarkerStyle->addItem(__tr2qs_ctx("DashDotDotLine", "options"));

	unsigned int uStyle = KVI_OPTION_UINT(KviOption_uintIrcViewMarkerStyle);
	m_pMarkerStyle->setCurrentIndex(uStyle < 5 ? uStyle : 0);
}

OptionsWidget_ircViewMarker::~OptionsWidget_ircViewMarker()
    = default;

void OptionsWidget_ircViewMarker::commit()
{
	KVI_OPTION_UINT(KviOption_uintIrcViewMarkerStyle) = m_pMarkerStyle->currentIndex();
	KviOptionsWidget::commit();
}
