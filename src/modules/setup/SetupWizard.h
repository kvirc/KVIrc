#ifndef _SETUPWIZARD_H_
#define _SETUPWIZARD_H_
//=============================================================================
//
//   File : SetupWizard.h
//   Creation date : Sat Oct  6 02:06:51 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviSelectors.h"
#include "KviTalWizard.h"
#include "KviTalVBox.h"
#include "KviTalHBox.h"
#include "KviTalGroupBox.h"

#include <QLabel>
#include <QPixmap>
#include <QLineEdit>
#include <QRadioButton>
#include <QComboBox>
#include <QCheckBox>
#include <QGridLayout>

class SetupWizard;

class SetupPage : public QWidget
{
	Q_OBJECT
public:
	SetupPage(SetupWizard * w);
	~SetupPage();

public:
	KviTalVBox * m_pVBox;
	QLabel * m_pPixmapLabel;
	QLabel * m_pTextLabel;
};

#define THEME_APPLY_NONE 0
#define THEME_APPLY_HIRES 1
#define THEME_APPLY_LORES 2

class SetupWizard : public KviTalWizard
{
	Q_OBJECT
public:
	SetupWizard();
	~SetupWizard();

public:
	QPixmap * m_pLabelPixmap;

	SetupPage * m_pWelcome;
	SetupPage * m_pLicense;
	SetupPage * m_pDirectory;
	SetupPage * m_pIdentity;
	SetupPage * m_pTheme;
	SetupPage * m_pDesktopIntegration;
	SetupPage * m_pServers;

	// Theme
	QGroupBox * m_pThemeButtonGroup;

	QRadioButton * m_pThemeHiRes;
	QRadioButton * m_pThemeLoRes;
	QRadioButton * m_pThemeNone;

	// Welcome
	QCheckBox * m_pCreateUrlHandlers;
	QCheckBox * m_pCreateDesktopShortcut;

	//Dir
	QLineEdit * m_pDataPathEdit;
	QLineEdit * m_pIncomingPathEdit;
	KviTalGroupBox * m_pDirButtonGroup;
	QRadioButton * m_pDirUsePrev;
	QRadioButton * m_pDirUseNew;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	QRadioButton * m_pDirMakePortable;
#endif
	//QRadioButton  * m_pDirRestore;
	QLineEdit * m_pOldDataPathEdit;
	KviTalHBox * m_pOldPathBox;
	KviTalHBox * m_pNewPathBox;
	KviTalHBox * m_pNewIncomingBox;

	//Identify
	QComboBox * m_pAgeCombo;
	QComboBox * m_pGenderCombo;

	KviStringSelector * m_pNickSelector;
	KviStringSelector * m_pRealNameSelector;
	KviStringSelector * m_pLocationSelector;
	KviStringSelector * m_pLanguagesSelector;
//KviStringSelector * m_pOtherInfoSelector;

//Servers
//KviTalGroupBox * m_pServersButtonGroup;
//QRadioButton  * m_pServersChooseFromList;
//QRadioButton  * m_pServersSpecifyManually;
//QRadioButton  * m_pServersOpenIrcUrl;
//QRadioButton  * m_pServersLoadConfig;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	QString m_szMircServerIniFile;
	QCheckBox * m_pUseMircServerList;
#endif

	QString m_szServerConfigFile;
	QString m_szServerUrl;
	QString m_szServerHost;
	unsigned int m_uServerPort;

	KviStringSelector * m_pServerHostSelector;
	KviStringSelector * m_pServerUrlSelector;
	//KviFileSelector   * m_pServerConfigSelector;
	KviUIntSelector * m_pServerPortSelector;

protected:
	void makeLink();
	void setUrlHandlers();
	void showEvent(QShowEvent * e) override;
	void enableOrDisableIdentityPageNextButton();
public slots:
	void chooseOldDataPath();
	void chooseDataPath();
	void chooseIncomingPath();
	void accept() override;
	void reject() override;

	void oldDirClicked();
	void newDirClicked();
	void portableClicked();

	void nickSelectorTextChanged(const QString &);
	void oldDataTextChanged(const QString &);
	void newDataTextChanged(const QString &);
	void newIncomingTextChanged(const QString &);
};

#endif //_SETUPWIZARD_H_
