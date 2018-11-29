#ifndef _LIBKVIAVATAR_H_
#define _LIBKVIAVATAR_H_
//=============================================================================
//
//   File : libkviavatar.h
//   Creation date : Thu Nov 12 02:32:59 2004 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviQString.h"

#include <QDialog>

class QLineEdit;
class KviIrcConnection;

class KviAsyncAvatarSelectionDialog : public QDialog
{
	Q_OBJECT
public:
	KviAsyncAvatarSelectionDialog(QWidget * par, const QString & szInitialPath, KviIrcConnection * c);
	~KviAsyncAvatarSelectionDialog();

protected:
	QLineEdit * m_pLineEdit;
	QString m_szAvatarName;
	KviIrcConnection * m_pConnection;

protected:
	void closeEvent(QCloseEvent * e) override;
	const QString & avatarName() { return m_szAvatarName; };
protected slots:
	void okClicked();
	void cancelClicked();
	void chooseFileClicked();
};

#endif //!_LIBKVIAVATAR_H_
