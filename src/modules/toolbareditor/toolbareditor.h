#ifndef _TOOLBAREDITOR_H_
#define _TOOLBAREDITOR_H_
//=============================================================================
//
//   File : toolbareditor.h
//   Created on Wed 01 Dec 2004 14:42:20 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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

#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QColor>

class QTimer;
class QLineEdit;
class QPushButton;
class QPushButton;
class KviActionDrawer;


class KviCustomizeToolBarsDialog : public QDialog
{
	Q_OBJECT
protected:
	KviCustomizeToolBarsDialog(QWidget * p);
public:
	~KviCustomizeToolBarsDialog();
protected:
	KviActionDrawer * m_pDrawer;
	static KviCustomizeToolBarsDialog * m_pInstance;
	QPushButton * m_pDeleteToolBarButton;
	QPushButton * m_pRenameToolBarButton;
	QPushButton * m_pExportToolBarButton;
public:
	static KviCustomizeToolBarsDialog * instance(){ return m_pInstance; };
	static void display();
	static void cleanup();
protected:
	virtual void showEvent(QShowEvent * e);
	virtual void closeEvent(QCloseEvent *e);
protected slots:
	void closeClicked();
	void newToolBar();
	void deleteToolBar();
	void renameToolBar();
	void exportToolBar();
	void currentToolBarChanged();
};


class KviCustomToolBarPropertiesDialog : public QDialog
{
	Q_OBJECT
public:
	KviCustomToolBarPropertiesDialog(QWidget * p,const QString &szText,const QString &szId = QString::null,const QString &szLabel = QString::null,const QString &szIconId = QString::null);
	~KviCustomToolBarPropertiesDialog();
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
	const QString &id(){ return m_szId; };
	const QString &label(){ return m_szLabel; };
	const QString &iconId(){ return m_szIconId; };
protected slots:
	void okClicked();
	void advancedClicked();
	void iconButtonClicked();
	void labelTextChanged(const QString &s);
protected:
	void iconSelected(const QString &szIconId);
};


class KviTrashcanLabel : public QLabel
{
	Q_OBJECT
public:
	KviTrashcanLabel(QWidget * p);
	virtual ~KviTrashcanLabel();
protected:
	unsigned int m_uFlashCount;
	QTimer * m_pFlashTimer;
	QColor   m_clrOriginal;
protected:
	virtual void dragEnterEvent(QDragEnterEvent * e);
	virtual void dropEvent(QDropEvent * e);
public slots:
	void flash();
protected slots:
	void heartbeat();
};

#endif //!_TOOLBAREDITOR_H_
