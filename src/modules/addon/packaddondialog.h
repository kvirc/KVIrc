#ifndef _PACKADDONDIALOG_H_
#define _PACKADDONDIALOG_H_
//=============================================================================
//
//   File : packaddondialog.h
//   Creation date : Sat 03 May 2008 01:40:44 by Elvio Basello
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008-2009 Elvio Basello <hellvis69 at netsons dot org>
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
* \file packaddondialog.h
* \author Elvio Basello
* \brief Addon package creator
*
* This set of classes allow users to create addon for KVIrc in a standard
* distributable *.kva packages.
*/

#include "kvi_settings.h"
#include "kvi_pointerlist.h"
#include "kvi_selectors.h"
#include "kvi_tal_textedit.h"

#include <QWizard>
#include <QWizardPage>

class QLineEdit;
class QLabel;

class KviPackAddonInfoPackageWidget;
class KviPackAddonFileSelectionWidget;
class KviPackAddonSaveSelectionWidget;
class KviPackAddonSummaryWidget;

/**
* \class KviPackAddonDialog
* \brief Wizard for addon creation
*/
class KviPackAddonDialog : public QWizard
{
	Q_OBJECT
public:
	/**
	* \brief Create the wizard object
	* \param pParent The parent widget
	* \return KviPackAddonDialog
	*/
	KviPackAddonDialog(QWidget * pParent);

	/**
	* \brief Destroys the wizard object
	*/
	~KviPackAddonDialog();
public:
	KviPackAddonInfoPackageWidget   * m_pPackAddonInfoPackageWidget;
	KviPackAddonFileSelectionWidget * m_pPackAddonFileSelectionWidget;
	KviPackAddonSaveSelectionWidget * m_pPackAddonSaveSelectionWidget;
	KviPackAddonSummaryWidget       * m_pPackAddonSummaryWidget;
protected:
	/**
	* \brief Runs the packAddon() function and closes the wizard
	* \return void
	*/
	virtual void accept();

	/**
	* \brief Create the addon package
	* \return bool
	*/
	bool packAddon();

	/**
	* \brief Ensures the source directory is complete
	* \param szSourceDir The sources' directory
	* \param pszError The buffer containing errors
	* \return bool
	*/
	bool checkDirTree(QString & szSourceDir, QString * pszError);

	/**
	* \brief Creates the installer file
	* \param szSourceDir The sources' directory
	* \param pszError The buffer containing errors
	* \return bool
	*/
	bool createInstaller(QString & szSourceDir, QString * pszError);
};

/**
* \class KviPackAddonInfoPackageWidget
* \brief Wizard page for addon informations
*/
class KviPackAddonInfoPackageWidget : public QWizardPage
{
	Q_OBJECT
public:
	/**
	* \brief Create the wizard package info page object
	* \param pParent The parent widget
	* \return KviPackAddonInfoPackageWidget
	*/
	KviPackAddonInfoPackageWidget(KviPackAddonDialog * pParent);

	/**
	* \brief Destroys the wizard package info page object
	*/
	~KviPackAddonInfoPackageWidget();
protected:
	QLineEdit       * m_pPackageAuthorEdit;
	QLineEdit       * m_pPackageNameEdit;
	QLineEdit       * m_pPackageVersionEdit;
	KviTalTextEdit  * m_pPackageDescriptionEdit;
	QLineEdit       * m_pPackageMinVersionEdit;
};

/**
* \class KviPackAddonFileSelectionWidget
* \brief Wizard page for directory source selection
*/
class KviPackAddonFileSelectionWidget : public QWizardPage
{
	Q_OBJECT
public:
	/**
	* \brief Create the wizard package source dir object
	* \param pParent The parent widget
	* \return KviPackAddonFileSelectionWidget
	*/
	KviPackAddonFileSelectionWidget(KviPackAddonDialog * pParent);

	/**
	* \brief Destroys the wizard package source dir object
	*/
	~KviPackAddonFileSelectionWidget();
protected:
	KviDirectorySelector * m_pDirPathSelector;
	KviFileSelector      * m_pPackageIconEdit;
	QString                m_szDirPath;
	QString                m_szPackageIcon;
};

/**
* \class KviPackAddonCreateInfoPackageWidget
* \brief Wizard page for saving package
*/
class KviPackAddonSaveSelectionWidget : public QWizardPage
{
	Q_OBJECT
public:
	/**
	* \brief Create the wizard save page object
	* \param pParent The parent widget
	* \return KviPackAddonSaveSelectionWidget
	*/
	KviPackAddonSaveSelectionWidget(KviPackAddonDialog * pParent);

	/**
	* \brief Destroys the wizard save page object
	*/
	~KviPackAddonSaveSelectionWidget();
protected:
	KviFileSelector * m_pSavePathSelector;
	QString           m_szSavePath;
protected:
	/**
	* \brief Perform initial tasks before showing the widget
	* \return void
	*/
	virtual void initializePage();
};

/**
* \class KviPackAddonSummaryWidget
* \brief Wizard page for showing informations inserted
*/
class KviPackAddonSummaryWidget : public QWizardPage
{
	Q_OBJECT
public:
	/**
	* \brief Create the wizard save page object
	* \param pParent The parent widget
	* \return KviPackAddonSaveSelectionWidget
	*/
	KviPackAddonSummaryWidget(KviPackAddonDialog * pParent);

	/**
	* \brief Destroys the wizard save page object
	*/
	~KviPackAddonSummaryWidget();
protected:
	KviPackAddonDialog * m_pParent;
	QLabel             * m_pLabelInfo;
	QLabel             * m_pLabelAuthor;
	QLabel             * m_pPackageName;
	QLabel             * m_pPackageVersion;
	QLabel             * m_pPackageDescription;
protected:
	/**
	* \brief Perform initial tasks before showing the widget
	* \return void
	*/
	virtual void initializePage();
};

#endif //!_PACKADDONDIALOG_H_
