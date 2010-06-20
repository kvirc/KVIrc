//=============================================================================
//
//   File : kvi_texticonmanager.cpp
//   Creation date : Thu 15 May 2002 12:04:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_TEXTICONMANAGER_CPP_

#include "kvi_iconmanager.h"
#include "kvi_texticonmanager.h"
#include "kvi_string.h"
#include "kvi_pointerlist.h"
#include "kvi_config.h"
#include "kvi_app.h"
#include "kvi_confignames.h"
#include "kvi_animatedpixmap.h"
#include "kvi_options.h"

#include <QPixmap>
#include <QFile>

static KviTextIconAssocEntry default_associations[]=
{
	{ ":)"    , KVI_SMALLICON_SMILE       },
	{ ":*"    , KVI_SMALLICON_KISS        },
	{ ":D"    , KVI_SMALLICON_BIGGRIN     },
	{ ":("    , KVI_SMALLICON_UGLY        },
	{ ":/"    , KVI_SMALLICON_ANGRY       },
	{ ":O"    , KVI_SMALLICON_SURPRISED2  },
	{ ":P"    , KVI_SMALLICON_TONGUE      },
	{ ";)"    , KVI_SMALLICON_EYE         },
	{ ":°)"   , KVI_SMALLICON_TEARSMILE   },
	{ ":°"    , KVI_SMALLICON_CRY         },
	{ ":S"    , KVI_SMALLICON_AFRAID      },
	{ ":|"    , KVI_SMALLICON_DEMORALIZED },
	{ ":P°"   , KVI_SMALLICON_SLURP       },
	{ 0       , 0                         }
};

KVIRC_API KviTextIconManager * g_pTextIconManager = 0;

KviTextIcon::KviTextIcon(int iId)
: m_iId(iId),m_pAnimatedPixmap(0)
{
}

KviTextIcon::KviTextIcon(QString szFile)
: m_iId(-1), m_szFileName(szFile)
{
	QString szRetPath;

	if(g_pApp->findImage(szRetPath, szFile))
	{
		m_pAnimatedPixmap = new KviAnimatedPixmap(szRetPath);
#if 0 // this doesn't work anyway
		if(KVI_OPTION_BOOL(KviOption_boolEnableAnimatedSmiles))
		{
			m_pAnimatedPixmap->start();
		} else {
			m_pAnimatedPixmap->stop();
		}
#endif
	} else {
		m_pAnimatedPixmap = NULL;
	}
}

KviTextIcon::KviTextIcon(KviTextIcon * pIcon)
{
	m_iId = pIcon->id();
	m_szFileName = pIcon->m_szFileName;
	if(pIcon->m_pAnimatedPixmap)
	{
		m_pAnimatedPixmap = new KviAnimatedPixmap(*(pIcon->m_pAnimatedPixmap));
	} else {
		m_pAnimatedPixmap = NULL;
	}
}

KviTextIcon::~KviTextIcon()
{
	if(m_pAnimatedPixmap)
		delete m_pAnimatedPixmap;
}

void KviTextIcon::setId(int iId)
{
	m_iId = iId;
	m_szFileName=QString();
}

void KviTextIcon::setFilename(QString szFileName)
{
	m_iId = -1;
	m_szFileName = szFileName;
}

