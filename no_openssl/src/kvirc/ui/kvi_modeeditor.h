#ifndef _KVI_MODEEDITOR_H_
#define _KVI_MODEEDITOR_H_
//=============================================================================
//
//   File : kvi_modeeditor.h
//   Creation date : Sat Apr 14 2001 13:50:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_string.h"
#include "kvi_pointerlist.h"
#include "kvi_toolwindows_container.h"

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>

class KviConsole;

//////////////////////////////////////////////////////////////////////
// class KviModeEditor
//
//////////////////////////////////////////////////////////////////////

class KVIRC_API KviModeEditor : public KviWindowToolWidget
{
	Q_OBJECT
public:
	KviModeEditor(QWidget * par,KviWindowToolPageButton* button,const char * nam,KviConsole * c,const QString &mode,const QString &key,const QString &limit);
	~KviModeEditor();
protected: // fields
	QString                     m_szMode;
	QString                     m_szKey;
	QString                     m_szLimit;
	KviPointerList<QCheckBox> * m_pCheckBoxes;
	QCheckBox                 * m_pLimitBox;
	QLineEdit                 * m_pLimitEdit;
	QCheckBox                 * m_pKeyBox;
	QLineEdit                 * m_pKeyEdit;
signals:
	void setMode(const char *);
	void done();
protected slots:
	void limitBoxToggled(bool bChecked);
	void keyBoxToggled(bool bChecked);
	void commit();
};

#endif //_KVI_MODEEDITOR_H_
