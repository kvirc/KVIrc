//=============================================================================
//
//   File : kvi_window.cpp
//   Creation date : Tue Jul 6 1999 14:52:11 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2008 Szymon Stefanek (pragma at kvirc dot net)
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
* \file kvi_window.cpp
* \brief Contains the KviWindow class
*/

#define KVI_WINDOW_MIN_WIDTH 100
#define KVI_WINDOW_MIN_HEIGHT 100

#define _KVI_WINDOW_CPP_

#define _KVI_DEBUG_CHECK_RANGE_

#include "kvi_debug.h"
#include "kvi_app.h"
#include "kvi_window.h"
#include "kvi_frame.h"
#include "kvi_windowlist.h"
#include "kvi_iconmanager.h"
#include "kvi_mdichild.h"
#include "kvi_locale.h"
#include "kvi_ircview.h"
#include "kvi_out.h"
#include "kvi_malloc.h"
#include "kvi_input.h"
#include "kvi_fileutils.h"
#include "kvi_options.h"
#include "kvi_config.h"
#include "kvi_irccontext.h"
#include "kvi_console.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_mirccntrl.h"
#include "kvi_toolwindows_container.h"
#include "kvi_kvs_script.h"
#include "kvi_tal_popupmenu.h"
#include "kvi_tal_tooltip.h"

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

#ifdef COMPILE_CRYPT_SUPPORT
	#include "kvi_crypt.h"
	#include "kvi_cryptcontroller.h"
#endif

#ifdef COMPILE_KDE_SUPPORT
	#include <KWindowSystem>
#endif

#if defined(COMPILE_ON_WINDOWS)
	#include <windows.h>
#endif

KVIRC_API KviWindow * g_pActiveWindow = 0;

static KviTalPopupMenu * g_pMdiWindowSystemMainPopup = 0;
static KviTalPopupMenu * g_pMdiWindowSystemTextEncodingPopup = 0;
static KviTalPopupMenu * g_pMdiWindowSystemTextEncodingPopupStandard = 0;
static KviTalPopupMenu * g_pMdiWindowSystemTextEncodingPopupSmart = 0;
static KviTalPopupMenu * g_pMdiWindowSystemTextEncodingPopupSmartUtf8 = 0;

unsigned long int g_uUniqueWindowId = 1;

