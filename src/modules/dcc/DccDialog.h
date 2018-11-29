#ifndef _DIALOGS_H_
#define _DIALOGS_H_
//=============================================================================
//
//   File : DccDialog.h
//   Creation date : Tue Sep 19 09 2000 15:17:22 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "DccBroker.h"

#include <QWidget>

class DccDialog
{
protected:
	DccDescriptor * m_pDescriptor;
	DccBroker * m_pBroker;

public:
	DccDialog(DccBroker * br, DccDescriptor * dcc);
	virtual ~DccDialog();

public:
	virtual void forgetDescriptor();
};

class DccAcceptDialog : public QWidget, public DccDialog
{
	Q_OBJECT
public:
	DccAcceptDialog(DccBroker * br, DccDescriptor * dcc, const QString & text, const QString & capt);
	~DccAcceptDialog();

protected:
	void closeEvent(QCloseEvent * e) override;
	void showEvent(QShowEvent * e) override;
private slots:
	void acceptClicked();
	void rejectClicked();
signals:
	void accepted(DccDialog *, DccDescriptor *);
	void rejected(DccDialog *, DccDescriptor *);
};

class DccRenameDialog : public QWidget, public DccDialog
{
	Q_OBJECT
public:
	DccRenameDialog(DccBroker * br, DccDescriptor * dcc, const QString & text, bool bDisableResume);
	~DccRenameDialog();

protected:
	void closeEvent(QCloseEvent * e) override;
	void showEvent(QShowEvent * e) override;
private slots:
	void renameClicked();
	void overwriteClicked();
	void resumeClicked();
	void cancelClicked();
signals:
	void overwriteSelected(DccDialog *, DccDescriptor *);
	void renameSelected(DccDialog *, DccDescriptor *);
	void cancelSelected(DccDialog *, DccDescriptor *);
};

#endif
