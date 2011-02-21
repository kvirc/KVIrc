//=============================================================================
//
//   File : KviWindow.cpp
//   Creation date : Tue Jul 6 1999 14:52:11 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviWindow.cpp
* \brief Contains the KviWindow class
*/

#define KVI_WINDOW_MIN_WIDTH 100
#define KVI_WINDOW_MIN_HEIGHT 100

#define _KVI_WINDOW_CPP_
#define _KVI_DEBUG_CHECK_RANGE_

#include "kvi_debug.h"
#include "KviApplication.h"
#include "KviWindow.h"
#include "KviMainWindow.h"
#include "KviWindowListBase.h"
#include "KviMdiChild.h"
#include "KviLocale.h"
#include "KviIrcView.h"
#include "kvi_out.h"
#include "KviMemory.h"
#include "KviInput.h"
#include "KviFileUtils.h"
#include "KviOptions.h"
#include "KviConfigurationFile.h"
#include "KviIrcContext.h"
#include "KviConsoleWindow.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviControlCodes.h"
#include "KviWindowToolWidget.h"
#include "KviKvsScript.h"
#include "KviTalPopupMenu.h"
#include "KviTalToolTip.h"
#include "KviKvsScript.h"
#include "KviKvsEventTriggers.h"

#include <QPixmap>
#include <QCursor>
#include <QTimer>
#include <QSplitter>
#include <QMetaObject>
#include <QDateTime>
#include <QTextCodec>
#include <QPushButton>
#include <QDesktopWidget>
#include <QVariant>
#include <QMessageBox>
#include <QEvent>
#include <QCloseEvent>
#include <QIcon>
#include <QActionGroup>

#ifdef COMPILE_CRYPT_SUPPORT
	#include "KviCryptEngine.h"
	#include "KviCryptController.h"
#endif

#ifdef COMPILE_KDE_SUPPORT
	#include <KWindowSystem>
#endif

#ifdef COMPILE_ON_WINDOWS
	#include <windows.h>
#endif

KVIRC_API KviWindow * g_pActiveWindow = 0;

static KviTalPopupMenu * g_pMdiWindowSystemMainPopup = 0;
static KviTalPopupMenu * g_pMdiWindowSystemTextEncodingPopup = 0;
static KviTalPopupMenu * g_pMdiWindowSystemTextEncodingPopupStandard = 0;
static KviTalPopupMenu * g_pMdiWindowSystemTextEncodingPopupSmart = 0;
static KviTalPopupMenu * g_pMdiWindowSystemTextEncodingPopupSmartUtf8 = 0;
static QActionGroup    * g_pMdiWindowSystemTextEncodingActionGroup = 0;
static QAction         * g_pMdiWindowSystemTextEncodingCurrentAction = 0;
static QAction         * g_pMdiWindowSystemTextEncodingDefaultAction = 0;

unsigned long int g_uUniqueWindowId = 1;

KviWindow::KviWindow(Type eType, KviMainWindow * lpFrm, const QString & szName, KviConsoleWindow * lpConsole)
: QWidget(0)
{
	m_uId = g_uUniqueWindowId;
	g_uUniqueWindowId++;

	m_szName = szName;
	setObjectName(szName);

	g_pApp->registerWindow(this);

	m_eType                 = eType;
	m_pFocusHandler         = 0;

	m_pFrm = lpFrm; // FIXME: Should disappear!
	m_pIrcView              = 0;
	m_pInput                = 0;
	m_pSplitter             = 0;
	m_pButtonBox            = 0;
	m_pConsole              = lpConsole;
	m_pLastFocusedChild     = 0;
	m_pTextCodec            = 0; // will be set by loadProperties
	m_pTextEncodingButton   = 0;
	m_pHideToolsButton      = 0;
//	m_pEditorsContainer     = 0;
	m_bIsDocked             = false;

#ifdef COMPILE_CRYPT_SUPPORT
	m_pCryptControllerButton = 0;
	m_pCryptController = 0;
	m_pCryptSessionInfo = 0;
#endif

	m_pWindowListItem = 0;

	setMinimumSize(KVI_WINDOW_MIN_WIDTH,KVI_WINDOW_MIN_HEIGHT);
	//setAutoFillBackground(false);
	setFocusPolicy(Qt::StrongFocus);
	connect(g_pApp,SIGNAL(reloadImages()),this,SLOT(reloadImages()));
}

KviWindow::~KviWindow()
{
	destroyWindowListItem();
	g_pApp->unregisterWindow(this);
	if(g_pApp->windowCount() == 0)
	{
		// this is the last window!
		if(g_pMdiWindowSystemMainPopup)
			delete g_pMdiWindowSystemMainPopup;
		if(g_pMdiWindowSystemTextEncodingPopup)
			delete g_pMdiWindowSystemTextEncodingPopup;
		if(g_pMdiWindowSystemTextEncodingPopupStandard)
			delete g_pMdiWindowSystemTextEncodingPopupStandard;
		if(g_pMdiWindowSystemTextEncodingPopupSmart)
			delete g_pMdiWindowSystemTextEncodingPopupSmart;
		if(g_pMdiWindowSystemTextEncodingPopupSmartUtf8)
			delete g_pMdiWindowSystemTextEncodingPopupSmartUtf8;
	}
#ifdef COMPILE_CRYPT_SUPPORT
	if(m_pCryptSessionInfo)
		KviCryptController::destroyCryptSessionInfo(&m_pCryptSessionInfo);
#endif
}

void KviWindow::setWindowName(const QString & szName)
{
	m_szName = szName;
	setObjectName(szName);
	emit windowNameChanged();
}

void KviWindow::toggleButtonContainer()
{
	QFrame * pContainer = buttonContainer();
	if(pContainer)
		pContainer->setHidden(!pContainer->isHidden());
}

void KviWindow::reloadImages()
{
	updateIcon();
}

