#ifndef _MANAGEMENTDIALOG_H_
#define _MANAGEMENTDIALOG_H_
//=============================================================================
//
//   File : ThemeManagementDialog.h
//   Creation date : Sat 30 Dec 2006 14:54:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2006-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviOptionsWidget.h"
#include "KviPointerList.h"
#include "KviTheme.h"
#include "KviTalWizard.h"
#include "KviTalListWidget.h"

#include <QWidget>
#include <QComboBox>
#include <QCursor>
#include <QListWidget>
#include <QItemDelegate>
#include <QToolButton>
#include <QMenu>
#include "kvi_settings.h"

#if defined(COMPILE_WEBKIT_SUPPORT) || defined(Q_MOC_RUN)
#include "WebThemeInterfaceDialog.h"
#endif

class QLineEdit;
class QPushButton;
class QLabel;
class KviDynamicToolTip;

class ThemeListWidgetItem : public KviTalListWidgetItem
{
public:
	ThemeListWidgetItem(KviTalListWidget * pBox, KviThemeInfo * pInfo);
	~ThemeListWidgetItem();

public:
	KviThemeInfo * m_pThemeInfo;

public:
	KviThemeInfo * themeInfo() { return m_pThemeInfo; }
};

class ThemeManagementDialog : public QWidget
{
	Q_OBJECT
public:
	ThemeManagementDialog(QWidget * parent);
	~ThemeManagementDialog();

protected:
	static ThemeManagementDialog * m_pInstance;
	KviTalIconAndRichTextItemDelegate * m_pItemDelegate;
	KviTalListWidget * m_pListWidget;
	QLabel * m_pCurrentInstalledThemeLabel;
	QMenu * m_pContextPopup;
	QToolButton * m_pDeleteThemeButton;
	QToolButton * m_pPackThemeButton;
#if defined(COMPILE_WEBKIT_SUPPORT) || defined(Q_MOC_RUN)
	WebThemeInterfaceDialog * m_pWebThemeInterfaceDialog;
#endif
public:
	static ThemeManagementDialog * instance() { return m_pInstance; }
	static void display(bool bTopLevel);
	static void cleanup();

protected:
	void fillThemeBox(bool bBuiltin);
	void closeEvent(QCloseEvent * e) override;
protected slots:
	void saveCurrentTheme();
	void getMoreThemes();
	void installFromFile();
	void fillThemeBox();
	void deleteTheme();
	void closeClicked();
	void packTheme();
	void applyTheme(QListWidgetItem *);
	void applyCurrentTheme();
	void enableDisableButtons();
	void contextMenuRequested(const QPoint & pos);
	//void tipRequest(KviDynamicToolTip *pTip,const QPoint &pnt);
	void tipRequest(QListWidgetItem * pItem, const QPoint & pnt);
	void webThemeInterfaceDialogDestroyed();
};

#endif //!_MANAGEMENTDIALOG_H_