KviWindow::KviWindow(int type,KviFrame * lpFrm,const QString &name,KviConsole * lpConsole)
		: QWidget(0)
{
	m_uId = g_uUniqueWindowId;
	g_uUniqueWindowId++;

	m_szName = name;
	setObjectName(name);

	g_pApp->registerWindow(this);

	m_iType                 = type;
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

void KviWindow::setWindowName(const QString &szName)
{
	m_szName = szName;
	setObjectName(szName);
	emit windowNameChanged();
}

void KviWindow::toggleButtonContainer()
{
	QFrame *pContainer=buttonContainer();
	if(pContainer)
	{
		pContainer->setHidden(!pContainer->isHidden());
	}
}

void KviWindow::reloadImages()
{
	updateIcon();
}

bool KviWindow::hasAttention()
{
	if(((QApplication *)g_pApp)->activeWindow() == 0)return false; // no application window has the focus atm

	if(mdiParent())
	{
		if(frame()->isActiveWindow())return true;
		// This frame is not the active window but the
		// active window still belongs to KVIrc.
		// When the active window is derived from QDialog
		// then it is probably a KVIrc's option dialog
		// or something similar.
		// In this case we assume that the user has the
		// KVIrc window just below and can see it.

		// Handle the special case of the dialogs then
		QWidget * w = ((QApplication *)g_pApp)->activeWindow();
		if(w->inherits("QDialog"))
		{
			// but make sure that the frame is visible at all!
			if(!frame()->isVisible())return false;
			return true;
		}
		// any other class is so unfrequent that we ignore it
	} else {
		// when the window is undocked, instead
		// it is likely to be covered by KVIrc or other windows...
		if(isActiveWindow())return true;
	}
	return false;
}

void KviWindow::demandAttention()
{
	if(mdiParent())
	{
		if(frame()->isActiveWindow())return;
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
		if(isActiveWindow())return;
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

bool KviWindow::focusNextPrevChild(bool next)
{
	QWidget * w = focusWidget();
	if(w)
	{
		if(w->focusPolicy() == Qt::StrongFocus)return false;
		if(w->parent())
		{
			if(w->parent()->metaObject()->indexOfProperty("KviProperty_ChildFocusOwner") == -1)
				return false; // Do NOT change the focus widget!
		}
	}

	return QWidget::focusNextPrevChild(next);
}

void KviWindow::forceTextCodec(QTextCodec * c)
{
	if(!c)return;
	m_pTextCodec = c;
	QTextCodec * dc = defaultTextCodec();
	if(dc != c)
		m_szTextEncoding = c->name();
	else
		m_szTextEncoding = ""; // this is the default anyway
}

bool KviWindow::setTextEncoding(const QString &szTextEncoding)
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
	if(!m_pTextCodec)return defaultTextCodec()->makeEncoder();
	return m_pTextCodec->makeEncoder();
}

bool KviWindow::activityMeter(unsigned int *,unsigned int *)
{
	return false;
}

bool KviWindow::highlightMeter(unsigned int *v)
{
	if(!m_pWindowListItem)
	{
		*v=0;
		return false;
	}
	*v= m_pWindowListItem->highlightLevel();
	return true;
}


bool KviWindow::highlightMe(unsigned int v)
{
	if(v>5) v=5;
	if(m_pWindowListItem)
		m_pWindowListItem->highlight(v);
	return true;
}

const char * KviWindow::m_typeTable[KVI_WINDOW_NUM_TYPES + 1]=
{
	"console",
	"channel",
	"query",
	"help",
	"terminal",
	"editor",
	"dccchat",
	"dccsend",
	"socketspy",
	"links",
	"tool",
	"gnutella",
	"dirbrowser",
	"dcccanvas",
	"dccvoice",
	"list",
	"offer",
	"logview",
	"deadchannel",
	"deadquery",
	"scripteditor",
	"scriptobject",
	"userwindow",
	"debug",
	// <------ NEW TYPES GO HERE!
	"unknown"
};

const char * KviWindow::typeString()
{
	if(m_iType < KVI_WINDOW_NUM_TYPES)
	{
		return m_typeTable[m_iType];
	}
	return m_typeTable[KVI_WINDOW_NUM_TYPES];
}

void KviWindow::setType(int iType)
{
	m_iType = iType;
}

void KviWindow::createWindowListItem()
{
	if(m_pWindowListItem)
		return;
	m_pWindowListItem = g_pFrame->m_pWindowList->addItem(this);
}

void KviWindow::destroyWindowListItem()
{
	if(m_pWindowListItem)
	{
		g_pFrame->m_pWindowList->removeItem(m_pWindowListItem);
		m_pWindowListItem = 0;
	}
}

QToolButton * KviWindow::createToolButton(QWidget * par,const char * nam,int pixon,int,const QString & tooltip,bool bOn)
{
	QToolButton * b = new QToolButton(par);
	b->setObjectName(nam);
	b->setIcon(QIcon(*(g_pIconManager->getSmallIcon(pixon))));
	b->setAutoRaise(true);
	KviTalToolTip::add(b,tooltip);
	b->setChecked(bOn);
	return b;
}

// This is always defined...
void KviWindow::createCryptControllerButton(QWidget *)
{
#ifdef COMPILE_CRYPT_SUPPORT
	m_pCryptControllerButton = new KviWindowToolPageButton(KVI_SMALLICON_UNLOCKEDOFF,KVI_SMALLICON_UNLOCKED,__tr2qs("Crypting"),buttonContainer(),false,"crypt_controller_button");
	connect(m_pCryptControllerButton,SIGNAL(clicked()),this,SLOT(toggleCryptController()));
#endif // COMPILE_CRYPT_SUPPORT
}

void KviWindow::createTextEncodingButton(QWidget * par)
{
	if(m_pTextEncodingButton)delete m_pTextEncodingButton;
	m_pTextEncodingButton = createToolButton(par,"text_encoding_button",KVI_SMALLICON_TEXTENCODING,KVI_SMALLICON_TEXTENCODING,__tr2qs("Private Text Encoding"),false);
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
			m_pCryptController = new KviCryptController(m_pSplitter,m_pCryptControllerButton,"crypt_controller",this,m_pCryptSessionInfo);
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
void KviWindow::setCryptSessionInfo(KviCryptSessionInfo * inf)
{
	if(m_pCryptSessionInfo)
		KviCryptController::destroyCryptSessionInfo(&m_pCryptSessionInfo);
	m_pCryptSessionInfo = inf;
	if(m_pCryptSessionInfo)
	{
		connect(m_pCryptSessionInfo->pEngine,SIGNAL(destroyed()),this,SLOT(cryptSessionInfoDestroyed()));
	}
	if(m_pCryptControllerButton)
	{
		QIcon is;
		is.addPixmap(*(g_pIconManager->getSmallIcon(m_pCryptSessionInfo ? KVI_SMALLICON_LOCKEDOFF : KVI_SMALLICON_UNLOCKEDOFF)),QIcon::Normal,QIcon::Off);
		is.addPixmap(*(g_pIconManager->getSmallIcon(m_pCryptSessionInfo ? KVI_SMALLICON_LOCKED : KVI_SMALLICON_UNLOCKED)),QIcon::Normal,QIcon::On);
		m_pCryptControllerButton->setIcon(is);

		if(m_pCryptControllerButton->isChecked())
			m_pCryptControllerButton->setChecked(false);
	}
}
#endif // COMPILE_CRYPT_SUPPORT

void KviWindow::cryptControllerFinished()
{
#ifdef COMPILE_CRYPT_SUPPORT
	KviCryptSessionInfo * inf = m_pCryptController->getNewSessionInfo();
	setCryptSessionInfo(inf);
	delete m_pCryptController;
	m_pCryptController = 0;
#endif
}

void KviWindow::cryptSessionInfoDestroyed()
{
#ifdef COMPILE_CRYPT_SUPPORT
	output(KVI_OUT_SYSTEMERROR,__tr2qs("Ops...I've accidentally lost the crypting engine..."));
	m_pCryptSessionInfo->pEngine = 0;
	delete m_pCryptSessionInfo;
	m_pCryptSessionInfo = 0;
#endif
}

void KviWindow::setProgress(int progress)
{
	m_pWindowListItem->setProgress(progress);
}

void KviWindow::listWindowTypes()
{
	outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("List of window types available in this release of KVIrc:"));
	for(int i=0;i< KVI_WINDOW_NUM_TYPES;i++)
		outputNoFmt(KVI_OUT_SYSTEMMESSAGE,m_typeTable[i]);
}

void KviWindow::getConfigGroupName(QString &buf)
{
	buf = typeString();
}

void KviWindow::getDefaultLogFileName(QString &buffer)
{
	// FIXME: #warning "Make it configurable ?"
	QString szDate;
	QDate date(QDate::currentDate());
	
	switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
	{
		case 0:
			szDate = date.toString("yyyy.MM.dd");
			break;
		case 1:
			szDate = date.toString(Qt::ISODate);
			break;
		case 2:
			szDate = date.toString(Qt::SystemLocaleDate);
			break;
	}

	QString base;
	getBaseLogFileName(base);
	KviFileUtils::encodeFileName(base);
	base.replace("%%2e","%2e");
	base=base.toLower();
	QString tmp;
	if(KVI_OPTION_BOOL(KviOption_boolGzipLogs))
	{
		KviQString::sprintf(tmp,"%s_%s_%s.log.gz",typeString(),base.toUtf8().data(),szDate.toUtf8().data());
	} else {
		KviQString::sprintf(tmp,"%s_%s_%s.log",typeString(),base.toUtf8().data(),szDate.toUtf8().data());
	}
	g_pApp->getLocalKvircDirectory(buffer,KviApp::Log,tmp);
}

void KviWindow::getBaseLogFileName(QString &buffer)
{
	buffer = m_szName;
}

void KviWindow::saveProperties(KviConfig *cfg)
{
	// store only the non-default text encoding.
	QString szCodec = m_szTextEncoding;
	QTextCodec * c = defaultTextCodec();
	if(c && m_pTextCodec)
	{
		if(KviQString::equalCI(szCodec,c->name().data()))szCodec = KviQString::Empty; // store "default"
	}
	QString szKey = "TextEncoding_";
	szKey += m_szName;
	cfg->writeEntry(szKey,szCodec);
	if(m_pInput)
	{
		cfg->writeEntry("inputToolButtonsHidden",m_pInput->isButtonsHidden());
		cfg->writeEntry("commandLineIsUserFriendly",m_pInput->isUserFriendly());
	}

	/*
	if(m_pIrcView && m_iType==KVI_WINDOW_TYPE_CHANNEL)
	if(m_pIrcView->isLogging())
		cfg->writeEntry("LoggingEnabled",m_pIrcView->isLogging());
	*/
}

void KviWindow::loadProperties(KviConfig *cfg)
{
	QString szKey = "TextEncoding_";
	szKey += m_szName;
	setTextEncoding(cfg->readQStringEntry(szKey,KviQString::Empty).toUtf8().data());
	if(m_pInput)
	{
		m_pInput->setButtonsHidden(cfg->readBoolEntry("inputToolButtonsHidden",KVI_OPTION_BOOL(KviOption_boolHideInputToolButtons)));
		m_pInput->setUserFriendly(cfg->readBoolEntry("commandLineIsUserFriendly",KVI_OPTION_BOOL(KviOption_boolCommandlineInUserFriendlyModeByDefault)));
	}
	/*
	if(m_pIrcView && m_iType==KVI_WINDOW_TYPE_CHANNEL)
	{
		bool bEnableLogs=cfg->readBoolEntry("LoggingEnabled",0);
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
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_DEFAULTICON);
}

void KviWindow::getWindowListTipText(QString &buffer)
{
	buffer = m_szPlainTextCaption;
}

void KviWindow::setFixedCaption(const QString &szCaption)
{
	m_szPlainTextCaption = szCaption;
}

void KviWindow::fillCaptionBuffers()
{
	QString szCaption = m_szPlainTextCaption;
	if(szCaption.isEmpty())
		szCaption = m_szName;

	fillSingleColorCaptionBuffers(szCaption);
}

void KviWindow::fillSingleColorCaptionBuffers(const QString &szName)
{
	m_szPlainTextCaption = szName;
}

void KviWindow::updateCaption()
{
	fillCaptionBuffers();
	if(mdiParent())
		mdiParent()->setWindowTitle(m_szPlainTextCaption);
	else
		setWindowTitle(m_szPlainTextCaption);
	if(m_pWindowListItem)
		m_pWindowListItem->captionChanged();
}

void KviWindow::createSystemTextEncodingPopup()
{
	if(!g_pMdiWindowSystemTextEncodingPopup)
		g_pMdiWindowSystemTextEncodingPopup = new KviTalPopupMenu();
	else
	{
		g_pMdiWindowSystemTextEncodingPopup->clear();
	}

	if(!g_pMdiWindowSystemTextEncodingPopupStandard)
		g_pMdiWindowSystemTextEncodingPopupStandard = new KviTalPopupMenu();
	else
	{
		g_pMdiWindowSystemTextEncodingPopupStandard->clear();
		disconnect(g_pMdiWindowSystemTextEncodingPopupStandard,SIGNAL(activated(int)),0,0);
	}

	if(!g_pMdiWindowSystemTextEncodingPopupSmart)
		g_pMdiWindowSystemTextEncodingPopupSmart = new KviTalPopupMenu();
	else
	{
		g_pMdiWindowSystemTextEncodingPopupSmart->clear();
		disconnect(g_pMdiWindowSystemTextEncodingPopupSmart,SIGNAL(activated(int)),0,0);
	}

	if(!g_pMdiWindowSystemTextEncodingPopupSmartUtf8)
		g_pMdiWindowSystemTextEncodingPopupSmartUtf8 = new KviTalPopupMenu();
	else
	{
		g_pMdiWindowSystemTextEncodingPopupSmartUtf8->clear();
		disconnect(g_pMdiWindowSystemTextEncodingPopupSmartUtf8,SIGNAL(activated(int)),0,0);
	}

	QTextCodec * c = defaultTextCodec();
	QString tmp = __tr2qs("Use Default Encoding");
	if(c)
	{
		tmp += " (";
		tmp += c->name();
		tmp += ")";
	}

	int id = g_pMdiWindowSystemTextEncodingPopup->insertItem(tmp,this,SLOT(systemTextEncodingPopupDefault()));
	if(m_szTextEncoding.isEmpty())g_pMdiWindowSystemTextEncodingPopup->setItemChecked(id,true);
	g_pMdiWindowSystemTextEncodingPopup->insertSeparator();

	g_pMdiWindowSystemTextEncodingPopup->insertItem(__tr2qs("Standard"),g_pMdiWindowSystemTextEncodingPopupStandard);
	g_pMdiWindowSystemTextEncodingPopup->insertItem(__tr2qs("Smart (Send Local)"),g_pMdiWindowSystemTextEncodingPopupSmart);
	g_pMdiWindowSystemTextEncodingPopup->insertItem(__tr2qs("Smart (Send UTF-8)"),g_pMdiWindowSystemTextEncodingPopupSmartUtf8);

	int i = 0;
	KviLocale::EncodingDescription * d = KviLocale::encodingDescription(i);
	while(d->szName)
	{
		KviQString::sprintf(tmp,"%s (%s)",d->szName,d->szDescription);
		KviTalPopupMenu * ppp = d->bSmart ? (d->bSendUtf8 ? g_pMdiWindowSystemTextEncodingPopupSmartUtf8 : g_pMdiWindowSystemTextEncodingPopupSmart) : g_pMdiWindowSystemTextEncodingPopupStandard;
		id = ppp->insertItem(tmp);
		if(KviQString::equalCI(m_szTextEncoding,d->szName))
			ppp->setItemChecked(id,true);
		i = i + 1;
		d = KviLocale::encodingDescription(i);
	}

	connect(g_pMdiWindowSystemTextEncodingPopupSmart,SIGNAL(activated(int)),this,SLOT(systemTextEncodingPopupSmartActivated(int)));
	connect(g_pMdiWindowSystemTextEncodingPopupSmartUtf8,SIGNAL(activated(int)),this,SLOT(systemTextEncodingPopupSmartUtf8Activated(int)));
	connect(g_pMdiWindowSystemTextEncodingPopupStandard,SIGNAL(activated(int)),this,SLOT(systemTextEncodingPopupStandardActivated(int)));
}

KviTalPopupMenu * KviWindow::generatePopup()
{
	if(!g_pMdiWindowSystemMainPopup)
		g_pMdiWindowSystemMainPopup = new KviTalPopupMenu();
	else
	{
		g_pMdiWindowSystemMainPopup->clear();
		g_pMdiWindowSystemMainPopup->disconnect();
	}

	if(mdiParent())
		g_pMdiWindowSystemMainPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_UNDOCK)),
		                                        __tr2qs("&Undock"),this,SLOT(undock()));
	else
		g_pMdiWindowSystemMainPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DOCK)),
		                                        __tr2qs("&Dock"),this,SLOT(dock()));

	g_pMdiWindowSystemMainPopup->insertSeparator();

	int id = g_pMdiWindowSystemMainPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MINIMIZE)),
		                                        __tr2qs("Mi&nimize"),this,SLOT(minimize()));
	g_pMdiWindowSystemMainPopup->setItemEnabled(id,!isMinimized());
	id = g_pMdiWindowSystemMainPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MAXIMIZE)),
		                                        __tr2qs("Ma&ximize"),this,SLOT(maximize()));
	g_pMdiWindowSystemMainPopup->setItemEnabled(id,!isMaximized());
	id = g_pMdiWindowSystemMainPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_RESTORE)),
		                                        __tr2qs("&Restore"),this,SLOT(restore()));
	g_pMdiWindowSystemMainPopup->setItemEnabled(id,isMinimized()||isMaximized());

	g_pMdiWindowSystemMainPopup->insertSeparator();

	g_pMdiWindowSystemMainPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLOSE)),
	                                        __tr2qs("Close"),this,SLOT(close()));

	g_pMdiWindowSystemMainPopup->insertSeparator();

	if(m_pTextEncodingButton)
	{
		createSystemTextEncodingPopup();
		g_pMdiWindowSystemMainPopup->insertItem(__tr2qs("Text &Encoding"),g_pMdiWindowSystemTextEncodingPopup);
	} // else we don't support setting private encoding anyway


	g_pMdiWindowSystemMainPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_XY)),
	                                        __tr2qs("Set Window Props as &default"),this,SLOT(savePropertiesAsDefault()));

	fillContextPopup(g_pMdiWindowSystemMainPopup);

	return g_pMdiWindowSystemMainPopup;
}

void KviWindow::systemPopupRequest(const QPoint &pnt)
{
	generatePopup();

	g_pMdiWindowSystemMainPopup->popup(pnt);
}

void KviWindow::systemTextEncodingPopupDefault()
{
	// default
	setTextEncoding("");
}

void KviWindow::systemTextEncodingPopupSmartActivated(int id)
{
	if(!g_pMdiWindowSystemTextEncodingPopupSmart)
		return;
	QString tmp = g_pMdiWindowSystemTextEncodingPopupSmart->text(id);
	KviQString::cutFromFirst(tmp," (");
	setTextEncoding(tmp);
}

void KviWindow::systemTextEncodingPopupSmartUtf8Activated(int id)
{
	if(!g_pMdiWindowSystemTextEncodingPopupSmartUtf8)
		return;
	QString tmp = g_pMdiWindowSystemTextEncodingPopupSmartUtf8->text(id);
	KviQString::cutFromFirst(tmp," (");
	setTextEncoding(tmp);
}

void KviWindow::systemTextEncodingPopupStandardActivated(int id)
{
	if(!g_pMdiWindowSystemTextEncodingPopupStandard)
		return;
	QString tmp = g_pMdiWindowSystemTextEncodingPopupStandard->text(id);
	KviQString::cutFromFirst(tmp," (");
	setTextEncoding(tmp);
}

void KviWindow::savePropertiesAsDefault()
{
	QString group;
	getConfigGroupName(group);

	if(!KviQString::equalCI(group,typeString()))
	{
		// save also the settings for THIS specialized window
		g_pFrame->saveWindowProperties(this,group);
	}

	g_pFrame->saveWindowProperties(this,typeString());
}

void KviWindow::contextPopup()
{
	systemPopupRequest(QCursor::pos());
}

void KviWindow::fillContextPopup(KviTalPopupMenu *)
{
	// nothing here
}

void KviWindow::undock()
{
	g_pFrame->undockWindow(this);
}

void KviWindow::dock()
{
	g_pFrame->dockWindow(this);
	g_pFrame->setActiveWindow(this);
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

void KviWindow::closeEvent(QCloseEvent *e)
{
	e->ignore();
	if(g_pFrame)
	{
		g_pFrame->childWindowCloseRequest(this);
	} else {
		/* In kvi_app destructor, g_pFrame gets deleted before modules gets unloaded.
		 * So if a module tries to destroy a kviwindow while it gets unloaded, we end up here,
		 * having to delete this window without the help of g_pFrame.
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
	m_bIsDocked=true;
	((KviMdiChild *)parent())->setIcon(*myIconPtr());
	updateCaption();
}

void KviWindow::youAreUndocked()
{
	m_bIsDocked=false;
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
		mdiParent()->activate();

	g_pFrame->childWindowActivated(this);
}

void KviWindow::setFocus()
{
	// don't trigger the whole Qt focus mechanism..
	// just trigger directly our focusInEvent
	// so we'll redirect the focus to the m_pFocusHandler
	focusInEvent(0);
}

void KviWindow::focusInEvent(QFocusEvent *)
{
	if(m_pLastFocusedChild)
	{
		if(m_pLastFocusedChild->hasFocus() && m_pLastFocusedChild->isVisible())
		{
			// the last focused child still has focus (ehm ???)
			if(g_pActiveWindow != this)activateSelf();
			return;
		}
	}

	if(!m_pFocusHandler)
	{
		// must find one NOW
		// we probably have no KviInput since it would have been grabbed anyway

		if(m_pIrcView)m_pFocusHandler = m_pIrcView;
		else {
			QList<QObject *> list = children();
			for(QList<QObject *>::Iterator it = list.begin();it != list.end();++it)
			{
				QObject * c = *it;
				if(c->isWidgetType())
				{
					m_pFocusHandler = (QWidget *)c;
					break;
				}
			}
		}
		if(m_pFocusHandler)m_pFocusHandler->setFocus();
		else {
			// else too bad :/
			debug("No widget able to handle focus for window %s",objectName().toUtf8().data());
			return;
		}
	} else {
		m_pFocusHandler->setFocus();
	}

	// Setting the focus to the focus handler usually
	// triggers our filter for the children's focusInEvent.
	// This should call activateSelf() and thus
	// we should be already the active window at this point.
	// If we're not, then run activateSelf() to fix this.
	if(g_pActiveWindow != this)activateSelf();
	//else debug("ACTIVE WINDOW IS ALREADY THIS");
	updateCaption();
}

bool KviWindow::eventFilter(QObject *o,QEvent *e)
{
	switch(e->type())
	{
		case QEvent::FocusIn:
			m_pLastFocusedChild = (QWidget *)o;
			if(g_pActiveWindow != this)activateSelf();
			break;
		case QEvent::Enter:
			// this is a handler moved here from KviMdiChild::eventFilter
			if(QApplication::overrideCursor())
				QApplication::restoreOverrideCursor();
			break;
		case QEvent::MouseButtonPress:
			if( (((QWidget *)o)->focusPolicy() == Qt::NoFocus) ||
				(((QWidget *)o)->focusPolicy() == Qt::TabFocus))
			{
				// this will not focus our window
				// set the focus to the focus handler
				if(m_pLastFocusedChild)
				{
					if(m_pLastFocusedChild->hasFocus() && m_pLastFocusedChild->isVisible())
						return false;
				}

				if(m_pFocusHandler)
				{
					m_pFocusHandler->setFocus();
				} else {
					setFocus(); // we grab the focus (someone must do it , damn :D)
				}
			}
			break;
		case QEvent::ChildAdded:
			if(((QChildEvent *)e)->child()->isWidgetType())
				childInserted((QWidget *)((QChildEvent *)e)->child());
			break;
		case QEvent::ChildRemoved:
			if(((QChildEvent *)e)->child()->isWidgetType())
				childRemoved((QWidget *)((QChildEvent *)e)->child());
			break;
		default: /* make gcc happy */ break;
	}
	return false;
}

void KviWindow::childInserted(QWidget * o)
{
	o->removeEventFilter(this); // ensure that we don't filter twice
	o->installEventFilter(this); // we filter its events
	connect(o,SIGNAL(destroyed()),this,SLOT(childDestroyed()));

	if(o->inherits("KviInput") || (m_iType==KVI_WINDOW_TYPE_LOGVIEW && o->inherits("KviIrcView")))
		m_pFocusHandler = o;
	else
	{
		if(!m_pFocusHandler && (o->focusPolicy() == Qt::StrongFocus))
		{
			m_pFocusHandler = o;
		}
	}

	QList<QObject *> list = o->children();
	for(QList<QObject *>::Iterator it = list.begin();it != list.end();++it)
	{
		QObject * c = *it;
		if(c->isWidgetType())
		{
			childInserted((QWidget *)c);
		}
	}
}

void KviWindow::childDestroyed()
{
	QWidget * s = (QWidget *)sender();
	childRemoved(s);
}

void KviWindow::childRemoved(QWidget * o)
{
	//debug("CHILD REMOVED %d",o);
	o->removeEventFilter(this);
	if(o == m_pFocusHandler)
		m_pFocusHandler = 0;
	if(o == m_pLastFocusedChild)
		m_pLastFocusedChild = 0;

	QList<QObject *> list = o->children();
	for(QList<QObject *>::Iterator it = list.begin();it != list.end();++it)
	{
		QObject * c = *it;
		if(c->isWidgetType())
		{
			childRemoved((QWidget *)c);
		}
	}
}

void KviWindow::childEvent(QChildEvent *e)
{
	if(e->child()->isWidgetType())
	{
		if(e->removed())
			childRemoved((QWidget *)(e->child()));
		else
			childInserted((QWidget *)(e->child()));
	}
	QWidget::childEvent(e);
}

void KviWindow::childrenTreeChanged(QWidget *)
{
	//	if(widgetAdded && m_pFocusHandler)setFocusHandler(m_pFocusHandler,widgetAdded);
	// FIXME: This might be useless
	QResizeEvent * e = new QResizeEvent(size(),size());
	resizeEvent(e);
	delete e;
}

void KviWindow::updateBackgrounds(QObject * obj)
{
	if(!obj) obj = this;
	QList<QObject *> list = obj->children();
	if (list.count())
	{
		for(QList<QObject *>::Iterator it = list.begin();it != list.end();++it)
		{
			QObject * child = *it;
			if(child->metaObject()->indexOfProperty("TransparencyCapable") != -1){
			//	if (child->isWidgetType())
				((QWidget *)child)->update();
			}
			updateBackgrounds(child);
		}
	}
}

void KviWindow::moveEvent(QMoveEvent *e)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	updateBackgrounds();
#endif
	QWidget::moveEvent(e);
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
	return mdiParent() ? mdiParent()->restoredGeometry() : frameGeometry();
#else
	return mdiParent() ? mdiParent()->restoredGeometry() : geometry();
#endif
}

