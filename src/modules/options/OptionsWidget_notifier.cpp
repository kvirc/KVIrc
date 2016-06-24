//=============================================================================
//
//   File : OptionsWidget_notifier.cpp
//   Creation date : Thu Jul 09 2009 10:54:39 CEST by Fabio Bas
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

#include "OptionsWidget_notifier.h"

#include "kvi_settings.h"
#include "KviOptions.h"
#include "KviLocale.h"

#include <QLayout>

OptionsWidget_notifierLook::OptionsWidget_notifierLook(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("notifierlook_options_widget");
	createLayout();

	addFontSelector(0, 0, 1, 0, __tr2qs_ctx("Font:", "options"), KviOption_fontNotifier);
	addColorSelector(0, 1, 1, 1, __tr2qs_ctx("Background color:", "options"), KviOption_colorNotifierBackground);
	addColorSelector(0, 2, 1, 2, __tr2qs_ctx("Foreground color:", "options"), KviOption_colorNotifierForeground);
	addFontSelector(0, 3, 1, 3, __tr2qs_ctx("Title font:", "options"), KviOption_fontNotifierTitle);
	addColorSelector(0, 4, 1, 4, __tr2qs_ctx("Title foreground color:", "options"), KviOption_colorNotifierTitleForeground);

	addPixmapSelector(0, 5, 1, 5, __tr2qs_ctx("Background image:", "options"), KviOption_pixmapNotifierBackground);

	addLabel(0, 6, 0, 6, __tr2qs_ctx("Horizontal align:", "options"));
	m_pHorizontalAlign = new QComboBox(this);
	addWidgetToLayout(m_pHorizontalAlign, 1, 6, 1, 6);

	addLabel(0, 7, 0, 7, __tr2qs_ctx("Vertical align:", "options"));
	m_pVerticalAlign = new QComboBox(this);
	addWidgetToLayout(m_pVerticalAlign, 1, 7, 1, 7);

	m_pHorizontalAlign->addItem(__tr2qs_ctx("Tile", "options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Left", "options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Right", "options"));
	m_pHorizontalAlign->addItem(__tr2qs_ctx("Center", "options"));

	m_pVerticalAlign->addItem(__tr2qs_ctx("Tile", "options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Top", "options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Bottom", "options"));
	m_pVerticalAlign->addItem(__tr2qs_ctx("Center", "options"));

	switch(KVI_OPTION_UINT(KviOption_uintNotifierPixmapAlign) & Qt::AlignHorizontal_Mask)
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

	switch(KVI_OPTION_UINT(KviOption_uintNotifierPixmapAlign) & Qt::AlignVertical_Mask)
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

	layout()->setRowStretch(5, 1);
}

OptionsWidget_notifierLook::~OptionsWidget_notifierLook()
    = default;

void OptionsWidget_notifierLook::commit()
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

	KVI_OPTION_UINT(KviOption_uintNotifierPixmapAlign) = iFlags;
	KviOptionsWidget::commit();
}

