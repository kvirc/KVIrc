#ifndef _OPTW_TEXTENCODING_H_
#define _OPTW_TEXTENCODING_H_
//=============================================================================
//
//   File : OptionsWidget_textEncoding.h
//   Creation date : Sat Mar 02 2002 12:09:19 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
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

#include <QComboBox>
#include <QTableWidget>

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_textEncoding KviIconManager::TextEncoding
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_textEncoding __tr2qs_no_lookup("Language")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_textEncoding 50000
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_textEncoding OptionsWidget_general
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_textEncoding __tr2qs_no_lookup("language,encoding,charset,codepage")

class OptionsWidget_textEncoding : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_textEncoding(QWidget * parent);
	~OptionsWidget_textEncoding();

protected:
	QString m_szLanguage;
	QComboBox * m_pTextEncodingCombo;
	QComboBox * m_pSrvEncodingCombo;
	QComboBox * m_pForcedLocaleCombo;
	QTableWidget * m_pSpellCheckerDictionaries;

public:
	virtual void commit();
};

#endif //!_OPTW_TEXTENCODING_H_
