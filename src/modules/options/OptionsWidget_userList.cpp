//=============================================================================
//
//   File : OptionsWidget_userList.cpp
//   Creation date : Sat Aug 11 2001 03:29:52 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidget_userList.h"

#include "KviOptions.h"
#include "KviLocale.h"
#include "KviUserListView.h"
#include "KviTalToolTip.h"

#include <QLayout>

OptionsWidget_userList::OptionsWidget_userList(QWidget * parent)
    : KviOptionsWidget(parent, "userlistlook_options_widget")
{
}

OptionsWidget_userList::~OptionsWidget_userList()
    = default;

OptionsWidget_userListForeground::OptionsWidget_userListForeground(QWidget * parent)
    : KviOptionsWidget(parent, "userlistlook_foreground_options_widget")
{
	createLayout();

	addFontSelector(0, 0, 1, 0, __tr2qs_ctx("Font:", "options"), KviOption_fontUserListView);

	KviTalGroupBox * g = addGroupBox(0, 1, 1, 1, Qt::Horizontal, __tr2qs_ctx("Nickname Colors", "options"));
	addColorSelector(g, __tr2qs_ctx("Normal:", "options"), KviOption_colorUserListViewNormalForeground);
	addColorSelector(g, __tr2qs_ctx("Selected:", "options"), KviOption_colorUserListViewSelectionForeground);
	addColorSelector(g, __tr2qs_ctx("IRC Op:", "options"), KviOption_colorUserListViewIrcOpForeground);
	addColorSelector(g, __tr2qs_ctx("Channel owner:", "options"), KviOption_colorUserListViewChanOwnerForeground);
	addColorSelector(g, __tr2qs_ctx("Channel admin:", "options"), KviOption_colorUserListViewChanAdminForeground);
	addColorSelector(g, __tr2qs_ctx("Op:", "options"), KviOption_colorUserListViewOpForeground);
	addColorSelector(g, __tr2qs_ctx("Half-op:", "options"), KviOption_colorUserListViewHalfOpForeground);
	addColorSelector(g, __tr2qs_ctx("Voice:", "options"), KviOption_colorUserListViewVoiceForeground);
	addColorSelector(g, __tr2qs_ctx("User-op:", "options"), KviOption_colorUserListViewUserOpForeground);

	KviTalHBox * ahb = new KviTalHBox(g);
	ahb->setSpacing(4);
	KviBoolSelector * ab = addBoolSelector(ahb, __tr2qs_ctx("Away (blend color):", "options"), KviOption_boolUserListViewUseAwayColor);
	QString szTip = __tr2qs("The color selected, will be blended with the user rank foreground color.<br>"
	                        "You should also consider your theme's style, color and the userlist background selected color for optimum visibility.<br>"
	                        "If no color is selected respective user rank color specified will be used instead.");

	KviColorSelector * as = addColorSelector(ahb, QString(), KviOption_colorUserListViewAwayForeground);
	connect(ab, SIGNAL(toggled(bool)), as, SLOT(setEnabled(bool)));
	mergeTip(ab, szTip);

	KviTalHBox * hb = new KviTalHBox(g);
	hb->setSpacing(4);
	KviBoolSelector * b = addBoolSelector(hb, __tr2qs_ctx("Use different color for own nick:", "options"), KviOption_boolUseDifferentColorForOwnNick);
	KviColorSelector * s = addColorSelector(hb, "", KviOption_colorUserListViewOwnForeground, KVI_OPTION_BOOL(KviOption_boolUseDifferentColorForOwnNick));
	connect(b, SIGNAL(toggled(bool)), s, SLOT(setEnabled(bool)));

	addRowSpacer(0, 3, 0, 3);
}

OptionsWidget_userListForeground::~OptionsWidget_userListForeground()
    = default;

OptionsWidget_userListGrid::OptionsWidget_userListGrid(QWidget * parent)
    : KviOptionsWidget(parent)
{
	createLayout();
	KviBoolSelector * b = addBoolSelector(0, 0, 0, 0, __tr2qs_ctx("Draw nickname grid", "options"), KviOption_boolUserListViewDrawGrid);

	KviColorSelector * s = addColorSelector(0, 1, 0, 1, __tr2qs_ctx("Grid color:", "options"), KviOption_colorUserListViewGrid, KVI_OPTION_BOOL(KviOption_boolUserListViewDrawGrid));
	connect(b, SIGNAL(toggled(bool)), s, SLOT(setEnabled(bool)));

	KviTalHBox * hb = new KviTalHBox(this);
	addWidgetToLayout(hb, 0, 2, 0, 2);
	hb->setSpacing(4);

	QLabel * l = new QLabel(__tr2qs_ctx("Grid type:", "options"), hb);
	l->setEnabled(KVI_OPTION_BOOL(KviOption_boolUserListViewDrawGrid));
	connect(b, SIGNAL(toggled(bool)), l, SLOT(setEnabled(bool)));
	m_pGridTypeCombo = new QComboBox(hb);
	m_pGridTypeCombo->addItem(__tr2qs_ctx("3D Grid", "options"));
	m_pGridTypeCombo->addItem(__tr2qs_ctx("3D Buttons", "options"));
	m_pGridTypeCombo->addItem(__tr2qs_ctx("Plain Grid", "options"));
	m_pGridTypeCombo->addItem(__tr2qs_ctx("Dotted Grid", "options"));
	m_pGridTypeCombo->setCurrentIndex(KVI_OPTION_UINT(KviOption_uintUserListViewGridType));
	m_pGridTypeCombo->setEnabled(KVI_OPTION_BOOL(KviOption_boolUserListViewDrawGrid));
	connect(b, SIGNAL(toggled(bool)), m_pGridTypeCombo, SLOT(setEnabled(bool)));

	addRowSpacer(0, 3, 0, 3);
}

