#ifndef _KVI_INPUT_HISTORY_H_
#define _KVI_INPUT_HISTORY_H_
//============================================================================
//
//   File : KviInputHistory.h
//   Creation date : Fri Sep 5 2008 17:26:34 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
//============================================================================

//
//   This file was originally part of KviInput.h
//

/**
* \file KviInputHistory.h
* \author Elvio Basello
* \brief Input history management
*
* \def KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES
* \def KVI_INPUT_MAX_LOCAL_HISTORY_ENTRIES
*/

#include "kvi_settings.h"

#include <vector>

class QString;

#define KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES 500
#define KVI_INPUT_MAX_LOCAL_HISTORY_ENTRIES 50

/**
* \class KviInputHistory
* \brief Input history class
*/
class KVIRC_API KviInputHistory
{
private:
	static KviInputHistory * m_pSelf;
	static unsigned int m_uCount;

protected:
	std::vector<QString> m_StringList;

public:
	/**
	* \brief Initializes the class instance
	* \return void
	*/
	static void init();

	/**
	* \brief Destroys the class instance
	* \return void
	*/
	static void done();

	/**
	* \brief Returns the instance of the class
	* \return KviInputHistory *
	*/
	static inline KviInputHistory * instance() { return m_pSelf; };

	/**
	* \brief Returns the number of instances of the class
	* \return unsigned int
	*/
	unsigned int count() { return m_uCount; };

	/**
	* \brief Adds a reference to the class
	* \return void
	*/
	static void addRef();

	/**
	* \brief Removes a reference to the class
	* \return void
	*/
	static void delRef();

	/**
	* \brief Adds a string to the history
	* \param szString The string to add
	* \return void
	*/
	void add(const QString & szString);

	/**
	* \brief Returns the list of string in the history
	* \return KviPointerList<QString> *
	*/
	std::vector<QString> list() { return m_StringList; };

	/**
	* \brief Saves the history
	* \param szFileName The name of the file where to save
	* \return void
	*/
	void save(const QString & szFileName);

	/**
	* \brief Loads the history
	* \param szFileName The name of the file to load
	* \return void
	*/
	void load(const QString & szFileName);
};

#endif //_KVI_INPUT_HISTORY_H_
