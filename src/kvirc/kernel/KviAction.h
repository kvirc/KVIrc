#ifndef _KVI_ACTION_H_
#define _KVI_ACTION_H_
//=============================================================================
//
//   File : KviAction.h
//   Creation date : Sun 21 Nov 2004 03:36:34 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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
* \file KviAction.h
* \author Szymon Stefanek
* \brief Actions handling
*/

#include "kvi_settings.h"
#include "KviIconManager.h"

#include <QObject>
#include <QPointer>
#include <QShortcut>
#include <QString>

#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

class KviCustomToolBar;
class QMenu;
class QPixmap;

#undef None

/**
* \class KviActionCategory
* \brief Holds the categories of an action
*
* Known categories are: general (or empty), irc, script
*/
class KVIRC_API KviActionCategory
{
protected:
	QString m_szName;
	QString m_szVisibleName;
	QString m_szDescription;

public:
	/**
	* \brief Constructs an action category object
	* \param szName The name of the category
	* \param szVisibleName Permanent visible name, visible at least in the action drawer
	* \param szDescription Description of the category
	* \return KviActionCategory
	*/
	KviActionCategory(QString szName, QString szVisibleName, QString szDescription)
	    : m_szName(std::move(szName)),
	      m_szVisibleName(std::move(szVisibleName)),
	      m_szDescription(std::move(szDescription))
	{
	}

public:
	/**
	* \brief Returns the name of the category
	* \return const QString &
	*/
	const QString & name() const { return m_szName; }

	/**
	* \brief Returns the visible name of the category
	* \return const QString &
	*/
	const QString & visibleName() const { return m_szVisibleName; }

	/**
	* \brief Returns the description of the category
	* \return const QString &
	*/
	const QString & description() const { return m_szDescription; }
};

/**
* \class KviAction
* \brief Defines an action inside KVIrc
*/
class KVIRC_API KviAction : public QObject
{
	friend class KviActionManager;
	Q_OBJECT
public:
	/**
	* \enum InternalFlags
	* \brief Holds the internal flags of an action
	*/
	enum InternalFlags
	{
		Enabled = 1,  /**< the action is enabled */
		SetupDone = 2 /**< the setup of the action is done */
	};

	/**
	* \enum Flags
	* \brief Holds the flags of an action
	*/
	enum Flags
	{
		NeedsContext = 1,                                                                 /**< the action needs a context */
		NeedsConnection = 2,                                                              /**< the action needs a connection; implies NeedsContext */
		WindowConsole = 4,                                                                /**< the action is bound to a console window */
		WindowChannel = 8,                                                                /**< the action is bound to a channel window */
		WindowQuery = 16,                                                                 /**< the action is bound to a query window */
		WindowDccChat = 32,                                                               /**< the action is bound to a DCC Chat window */
		InternalWindowMask = WindowConsole | WindowChannel | WindowQuery | WindowDccChat, /**< the action is bound to a window */
		EnableAtLogin = 64,                                                               /**< the action is enabled at login; implies NeedsConnection */
		WindowOnlyIfUsersSelected = 128                                                   /**< the action is bound to a window only if it's selected by the user; implies at least one of WindowConsole | WindowChannel | WindowQuery */
	};

	/**
	* \brief Constructs the action object
	* \param pParent The parent object
	*
	* It can be 0, but using a QObject will help in deleting this action :)
	* \param szName Internal name of this action, in form [module.]name
	* \param szVisibleName Permanent visible name, visible at least in the action drawer
	* \param szDescription What this action does
	* \param pCategory One of KviActionManager::category*() or 0 (default category), not owned!
	* \param szBigIconId The big icon associated to the action (32x32)
	* \param szSmallIconId The small icon associated to the action (16x16)
	* \param uFlags The flags of the action, like needs and configuration
	* \param szKeySequence The shortcut to activate the action
	* \note The action name is INTERNAL: it should be never shown to the user. If it
	* contains a dot then the part before the dot is considered to be a module name to be
	* loaded when the module is loaded.
	* \return KviAction
	*/
	KviAction(
	    QObject * pParent,
	    QString szName,
	    QString szVisibleName,
	    QString szDescription,
	    KviActionCategory * pCategory = nullptr,
	    QString szBigIconId = QString(),
	    QString szSmallIconId = QString(),
	    unsigned int uFlags = 0,
	    QString szKeySequence = QString());

	/**
	* \brief Constructs the action object
	* \param pParent The parent object
	*
	* It can be 0, but using a QObject will help in deleting this action :)
	* \param szName Internal name of this action, in form [module.]name
	* \param szVisibleName Permanent visible name, visible at least in the action drawer
	* \param szDescription What this action does
	* \param pCategory One of KviActionManager::category*() or 0 (default category), not owned!
	* \param szBigIconId The big icon associated to the action (32x32)
	* \param eIcon The small icon associated to the action (16x16)
	* \param uFlags The flags of the action, like needs and configuration
	* \param szKeySequence The shortcut to activate the action
	* \note The action name is INTERNAL: it should be never shown to the user. If it
	* contains a dot then the part before the dot is considered to be a module name to be
	* loaded when the module is loaded.
	* \return KviAction
	*/
	KviAction(
	    QObject * pParent,
	    QString szName,
	    QString szVisibleName,
	    QString szDescription,
	    KviActionCategory * pCategory = nullptr,
	    QString szBigIconId = QString(),
	    KviIconManager::SmallIcon eSmallIcon = KviIconManager::None,
	    unsigned int uFlags = 0,
	    QString szKeySequence = QString());

