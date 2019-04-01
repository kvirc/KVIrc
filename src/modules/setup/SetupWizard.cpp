//=============================================================================
//
//   File : SetupWizard.cpp
//   Creation date : Sat Oct  6 02:06:53 2001 GMT by Szymon Stefanek
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
//==============================================================================

bool g_bFoundMirc;

#include "SetupWizard.h"

#include "KviApplication.h"
#include "KviLocale.h"
#include "KviFileUtils.h"
#include "kvi_defaults.h"
#include "KviMessageBox.h"
#include "KviTalFileDialog.h"
#include "KviQString.h"
#include "KviEnvironment.h"
#include "KviOptions.h"
#include "KviConfigurationFile.h"
#include "KviTalHBox.h"

#include <QTextEdit>
#include <QMessageBox>
#include <QDir>
#include <QPushButton>
#include <QValidator>
#include <QTextCodec>
#include <QLayout>
#include <QDesktopWidget>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <winnls.h>  // for MultiByteToWideChar
#include <objbase.h> // CoCreateInstance, CoInitialize & CO.
#include <shlobj.h>  // IShellLink IPersistFile & CO.
#else
#include <unistd.h> // for symlink()
#endif

// libkvisetup.cpp
extern QString g_szChoosenIncomingDirectory;
extern int g_iThemeToApply;
extern bool bNeedToApplyDefaults;
extern unsigned int uPort;
extern QString szHost;
extern QString szUrl;
extern QString szMircServers;
extern QString szMircIni;

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#define KVI_LOCAL_KVIRC_SUBDIRECTORY_NAME "KVIrc4"
#else
#define KVI_LOCAL_KVIRC_SUBDIRECTORY_NAME ".config/KVIrc"
#endif

SetupPage::SetupPage(SetupWizard * w)
    : QWidget(w)
{
	QGridLayout * g = new QGridLayout(this);

	//setBackgroundColor(QColor(255,0,0));

	// we need this to set localized text on buttons (see QT doc/ KviTalWizard class)
	w->KviTalWizard::backButton()->setText(__tr2qs("< &Back"));
	w->KviTalWizard::nextButton()->setText(__tr2qs("&Next >"));
	w->KviTalWizard::finishButton()->setText(__tr2qs("Finish"));
	w->KviTalWizard::cancelButton()->setText(__tr2qs("Cancel"));
	//w->KviTalWizard::helpButton()->setText(__tr2qs("Help"));

	m_pPixmapLabel = new QLabel(this);
	g->addWidget(m_pPixmapLabel, 0, 0);

	m_pPixmapLabel->setPixmap(*(w->m_pLabelPixmap));
	m_pPixmapLabel->setFixedSize(w->m_pLabelPixmap->size());
	m_pPixmapLabel->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	m_pPixmapLabel->setMargin(0);

	g->setSpacing(8);
	g->setMargin(0);

	m_pVBox = new KviTalVBox(this);
	m_pVBox->setSpacing(4);
	m_pVBox->setMargin(0);
	//m_pVBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding));
	//m_pVBox->setBackgroundColor(QColor(0,80,0));
	//m_pVBox->setMaximumHeight(450);
	g->addWidget(m_pVBox, 0, 1);

	g->setColumnStretch(1, 1);

	QLabel * l = new QLabel(m_pVBox);
	l->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QPalette pal = l->palette();
	pal.setColor(l->backgroundRole(), QColor(48, 48, 48));    //aka #303030
	pal.setColor(l->foregroundRole(), QColor(255, 255, 255)); //aka #FFFFFF
	l->setPalette(pal);
	l->setText("<h1>&nbsp;KVIrc " KVI_VERSION "</h1>");
	l->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	l->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	l->setAutoFillBackground(true);
	l->setMargin(0);

	m_pTextLabel = new QLabel(m_pVBox);
	m_pTextLabel->setWordWrap(true);
	m_pTextLabel->setAlignment(Qt::AlignJustify | Qt::AlignTop);
	m_pVBox->setStretchFactor(m_pTextLabel, 1);
}

SetupPage::~SetupPage()
    = default;

