//=============================================================================
//
//   File : OptionsWidget_input.cpp
//   Creation date : Sat Aug 11 2001 04:25:52 CEST by Szymon Stefanek
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

#include "OptionsWidget_input.h"

#include "KviOptions.h"
#include "KviLocale.h"

#include <QLayout>

OptionsWidget_inputLook::OptionsWidget_inputLook(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("inputlook_options_widget");
	createLayout();

	addFontSelector(0, 0, 1, 0, __tr2qs_ctx("Font:", "options"), KviOption_fontInput);
	addColorSelector(0, 1, 1, 1, __tr2qs_ctx("Background color:", "options"), KviOption_colorInputBackground);
	addColorSelector(0, 2, 1, 2, __tr2qs_ctx("Foreground color:", "options"), KviOption_colorInputForeground);
	addColorSelector(0, 3, 1, 3, __tr2qs_ctx("Selection background color:", "options"), KviOption_colorInputSelectionBackground);
	addColorSelector(0, 4, 1, 4, __tr2qs_ctx("Selection foreground color:", "options"), KviOption_colorInputSelectionForeground);
	addColorSelector(0, 5, 1, 5, __tr2qs_ctx("Control char color:", "options"), KviOption_colorInputControl);
	addColorSelector(0, 6, 1, 6, __tr2qs_ctx("Cursor color:", "options"), KviOption_colorInputCursor);

	addPixmapSelector(0, 7, 1, 7, __tr2qs_ctx("Background image:", "options"), KviOption_pixmapInputBackground);

	addLabel(0, 8, 0, 8, __tr2qs_ctx("Horizontal align:", "options"));
	m_pHorizontalAlign = new QComboBox(this);
	addWidgetToLayout(m_pHorizontalAlign, 1, 8, 1, 8);

	addLabel(0, 9, 0, 9, __tr2qs_ctx("Vertical align:", "options"));
	m_pVerticalAlign = new QComboBox(this);
	addWidgetToLayout(m_pVerticalAlign, 1, 9, 1, 9);

	m_pHorizontalAlign->addItem(__tr2qs_ctx("Tile", "options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Left", "options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Right", "options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Center", "options"));

	m_pVerticalAlign->addItem(__tr2qs_ctx("Tile", "options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Top", "options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Bottom", "options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Center", "options"));

	switch(KVI_OPTION_UINT(KviOption_uintInputPixmapAlign) & Qt::AlignHorizontal_Mask)
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

	switch(KVI_OPTION_UINT(KviOption_uintInputPixmapAlign) & Qt::AlignVertical_Mask)
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

	layout()->setRowStretch(7, 1);
}

OptionsWidget_inputLook::~OptionsWidget_inputLook()
    = default;

void OptionsWidget_inputLook::commit()
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

	KVI_OPTION_UINT(KviOption_uintInputPixmapAlign) = iFlags;
	KviOptionsWidget::commit();
}

OptionsWidget_inputFeatures::OptionsWidget_inputFeatures(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("ircviewfeatures_options_widget");
	createLayout();

	KviUIntSelector * u;

	addBoolSelector(0, 0, 0, 0, __tr2qs_ctx("Warp cursor at the end of line when browsing history", "options"), KviOption_boolInputHistoryCursorAtEnd);
	addBoolSelector(0, 1, 0, 1, __tr2qs_ctx("Enable the input history logging", "options"), KviOption_boolEnableInputHistory); //G&N 2005
	addBoolSelector(0, 2, 0, 2, __tr2qs_ctx("Hide input tool buttons by default", "options"), KviOption_boolHideInputToolButtons);
	addBoolSelector(0, 3, 0, 3, __tr2qs_ctx("Show warning about pasting multiple lines", "options"), KviOption_boolWarnAboutPastingMultipleLines);
	addBoolSelector(0, 4, 0, 4, __tr2qs_ctx("Commandline in user-friendly mode by default", "options"), KviOption_boolCommandlineInUserFriendlyModeByDefault);
	u = addUIntSelector(0, 5, 0, 5, __tr2qs_ctx("Expand tabulations in input using:", "options"), KviOption_uintSpacesToExpandTabulationInput, 1, 24, 8, true);
	u->setSuffix(__tr2qs_ctx(" spaces", "options"));

	KviTalGroupBox * g = addGroupBox(0, 6, 0, 6, Qt::Horizontal, __tr2qs_ctx("Nick Completion", "options"));
	KviBoolSelector *b, *c, *d;
	b = addBoolSelector(g, __tr2qs_ctx("Use bash-like nick completion", "options"), KviOption_boolBashLikeNickCompletion, !KVI_OPTION_BOOL(KviOption_boolZshLikeNickCompletion));
	c = addBoolSelector(g, __tr2qs_ctx("Use zsh-like nick completion", "options"), KviOption_boolZshLikeNickCompletion, !KVI_OPTION_BOOL(KviOption_boolBashLikeNickCompletion));
	d = addBoolSelector(g, __tr2qs_ctx("Prioritize nick completion by last action time", "options"), KviOption_boolPrioritizeLastActionTime);
	connect(b, SIGNAL(toggled(bool)), c, SLOT(setDisabled(bool)));
	connect(b, SIGNAL(toggled(bool)), d, SLOT(setDisabled(bool)));
	connect(c, SIGNAL(toggled(bool)), b, SLOT(setDisabled(bool)));
	connect(c, SIGNAL(toggled(bool)), d, SLOT(setDisabled(bool)));

	addStringSelector(g, __tr2qs_ctx("Nick completion postfix string:", "options"), KviOption_stringNickCompletionPostfix);
	addBoolSelector(g, __tr2qs_ctx("Use the completion postfix string for the first word only", "options"), KviOption_boolUseNickCompletionPostfixForFirstWordOnly);

	addBoolSelector(g, __tr2qs_ctx("Ignore special characters in nick completion", "options"), KviOption_boolIgnoreSpecialCharactersInNickCompletion);

	KviBoolSelector * e = addBoolSelector(0, 7, 0, 7, __tr2qs_ctx("Use a custom cursor width", "options"), KviOption_boolEnableCustomCursorWidth);
	KviUIntSelector * f = addUIntSelector(0, 8, 0, 8, __tr2qs_ctx("Custom cursor width:", "options"), KviOption_uintCustomCursorWidth, 1, 24, 8, KVI_OPTION_BOOL(KviOption_boolEnableCustomCursorWidth));
	f->setSuffix(__tr2qs_ctx(" pixels", "options"));
	connect(e, SIGNAL(toggled(bool)), f, SLOT(setEnabled(bool)));
	addRowSpacer(0, 9, 0, 9);
}

OptionsWidget_inputFeatures::~OptionsWidget_inputFeatures()
    = default;
