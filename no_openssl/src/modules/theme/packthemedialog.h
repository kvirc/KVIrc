#ifndef _PACKTHEMEDIALOG_H_
#define _PACKTHEMEDIALOG_H_
//=============================================================================
//
//   File : packthemedialog.h
//   Creation date : Wed 03 Jan 2007 01:11:44 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2008 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_pointerlist.h"
#include "kvi_theme.h"
#include "kvi_tal_wizard.h"

#include <QDialog>

class QLineEdit;
class QLabel;
class QTextEdit;
class KviFileSelector;


class KviPackThemeDialog : public KviTalWizard
{
	Q_OBJECT
public:
	KviPackThemeDialog(QWidget * pParent,KviPointerList<KviThemeInfo> * pThemeInfoList);
	virtual ~KviPackThemeDialog();
protected:
	QString                        m_szImagePath;
	KviFileSelector              * m_pImageSelector;
	QString                        m_szPackagePath;
	KviFileSelector              * m_pPathSelector;
	QLabel                       * m_pImageLabel;
	KviPointerList<KviThemeInfo> * m_pThemeInfoList;
	QLineEdit                    * m_pPackageNameEdit;
	QTextEdit                    * m_pPackageDescriptionEdit;
	QLineEdit                    * m_pPackageVersionEdit;
	QLineEdit                    * m_pPackagerNameEdit;
	QWidget                      * m_pImageSelectionPage;
protected:
	virtual void accept();
	bool packTheme();
protected slots:
	void imageSelectionChanged(const QString &szImagePath);
};

#endif //!_PACKTHEMEDIALOG_H_