SetupWizard::SetupWizard()
    : KviTalWizard(nullptr)
{
	setWindowTitle(__tr2qs("Setup Wizard - KVIrc"));

	setWindowModality(Qt::NonModal); // non modal, otherwise the dialogs we show will not be usable

	g_bFoundMirc = false;
	QString szLabelText;

	QString szImagePath;
	g_pApp->getGlobalKvircDirectory(szImagePath, KviApplication::Pics, "kvi_setup_label.png");

	m_pLabelPixmap = new QPixmap(szImagePath);
	if(m_pLabelPixmap->isNull())
	{
		delete m_pLabelPixmap;
		m_pLabelPixmap = new QPixmap(250, 450);
	}

	//
	// Welcome
	//

	m_pWelcome = new SetupPage(this);

	// here we go...
	QString szText = __tr2qs("<p>"
	                         "<h2>Welcome :)</h2>"
	                         "This is your first time running this version of KVIrc.<br>"
	                         "This wizard will guide you through the few steps "
	                         "required to complete the setup.<br><br>"
	                         "If you had a previous version of KVIrc installed, no worries. You will "
	                         "have a chance to preserve the old configuration."
	                         "</p>"
	                         "<p>Click \"<b>Next</b>\" to proceed.</p>");

	m_pWelcome->m_pTextLabel->setText(szText);

	addPage(m_pWelcome, __tr2qs("Welcome to KVIrc"));

	setBackEnabled(m_pWelcome, false);
	setHelpEnabled(m_pWelcome, false);

//
// License
//

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW) //it have been already shown by installer
	SetupPage * m_pLicense = new SetupPage(this);
	m_pLicense->m_pTextLabel->setText(__tr2qs(
	    "<p>All of the files in this distribution are covered by the GPL. "
	    "In human terms this can be read as follows:<br>"
	    "<ul>"
	    "<li><b>KVIrc is free</b>, use it, have fun! <b>:)</b></li>"
	    "<li>If you use <b>any</b> part of KVIrc in your own project, you <b>must</b> release that project under the same license.</li>"
	    "</ul></p>"
	    "<p>The \"legalese\" version of the license is shown in the box below.</p>"));

	QTextEdit * ed = new QTextEdit(m_pLicense->m_pVBox);
	ed->setReadOnly(true);
	ed->setWordWrapMode(QTextOption::NoWrap);
	QString szLicense;
	QString szLicensePath;
	g_pApp->getGlobalKvircDirectory(szLicensePath, KviApplication::License, "ABOUT-LICENSE");
	if(!KviFileUtils::loadFile(szLicensePath, szLicense))
	{
		szLicense = __tr("Oops! Can't find the license file.\n"
		                 "It MUST be included in the distribution...\n"
		                 "Please report to <pragma at kvirc dot net>");
	}
	ed->setText(szLicense);

	m_pLicense->m_pVBox->setStretchFactor(ed, 1);

	addPage(m_pLicense, __tr2qs("Dreaded License Agreement"));

	setHelpEnabled(m_pLicense, false);
#else
	m_pLicense = nullptr;
#endif
	//
	// Directories
	//

	m_pDirectory = new SetupPage(this);

	m_pDirectory->m_pTextLabel->setText(__tr2qs("<p>Please choose a folder for "
	                                            "KVIrc to store its settings and other data, and another for downloaded files. "
	                                            "Make sure that you have permission to write to both folders.<br><br>"
	                                            "The suggested locations are fine in most cases. If you don't know what "
	                                            "this is about, just click \"<b>Next</b>\".<br><br>"
	                                            "If you had a previous version of KVIrc installed, you can choose "
	                                            "the existing directory for the settings, and your configuration "
	                                            "will be preserved."
	                                            "</p>"));

	QString tmp;

	m_pDirButtonGroup = new KviTalGroupBox(Qt::Horizontal, __tr2qs("Store configuration in folder"), m_pDirectory->m_pVBox);
	m_pDirUsePrev = new QRadioButton(__tr2qs("Use settings folder from previous installation"), m_pDirButtonGroup);
	connect(m_pDirUsePrev, SIGNAL(clicked()), this, SLOT(oldDirClicked()));

	m_pOldPathBox = new KviTalHBox(m_pDirButtonGroup);
	m_pOldDataPathEdit = new QLineEdit(m_pOldPathBox);
	connect(m_pOldDataPathEdit, SIGNAL(textChanged(const QString &)), this, SLOT(oldDataTextChanged(const QString &)));

	QPushButton * pb = new QPushButton(__tr2qs("&Browse..."), m_pOldPathBox);
	connect(pb, SIGNAL(clicked()), this, SLOT(chooseOldDataPath()));
	m_pOldPathBox->setSpacing(3);
	m_pOldPathBox->setStretchFactor(m_pOldDataPathEdit, 1);

	m_pDirUseNew = new QRadioButton(__tr2qs("Use new settings folder"), m_pDirButtonGroup);
	connect(m_pDirUseNew, SIGNAL(clicked()), this, SLOT(newDirClicked()));

	QLabel * l = new QLabel(m_pDirButtonGroup);
	l->setText(__tr2qs("Settings folder:"));

	m_pNewPathBox = new KviTalHBox(m_pDirButtonGroup);
	m_pDataPathEdit = new QLineEdit(m_pNewPathBox);

	pb = new QPushButton(__tr2qs("&Browse..."), m_pNewPathBox);
	connect(pb, SIGNAL(clicked()), this, SLOT(chooseDataPath()));

	m_pNewPathBox->setSpacing(3);
	m_pNewPathBox->setStretchFactor(m_pDataPathEdit, 1);

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	tmp = QTextCodec::codecForLocale()->toUnicode(getenv("APPDATA"));
	if(tmp.isEmpty())
		tmp = QDir::homePath();
#else
	tmp = QDir::homePath();
#endif //COMPILE_ON_WINDOWS
	KviQString::ensureLastCharIs(tmp, KVI_PATH_SEPARATOR_CHAR);
	tmp.append(KVI_LOCAL_KVIRC_SUBDIRECTORY_NAME);
	KviFileUtils::adjustFilePath(tmp);
	m_pDataPathEdit->setText(tmp);

	l = new QLabel(m_pDirButtonGroup);
	l->setText(__tr2qs("Download files to folder:"));

	m_pNewIncomingBox = new KviTalHBox(m_pDirButtonGroup);

	m_pIncomingPathEdit = new QLineEdit(m_pNewIncomingBox);

	pb = new QPushButton(__tr2qs("&Browse..."), m_pNewIncomingBox);
	connect(pb, SIGNAL(clicked()), this, SLOT(chooseIncomingPath()));

	m_pNewIncomingBox->setSpacing(3);
	m_pNewIncomingBox->setStretchFactor(m_pIncomingPathEdit, 1);

	tmp = QDir::homePath();
	KviQString::ensureLastCharIs(tmp, KVI_PATH_SEPARATOR_CHAR);
	tmp.append(KVI_DEFAULT_INCOMING_SUBDIRECTORY_NAME);
	KviFileUtils::adjustFilePath(tmp);
	m_pIncomingPathEdit->setText(tmp);

	m_pDirUseNew->toggle();

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	m_pDirMakePortable = new QRadioButton(__tr2qs("All settings in shared program folder (portable)"), m_pDirButtonGroup);
	connect(m_pDirMakePortable, SIGNAL(clicked()), this, SLOT(portableClicked()));
