//=============================================================================
//
//   File : kvi_texticonmanager.cpp
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================


#define __KVIRC__

#define _KVI_TEXTICONMANAGER_CPP_

#include "kvi_iconmanager.h"
#include "kvi_texticonmanager.h"
#include "kvi_string.h"
#include "kvi_pointerlist.h"
#include "kvi_config.h"
#include "kvi_app.h"
#include "kvi_confignames.h"

#include <QPixmap>
#include <QFile>

KVIRC_API KviTextIconManager * g_pTextIconManager = 0;

KviTextIcon::KviTextIcon(KviTextIcon* icon)
{
	m_iId=icon->id();
	m_szFilename=icon->m_szFilename;
}

void KviTextIcon::setId(int id)
{
	m_iId=id;
	m_szFilename=QString::null;
}

void KviTextIcon::setFilename(QString filename)
{
	m_iId=-1;
	m_szFilename=filename;
}

QPixmap * KviTextIcon::pixmap()
{
	if(m_iId>=0)
	{
		return g_pIconManager->getSmallIcon(m_iId);
	} else {
		// This is actually wrong (at least for the current implementation).
		// Users of this class expect the pointer to be permanent while
		// g_pIconManager returns temporary pointers.
		// KviIrcView will happily crash dereferencing a hollow pointer sooner or later
		return g_pIconManager->getPixmap(m_szFilename);
	}
}

KviTextIconManager::KviTextIconManager()
: QObject()
{
	m_pTextIconDict = new KviPointerHashTable<QString,KviTextIcon>(47,false);
	m_pTextIconDict->setAutoDelete(true);
}

KviTextIconManager::~KviTextIconManager()
{
	delete m_pTextIconDict;
}

void KviTextIconManager::clear()
{
	m_pTextIconDict->clear();
}

void KviTextIconManager::insert(const QString &name,int id)
{
	m_pTextIconDict->replace(name,new KviTextIcon(id));
	emit changed();
}

void KviTextIconManager::insert(const QString &name,KviTextIcon& icon)
{
	m_pTextIconDict->replace(name,new KviTextIcon(&icon));
	emit changed();
}

typedef struct _KviTextIconAssocEntry
{
	const char * name;
	int          iVal;
} KviTextIconAssocEntry;

static KviTextIconAssocEntry default_associations[]=
{
	{ ":)"                , KVI_SMALLICON_SMILE       },
	{ ":*"                , KVI_SMALLICON_KISS        },
	{ ":D"                , KVI_SMALLICON_BIGGRIN     },
	{ ":("                , KVI_SMALLICON_UGLY        },
	{ ":/"                , KVI_SMALLICON_ANGRY       },
	{ ":O"                , KVI_SMALLICON_SURPRISED2  },
	{ ":P"                , KVI_SMALLICON_TONGUE      },
	{ ";)"                , KVI_SMALLICON_EYE         },
	{ ":°)"               , KVI_SMALLICON_TEARSMILE   },
	{ ":°"                , KVI_SMALLICON_CRY         },
	{ ":S"                , KVI_SMALLICON_AFRAID      },
	{ ":|"                , KVI_SMALLICON_DEMORALIZED },
	{ ":P°"               , KVI_SMALLICON_SLURP       },
	{ 0                   , 0                         }
};

void KviTextIconManager::checkDefaultAssociations()
{
	for(int i=0;default_associations[i].name;i++)
	{
		if(!m_pTextIconDict->find(default_associations[i].name))
			insert(QString::fromUtf8(default_associations[i].name),default_associations[i].iVal);
	}
	emit changed();
}

// this MUST match the ConfigUpdate entry in the configuration
// file shipped with KVIrc.
#define TEXTICONMANAGER_CURRENT_CONFIG_UPDATE 3

void KviTextIconManager::load()
{
	KviStr tmps;
	int upd = 0;
	if(g_pApp->getReadOnlyConfigPath(tmps,KVI_CONFIGFILE_TEXTICONS))
	{
		upd = load(tmps.ptr(),false);
	}

	if(upd == TEXTICONMANAGER_CURRENT_CONFIG_UPDATE)return;

	// do a merge of the texticons if we have a new config version
	QString tmp;
	g_pApp->getGlobalKvircDirectory(tmp,KviApp::Config,KVI_CONFIGFILE_TEXTICONS);
	if(QFile::exists(tmp))load(tmp,true);
}

void KviTextIconManager::save()
{
	QString tmp;
	g_pApp->getLocalKvircDirectory(tmp,KviApp::Config,KVI_CONFIGFILE_TEXTICONS);
	save(tmp);
}

// returns the config update
int KviTextIconManager::load(const QString &filename,bool bMerge)
{
	if(!bMerge)m_pTextIconDict->clear();
	KviConfig cfg(filename,KviConfig::Read);

	cfg.setGroup("Manager");
	int upd = cfg.readIntEntry("ConfigUpdate",0);

	KviConfigGroup * dict = cfg.dict()->find("TextIcons");
	if(dict)
	{
		KviConfigGroupIterator it(*dict);
	
		KviPointerList<QString> names;
		names.setAutoDelete(true);
	
		while(it.current())
		{
			names.append(new QString(it.currentKey()));
			++it;
		}
	
		cfg.setGroup("TextIcons");
	
		for(QString * s = names.first();s;s = names.next())
		{
			int id = cfg.readIntEntry(*s,-1);
			QString szTmp;
			QPixmap * pix=0;
//			debug("%s %s %i %i",__FILE__,__FUNCTION__,__LINE__,id);
			if(id!=-1)
				pix = g_pIconManager->getSmallIcon(id);
			else {
				szTmp=cfg.readEntry(*s);
				pix=g_pIconManager->getPixmap(szTmp);
				if(!pix)
				{
					id=KVI_SMALLICON_HELP;
					pix = g_pIconManager->getSmallIcon(id);
				}
			}
			if(pix)
			{
				if(bMerge)
				{
					if(!m_pTextIconDict->find(*s))
						if(id!=-1)
							m_pTextIconDict->replace(*s,new KviTextIcon(id));
						else
							m_pTextIconDict->replace(*s,new KviTextIcon(szTmp));
				} else {
					if(id!=-1)
						m_pTextIconDict->replace(*s,new KviTextIcon(id));
					else
						m_pTextIconDict->replace(*s,new KviTextIcon(szTmp));
				}
			}
		}
	}

	checkDefaultAssociations();
	emit changed();

	return upd;
}

void KviTextIconManager::save(const QString &filename)
{
	KviConfig cfg(filename,KviConfig::Write);

	cfg.setGroup("Manager");
	cfg.writeEntry("ConfigUpdate",TEXTICONMANAGER_CURRENT_CONFIG_UPDATE);


	cfg.setGroup("TextIcons");

	KviPointerHashTableIterator<QString,KviTextIcon> it(*m_pTextIconDict);
	while(KviTextIcon * i = it.current())
	{
		if(i->id()!=-1)
			cfg.writeEntry(it.currentKey(),i->id());
		else
			cfg.writeEntry(it.currentKey(),i->filename());
		++it;
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_texticonmanager.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
