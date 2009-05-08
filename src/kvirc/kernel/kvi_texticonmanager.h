#ifndef _KVI_TEXTICONMANAGER_H_
#define _KVI_TEXTICONMANAGER_H_

//=============================================================================
//
//   File : kvi_texticonmanager.h
//   Creation date : Thu 15 May 2002 12:04:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"

#include <qpixmap.h>
#include "kvi_pointerhashtable.h"


class KVIRC_API KviTextIcon
{
protected:
	int      m_iId;
	QString  m_szFilename;
public:
	KviTextIcon(int id)
		: m_iId(id) {};
	KviTextIcon(QString szFile)
		: m_iId(-1),m_szFilename(szFile) {};
	KviTextIcon(KviTextIcon* icon);
	~KviTextIcon(){};
public:
	int id(){ return m_iId; };
	void setId(int id);
	void setFilename(QString filename);
	QString filename(){ return m_szFilename; };
	QPixmap * pixmap();
};

class KVIRC_API KviTextIconManager : public QObject
{
	friend class KviTextIconWindow;
	friend class KviTextIconsOptionsWidget;
	Q_OBJECT
public:
	KviTextIconManager();
	~KviTextIconManager();
private:
	KviPointerHashTable<QString,KviTextIcon> * m_pTextIconDict;
signals:
	void changed();
public:
	KviPointerHashTable<QString,KviTextIcon> * textIconDict(){ return m_pTextIconDict; };
	void checkDefaultAssociations();
	void clear();
	void insert(const QString &name,int id);
	void insert(const QString &name,KviTextIcon& icon);
	KviTextIcon * lookupTextIcon(const QString &name){ return m_pTextIconDict->find(name); };
	void load();
	void save();
protected:
	void save(const QString &filename);
	int load(const QString &filename,bool bMerge = false);
};

#ifndef _KVI_TEXTICONMANAGER_CPP_
	extern KVIRC_API KviTextIconManager * g_pTextIconManager;
#endif


#endif //_KVI_TEXTICONMANAGER_H_