bool KviWindow::hasAttention()
{
	if(((QApplication *)g_pApp)->activeWindow() == 0)
		return false; // no application window has the focus atm

	if(mdiParent())
	{
		// This frame is not the active window but the
		// active window still belongs to KVIrc.
		// When the active window is derived from QDialog
		// then it is probably a KVIrc's option dialog
		// or something similar.
		// In this case we assume that the user has the
		// KVIrc window just below and can see it.
		if(frame()->isActiveWindow())
			return true;

		// Handle the special case of the dialogs then
		QWidget * pWidget = ((QApplication *)g_pApp)->activeWindow();
		if(pWidget->inherits("QDialog"))
		{
			// but make sure that the frame is visible at all!
			if(!frame()->isVisible())
				return false;
			return true;
		}
		// any other class is so unfrequent that we ignore it
	} else {
		// when the window is undocked, instead
		// it is likely to be covered by KVIrc or other windows...
		if(isActiveWindow())
			return true;
	}
	return false;
}

void KviWindow::demandAttention()
{
	if(mdiParent())
	{
		if(frame()->isActiveWindow())
			return;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		FLASHWINFO fwi;
		fwi.cbSize = sizeof(fwi);
		fwi.hwnd = (HWND)(frame()->winId());
		fwi.dwFlags = FLASHW_TRAY | FLASHW_TIMERNOFG;
		fwi.uCount = 20;
		fwi.dwTimeout = 500;
		FlashWindowEx(&fwi);
#else
	#ifdef COMPILE_KDE_SUPPORT
		KWindowSystem::demandAttention(frame()->winId(),true);
	#endif
#endif
	} else {
		if(isActiveWindow())
			return;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		FLASHWINFO fwi;
		fwi.cbSize = sizeof(fwi);
		fwi.hwnd = (HWND)winId();
		fwi.dwFlags = FLASHW_TRAY | FLASHW_TIMERNOFG;
		fwi.uCount = 20;
		fwi.dwTimeout = 500;
		FlashWindowEx(&fwi);
#else
	#ifdef COMPILE_KDE_SUPPORT
		KWindowSystem::demandAttention(winId(),true);
	#endif
#endif
	}
}

bool KviWindow::focusNextPrevChild(bool bNext)
{
	QWidget * pWidget = focusWidget();
	if(pWidget)
	{
		if(pWidget->focusPolicy() == Qt::StrongFocus)
			return false;
		if(pWidget->parent())
		{
			if(pWidget->parent()->metaObject()->indexOfProperty("KviProperty_ChildFocusOwner") == -1)
				return false; // Do NOT change the focus widget!
		}
	}

	return QWidget::focusNextPrevChild(bNext);
}

void KviWindow::forceTextCodec(QTextCodec * pCodec)
{
	if(!pCodec)
		return;
	m_pTextCodec = pCodec;
	QTextCodec * pDefault = defaultTextCodec();
	if(pDefault != pCodec)
		m_szTextEncoding = pCodec->name();
	else
		m_szTextEncoding = ""; // this is the default anyway
}

bool KviWindow::setTextEncoding(const QString & szTextEncoding)
{
	if(!szTextEncoding.isEmpty())
	{
		m_pTextCodec = KviLocale::codecForName(szTextEncoding.toLatin1());
		if(m_pTextCodec)
		{
			m_szTextEncoding = szTextEncoding;
			return true;
		}
		// this is an error because we specified an encoding
		// and we couldn't find a codec for this
	} // else it is empty : this means : guess from locale
	// either empty or not found...
	m_pTextCodec = 0;
	m_szTextEncoding = ""; // empty: we're using the default
	return false;
}

QTextEncoder * KviWindow::makeEncoder()
{
	if(!m_pTextCodec)
		return defaultTextCodec()->makeEncoder();
	return m_pTextCodec->makeEncoder();
}

bool KviWindow::activityMeter(unsigned int *, unsigned int *)
{
	return false;
}

bool KviWindow::highlightMeter(unsigned int * puValue)
{
	if(!m_pWindowListItem)
	{
		*puValue = 0;
		return false;
	}
	*puValue = m_pWindowListItem->highlightLevel();
	return true;
}

bool KviWindow::highlightMe(unsigned int uValue)
{
	if(uValue > 5)
		uValue = 5;
	if(m_pWindowListItem)
		m_pWindowListItem->highlight(uValue);
	return true;
}

const char * KviWindow::m_typeTable[TypeCount] =
{
	"console",  // 0
	"channel",  // 1
	"query",  // 2
	"deadchannel",  // 3
	"deadquery",  // 4
	"editor",  // 5
	"help",  // 6
	"terminal",  // 7
	"socketspy",  // 8
	"links",  // 9
	"list",  // 10
	"dccchat",  // 11
	"dcctransfer",  // 12
	"dcccanvas",  // 13
	"dccvoice",  // 14
	"dccvideo",  // 15
	"userwindow",  // 16
	"tool",  // 17
	"iograph",  // 18
	"dirbrowser",  // 19
	"scripteditor",  // 20
	"scriptobject",  // 21
	"logview",  // 22
	"offer",  // 23
	"debug",  // 24
	// <------ NEW TYPES GO HERE!
	"unknown"  // 25
};

const char * KviWindow::typeString()
{
	if(m_eType < TypeCount)
	{
		return m_typeTable[m_eType];
	}
	return m_typeTable[Unknown];
}

void KviWindow::createWindowListItem()
{
	if(m_pWindowListItem)
		return;
	m_pWindowListItem = g_pMainWindow->m_pWindowList->addItem(this);
}

void KviWindow::destroyWindowListItem()
{
	if(m_pWindowListItem)
	{
		g_pMainWindow->m_pWindowList->removeItem(m_pWindowListItem);
		m_pWindowListItem = 0;
	}
}