QPixmap * KviTextIcon::pixmap()
{
	if(m_iId >= 0)
		return g_pIconManager->getSmallIcon(m_iId);

	// This is actually wrong (at least for the current implementation).
	// Users of this class expect the pointer to be permanent while
	// g_pIconManager returns temporary pointers.
	// KviIrcView will happily crash dereferencing a hollow pointer sooner or later
	return g_pIconManager->getPixmap(m_szFileName);
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

void KviTextIconManager::insert(const QString & szName, int iId)
{
	m_pTextIconDict->replace(szName, new KviTextIcon(iId));
	emit changed();
}

void KviTextIconManager::insert(const QString & szName,KviTextIcon & icon)
{
	m_pTextIconDict->replace(szName, new KviTextIcon(&icon));
	emit changed();
}

void KviTextIconManager::checkDefaultAssociations()
{
	for(int i=0; default_associations[i].name; i++)
	{
		if(!m_pTextIconDict->find(default_associations[i].name))
			insert(QString::fromUtf8(default_associations[i].name),default_associations[i].iVal);
	}
	emit changed();
}

void KviTextIconManager::load()
{
	QString szTmp;
	int iUpd = 0;
	if(g_pApp->getReadOnlyConfigPath(szTmp,KVI_CONFIGFILE_TEXTICONS))
	{
		iUpd = load(szTmp,false);
	}

	if(iUpd == TEXTICONMANAGER_CURRENT_CONFIG_UPDATE)
		return;

	// do a merge of the texticons if we have a new config version
	g_pApp->getGlobalKvircDirectory(szTmp,KviApp::Config,KVI_CONFIGFILE_TEXTICONS);

	if(QFile::exists(szTmp))
		load(szTmp,true);
}

void KviTextIconManager::applyOptions()
{
	for(
		KviTextIcon * pIcon = m_pTextIconDict->first();
		pIcon;
		pIcon = m_pTextIconDict->next()
		)
	{
		if(pIcon->animatedPixmap())
		{
			if(KVI_OPTION_BOOL(KviOption_boolEnableAnimatedSmiles))
			{
				pIcon->animatedPixmap()->start();
			} else {
				pIcon->animatedPixmap()->stop();
			}
		}
	}
}

void KviTextIconManager::save()
{
	QString szTmp;
	g_pApp->getLocalKvircDirectory(szTmp,KviApp::Config,KVI_CONFIGFILE_TEXTICONS);
	save(szTmp);
}

int KviTextIconManager::load(const QString & szFileName, bool bMerge)
{
	if(!bMerge) m_pTextIconDict->clear();

	KviConfig cfg(szFileName,KviConfig::Read);

	cfg.setGroup("Manager");
	int iUpd = cfg.readIntEntry("ConfigUpdate",0);

	KviConfigGroup * pDict = cfg.dict()->find("TextIcons");
	if(pDict)
	{
		KviConfigGroupIterator it(*pDict);

		KviPointerList<QString> names;
		names.setAutoDelete(true);

		while(it.current())
		{
			names.append(new QString(it.currentKey()));
			++it;
		}

		cfg.setGroup("TextIcons");

		for(QString * s = names.first(); s; s = names.next())
		{
			int iId = cfg.readIntEntry(*s,-1);
			QString szTmp;
			QPixmap * pix = 0;
			//debug("%s %s %i %i",__FILE__,__FUNCTION__,__LINE__,id);
			if(iId != -1)
			{
				pix = g_pIconManager->getSmallIcon(iId);
			} else {
				szTmp = cfg.readEntry(*s);
				pix=g_pIconManager->getPixmap(szTmp);
				if(!pix)
				{
					iId = KVI_SMALLICON_HELP;
					pix = g_pIconManager->getSmallIcon(iId);
				}
			}

			if(pix)
			{
				if(bMerge)
				{
					if(!m_pTextIconDict->find(*s))
					{
						if(iId != -1)
						{
							m_pTextIconDict->replace(*s,new KviTextIcon(iId));
						} else {
							m_pTextIconDict->replace(*s,new KviTextIcon(szTmp));
						}
					}
				} else {
					if(iId != -1)
					{
						m_pTextIconDict->replace(*s,new KviTextIcon(iId));
					} else {
						m_pTextIconDict->replace(*s,new KviTextIcon(szTmp));
					}
				}
			}
		}
	}

	checkDefaultAssociations();
	emit changed();

	return iUpd;
}

void KviTextIconManager::save(const QString & szFileName)
{
	KviConfig cfg(szFileName,KviConfig::Write);

	cfg.setGroup("Manager");
	cfg.writeEntry("ConfigUpdate",TEXTICONMANAGER_CURRENT_CONFIG_UPDATE);

	cfg.setGroup("TextIcons");

	KviPointerHashTableIterator<QString,KviTextIcon> it(*m_pTextIconDict);
	while(KviTextIcon * pIcon = it.current())
	{
		if(pIcon->id() != -1)
			cfg.writeEntry(it.currentKey(),pIcon->id());
		else
			cfg.writeEntry(it.currentKey(),pIcon->filename());
		++it;
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_texticonmanager.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
