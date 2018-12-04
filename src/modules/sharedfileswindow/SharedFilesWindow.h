#ifndef _SHAREDFILESWINDOW_H_
#define _SHAREDFILESWINDOW_H_
//=============================================================================
//
//   File : SharedFilesWindow.h
//   Creation date : Mon Apr 21 2003 23:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviConsoleWindow.h"
#include "KviCString.h"
#include "KviIrcServerParser.h"
#include "KviModuleExtension.h"
#include "KviSharedFilesManager.h"
#include "KviThemedTreeWidget.h"
#include "KviWindow.h"

#include <QDialog>

class QCheckBox;
class QDateTimeEdit;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class SharedFilesTreeWidgetItem : public QTreeWidgetItem
{
public:
	SharedFilesTreeWidgetItem(QTreeWidget * lv, KviSharedFile * f);
	virtual ~SharedFilesTreeWidgetItem();

protected:
	KviSharedFile * m_pSharedFilePointer; // THIS IS READ ONLY!
public:
	KviSharedFile * readOnlySharedFilePointer() const { return m_pSharedFilePointer; }
};

class SharedFileEditDialog : public QDialog
{
	Q_OBJECT
public:
	SharedFileEditDialog(QWidget * par, KviSharedFile * f = nullptr);
	virtual ~SharedFileEditDialog();

	QDateTimeEdit * m_pExpireDateTimeEdit;
	QLineEdit * m_pFilePathEdit;
	QLineEdit * m_pUserMaskEdit;
	QPushButton * m_pBrowseButton;
	QLineEdit * m_pShareNameEdit;
	QCheckBox * m_pExpireCheckBox;

public:
	KviSharedFile * getResult() const;
protected slots:
	void okClicked();
	void browse();
};

class SharedFilesWindow : public KviWindow
{
	friend class KviSharedFilesItem;
	Q_OBJECT
public:
	SharedFilesWindow();
	virtual ~SharedFilesWindow();

protected:
	KviThemedTreeWidget * m_pTreeWidget;

	QPushButton * m_pRemoveButton;
	QPushButton * m_pEditButton;
	QPushButton * m_pAddButton;

public: // Methods
	virtual void die();

protected:
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void resizeEvent(QResizeEvent * e) override;
	void getBaseLogFileName(QString & buffer) override;

public:
	QSize sizeHint() const override;
protected slots:
	void fillFileView();
	void sharedFileAdded(KviSharedFile * f);
	void sharedFileRemoved(KviSharedFile * f);
	void enableButtons();
	void removeClicked();
	void addClicked();
	void editClicked();
};

#endif //_SHAREDFILESWINDOW_H_