QToolButton * KviWindow::createToolButton(QWidget * pPar, const char * pcName, KviIconManager::SmallIcon eIcon, const QString & szToolTip, bool bOn)
{
	QToolButton * pTool = new QToolButton(pPar);
	pTool->setObjectName(pcName);
	pTool->setIcon(QIcon(*(g_pIconManager->getSmallIcon(eIcon))));
	pTool->setAutoRaise(true);
	pTool->setChecked(bOn);
	KviTalToolTip::add(pTool,szToolTip);
	return pTool;
}

// This is always defined...
void KviWindow::createCryptControllerButton(QWidget *)
{
#ifdef COMPILE_CRYPT_SUPPORT
	m_pCryptControllerButton = new KviWindowToolPageButton(KviIconManager::UnLockedOff,KviIconManager::UnLocked,__tr2qs("Crypting"),buttonContainer(),false);
	m_pCryptControllerButton->setObjectName("crypt_controller_button");
	connect(m_pCryptControllerButton,SIGNAL(clicked()),this,SLOT(toggleCryptController()));
#endif // COMPILE_CRYPT_SUPPORT
}

void KviWindow::createTextEncodingButton(QWidget * pPar)
{
	if(m_pTextEncodingButton)
		delete m_pTextEncodingButton;
	m_pTextEncodingButton = createToolButton(pPar,"text_encoding_button",KviIconManager::TextEncoding,__tr2qs("Text Encoding"),false);
	connect(m_pTextEncodingButton,SIGNAL(clicked()),this,SLOT(textEncodingButtonClicked()));
}

void KviWindow::textEncodingButtonClicked()
{
	createSystemTextEncodingPopup();
	g_pMdiWindowSystemTextEncodingPopup->popup(m_pTextEncodingButton->mapToGlobal(QPoint(0,m_pTextEncodingButton->height())));
	m_pTextEncodingButton->setChecked(false);
}

const QString & KviWindow::lastLineOfText()
{
	if(m_pIrcView)
		return m_pIrcView->lastLineOfText();
	return KviQString::Empty;
}

const QString & KviWindow::lastMessageText()
{
	if(m_pIrcView)
		return m_pIrcView->lastMessageText();
	return KviQString::Empty;
}

// The following three have to be here even if the crypt support is disabled...moc does not support conditional compilations
void KviWindow::toggleCryptController()
{
#ifdef COMPILE_CRYPT_SUPPORT
	if(!m_pCryptControllerButton->isChecked())
	{
		if(m_pCryptController)
		{
			delete m_pCryptController;
			m_pCryptController = 0;
			if(!m_pCryptControllerButton)
				return;
			if(m_pCryptControllerButton->isChecked())
				m_pCryptControllerButton->setChecked(false);
		}
	} else {
		if(m_pSplitter && m_pInput)
		{
			m_pCryptController = new KviCryptController(m_pSplitter,m_pCryptControllerButton,this,m_pCryptSessionInfo);
			connect(m_pCryptController,SIGNAL(done()),this,SLOT(cryptControllerFinished()));
			//setFocusHandlerNoClass(m_pInput,m_pCryptController,"QLineEdit"); //link it!
			m_pCryptController->show();
			if(!m_pCryptControllerButton)
				return;
			if(!(m_pCryptControllerButton->isChecked()))
				m_pCryptControllerButton->setChecked(true);
		}
	}
#endif // COMPILE_CRYPT_SUPPORT
}

#ifdef COMPILE_CRYPT_SUPPORT
void KviWindow::setCryptSessionInfo(KviCryptSessionInfo * pInfo)
{
	if(m_pCryptSessionInfo)
		KviCryptController::destroyCryptSessionInfo(&m_pCryptSessionInfo);
	m_pCryptSessionInfo = pInfo;
	if(m_pCryptSessionInfo)
	{
		connect(m_pCryptSessionInfo->m_pEngine,SIGNAL(destroyed()),this,SLOT(cryptSessionInfoDestroyed()));
	}
	if(m_pCryptControllerButton)
	{
		QIcon is;
		is.addPixmap(*(g_pIconManager->getSmallIcon(m_pCryptSessionInfo ? KviIconManager::LockedOff : KviIconManager::UnLockedOff)),QIcon::Normal,QIcon::Off);
		is.addPixmap(*(g_pIconManager->getSmallIcon(m_pCryptSessionInfo ? KviIconManager::Locked : KviIconManager::UnLocked)),QIcon::Normal,QIcon::On);
		m_pCryptControllerButton->setIcon(is);

		if(m_pCryptControllerButton->isChecked())
			m_pCryptControllerButton->setChecked(false);
	}
}
#endif // COMPILE_CRYPT_SUPPORT

void KviWindow::cryptControllerFinished()
{
#ifdef COMPILE_CRYPT_SUPPORT
	KviCryptSessionInfo * pInfo = m_pCryptController->getNewSessionInfo();
	setCryptSessionInfo(pInfo);
	delete m_pCryptController;
	m_pCryptController = 0;
#endif
}

void KviWindow::cryptSessionInfoDestroyed()
{
#ifdef COMPILE_CRYPT_SUPPORT
	output(KVI_OUT_SYSTEMERROR,__tr2qs("Ops...I've accidentally lost the crypting engine..."));
	m_pCryptSessionInfo->m_pEngine = 0;
	delete m_pCryptSessionInfo;
	m_pCryptSessionInfo = 0;
#endif
}

void KviWindow::setProgress(int iProgress)
{
	m_pWindowListItem->setProgress(iProgress);
}

void KviWindow::listWindowTypes()
{
	outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("List of window types available in this release of KVIrc:"));
	for(int i=0;i< KviWindow::TypeCount;i++)
		outputNoFmt(KVI_OUT_SYSTEMMESSAGE,m_typeTable[i]);
}

void KviWindow::getConfigGroupName(QString & szBuffer)
{
	szBuffer = typeString();
}

