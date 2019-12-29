#ifndef _H_URLDLG
#define _H_URLDLG
//=============================================================================
//
//   File : libkviurl.h
//   Creation date : 1999 Andrea Parella
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2008 Andrea Parrella (yap@kvirc.net)
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

#include "KviLocale.h"
#include "KviConfigurationFile.h"
#include "KviIrcToolBar.h"
#include "KviWindow.h"
#include "KviTalMenuBar.h"
#include "KviMexToolBar.h"
#include "KviKvsAction.h"
#include <QTreeWidget>

#include <QDialog>
#include <QLayout>
#include <QString>
#include <QPushButton>
#include <QFile>
#include <QMenuBar>
#include <QInputDialog>
#include <QIcon>
#include <QPixmap>
#include <QCheckBox>
#include <QListWidget>

#include <unordered_set>
#include <vector>

struct KviUrl
{
	QString url;
	QString window;
	int count;
	QString timestamp;
};

class UrlDialogTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	UrlDialogTreeWidget(QWidget *);
	~UrlDialogTreeWidget(){};

protected:
	void paintEvent(QPaintEvent * event) override;
};

class UrlDialog : public KviWindow
{
	Q_OBJECT
public:
	UrlDialog(std::unordered_set<KviUrl *> g_List);
	~UrlDialog();

private:
	KviTalMenuBar * m_pMenuBar;
	QMenu * m_pListPopup = nullptr; // dynamic popup menu
	QString m_szUrl;      // used to pass URLs to sayToWin slot
protected:
	QPixmap * myIconPtr() override;
	void resizeEvent(QResizeEvent *) override;

public:
	UrlDialogTreeWidget * m_pUrlList;
	void addUrl(QString url, QString window, QString count, QString timestamp);
	//	void saveProperties();
protected slots:
	void config();
	void help();
	void saveList();
	void loadList();
	void clear();
	void close_slot();
	void open();
	void remove();
	void findtext();
	void open_url(QString szUrl);
	void activate(QTreeWidgetItem * item, int);
	void contextMenu(const QPoint & p);
	void sayToWin(QAction * act);
};

class BanFrame : public QFrame
{
	Q_OBJECT
public:
	BanFrame(QWidget * parent = nullptr, const char * name = nullptr, bool banEnable = false);
	~BanFrame();
	void saveBans(KviConfigurationFile * cfg);

private:
	QCheckBox * m_pEnable;
	QListWidget * m_pBanList;
	QPushButton * m_pAddBtn;
	QPushButton * m_pRemoveBtn;
protected slots:
	void enableClicked();
	void addBan();
	void removeBan();
};

#define cbnum 2

class ConfigDialog : public QDialog
{
	Q_OBJECT
public:
	ConfigDialog();
	~ConfigDialog();

private:
	QCheckBox * cb[cbnum];
	BanFrame * m_pBanFrame;
	void closeEvent(QCloseEvent *) override;
protected slots:
	void acceptbtn();
	void discardbtn();
};

#endif
