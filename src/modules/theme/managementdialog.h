#ifndef _MANAGEMENTDIALOG_H_
#define _MANAGEMENTDIALOG_H_
//=============================================================================
//
//   File : managementdialog.h
//   Created on Sat 30 Dec 2006 14:54:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2006 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_optionswidget.h"
#include "kvi_list.h"

#include <qdialog.h>
#include <qsimplerichtext.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qwizard.h>

class QLineEdit;
class QTextEdit;
class QPushButton;
class QLabel;
class QCheckBox;
class QMultiLineEdit;

typedef struct _KviThemeInfoTag KviThemeInfo;

class KviSaveThemeDialog : public QDialog
{
	Q_OBJECT
public:
	KviSaveThemeDialog(QWidget *par);
	~KviSaveThemeDialog();
protected:
	QLineEdit   * m_pAuthorEdit;
	QLineEdit   * m_pThemeNameEdit;
	QLineEdit   * m_pVersionEdit;
	QTextEdit   * m_pDescriptionEdit;
	QPushButton * m_pOkButton;
//	QComboBox   * m_pFormatCombo;
	QCheckBox   * m_pIncludeMsgcolors;

private:
	void saveThemeInNewFormat(KviThemeInfo& info);
	void saveThemeInOldFormat(KviThemeInfo& info);
	void saveThemeInXmlFormat(KviThemeInfo& info);
protected slots:
	void saveTheme();
	void themeNameChanged(const QString &txt);
};

class KviPackThemeDialog : public QWizard
{
	Q_OBJECT
public:
	KviPackThemeDialog(QWidget * pParent,KviPtrList<KviThemeInfo> * pThemeInfoList);
	~KviPackThemeDialog();
protected:
	QString m_szImagePath;
	KviFileSelector * m_pImageSelector;
	QString m_szPackagePath;
	KviFileSelector * m_pPathSelector;
	QLabel * m_pImageLabel;
	KviPtrList<KviThemeInfo> * m_pThemeInfoList;
	QLineEdit * m_pPackageNameEdit;
	QMultiLineEdit * m_pPackageDescriptionEdit;
	QLineEdit * m_pPackageVersionEdit;
	QLineEdit * m_pPackagerNameEdit;
	QWidget * m_pImageSelectionPage;
protected:
	virtual void accept();
protected slots:
	bool packTheme();
	void imageSelectionChanged(const QString &szImagePath);
};

/*
class KviUnpackThemeDialog : public QDialog
{
	Q_OBJECT
public:
	KviUnpackThemeDialog(QWidget * pParent);
	~KviUnpackThemeDialog();
};
*/

class KviThemeListBoxItem : public QListBoxText
{
public:
	KviThemeListBoxItem(QListBox * box,KviThemeInfo * inf);
	~KviThemeListBoxItem();
public:
	KviThemeInfo * m_pThemeInfo;
	QSimpleRichText * m_pText;
public:
	KviThemeInfo * themeInfo(){ return m_pThemeInfo; };
	virtual int height ( const QListBox * lb ) const ;
protected:
	virtual void paint ( QPainter * painter );
};


class KviStyledToolButton;

class KviThemeManagementDialog : public QDialog
{
	Q_OBJECT
public:
	KviThemeManagementDialog(QWidget * parent);
	~KviThemeManagementDialog();
protected:
	static KviThemeManagementDialog * m_pInstance;
	QListBox            * m_pListBox;
	QPopupMenu          * m_pContextPopup; 
	KviSaveThemeDialog  * m_pSaveThemeDialog;
	KviStyledToolButton * m_pDeleteThemeButton;
	KviStyledToolButton * m_pPackThemeButton;
public:
	static KviThemeManagementDialog * instance(){ return m_pInstance; };
	static void display();
	static void cleanup();
protected:
	void fillThemeBox(const QString &szDir);
	bool hasSelectedItems();
protected slots:
	void saveCurrentTheme();
	void getMoreThemes();
	void installFromXml();
	void fillThemeBox();
	void deleteTheme();
	void closeClicked();
	void packTheme();
	void applyTheme(QListBoxItem *);
	void applyCurrentTheme();
	void enableDisableButtons();
	void contextMenuRequested(QListBoxItem * item, const QPoint & pos);
};

#endif //!_MANAGEMENTDIALOG_H_
