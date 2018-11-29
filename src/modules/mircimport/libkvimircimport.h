#ifndef _LIBKVIMIRCIMPORT_H_
#define _LIBKVIMIRCIMPORT_H_
//=============================================================================
//
//   File : libkvimircimport.h
//   Creation date : Tue Aug 27 01:28:02 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviMexServerImport.h"
#include "KviHttpRequest.h"
#include "KviTalWizard.h"

#include <QString>

class KviRemoteMircServerImportWizard;
class QLabel;
class QLineEdit;

class KviMircServersIniImport : public KviMexServerImport
{
	Q_OBJECT
public:
	KviMircServersIniImport(KviModuleExtensionDescriptor * d);
	~KviMircServersIniImport();

public:
	int doImport(const QString & filename);
	void start() override;
	void die() override;
};

class KviRemoteMircServersIniImport : public KviMircServersIniImport
{
	Q_OBJECT
public:
	KviRemoteMircServersIniImport(KviModuleExtensionDescriptor * d);
	~KviRemoteMircServersIniImport();

protected:
	KviRemoteMircServerImportWizard * m_pWizard;

public:
	void start() override;
	void die() override;
};

class KviRemoteMircServerImportWizard : public KviTalWizard
{
	Q_OBJECT
public:
	KviRemoteMircServerImportWizard(KviRemoteMircServersIniImport * f);
	~KviRemoteMircServerImportWizard();

protected:
	QLineEdit * m_pUrlEdit;
	QLabel * m_pOutput;
	KviRemoteMircServersIniImport * m_pFilter;
	KviHttpRequest * m_pRequest;
	QString m_szTmpFileName;

protected:
	void closeEvent(QCloseEvent * e) override;
	void done(int r) override;
	void start();
protected slots:
	void getListMessage(const QString & message);
	void getListTerminated(bool bSuccess);
	void pageSelected(const QString & title);
};

#endif //_LIBKVIMIRCIMPORT_H_
