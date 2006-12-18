//
//   File : optw_textencoding.cpp
//   Creation date : Sat Aug 11 2001 03:29:52 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
#include "optw_textencoding.h"

#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_qstring.h"
#include "kvi_app.h"
#include "kvi_fileutils.h"

#include <qdir.h>
#include <qmessagebox.h>

KviTextEncodingOptionsWidget::KviTextEncodingOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"textencoding_options_widget")
{
	createLayout(4,2);

	addLabel(0,0,0,0,__tr2qs_ctx("Default text encoding:","options"));

	m_pTextEncodingCombo = new QComboBox(this);
	addWidgetToLayout(m_pTextEncodingCombo,1,0,1,0);

	m_pTextEncodingCombo->insertItem(__tr2qs_ctx("Use Language Encoding","options"));
	int i = 0;
	int iMatch = 0;
	KviLocale::EncodingDescription * d = KviLocale::encodingDescription(i);
	while(d->szName)
	{
		if(KviQString::equalCI(d->szName,KVI_OPTION_STRING(KviOption_stringDefaultTextEncoding)))iMatch = i + 1;
		m_pTextEncodingCombo->insertItem(d->szName);
		i++;
		d = KviLocale::encodingDescription(i);
	}

	m_pTextEncodingCombo->setCurrentItem(iMatch);
	
 	addLabel(0,1,0,1,__tr2qs_ctx("Force language:","options"));
	
	m_pForcedLocaleCombo = new QComboBox(this);
	
	addWidgetToLayout(m_pForcedLocaleCombo,1,1,1,1);
	
	QLabel * l = new QLabel(__tr2qs_ctx("You need to restart KVirc to apply a language changing","options"),this);
	addWidgetToLayout(l,0,2,1,2);
	
	m_pForcedLocaleCombo->insertItem(__tr2qs_ctx("Automatic detection","options"));
	m_pForcedLocaleCombo->insertItem(__tr2qs_ctx("en","options"));
	
	QString szLangFile=QString("%1/.kvirc_force_locale").arg(QDir::homeDirPath());
	if(KviFileUtils::fileExists(szLangFile))
	{
		KviFileUtils::readFile(szLangFile,m_szLanguage);
		m_szLanguage=m_szLanguage.stripWhiteSpace();
	}
	
	QString szLocaleDir;
	g_pApp->getGlobalKvircDirectory(szLocaleDir,KviApp::Locale);

	QStringList list=QDir(szLocaleDir).entryList("kvirc_*.mo",QDir::Files);
	
	i = 0;
	iMatch = 0;
	
	for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
		QString szTmp=*it;
		szTmp.replace("kvirc_","");
		szTmp.replace(".mo","");
		m_pForcedLocaleCombo->insertItem(szTmp);
//		debug("|%s|%s|",m_szLanguage.utf8().data(),szTmp.utf8().data());
		//debug(m_szLanguage);
		if(KviQString::equalCI(szTmp,m_szLanguage))
			iMatch = i + 2;
		i++;
	}
	if(KviQString::equalCI(m_szLanguage,"en"))
		m_pForcedLocaleCombo->setCurrentItem(1);
	else
		m_pForcedLocaleCombo->setCurrentItem(iMatch);
//	addRowSpacer(0,5,1,5);
}

KviTextEncodingOptionsWidget::~KviTextEncodingOptionsWidget()
{
}

void KviTextEncodingOptionsWidget::commit()
{
	int idx = m_pTextEncodingCombo->currentItem();
	if(idx <= 0)
	{
		// guess from locale
		KVI_OPTION_STRING(KviOption_stringDefaultTextEncoding) = "";
		return;
	}
	KVI_OPTION_STRING(KviOption_stringDefaultTextEncoding) = m_pTextEncodingCombo->text(idx);
	
	idx=m_pForcedLocaleCombo->currentItem();
	QString szLangFile=QString("%1/.kvirc_force_locale").arg(QDir::homeDirPath());
	if(idx==0) {
		if(KviFileUtils::fileExists(szLangFile))
			KviFileUtils::removeFile(szLangFile);
	} else {
		KviFileUtils::writeFile(szLangFile,m_pForcedLocaleCombo->text(idx));
	}
/*	if(!KviQString::equalCI(m_pForcedLocaleCombo->text(idx),m_szLanguage))
		QMessageBox::information(0,"KVIrc",__tr2qs_ctx("You need to restart KVirc to apply a language changing","options"),QMessageBox::Ok);*/
}

#include "m_optw_textencoding.moc"