#endif
	// Pragma: Unused, takes only space.
	//m_pDirRestore = new QRadioButton(__tr2qs("Restore from backup archive"),m_pDirButtonGroup);
	//m_pDirRestore->setEnabled(false);

	//l = new QLabel(m_pDirectory->m_pVBox,"<b> </b>");

	//m_pDirectory->m_pVBox->setStretchFactor(m_pDirectory->m_pTextLabel,1);

	addPage(m_pDirectory, __tr2qs("Application Folders"));
	setHelpEnabled(m_pDirectory, false);

	connect(m_pDataPathEdit, SIGNAL(textChanged(const QString &)), this, SLOT(newDataTextChanged(const QString &)));
	connect(m_pIncomingPathEdit, SIGNAL(textChanged(const QString &)), this, SLOT(newIncomingTextChanged(const QString &)));

	//
	// Identity
	//

	m_pIdentity = new SetupPage(this);

	m_pIdentity->m_pTextLabel->setText(__tr2qs("Please choose a nickname.<br><br>"
	                                           "Your nickname is the name that other IRC users will know you by. "
	                                           "It can't contain spaces or punctuation. Some IRC networks will shorten your nickname if it is more than 32 characters "
	                                           "long.<br><br>"
	                                           "If in doubt, just enter the first nick that comes to mind. "
	                                           "You will be able to change it later in the Identity properties, or with the /NICK command."));

	KviTalGroupBox * gbox = new KviTalGroupBox(Qt::Horizontal, QString(), m_pIdentity->m_pVBox);

	m_pNickSelector = new KviStringSelector(gbox, __tr2qs("Nickname:"), &(KVI_OPTION_STRING(KviOption_stringNickname1)), true);
	m_pNickSelector->setMinimumLabelWidth(120);
	m_pNickSelector->setMargin(0);
	QObject::connect(m_pNickSelector->lineEdit(), SIGNAL(textChanged(const QString &)), this, SLOT(nickSelectorTextChanged(const QString &)));

	QValidator * v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*"), gbox);
	m_pNickSelector->setValidator(v);

	QString szOptionalCtcpUserInfo = __tr2qs("This field is optional and will appear as part of the CTCP USERINFO reply.");
	QString szTrailing = "<br><br>" + szOptionalCtcpUserInfo;

	gbox = new KviTalGroupBox(Qt::Horizontal, __tr2qs("Profile"), m_pIdentity->m_pVBox);

	m_pRealNameSelector = new KviStringSelector(gbox, __tr2qs("Real name:"), &(KVI_OPTION_STRING(KviOption_stringRealname)), true);
	m_pRealNameSelector->setMinimumLabelWidth(120);
	m_pRealNameSelector->setMargin(0);

	KviTalHBox * hb = new KviTalHBox(gbox);
	hb->setSpacing(4);
	hb->setMargin(0);

	l = new QLabel(__tr2qs("Age:"), hb);
	l->setMinimumWidth(120);

	m_pAgeCombo = new QComboBox(hb);

	m_pAgeCombo->insertItem(0, __tr2qs("Unspecified"));
	unsigned int i;
	for(i = 1; i < 120; i++)
	{
		QString tmp;
		tmp.setNum(i);
		m_pAgeCombo->insertItem(m_pAgeCombo->count(), tmp);
	}

	bool bOk;
	i = KVI_OPTION_STRING(KviOption_stringCtcpUserInfoAge).toUInt(&bOk);
	if(!bOk)
		i = 0;
	if(i > 120)
		i = 120;
	m_pAgeCombo->setCurrentIndex(i);

	hb->setStretchFactor(m_pAgeCombo, 1);

	hb = new KviTalHBox(gbox);
	hb->setSpacing(4);
	hb->setMargin(0);

	l = new QLabel(__tr2qs("Gender:"), hb);
	l->setMinimumWidth(120);

	m_pGenderCombo = new QComboBox(hb);

	m_pGenderCombo->insertItem(0, __tr2qs("Unspecified"));
	m_pGenderCombo->insertItem(1, __tr2qs("Female"));
	m_pGenderCombo->insertItem(2, __tr2qs("Male"));

	if(KviQString::equalCI(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender), "Male"))
		m_pGenderCombo->setCurrentIndex(2);
	else if(KviQString::equalCI(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender), "Female"))
		m_pGenderCombo->setCurrentIndex(1);
	else
		m_pGenderCombo->setCurrentIndex(0);

	hb->setStretchFactor(m_pGenderCombo, 1);

	m_pLocationSelector = new KviStringSelector(gbox, __tr2qs("Location:"), &(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoLocation)), true);
	m_pLocationSelector->setMinimumLabelWidth(120);
	m_pLocationSelector->setMargin(0);

	m_pLanguagesSelector = new KviStringSelector(gbox, __tr2qs("Languages:"), &(KVI_OPTION_STRING(KviOption_stringCtcpUserInfoLanguages)), true);
	m_pLanguagesSelector->setMinimumLabelWidth(120);
	m_pLanguagesSelector->setMargin(0);

	addPage(m_pIdentity, __tr2qs("Identity"));

	setHelpEnabled(m_pIdentity, false);

	//
	// Default theme
	//

	m_pTheme = new SetupPage(this);
	m_pTheme->m_pTextLabel->setText(__tr2qs(
	    "<p>Here you can choose the default appearance of KVIrc.<br><br>"
	    "The Fancy theme uses icons, a transparent background and a lot of colors. "
	    "The Minimalist theme is designed for low-color displays "
	    "or for \"console\" extremists; it's more or less white text on a black background.<br><br>"
	    "If you had a previous version of KVIrc installed, you can choose to not apply any theme "
	    "in order to preserve your current visual settings.<br><br>"
	    "If you don't know what to choose, just use the default.</p>"));

	m_pThemeButtonGroup = new QGroupBox(m_pTheme->m_pVBox);

	QGridLayout * pThemeGrid = new QGridLayout(m_pThemeButtonGroup);
	m_pThemeButtonGroup->setLayout(pThemeGrid);

	QString szThemeImagePath;
	g_pApp->getGlobalKvircDirectory(szThemeImagePath, KviApplication::Pics, "kvi_setup_theme_hires.png");

	QPixmap * pHiResPixmap = new QPixmap(szThemeImagePath);
	if(pHiResPixmap->isNull())
	{
		delete pHiResPixmap;
		pHiResPixmap = new QPixmap(200, 118);
	}

	QLabel * pPixmapLabelHiRes = new QLabel(m_pThemeButtonGroup);

	pPixmapLabelHiRes->setPixmap(*pHiResPixmap);
	pPixmapLabelHiRes->setFixedSize(pHiResPixmap->size());
	pPixmapLabelHiRes->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	pPixmapLabelHiRes->setMargin(0);
	pThemeGrid->addWidget(pPixmapLabelHiRes, 1, 1);

	m_pThemeHiRes = new QRadioButton(__tr2qs("&Fancy Theme"), m_pThemeButtonGroup);
	pThemeGrid->addWidget(m_pThemeHiRes, 2, 1);

	g_pApp->getGlobalKvircDirectory(szThemeImagePath, KviApplication::Pics, "kvi_setup_theme_lowres.png");
	QPixmap * pLowResPixmap = new QPixmap(szThemeImagePath);
	if(pLowResPixmap->isNull())
	{
		delete pLowResPixmap;
		pLowResPixmap = new QPixmap(250, 200);
	}

	QLabel * pPixmapLabelLowRes = new QLabel(m_pThemeButtonGroup);

	pPixmapLabelLowRes->setPixmap(*pLowResPixmap);
	pPixmapLabelLowRes->setFixedSize(pLowResPixmap->size());
	pPixmapLabelLowRes->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	pPixmapLabelLowRes->setMargin(0);
	pThemeGrid->addWidget(pPixmapLabelLowRes, 1, 2);

	m_pThemeLoRes = new QRadioButton(__tr2qs("&Minimalist Theme"), m_pThemeButtonGroup);
	pThemeGrid->addWidget(m_pThemeLoRes, 2, 2);
	m_pThemeNone = new QRadioButton(__tr2qs("&Don't apply any theme"), m_pThemeButtonGroup);
	pThemeGrid->addWidget(m_pThemeNone, 3, 1);

	m_pThemeHiRes->setChecked(true);

	addPage(m_pTheme, __tr2qs("Default Theme"));

	setHelpEnabled(m_pTheme, false);

	//
	// Desktop integration
	//

	m_pDesktopIntegration = new SetupPage(this);

	szText = __tr2qs(
	    "<p>"
	    "Here you can choose how much KVIrc will integrate with "
	    "your system."
	    "<br><br>"
	    "The default settings are fine for most users so if "
	    "you're in doubt just click \"<b>Finish</b>\" to continue."
	    "</p>");

	m_pDesktopIntegration->m_pTextLabel->setText(szText);

	addPage(m_pDesktopIntegration, __tr2qs("Desktop Integration"));

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	m_pCreateUrlHandlers = new QCheckBox(__tr2qs("Make KVIrc the default IRC client"), m_pDesktopIntegration->m_pVBox);
	m_pCreateUrlHandlers->setChecked(true);
