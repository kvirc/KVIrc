#ifndef _OPTW_INTERFACEFEATURES_H_
#define _OPTW_INTERFACEFEATURES_H_

//=============================================================================
//
//   File : optw_interfacefeatures.h
//   Creation date : Sat Jun 2 2001 14:50:01 CEST by Szymon Stefanek
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

#include "kvi_optionswidget.h"
#include <QCheckBox>

#define KVI_OPTIONS_WIDGET_ICON_KviInterfaceFeaturesOptionsWidget KVI_SMALLICON_IDEA
#define KVI_OPTIONS_WIDGET_NAME_KviInterfaceFeaturesOptionsWidget __tr2qs_no_lookup("Interface")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviInterfaceFeaturesOptionsWidget 50000

class KviInterfaceFeaturesOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviInterfaceFeaturesOptionsWidget(QWidget * parent);
	~KviInterfaceFeaturesOptionsWidget();
protected:
	QCheckBox * m_pDisableSplash;
public:
	virtual void commit();
};

#endif //!_OPTW_INTERFACEFEATURES_H_