void KviWindow::getDefaultLogFileName(QString & szBuffer)
{
	QString szLog;

	// dynamic log path
	QString szDynamicPath=KVI_OPTION_STRING(KviOption_stringLogsDynamicPath).trimmed();
	if(!szDynamicPath.isEmpty())
	{
		KviQString::escapeKvs(&szDynamicPath, KviQString::PermitVariables | KviQString::PermitFunctions );

		KviKvsVariant vRet;
		if(KviKvsScript::evaluate(szDynamicPath,this,0,&vRet))
			vRet.asString(szDynamicPath);
	}

	g_pApp->getLocalKvircDirectory(szLog, KviApplication::Log, szDynamicPath);
	KviQString::ensureLastCharIs(szLog,KVI_PATH_SEPARATOR_CHAR);

	//ensure the directory exists
	KviFileUtils::makeDir(szLog);

	QString szDate;
	QDate date(QDate::currentDate());

	switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
	{
		case 1:
			szDate = date.toString(Qt::ISODate);
			break;
		case 2:
			szDate = date.toString(Qt::SystemLocaleDate);
			break;
		case 0:
		default:
			szDate = date.toString("yyyy.MM.dd");
			break;
	}

	QString szBase;
	getBaseLogFileName(szBase);
	KviFileUtils::encodeFileName(szBase);
	szBase = szBase.toLower();
	szBase.replace("%%2e","%2e");

	QString szTmp;
	if(KVI_OPTION_BOOL(KviOption_boolGzipLogs))
		szTmp = "%1_%2_%3.log.gz";
	else
		szTmp = "%1_%2_%3.log";

	szLog.append(QString(szTmp).arg(typeString(),szBase,szDate));
	
	szBuffer=szLog;
}

void KviWindow::getBaseLogFileName(QString & szBuffer)
{
	szBuffer = m_szName;
}

void KviWindow::saveProperties(KviConfigurationFile * pCfg)
{
	// store only the non-default text encoding.
	QString szCodec = m_szTextEncoding;
	QTextCodec * pCodec = defaultTextCodec();
	if(pCodec && m_pTextCodec)
	{
		if(KviQString::equalCI(szCodec,pCodec->name().data()))
			szCodec = KviQString::Empty; // store "default"
	}

	QString szKey = "TextEncoding_";
	szKey += m_szName;
	pCfg->writeEntry(szKey,szCodec);
	if(m_pInput)
	{
		pCfg->writeEntry("inputToolButtonsHidden",m_pInput->isButtonsHidden());
		pCfg->writeEntry("commandLineIsUserFriendly",m_pInput->isUserFriendly());
	}

	/*
	if(m_pIrcView && m_eType == KviWindow::Channel)
	if(m_pIrcView->isLogging())
		pCfg->writeEntry("LoggingEnabled",m_pIrcView->isLogging());
	*/
}

void KviWindow::loadProperties(KviConfigurationFile * pCfg)
{
	QString szKey = "TextEncoding_";
	szKey += m_szName;
	setTextEncoding(pCfg->readEntry(szKey,KviQString::Empty).toUtf8().data());
	if(m_pInput)
	{
		m_pInput->setButtonsHidden(pCfg->readBoolEntry("inputToolButtonsHidden",KVI_OPTION_BOOL(KviOption_boolHideInputToolButtons)));
		m_pInput->setUserFriendly(pCfg->readBoolEntry("commandLineIsUserFriendly",KVI_OPTION_BOOL(KviOption_boolCommandlineInUserFriendlyModeByDefault)));
	}
	/*
	if(m_pIrcView && m_eType == KviWindow::Channel)
	{
		bool bEnableLogs = pCfg->readBoolEntry("LoggingEnabled",0);
		if(!m_pIrcView->isLogging() && bEnableLogs)
		{
			QString szTmp;
			getBaseLogFileName(szTmp);
			m_pIrcView->startLogging();
		}
	}
	*/
}

QPixmap * KviWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::DefaultIcon);
}

void KviWindow::fillCaptionBuffers()
{
	QString szCaption = m_szPlainTextCaption;
	if(szCaption.isEmpty())
		szCaption = m_szName;

	fillSingleColorCaptionBuffers(szCaption);
}

void KviWindow::updateCaption()
{
	fillCaptionBuffers();
	bool bHaltOutput = false;
	bHaltOutput = KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnWindowTitleRequest,this,id(),m_szPlainTextCaption);

	if(!bHaltOutput)
	{
		if(mdiParent())
			mdiParent()->setWindowTitle(m_szPlainTextCaption);
		else
			setWindowTitle(m_szPlainTextCaption);
	}
	if(m_pWindowListItem)
		m_pWindowListItem->captionChanged();
}

