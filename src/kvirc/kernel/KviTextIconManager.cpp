//=============================================================================
//
//   File : KviTextIconManager.cpp
//   Creation date : Thu 15 May 2002 12:04:12 by Szymon Stefanek
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

#define _KVI_TEXTICONMANAGER_CPP_

#include "KviTextIconManager.h"
#include "kvi_confignames.h"
#include "KviAnimatedPixmap.h"
#include "KviApplication.h"
#include "KviConfigurationFile.h"
#include "KviCString.h"
#include "KviFileUtils.h"
#include "KviOptions.h"

#include <QFile>
#include <QPixmap>
#include <vector>

static KviTextIconAssocEntry default_associations[] = {
	{ ":)", KviIconManager::Smile },
	{ ":*", KviIconManager::Kiss },
	{ ":D", KviIconManager::BigGrin },
	{ ":(", KviIconManager::Ugly },
	{ ":/", KviIconManager::Angry },
	{ ":O", KviIconManager::Surprised2 },
	{ ":P", KviIconManager::Tongue },
	{ ":B", KviIconManager::Teeth },
	{ ";)", KviIconManager::Eye },
	{ ":')", KviIconManager::TearSmile },
	{ ":'", KviIconManager::Cry },
	{ ":S", KviIconManager::Afraid },
	{ ":|", KviIconManager::Demoralized },
	{ ":P'", KviIconManager::Slurp },
	{ nullptr, 0 }
};

KVIRC_API KviTextIconManager * g_pTextIconManager = nullptr;

KviTextIcon::KviTextIcon(KviIconManager::SmallIcon eIcon)
    : m_eIcon(eIcon)
{
}

KviTextIcon::KviTextIcon(const QString & szFile)
    : m_eIcon(KviIconManager::None), m_szFileName(szFile)
{
	QString szRetPath;

	if(g_pApp->findImage(szRetPath, szFile))
	{
		m_pAnimatedPixmap = new KviAnimatedPixmap(szRetPath, 16, 16);
		if(KVI_OPTION_BOOL(KviOption_boolEnableAnimatedSmiles))
			m_pAnimatedPixmap->start();
		else
			m_pAnimatedPixmap->stop();
	}
}

KviTextIcon::KviTextIcon(KviTextIcon * pIcon)
{
	m_eIcon = pIcon->id();
	m_szFileName = pIcon->m_szFileName;
	if(pIcon->m_pAnimatedPixmap)
		m_pAnimatedPixmap = new KviAnimatedPixmap(*(pIcon->m_pAnimatedPixmap));
}

KviTextIcon::~KviTextIcon()
{
	if(m_pAnimatedPixmap)
		delete m_pAnimatedPixmap;
}

void KviTextIcon::setId(KviIconManager::SmallIcon eIcon)
{
	m_eIcon = eIcon;
	m_szFileName.clear();
}

void KviTextIcon::setId(int iIcon)
{
	m_eIcon = g_pIconManager->iconName(iIcon);
	m_szFileName.clear();
}

void KviTextIcon::setFilename(const QString & szFileName)
{
	m_eIcon = KviIconManager::None;
	QString szRetPath;
	if(m_pAnimatedPixmap)
		delete m_pAnimatedPixmap;
	if(g_pApp->findImage(szRetPath, szFileName))
	{
		m_pAnimatedPixmap = new KviAnimatedPixmap(szRetPath, 16, 16);
		if(KVI_OPTION_BOOL(KviOption_boolEnableAnimatedSmiles))
			m_pAnimatedPixmap->start();
		else
			m_pAnimatedPixmap->stop();
	}
	else
	{
		m_pAnimatedPixmap = nullptr;
	}
	m_szFileName = szFileName;
}

QPixmap * KviTextIcon::pixmap()
{
	if(m_eIcon > 0)
		return g_pIconManager->getSmallIcon(m_eIcon);
	if(m_pAnimatedPixmap)
		return m_pAnimatedPixmap->pixmap();

	// This is actually wrong (at least for the current implementation).
	// Users of this class expect the pointer to be permanent while
	// g_pIconManager returns temporary pointers.
	// KviIrcView will happily crash dereferencing a hollow pointer sooner or later
	return g_pIconManager->getPixmap(m_szFileName);
}

KviTextIconManager::KviTextIconManager()
    : QObject()
{
	m_pTextIconDict = new KviPointerHashTable<QString, KviTextIcon>(47, false);
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
	m_pTextIconDict->replace(szName, new KviTextIcon(g_pIconManager->iconName(iId)));
	emit changed();
}

