#ifndef _KVI_WINDOW_H_
#define _KVI_WINDOW_H_
//=============================================================================
//
//   File : kvi_window.h
//   Creation date : Tue Jul 6 1999 14:52:20 by Szymon Stefanek
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
* \file kvi_window.h
* \brief Contains the KviWindow class
*/


#include "kvi_app.h"
#include "kvi_string.h"
#include "kvi_qstring.h"
#include "kvi_settings.h"
#include "kvi_irccontext.h"
#include "kvi_ircconnection.h"
#include "kvi_input.h"
#include "kvi_tal_hbox.h"
#include "kvi_tal_splitter.h"

#ifdef COMPILE_ON_WINDOWS
	// The brain-damaged MSVC compiler can't instantiate QList templates without a destructor definition
	#include "kvi_mdichild.h"
#else
	class KviMdiChild;
#endif

#include <QFrame>
#include <QWidget>
#include <QToolButton>
#include <QTextEncoder>
#include <QByteArray>

class QPushButton;
class QPixmap;
class QTextCodec;
class KviFrame;
class KviWindowListItem;
class KviConfig;
class KviIrcView;
class KviConsole;
class KviIrcConnection;
class KviWindowToolPageButton;
class KviTalPopupMenu;
class KviTalHBox;

#ifdef COMPILE_CRYPT_SUPPORT
	class KviCryptController;
	class KviCryptSessionInfo;
#endif

//
// Window types
//
// There are KVI_WINDOW_NUM_TYPES predefined
// window types: these are used by the KVIrc core and distributed modules
// If you add a new def, increase KVI_WINDOW_NUM_TYPES
//

#define KVI_WINDOW_TYPE_CONSOLE 0
#define KVI_WINDOW_TYPE_CHANNEL 1
#define KVI_WINDOW_TYPE_QUERY 2
#define KVI_WINDOW_TYPE_HELP 3
#define KVI_WINDOW_TYPE_TERM 4
#define KVI_WINDOW_TYPE_EDITOR 5
#define KVI_WINDOW_TYPE_DCCCHAT 6
#define KVI_WINDOW_TYPE_DCCSEND 7
#define KVI_WINDOW_TYPE_SOCKETSPY 8
#define KVI_WINDOW_TYPE_LINKS 9
#define KVI_WINDOW_TYPE_TOOL 10
#define KVI_WINDOW_TYPE_IOGRAPH 11
#define KVI_WINDOW_TYPE_DIRBROWSER 12
#define KVI_WINDOW_TYPE_DCCCANVAS 13
#define KVI_WINDOW_TYPE_DCCVOICE 14
#define KVI_WINDOW_TYPE_LIST 15
#define KVI_WINDOW_TYPE_OFFER 16
#define KVI_WINDOW_TYPE_LOGVIEW 17
#define KVI_WINDOW_TYPE_DEADCHANNEL 18
#define KVI_WINDOW_TYPE_DEADQUERY 19
#define KVI_WINDOW_TYPE_SCRIPTEDITOR 20
#define KVI_WINDOW_TYPE_SCRIPTOBJECT 21
#define KVI_WINDOW_TYPE_USERWINDOW 22
#define KVI_WINDOW_TYPE_DEBUG 23
#define KVI_WINDOW_TYPE_DCCVIDEO 24

#define KVI_WINDOW_NUM_TYPES 25

#define KVI_WINDOW_TYPE_USER 10000

#define KVI_ACTIVITY_NONE 0
#define KVI_ACTIVITY_VERYLOW 1
#define KVI_ACTIVITY_LOW 2
#define KVI_ACTIVITY_MEDIUM 3
#define KVI_ACTIVITY_HIGH 4
#define KVI_ACTIVITY_VERYHIGH 5

