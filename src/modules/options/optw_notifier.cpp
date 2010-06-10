//=============================================================================
//
//   File : optw_notifier.cpp
//   Creation date : Thu Jul 09 2009 10:54:39 CEST by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2009 Szymon Stefanek (pragma at kvirc dot net)
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

#include "optw_notifier.h"

#include "kvi_options.h"
#include "kvi_locale.h"

#include <QLayout>

KviNotifierLookOptionsWidget::KviNotifierLookOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("notifierlook_options_widget");
	createLayout();

	addFontSelector(0,0,1,0,__tr2qs_ctx("Font","options"),KviOption_fontNotifier);
	addColorSelector(0,1,1,1,__tr2qs_ctx("Background color","options"),KviOption_colorNotifierBackground);
	addColorSelector(0,2,1,2,__tr2qs_ctx("Foreground color","options"),KviOption_colorNotifierForeground);
	addFontSelector(0,3,1,3,__tr2qs_ctx("Title Font","options"),KviOption_fontNotifierTitle);
	addColorSelector(0,4,1,4,__tr2qs_ctx("Title Foreground color","options"),KviOption_colorNotifierTitleForeground);

	addPixmapSelector(0,5,1,5,__tr2qs_ctx("Background image","options"),KviOption_pixmapNotifierBackground);

	addLabel(0,6,0,6,__tr2qs_ctx("Horizontal align:","options"));
	m_pHorizontalAlign=new QComboBox(this);
	addWidgetToLayout(m_pHorizontalAlign,1,6,1,6);

	addLabel(0,7,0,7,__tr2qs_ctx("Vertical align:","options"));
	m_pVerticalAlign=new QComboBox(this);
	addWidgetToLayout(m_pVerticalAlign,1,7,1,7);

	m_pHorizontalAlign->addItem(__tr2qs_ctx("Tile","options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Left","options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Right","options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Center","options"));

	m_pVerticalAlign->addItem(__tr2qs_ctx("Tile","options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Top","options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Bottom","options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Center","options"));

	switch( KVI_OPTION_UINT(KviOption_uintNotifierPixmapAlign) & Qt::AlignHorizontal_Mask)
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

	switch( KVI_OPTION_UINT(KviOption_uintNotifierPixmapAlign) & Qt::AlignVertical_Mask)
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

	layout()->setRowStretch(5,1);
}

KviNotifierLookOptionsWidget::~KviNotifierLookOptionsWidget()
{
}

void KviNotifierLookOptionsWidget::commit()
{
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

	KVI_OPTION_UINT(KviOption_uintNotifierPixmapAlign)=iFlags;
	KviOptionsWidget::commit();
}



KviNotifierOptionsWidget::KviNotifierOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("notifier_options_widget");

	createLayout();

	int iRow = 0;

	KviBoolSelector * b = addBoolSelector(0,iRow,0,iRow,__tr2qs_ctx("Enable the notifier","options"),KviOption_boolEnableNotifier);
	QString tip = "<center>";
	tip += __tr2qs_ctx("This is an option for the impatient: it allows to forcibly and permanently disable " \
					"the notifier window. Please note that if this option is activated then " \
					"the notifier will NOT popup even if all the other options around specify " \
					"to use it in response to particular events. Also note that this option " \
					"will make all the /notifier.* commands fail silently.","options");
	tip += "</center>";
	mergeTip(b,tip);

	iRow++;

	KviBoolSelector * b2;

#if defined(COMPILE_KDE_SUPPORT) || defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

	b2 = addBoolSelector(0,iRow,0,iRow,__tr2qs_ctx("Don't show notifier when there is an active fullscreen window","options"),KviOption_boolDontShowNotifierIfActiveWindowIsFullScreen);

	tip = "<center>";
	tip += __tr2qs_ctx("This option stops the notifier from being displayed when there is an active fullscreen window. " \
						"This is useful for gaming sessions where you may be distracted by the notifier or it may even switch " \
						"your game from fullscreen to window mode.","options");
	tip += "</center>";
	mergeTip(b2,tip);
	
	b2->setEnabled(KVI_OPTION_BOOL(KviOption_boolEnableNotifier));
	QObject::connect(b,SIGNAL(toggled(bool)),b2,SLOT(setEnabled(bool)));

	iRow++;

#endif //COMPILE_KDE_SUPPORT || COMPILE_ON_WINDOWS || COMPILE_ON_MINGW

	b2 = addBoolSelector(0,iRow,0,iRow,__tr2qs_ctx("Enable notifier window flashing","options"),KviOption_boolNotifierFlashing);

	b2->setEnabled(KVI_OPTION_BOOL(KviOption_boolEnableNotifier));
	QObject::connect(b,SIGNAL(toggled(bool)),b2,SLOT(setEnabled(bool)));


	iRow++;
	
	b2 = addBoolSelector(0,iRow,0,iRow,__tr2qs_ctx("Enable notifier window fade effect","options"),KviOption_boolNotifierFading);

	b2->setEnabled(KVI_OPTION_BOOL(KviOption_boolEnableNotifier));
	QObject::connect(b,SIGNAL(toggled(bool)),b2,SLOT(setEnabled(bool)));

	iRow++;

	KviTalGroupBox *g = addGroupBox(0,iRow,0,iRow,Qt::Horizontal,__tr2qs_ctx("Advanced configuration","options"));
	connect(b,SIGNAL(toggled(bool)),g,SLOT(setEnabled(bool)));

	connect(b,
		SIGNAL(toggled(bool)),
		addUIntSelector(g,__tr2qs_ctx("Default auto hiding time for messages (0 to disable)","options"),
				KviOption_uintNotifierAutoHideTime,
				0,86400,30,KVI_OPTION_BOOL(KviOption_boolEnableNotifier)),
		SLOT(setEnabled(bool)));

	connect(b2,
		SIGNAL(toggled(bool)),
		addUIntSelector(g,__tr2qs_ctx("Notifier window opacity while active (mouseover)","options"),
				KviOption_uintNotifierActiveTransparency,
				0,100,90,KVI_OPTION_BOOL(KviOption_boolNotifierFading)),
		SLOT(setEnabled(bool)));

	connect(b2,
		SIGNAL(toggled(bool)),
		addUIntSelector(g,__tr2qs_ctx("Notifier window opacity while inactive","options"),
				KviOption_uintNotifierInactiveTransparency,
				0,100,40,KVI_OPTION_BOOL(KviOption_boolNotifierFading)),
		SLOT(setEnabled(bool)));

	iRow++;

	addRowSpacer(0,iRow,0,iRow);
}

KviNotifierOptionsWidget::~KviNotifierOptionsWidget()
{
}
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_input.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
