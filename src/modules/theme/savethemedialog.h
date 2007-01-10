#ifndef _SAVETHEMEDIALOG_H_
#define _SAVETHEMEDIALOG_H_
//=============================================================================
//
//   File : savethemedialog.h
//   Created on Wed 03 Jan 2007 03:01:34 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007 Szymon Stefanek <pragma at kvirc dot net>
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_list.h"

#include <qdialog.h>

#include "kvi_theme.h"

class QLineEdit;
class QTextEdit;
class QPushButton;
class QCheckBox;
class QLabel;
class QMultiLineEdit;
class KviFileSelector;


class KviSaveThemeDialog : public QDialog
{
	Q_OBJECT
public:
	KviSaveThemeDialog(QWidget *par);
	virtual ~KviSaveThemeDialog();
protected:
	QLineEdit   * m_pAuthorEdit;
	QLineEdit   * m_pThemeNameEdit;
	QLineEdit   * m_pVersionEdit;
	QTextEdit   * m_pDescriptionEdit;
	QPushButton * m_pOkButton;
protected slots:
	void saveTheme();
	void themeNameChanged(const QString &txt);
};



#endif //!_SAVETHEMEDIALOG_H_