#define KVI_ACTIVITY_ICE 0
#define KVI_ACTIVITY_VERYCOLD 1
#define KVI_ACTIVITY_COLD 2
#define KVI_ACTIVITY_UNDEFINED 3
#define KVI_ACTIVITY_HOT 4
#define KVI_ACTIVITY_VERYHOT 5
#define KVI_ACTIVITY_FIRE 6

/**
* \class KviWindow
* \brief Base class for all windows in KVIrc
*/

class KVIRC_API KviWindow : public QWidget
{
	friend class KviInput;
	friend class KviFrame;
	friend class KviWindowListItem;
	friend class KviWindowListButton;
	friend class KviTreeWindowListItem;
	friend class KviTreeWindowList;
	friend class KviTreeWindowListItemDelegate;
	Q_PROPERTY(int KviProperty_ChildFocusOwner READ type)
	Q_OBJECT
public:
	KviWindow(int type,KviFrame * lpFrm,const QString &name,KviConsole * pConsole = 0);
	virtual ~KviWindow();
protected: // almost private: don't touch :D
	QString                               m_szName;                  // the current window name (usually also the target)
	KviFrame                            * m_pFrm;
	KviConsole                          * m_pConsole;

	int                                   m_iType;

	KviWindowListItem                   * m_pWindowListItem;
	QWidget                             * m_pFocusHandler;
	QString                               m_szPlainTextCaption;
	KviIrcView                          * m_pIrcView;
	KviInput                            * m_pInput;
	KviTalSplitter                      * m_pSplitter;
	KviTalHBox                          * m_pButtonBox;
	unsigned long int                     m_uId;
	QString                               m_szTextEncoding;
#ifdef COMPILE_CRYPT_SUPPORT
	KviWindowToolPageButton             * m_pCryptControllerButton;
	KviCryptController                  * m_pCryptController;
	KviCryptSessionInfo                 * m_pCryptSessionInfo;
#endif
	QToolButton                         * m_pTextEncodingButton;
	QToolButton                         * m_pHideToolsButton;
	QWidget                             * m_pLastFocusedChild;
	static const char                   * m_typeTable[KVI_WINDOW_NUM_TYPES + 1];
	// text encoding and decoding
	//unsigned int                          m_uTextEncoding;
	QTextCodec                          * m_pTextCodec;
//	KviToolWindowsContainer             * m_pEditorsContainer;
	bool                                m_bIsDocked;
public:
	inline bool isDocked() { return m_bIsDocked; }
	// The global ID of this window: unique in the application
	inline QString id(){ return QString("%1").arg(m_uId); };
	inline unsigned long int numericId(){ return m_uId; };
	KviTalPopupMenu * generatePopup();

	// THIS is the function that should be used
	inline const QString & windowName(){ return m_szName; };
	void setWindowName(const QString &szName);
	// Sets the progress for the WindowList item: if "progress" makes sense in your window , well , use this
	void setProgress(int progress);
	// Window type management
	inline int type() const { return m_iType; };
	// This returns a descriptive name of the window type
	// if the window is an user window, the typeString returned
	// by THIS implementation is "unknown"
	virtual const char * typeString();

	inline QTextCodec * textCodec(){ return m_pTextCodec ? m_pTextCodec : defaultTextCodec(); };
	void forceTextCodec(QTextCodec * c);

	// The frame that this window belongs to
	// It is always non-null and never changes
	inline KviFrame * frame() const { return m_pFrm; };
	// The KviIrcView of this window: may be NULL if the window has no KviIrcView (and thus supports no direct output)
	inline KviIrcView * view() const { return m_pIrcView; };
	// The mdiParent widget: may be nulll if the window is undocked
	inline KviMdiChild * mdiParent(){ return (KviMdiChild *)parent(); };
	// The console that this window belongs to: may be null for windows that aren't bound to irc contexts
	inline KviConsole * console(){ return m_pConsole; };
	KviIrcContext * context();
	// the current IRC connection (if any)
	KviIrcConnection * connection();
	// The splitter of this window: it *shouldn't* be null... but ... well.. who knows ? :D ...better check it
	inline KviTalSplitter * splitter(){ return m_pSplitter; };
	// The window has ALWAYS a WindowList item
	inline KviWindowListItem * windowListItem(){ return m_pWindowListItem; };
	// The window *might* have a button container
	virtual QFrame * buttonContainer(){ return (QFrame*)m_pButtonBox; };
	virtual void toggleButtonContainer();
	// The window *might* have an output proxy: if it has no view() for example
	virtual KviWindow * outputProxy();
	// The window input widget
	inline KviInput * input(){ return m_pInput; };