#endif
#ifdef COMPILE_KDE_SUPPORT
	m_pCreateDesktopShortcut = new QCheckBox(__tr2qs("Create desktop shortcut"), m_pDesktopIntegration->m_pVBox);
	m_pCreateDesktopShortcut->setChecked(true);
#endif

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	m_pUseMircServerList = new QCheckBox(__tr2qs("Import server list from mIRC"), m_pDesktopIntegration->m_pVBox);
	m_pUseMircServerList->setEnabled(false);
#endif

	setHelpEnabled(m_pDesktopIntegration, false);
	setFinishEnabled(m_pDesktopIntegration, true);

// Preconfigured values
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	QString szTmp;
	g_pApp->getGlobalKvircDirectory(szTmp, KviApplication::Config, "preinstalled.kvc");
	if(KviFileUtils::fileExists(szTmp))
	{
		KviConfigurationFile cfg(szTmp, KviConfigurationFile::Read);
		cfg.setGroup("Setup");
		if(cfg.readBoolEntry("hideServerList", false))
		{
			//setPageEnabled(m_pServers,false);
			//setFinishEnabled(m_pIdentity,true);
			KVI_OPTION_BOOL(KviOption_boolShowChannelsJoinOnIrc) = false;
			KVI_OPTION_BOOL(KviOption_boolShowServersConnectDialogOnStart) = false;
		}
		int iDir;
		iDir = cfg.readIntEntry("settingsDir", -1);
		if(iDir > 0)
		{
			switch(iDir)
			{
				case 1:
					m_pDirUseNew->toggle();
					setPageEnabled(m_pDirectory, false);
					break;
				case 2:
					m_pDirMakePortable->toggle();
					setPageEnabled(m_pDirectory, false);
					break;
			}
		}
	}