void KviTextIconManager::insert(const QString & szName, KviTextIcon & icon)
{
	m_pTextIconDict->replace(szName, new KviTextIcon(&icon));
	emit changed();
}

void KviTextIconManager::checkDefaultAssociations()
{
	for(int i = 0; default_associations[i].name; i++)
	{
		if(!m_pTextIconDict->find(default_associations[i].name))
			insert(QString::fromUtf8(default_associations[i].name), default_associations[i].iVal);
	}
	emit changed();
}

void KviTextIconManager::load()
{
	QString szTmp;
	int iUpd = 0;

	if(g_pApp->getReadOnlyConfigPath(szTmp, KVI_CONFIGFILE_TEXTICONS))
		iUpd = load(szTmp, false);

	if(iUpd == TEXTICONMANAGER_CURRENT_CONFIG_UPDATE)
		return;

	// do a merge of the texticons if we have a new config version
	g_pApp->getGlobalKvircDirectory(szTmp, KviApplication::Config, KVI_CONFIGFILE_TEXTICONS);

	if(QFile::exists(szTmp))
		load(szTmp, true);
}

void KviTextIconManager::applyOptions()
{
	for(
	    KviTextIcon * pIcon = m_pTextIconDict->first();
	    pIcon;
	    pIcon = m_pTextIconDict->next())
	{
		if(pIcon->animatedPixmap())
		{
			if(KVI_OPTION_BOOL(KviOption_boolEnableAnimatedSmiles))
				pIcon->animatedPixmap()->start();
			else
				pIcon->animatedPixmap()->stop();
		}
	}
}

void KviTextIconManager::save()
{
	QString szPath;
	g_pApp->getLocalKvircDirectory(szPath, KviApplication::Config, KVI_CONFIGFILE_TEXTICONS);

	//qDebug("Saving text icons to %s",szPath.toUtf8().data());
	save(szPath);
}

int KviTextIconManager::load(const QString & szFileName, bool bMerge)
{
	if(!bMerge)
		m_pTextIconDict->clear();

	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Read);

	cfg.setGroup("Manager");
	int iUpd = cfg.readIntEntry("ConfigUpdate", 0);

	KviConfigurationFileGroup * pDict = cfg.dict()->find("TextIcons");
	if(pDict)
	{
		KviConfigurationFileGroupIterator it(*pDict);

		std::vector<QString> names;

		while(it.current())
		{
			names.push_back(it.currentKey());
			++it;
		}

		cfg.setGroup("TextIcons");

		for(auto & s : names)
		{
			int iId = cfg.readIntEntry(s, -1);
			QString szTmp;
			QPixmap * pix = nullptr;
			//qDebug("%s %s %i %i",__FILE__,__FUNCTION__,__LINE__,id);
			if(iId > 0)
			{
				pix = g_pIconManager->getSmallIcon(iId);
			}
			else
			{
				szTmp = cfg.readEntry(s);
				pix = g_pIconManager->getPixmap(szTmp);
				if(!pix)
				{
					iId = KviIconManager::Help;
					pix = g_pIconManager->getSmallIcon(iId);
				}
			}

			if(pix)
			{
				if(bMerge)
				{
					if(!m_pTextIconDict->find(s))
					{
						if(iId != -1)
						{
							m_pTextIconDict->replace(s, new KviTextIcon(g_pIconManager->iconName(iId)));
						}
						else
						{
							m_pTextIconDict->replace(s, new KviTextIcon(szTmp));
						}
					}
				}
				else
				{
					if(iId > 0)
					{
						m_pTextIconDict->replace(s, new KviTextIcon(g_pIconManager->iconName(iId)));
					}
					else
					{
						m_pTextIconDict->replace(s, new KviTextIcon(szTmp));
					}
				}
			}
		}
	}

	emit changed();

	return iUpd;
}

void KviTextIconManager::save(const QString & szFileName)
{
	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Write);

	cfg.setGroup("Manager");
	cfg.writeEntry("ConfigUpdate", TEXTICONMANAGER_CURRENT_CONFIG_UPDATE);

	cfg.setGroup("TextIcons");

	KviPointerHashTableIterator<QString, KviTextIcon> it(*m_pTextIconDict);
	while(KviTextIcon * pIcon = it.current())
	{
		if(pIcon->id() > 0)
			cfg.writeEntry(it.currentKey(), pIcon->id());
		else
			cfg.writeEntry(it.currentKey(), pIcon->filename());
		++it;
	}
}