void KviWindow::applyOptions()
{
	updateCaption();
	if(m_pIrcView)m_pIrcView->applyOptions();
	if(m_pInput)m_pInput->applyOptions();

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
	if(!m_pIrcView)return;
	if(m_pIrcView->hasLineMark())m_pIrcView->clearLineMark(true);
}

void KviWindow::internalOutput(KviIrcView * pView,int msg_type,const kvi_wchar_t * pText,int iFlags)
{
	// all roads lead to Rome :)

	if(pView)
	{
		if((this != g_pActiveWindow) || (!isActiveWindow()))
		{
			if(!pView->hasLineMark())
			{
				iFlags |= KviIrcView::SetLineMark;
			}
		}
		pView->appendText(msg_type,pText,iFlags);
	} else {
		// Redirect to the output proxy
		KviWindow *wnd = outputProxy();
		if(wnd)wnd->outputNoFmt(msg_type,pText,iFlags);
	}

	if(!m_pWindowListItem) return;

	// if this option is checked we don't highlight other than channel msg
	if(KVI_OPTION_BOOL(KviOption_boolHighlightOnlyNormalMsg))
	{
		if((msg_type != KVI_OUT_CHANPRIVMSG) && (msg_type != KVI_OUT_CHANPRIVMSGCRYPTED))
		{
			if(!(
					(
						KVI_OPTION_BOOL(KviOption_boolHighlightOnlyNormalMsgQueryToo) &&
						(
							(msg_type == KVI_OUT_QUERYPRIVMSG) || (msg_type == KVI_OUT_QUERYTRACE) ||
							(msg_type == KVI_OUT_QUERYPRIVMSGCRYPTED) || (msg_type == KVI_OUT_QUERYNOTICE) || (msg_type == KVI_OUT_QUERYNOTICECRYPTED)
						)
					)
					||
					(
						KVI_OPTION_BOOL(KviOption_boolHighlightOnlyNormalMsgHighlightInChanToo) && (msg_type == KVI_OUT_HIGHLIGHT)
					)
				)
			)
			return;
		}
	}

	if(KVI_OPTION_BOOL(KviOption_boolHighlightOnlyAtCostumHighlightLevel) &&
			(KVI_OPTION_MSGTYPE(msg_type).level() < ((int)(KVI_OPTION_UINT(KviOption_uintMinHighlightLevel)))))
	{
		return;
	}

	m_pWindowListItem->highlight(KVI_OPTION_MSGTYPE(msg_type).level());
}

