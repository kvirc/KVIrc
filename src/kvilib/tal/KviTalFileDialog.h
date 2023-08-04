#ifndef _KVI_TAL_FILEDIALOG_H_
#define _KVI_TAL_FILEDIALOG_H_
//=============================================================================
//
//   File : KviTalFileDialog.h
//   Creation date : Thu Sep 11 2003 04:41:08 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviTalFileDialog.h
* \author Szymon Stefanek
* \brief Toolkit Abstraction Layer class
*/

#include "kvi_settings.h"

#ifdef None
#undef None
#endif // otherwise gcc freaks out somewhere inside the include chain of KFileDialog

/**
* \class KviTalFileDialog
* \brief Toolkit Abstraction Layer: filedialog class
*/

#include <QFileDialog>
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
	KviTalFileDialog(const QString & szDirName, const QString & szFilter = QString(), QWidget * pParent = nullptr, const char * pcName = nullptr, bool bModal = false);

	/**
	* \brief Destroys the filedialog object
	*/
	~KviTalFileDialog();

public:
	/**
	* \enum FileMode
	* \brief Indicate what the user may select in the file dialog
	*/
	enum FileMode
	{
		AnyFile,       /**< The name of a file, whether it exists or not */
		ExistingFile,  /**< The name of a single existing file */
		ExistingFiles, /**< The names of zero or more existing files */
		Directory,     /**< The name of a directory. Both files and directories are displayed */
		DirectoryOnly  /**< The name of a directory. The file dialog will only display directories */
	};

	/**
	* \brief Sets the file mode
	* \param m The mode :)
	* \return void
	*/
	void setFileMode(FileMode m);

	/**
	* \brief Returns an existing directory selected by the user
	* \param szDir The directory to display
	* \param szCaption The caption of the dialog
	* \param pParent The parent object
	* \return QString
	*/
	static QString getExistingDirectoryPath(const QString & szDir = QString(), const QString & szCaption = QString(), QWidget * pParent = nullptr)
	{
		return getExistingDirectory(pParent, szCaption, szDir);
	};
};

#endif // _KVI_TAL_FILEDIALOG_H_
