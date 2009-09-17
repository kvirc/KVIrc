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

//class KviPackAddonDialog;
class KviPackAddonInfoPackageWidget;
class KviPackAddonFileSelectionWidget;
class KviPackAddonSaveSelectionWidget;
class KviPackAddonSummaryWidget;

#if 0
/**
* \class KviPackAddonDialogData
* \brief Class for storing addon data
*/
class KviPackAddonData
{
public:
	/**
	* \brief Creates the addon data object
	* \param pParent The parent wizard dialog
	* \return KviPackAddonData
	*/
	KviPackAddonData(KviPackAddonDialog * pParent);

	/**
	* \brief Destroys the addon data object
	*/
	~KviPackAddonData();
protected:
	QString m_szAuthor;
	QString m_szName;
	QString m_szVersion;
	QString m_szDescription;
	QString m_szMinVersion;
public:
	/**
	* \brief Sets the package author
	* \param szAuthor The author name
	* \return void
	*/
	void setAuthor(QString & szAuthor){ m_szAuthor = szAuthor; };

	/**
	* \brief Sets the package name
	* \param szName The package name
	* \return void
	*/
	void setName(QString & szName){ m_szName = szName; };

	/**
	* \brief Sets the package version
	* \param szVersion The author name
	* \return void
	*/
	void setVersion(QString & szVersion){ m_szVersion = szVersion; };

	/**
	* \brief Sets the package description
	* \param szDescription
	* \return void
	*/
	void setDescription(QString & szDescription){ m_szDescription = szDescription; };

	/**
	* \brief Sets the package minimum version
	* \param szMinVersion
	* \return void
	*/
	void setMinVersion(QString & szMinVersion){ m_szMinVersion = szMinVersion; };

	/**
	* \brief Returns the author name
	* \return QString
	*/
	QString author(){ return m_szAuthor; };

	/**
	* \brief Returns the package name
	* \return QString
	*/
	QString name(){ return m_szName; };

	/**
	* \brief Returns the package version
	* \return QString
	*/
	QString version(){ return m_szVersion; };

	/**
	* \brief Returns the package description
	* \return QString
	*/
	QString description(){ return m_szDescription; };

	/**
	* \brief Returns the minimum version allowed of KVIrc
	* \return QString
	*/
	QString minVersion(){ return m_szMinVersion; };
};
#endif
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
	//KviPackAddonData                * m_pData;
#if 0
protected:
	QString m_szPackagePath;
public:
	/**
	* \brief Returns the author's info selection widget
	* \return KviPackAddonCreateInfoPackageWidget *
	*/
	KviPackAddonInfoPackageWidget * infoWidget(){ return m_pPackAddonInfoPackageWidget; };

	/**
	* \brief Returns the directory selection widget
	* \return KviPackAddonFileSelectionWidget *
	*/
	KviPackAddonFileSelectionWidget * fileWidget(){ return m_pPackAddonFileSelectionWidget; };

	/**
	* \brief Returns the saving selection widget
	* \return KviPackAddonSaveSelectionWidget *
	*/
	KviPackAddonSaveSelectionWidget * saveWidget(){ return m_pPackAddonSaveSelectionWidget; };

	/**
	* \brief Returns the addon data object
	* \return KviPackAddonData *
	*/
	KviPackAddonData * data(){ return m_pData; };
#endif
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
public:
	QString m_szPackageName;
	QString m_szPackageAuthor;
	QString m_szPackageDescription;
	QString m_szPackageVersion;
	QString m_szPackageMinVersion;
protected:
	QLineEdit       * m_pPackageAuthorEdit;
	QLineEdit       * m_pPackageNameEdit;
	QLineEdit       * m_pPackageVersionEdit;
	KviTalTextEdit  * m_pPackageDescriptionEdit;
	QLineEdit       * m_pPackageMinVersionEdit;
#if 0
public:
	/**
	* \brief Returns the author name
	* \return QString
	*/
	QString packageAuthor(){ return m_pPackageAuthorEdit->text(); };

	/**
	* \brief Returns the package name
	* \return QString
	*/
	QString packageName(){ return m_pPackageNameEdit->text(); };

	/**
	* \brief Returns the package version
	* \return QString
	*/
	QString packageVersion(){ return m_pPackageVersionEdit->text(); };

	/**
	* \brief Returns the package description
	* \return QString
	*/
	QString packageDescription(){ return m_pPackageDescriptionEdit->toPlainText(); };

	/**
	* \brief Returns the minimum version allowed of KVIrc
	* \return QString
	*/
	QString packageMinVersion(){ return m_pPackageMinVersionEdit->text(); };
#endif
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
#if 0
/*
	KviDirectorySelector * m_pSourcePathSelector;
	KviDirectorySelector * m_pConfigPathSelector;
	KviDirectorySelector * m_pImagePathSelector;
	KviDirectorySelector * m_pLocalePathSelector;
	KviDirectorySelector * m_pHelpPathSelector;
	KviDirectorySelector * m_pSoundPathSelector;
	KviFileSelector      * m_pInstallPathSelector;
	QString                szSourcePath;
	QString                szConfigPath;
	QString                szImagePath;
	QString                szLocalePath;
	QString                szHelpPath;
	QString                szSoundPath;
	QString                szInstallPath;
*/
public:
	/**
	* \brief Returns the source directory
	* \return QString
	*/
	QString dirPath(){ m_pDirPathSelector->commit(); return m_szDirPath; };

	/**
	* \brief Returns the path to the package icon
	* \return QString
	*/
	QString packageIcon(){ m_pPackageIconEdit->commit(); return m_szPackageIcon; };
/*
	QString sourcePath(){ m_pSourcePathSelector->commit(); return szSourcePath; };
	QString configPath(){ m_pConfigPathSelector->commit(); return szConfigPath; };
	QString imagePath(){ m_pImagePathSelector->commit(); return szImagePath; };
	QString localePath(){ m_pLocalePathSelector->commit(); return szLocalePath; };
	QString helpPath(){ m_pHelpPathSelector->commit(); return szHelpPath; };
	QString soundPath(){ m_pSoundPathSelector->commit(); return szSoundPath; };
	QString installerPath(){ m_pInstallPathSelector->commit(); return szInstallPath; };
*/
#endif
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
#if 0
public:
	/**
	* \brief Returns the saving path
	* \return QString
	*/
	QString savePath(){ m_pSavePathSelector->commit(); return szSavePath; };
#endif
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