void KviWindow::output(int msg_type,const char *format,...)
{
	QString szFmt(format);
	kvi_va_list l;
	kvi_va_start(l,format);
	QString szBuf;
	KviQString::vsprintf(szBuf,szFmt,l);
	kvi_va_end(l);
	preprocessMessage(szBuf);
	const QChar * pC = KviQString::nullTerminatedArray(szBuf);
	if(!pC)return;
	internalOutput(m_pIrcView,msg_type,(kvi_wchar_t *)pC);
}

void KviWindow::output(int msg_type,const QString &szFmt,...)
{
	kvi_va_list l;
	kvi_va_start_by_reference(l,szFmt);
	QString szBuf;
	KviQString::vsprintf(szBuf,szFmt,l);
	kvi_va_end(l);
	preprocessMessage(szBuf);
	const QChar * pC = KviQString::nullTerminatedArray(szBuf);
	if(!pC)return;
	internalOutput(m_pIrcView,msg_type,(kvi_wchar_t *)pC);
}

void KviWindow::output(int msg_type,const kvi_wchar_t *format,...)
{
	QString szFmt=QString::fromUtf8(KviStr(format).ptr());
	kvi_va_list l;
	kvi_va_start(l,format);
	QString szBuf;
	KviQString::vsprintf(szBuf,szFmt,l);
	kvi_va_end(l);
	preprocessMessage(szBuf);
	const QChar * pC = KviQString::nullTerminatedArray(szBuf);
	if(!pC)return;
	internalOutput(m_pIrcView,msg_type,(kvi_wchar_t *)pC);
}

