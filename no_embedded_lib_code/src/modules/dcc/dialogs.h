#ifndef _DIALOGS_H_
#define _DIALOGS_H_
//=============================================================================
//
//   File : dialogs.h
//   Creation date : Tue Sep 19 09 2000 15:17:22 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "broker.h"

#include "kvi_filedialog.h"

#include <QWidget>

class KviDccBox
{
protected:
	KviDccDescriptor * m_pDescriptor;
	KviDccBroker           * m_pBroker;
public:
	KviDccBox(KviDccBroker * br,KviDccDescriptor * dcc);
	virtual ~KviDccBox();
public:
	virtual void forgetDescriptor();
};


class KviDccAcceptBox : public QWidget , public KviDccBox
{
	Q_OBJECT
public:
	KviDccAcceptBox(KviDccBroker * br,KviDccDescriptor * dcc,const QString &text,const QString &capt);
	~KviDccAcceptBox();
protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void showEvent(QShowEvent *e);
private slots:
	void acceptClicked();
	void rejectClicked();
signals:
	void accepted(KviDccBox *,KviDccDescriptor *);
	void rejected(KviDccBox *,KviDccDescriptor *);
};

class KviDccRenameBox : public QWidget , public KviDccBox
{
	Q_OBJECT
public:
	KviDccRenameBox(KviDccBroker * br,KviDccDescriptor * dcc,const QString &text,bool bDisableResume);
	~KviDccRenameBox();
protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void showEvent(QShowEvent *e);
private slots:
	void renameClicked();
	void overwriteClicked();
	void resumeClicked();
	void cancelClicked();
signals:
	void overwriteSelected(KviDccBox *,KviDccDescriptor *);
	void renameSelected(KviDccBox *,KviDccDescriptor *);
	void cancelSelected(KviDccBox *,KviDccDescriptor *);
};

#endif