OptionsWidget_notifier::OptionsWidget_notifier(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("notifier_options_widget");
	createLayout();

	int iRow = 0;
	QString szTip;

	KviBoolSelector * b = addBoolSelector(0, iRow, 0, iRow, __tr2qs_ctx("Enable the notifier", "options"), KviOption_boolEnableNotifier);
	szTip += __tr2qs_ctx("This is an option for the impatient: it allows one to forcibly and permanently disable "
	                     "the notifier window. Please note that if this option is not activated then "
	                     "the notifier will NOT popup even if all the other options around specify "
	                     "to use it in response to particular events. Also note that this option "
	                     "will make all the /notifier.* commands fail silently.", "options");
	mergeTip(b, szTip);

	iRow++;

	KviBoolSelector * b2;

#ifdef COMPILE_KDE_SUPPORT
	m_pKdeNotifier = addBoolSelector(0, iRow, 0, iRow, __tr2qs_ctx("Use the KDE notifier", "options"), KviOption_boolUseKDENotifier);
	szTip = __tr2qs_ctx("This option uses the KDE notification system instead of the KVIrc's builtin.<br>"
	                     "This is cool if you want to better integrate KVIrc inside KDE.<br>"
	                     "Note that KDE's notifier isn't as flexible or \"tabbed\" like KVIrc's", "options");

	mergeTip(m_pKdeNotifier, szTip);

	m_pKdeNotifier->setEnabled(KVI_OPTION_BOOL(KviOption_boolEnableNotifier));
	connect(b, SIGNAL(toggled(bool)), m_pKdeNotifier, SLOT(setEnabled(bool)));
	connect(m_pKdeNotifier, SIGNAL(toggled(bool)), this, SLOT(toggleNotifierProtocol(bool)));

	iRow++;

#endif // COMPILE_KDE_SUPPORT

#ifdef COMPILE_DBUS_SUPPORT
	m_pDBusNotifier = addBoolSelector(0, iRow, 0, iRow, __tr2qs_ctx("Use the D-Bus-based notifiers", "options"), KviOption_boolUseDBusNotifier);
	szTip = __tr2qs_ctx("This option uses the D-Bus-based notifier instead of the KVIrc's builtin.<br>"
	                     "This is cool if you want to better integrate KVIrc inside your desktop environment.<br>"
	                     "Note that this notifier isn't as flexible or \"tabbed\" like KVIrc's is.", "options");

	mergeTip(m_pDBusNotifier, szTip);

	m_pDBusNotifier->setEnabled(KVI_OPTION_BOOL(KviOption_boolEnableNotifier));
#ifdef COMPILE_KDE_SUPPORT
	m_pDBusNotifier->setEnabled(!KVI_OPTION_BOOL(KviOption_boolUseKDENotifier));
#endif // COMPILE_KDE_SUPPORT
	connect(b, SIGNAL(toggled(bool)), m_pDBusNotifier, SLOT(setEnabled(bool)));

#ifdef COMPILE_KDE_SUPPORT
	connect(m_pDBusNotifier, SIGNAL(toggled(bool)), this, SLOT(toggleNotifierProtocol(bool)));
#endif

	iRow++;
#endif // COMPILE_DBUS_SUPPORT

#if defined(COMPILE_KDE_SUPPORT) || defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

	b2 = addBoolSelector(0, iRow, 0, iRow, __tr2qs_ctx("Don't show notifier when there is an active fullscreen window", "options"), KviOption_boolDontShowNotifierIfActiveWindowIsFullScreen);

	szTip = __tr2qs_ctx("This option stops the notifier from being displayed when there is an active fullscreen window. "
	                     "This is useful for gaming sessions where you may be distracted by the notifier or it may even switch "
	                     "your game from fullscreen to window mode.", "options");
	mergeTip(b2, szTip);

	b2->setEnabled(KVI_OPTION_BOOL(KviOption_boolEnableNotifier));
	QObject::connect(b, SIGNAL(toggled(bool)), b2, SLOT(setEnabled(bool)));

	iRow++;

#endif //COMPILE_KDE_SUPPORT || COMPILE_ON_WINDOWS || COMPILE_ON_MINGW

	b2 = addBoolSelector(0, iRow, 0, iRow, __tr2qs_ctx("Enable notifier window flashing", "options"), KviOption_boolNotifierFlashing);

	b2->setEnabled(KVI_OPTION_BOOL(KviOption_boolEnableNotifier));
	QObject::connect(b, SIGNAL(toggled(bool)), b2, SLOT(setEnabled(bool)));

	iRow++;

	b2 = addBoolSelector(0, iRow, 0, iRow, __tr2qs_ctx("Enable notifier window fade effect", "options"), KviOption_boolNotifierFading);

	b2->setEnabled(KVI_OPTION_BOOL(KviOption_boolEnableNotifier));
	QObject::connect(b, SIGNAL(toggled(bool)), b2, SLOT(setEnabled(bool)));

	iRow++;

	KviTalGroupBox * g = addGroupBox(0, iRow, 0, iRow, Qt::Horizontal, __tr2qs_ctx("Advanced Configuration", "options"));
	connect(b, SIGNAL(toggled(bool)), g, SLOT(setEnabled(bool)));

	connect(b, SIGNAL(toggled(bool)), addUIntSelector(g, __tr2qs_ctx("Default auto hiding time for messages (0 to disable):", "options"),
	    KviOption_uintNotifierAutoHideTime, 0, 86400, 30, KVI_OPTION_BOOL(KviOption_boolEnableNotifier)), SLOT(setEnabled(bool)));

	KviUIntSelector * u;

	u = addUIntSelector(g, __tr2qs_ctx("Notifier window opacity while active (mouseover):", "options"),
	    KviOption_uintNotifierActiveTransparency, 0, 100, 90, KVI_OPTION_BOOL(KviOption_boolNotifierFading));
	u->setSuffix("%");
	connect(b2, SIGNAL(toggled(bool)), u, SLOT(setEnabled(bool)));

	u = addUIntSelector(g, __tr2qs_ctx("Notifier window opacity while inactive:", "options"),
	    KviOption_uintNotifierInactiveTransparency, 0, 100, 40, KVI_OPTION_BOOL(KviOption_boolNotifierFading));
	u->setSuffix("%");
	connect(b2, SIGNAL(toggled(bool)), u, SLOT(setEnabled(bool)));

	iRow++;

	addRowSpacer(0, iRow, 0, iRow);
}

OptionsWidget_notifier::~OptionsWidget_notifier()
    = default;

#ifdef COMPILE_KDE_SUPPORT
void OptionsWidget_notifier::toggleNotifierProtocol(bool)
{
#ifdef COMPILE_DBUS_SUPPORT
	if(m_pKdeNotifier->isEnabled())
		m_pDBusNotifier->setEnabled(!m_pKdeNotifier->isChecked());

	if(m_pDBusNotifier->isEnabled())
		m_pKdeNotifier->setEnabled(!m_pDBusNotifier->isChecked());
#endif
}
#endif // COMPILE_KDE_SUPPORT
