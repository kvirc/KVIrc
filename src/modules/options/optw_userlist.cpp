//=============================================================================
//
//   File : optw_userlist.cpp
//   Creation date : Sat Aug 11 2001 03:29:52 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "optw_userlist.h"

#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_userlistview.h"

#include <QLayout>

KviUserListLookOptionsWidget::KviUserListLookOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"userlistlook_options_widget")
{
}

KviUserListLookOptionsWidget::~KviUserListLookOptionsWidget()
{
}



//////////////////////////////////////////////////////////////////////////////////////////////////////


KviUserListLookForegroundOptionsWidget::KviUserListLookForegroundOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"userlistlook_foreground_options_widget")
{
	createLayout();

	addFontSelector(0,0,1,0,__tr2qs_ctx("Font:","options"),KviOption_fontUserListView);

	KviTalGroupBox * g = addGroupBox(0,1,1,1,Qt::Horizontal,__tr2qs_ctx("Nickname Colors","options"));
	addColorSelector(g,__tr2qs_ctx("Normal:","options"),KviOption_colorUserListViewNormalForeground);
	addColorSelector(g,__tr2qs_ctx("Selected:","options"),KviOption_colorUserListViewSelectionForeground);
	addColorSelector(g,__tr2qs_ctx("IRC Op:","options"),KviOption_colorUserListViewIrcOpForeground);
	addColorSelector(g,__tr2qs_ctx("Channel owner:","options"),KviOption_colorUserListViewChanOwnerForeground);
	addColorSelector(g,__tr2qs_ctx("Channel admin:","options"),KviOption_colorUserListViewChanAdminForeground);
	addColorSelector(g,__tr2qs_ctx("Op:","options"),KviOption_colorUserListViewOpForeground);
	addColorSelector(g,__tr2qs_ctx("Half-op:","options"),KviOption_colorUserListViewHalfOpForeground);
	addColorSelector(g,__tr2qs_ctx("Voice:","options"),KviOption_colorUserListViewVoiceForeground);
	addColorSelector(g,__tr2qs_ctx("User-op:","options"),KviOption_colorUserListViewUserOpForeground);
	addColorSelector(g,__tr2qs_ctx("Away:","options"),KviOption_colorUserListViewAwayForeground);

	KviTalHBox * hb = new KviTalHBox(g);
	hb->setSpacing(4);
	KviBoolSelector * b = addBoolSelector(hb,__tr2qs_ctx("Use different color for own nick:","options"),KviOption_boolUseDifferentColorForOwnNick);
	KviColorSelector * s = addColorSelector(hb,"",KviOption_colorUserListViewOwnForeground,KVI_OPTION_BOOL(KviOption_boolUseDifferentColorForOwnNick));
	connect(b,SIGNAL(toggled(bool)),s,SLOT(setEnabled(bool)));

	addRowSpacer(0,3,0,3);
}

KviUserListLookForegroundOptionsWidget::~KviUserListLookForegroundOptionsWidget()
{
}

KviUserListGridOptionsWidget::KviUserListGridOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	createLayout();
	KviBoolSelector* b = addBoolSelector(0,0,0,0,__tr2qs_ctx("Draw nickname grid","options"),KviOption_boolUserListViewDrawGrid);

	KviColorSelector* s = addColorSelector(0,1,0,1,__tr2qs_ctx("Grid color:","options"),KviOption_colorUserListViewGrid,KVI_OPTION_BOOL(KviOption_boolUserListViewDrawGrid));
	connect(b,SIGNAL(toggled(bool)),s,SLOT(setEnabled(bool)));

	KviTalHBox* hb = new KviTalHBox(this);
	addWidgetToLayout(hb,0,2,0,2);
	hb->setSpacing(4);

	QLabel * l = new QLabel(__tr2qs_ctx("Grid type:","options"),hb);
	l->setEnabled(KVI_OPTION_BOOL(KviOption_boolUserListViewDrawGrid));
	connect(b,SIGNAL(toggled(bool)),l,SLOT(setEnabled(bool)));
	m_pGridTypeCombo = new QComboBox(hb);
	m_pGridTypeCombo->addItem(__tr2qs_ctx("3D Grid","options"));
	m_pGridTypeCombo->addItem(__tr2qs_ctx("3D Buttons","options"));
	m_pGridTypeCombo->addItem(__tr2qs_ctx("Plain Grid","options"));
	m_pGridTypeCombo->addItem(__tr2qs_ctx("Dotted Grid","options"));
	m_pGridTypeCombo->setCurrentIndex(KVI_OPTION_UINT(KviOption_uintUserListViewGridType));
	m_pGridTypeCombo->setEnabled(KVI_OPTION_BOOL(KviOption_boolUserListViewDrawGrid));
	connect(b,SIGNAL(toggled(bool)),m_pGridTypeCombo,SLOT(setEnabled(bool)));
	addRowSpacer(0,3,0,3);
}