void KviWindow::createSystemTextEncodingPopup()
{
	if(!g_pMdiWindowSystemTextEncodingPopup)
	{
		// first time called, create the menu
		g_pMdiWindowSystemTextEncodingPopup = new KviTalPopupMenu();
		g_pMdiWindowSystemTextEncodingPopupStandard = new KviTalPopupMenu();
		g_pMdiWindowSystemTextEncodingPopupSmart = new KviTalPopupMenu();
		g_pMdiWindowSystemTextEncodingPopupSmartUtf8 = new KviTalPopupMenu();
		g_pMdiWindowSystemTextEncodingActionGroup = new QActionGroup(g_pMdiWindowSystemTextEncodingPopup);

		//default action
		QTextCodec * pCodec = defaultTextCodec();
		QString szTmp = __tr2qs("Use Default Encoding");
		if(pCodec)
		{
			szTmp += " (";
			szTmp += pCodec->name();
			szTmp += ")";
		}

		g_pMdiWindowSystemTextEncodingDefaultAction = g_pMdiWindowSystemTextEncodingPopup->addAction(szTmp);
		g_pMdiWindowSystemTextEncodingActionGroup->addAction(g_pMdiWindowSystemTextEncodingDefaultAction);
		g_pMdiWindowSystemTextEncodingDefaultAction->setData(-1);
		if(m_szTextEncoding.isEmpty())
		{
			g_pMdiWindowSystemTextEncodingDefaultAction->setCheckable(true);
			g_pMdiWindowSystemTextEncodingDefaultAction->setChecked(true);
		}

		//current action
		g_pMdiWindowSystemTextEncodingCurrentAction = g_pMdiWindowSystemTextEncodingPopup->addAction(__tr2qs("Current: "));
		g_pMdiWindowSystemTextEncodingActionGroup->addAction(g_pMdiWindowSystemTextEncodingCurrentAction);
		g_pMdiWindowSystemTextEncodingCurrentAction->setVisible(false);
		
		// other first level menus
		g_pMdiWindowSystemTextEncodingPopup->insertSeparator();

		g_pMdiWindowSystemTextEncodingPopup->insertItem(__tr2qs("Standard"),g_pMdiWindowSystemTextEncodingPopupStandard);
		g_pMdiWindowSystemTextEncodingPopup->insertItem(__tr2qs("Smart (Send Local)"),g_pMdiWindowSystemTextEncodingPopupSmart);
		g_pMdiWindowSystemTextEncodingPopup->insertItem(__tr2qs("Smart (Send UTF-8)"),g_pMdiWindowSystemTextEncodingPopupSmartUtf8);

		// second level menus (encoding groups)
		QMenu * pPopupStandard[KVI_NUM_ENCODING_GROUPS];
		QMenu * pPopupSmart[KVI_NUM_ENCODING_GROUPS];
		QMenu * pPopupSmartUtf8[KVI_NUM_ENCODING_GROUPS];

		uint u = 0;
		const char * pcEncodingGroup = KviLocale::encodingGroup(u);

		while(pcEncodingGroup)
		{
			pPopupStandard[u] = g_pMdiWindowSystemTextEncodingPopupStandard->addMenu(pcEncodingGroup);
			if(u) //only standard popup contains unicode menu
			{
				pPopupSmart[u] = g_pMdiWindowSystemTextEncodingPopupSmart->addMenu(pcEncodingGroup);
				pPopupSmartUtf8[u] = g_pMdiWindowSystemTextEncodingPopupSmartUtf8->addMenu(pcEncodingGroup);
			}
			
			pcEncodingGroup = KviLocale::encodingGroup(++u);
		}

		// third level menus (encodings)
		uint i = 0;
		KviLocale::EncodingDescription * pDesc = KviLocale::encodingDescription(i);
		while(pDesc->szName)
		{
			szTmp = QString("%1 (%2)").arg(pDesc->szName,pDesc->szDescription);
			if(KviQString::equalCI(m_szTextEncoding,pDesc->szName))
			{
				g_pMdiWindowSystemTextEncodingCurrentAction->setText(__tr2qs("Current: ") + szTmp);
				g_pMdiWindowSystemTextEncodingCurrentAction->setCheckable(true);
				g_pMdiWindowSystemTextEncodingCurrentAction->setChecked(true);
				g_pMdiWindowSystemTextEncodingCurrentAction->setVisible(true);
				g_pMdiWindowSystemTextEncodingCurrentAction->setData(i);
			} else {
				QMenu * pMenu = pDesc->bSmart ? (pDesc->bSendUtf8 ? 
					pPopupSmartUtf8[pDesc->uGroup] : 
					pPopupSmart[pDesc->uGroup]
				) : pPopupStandard[pDesc->uGroup];
			
				QAction * pAction = pMenu->addAction(szTmp);
				pAction->setData(i);
				g_pMdiWindowSystemTextEncodingActionGroup->addAction(pAction);
			}

			pDesc = KviLocale::encodingDescription(++i);
		}
	} else {
		//default action: refresh the name
		QTextCodec * pCodec = defaultTextCodec();
		QString szTmp = __tr2qs("Use Default Encoding");
		if(pCodec)
		{
			szTmp += " (";
			szTmp += pCodec->name();
			szTmp += ")";
		}

		disconnect(g_pMdiWindowSystemTextEncodingActionGroup,SIGNAL(triggered(QAction*)), 0, 0);
		connect(g_pMdiWindowSystemTextEncodingActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(systemTextEncodingPopupActivated(QAction*)));
		g_pMdiWindowSystemTextEncodingDefaultAction->setText(szTmp);

		//menu already exists, choose the right item
		if(m_szTextEncoding.isEmpty())
		{
			//default action; hide the current action
			g_pMdiWindowSystemTextEncodingCurrentAction->setVisible(false);
			g_pMdiWindowSystemTextEncodingDefaultAction->setCheckable(true);
			g_pMdiWindowSystemTextEncodingDefaultAction->setChecked(true);
		} else {
			int i = 0;
			KviLocale::EncodingDescription * pDesc = KviLocale::encodingDescription(i);
			while(pDesc->szName)
			{
				if(KviQString::equalCI(m_szTextEncoding,pDesc->szName))
				{
					szTmp = QString("%1 (%2)").arg(pDesc->szName,pDesc->szDescription);
					g_pMdiWindowSystemTextEncodingCurrentAction->setText(__tr2qs("Current: ") + szTmp);
					g_pMdiWindowSystemTextEncodingCurrentAction->setCheckable(true);
					g_pMdiWindowSystemTextEncodingCurrentAction->setChecked(true);
					g_pMdiWindowSystemTextEncodingCurrentAction->setVisible(true);
					g_pMdiWindowSystemTextEncodingCurrentAction->setData(i);
					break;
				}

				pDesc = KviLocale::encodingDescription(++i);
			}
		}
	}

	disconnect(g_pMdiWindowSystemTextEncodingActionGroup,SIGNAL(triggered(QAction*)),NULL,NULL);
	connect(g_pMdiWindowSystemTextEncodingActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(systemTextEncodingPopupActivated(QAction*)));

}

