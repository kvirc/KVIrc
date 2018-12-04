#ifndef _KVI_FILEDIALOG_H_
#define _KVI_FILEDIALOG_H_
//=============================================================================
//
//   File : KviFileDialog.h
//   Creation date : Mon Nov 20 2000 03:52:12 CEST by Szymon Stefanek
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

/**
* \file KviFileDialog.h
* \author Szymon Stefanek
* \brief File dialog helpers
*/

#ifdef Unsorted
#undef Unsorted
#endif

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviTalFileDialog.h"

#include <QStringList>

/**
* \class KviFileDialog
* \brief File dialog class
*/
class KVIRC_API KviFileDialog : public KviTalFileDialog
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the file dialog object
	* \param szDirName The directory where to use the dialog
	* \param szFilter The filter to use
	* \param pParent The parent widget
	* \param name The name of the dialog
	* \param bModal Whether to have a modal behaviour
	* \return KviFileDialog
	*/
	KviFileDialog(const QString & szDirName, const QString & szFilter = QString(), QWidget * pParent = nullptr, const char * name = nullptr, bool bModal = false);

	/**
	* \brief Destroys the file dialog object
	*/
	~KviFileDialog();

public:
	/**
	* \param szBuffer The buffer where to store the data
	* \param szCaption The caption of the dialog
	* \param szInitial The initial directory
	* \param szFilter The filter to use
	* \param bShowHidden Whether to show hidden files
	* \param bShowNative Whether to show native dialogs
	* \param pParent The parent widget
	* \return bool
	*/
	static bool askForOpenFileName(QString & szBuffer, const QString & szCaption, const QString & szInitial = QString(), const QString & szFilter = QString(), bool bShowHidden = false, bool bShowNative = true, QWidget * pParent = nullptr);

	/**
	* \param szBuffer The buffer where to store the data
	* \param szCaption The caption of the dialog
	* \param szInitial The initial directory
	* \param szFilter The filter to use
	* \param bShowHidden Whether to show hidden files
	* \param bConfirmOverwrite Whether to avoid the check for an already existing file being overwritten
	* \param bShowNative Whether to show native dialogs
	* \param pParent The parent widget
	* \return bool
	*/
	static bool askForSaveFileName(QString & szBuffer, const QString & szCaption, const QString & szInitial = QString(), const QString & szFilter = QString(), bool bShowHidden = false, bool bConfirmOverwrite = false, bool bShowNative = true, QWidget * pParent = nullptr);

	/**
	* \param szBuffer The buffer where to store the data
	* \param szCaption The caption of the dialog
	* \param szInitial The initial directory
	* \param szFilter The filter to use
	* \param bShowHidden Whether to show hidden files
	* \param bShowNative Whether to show native dialogs
	* \param pParent The parent widget
	* \return bool
	*/
	static bool askForDirectoryName(QString & szBuffer, const QString & szCaption, const QString & szInitial, const QString & szFilter = QString(), bool bShowHidden = false, bool bShowNative = true, QWidget * pParent = nullptr);

	/**
	* \param szBuffer The buffer where to store the data
	* \param szCaption The caption of the dialog
	* \param szInitial The initial directory
	* \param szFilter The filter to use
	* \param bShowHidden Whether to show hidden files
	* \param bShowNative Whether to show native dialogs
	* \param pParent The parent widget
	* \return bool
	*/
	static bool askForOpenFileNames(QStringList & szBuffer, const QString & szCaption, const QString & szInitial, const QString & szFilter = QString(), bool bShowHidden = false, bool bShowNative = true, QWidget * pParent = nullptr);
};

#endif //_KVI_FILEDIALOG_H_