//mIRC import
#define QUERY_BUFFER 2048
	char * buffer;
	DWORD len = QUERY_BUFFER;
	buffer = (char *)malloc(len * sizeof(char));
	HKEY hKey;
	QString szMircDir;

	if(RegOpenKeyEx(HKEY_CLASSES_ROOT, TEXT("ChatFile\\DefaultIcon"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(hKey, 0, 0, 0, (LPBYTE)buffer, &len) == ERROR_SUCCESS)
		{
			szMircDir = QString::fromLocal8Bit(buffer, len);

			szMircDir.remove('"');
			QString szMircFile = KviFileUtils::extractFileName(szMircDir);
			szMircFile = szMircFile.left(szMircFile.length() - 4); //cut off ".exe"
			szMircDir = KviFileUtils::extractFilePath(szMircDir);

			szMircIni = szMircDir + "/" + szMircFile + ".ini";

			if(!KviFileUtils::fileExists(szMircIni))
				szMircIni = szMircDir + "/mirc.ini";

			if(!KviFileUtils::fileExists(szMircIni))
				szMircIni = szMircDir + "/pirc.ini";

			if(KviFileUtils::fileExists(szMircIni))
			{
				KviConfigurationFile cfg(szMircIni, KviConfigurationFile::Read, true);
				if(cfg.hasGroup("mirc"))
				{
					g_bFoundMirc = true;
					cfg.setGroup("mirc");
					m_pNickSelector->setText(cfg.readEntry("nick", KVI_OPTION_STRING(KviOption_stringNickname1)));
					m_pRealNameSelector->setText(cfg.readEntry("user", KVI_OPTION_STRING(KviOption_stringRealname)));
					KVI_OPTION_STRING(KviOption_stringNickname2) = cfg.readEntry("anick", KVI_OPTION_STRING(KviOption_stringNickname2));
					KVI_OPTION_STRING(KviOption_stringUsername) = cfg.readEntry("email", KVI_OPTION_STRING(KviOption_stringUsername)).section('@', 0, 0);

					if(cfg.hasGroup("files"))
					{
						m_szMircServerIniFile = cfg.readEntry("servers", "servers.ini");
						m_szMircServerIniFile.prepend('/');
						m_szMircServerIniFile.prepend(szMircDir);
						if(KviFileUtils::fileExists(m_szMircServerIniFile))
						{
							m_pUseMircServerList->setEnabled(true);
							m_pUseMircServerList->setChecked(true);
						}
					}
				}
			}
		}
	}
	free(buffer);

#endif // COMPILE_ON_WINDOWS

	newDirClicked();

	//setMinimumSize(630,450);

	enableOrDisableIdentityPageNextButton();
}

SetupWizard::~SetupWizard()
{
	delete m_pLabelPixmap;
}

void SetupWizard::showEvent(QShowEvent * e)
{
	int primary_screen = g_pApp->desktop()->primaryScreen();
	QRect r = g_pApp->desktop()->screenGeometry(primary_screen);

	int w = r.width();
	int h = r.height();

	int ww = width();
	int wh = height();

	if(w < 800)
	{
		// 640x480
		if(ww < 630)
			ww = 630;
	}
	else
	{
		if(ww < 770)
			ww = 770;
	}

	setGeometry(r.left() + (w - ww) / 2, r.top() + (h - wh) / 2, ww, wh);

	KviTalWizard::showEvent(e);
}

void SetupWizard::enableOrDisableIdentityPageNextButton()
{
	setNextEnabled(m_pIdentity, !m_pNickSelector->currentText().trimmed().isEmpty());
}

void SetupWizard::nickSelectorTextChanged(const QString & str)
{
	enableOrDisableIdentityPageNextButton();
}

void SetupWizard::oldDirClicked()
{
	m_pOldPathBox->setEnabled(true);
	m_pNewPathBox->setEnabled(false);
	m_pNewIncomingBox->setEnabled(false);

	if(m_pIdentity)
		setPageEnabled(m_pIdentity, false);
	if(m_pTheme)
		setPageEnabled(m_pTheme, false);
	if(m_pServers)
		setPageEnabled(m_pServers, false);

	if(m_pOldDataPathEdit->text().isEmpty())
		setNextEnabled(m_pDirectory, false);
	else
		setNextEnabled(m_pDirectory, true);
}

void SetupWizard::oldDataTextChanged(const QString & str)
{
	setNextEnabled(m_pDirectory, !str.isEmpty());
}

void SetupWizard::newDataTextChanged(const QString & str)
{
	setNextEnabled(m_pDirectory, !str.isEmpty() && !m_pIncomingPathEdit->text().isEmpty());
}

void SetupWizard::newIncomingTextChanged(const QString & str)
{
	setNextEnabled(m_pDirectory, !str.isEmpty() && !m_pDataPathEdit->text().isEmpty());
}