void KviWindow::systemTextEncodingPopupActivated(QAction * pAction)
{
	if(!pAction || pAction == g_pMdiWindowSystemTextEncodingCurrentAction)
		return;
	if(pAction == g_pMdiWindowSystemTextEncodingDefaultAction)
	{
		setTextEncoding("");
	} else {
		QString szTmp = pAction->text();
		KviQString::cutFromFirst(szTmp," (");
		setTextEncoding(szTmp);
	}
}

void KviWindow::savePropertiesAsDefault()
{
	QString szGroup;
	getConfigGroupName(szGroup);

	if(!KviQString::equalCI(szGroup,typeString()))
	{
		// save also the settings for THIS specialized window
		g_pMainWindow->saveWindowProperties(this,szGroup);
	}

	g_pMainWindow->saveWindowProperties(this,typeString());
}

void KviWindow::contextPopup()
{
	KVS_TRIGGER_EVENT_0(KviEvent_OnWindowPopupRequest,this);
}

void KviWindow::undock()
{
	g_pMainWindow->undockWindow(this);
}

void KviWindow::dock()
{
	g_pMainWindow->dockWindow(this);
	g_pMainWindow->setActiveWindow(this);
}

void KviWindow::delayedAutoRaise()
{
	QTimer::singleShot(0,this,SLOT(autoRaise()));
}

void KviWindow::autoRaise()
{
	if(!mdiParent())
	{
		raise();
		activateWindow();
	}

	if(m_pFocusHandler)
		m_pFocusHandler->setFocus();
	else
		setFocus();
}

void KviWindow::delayedClose()
{
	QTimer::singleShot(0,this,SLOT(close()));
}

void KviWindow::closeEvent(QCloseEvent * pEvent)
{
	pEvent->ignore();
	if(g_pMainWindow)
	{
		g_pMainWindow->childWindowCloseRequest(this);
	} else {
		/* In kvi_app destructor, g_pMainWindow gets deleted before modules gets unloaded.
		 * So if a module tries to destroy a kviwindow while it gets unloaded, we end up here,
		 * having to delete this window without the help of g_pMainWindow.
		 * So we have 3 choices:
		 * 1) delete this => will just print a qt warning "don't delete things on their event handler"
		 * 2) deleteLater() => will tipically create an infinite recursion in the module unload routine
		 * 3) do nothing => same as #2
		 */
		delete this;
	}
}

void KviWindow::updateIcon()
{
	if(parent())
	{
		((KviMdiChild *)parent())->setIcon(*myIconPtr());
	} else {
		setWindowIcon(QIcon(*myIconPtr()));
	}
}

void KviWindow::youAreDocked()
{
	m_bIsDocked = true;
	((KviMdiChild *)parent())->setIcon(*myIconPtr());
	updateCaption();
}

void KviWindow::youAreUndocked()
{
	m_bIsDocked = false;
	setWindowIcon(QIcon(*myIconPtr()));
	updateCaption();
}

#ifdef FocusIn
// Hack for X.h
#undef FocusIn
#endif

void KviWindow::activateSelf()
{
	if(mdiParent())
	{
		// raise and set focus
		mdiParent()->activate();
	} else {
		raise();
		setFocus();
	}
}

void KviWindow::focusInEvent(QFocusEvent *)
{
	if(m_pLastFocusedChild)
	{
		if(m_pLastFocusedChild->hasFocus() && m_pLastFocusedChild->isVisible())
		{
			// focus is still in this window.
			// just make sure that we're the active one.
			if(g_pActiveWindow != this)
				g_pMainWindow->childWindowActivated(this);
			return;
		}
	}

	// focus doesn't seem to be in this window
	if(!m_pFocusHandler)
	{
		// figure out a child to give focus to.
		// we probably have no KviInput since it would have been grabbed anyway

		if(m_pIrcView)
			m_pFocusHandler = m_pIrcView;
		else {
			QList<QObject *> list = children();
			for(QList<QObject *>::Iterator it = list.begin(); it != list.end(); ++it)
			{
				QObject * pObj = *it;
				if(pObj->isWidgetType())
				{
					m_pFocusHandler = (QWidget *)pObj;
					break;
				}
			}
		}

		if(m_pFocusHandler)
			m_pFocusHandler->setFocus();
		else {
			// else too bad :/
			qDebug("No widget able to handle focus for window %s",objectName().toUtf8().data());
			updateCaption(); // do it anyway
			return;
		}
	} else {
		m_pFocusHandler->setFocus();
	}

	// Setting the focus to the focus handler usually
	// triggers our filter for the children's focusInEvent
	// which in turn should invoke our filter and make this window the active one.
	// So we should be already the active window at this point.
	// If we're not, then fix this.
	if(g_pActiveWindow != this)
		g_pMainWindow->childWindowActivated(this);
	updateCaption();
}

bool KviWindow::eventFilter(QObject * pObject, QEvent * pEvent)
{
	switch(pEvent->type())
	{
		case QEvent::FocusIn:
			// a child got focused
			m_pLastFocusedChild = (QWidget *)pObject;
			if(g_pActiveWindow != this)
				g_pMainWindow->childWindowActivated(this);
			break;
		case QEvent::Enter:
			// this is a handler moved here from KviMdiChild::eventFilter
			if(QApplication::overrideCursor())
				QApplication::restoreOverrideCursor();
			break;
#if 0
		case QEvent::MouseButtonPress:
			if((((QWidget *)pObject)->focusPolicy() == Qt::NoFocus) ||
				(((QWidget *)pObject)->focusPolicy() == Qt::TabFocus))
			{
				// this will not focus our window
				// set the focus to the focus handler
				if(m_pLastFocusedChild)
				{
					if(m_pLastFocusedChild->hasFocus() && m_pLastFocusedChild->isVisible())
						return false;
				}

				if(m_pFocusHandler)
					m_pFocusHandler->setFocus();
				else
					setFocus(); // we grab the focus (someone must do it, damn :D)
			}
			break;
#endif
		case QEvent::ChildAdded:
			if(((QChildEvent *)pEvent)->child()->isWidgetType())
				childInserted((QWidget *)((QChildEvent *)pEvent)->child());
			break;
		case QEvent::ChildRemoved:
			if(((QChildEvent *)pEvent)->child()->isWidgetType())
				childRemoved((QWidget *)((QChildEvent *)pEvent)->child());
			break;
		default: /* make gcc happy */ break;
	}
	return false;
}

