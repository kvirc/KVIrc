#ifndef _OPTW_IGNORE_H_
#define _OPTW_IGNORE_H_
//=============================================================================
//
//   File : OptionsWidget_ignore.h
//   Creation date : Thu Feb 12 15:11:30 2002 GMT by Juanjo Álvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Juanjo Álvarez
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviOptionsWidget.h"
#include "KviSelectors.h"

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_ignore KviIconManager::Ignore
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_ignore __tr2qs_no_lookup("Ignore")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_ignore OptionsWidget_protection
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_ignore __tr2qs_no_lookup("protection")

class OptionsWidget_ignore : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_ignore(QWidget * parent);
	~OptionsWidget_ignore();

public:
	KviBoolSelector * m_pIgnorePrivmsg;
	KviBoolSelector * m_pIgnoreNotice;
	KviBoolSelector * m_pVerboseIgnore;
protected slots:
	void enableVerbose(bool);
};

#endif //_OPTW_IGNORE_H_
