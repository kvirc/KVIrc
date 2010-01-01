#ifndef _KVI_TEXTICONMANAGER_H_
#define _KVI_TEXTICONMANAGER_H_

//=============================================================================
//
//   File : kvi_texticonmanager.h
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

/**
* \file kvi_texticonmanager.h
* \author Szymon Stefanek
* \brief
*
* \def TEXTICONMANAGER_CURRENT_CONFIG_UPDATE This MUST match the ConfigUpdate entry in the configuration file shipped with KVIrc
*/

#include "kvi_settings.h"
#include "kvi_pointerhashtable.h"
#include "kvi_animatedpixmap.h"

#include <QPixmap>

#define TEXTICONMANAGER_CURRENT_CONFIG_UPDATE 4

/**
* \typedef KviTextIconAssocEntry
* \struct _KviTextIconAssocEntry
* \brief A struct that contains the icon association entries
*/
typedef struct _KviTextIconAssocEntry
{
	const char * name; /**< the name of the icon */
	int          iVal; /**< the id of the icon */
} KviTextIconAssocEntry;

/**
* \class KviTextIcon
* \brief The class that holds the icons
*/
class KVIRC_API KviTextIcon
{
protected:
	int                 m_iId;
	QString             m_szFileName;
	KviAnimatedPixmap * m_pAnimatedPixmap;
public:
	/**
	* \brief Constructs the icon object
	* \param iId The id of the icon
	* \return KviTextIcon
	*/
	KviTextIcon(int iId);

	/**
	* \brief Constructs the icon object
	* \param szFile The filename the icon
	* \return KviTextIcon
	*/
	KviTextIcon(QString szFile);

	/**
	* \brief Constructs the icon object
	* \param pIcon The pointer to the icon
	* \return KviTextIcon
	*/
	KviTextIcon(KviTextIcon * pIcon);

	/**
	* \brief Destroys the icon object
	*/
	~KviTextIcon();
public:
	/**
	* \brief Returns the id of the icon
	* \return int
	*/
	inline int id(){ return m_iId; };

	/**
	* \brief Sets the id of the icon
	* \param iId The id of the icon
	* \return void
	*/
	void setId(int iId);

	/**
	* \brief Sets the filename of the icon
	* \param szFileName The filename of the icon
	* \return void
	*/
	void setFilename(QString szFileName);

	/**
	* \brief Returns the filename of the icon
	* \return QString
	*/
	inline QString filename(){ return m_szFileName; };

	/**
	* \brief Returns the pixmap associated to the icon
	* \return QPixmap *
	*/
	QPixmap * pixmap();

	/**
	* \brief Returns the animated pixmap associated to the icon
	* \return KviAnimatedPixmap *
	*/
	inline KviAnimatedPixmap * animatedPixmap(){ return m_pAnimatedPixmap; };
};

/**
* \class KviTextIconManager
* \brief The class that manages the icons
*/
class KVIRC_API KviTextIconManager : public QObject
{
	friend class KviTextIconWindow;
	friend class KviTextIconsOptionsWidget;
	Q_OBJECT
public:
	/**
	* \brief Constructs the icon manager object
	* \return KviTextIconManager
	*/
	KviTextIconManager();

	/**
	* \brief Destroys the icon manager object
	*/
	~KviTextIconManager();
private:
	KviPointerHashTable<QString,KviTextIcon> * m_pTextIconDict;
public:
	/**
	* \brief Returns the dictionary of the icons
	* \return KviPointerHashTable<QString,KviTextIcon> *
	*/
	inline KviPointerHashTable<QString,KviTextIcon> * textIconDict(){ return m_pTextIconDict; };

	/**
	* \brief Checks and updates the default associations
	* \return void
	*/
	void checkDefaultAssociations();

	/**
	* \brief Clears the dictionary
	* \return void
	*/
	void clear();

	/**
	* \brief Inserts an icon in the dictionary
	* \param szName The name of the icon
	* \param iId The id of the icon
	* \return void
	*/
	void insert(const QString & szName, int iId);

	/**
	* \brief Inserts an icon in the dictionary
	* \param szName The name of the icon
	* \param icon The icon
	* \return void
	*/
	void insert(const QString & szName, KviTextIcon & icon);

	/**
	* \brief Returns the text of the icon
	* \param szName The name of the icon
	* \return KviTextIcon *
	*/
	inline KviTextIcon * lookupTextIcon(const QString & szName){ return m_pTextIconDict->find(szName); };

	/**
	* \brief Loads the dictionary
	* \return void
	*/
	void load();

	/**
	* \brief Saves the dictionary
	* \return void
	*/
	void save();

	/**
	* \brief Applies the options
	* \return void
	*/
	void applyOptions();
protected:
	/**
	* \brief Saves the dictionary to a file
	*
	* This is the real save() function
	* \param szFileName The filename where to save
	* \return void
	*/
	void save(const QString & szFileName);

	/**
	* \brief Loads the dictionary from a file
	*
	* This is the real load() function
	* It returns the config update
	* \param szFileName The filename from where to load
	* \param bMerge Whether to merge the existing dictionary with the loaded one
	* \return int
	*/
	int load(const QString & szFileName, bool bMerge = false);
signals:
	/**
	* \brief Called when the default associations change
	* \return void
	*/
	void changed();
};

#ifndef _KVI_TEXTICONMANAGER_CPP_
	extern KVIRC_API KviTextIconManager * g_pTextIconManager;
#endif

#endif //_KVI_TEXTICONMANAGER_H_
