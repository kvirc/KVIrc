#ifndef _KVI_ACTION_H_
#define _KVI_ACTION_H_
//=============================================================================
//
//   File : kvi_action.h
//   Creation date : Sun 21 Nov 2004 03:36:34 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include <QObject>
#include <QString>
#include <QShortcut>

// The action name is INTERNAL: it should be never shown to the user
// if it contains a dot then the part before the dot is considered to
// be a module name to be loaded when the

// Known categories are: general (or empty), irc, script
//

class KviTalPopupMenu;
class QWidget;
class KviCustomToolBar;
class QPixmap;
class KviKvsScript;

class KVIRC_API KviActionCategory
{
protected:
	QString m_szName;
	QString m_szVisibleName;
	QString m_szDescription;
public:
	KviActionCategory(const QString &szName,const QString &szVisibleName,const QString &szDescription)
		: m_szName(szName), m_szVisibleName(szVisibleName), m_szDescription(szDescription) {};
	~KviActionCategory(){};
public:
	const QString & name(){ return m_szName; };
	const QString & visibleName(){ return m_szVisibleName; };
	const QString & description(){ return m_szDescription; };
};

// these flags are INTERNAL
#define KVI_ACTION_FLAG_ENABLED 1
#define KVI_ACTION_FLAG_SETUPDONE 2

class KVIRC_API KviAction : public QObject
{
	friend class KviActionManager;
	Q_OBJECT
protected:
	QString m_szName;                               // the internal name of the action, must be in form [module.]name
	QString m_szVisibleName;
	QString m_szDescription;
	KviActionCategory * m_pCategory;                // may be 0, not owned!
	QString m_szBigIconId;
	QString m_szSmallIconId;
	KviPointerList<QWidget> * m_pWidgetList;
	unsigned short int m_uInternalFlags;
	unsigned int m_uFlags;
	QString m_szKeySequence;
	QShortcut * m_pAccel;
public:
	enum Flags {
		NeedsContext = 1,
		NeedsConnection = 2,             // implies NeedsContext
		WindowConsole = 4,
		WindowChannel = 8,
		WindowQuery = 16,
		WindowDccChat = 32,
		InternalWindowMask = WindowConsole | WindowChannel | WindowQuery | WindowDccChat,
		EnableAtLogin = 64,              // implies NeedsConnection
		WindowOnlyIfUsersSelected = 128  // implies at least one of WindowConsole | WindowChannel | WindowQuery
	};
public:
	KviAction(QObject * pParent,                    // can be 0, but using a QObject will help in deleting this action :)
			const QString &szName,                      // internal name of this action, in form [module.]name
			const QString &szVisibleName,               // permanent visible name, visible at least in the action drawer
			const QString &szDescription,               // what this action does ?
			KviActionCategory * pCategory = NULL,       // one of KviActionManager::category*() or 0 (default category)
			const QString &szBigIconId = QString(),
			const QString &szSmallIconId = QString(),
			unsigned int uFlags = 0,
			const QString &szKeySequence = QString()
		);
	virtual ~KviAction();
public:
	static int validateFlags(int iFlagsToValidate);
	const QString & name() const { return m_szName; };
	virtual const QString & visibleName();
	virtual const QString & description();
	const QString & keySequence() const { return m_szKeySequence; };
	const QString & bigIconId() const { return m_szBigIconId; };
	const QString & smallIconId() const { return m_szSmallIconId; };
	KviActionCategory * category() const { return m_pCategory; };
	bool isEnabled() const { return (m_uInternalFlags & KVI_ACTION_FLAG_ENABLED); };
	unsigned int flags(){ return m_uFlags; };
	virtual bool isKviUserActionNeverOverrideThis();
	virtual void setEnabled(bool bEnabled);
	virtual QPixmap * smallIcon(); // FIXME: maybe doesn't need to be virtual anymore
	virtual QPixmap * bigIcon();   // FIXME: maybe doesn't need to be virtual anymore
	virtual bool addToPopupMenu(KviTalPopupMenu *pMenu);
	virtual QWidget * addToCustomToolBar(KviCustomToolBar *pParentToolBar);
	void suicide() { delete this; };
protected:
	// called once before the FIRST button or menu item is created
	bool setupDone() const { return (m_uInternalFlags & KVI_ACTION_FLAG_SETUPDONE); };
	virtual void setup();
	KviPointerList<QWidget> * widgetList(){ return m_pWidgetList; };
	void registerAccelerator();
	void unregisterAccelerator();
	void registerWidget(QWidget * b);
protected slots:
	virtual void widgetDestroyed();
	virtual void reloadImages();
	virtual void activeContextChanged();
	virtual void activeContextStateChanged();
	virtual void activeWindowChanged();
	virtual void activeWindowSelectionStateChanged(bool bSelectedNow);
public slots:
	virtual void activate();
signals:
	void activated();
};



#endif //!_KVI_ACTION_H_
