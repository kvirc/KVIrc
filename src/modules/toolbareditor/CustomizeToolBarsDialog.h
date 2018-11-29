#ifndef _TOOLBAREDITOR_H_
#define _TOOLBAREDITOR_H_
//=============================================================================
//
//   File : CustomizeToolBarsDialog.h
//   Creation date : Wed 01 Dec 2004 14:42:20 by Szymon Stefanek
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

#include <QWidget>
#include <QLabel>
#include <QWidget>
#include <QDialog>
#include <QColor>

class QTimer;
class QLineEdit;
class QPushButton;
class QPushButton;
class KviActionDrawer;

class CustomizeToolBarsDialog : public QWidget
{
	Q_OBJECT
protected:
	CustomizeToolBarsDialog(QWidget * p);

public:
	~CustomizeToolBarsDialog();

protected:
	KviActionDrawer * m_pDrawer;
	static CustomizeToolBarsDialog * m_pInstance;
	QPushButton * m_pDeleteToolBarButton;
	QPushButton * m_pRenameToolBarButton;
	QPushButton * m_pExportToolBarButton;
	QPushButton * m_pImportToolBarButton;

public:
	static CustomizeToolBarsDialog * instance() { return m_pInstance; };
	static void display(bool bTopLevel);
	static void cleanup();

protected:
	void showEvent(QShowEvent * e) override;
	void closeEvent(QCloseEvent * e) override;
protected slots:
	void closeClicked();
	void newToolBar();
	void deleteToolBar();
	void renameToolBar();
	void exportToolBar();
	void importToolBar();
	void currentToolBarChanged();
	virtual void reject();
};

class CustomToolBarPropertiesDialog : public QDialog
{
	Q_OBJECT
public:
	CustomToolBarPropertiesDialog(QWidget * p, const QString & szText, const QString & szId = QString(), const QString & szLabel = QString(), const QString & szIconId = QString());
	~CustomToolBarPropertiesDialog();

protected:
	QString m_szId;
	QString m_szOriginalId;
	QString m_szLabel;
	QString m_szIconId;
	QWidget * m_pAdvanced;
	QLineEdit * m_pIdEdit;
	QLineEdit * m_pLabelEdit;
	QLineEdit * m_pIconEdit;
	QPushButton * m_pIconButton;
	QPushButton * m_pAdvancedButton;

public:
	const QString & id() { return m_szId; };
	const QString & label() { return m_szLabel; };
	const QString & iconId() { return m_szIconId; };
protected slots:
	void okClicked();
	void advancedClicked();
	void iconButtonClicked();
	void labelTextChanged(const QString & s);

protected:
	void iconSelected(const QString & szIconId);
};

class TrashcanLabel : public QLabel
{
	Q_OBJECT
public:
	TrashcanLabel(QWidget * p);
	~TrashcanLabel();

protected:
	unsigned int m_uFlashCount;
	QTimer * m_pFlashTimer;
	QColor m_clrOriginal;

protected:
	void dragEnterEvent(QDragEnterEvent * e) override;
	void dropEvent(QDropEvent * e) override;
public slots:
	void flash();
protected slots:
	void heartbeat();
};

#endif //!_TOOLBAREDITOR_H_
