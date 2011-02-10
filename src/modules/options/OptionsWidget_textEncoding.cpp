//=============================================================================
//
//   File : OptionsWidget_textEncoding.cpp
//   Creation date : Sat Aug 11 2001 03:29:52 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidget_textEncoding.h"

#include "kvi_defaults.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviQString.h"
#include "KviApplication.h"
#include "KviFileUtils.h"

#include <QDir>
#include <QMessageBox>

QString g_szPrevSettedLocale;

OptionsWidget_textEncoding::OptionsWidget_textEncoding(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("textencoding_options_widget");
	createLayout();

	//server encoding
	addLabel(0,0,0,0,__tr2qs_ctx("Default server encoding:","options"));

	m_pSrvEncodingCombo = new QComboBox(this);
	addWidgetToLayout(m_pSrvEncodingCombo,1,0,1,0);

	m_pSrvEncodingCombo->addItem(__tr2qs_ctx("Use Language Encoding","options"));

	//text encoding
	addLabel(0,1,0,1,__tr2qs_ctx("Default text encoding:","options"));

	m_pTextEncodingCombo = new QComboBox(this);
	addWidgetToLayout(m_pTextEncodingCombo,1,1,1,1);

	m_pTextEncodingCombo->addItem(__tr2qs_ctx("Use Language Encoding","options"));

	//common between text and server encoding
	int i = 0;
	int iTextMatch = 0, iSrvMatch=0;
	KviLocale::EncodingDescription * d = KviLocale::encodingDescription(i);
	while(d->szName)
	{
		if(KviQString::equalCI(d->szName,KVI_OPTION_STRING(KviOption_stringDefaultTextEncoding)))iTextMatch = i + 1;
		if(KviQString::equalCI(d->szName,KVI_OPTION_STRING(KviOption_stringDefaultSrvEncoding)))iSrvMatch = i + 1;

		m_pTextEncodingCombo->insertItem(m_pTextEncodingCombo->count(),d->szName);
		m_pSrvEncodingCombo->insertItem(m_pSrvEncodingCombo->count(),d->szName);
		i++;
		d = KviLocale::encodingDescription(i);
	}

	m_pTextEncodingCombo->setCurrentIndex(iTextMatch);
	m_pSrvEncodingCombo->setCurrentIndex(iSrvMatch);

 	addLabel(0,2,0,2,__tr2qs_ctx("Force language:","options"));

	m_pForcedLocaleCombo = new QComboBox(this);

	addWidgetToLayout(m_pForcedLocaleCombo,1,2,1,2);

	QLabel * l = new QLabel(__tr2qs_ctx("<b>Note:</b> You need to restart KVirc to apply a language changing","options"),this);
	addWidgetToLayout(l,0,3,1,3);

	m_pForcedLocaleCombo->addItem(__tr2qs_ctx("Automatic detection","options"));
	m_pForcedLocaleCombo->addItem(__tr2qs_ctx("en","options"));

	QString szLangFile;
	g_pApp->getLocalKvircDirectory(szLangFile,KviApplication::None,KVI_FORCE_LOCALE_FILE_NAME);

	bool bIsDefaultLocale = !KviFileUtils::fileExists(szLangFile);
	//We Have setted locale, but not restarted kvirc
	if(!g_szPrevSettedLocale.isEmpty())
	{
		m_szLanguage=g_szPrevSettedLocale;
	} else {
		m_szLanguage=KviLocale::localeName();
	}

	QString szLocaleDir;
	g_pApp->getGlobalKvircDirectory(szLocaleDir,KviApplication::Locale);

	QStringList list=QDir(szLocaleDir).entryList(QStringList("kvirc_*.mo"),QDir::Files);

	i = 0;
	int iMatch = 0;

	for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
		QString szTmp=*it;
		szTmp.replace("kvirc_","");
		szTmp.replace(".mo","");
		m_pForcedLocaleCombo->insertItem(m_pForcedLocaleCombo->count(),szTmp);
		if(KviQString::equalCI(szTmp,m_szLanguage))
			iMatch = i + 2;
		i++;
	}
	if(bIsDefaultLocale)
		m_pForcedLocaleCombo->setCurrentIndex(0);
	else if(KviQString::equalCI(m_szLanguage,"en"))
		m_pForcedLocaleCombo->setCurrentIndex(1);
	else
		m_pForcedLocaleCombo->setCurrentIndex(iMatch);
	addRowSpacer(0,4,1,4);
}

OptionsWidget_textEncoding::~OptionsWidget_textEncoding()
{
}

void OptionsWidget_textEncoding::commit()
{
	int idx = m_pTextEncodingCombo->currentIndex();
	if(idx <= 0)
	{
		// guess from locale
		KVI_OPTION_STRING(KviOption_stringDefaultTextEncoding) = "";
	} else {
		KVI_OPTION_STRING(KviOption_stringDefaultTextEncoding) = m_pTextEncodingCombo->itemText(idx);
	}

	idx = m_pSrvEncodingCombo->currentIndex();
	if(idx <= 0)
	{
		// guess from locale
		KVI_OPTION_STRING(KviOption_stringDefaultSrvEncoding) = "";
	} else {
		KVI_OPTION_STRING(KviOption_stringDefaultSrvEncoding) = m_pSrvEncodingCombo->itemText(idx);
	}

	idx=m_pForcedLocaleCombo->currentIndex();
	QString szLangFile;
	g_pApp->getLocalKvircDirectory(szLangFile,KviApplication::None,KVI_FORCE_LOCALE_FILE_NAME);
	if(idx==0) {
		if(KviFileUtils::fileExists(szLangFile))
			KviFileUtils::removeFile(szLangFile);
	} else {
		g_szPrevSettedLocale=m_pForcedLocaleCombo->itemText(idx);
		if(!KviFileUtils::writeFile(szLangFile,m_pForcedLocaleCombo->itemText(idx)))
		{
			QMessageBox::critical(this,"KVIrc",__tr2qs_ctx("Unable to write language information to","options")+"\n"+szLangFile,__tr2qs_ctx("Ok","options"));
		}
	}
/*	if(!KviQString::equalCI(m_pForcedLocaleCombo->text(idx),m_szLanguage))
		QMessageBox::information(0,"KVIrc",__tr2qs_ctx("You need to restart KVirc to apply a language changing","options"),QMessageBox::Ok);*/
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_OptionsWidget_textEncoding.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