void SetupWizard::newDirClicked()
{
	m_pOldPathBox->setEnabled(false);
	m_pNewPathBox->setEnabled(true);
	m_pNewIncomingBox->setEnabled(true);

	if(m_pIdentity)
		setPageEnabled(m_pIdentity, true);
	if(m_pTheme)
		setPageEnabled(m_pTheme, true);
	if(m_pServers)
		setPageEnabled(m_pServers, true);

	if(m_pDataPathEdit->text().isEmpty() || m_pIncomingPathEdit->text().isEmpty())
		setNextEnabled(m_pDirectory, false);
	else
		setNextEnabled(m_pDirectory, true);
}

void SetupWizard::portableClicked()
{
	m_pOldPathBox->setEnabled(false);
	m_pNewPathBox->setEnabled(false);
	m_pNewIncomingBox->setEnabled(false);

	if(m_pIdentity)
		setPageEnabled(m_pIdentity, true);
	if(m_pTheme)
		setPageEnabled(m_pTheme, true);
	if(m_pServers)
		setPageEnabled(m_pServers, true);

	setNextEnabled(m_pDirectory, true);
}

void SetupWizard::chooseOldDataPath()
{
	// FIXME: We'd like to show hidden directories here ($HOME/.kvirc is hidden)...
	QString szBuffer = KviTalFileDialog::getExistingDirectoryPath(
	    m_pDataPathEdit->text(),
	    __tr2qs("Choose an Existing Configuration Folder - KVIrc Setup"),
	    this);

	if(szBuffer.isEmpty())
		return;

	KviFileUtils::adjustFilePath(szBuffer);

	KviQString::ensureLastCharIs(szBuffer, KVI_PATH_SEPARATOR_CHAR);
	if(!g_pApp->checkLocalKvircDirectory(szBuffer))
	{
		if(
		    QMessageBox::question(
		        this,
		        __tr2qs("Confirm Setting Configuration Folder - KVIrc Setup"),
		        __tr2qs("The folder %1 doesn't seem to be a valid KVIrc settings folder. Do you want to use it anyway?")
		            .arg(szBuffer),
		        __tr2qs("&Yes"),
		        __tr2qs("&No"),
		        QString(),
		        0,
		        1)
		    == 0)
			m_pOldDataPathEdit->setText(szBuffer);
	}
	else
	{
		m_pOldDataPathEdit->setText(szBuffer);
	}
}

void SetupWizard::chooseDataPath()
{
	QString szBuffer = KviTalFileDialog::getExistingDirectoryPath(
	    m_pDataPathEdit->text(),
	    __tr2qs("Choose a Configuration Folder - KVIrc Setup"),
	    this);
	KviFileUtils::adjustFilePath(szBuffer);
	if(!szBuffer.isEmpty())
	{
		KviQString::ensureLastCharIs(szBuffer, KVI_PATH_SEPARATOR_CHAR);
		m_pDataPathEdit->setText(szBuffer);
	}
}

void SetupWizard::chooseIncomingPath()
{
	QString szBuffer = KviTalFileDialog::getExistingDirectoryPath(
	    m_pIncomingPathEdit->text(),
	    __tr2qs("Choose a Download Folder - KVIrc Setup"),
	    this);
	KviFileUtils::adjustFilePath(szBuffer);
	if(!szBuffer.isEmpty())
	{
		m_pIncomingPathEdit->setText(szBuffer);
	}
}

void SetupWizard::makeLink()
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	// Let's make a link on the desktop :)
	// You need this horrible snippet of code to create a shortcut!!!!
	//
	// you have to:
	// - dig in the registry,
	// - trigger the entire COM subsystem
	// - bring up a couple of OLE interfaces....
	// - use some obscure functions like MultiByteToWideChar
	// - and the finally drop the entire OLE+COM interface in the garbadge
	// ...
	// Isn't this horrible ? (compared to symlink()!)
	//

	// Well..let's go (please note that we don't handle most possible errors!
	// otherwise there would be 150 lines for a stupid symlink!)

	HKEY hCU;
	DWORD lpType;
	ULONG ulSize = MAX_PATH;
	char szLink[MAX_PATH];

	// Dig in the registry looking up the Desktop path
	if(RegOpenKeyEx(HKEY_CURRENT_USER,
	       TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"),
	       0, KEY_QUERY_VALUE, &hCU)
	    == ERROR_SUCCESS)
	{
		RegQueryValueEx(hCU, TEXT("Desktop"), nullptr, &lpType,
		    (unsigned char *)&szLink, &ulSize);
		RegCloseKey(hCU);
	}

	// Build our paths
	QString szLinkTarget = szLink;
	szLinkTarget.append("\\kvirc.lnk");

	QString szKvircExec = g_pApp->m_szGlobalKvircDir;
	szKvircExec.append("\\kvirc.exe");

	// Trigger a horrible machinery
	CoInitialize(nullptr); // we need COM+OLE

	// Fiddle with an obscure shell interface
	IShellLink * psl;

	// Get a pointer to the IShellLink interface: this is kinda ugly :)
	if(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
	       IID_IShellLink, (void **)&psl)
	    == S_OK)
	{
		// just for fun, lookup another shell interface

		IPersistFile * ppf;

		// Query IShellLink for the IPersistFile interface for
		// saving the shell link in persistent storage.
		if(psl->QueryInterface(IID_IPersistFile, (void **)&ppf) == S_OK)
		{
			WORD wsz[MAX_PATH];
			// Set the path to the shell link target.
			psl->SetPath(szKvircExec.toStdWString().c_str());
			// Set the description of the shell link.
			psl->SetDescription(TEXT("kvirc"));
			// Ensure string is ANSI.
			MultiByteToWideChar(CP_ACP, 0, QTextCodec::codecForLocale()->fromUnicode(szLinkTarget).data(), -1, (LPWSTR)wsz, MAX_PATH);
			// Save the link via the IPersistFile::Save method.
			ppf->Save((LPCOLESTR)wsz, true);
			ppf->Release();
		}
		psl->Release();
	}
	// And throw OLE & Co. in the garbadge
	CoUninitialize();
