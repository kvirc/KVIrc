#ifndef _LOGFILE_H_
#define _LOGFILE_H_
//=============================================================================
//
//   File : LogFile.h
//   Creation date : Thu Apr 14 2011 19:16:59 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 Elvio Basello (hellvis69 at gmail dot com)
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

/**
* \file LogFile.h
* \author Elvio Basello
* \brief Describes a log file
*
* This file was originally part of LogViewWindow.h
*/

#include <QDate>

class QString;

/**
* \struct _LogFileData
* \brief A struct that contains the data of a log
*/
struct LogFileData
{
	QString szName; /**< the name of the log */
	QString szType; /**< the type of the log */
	QString szFile; /**< the name of the exported log */
};

/**
* \class LogFile
* \brief The LogFile class which handle any log file
*
* Log is in the format:
* $type_$nick.$network_$YYYY.$MM.$DD.log
* Examples:
* query_noldor.azzurra_2009.05.20.log
* channel_#slackware.azzurra_2009.11.03.log
*/
class LogFile
{
public:
	/**
	* \enum Type
	* \brief Holds the type of the log file
	*/
	enum Type
	{
		Channel = 0, /**< the log file of a channel */
		Console = 1, /**< the log file of a console */
		Query = 2,   /**< the log file of a query */
		DccChat = 3, /**< the log file of a dcc chat */
		Other = 4    /**< any other log file */
	};

	/**
	* \enum ExportType
	* \brief Holds the type of the exported log file
	*/
	enum ExportType
	{
		PlainText, /**< export log in plain text file */
		HTML       /**< export log in a HTML archive */
		           //XML        /**< export log in a XML file */
		           //DB         /**< export log in a database file */
	};

	/**
	* \brief Constructs the log file object
	* \param szName The name of the log
	* \return LogFile
	*/
	LogFile(const QString & szName);

private:
	Type m_eType;
	QString m_szType;
	QString m_szFilename;
	bool m_bCompressed;
	QString m_szName;
	QString m_szNetwork;
	QDate m_date;

public:
	/**
	* \brief Returns the type of the log
	* \return Type
	*/
	Type type() const { return m_eType; };

	/**
	* \brief Returns the type of the log
	* \return const QString &
	*/
	const QString & typeString() const { return m_szType; };

	/**
	* \brief Returns the filename of the log
	* \return const QString &
	*/
	const QString & fileName() const { return m_szFilename; };

	/**
	* \brief Returns the name of the log
	* \return const QString &
	*/
	const QString & name() const { return m_szName; };

	/**
	* \brief Returns the network of the log
	* \return const QString &
	*/
	const QString & network() const { return m_szNetwork; };

	/**
	* \brief Returns the date of the log
	* \return const QDate &
	*/
	const QDate & date() const { return m_date; };

	/**
	* \brief Returns the text of the log file
	* \param szText The buffer where to save the contents of the log
	* \return void
	*/
	void getText(QString & szText);
};

#endif // _LOGFILE_H_
