#ifndef _KVI_TAL_APPLICATION_H_
#define _KVI_TAL_APPLICATION_H_
//=============================================================================
//
//   File : KviTalApplication.h
//   Creation date : Sun Jun 18 2000 12:53:08 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

//
// This file was originally named kvi_appbase_kde.h, created on 1999
//

/**
* \file KviTalApplication.h
* \author Szymon Stefanek
* \brief Toolkit Abstraction Level class
*/

#include "kvi_settings.h"

/**
* \class KviTalApplication
* \brief Toolkit Abstraction Layer: application class
*/

#if defined(COMPILE_KDE4_SUPPORT)

#include <KApplication>
class KVILIB_API KviTalApplication : public KApplication

#else //!defined(COMPILE_KDE4_SUPPORT)

// Either no KDE or KDE5 (which uses QApplication)
#include <QApplication>
class KVILIB_API KviTalApplication : public QApplication

#endif //!defined(COMPILE_KDE4_SUPPORT)
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the application object
	* \param iArgc The number of arguments
	* \param ppcArgv The arguments list
	* \return KviTalApplication
	*/
	KviTalApplication(int & iArgc, char ** ppcArgv);

	/**
	* \brief Destructs the application object
	*/
	~KviTalApplication();

	/**
	* \brief Saves configuration to disk
	* \return void
	*/
	virtual void saveConfiguration() = 0;

public Q_SLOTS:
	/**
	* \brief Commits the data and save the session
	* \param manager The session manager
	* \return void
	*/
	virtual void commitData(QSessionManager & manager);
};

#endif // _KVI_TAL_APPLICATION_H_
