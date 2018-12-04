#ifndef _KVI_WINDOW_H_
#define _KVI_WINDOW_H_
//=============================================================================
//
//   File : KviWindow.h
//   Creation date : Tue Jul 6 1999 14:52:20 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviWindow.h
* \brief Contains the KviWindow class
*/

#include "kvi_settings.h"
#include "KviApplication.h"
#include "KviCString.h"
#include "KviQString.h"
#include "KviIrcContext.h"
#include "KviIrcConnection.h"
#include "KviInput.h"
#include "KviTalHBox.h"
#include "KviTalSplitter.h"
#include "KviIconManager.h"

#include <QFrame>
#include <QWidget>
#include <QToolButton>
#include <QTextEncoder>
#include <QByteArray>
#include <QDateTime>

class QPushButton;
class QPixmap;
class QTextCodec;
class KviMainWindow;
class KviWindowListItem;
class KviConfigurationFile;
class KviIrcView;
class KviConsoleWindow;
class KviIrcConnection;
class KviWindowToolPageButton;
class QMenu;
class KviTalHBox;

#ifdef COMPILE_CRYPT_SUPPORT
class KviCryptController;
class KviCryptSessionInfo;
#endif

#define KVI_WINDOW_TYPE_USER 10000

/**
* \class KviWindow
* \brief Base class for all windows in KVIrc
*/
class KVIRC_API KviWindow : public QWidget
{
	friend class KviInput;
	friend class KviMainWindow;
	friend class KviWindowStack;
	friend class KviWindowListItem;
	friend class KviWindowListButton;
	friend class KviTreeWindowListItem;
	friend class KviTreeWindowList;
	friend class KviTreeWindowListItemDelegate;
	Q_PROPERTY(int KviProperty_ChildFocusOwner READ type)
	Q_OBJECT
public:
	/**
	* \enum ActivityValue
	* \brief Holds the activity meter in value scale
	*/
	enum ActivityValue
	{
		None = 0,    /**< None */
		VeryLow = 1, /**< Very low */
		Low = 2,     /**< Low */
		Medium = 3,  /**< Medium */
		High = 4,    /**< High */
		VeryHigh = 5 /**< Very high */
	};

	/**
	* \enum ActivityTemperature
	* \brief Holds the activity meter in temperature scale
	*/
	enum ActivityTemperature
	{
		Ice = 0,       /**< Ice */
		VeryCold = 1,  /**< Very cold */
		Cold = 2,      /**< Cold */
		Undefined = 3, /**< Undefined */
		Hot = 4,       /**< Hot */
		VeryHot = 5,   /**< Very hot */
		Fire = 6       /**< Fire */
	};

	/**
	* \enum AttentionLevel
	* \brief attention levels usable in hasAttention()
	*/
	enum AttentionLevel
	{
		VisibleAndActive = 0,   /**< The window is visible and active (aka: has user focus) */
		MainWindowIsVisible = 1 /**< The kvirc frame is visible but the window is not the active one */
	};
	/**
	* \enum Type
	* \brief Holds the types of a window; these are used by the KVIrc core and distributed modules
	* \note If you add a new type, insert it just before Unknown
	*/
	enum Type
	{
		Console = 0,
		Channel = 1,
		Query = 2,
		DeadChannel = 3,
		DeadQuery = 4,
		Editor = 5,
		Help = 6,
		Terminal = 7,
		SocketSpy = 8,
		Links = 9,
		List = 10,
		DccChat = 11,
		DccTransfer = 12,
		DccCanvas = 13,
		DccVoice = 14,
		DccVideo = 15,
		UserWindow = 16,
		Tool = 17,
		IOGraph = 18,
		DirBrowser = 19,   //!\ no longer exists please reuse entry
		ScriptEditor = 20,
		ScriptObject = 21,
		LogView = 22,
		Offer = 23,
		Debug = 24,
		Unknown = 25,
		TypeCount = 26
	};

	/**
	* \brief Constructs the window object
	* \param eType The type of the window
	* \param lpFrm The main window
	* \param szName The name of the window
	* \param pConsole The parent console
	* \return KviWindow
	*/
	KviWindow(Type eType, const QString & szName, KviConsoleWindow * pConsole = nullptr);

