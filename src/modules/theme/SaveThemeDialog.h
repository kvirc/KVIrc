#ifndef _SAVETHEMEDIALOG_H_
#define _SAVETHEMEDIALOG_H_
//=============================================================================
//
//   File : SaveThemeDialog.h
//   Creation date : Wed 03 Jan 2007 03:01:34 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek <pragma at kvirc dot net>
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
#include "KviTheme.h"
#include "KviTalWizard.h"

#include <QDialog>

class QLineEdit;
class QPushButton;
class QLabel;
class QTextEdit;
class QCheckBox;
class KviFileSelector;

class SaveThemeDialog : public KviTalWizard
{
	Q_OBJECT
public:
	SaveThemeDialog(QWidget * pParent);
	virtual ~SaveThemeDialog();

protected:
	QString m_szScreenshotPath;
	KviFileSelector * m_pImageSelector;
	QLabel * m_pImageLabel;
	QLineEdit * m_pThemeNameEdit;
	QTextEdit * m_pThemeDescriptionEdit;
	QLineEdit * m_pThemeVersionEdit;
	QLineEdit * m_pAuthorNameEdit;
	QWidget * m_pImageSelectionPage;
	QCheckBox * m_pSaveIconsCheckBox;

protected:
	void accept() override;
	bool saveTheme();
protected slots:
	void makeScreenshot();
	void imageSelectionChanged(const QString & szImagePath);
};

#endif //!_SAVETHEMEDIALOG_H_