void KviWindow::childInserted(QWidget * pObject)
{
	pObject->removeEventFilter(this); // ensure that we don't filter twice
	pObject->installEventFilter(this); // we filter its events

	connect(pObject,SIGNAL(destroyed()),this,SLOT(childDestroyed()));

	// attempt to grab a decent focus handler

	if(pObject->inherits("KviInput"))
	{
		// KviInput is our preferential focus handler
		m_pFocusHandler = pObject;
	} else {
		// not a KviInput
		if(!m_pFocusHandler && (pObject->focusPolicy() == Qt::StrongFocus))
		{
			// still without a focus handler: take this widget (possibly only temporairly)
			m_pFocusHandler = pObject;
		}
	}

	QList<QObject *> list = pObject->children();
	for(QList<QObject *>::Iterator it = list.begin(); it != list.end(); ++it)
	{
		QObject * pObj = *it;
		if(pObj->isWidgetType())
		{
			childInserted((QWidget *)pObj);
		}
	}
}

void KviWindow::childDestroyed()
{
	QWidget * pWidget = (QWidget *)sender();
	childRemoved(pWidget);
}

void KviWindow::childRemoved(QWidget * pObject)
{
	pObject->removeEventFilter(this);

	if(pObject == m_pFocusHandler)
		m_pFocusHandler = NULL;
	if(pObject == m_pLastFocusedChild)
		m_pLastFocusedChild = NULL;

	QList<QObject *> list = pObject->children();
	for(QList<QObject *>::Iterator it = list.begin();it != list.end();++it)
	{
		QObject * pObj = *it;
		if(pObj->isWidgetType())
		{
			childRemoved((QWidget *)pObj);
		}
	}
}

void KviWindow::childEvent(QChildEvent * pEvent)
{
	if(pEvent->child()->isWidgetType())
	{
		if(pEvent->removed())
			childRemoved((QWidget *)(pEvent->child()));
		else
			childInserted((QWidget *)(pEvent->child()));
	}
	QWidget::childEvent(pEvent);
}

void KviWindow::childrenTreeChanged(QWidget *)
{
	//	if(widgetAdded && m_pFocusHandler)setFocusHandler(m_pFocusHandler,widgetAdded);
	// FIXME: This might be useless
	QResizeEvent * pEvent = new QResizeEvent(size(),size());
	resizeEvent(pEvent);
	delete pEvent;
}

void KviWindow::updateBackgrounds(QObject * pObject)
{
	if(!pObject)
		pObject = this;
	QList<QObject *> list = pObject->children();
	if(list.count())
	{
		for(QList<QObject *>::Iterator it = list.begin(); it != list.end(); ++it)
		{
			QObject * pChild = *it;
			if(pChild->metaObject()->indexOfProperty("TransparencyCapable") != -1)
			{
				//if (child->isWidgetType())
				((QWidget *)pChild)->update();
			}
			updateBackgrounds(pChild);
		}
	}
}

void KviWindow::moveEvent(QMoveEvent * pEvent)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	updateBackgrounds();
#endif
	QWidget::moveEvent(pEvent);
}

void KviWindow::minimize()
{
	if(mdiParent())
	{
		if(!isMinimized())
			mdiParent()->minimize();
	}
	else
		showMinimized();
}

void KviWindow::maximize()
{
	if(mdiParent())
	{
		if(!isMaximized())
			mdiParent()->maximize();
	}
	else
		showMaximized();
	autoRaise();
}

bool KviWindow::isMinimized()
{
	if(mdiParent())
		return (mdiParent()->state() == KviMdiChild::Minimized);
	return QWidget::isMinimized();
}

bool KviWindow::isMaximized()
{
	if(mdiParent())
	{
		return (mdiParent()->state() == KviMdiChild::Maximized);
	}
	return QWidget::isMaximized();
}

void KviWindow::restore()
{
	if(mdiParent())
		mdiParent()->restore();
	else
		showNormal();
	autoRaise();
}

QRect KviWindow::externalGeometry()
{
#ifndef COMPILE_ON_MAC
	return mdiParent() ? mdiParent()->geometry() : frameGeometry();
#else
	return mdiParent() ? mdiParent()->geometry() : geometry();
#endif
}

void KviWindow::applyOptions()
{
	updateCaption();
	if(m_pIrcView)
		m_pIrcView->applyOptions();
	if(m_pInput)
		m_pInput->applyOptions();

	// trick: relayout
	resize(width() - 1,height() - 1);
	resize(width() + 1,height() + 1);
}

KviWindow * KviWindow::outputProxy()
{
	return 0;
}

void KviWindow::lostUserFocus()
{
	if(!m_pIrcView)
		return;
	if(m_pIrcView->hasLineMark())
		m_pIrcView->clearLineMark(true);
}