	/**
	* \brief Destroys the window object
	*/
	~KviWindow();

protected:            // almost private: don't touch :D
	QString m_szName; // the current window name (usually also the target)
	KviConsoleWindow * m_pConsole;
	Type m_eType;
	KviWindowListItem * m_pWindowListItem;
	QWidget * m_pFocusHandler;
	QString m_szPlainTextCaption;
	KviIrcView * m_pIrcView;
	KviInput * m_pInput;
	KviTalSplitter * m_pSplitter;
	KviTalHBox * m_pButtonBox;
	unsigned long int m_uId;
	QString m_szTextEncoding;
#ifdef COMPILE_CRYPT_SUPPORT
	KviWindowToolPageButton * m_pCryptControllerButton;
	KviCryptController * m_pCryptController;
	KviCryptSessionInfo * m_pCryptSessionInfo;
#endif
	QToolButton * m_pTextEncodingButton;
	QToolButton * m_pHideToolsButton;
	QWidget * m_pLastFocusedChild;
	static const char * m_typeTable[TypeCount];
	// text encoding and decoding
	//unsigned int              m_uTextEncoding;
	QTextCodec * m_pTextCodec;
	//KviToolWindowsContainer * m_pEditorsContainer;
	bool m_bIsDocked;
	bool m_bProcessingInputEvent;

public:
	bool isDocked() const { return m_bIsDocked; }

	/**
	* \brief Returns the global ID of this window
	*
	* This is unique in the application
	* \return QString
	*/
	QString id() const { return QString("%1").arg(m_uId); }

	/**
	* \brief Returns the global ID of this window
	*
	* This is unique in the application
	* \return QString
	*/
	unsigned long int numericId() const { return m_uId; }

	/**
	* \brief Returns the name of this window
	* \return const QString &
	*/
	const QString & windowName() const { return m_szName; }

	/**
	* \brief Sets the name of the window
	* \param szName The name of the window
	* \return void
	*/
	void setWindowName(const QString & szName);

	/**
	* \brief Sets the progress for the WindowList item
	*
	* If "progress" makes sense in your window, well, use this
	* \param iProgress The progress to set
	* \return void
	*/
	void setProgress(int iProgress);

	/**
	* \brief Returns the type of the window
	* \return Type
	*/
	Type type() const { return m_eType; }

	/**
	* Returns true if the window is a channel
	*/
	bool isChannel() const { return m_eType == Channel; }

	/**
	* Returns true if the window is a query
	*/
	bool isQuery() const { return m_eType == Query; }

	/**
	* Returns true if the window is a console
	*/
	bool isConsole() const { return m_eType == Console; }

	/**
	* \brief Returns a descriptive name of the window type
	* \return const char *
	*/
	virtual const char * typeString();

	QTextCodec * textCodec() { return m_pTextCodec ? m_pTextCodec : defaultTextCodec(); }
	void forceTextCodec(QTextCodec * pCodec);

	/**
	* \brief Returns the KviIrcView of this window
	*
	* May be nullptr if the window has no KviIrcView (and thus supports no direct output)
	* \return KviIrcView *
	*/
	KviIrcView * view() const { return m_pIrcView; }

	/**
	* \brief Returns the KviIrcView that was last clicked in this window
	*
	* Acts as view() except for split view windows
	* See also: view()
	* \return KviIrcView *
	*/
	virtual KviIrcView * lastClickedView() const { return m_pIrcView; };

	/**
	* \brief Returns the console that this window belongs to
	*
	* May be null for windows that aren't bound to irc contexts
	* \return KviConsoleWindow *
	*/
	KviConsoleWindow * console() const { return m_pConsole; }

	KviIrcContext * context();

	/**
	* \brief Returns the current IRC connection (if any)
	* \return KviIrcConnection *
	*/
	KviIrcConnection * connection();

	/**
	* \brief Returns the splitter of this window
	*
	* It *shouldn't* be null... but... well... who knows ? :D ...better check it
	* \return KviTalSplitter *
	*/
	KviTalSplitter * splitter() const { return m_pSplitter; }

	/**
	* \brief Returns the windowList item
	*
	* The window has ALWAYS a WindowList item
	* \return KviWindowListItem *
	*/
	KviWindowListItem * windowListItem() const { return m_pWindowListItem; }

	// The window *might* have a button container
	virtual QFrame * buttonContainer() { return (QFrame *)m_pButtonBox; };
	virtual void toggleButtonContainer();

	// The window *might* have an output proxy: if it has no view() for example
	virtual KviWindow * outputProxy();

	// The window input widget
	KviInput * input() const { return m_pInput; }

	// The target of this window: empty when it makes no sense :D
	virtual const QString & target() { return KviQString::Empty; };

