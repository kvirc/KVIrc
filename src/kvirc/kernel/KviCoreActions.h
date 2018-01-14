#ifndef _KVI_COREACTIONS_H_
#define _KVI_COREACTIONS_H_
//=============================================================================
//
//   File : KviCoreActions.h
//   Creation date : Mon 22 Nov 2004 02:30:47 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviAction.h"
#include "KviKvsAction.h"

class KviConnectAction : public KviAction
{
	Q_OBJECT
public:
	KviConnectAction(QObject * pParent);

protected:
	QString m_szConnectString;
	QString m_szAbortConnectionString;
	QString m_szDisconnectString;

public:
	bool addToPopupMenu(QMenu * pMenu) override;
	QAction * addToCustomToolBar(KviCustomToolBar * pTool) override;

protected:
	void setup() override;
	void reloadImages() override;
	void activate() override;
	void activeContextChanged() override;
	void activeContextStateChanged() override;
};

class KviSeparatorAction : public KviAction
{
	Q_OBJECT
public:
	KviSeparatorAction(QObject * pParent);

public:
	bool addToPopupMenu(QMenu * pMenu) override;
	QAction * addToCustomToolBar(KviCustomToolBar * pTool) override;
};

class QMenu;

class KviSubmenuAction : public KviKvsAction
{
	Q_OBJECT
public:
	KviSubmenuAction(
	    QObject * pParent,
	    const QString & szName,
	    const QString & szScriptCode,
	    const QString & szVisibleName,
	    const QString & szDescription,
	    KviActionCategory * pCategory = nullptr,
	    const QString & szBigIconId = QString(),
	    KviIconManager::SmallIcon eSmallIcon = KviIconManager::None,
	    unsigned int uFlags = 0);
	~KviSubmenuAction();

protected:
	QMenu * m_pPopup;

protected:
	void setup() override;

public:
	bool addToPopupMenu(QMenu * pMenu) override;
	QAction * addToCustomToolBar(KviCustomToolBar * pTool) override;
protected slots:
	virtual void popupAboutToShow();
	virtual void popupActivated(QAction * pAction);
};

class KviJoinChannelAction : public KviSubmenuAction
{
	Q_OBJECT
public:
	KviJoinChannelAction(QObject * pParent);
protected slots:
	virtual void popupAboutToShow();
	virtual void popupActivated(QAction * pAction);
};

class KviChangeNickAction : public KviSubmenuAction
{
	Q_OBJECT
public:
	KviChangeNickAction(QObject * pParent);
protected slots:
	virtual void popupAboutToShow();
	virtual void popupActivated(QAction * pAction);
};

class KviConnectToServerAction : public KviSubmenuAction
{
	Q_OBJECT
public:
	KviConnectToServerAction(QObject * pParent);
protected slots:
	virtual void popupAboutToShow();
	virtual void popupActivated(QAction * pAction);
};

class KviChangeUserModeAction : public KviSubmenuAction
{
	Q_OBJECT
public:
	KviChangeUserModeAction(QObject * pParent);
protected slots:
	virtual void popupAboutToShow();
	virtual void popupActivated(QAction * pAction);
};

class KviIrcToolsAction : public KviSubmenuAction
{
	Q_OBJECT
public:
	KviIrcToolsAction(QObject * pParent);
protected slots:
	virtual void popupAboutToShow();
	virtual void popupActivated(QAction * pAction);
};

class KviIrcOperationsAction : public KviSubmenuAction
{
	Q_OBJECT
public:
	KviIrcOperationsAction(QObject * pParent);
protected slots:
	virtual void popupAboutToShow();
	virtual void popupActivated(QAction * pAction);
};

#include "KviIrcToolBar.h"

class KviIrcContextDisplayAction : public KviAction
{
	Q_OBJECT
public:
	KviIrcContextDisplayAction(QObject * pParent);

public:
	bool addToPopupMenu(QMenu * pMenu) override;
	QAction * addToCustomToolBar(KviCustomToolBar * pTool) override;
	void activeContextStateChanged() override;
	void activeContextChanged() override;
	void setEnabled(bool) override;
	void setup() override;
};

class KviGoAwayAction : public KviKvsAction
{
	Q_OBJECT
public:
	KviGoAwayAction(QObject * pParent);

protected:
	QString m_szAwayString;
	QString m_szBackString;

public:
	bool addToPopupMenu(QMenu * pMenu) override;
	QAction * addToCustomToolBar(KviCustomToolBar * pTool) override;

protected:
	void setup() override;
	void reloadImages() override;
	void activeContextChanged() override;
	void activeContextStateChanged() override;
};

#endif //_KVI_COREACTIONS_H_
