#ifndef _PACKADDONDIALOG_H_
#define _PACKADDONDIALOG_H_
//=============================================================================
//
//   File : PackAddonDialog.h
//   Creation date : Sat 03 May 2008 01:40:44 by Elvio Basello
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008-2009 Elvio Basello <hellvis69 at netsons dot org>
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
* \file PackAddonDialog.h
* \author Elvio Basello
* \brief Addon package creator
*
* This set of classes allow users to create addon for KVIrc in a standard
* distributable *.kva packages.
*/

#include "kvi_settings.h"
#include "KviPointerList.h"
#include "KviSelectors.h"

#include <QWizard>
#include <QWizardPage>

class QTextEdit;
class QLineEdit;
class QLabel;

class KviPackageWriter;

class PackAddonInfoPackageWidget;
class PackAddonFileSelectionWidget;
class PackAddonSaveSelectionWidget;
class PackAddonSummaryInfoWidget;
class PackAddonSummaryFilesWidget;

/**
* \class PackAddonDialog
* \brief Wizard for addon creation
*/
class PackAddonDialog : public QWizard
{
	Q_OBJECT
public:
	/**
	* \brief Creates the wizard object
	* \param pParent The parent widget
	* \return PackAddonDialog
	*/
	PackAddonDialog(QWidget * pParent);

	/**
	* \brief Destroys the wizard object
	*/
	~PackAddonDialog();

protected:
	PackAddonInfoPackageWidget * m_pPackAddonInfoPackageWidget;
	PackAddonFileSelectionWidget * m_pPackAddonFileSelectionWidget;
	PackAddonSaveSelectionWidget * m_pPackAddonSaveSelectionWidget;
	PackAddonSummaryInfoWidget * m_pPackAddonSummaryInfoWidget;
	PackAddonSummaryFilesWidget * m_pPackAddonSummaryFilesWidget;
	QString m_szAuthor;
	QString m_szName;
	QString m_szVersion;
	QString m_szDescription;
	QString m_szMinVersion;
	QString m_szIcon;
	QString m_szDirPath;
	QString m_szSavePath;

protected:
	/**
	* \brief Runs the packAddon() function and closes the wizard
	* \return void
	*/
	void accept() override;

	/**
	* \brief Creates the addon package
	* \return bool
	*/
	bool packAddon();

#if 0
	/**
	* \brief Ensures the sources directory is complete
	* \param pszError The buffer containing errors
	* \return bool
	*/
	bool checkDirTree(QString * pszError);

	/**
	* \brief Creates the installer file
	* \param pszError The buffer containing errors
	* \return bool
	*/
	bool createInstaller(QString * pszError);
#endif
};

/**
* \class PackAddonInfoPackageWidget
* \brief Wizard page for addon information
*/
class PackAddonInfoPackageWidget : public QWizardPage
{
	Q_OBJECT
public:
	/**
	* \brief Create the wizard package info page object
	* \param pParent The parent widget
	* \return PackAddonInfoPackageWidget
	*/
	PackAddonInfoPackageWidget(PackAddonDialog * pParent);

	/**
	* \brief Destroys the wizard package info page object
	*/
	~PackAddonInfoPackageWidget();

protected:
	QLineEdit * m_pPackageAuthorEdit;
	QLineEdit * m_pPackageNameEdit;
	QLineEdit * m_pPackageVersionEdit;
	QLineEdit * m_pPackageDescriptionEdit;
	QLineEdit * m_pPackageMinVersionEdit;
};

/**
* \class PackAddonFileSelectionWidget
* \brief Wizard page for directory source selection
*/
class PackAddonFileSelectionWidget : public QWizardPage
{
	Q_OBJECT
public:
	/**
	* \brief Create the wizard package source dir object
	* \param pParent The parent widget
	* \return PackAddonFileSelectionWidget
	*/
	PackAddonFileSelectionWidget(PackAddonDialog * pParent);

	/**
	* \brief Destroys the wizard package source dir object
	*/
	~PackAddonFileSelectionWidget();

protected:
	KviDirectorySelector * m_pDirPathSelector;
	KviFileSelector * m_pPackageImageEdit;
	QString m_szDirPath;
	QString m_szPackageImage;
};

/**
* \class PackAddonSaveSelectionWidget
* \brief Wizard page for saving package
*/
class PackAddonSaveSelectionWidget : public QWizardPage
{
	Q_OBJECT
public:
	/**
	* \brief Create the wizard save page object
	* \param pParent The parent widget
	* \return PackAddonSaveSelectionWidget
	*/
	PackAddonSaveSelectionWidget(PackAddonDialog * pParent);

	/**
	* \brief Destroys the wizard save page object
	*/
	~PackAddonSaveSelectionWidget();

protected:
	KviFileSelector * m_pSavePathSelector;
	QString m_szFilePath;

protected:
	/**
	* \brief Perform initial tasks before showing the widget
	* \return void
	*/
	void initializePage() override;
};

/**
* \class PackAddonSummaryInfoWidget
* \brief Wizard page for showing information inserted
*/
class PackAddonSummaryInfoWidget : public QWizardPage
{
	Q_OBJECT
public:
	/**
	* \brief Create the wizard summary info page object
	* \param pParent The parent widget
	* \return PackAddonSummaryInfoWidget
	*/
	PackAddonSummaryInfoWidget(PackAddonDialog * pParent);

	/**
	* \brief Destroys the wizard summary info page object
	*/
	~PackAddonSummaryInfoWidget();

protected:
	QLabel * m_pLabelInfo;

protected:
	/**
	* \brief Perform initial tasks before showing the widget
	* \return void
	*/
	void initializePage() override;
};

class PackAddonSummaryFilesWidget : public QDialog
{
	Q_OBJECT
public:
	PackAddonSummaryFilesWidget(PackAddonDialog * pParent);
	~PackAddonSummaryFilesWidget();

protected:
	QTextEdit * m_pFiles;
	QString m_szPath;

public:
	void setPath(QString & szPath) { m_szPath = szPath; };
protected:
	void showEvent(QShowEvent *) override;
protected slots:
	void accept() override;
	void reject() override;
};

#endif //!_PACKADDONDIALOG_H_