	// The target of this window: empty when it makes no sense :D
	virtual const QString & target(){ return KviQString::Empty; };
	// The local nickname bound to this window: might be empty when a local nickname makes no sense
	virtual const QString & localNick(){ return KviQString::Empty; };

#ifdef COMPILE_CRYPT_SUPPORT
	KviCryptSessionInfo * cryptSessionInfo(){ return m_pCryptSessionInfo; };
	void setCryptSessionInfo(KviCryptSessionInfo * i);
#endif

	virtual bool activityMeter(unsigned int * puActivityValue,unsigned int * puActivityTemperature);
	virtual bool highlightMeter(unsigned int * puHighlightValue);
	virtual bool highlightMe(unsigned int v);

	void unhighlight();

	virtual void getWindowListTipText(QString &buffer);

	// This is meaningful only if view() is non NULL
	const QString & lastLineOfText();
	const QString & lastMessageText();

	inline const QString &textEncoding(){ return m_szTextEncoding; };
	// returns true if the encoding could be successfully set
	bool setTextEncoding(const QString &szTextEncoding);
	// this must return a default text codec suitable for this window
	virtual QTextCodec * defaultTextCodec();
	// encode the text from szSource by using m_uTextEncoding
	inline QByteArray encodeText(const QString &szText);
	inline QString decodeText(const char * szText);
	//return a text encoder
	QTextEncoder * makeEncoder();

	void contextPopup();
	// Raises the window (after a light delay to prevent focus pingpongs)
	void delayedAutoRaise();
	// Window state: might work :D
	bool isMinimized();
	bool isMaximized();
	// Retrieves the default log file name: this is pre-build
	void getDefaultLogFileName(QString &buffer);
	// Well...the external geometry :)
	QRect externalGeometry();

	void delayedClose(); // close that jumps out of the current event loop

	// Interesting overridables:
	virtual void fillContextPopup(KviTalPopupMenu * p);
	virtual void getConfigGroupName(QString &buf);
	virtual void getBaseLogFileName(QString &buffer);
	virtual void updateCaption();
	virtual void applyOptions();
	virtual void updateIcon();
	virtual void ownMessage(const QString &){};
	virtual void ownAction(const QString &){};
	virtual const QString & plainTextCaption(){ return m_szPlainTextCaption; };
	virtual void setFocus();

	void internalOutput(KviIrcView * pView,int msg_type,const kvi_wchar_t * text,int iFlags=0);
	// You *might* want to override these too.. but better don't touch them :D
	virtual void output(int msg_type,const char * format,...);
	virtual void outputNoFmt(int msg_type,const char * text,int iFlags=0);
	virtual void output(int msg_type,const kvi_wchar_t * format,...);
	virtual void outputNoFmt(int msg_type,const kvi_wchar_t * text,int iFlags=0){ internalOutput(m_pIrcView,msg_type,text,iFlags); };
	virtual void output(int msg_type,const QString &szFmt,...);
	virtual void outputNoFmt(int msg_type,const QString &szText,int iFlags=0); // <-- these are KviIrcView::AppendTextFlags
	// Just helpers.. FIXME: might be redesigned in some other way
	void updateBackgrounds(QObject * obj = 0);

	void demandAttention();
	bool hasAttention();

	// This should die, probably
	void listWindowTypes();