#endif //COMPILE_ON_WINDOWS

#ifdef COMPILE_KDE_SUPPORT
	QString tmp = QDir::homePath();
	KviQString::ensureLastCharIs(tmp, KVI_PATH_SEPARATOR_CHAR);
	tmp.append("Desktop");
	KviQString::ensureLastCharIs(tmp, KVI_PATH_SEPARATOR_CHAR);
	tmp.append("kvirc.desktop");

	QString contents = "[Desktop Entry]\n"
	                   "GenericName=IRC Client\n"
	                   "Comment=Connect to Internet Relay Chat\n"
	                   "Exec=kvirc -m %u\n"
	                   "Icon=kvirc\n"
	                   "MapNotify=true\n"
	                   "Name=KVIrc\n"
	                   "Terminal=false\n"
	                   "Type=Application\n"
	                   "X-KDE-SubstituteUID=false\n";

	KviFileUtils::writeFile(tmp, contents, false);
#endif //COMPILE_KDE_SUPPORT
}

void SetupWizard::setUrlHandlers()
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	QString szReg = "REGEDIT4\r\n"
	                "\r\n"
	                "[HKEY_CLASSES_ROOT\\irc]\r\n"
	                "@=\"URL:IRC Protocol\"\r\n"
	                "\"IRC Protocol\"=\"http://www.kvirc.net/\"\r\n"
	                "\"EditFlags\"=hex:02,00,00,00\r\n"
	                "\"URL Protocol\"="
	                "\r\n"
	                "\r\n"
	                "[HKEY_CLASSES_ROOT\\irc\\DefaultIcon]\r\n"
	                "@=\"\\\"@KVIRCEXECUTABLE@\\\"\"\r\n"
	                "\r\n"
	                "[HKEY_CLASSES_ROOT\\irc\\shell]\r\n"
	                "@=\"\"\r\n"
	                "\r\n"
	                "[HKEY_CLASSES_ROOT\\irc\\shell\\open]\r\n"
	                "\"EditFlags\"=hex:01,00,00,00\r\n"
	                "\r\n"
	                "[HKEY_CLASSES_ROOT\\irc\\shell\\open\\command]\r\n"
	                "@=\"\\\"@KVIRCEXECUTABLE@\\\" %1\"\r\n"
	                "\r\n"
	                "[HKEY_CLASSES_ROOT\\irc6]\r\n"
	                "@=\"URL:IRC6 Protocol\"\r\n"
	                "\"IRC6 Protocol\"=\"http://www.kvirc.net/\"\r\n"
	                "\"EditFlags\"=hex:02,00,00,00\r\n"
	                "\"URL Protocol\"="
	                "\r\n"
	                "\r\n"
	                "[HKEY_CLASSES_ROOT\\irc6\\DefaultIcon]\r\n"
	                "@=\"\\\"@KVIRCEXECUTABLE@\\\"\"\r\n"
	                "\r\n"
	                "[HKEY_CLASSES_ROOT\\irc6\\shell]\r\n"
	                "@=\"\"\r\n"
	                "\r\n"
	                "[HKEY_CLASSES_ROOT\\irc6\\shell\\open]\r\n"
	                "\"EditFlags\"=hex:01,00,00,00\r\n"
	                "\r\n"
	                "[HKEY_CLASSES_ROOT\\irc6\\shell\\open\\command]\r\n"
	                "@=\"\\\"@KVIRCEXECUTABLE@\\\" %1\"\r\n";

	QString szKvircExec = g_pApp->m_szGlobalKvircDir;
	szKvircExec.append("\\kvirc.exe");
	szKvircExec.replace("\\", "\\\\");

	szReg.replace("@KVIRCEXECUTABLE@", QTextCodec::codecForLocale()->fromUnicode(szKvircExec));

	QString szRegFile = g_pApp->m_szGlobalKvircDir;
	szRegFile.append("\\kvirc.reg");

	KviFileUtils::writeFile(szRegFile, szReg, false);

	QString szCmd = "regedit /S \"";
	szCmd += szRegFile;
	szCmd += "\"";

	WinExec(QTextCodec::codecForLocale()->fromUnicode(szCmd).data(), SW_SHOW);
#endif
}

void SetupWizard::reject()
{
	if(QMessageBox::warning(this, __tr2qs("Confirm Setup Abort - KVIrc Setup"),
	       __tr2qs("You have chosen to abort the setup.<br>KVIrc can't run until you complete this procedure.<br><br>Do you really wish to abort?"),
	       QMessageBox::Yes, QMessageBox::No | QMessageBox::Default | QMessageBox::Escape)
	    != QMessageBox::Yes)
		return;

	KviTalWizard::reject();
}