	// The local nickname bound to this window: might be empty when a local nickname makes no sense
	virtual const QString & localNick() { return KviQString::Empty; };

#ifdef COMPILE_CRYPT_SUPPORT
	KviCryptSessionInfo * cryptSessionInfo()
	{
		return m_pCryptSessionInfo;
	};
	void setCryptSessionInfo(KviCryptSessionInfo * pInfo);
#endif

	virtual bool activityMeter(unsigned int * puActivityValue, unsigned int * puActivityTemperature);

	//* \param puValue Highlight level
	virtual bool highlightMeter(unsigned int * puValue);
	virtual bool highlightMe(unsigned int uValue);

	void unhighlight();

	virtual void getWindowListTipText(QString & szBuffer) { szBuffer = m_szPlainTextCaption; }

	// This is meaningful only if view() is non nullptr
	const QString & lastLineOfText();
	const QString & lastMessageText();

	const QString & textEncoding() const { return m_szTextEncoding; }
	// returns true if the encoding could be successfully set
	bool setTextEncoding(const QString & szTextEncoding);
	// this must return a default text codec suitable for this window
	virtual QTextCodec * defaultTextCodec();
	// encode the text from szSource by using m_uTextEncoding
	QByteArray encodeText(const QString & szText);
	QString decodeText(const char * pcText);
	//return a text encoder
	QTextEncoder * makeEncoder();

	void contextPopup();
	// Raises the window (after a light delay to prevent focus pingpongs)
	void delayedAutoRaise();

	// Retrieves the default log file name: this is pre-build
	void getDefaultLogFileName(QString & szBuffer);
	void getDefaultLogFileName(QString & szBuffer, QDate date, bool bGzip, unsigned int uDatetimeFormat);

	void delayedClose(); // close that jumps out of the current event loop

	// Interesting overridables:
	virtual void getConfigGroupName(QString & szBuffer);
	virtual void getBaseLogFileName(QString & szBuffer);
	virtual void updateCaption();
	virtual void applyOptions();
	virtual void updateIcon();
	virtual void ownMessage(const QString &, bool = true){};
	virtual void ownAction(const QString &){};
	virtual const QString & plainTextCaption() { return m_szPlainTextCaption; };

	void internalOutput(KviIrcView * pView, int iMsgType, const kvi_wchar_t * pwText, int iFlags = 0, const QDateTime & datetime = QDateTime());
	// You *might* want to override these too.. but better don't touch them :D
	virtual void output(int iMsgType, const char * pcFormat, ...);
	virtual void output(int iMsgType, const kvi_wchar_t * pwFormat, ...);
	virtual void output(int iMsgType, QString szFmt, ...);
	void output(int iMsgType, const QDateTime & datetime, const char * pcFormat, ...);
	void output(int iMsgType, const QDateTime & datetime, const kvi_wchar_t * pwFormat, ...);
	void output(int iMsgType, const QDateTime & datetime, QString szFmt, ...);
	virtual void outputNoFmt(int iMsgType, const char * pcText, int iFlags = 0, const QDateTime & datetime = QDateTime());
	virtual void outputNoFmt(int iMsgType, const kvi_wchar_t * pwText, int iFlags = 0, const QDateTime & datetime = QDateTime()) { internalOutput(m_pIrcView, iMsgType, pwText, iFlags, datetime); };
	virtual void outputNoFmt(int iMsgType, const QString & szText, int iFlags = 0, const QDateTime & datetime = QDateTime()); // <-- iFlags are KviIrcView::AppendTextFlags
	// Just helpers.. FIXME: might be redesigned in some other way
	void updateBackgrounds(QObject * pObj = nullptr);

	/**
	* \brief Notify the window manager that this window demands attention
	*
	* Depending on the os/window manager, this method uses different approaches to
	* trigger a notification that typically makes the application entry flash
	* on the application bar.
	* \return void
	*/
	void demandAttention();

	/**
	* \brief Returns whether this window is the active one
	*
	* This method is useful because it takes in consideration that this window
	* can be either docked or undocked, and takes a parameter that defines the strictness
	* of the checks used.
	* Level MainWindowIsVisible is fine to check if we needs to trigger the notifier
	* Level VisibleAndActive is useful to trigger highlights
	* @param eLevel the level of checks to be done
	* \return bool
	*/
	bool hasAttention(AttentionLevel eLevel = VisibleAndActive);

	// This should die, probably
	void listWindowTypes();

