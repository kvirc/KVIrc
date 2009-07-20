#ifndef _OPTW_NOTIFIER_H_
#define _OPTW_NOTIFIER_H_

//=============================================================================
//
//   File : optw_notifier.h
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

#include "kvi_optionswidget.h"

#include <QComboBox>

#define KVI_OPTIONS_WIDGET_ICON_KviNotifierLookOptionsWidget KVI_SMALLICON_ALERT
#define KVI_OPTIONS_WIDGET_NAME_KviNotifierLookOptionsWidget __tr2qs_no_lookup("Notifier")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviNotifierLookOptionsWidget __tr2qs_no_lookup("theme,colors,text")
#define KVI_OPTIONS_WIDGET_GROUP_KviNotifierLookOptionsWidget "theme"

class KviNotifierLookOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
private:
	QComboBox* m_pHorizontalAlign;
	QComboBox* m_pVerticalAlign;
public:
	KviNotifierLookOptionsWidget(QWidget * parent);
	~KviNotifierLookOptionsWidget();

	virtual void commit();
};

#define KVI_OPTIONS_WIDGET_ICON_KviNotifierOptionsWidget KVI_SMALLICON_ALERT
#define KVI_OPTIONS_WIDGET_NAME_KviNotifierOptionsWidget __tr2qs_no_lookup("Notifier")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviNotifierOptionsWidget __tr2qs_no_lookup("popup")
#define KVI_OPTIONS_WIDGET_PARENT_KviNotifierOptionsWidget KviInterfaceFeaturesOptionsWidget

class KviNotifierOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviNotifierOptionsWidget(QWidget * parent);
	~KviNotifierOptionsWidget();
};

#endif //!_OPTW_NOTIFIER_H_
