#ifndef _KVI_TAL_FILEDIALOG_QT_H_
#define _KVI_TAL_FILEDIALOG_QT_H_

//=============================================================================
//
//   File : kvi_tal_filedialog_qt.h
//   Creation date : Fri 19 Jan 2007 02:17:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007-2008 Szymon Stefanek (pragma at kvirc dot net)
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

/**
* \file kvi_tal_filedialog_qt.h
* \author Szymon Stefanek
* \brief Qt class for application
*/

#include "kvi_settings.h"

#include <QFileDialog>

/**
* \class KviTalFileDialog
* \brief Toolkit Abstraction Layer: filedialog class
*/
class KVILIB_API KviTalFileDialog : public QFileDialog
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the filedialog object
	* \param szDirName The initial directory to display
	* \param szFilter The filter to use to display
	* \param pParent The parent object
	* \param pcName The caption name of the dialog
	* \param bModal Whether the dialog is modal
	* \return KviTalFileDialog
	*/
	KviTalFileDialog(const QString & szDirName, const QString & szFilter = QString::null, QWidget * pParent = 0, const char * pcName = 0, bool bModal = false);

	/**
	* \brief Destroys the filedialog object
	*/
	~KviTalFileDialog();
public:
	/**
	* \enum FileMode
	* \brief Indicate what the user may select in the file dialog
	*/
	enum FileMode {
		AnyFile,         /**< The name of a file, whether it exists or not */
		ExistingFile,    /**< The name of a single existing file */
		ExistingFiles,   /**< The names of zero or more existing files */
		Directory,       /**< The name of a directory. Both files and directories are displayed */
		DirectoryOnly    /**< The name of a directory. The file dialog will only display directories */
	};

	/**
	* \brief Sets the file mode
	* \param m The mode :)
	* \return void
	*/
	void setFileMode(FileMode m);

	/**
	* \brief Sets the current directory
	* \param szDirectory The directory
	* \return void
	*/
	void setDirectory(const QString & szDirectory);

	/**
	* \brief Returns an existing directory selected by the user
	* \param szDir The directory to display
	* \param szCaption The caption of the dialog
	* \param pParent The parent object
	* \return QString
	*/
	static QString getExistingDirectoryPath(const QString & szDir = QString::null, const QString & szCaption = QString::null, QWidget * pParent = 0){ return getExistingDirectory(pParent,szCaption,szDir); };
};

#endif // _KVI_TAL_FILEDIALOG_QT_H_
