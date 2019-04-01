#ifndef _KVI_CUSTOMTOOLBARDESCRIPTOR_H_
#define _KVI_CUSTOMTOOLBARDESCRIPTOR_H_
//=============================================================================
//
//   File : KviCustomToolBarDescriptor.h
//   Creation date : Sun 05 Dec 2004 18:20:58 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
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
#include "KviPointerList.h"
#include "KviQString.h"

class KviConfigurationFile;
class KviCustomToolBar;
class KviKvsScript;

class KVIRC_API KviCustomToolBarDescriptor
{
	friend class KviCustomToolBar;
	friend class KviCustomToolBarManager;

protected:
	KviCustomToolBarDescriptor(QString szId, const QString & szLabelCode);

public:
	~KviCustomToolBarDescriptor();

protected:
	QString m_szId;
	QString m_szIconId;
	QString m_szParsedLabel;
	KviPointerList<QString> * m_pActions;
	KviCustomToolBar * m_pToolBar = nullptr;
	int m_iInternalId;
	bool m_bVisibleAtStartup = false;
	KviKvsScript * m_pLabelScript = nullptr;

public:
	const QString & iconId() const { return m_szIconId; }
	const QString & label();
	const QString & labelCode() const;
	const QString & id() const { return m_szId; }
	int internalId() const { return m_iInternalId; } // useful only for KviMainWindow
	KviCustomToolBar * toolBar() const { return m_pToolBar; }
	KviCustomToolBar * createToolBar();
	void setIconId(const QString & szIconId) { m_szIconId = szIconId; }
	bool addAction(const QString & szAction);
	bool removeAction(const QString & szAction);
	bool removeAction(unsigned int iAction);
	void clear();
	void rename(const QString & szNewName);
	KviPointerList<QString> * actions() const { return m_pActions; }
	void updateToolBar();

protected:
	void createLabelScript(const QString & szNewLabelCode);
	void fillToolBar();
	bool load(KviConfigurationFile * cfg);
	void save(KviConfigurationFile * cfg);
	void registerToolBar(KviCustomToolBar * t);
	void unregisterToolBar(KviCustomToolBar * t);
};

#endif //_KVI_CUSTOMTOOLBARDESCRIPTOR_H_
