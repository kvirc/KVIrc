#ifndef _SHAREDFILESWINDOW_H_
#define _SHAREDFILESWINDOW_H_
//=============================================================================
//
//   File : SharedFilesWindow.h
//   Creation date : Mon Apr 21 2003 23:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the linkss of the GNU General Public License
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

#include "KviWindow.h"
#include "KviCString.h"
#include "KviIrcServerParser.h"
#include "KviConsoleWindow.h"
#include "KviModuleExtension.h"
#include "KviSharedFilesManager.h"
#include "KviThemedTreeWidget.h"

#include <QDialog>
#include <QCheckBox>

class QGridLayout;
class QDateTimeEdit;
class QLabel;
class QLineEdit;
class QPushButton;

class SharedFilesTreeWidgetItem : public QTreeWidgetItem
{
public:
	SharedFilesTreeWidgetItem(QTreeWidget * lv,KviSharedFile * f);
	~SharedFilesTreeWidgetItem();
protected:
	KviSharedFile * m_pSharedFilePointer; // THIS IS READ ONLY!
public:
	KviSharedFile * readOnlySharedFilePointer(){ return m_pSharedFilePointer; };
};


class SharedFileEditDialog : public QDialog
{
	Q_OBJECT
public:
	SharedFileEditDialog(QWidget* par,KviSharedFile * f = 0);
	~SharedFileEditDialog();

	QDateTimeEdit * m_pExpireDateTimeEdit;
	QLineEdit     * m_pFilePathEdit;
	QLineEdit     * m_pUserMaskEdit;
	QPushButton   * m_pBrowseButton;
	QLineEdit     * m_pShareNameEdit;
	QCheckBox     * m_pExpireCheckBox;
public:
	KviSharedFile * getResult();
protected slots:
	void okClicked();
	void browse();
};


class SharedFilesWindow : public KviWindow, public KviModuleExtension
{
	friend class KviSharedFilesItem;
	Q_OBJECT
public:
	SharedFilesWindow(KviModuleExtensionDescriptor * d,KviMainWindow * lpFrm);
	~SharedFilesWindow();
protected:
	QSplitter           * m_pVertSplitter;

	KviThemedTreeWidget * m_pTreeWidget;

	QPushButton         * m_pRemoveButton;
	QPushButton         * m_pEditButton;
	QPushButton         * m_pAddButton;
public: // Methods
	virtual void die();
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void getBaseLogFileName(QString &buffer);
public:
	virtual QSize sizeHint() const;
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
