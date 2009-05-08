#ifndef _KVI_QUERY_H_
#define _KVI_QUERY_H_
//=============================================================================
//
//   File : kvi_query.h
//   Creation date : Mon Aug 7 2000 14:19:00 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_ircuserdb.h"
#include "kvi_userlistview.h"
#include "kvi_themedlabel.h"

#include "kvi_pointerhashtable.h"
#include "kvi_tal_grid.h"

class KviConsole;
class KviConfig;
class QSplitter;
class KviWindowToolPageButton;

#define KVI_QUERY_FLAG_DEAD 1

class KVIRC_API KviQuery : public KviWindow
{
	Q_OBJECT
public:
	KviQuery(KviFrame * lpFrm,KviConsole * lpConsole,const QString &nick);
	~KviQuery();
protected:
	KviUserListView                     * m_pUserListView;
	KviWindowToolPageButton             * m_pListViewButton;
	int                                   m_iFlags;
	KviThemedLabel                      * m_pLabel;
	QFrame                              * m_pButtonGrid;
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void loadProperties(KviConfig * cfg);
	virtual void saveProperties(KviConfig * cfg);
	virtual void getBaseLogFileName(QString &buffer);
	virtual void triggerCreationEvents();
public:
	void setDeadQuery();
	void setAliveQuery();
	bool isDeadQuery(){ return m_iFlags & KVI_QUERY_FLAG_DEAD; };
	virtual QSize sizeHint() const;
	virtual const QString & target(){ return windowName(); };
	virtual void applyOptions();
	KviUserListEntry * setTarget(const QString &nick,const QString &user,const QString &host);
	void userAction(KviIrcMask *user,unsigned int uActionType);
	void userAction(const QString &nick,unsigned int uActionType);
	void userAction(const QString &nick,const QString &user,const QString &host,unsigned int uActionType);
	bool nickChange(const QString &oldNick,const QString &newNick);
	void ownMessage(const QString &buffer);
	void ownAction(const QString &buffer);
	int selectedCount(){ return m_pUserListView->selectedCount(); };
	bool avatarChanged(const QString &nick){ return m_pUserListView->avatarChanged(nick); };
	void notifyCommonChannels(const QString &nick,const QString &user,const QString &host,int iChans,const QString &szChans);
	void showListView(bool bShow);
	void mergeQuery(KviQuery * q);
	void updateLabelText();
	QFrame * buttonContainer() { return (QFrame*)m_pButtonGrid; };
protected:
	void notifyTargetChange(const QString &oldNick,const QString &oldUser,const QString &oldHost,const QString &nick,const QString &user,const QString &host);
	QString getInfoLabelText();
	QString getInfoLabelTipText();
protected slots:
	void textViewRightClicked();
	void toggleListView();
	void slotDndEvents(const char *);
};

#endif //_KVI_CHANNEL_H_