void KviWindow::outputNoFmt(int msg_type,const char * text,int iFlags)
{
	QString szText(text);
	preprocessMessage(szText);
	const QChar * pC = KviQString::nullTerminatedArray(szText);
	if(!pC)return;
	internalOutput(m_pIrcView,msg_type,(kvi_wchar_t *)pC,iFlags);
}

void KviWindow::outputNoFmt(int msg_type,const QString &szText,int iFlags)
{
	QString szBuf(szText);
	preprocessMessage(szBuf);
	const QChar * pC = KviQString::nullTerminatedArray(szBuf);
	if(!pC)return;
	internalOutput(m_pIrcView,msg_type,(kvi_wchar_t *)pC,iFlags);
}

void KviWindow::unhighlight()
{
	if(!m_pWindowListItem)return;
	m_pWindowListItem->unhighlight();
}

/* This messes up a bit: for example it breaks the WHOIS output where
   escapes are already present (checking for them here would be an overkill).
   This should be eventually done ONLY for remote user message texts
   in the server parser.

	Fixed
*/

void KviWindow::preprocessMessage(QString & szMessage)
{
	// slow
	QStringList strings = szMessage.split(" ");
	for ( QStringList::Iterator it = strings.begin(); it != strings.end(); ++it ) {
		QString tmp(*it);
		if(tmp.contains('\r')) continue;
		tmp = KviMircCntrl::stripControlBytes(tmp);
		tmp.trimmed();
		if(m_pConsole)
		{
			if(m_pConsole->connection())
			{
				if(m_pConsole->connection()->serverInfo()->supportedChannelTypes().contains(tmp[0]))
				{
					if((*it)==tmp)
					{
						*it=QString("\r!c\r%1\r").arg(*it);
					} else {
						*it=QString("\r!c%1\r%2\r").arg(tmp, *it);
					}
				}
			}
		}
	}
	szMessage=strings.join(" ");
}

QTextCodec * KviWindow::defaultTextCodec()
{
	// if we have a connection try to inherit from there...
	if(connection())
	{
		QTextCodec * c = connection()->textCodec();
		if(c)return c;
	}
	return KviApp::defaultTextCodec();
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
		{
			return ((KviConsole*)this)->context();
		} else {
			return console()->context();
		}
	}
	return 0;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_window.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