KviUserListGridOptionsWidget::~KviUserListGridOptionsWidget()
{
}

void KviUserListGridOptionsWidget::commit()
{
	KviOptionsWidget::commit();
	KVI_OPTION_UINT(KviOption_uintUserListViewGridType) = m_pGridTypeCombo->currentIndex();
	if(KVI_OPTION_UINT(KviOption_uintUserListViewGridType) > KVI_USERLISTVIEW_GRIDTYPE_MAXIMUM)
		KVI_OPTION_UINT(KviOption_uintUserListViewGridType) = KVI_USERLISTVIEW_GRIDTYPE_DEFAULT;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////

KviUserListLookBackgroundOptionsWidget::KviUserListLookBackgroundOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"userlistlook_background_options_widget")
{
	createLayout();

	KviTalGroupBox * g = addGroupBox(0,0,1,0,Qt::Horizontal,__tr2qs_ctx("Background Colors","options"));
	addColorSelector(g,__tr2qs_ctx("Normal:","options"),KviOption_colorUserListViewBackground);
	addColorSelector(g,__tr2qs_ctx("Selected:","options"),KviOption_colorUserListViewSelectionBackground);


	addPixmapSelector(0,1,1,1,__tr2qs_ctx("Background image:","options"),KviOption_pixmapUserListViewBackground);

	addLabel(0,2,0,2,__tr2qs_ctx("Horizontal Alignment:","options"));
	m_pHorizontalAlign=new QComboBox(this);
	addWidgetToLayout(m_pHorizontalAlign,1,2,1,2);

	addLabel(0,3,0,3,__tr2qs_ctx("Vertical Alignment:","options"));
	m_pVerticalAlign=new QComboBox(this);
	addWidgetToLayout(m_pVerticalAlign,1,3,1,3);

	m_pHorizontalAlign->addItem(__tr2qs_ctx("Tile","options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Left","options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Right","options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Center","options"));

	m_pVerticalAlign->addItem(__tr2qs_ctx("Tile","options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Top","options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Bottom","options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Center","options"));

	switch( KVI_OPTION_UINT(KviOption_uintUserListPixmapAlign) & Qt::AlignHorizontal_Mask)
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

	switch( KVI_OPTION_UINT(KviOption_uintUserListPixmapAlign) & Qt::AlignVertical_Mask)
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



	//layout()->setColumnStretch(0,1);
	layout()->setRowStretch(1,1);
}

KviUserListLookBackgroundOptionsWidget::~KviUserListLookBackgroundOptionsWidget()
{
}

void KviUserListLookBackgroundOptionsWidget::commit()
{
	KviOptionsWidget::commit();

	int iFlags=0;
	switch(m_pHorizontalAlign->currentIndex())
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
	switch(m_pVerticalAlign->currentIndex())
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

	KVI_OPTION_UINT(KviOption_uintUserListPixmapAlign)=iFlags;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////


KviUserListFeaturesOptionsWidget::KviUserListFeaturesOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"userlistfeatures_options_widget")
{
	createLayout();

	KviBoolSelector *b = addBoolSelector(0,0,0,0,__tr2qs_ctx("Show gender icons","options"),KviOption_boolDrawGenderIcons);
	b = addBoolSelector(0,1,0,1,__tr2qs_ctx("Show user channel icons","options"),KviOption_boolShowUserChannelIcons);
	b = addBoolSelector(0,2,0,2,__tr2qs_ctx("Show user channel activity indicator","options"),KviOption_boolShowUserChannelState);
	b = addBoolSelector(0,3,0,3,__tr2qs_ctx("Show label with userlist stats","options"),KviOption_boolShowUserListStatisticLabel);
	b = addBoolSelector(0,4,0,4,__tr2qs_ctx("Enable user tooltips","options"),KviOption_boolShowUserListViewToolTips);
	b = addBoolSelector(0,5,0,5,__tr2qs_ctx("Show avatars in userlist","options"),KviOption_boolShowAvatarsInUserlist);
	b = addBoolSelector(0,6,0,6,__tr2qs_ctx("Enable animated avatars","options"),KviOption_boolEnableAnimatedAvatars);
	b = addBoolSelector(0,7,0,7,__tr2qs_ctx("Place nicks starting with non-alpha chars (such as _COOL_BOY_) after the normal nicks","options"),KviOption_boolPlaceNickWithNonAlphaCharsAtEnd);

	addRowSpacer(0,8,0,8);
}

KviUserListFeaturesOptionsWidget::~KviUserListFeaturesOptionsWidget()
{
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_userlist.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
