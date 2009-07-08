#ifndef _PACKADDONDIALOG_H_
#define _PACKADDONDIALOG_H_
//=============================================================================
//
//   File : packaddondialog.h
//   Creation date : Sat 03 May 2008 01:40:44 by Elvio Basello
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dot org>
//   Copyright (C) 2008 Alessandro Carbone <elfonol at gmail dot com>
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

#include "kvi_settings.h"
#include "kvi_pointerlist.h"
#include "kvi_selectors.h"
#include "kvi_tal_wizard.h"
#include "kvi_tal_textedit.h"

#include <QLineEdit>
#include <QLabel>
#include <QWidget>

class KviPackAddonCreateInfoPackageWidget;
class KviPackAddonFileSelectionWidget;
class KviPackAddonSaveSelectionWidget;
class KviPackAddonInfoWidget;

class KviPackAddonDialog : public KviTalWizard
{
	Q_OBJECT
public:
	KviPackAddonDialog(QWidget * pParent);
	~KviPackAddonDialog(){};
	KviPackAddonCreateInfoPackageWidget * m_pPackAddonInfoCreateWidget;
	KviPackAddonFileSelectionWidget     * m_pPackAddonFileSelectionWidget;
	KviPackAddonSaveSelectionWidget     * m_pPackAddonSaveSelectionWidget;
	KviPackAddonInfoWidget              * m_pPackAddonInfoWidget;
protected:
	QString m_szPackagePath;
protected:
	virtual void accept();
	bool packAddon();
};

class KviPackAddonCreateInfoPackageWidget : public QWidget
{
	Q_OBJECT
public:
	KviPackAddonCreateInfoPackageWidget(KviPackAddonDialog *pParent);
	~KviPackAddonCreateInfoPackageWidget(){};
	QString authorName(){return m_pAuthorNameEdit->text();};
	QString packageName(){return m_pPackageNameEdit->text();};
	QString packageVersion(){return m_pPackageVersionEdit->text();};
	QString packageDescription(){return m_pPackageDescriptionEdit->toPlainText();};
protected:
	QLineEdit      * m_pAuthorNameEdit;
	QLineEdit      * m_pPackageNameEdit;
	QLineEdit      * m_pPackageVersionEdit;
	KviTalTextEdit * m_pPackageDescriptionEdit;
};

class KviPackAddonFileSelectionWidget : public QWidget
{
	Q_OBJECT
public:
	KviPackAddonFileSelectionWidget(KviPackAddonDialog *pParent);
	~KviPackAddonFileSelectionWidget(){};
	QString sourcePath(){m_pSourcePathSelector->commit();return szSourcePath;};
	QString configPath(){m_pConfigPathSelector->commit();return szConfigPath;};
	QString imagePath(){m_pImagePathSelector->commit();return szImagePath;};
	QString localePath(){m_pLocalePathSelector->commit();return szLocalePath;};
	QString helpPath(){m_pHelpPathSelector->commit();return szHelpPath;};
	QString soundPath(){m_pSoundPathSelector->commit();return szSoundPath;};
	QString installerPath(){m_pInstallPathSelector->commit();return szInstallPath;};
protected:
	KviDirectorySelector * m_pSourcePathSelector;
	KviDirectorySelector * m_pConfigPathSelector;
	KviDirectorySelector * m_pImagePathSelector;
	KviDirectorySelector * m_pLocalePathSelector;
	KviDirectorySelector * m_pHelpPathSelector;
	KviDirectorySelector * m_pSoundPathSelector;
	KviFileSelector      * m_pInstallPathSelector;
	QString szSourcePath;
	QString szConfigPath;
	QString szImagePath;
	QString szLocalePath;
	QString szHelpPath;
	QString szSoundPath;
	QString szInstallPath;
};

class KviPackAddonSaveSelectionWidget : public QWidget
{
	Q_OBJECT
public:
	KviPackAddonSaveSelectionWidget(KviPackAddonDialog *pParent);
	~KviPackAddonSaveSelectionWidget(){};
	QString savePath(){m_pSavePathSelector->commit();return szSavePath;};
protected:
	KviFileSelector * m_pSavePathSelector;
	QString szSavePath;
};

class KviPackAddonInfoWidget : public QWidget
{
public:
	KviPackAddonInfoWidget(KviPackAddonDialog *);
	~KviPackAddonInfoWidget(){};
protected:
	KviPackAddonDialog * m_pParent;
	QLabel * m_pLabelInfo;
	QLabel * m_pLabelAuthor;
	QLabel * m_pPackageName;
	QLabel * m_pPackageVersion;
	QLabel * m_pPackageDescription;
	void showEvent(QShowEvent *event);
};


#endif //!_PACKADDONDIALOG_H_