	// call this in the constructor if your caption is fixed:
	// it will set m_szPlainTextCaption to szCaption and it will
	// automatically use it without the need of overriding fillCaptionBuffers
	void setFixedCaption(const QString & szCaption) { m_szPlainTextCaption = szCaption; }

	void setWindowTitle(QString & szTitle);

	/**
	* \brief Pastes the most recent channel log into the buffer
	*
	* It searches for the most recent channel log of the channel we have just
	* joined, and pastes into the buffer the last N lines, where N is get from
	* the options
	* \return void
	*/
	void pasteLastLog();

private:
	/**
	* \brief Loads a log file
	*
	* It opens a logfile, gzipped or not, and returns the content in a buffer
	* \param szFileName The filename of the log file
	* \param bGzip Whether the log file is gzipped
	* \return QByteArray
	*/
	QByteArray loadLogFile(const QString & szFileName, bool bGzip);

protected:
	// Loading and saving of properties
	// Protected: only KviMainWindow can call these
	virtual void saveProperties(KviConfigurationFile * pCfg);
	virtual void loadProperties(KviConfigurationFile * pCfg);
	// Creation and destruction events: overridden in windows that have script events bound to creation and destruction
	virtual void triggerCreationEvents(){};
	virtual void triggerDestructionEvents(){};
	// Internal: do not touch :D (KviMainWindow)
	virtual void createWindowListItem();
	virtual void destroyWindowListItem();
	// called by KviMainWindow
	// either lost the active window status or the frame is no longer active (but we're still the active kvirc's subwindow)
	virtual void lostUserFocus();
	// this by default calls fillSingleColorCaptionBuffer(plainTextCaption());
	virtual void fillCaptionBuffers();
	// protected helper
	void fillSingleColorCaptionBuffers(const QString & szName) { m_szPlainTextCaption = szName; }
	// Virtual events that signal dock state change
	virtual void youAreDocked();
	virtual void youAreUndocked();
	// Reimplement to show a special icon in the WindowList items and captions
	virtual QPixmap * myIconPtr();
	// Sets the type of this window: be careful with this
	void setType(Type eType) { m_eType = eType; };

	bool eventFilter(QObject * pObject, QEvent * pEvent) override;

	// Virtuals overridden to manage the internal layouts...
	void moveEvent(QMoveEvent * pEvent) override;
	void closeEvent(QCloseEvent * pEvent) override;
	void childEvent(QChildEvent * pEvent) override;
	void focusInEvent(QFocusEvent *) override;
	void inputMethodEvent(QInputMethodEvent * e) override;

	void childInserted(QWidget * pObject);
	void childRemoved(QWidget * pObject);

	// Internal helpers
	void createCryptControllerButton(QWidget * pPar);
	void createTextEncodingButton(QWidget * pPar);
	void createSystemTextEncodingPopup();

	QToolButton * createToolButton(QWidget * pPar, const char * pcName, KviIconManager::SmallIcon eIcon, const QString & szToolTip, bool bOn);
	// This is called by KviInput: actually it links the widgetAdded
	virtual void childrenTreeChanged(QWidget * pAdded);

	bool focusNextPrevChild(bool bNext) override;

	virtual void preprocessMessage(QString & szMessage);
public slots:
	void dock();
	void undock();
	void autoRaise();
	void reloadImages();
	void savePropertiesAsDefault();
protected slots:
	void toggleCryptController();     // This has to be here even if the crypt support is enabled...moc does not support conditionals
	void cryptControllerFinished();   // same as above
	void cryptSessionInfoDestroyed(); //  same as above
	void textEncodingButtonClicked();
	void systemTextEncodingPopupActivated(QAction * pAction);
	void childDestroyed();
signals:
	void windowNameChanged();
};

#ifndef _KVI_WINDOW_CPP_
// The active window:
//   This is almost always non null
//   The exception is the startup (when there are no windows at all)
//   and the last phase of the destructor.
//   You usually shouldn't care of checking this pointer for nullptr unless
//   you're running very early at startup or very late at shutdown
extern KVIRC_API KviWindow * g_pActiveWindow;
#endif

inline QByteArray KviWindow::encodeText(const QString & szText)
{
	if(m_pTextCodec)
		return m_pTextCodec->fromUnicode(szText);
	else
		return defaultTextCodec()->fromUnicode(szText);
}

inline QString KviWindow::decodeText(const char * pcText)
{
	if(m_pTextCodec)
		return m_pTextCodec->toUnicode(pcText);
	else
		return defaultTextCodec()->toUnicode(pcText);
}

#endif //_KVI_WINDOW_H_
