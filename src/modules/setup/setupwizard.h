#ifndef _SETUPWIZARD_H_
#define _SETUPWIZARD_H_
//
//   File : setupwizard.h
//   Creation date : Sat Oct  6 02:06:51 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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


#include <qwizard.h>
#include "kvi_tal_vbox.h"
#include "kvi_tal_hbox.h"
#include <qlabel.h>
#include <qpixmap.h>
#include <qlineedit.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qcheckbox.h>

#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_selectors.h"

class KviSetupWizard;

class KviSetupPage : public KviTalHBox
{
	Q_OBJECT
public:
	KviSetupPage(KviSetupWizard * w);
	~KviSetupPage();
public:
	KviTalVBox * m_pVBox;
	QLabel * m_pPixmapLabel;
	QLabel * m_pTextLabel;
   
};


#define THEME_APPLY_NONE 0
#define THEME_APPLY_HIRES 1
#define THEME_APPLY_LORES 2

class KviSetupWizard : public QWizard
{
	Q_OBJECT
public:
	KviSetupWizard();
	~KviSetupWizard();
public:
	QPixmap       * m_pLabelPixmap;

	KviSetupPage  * m_pWelcome;
	KviSetupPage  * m_pLicense;
	KviSetupPage  * m_pDirectory;
	KviSetupPage  * m_pIdentity;
//	KviSetupPage  * m_pTheme;
	KviSetupPage  * m_pServers;
	
	// Theme
	QVButtonGroup * m_pThemeButtonGroup;
	QRadioButton  * m_pThemeHiRes;
	QRadioButton  * m_pThemeLoRes;
	QRadioButton  * m_pThemeNone;
	
	// Welcome
	QCheckBox     * m_pCreateUrlHandlers;
	QCheckBox     * m_pCreateDesktopShortcut;
	
	//Dir
	QLineEdit     * m_pDataPathEdit;
	QLineEdit     * m_pIncomingPathEdit;
	QVButtonGroup * m_pDirButtonGroup;
	QRadioButton  * m_pDirUsePrev;
	QRadioButton  * m_pDirUseNew;
#ifdef COMPILE_ON_WINDOWS
	QRadioButton  * m_pDirMakePortable;
#endif
	QRadioButton  * m_pDirRestore;
	QLineEdit     * m_pOldDataPathEdit;
	KviTalHBox         * m_pOldPathBox;
	KviTalHBox         * m_pNewPathBox;
	KviTalHBox         * m_pNewIncomingBox;
	
	//Identify
	QComboBox        * m_pAgeCombo;
	QComboBox        * m_pGenderCombo;
	
	KviStringSelector * m_pNickSelector;
	KviStringSelector * m_pRealNameSelector;
	KviStringSelector * m_pLocationSelector;
	KviStringSelector * m_pLanguagesSelector;
	KviStringSelector * m_pOtherInfoSelector;
	
	//Servers
	QVButtonGroup * m_pServersButtonGroup;
	QRadioButton  * m_pServersChooseFromList;
	QRadioButton  * m_pServersSpecifyManually;
	QRadioButton  * m_pServersOpenIrcUrl;
	QRadioButton  * m_pServersLoadConfig;
	
	QString		    m_szServerConfigFile;
	QString		    m_szServerUrl;
	QString		    m_szServerHost;
	unsigned int        m_uServerPort;
	
	KviStringSelector * m_pServerHostSelector;
	KviStringSelector * m_pServerUrlSelector;
	KviFileSelector   * m_pServerConfigSelector;
	KviUIntSelector   *m_pServerPortSelector;
	
protected:
	void makeLink();
	void setUrlHandlers();
public slots:
	void chooseOldDataPath();
	void chooseDataPath();
	void chooseIncomingPath();
	virtual void accept();
	virtual void reject();
	
	void oldDirClicked();
	void newDirClicked();
	
	void oldDataTextChanged ( const QString & );
	void newDataTextChanged ( const QString & );
	void newIncomingTextChanged ( const QString & );
};


#endif //_SETUPWIZARD_H_