OptionsWidget_userListGrid::~OptionsWidget_userListGrid()
    = default;

void OptionsWidget_userListGrid::commit()
{
	KviOptionsWidget::commit();
	KVI_OPTION_UINT(KviOption_uintUserListViewGridType) = m_pGridTypeCombo->currentIndex();
	if(KVI_OPTION_UINT(KviOption_uintUserListViewGridType) > KVI_USERLISTVIEW_GRIDTYPE_MAXIMUM)
		KVI_OPTION_UINT(KviOption_uintUserListViewGridType) = KVI_USERLISTVIEW_GRIDTYPE_DEFAULT;
}

OptionsWidget_userListBackground::OptionsWidget_userListBackground(QWidget * parent)
    : KviOptionsWidget(parent, "userlistlook_background_options_widget")
{
	createLayout();

	KviTalGroupBox * g = addGroupBox(0, 0, 1, 0, Qt::Horizontal, __tr2qs_ctx("Background Colors", "options"));
	addColorSelector(g, __tr2qs_ctx("Normal:", "options"), KviOption_colorUserListViewBackground);
	addColorSelector(g, __tr2qs_ctx("Selected:", "options"), KviOption_colorUserListViewSelectionBackground);

	addPixmapSelector(0, 1, 1, 1, __tr2qs_ctx("Background image:", "options"), KviOption_pixmapUserListViewBackground);

	addLabel(0, 2, 0, 2, __tr2qs_ctx("Horizontal alignment:", "options"));
	m_pHorizontalAlign = new QComboBox(this);
	addWidgetToLayout(m_pHorizontalAlign, 1, 2, 1, 2);

	addLabel(0, 3, 0, 3, __tr2qs_ctx("Vertical alignment:", "options"));
	m_pVerticalAlign = new QComboBox(this);
	addWidgetToLayout(m_pVerticalAlign, 1, 3, 1, 3);

	m_pHorizontalAlign->addItem(__tr2qs_ctx("Tile", "options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Left", "options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Right", "options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Center", "options"));

	m_pVerticalAlign->addItem(__tr2qs_ctx("Tile", "options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Top", "options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Bottom", "options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Center", "options"));

	switch(KVI_OPTION_UINT(KviOption_uintUserListPixmapAlign) & Qt::AlignHorizontal_Mask)
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

	switch(KVI_OPTION_UINT(KviOption_uintUserListPixmapAlign) & Qt::AlignVertical_Mask)
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
	layout()->setRowStretch(1, 1);
}

OptionsWidget_userListBackground::~OptionsWidget_userListBackground()
    = default;

void OptionsWidget_userListBackground::commit()
{
	KviOptionsWidget::commit();

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
	KVI_OPTION_UINT(KviOption_uintUserListPixmapAlign) = iFlags;
}

OptionsWidget_userListFeatures::OptionsWidget_userListFeatures(QWidget * parent)
    : KviOptionsWidget(parent, "userlistfeatures_options_widget")
{
	createLayout();

	KviUIntSelector * u;

	u = addUIntSelector(0, 0, 0, 0, __tr2qs_ctx("Minimum width:", "options"), KviOption_uintUserListMinimumWidth, 50, 1024, 150);
	u->setSuffix(__tr2qs_ctx(" pixels", "options"));
	mergeTip(u, __tr2qs_ctx("Here you can select a desired minimum width for the userlist when visible. "
	                        "Suggested values range between 120 and 170 pixels. "
	                        "The value set here, applies to all channels in all networks. "
	                        "For specific widths for different channels, drag widget to desired width, "
	                        "right click on channel on Tree Window List and select Save Window Properties As Default.<br>"
	                        "<b>Please note</b> dragging method may not be as reliable and values set may be lost randomly, "
	                        "However values will never be less than the width you specified.", "options"));

	addBoolSelector(0, 1, 0, 1, __tr2qs_ctx("Show gender icons", "options"), KviOption_boolDrawGenderIcons);
	addBoolSelector(0, 2, 0, 2, __tr2qs_ctx("Show user rank channel icons", "options"), KviOption_boolShowUserChannelIcons);
	addBoolSelector(0, 3, 0, 3, __tr2qs_ctx("Show user channel activity indicator", "options"), KviOption_boolShowUserChannelState);
	addBoolSelector(0, 4, 0, 4, __tr2qs_ctx("Show label with userlist stats", "options"), KviOption_boolShowUserListStatisticLabel);
	addBoolSelector(0, 5, 0, 5, __tr2qs_ctx("Enable user tooltip", "options"), KviOption_boolShowUserListViewToolTips);
	addBoolSelector(0, 6, 0, 6, __tr2qs_ctx("Show avatars in userlist", "options"), KviOption_boolShowAvatarsInUserlist);
	addBoolSelector(0, 7, 0, 7, __tr2qs_ctx("Enable animated avatars", "options"), KviOption_boolEnableAnimatedAvatars);
	KviBoolSelector * b = addBoolSelector(0, 8, 0, 8, __tr2qs_ctx("Place nicks starting with non-alpha characters (such as _COOL_BOY_) last", "options"), KviOption_boolPlaceNickWithNonAlphaCharsAtEnd);
	mergeTip(b, __tr2qs_ctx("All nicknames which include characters such as [ ] ^ _ &lt; &gt; | etc. "
	                        "will be sorted last in userlist after all regular nicknames.<br>"
	                        "Select this option if you prefer to see regular nicknames sorted topmost "
	                        "on userlist.<br><br><b>Note:</b> "
	                        "Changes to this option requires KVIrc restart.", "options"));

	addRowSpacer(0, 9, 0, 9);
}

OptionsWidget_userListFeatures::~OptionsWidget_userListFeatures()
    = default;