	/**
	* \brief Destroys the action object
	*/
	virtual ~KviAction();

protected:
	QString m_szName;
	QString m_szVisibleName;
	QString m_szDescription;
	KviActionCategory * m_pCategory = nullptr;
	QString m_szBigIconId;
	QString m_szSmallIconId; // this is alternative to m_eSmallIcon
	KviIconManager::SmallIcon m_eSmallIcon;
	std::unordered_set<QAction *> m_pActionList;
	unsigned short int m_uInternalFlags;
	unsigned int m_uFlags;
	QString m_szKeySequence;
	QPointer<QShortcut> m_pAccel;

public:
	/**
	* \brief Validates the flags of the action
	* \param iFlagsToValidate The flags to validate
	* \return int
	*/
	static int validateFlags(int iFlagsToValidate);

	/**
	* \brief Returns the name of the action
	* \return const QString &
	*/
	const QString & name() const { return m_szName; }

	/**
	* \brief Returns the visible name of the action
	* \return const QString &
	*/
	virtual const QString & visibleName();

	/**
	* \brief Returns the description of the action
	* \return const QString &
	*/
	virtual const QString & description();

	/**
	* \brief Returns the shortcut of the action
	* \return const QString &
	*/
	const QString & keySequence() const { return m_szKeySequence; }

	/**
	* \brief Returns the id of the big icon associated to the action
	* \return const QString &
	*/
	const QString & bigIconId() const { return m_szBigIconId; }

	/**
	* \brief Returns the id of the small icon associated to the action
	* \return const QString &
	*/
	const QString & smallIconId() const { return m_szSmallIconId; }

	/**
	* \brief Returns the category of the action
	* \return const QString &
	*/
	KviActionCategory * category() const { return m_pCategory; }

	/**
	* \brief Returns true if the action is enabled
	* \return bool
	*/
	bool isEnabled() const { return (m_uInternalFlags & KviAction::Enabled); }

	/**
	* \brief Returns the flag associated to the action
	* \return unsigned int
	*/
	unsigned int flags() const { return m_uFlags; }

	/**
	* \brief Returns true if the action is user-defined
	* \warning By default, this function returns always false
	* \return bool
	*/
	virtual bool isKviUserActionNeverOverrideThis();

	/**
	* \brief Enables the action
	* \param bEnabled Whether to enable the action
	* \return void
	*/
	virtual void setEnabled(bool bEnabled);

	/**
	* \brief Returns the small icon associated to the action
	* \return QPixmap *
	*/
	QPixmap * smallIcon();

	/**
	* \brief Returns the big icon associated to the action
	* \return QPixmap *
	*/
	QPixmap * bigIcon();

	/**
	* \brief Adds the action to the given popup
	* \param pMenu The menu where to add the action
	* \return bool
	*/
	virtual bool addToPopupMenu(QMenu * pMenu);

	/**
	* \brief Adds the action to the given toolbar
	* \param pParentToolBar The toolbar where to add the action
	* \return QAction *
	*/
	virtual QAction * addToCustomToolBar(KviCustomToolBar * pParentToolBar);

	/**
	* \brief Destroys itself. Maybe the best function in the whole APIs :)
	* \return void
	*/
	void suicide() { delete this; }
protected:
	/**
	* \brief Returns true if the setup is finished
	* \note Called once before the FIRST button or menu item is created
	* \return bool
	*/
	bool setupDone() const { return (m_uInternalFlags & KviAction::SetupDone); }

	/**
	* \brief Enables or disables the action upon starting KVIrc
	* \return void
	*/
	virtual void setup();

	/**
	* \brief Returns the list of actions associated to the action
	* \return std::unordered_set<QAction *>
	*/
	std::unordered_set<QAction *> const & actionList() const { return m_pActionList; }

	/**
	* \brief Registers the action shortcut in the application
	* \return void
	*/
	void registerAccelerator();

	/**
	* \brief Removes the action shortcut from the application
	* \return void
	*/
	void unregisterAccelerator();

	/**
	* \brief Adds the action to the list
	* \param pAction The action to add
	* \return void
	*/
	void registerAction(QAction * pAction);
public slots:
	/**
	* \brief Activates the action
	*
	* It's called when the user activates the action clicking on the toolbar, the menu or by
	* hitting its shortcut.
	* \return void
	*/
	virtual void activate();
protected slots:
	/**
	* \brief Removes the action from the list
	*
	* Called when the action is being destroyed
	* \return void
	*/
	virtual void actionDestroyed();

	/**
	* \brief Reloads the images
	*
	* Called when the application wants to refresh the images in the toolbar
	* \return void
	*/
	virtual void reloadImages();

	/**
	* \brief Enables or disables the action upon checking the active context
	*
	* If the context doesn't exist, the action is disabled
	*
	* Called when the frame changes the active context.
	* \note It works only if NeedsContext is specified
	* \return void
	*/
	virtual void activeContextChanged();

	/**
	* \brief Enables or disables the action upon checking the active context
	*
	* Called when the frame changes the state of the context
	* \return void
	*/
	virtual void activeContextStateChanged();

	/**
	* \brief Enables or disables the action upon checking the active window
	*
	* Called when the frame changes the active window.
	* \return void
	*/
	virtual void activeWindowChanged();

	/**
	* \brief Enables or disables the action upon checking the active window
	*
	* Called when the frame changes the active window.
	* \note We jump here ONLY if m_uFlags & WindowOnlyIfUsersSelected and thus also
	* m_uFlags & InternalWindowMask
	* \return void
	*/
	virtual void activeWindowSelectionStateChanged(bool bSelectedNow);
signals:
	/**
	* \brief Emitted when the action is being activated
	* \return void
	*/
	void activated();
};

#endif // _KVI_ACTION_H_