void KviWindow::internalOutput(KviIrcView * pView, int iMsgType, const kvi_wchar_t * pwText, int iFlags)
{
	// all roads lead to Rome :)

	if(pView)
	{
		if((this != g_pActiveWindow) || (!isActiveWindow()))
		{
			iFlags |= KviIrcView::TriggersNotification;
			if(!pView->hasLineMark())
			{
				iFlags |= KviIrcView::SetLineMark;
			}
		}
		pView->appendText(iMsgType,pwText,iFlags);
	} else {
		// Redirect to the output proxy
		KviWindow * pWnd = outputProxy();
		if(pWnd)
			pWnd->outputNoFmt(iMsgType,pwText,iFlags);
	}

	if(!m_pWindowListItem)
		return;

	// if this option is checked we don't highlight other than channel msg
	if(KVI_OPTION_BOOL(KviOption_boolHighlightOnlyNormalMsg))
	{
		if((iMsgType != KVI_OUT_CHANPRIVMSG) && (iMsgType != KVI_OUT_CHANPRIVMSGCRYPTED))
		{
			if(!(
					(
						KVI_OPTION_BOOL(KviOption_boolHighlightOnlyNormalMsgQueryToo) &&
						(
							(iMsgType == KVI_OUT_QUERYPRIVMSG) || (iMsgType == KVI_OUT_QUERYTRACE) ||
							(iMsgType == KVI_OUT_QUERYPRIVMSGCRYPTED) || (iMsgType == KVI_OUT_QUERYNOTICE) || (iMsgType == KVI_OUT_QUERYNOTICECRYPTED)
						)
					)
					||
					(
						KVI_OPTION_BOOL(KviOption_boolHighlightOnlyNormalMsgHighlightInChanToo) && (iMsgType == KVI_OUT_HIGHLIGHT)
					)
				)
			)
			return;
		}
	}

	if(KVI_OPTION_BOOL(KviOption_boolHighlightOnlyAtCostumHighlightLevel) &&
			(KVI_OPTION_MSGTYPE(iMsgType).level() < ((int)(KVI_OPTION_UINT(KviOption_uintMinHighlightLevel)))))
	{
		return;
	}

	m_pWindowListItem->highlight(KVI_OPTION_MSGTYPE(iMsgType).level());
}

void KviWindow::output(int iMsgType, const char * pcFormat, ...)
{
	QString szFmt(pcFormat);
	kvi_va_list l;
	kvi_va_start(l,pcFormat);
	QString szBuf;
	KviQString::vsprintf(szBuf,szFmt,l);
	kvi_va_end(l);
	preprocessMessage(szBuf);
	const QChar * pC = szBuf.constData();
	if(!pC)
		return;
	internalOutput(m_pIrcView,iMsgType,(kvi_wchar_t *)pC);
}

void KviWindow::output(int iMsgType, const QString & szFmt, ...)
{
	kvi_va_list l;
	kvi_va_start_by_reference(l,szFmt);
	QString szBuf;
	KviQString::vsprintf(szBuf,szFmt,l);
	kvi_va_end(l);
	preprocessMessage(szBuf);
	const QChar * pC = szBuf.constData();
	if(!pC)
		return;
	internalOutput(m_pIrcView,iMsgType,(kvi_wchar_t *)pC);
}

void KviWindow::output(int iMsgType, const kvi_wchar_t * pwFormat, ...)
{
	QString szFmt = QString::fromUtf8(KviCString(pwFormat).ptr());
	kvi_va_list l;
	kvi_va_start(l,pwFormat);
	QString szBuf;
	KviQString::vsprintf(szBuf,szFmt,l);
	kvi_va_end(l);
	preprocessMessage(szBuf);
	const QChar * pC = szBuf.constData();
	if(!pC)
		return;
	internalOutput(m_pIrcView,iMsgType,(kvi_wchar_t *)pC);
}

void KviWindow::outputNoFmt(int iMsgType, const char * pcText, int iFlags)
{
	QString szText(pcText);
	preprocessMessage(szText);
	const QChar * pC = szText.constData();
	if(!pC)
		return;
	internalOutput(m_pIrcView,iMsgType,(kvi_wchar_t *)pC,iFlags);
}

void KviWindow::outputNoFmt(int iMsgType, const QString & szText, int iFlags)
{
	QString szBuf(szText);
	preprocessMessage(szBuf);
	const QChar * pC = szBuf.constData();
	if(!pC)
		return;
	internalOutput(m_pIrcView,iMsgType,(kvi_wchar_t *)pC,iFlags);
}

void KviWindow::unhighlight()
{
	if(!m_pWindowListItem)
		return;
	m_pWindowListItem->unhighlight();
}

void KviWindow::preprocessMessage(QString & szMessage)
{
	// FIXME: slow

	if(!m_pConsole)
		return;
	if(!m_pConsole->connection())
		return;

	static QString szNonStandardLinkPrefix = QString::fromAscii("\r![");

	if(szMessage.contains(szNonStandardLinkPrefix))
		return; // contains a non standard link that may contain spaces, do not break it.

	// FIXME: This STILL breaks $fmtlink() in certain configurations

	QStringList strings = szMessage.split(" ");
	for(QStringList::Iterator it = strings.begin(); it != strings.end(); ++it )
	{
		if(it->contains('\r'))
			continue;
		QString szTmp(*it);
		szTmp = KviControlCodes::stripControlBytes(szTmp).trimmed();
		if(szTmp.length() < 1)
			continue;
		if(m_pConsole->connection()->serverInfo()->supportedChannelTypes().contains(szTmp[0]))
		{
			if((*it) == szTmp)
				*it = QString("\r!c\r%1\r").arg(*it);
			else
				*it = QString("\r!c%1\r%2\r").arg(szTmp,*it);
		}
	}
	szMessage = strings.join(" ");
}

QTextCodec * KviWindow::defaultTextCodec()
{
	// if we have a connection try to inherit from there...
	if(connection())
	{
		QTextCodec * pCodec = connection()->textCodec();
		if(pCodec)
			return pCodec;
	}
	return KviApplication::defaultTextCodec();
}

KviIrcConnection * KviWindow::connection()
{
	if(console())
		if(console()->context())
			return console()->context()->connection();
	return 0;
}

KviIrcContext * KviWindow::context()
{
	if(console())
	{
		if(console() == this)
			return ((KviConsoleWindow*)this)->context();
		else
			return console()->context();
	}
	return 0;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviWindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
