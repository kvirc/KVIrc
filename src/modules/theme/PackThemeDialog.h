#ifndef _PACKTHEMEDIALOG_H_
#define _PACKTHEMEDIALOG_H_
//=============================================================================
//
//   File : PackThemeDialog.h
//   Creation date : Wed 03 Jan 2007 01:11:44 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek <pragma at kvirc dot net>
//   Copyright (C) 2010 Elvio Basello <hell at hellvis69 dot netsons dot org>
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

#include <QWizard>
#include <QWizardPage>

class QLineEdit;
class QLabel;
class QTextEdit;
class KviFileSelector;

class PackThemeDataWidget;
class PackThemeInfoWidget;
class PackThemeImageWidget;
class PackThemeSaveWidget;

class PackThemeDialog : public QWizard
{
	Q_OBJECT
public:
	PackThemeDialog(QWidget * pParent, KviPointerList<KviThemeInfo> * pThemeInfoList);
	~PackThemeDialog();

protected:
	PackThemeDataWidget * m_pPackThemeDataWidget;
	PackThemeInfoWidget * m_pPackThemeInfoWidget;
	PackThemeImageWidget * m_pPackThemeImageWidget;
	PackThemeSaveWidget * m_pPackThemeSaveWidget;
	KviPointerList<KviThemeInfo> * m_pThemeInfoList;

	QString m_szAuthor;
	QString m_szName;
	QString m_szVersion;
	QString m_szDescription;
	QString m_szImagePath;
	QString m_szPackagePath;
	QString m_szSavePath;

	//QWidget                      * m_pImageSelectionPage;
protected:
	void accept() override;
	bool packTheme();
};

class PackThemeDataWidget : public QWizardPage
{
	Q_OBJECT
public:
	PackThemeDataWidget(PackThemeDialog * pParent);
	~PackThemeDataWidget();

public:
	void parseThemes(KviPointerList<KviThemeInfo> * pThemeInfoList);
};

class PackThemeInfoWidget : public QWizardPage
{
	Q_OBJECT
public:
	PackThemeInfoWidget(PackThemeDialog * pParent);
	~PackThemeInfoWidget();

public:
	QLineEdit * m_pPackageNameEdit;
	QTextEdit * m_pPackageDescriptionEdit;
	QLineEdit * m_pPackageVersionEdit;
	QLineEdit * m_pPackageAuthorEdit;

protected:
	void initializePage() override;
};

class PackThemeImageWidget : public QWizardPage
{
	Q_OBJECT
public:
	PackThemeImageWidget(PackThemeDialog * pParent);
	~PackThemeImageWidget();

protected:
	KviFileSelector * m_pImageSelector;
	QLabel * m_pImageLabel;
	QString m_szImagePath;
protected slots:
	void imageSelectionChanged(const QString & szImagePath);
};

class PackThemeSaveWidget : public QWizardPage
{
	Q_OBJECT
public:
	PackThemeSaveWidget(PackThemeDialog * pParent);
	~PackThemeSaveWidget();

protected:
	KviFileSelector * m_pSavePathSelector;
	QString m_szPackagePath;

protected:
	void initializePage() override;
};

#endif //!_PACKTHEMEDIALOG_H_
