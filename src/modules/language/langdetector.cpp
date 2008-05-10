//
//   File : helpwindow.cpp
//   Creation date : Tue Aug 11 2000 18:08:22 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the m_terms of the GNU General Public License
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

#include <QFile>

#include "langdetector.h"

KviLangDetetector::KviLangDetetector(QString doc)
{
	qint64 iMaxSize = 4096;
	char buffer[4096];
	QFile *file = new QFile(doc);

	if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	if(file->read(buffer, iMaxSize)<1)
		return;

	LanguageAndEncodingResult r;
	detect_language_and_encoding(buffer,&r,0);
	for(int i=0;i<DLE_NUM_BEST_MATCHES;i++)
		printf("LANGUAGE %s, ENCODING %s, SCORE: %f\n",r.match[i].szLanguage,r.match[i].szEncoding,r.match[i].dScore);
	printf("Accuracy: %f\n",r.dAccuracy);
	return;
}

