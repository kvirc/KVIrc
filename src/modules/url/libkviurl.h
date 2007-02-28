#ifndef _H_URLDLG
#define _H_URLDLG

//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 1999-2000 Andrea Parrella (anandrea@iname.com)
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


#include <qdialog.h>
#include <qlayout.h>
#include "kvi_tal_listview.h"
#include <qstring.h>
#include <qpushbutton.h>
#include <qfile.h>
#ifdef COMPILE_USE_QT4
#include <Q3TextStream>
#else
#include <qtextstream.h>
#endif
#include <qcheckbox.h>
#include "kvi_tal_listbox.h"
#include <qmenubar.h>
#include <qinputdialog.h>
#include <qtoolbutton.h>
#include <qiconset.h>

#include "kvi_locale.h"
#include "kvi_config.h"
#include "kvi_irctoolbar.h"
#include "kvi_imagelib.h"
#include "kvi_window.h"
#include "kvi_tal_menubar.h"
#include "kvi_mextoolbar.h"
#include "kvi_styled_controls.h"

#include <qpixmap.h>

typedef struct _KviUrl
{
	QString url;
	QString window;
	int count;
	QString timestamp;
} KviUrl;


class UrlDialog : public KviWindow
{
	Q_OBJECT
public:
	UrlDialog(KviPtrList<KviUrl> *g_pList);
	~UrlDialog();
private:
	KviTalMenuBar *m_pMenuBar;
	KviTalPopupMenu *m_pListPopup;	// dynamic popup menu
	KviStr m_szUrl;			// used to pass urls to sayToWin slot
protected:
	QPixmap *myIconPtr();
	void resizeEvent(QResizeEvent *);
public:
	KviTalListView *m_pUrlList;
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
	void dblclk_url(KviTalListViewItem *item);
	void popup(KviTalListViewItem *item, const QPoint &p, int col);
	void sayToWin(int);
};

class BanFrame : public QFrame
{
	Q_OBJECT
public:
	BanFrame(QWidget *parent=0, const char *name=0, bool banEnable = false);
	~BanFrame();
	void saveBans();
private:
	KviStyledCheckBox *m_pEnable;
	KviTalListBox *m_pBanList;
	QPushButton *m_pAddBtn;
	QPushButton *m_pRemoveBtn;
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
	KviStyledCheckBox *cb[cbnum];
	BanFrame *m_pBanFrame;
	void closeEvent(QCloseEvent *);
protected slots:
	void acceptbtn();
	void discardbtn();
};

#include "kvi_kvs_action.h"

class KviUrlAction : public KviKvsAction
{
	Q_OBJECT
public:
	KviUrlAction(QObject * pParent);
	~KviUrlAction();
protected:
	QPixmap * m_pBigIcon;
	QPixmap * m_pSmallIcon;
public:
	virtual QPixmap * bigIcon();
	virtual QPixmap * smallIcon();
};

#endif