void SetupWizard::accept()
{
	QString szDir;

	if(m_pDirUsePrev->isChecked())
	{
		bNeedToApplyDefaults = false;
		g_pApp->m_szLocalKvircDir = m_pOldDataPathEdit->text();
	}
	else
	{
		bNeedToApplyDefaults = true;
		if(m_pDirUseNew->isChecked())
		{
			szDir = m_pDataPathEdit->text();
		}
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		else
		{ //portable
			szDir = g_pApp->applicationDirPath() + KVI_PATH_SEPARATOR_CHAR + "Settings";
		}
#endif

		KviFileUtils::adjustFilePath(szDir);
		KviQString::ensureLastCharIs(szDir, KVI_PATH_SEPARATOR_CHAR);
		if(!KviFileUtils::directoryExists(szDir))
		{
			if(!KviFileUtils::makeDir(szDir))
			{
				KviMessageBox::warning(__tr("Can't create directory %s.\n"
				                            "You may not have write permission "
				                            "for that path. Please go back and choose another directory."));
				setCurrentPage(m_pDirectory);
				return;
			}
		}

		g_pApp->m_szLocalKvircDir = szDir;
		KviFileUtils::adjustFilePath(g_pApp->m_szLocalKvircDir);

		if(m_pDirUseNew->isChecked())
		{
			szDir = m_pIncomingPathEdit->text();
		}
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		else
		{ //portable
			szDir = g_pApp->applicationDirPath() + KVI_PATH_SEPARATOR_CHAR + "Downloads";
		}
#endif

		KviFileUtils::adjustFilePath(szDir);
		if(!KviFileUtils::directoryExists(szDir))
		{
			if(!KviFileUtils::makeDir(szDir))
			{
				KviMessageBox::warning(__tr("Can't create directory %s.\n"
				                            "You may not have write permission for that path.\n"
				                            "Please go back and choose another directory."));
				setCurrentPage(m_pDirectory);
				return;
			}
		}

		g_szChoosenIncomingDirectory = szDir;

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
		// Make local->global link
		QString localPath = QString("%1/global").arg(g_pApp->m_szLocalKvircDir);
		unlink(QTextCodec::codecForLocale()->fromUnicode(localPath).data());
		(void)symlink(QTextCodec::codecForLocale()->fromUnicode(g_pApp->m_szGlobalKvircDir).data(), QTextCodec::codecForLocale()->fromUnicode(localPath).data());
#endif

#ifdef COMPILE_KDE_SUPPORT
		if(m_pCreateDesktopShortcut->isChecked())
			makeLink();
#endif

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		if(m_pCreateUrlHandlers->isChecked())
			setUrlHandlers();
#endif

		if(m_pTheme)
		{
			if(m_pThemeHiRes->isChecked())
			{
				g_iThemeToApply = THEME_APPLY_HIRES;
			}
			else if(m_pThemeLoRes->isChecked())
			{
				g_iThemeToApply = THEME_APPLY_LORES;
			}
			else
			{
				g_iThemeToApply = THEME_APPLY_NONE;
			}
		}

		if(m_pIdentity)
		{
			m_pNickSelector->commit();
			m_pRealNameSelector->commit();
			m_pLocationSelector->commit();
			m_pLanguagesSelector->commit();

			KVI_OPTION_STRING(KviOption_stringNickname1) = KVI_OPTION_STRING(KviOption_stringNickname1).trimmed();

			if(KVI_OPTION_STRING(KviOption_stringNickname1).length() > 32)
				KVI_OPTION_STRING(KviOption_stringNickname1)
				    .truncate(32);

			if(KVI_OPTION_STRING(KviOption_stringNickname1).isEmpty())
				KVI_OPTION_STRING(KviOption_stringNickname1) = KVI_DEFAULT_NICKNAME1;

			QString szNickPart;
			if(KVI_OPTION_STRING(KviOption_stringNickname1).length() < 31)
			{
				szNickPart = KVI_OPTION_STRING(KviOption_stringNickname1);
			}
			else
			{
				szNickPart = KVI_OPTION_STRING(KviOption_stringNickname1).left(30);
			}

			if(!g_bFoundMirc)
				KVI_OPTION_STRING(KviOption_stringNickname2) = QString(KVI_DEFAULT_NICKNAME2).replace(KVI_DEFAULT_NICKNAME_TOKEN, szNickPart);

			KVI_OPTION_STRING(KviOption_stringNickname3) = QString(KVI_DEFAULT_NICKNAME3).replace(KVI_DEFAULT_NICKNAME_TOKEN, szNickPart);
			KVI_OPTION_STRING(KviOption_stringNickname4) = QString(KVI_DEFAULT_NICKNAME4).replace(KVI_DEFAULT_NICKNAME_TOKEN, szNickPart);

			int i = m_pAgeCombo->currentIndex();
			if(i < 0)
				i = 0;
			if(i > 120)
				i = 120;
			if(i <= 0)
				KVI_OPTION_STRING(KviOption_stringCtcpUserInfoAge) = "";
			else
				KVI_OPTION_STRING(KviOption_stringCtcpUserInfoAge)
				    .setNum(i);

			switch(m_pGenderCombo->currentIndex())
			{
				case 1:
					// this should be in English
					KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender) = "Female";
					break;
				case 2:
					// this should be in English
					KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender) = "Male";
					break;
				default:
					KVI_OPTION_STRING(KviOption_stringCtcpUserInfoGender) = "";
					break;
			}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			if(m_pUseMircServerList->isEnabled() && m_pUseMircServerList->isChecked())
				szMircServers = m_szMircServerIniFile;
#endif
		}
	}
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	if(m_pDirMakePortable->isChecked())
	{
		KviFileUtils::writeFile(g_pApp->applicationDirPath() + KVI_PATH_SEPARATOR_CHAR + "portable", QString("true"));
		g_pApp->m_bPortable = true;
	}
	else
	{
#endif
		g_pApp->saveKvircDirectory();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	}
#endif
	KviTalWizard::accept();
}