	void activateSelf();
public slots:
	void dock();
	void undock();
	void autoRaise();
	void maximize();
	void minimize();
	void restore();
	void reloadImages();
protected slots:
	void savePropertiesAsDefault();
	void toggleCryptController();     // This has to be here even if the crypt support is enabled...moc does not support conditionals
	void cryptControllerFinished();   // same as above
	void cryptSessionInfoDestroyed(); //  same as above
	void textEncodingButtonClicked();
	void systemPopupRequest(const QPoint &pnt);
	void systemTextEncodingPopupSmartActivated(int id);
	void systemTextEncodingPopupSmartUtf8Activated(int id);
	void systemTextEncodingPopupStandardActivated(int id);
	void systemTextEncodingPopupDefault();
	void childDestroyed();
signals:
	void windowNameChanged();
protected:
	// Loading and saving of properties
	// Protected: only KviFrame can call these
	virtual void saveProperties(KviConfig *cfg);
	virtual void loadProperties(KviConfig *cfg);
	// Creation and destruction events: overridden in windows that have script events bound to creation and destruction
	virtual void triggerCreationEvents(){};
	virtual void triggerDestructionEvents(){};
	// Internal: do not touch :D (KviFrame)
	virtual void createWindowListItem();
	virtual void destroyWindowListItem();
	// called by KviFrame
	// either lost the active window status or the frame is no longer active (but we're still the active kvirc's subwindow)
	virtual void lostUserFocus();

	// call this in the constructor if your caption is fixed:
	// it will set m_szPlainTextCaption to szCaption and it will
	// automatically use it without the need of overriding fillCaptionBuffers
	void setFixedCaption(const QString &szCaption);
	// this by default calls fillSingleColorCaptionBuffer(plainTextCaption());
	virtual void fillCaptionBuffers();
	// protected helper
	void fillSingleColorCaptionBuffers(const QString &szName);
	// Virtual events that signal dock state change
	virtual void youAreDocked();
	virtual void youAreUndocked();
	// Reimplement to show a special icon in the WindowList items and captions
	virtual QPixmap * myIconPtr();
	// Sets the type of this window: be careful with this
	void setType(int iType);

	bool eventFilter(QObject *o,QEvent *e);

	// Virtuals overridden to manage the internal layouts...
	virtual void moveEvent(QMoveEvent *e);
	virtual void closeEvent(QCloseEvent *e);
	virtual void childEvent(QChildEvent *e);
	virtual void focusInEvent(QFocusEvent *e);

	void childInserted(QWidget * o);
	void childRemoved(QWidget * o);

	// Internal helpers
	void createCryptControllerButton(QWidget * par);
	void createTextEncodingButton(QWidget * par);
	void createSystemTextEncodingPopup();

	QToolButton * createToolButton(QWidget * par,const char * nam,int pixon,int pixoff,const QString & tooltip,bool bOn);
	// This is called by KviInput: actually it links the widgetAdded
	virtual void childrenTreeChanged(QWidget * widgetAdded);

	virtual bool focusNextPrevChild(bool bNext);

	virtual void preprocessMessage(QString & szMessage);
};

#ifndef _KVI_WINDOW_CPP_
	// The active window:
	//   This is almost always non null
	//   The exception is the startup (when there are no windows at all)
	//   and the last phase of the destructor.
	//   You usually shouldn't care of checking this pointer for NULL unless
	//   you're running very early at startup or very late at shutdown
	extern KVIRC_API KviWindow * g_pActiveWindow;
#endif

inline QByteArray KviWindow::encodeText(const QString &szText)
{
	if(m_pTextCodec) {
		return m_pTextCodec->fromUnicode(szText);
	} else {
		return defaultTextCodec()->fromUnicode(szText);
	}
}

inline QString KviWindow::decodeText(const char * szText)
{
	if(m_pTextCodec) {
		return m_pTextCodec->toUnicode(szText);
	} else {
		return defaultTextCodec()->toUnicode(szText);
	}
}

#endif //_KVI_WINDOW_H_
