#ifndef _H_URLDLG
#define _H_URLDLG
//=============================================================================
//
//   File : libkviurl.h
//   Creation date : 1999 Andrea Parella
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 1999-2008 Andrea Parrella (yap@kvirc.net)
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

typedef struct _KviUrl
{
	QString url;
	QString window;
	int count;
	QString timestamp;
} KviUrl;

class UrlDialogTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	UrlDialogTreeWidget(QWidget*);
	~UrlDialogTreeWidget(){};
protected:
	void mousePressEvent (QMouseEvent *e);
	void paintEvent(QPaintEvent * event);
signals:
	void rightButtonPressed(QTreeWidgetItem *,QPoint);
	void contextMenuRequested(QPoint);
};

class UrlDialog : public KviWindow
{
	Q_OBJECT
public:
	UrlDialog(KviPointerList<KviUrl> *g_pList);
	~UrlDialog();
private:
	KviTalMenuBar *m_pMenuBar;
	KviTalPopupMenu *m_pListPopup;	// dynamic popup menu
	QString m_szUrl;		// used to pass urls to sayToWin slot
protected:
	QPixmap *myIconPtr();
	void resizeEvent(QResizeEvent *);
public:
	UrlDialogTreeWidget *m_pUrlList;
	void addUrl(QString url, QString window, QString count, QString timestamp);
//	void saveProperties();
protected slots:
	void config();
	void help();
	void saveList();
	void loadList();
	void clear();
	void close_slot();
	void remove();
	void findtext();
	void dblclk_url(QTreeWidgetItem *item, int);
	void popup(QTreeWidgetItem *item, const QPoint &p);
	void contextMenu(const QPoint &p);
	void sayToWin(QAction * act);
};

class BanFrame : public QFrame
{
	Q_OBJECT
public:
	BanFrame(QWidget *parent=0, const char *name=0, bool banEnable = false);
	~BanFrame();
	void saveBans(KviConfigurationFile *cfg);
private:
	QCheckBox     *m_pEnable;
	QListWidget   *m_pBanList;
	QPushButton   *m_pAddBtn;
	QPushButton   *m_pRemoveBtn;
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
	QCheckBox *cb[cbnum];
	BanFrame *m_pBanFrame;
	void closeEvent(QCloseEvent *);
protected slots:
	void acceptbtn();
	void discardbtn();
};


class KviUrlAction : public KviKvsAction
{
	Q_OBJECT
public:
	KviUrlAction(QObject * pParent);
	~KviUrlAction();
};

#endif
