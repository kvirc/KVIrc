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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_pointerlist.h"
#include "kvi_theme.h"

#include <qdialog.h>
#include <kvi_tal_wizard.h>


class QLineEdit;
class KviTalTextEdit;
class QPushButton;
class QCheckBox;
class QLabel;
class QMultiLineEdit;
class KviFileSelector;


class KviSaveThemeDialog : public KviTalWizard
{
	Q_OBJECT
public:
	KviSaveThemeDialog(QWidget * pParent);
	virtual ~KviSaveThemeDialog();
protected:
	QString m_szScreenshotPath;
	KviFileSelector * m_pImageSelector;
	QLabel * m_pImageLabel;
	QLineEdit * m_pThemeNameEdit;
	KviTalTextEdit * m_pThemeDescriptionEdit;
	QLineEdit * m_pThemeVersionEdit;
	QLineEdit * m_pAuthorNameEdit;
	QWidget * m_pImageSelectionPage;
	QPushButton * m_pOkButton;
protected:
	virtual void accept();
	bool saveTheme();
protected slots:
	void makeScreenshot();
	void imageSelectionChanged(const QString &szImagePath);
	//void themeNameChanged(const QString &txt);
};



#endif //!_SAVETHEMEDIALOG_H_
