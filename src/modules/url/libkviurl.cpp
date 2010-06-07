//=============================================================================
//
//   File : libkviurl.cpp
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

#include "libkviurl.h"

#include "kvi_module.h"
#include "kvi_app.h"
#include "kvi_frame.h"
#include "kvi_menubar.h"
#include "kvi_internalcmd.h"
#include "kvi_iconmanager.h"
#include "kvi_action.h"
#include "kvi_actionmanager.h"
#include "kvi_windowlist.h"
#include "kvi_pointerlist.h"
#include "kvi_kvs_eventmanager.h"
#include "kvi_tal_popupmenu.h"
#include "kvi_window.h"
#include "kvi_options.h"
#include "kvi_out.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCursor>
#include <QDateTime>
#include <QHeaderView>
#include <QTextStream>
#include <QMouseEvent>
#include <QPainter>

static KviUrlAction * g_pUrlAction = 0;

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern KVIRC_API QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

typedef struct _UrlDlgList
{
	UrlDialog *dlg;
	int menu_id;
} UrlDlgList;

const char *g_pUrlListFilename = "/list.kviurl";
const char *g_pBanListFilename = "/list.kviban";

KviPointerList<KviUrl> *g_pList;
KviPointerList<UrlDlgList> *g_pUrlDlgList;
KviPointerList<QString> *g_pBanList;
ConfigDialog *g_pConfigDialog;

QString szConfigPath;

void saveUrlList();
void loadUrlList();
void saveBanList();
void loadBanList();
UrlDlgList * findFrame();
bool urllist();
void url_module_help();


#define KVI_URL_EXTENSION_NAME "URL module extenstion"

static KviModuleExtension * url_extension_alloc(KviModuleExtensionAllocStruct *)
{
	urllist();
	return 0;
}

KviUrlDialogTreeWidget::KviUrlDialogTreeWidget(QWidget * par)
: QTreeWidget(par)
{
}

void KviUrlDialogTreeWidget::mousePressEvent (QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
	{
		QTreeWidgetItem *i= itemAt(e->pos());
		if (i) emit rightButtonPressed(i,QCursor::pos());
		else emit contextMenuRequested(QCursor::pos());
	}
	QTreeWidget::mousePressEvent(e);
}

void KviUrlDialogTreeWidget::paintEvent(QPaintEvent * event)
{
	QPainter *p = new QPainter(viewport());
	QStyleOptionViewItem option = viewOptions();
	QRect rect = event->rect();

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p->save();
		p->setCompositionMode(QPainter::CompositionMode_Source);
		QColor col=KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p->fillRect(rect, col);
		p->restore();
	} else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = ((KviWindow*)parent())->mdiParent() ? viewport()->mapTo(g_pFrame, rect.topLeft() + g_pFrame->mdiManager()->scrollBarsOffset()) : viewport()->mapTo((KviWindow*)parent(), rect.topLeft());
		p->drawTiledPixmap(rect,*(g_pShadedChildGlobalDesktopBackground), pnt);
	} else {
#endif
		//FIXME this is not the treewindowlist
		p->fillRect(rect,KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground));
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	delete p;

	//call paint on all childrens
	QTreeWidget::paintEvent(event);
}

KviUrlAction::KviUrlAction(QObject * pParent)
: KviKvsAction(
		pParent,
		"url.list",
		"url.list",
		__tr2qs("Show URL List"),
		__tr2qs("Shows the URL list window"),
		KviActionManager::categoryGeneric(),
		"kvi_bigicon_www.png",
		QString("%1").arg(KVI_SMALLICON_URL)
	)
{
}

KviUrlAction::~KviUrlAction()
{
}

// ---------------------------- CLASS URLDIALOG ------------------------begin //

UrlDialog::UrlDialog(KviPointerList<KviUrl> *)
	:KviWindow(KVI_WINDOW_TYPE_TOOL,g_pFrame,"URL List")
{
	setAutoFillBackground(false);
	
	m_pUrlList = new KviUrlDialogTreeWidget(this);

	m_pMenuBar = new KviTalMenuBar(this,"url menu");
	//m_pUrlList = new KviListView(this,"list");
	KviConfig cfg(szConfigPath,KviConfig::Read);
/*
	KviTalPopupMenu *pop;

	pop = new KviTalPopupMenu(this);
	pop->insertItem(__tr2qs("&Configure"),this,SLOT(config()));
// 	pop->insertItem(__tr2qs("&Help"),this,SLOT(help()));
	pop->insertItem(__tr2qs("Clo&se"),this,SLOT(close_slot()));
	pop->setTitle(__tr2qs("&Module"));
	m_pMenuBar->addMenu(pop);

	pop = new KviTalPopupMenu(this);
	pop->insertItem(__tr2qs("&Load"),this,SLOT(loadList()));
	pop->insertItem(__tr2qs("&Save"),this,SLOT(saveList()));
	pop->insertItem(__tr2qs("&Clear"),this,SLOT(clear()));
	pop->setTitle(__tr2qs("&List"));
	m_pMenuBar->addMenu(pop);
*/
	m_pUrlList->header()->setSortIndicatorShown(true);
	m_pUrlList->setColumnCount(4);

	QStringList labels;
	labels << __tr2qs("URL") << __tr2qs("Window") << __tr2qs("Count") << __tr2qs("Timestamp");
	m_pUrlList->setHeaderLabels(labels);
/*
	cfg.setGroup("colsWidth");
	m_pUrlList->setColumnWidth(0,cfg.readIntEntry("Url",170));
	m_pUrlList->setColumnWidth(1,cfg.readIntEntry("Window",130));
	m_pUrlList->setColumnWidth(2,cfg.readIntEntry("Count",70));
	m_pUrlList->setColumnWidth(3,cfg.readIntEntry("Timestamp",70));
*/
	connect(m_pUrlList,SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),SLOT(dblclk_url(QTreeWidgetItem *, int)));
	connect(m_pUrlList,SIGNAL(rightButtonPressed(QTreeWidgetItem *, const QPoint &)),SLOT(popup(QTreeWidgetItem *, const QPoint &)));
	connect(m_pUrlList,SIGNAL(contextMenuRequested(const QPoint &)),SLOT(contextMenu(const QPoint &)));
//	setFocusHandlerNoChildren(m_pUrlList);
	m_pUrlList->setFocusPolicy(Qt::StrongFocus);
	m_pUrlList->setFocus();
}

void UrlDialog::config()
{
	if (!g_pConfigDialog) g_pConfigDialog = new ConfigDialog();
}

void UrlDialog::help()
{
//#warning "help"
//	m_pFrm->requestHelpOn("doc_plugin_url.kvihelp");
}

void UrlDialog::saveList()
{
	saveUrlList();
}

void UrlDialog::loadList()
{
	loadUrlList();
}

void UrlDialog::clear()
{
	g_pList->clear();
	for (UrlDlgList *tmpitem=g_pUrlDlgList->first();tmpitem;tmpitem=g_pUrlDlgList->next()) {
		if (tmpitem->dlg) tmpitem->dlg->m_pUrlList->clear();
	}
}

/*
void UrlDialog::saveProperties()
{

	KviWindowProperty p;
	p.rect = externalGeometry();
	p.isDocked = isAttached();
	p.splitWidth1 = 0;
	p.splitWidth2 = 0;
	p.timestamp = 0;
	p.imagesVisible = 0;
	KviWindow * w = m_pFrm->activeWindow();
	p.isMaximized = isAttached() && w ? w->isMaximized() : isMaximized();
	p.topSplitWidth1 = 0;
	p.topSplitWidth2 = 0;
	p.topSplitWidth3 = 0;
	g_pOptions->m_pWinPropertiesList->setProperty(caption(),&p);
}
*/

void UrlDialog::close_slot()
{
	close();
}

void UrlDialog::remove()
{
	if (!m_pUrlList->currentItem()) {
		QMessageBox::warning(0,__tr2qs("Warning - KVIrc"),__tr2qs("Select an URL."),QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}

	for(KviUrl *tmp=g_pList->first();tmp;tmp=g_pList->next())
	{
		if (tmp->url == m_pUrlList->currentItem()->text(0)) {
			g_pList->removeRef(tmp);
			delete m_pUrlList->currentItem();
			//m_pUrlList->takeTopLevelItem(m_pUrlList->currentItem());
			return;
		}
	}
}

void UrlDialog::findtext()
{
//#warning "find text"
/*
	if (!m_pUrlList->currentItem()) {
		kvirc_plugin_warning_box(__tr("Select an URL"));
		return;
	}
	for(KviUrl *tmp=g_pList->first();tmp;tmp=g_pList->next())
	{
		if (tmp->url == KviStr(m_pUrlList->currentItem()->text(0))) {
			g_pList->find(tmp);
			KviStr ft="findtext %";
			ft.replaceAll('%',tmp->url.ptr());
			KviWindow *wnd = m_pFrm->findWindow(tmp->window.ptr());
			if (wnd) {
				if (kvirc_plugin_execute_command(wnd,ft.ptr())) {
					if (wnd->mdiParent()) m_pFrm->m_pMdi->setTopChild(wnd->mdiParent(),true);
				}
			} else kvirc_plugin_warning_box(__tr("Window not found"));
		}

	}
*/
}

void UrlDialog::dblclk_url(QTreeWidgetItem *item, int)
{
	QString cmd="openurl ";
	cmd.append(item->text(0));
	KviKvsScript::run(cmd,this);
}

void UrlDialog::popup(QTreeWidgetItem *item, const QPoint &point)
{
	m_szUrl = item->text(0);
	KviTalPopupMenu p(0,"menu");
	p.insertItem(__tr2qs("&Remove"),this,SLOT(remove()));
// 	p.insertItem(__tr2qs("&Find Text"),this,SLOT(findtext()));
	p.insertSeparator();
	m_pListPopup = new KviTalPopupMenu(0,"list");

	for(KviWindow *w=g_pFrame->windowList()->first();w;w=g_pFrame->windowList()->next())
	{
		if ((w->type() == KVI_WINDOW_TYPE_CHANNEL) ||
			(w->type() == KVI_WINDOW_TYPE_QUERY) ||
			 (w->type() == KVI_WINDOW_TYPE_DCCCHAT))
		{
			m_pListPopup->addAction(w->plainTextCaption());
		}
	}
	p.insertItem(__tr2qs("&Say to Window"),m_pListPopup);
	connect(m_pListPopup,SIGNAL(triggered(QAction *)), this, SLOT(sayToWin(QAction *)));
	p.exec(point);
}

void UrlDialog::contextMenu(const QPoint &point)
{
	KviTalPopupMenu p(0,"contextmenu");
	p.insertItem(__tr2qs("Configure"),this,SLOT(config()));
	p.exec(point);
}

void UrlDialog::sayToWin(QAction * act)
{
	KviWindow *wnd = g_pApp->findWindowByCaption(act->text());
	QString say=QString("PRIVMSG %1 %2").arg(wnd->windowName(), m_szUrl);
	if(wnd)
	{
		KviKvsScript::run(say,wnd);
		g_pFrame->setActiveWindow(wnd);
	} else QMessageBox::warning(0,__tr2qs("Warning - KVIrc"),__tr2qs("Window not found."),QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
}

QPixmap *UrlDialog::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_URL);
}

void UrlDialog::addUrl(QString url, QString window, QString count, QString timestamp)
{
	QTreeWidgetItem *UrlItem = new QTreeWidgetItem(m_pUrlList);

	UrlItem->setText(0, url);
	UrlItem->setText(1, window);
	UrlItem->setText(2, count);
	UrlItem->setText(3, timestamp);

	UrlItem->setForeground(0, KVI_OPTION_MIRCCOLOR(KVI_OPTION_MSGTYPE(KVI_OUT_URL).fore()));
	UrlItem->setForeground(1, KVI_OPTION_MIRCCOLOR(KVI_OPTION_MSGTYPE(KVI_OUT_NONE).fore()));
	UrlItem->setForeground(2, KVI_OPTION_MIRCCOLOR(KVI_OPTION_MSGTYPE(KVI_OUT_NONE).fore()));
	UrlItem->setForeground(3, KVI_OPTION_MIRCCOLOR(KVI_OPTION_MSGTYPE(KVI_OUT_NONE).fore()));

	m_pUrlList->resizeColumnToContents(0);
	m_pUrlList->resizeColumnToContents(3);
	m_pUrlList->resizeColumnToContents(2);
	m_pUrlList->resizeColumnToContents(1);
}

void UrlDialog::resizeEvent(QResizeEvent *)
{
	int hght = m_pMenuBar->heightForWidth(width());
	m_pMenuBar->setGeometry(0,0,width(),hght);
	m_pUrlList->setGeometry(0,hght,width(),height() - hght);
}

UrlDialog::~UrlDialog()
{
	/*
	KviConfig cfg(szConfigPath,KviConfig::Write);
	cfg.setGroup("ConfigDialog");
	if (cfg.readBoolEntry("SaveColumnWidthOnClose",false)) {
		cfg.setGroup("ColsWidth");
		cfg.writeEntry("Url",m_pUrlList->columnWidth(0));
		cfg.writeEntry("Window",m_pUrlList->columnWidth(1));
		cfg.writeEntry("Count",m_pUrlList->columnWidth(2));
		cfg.writeEntry("Timestamp",m_pUrlList->columnWidth(3));
	}
*/
	delete m_pUrlList;
/*	if (m_pListPopup) delete m_pListPopup;
	m_pListPopup = 0;
	if (m_pMenuBar) delete m_pMenuBar;
	m_pMenuBar = 0;*/
	UrlDlgList *tmpitem = findFrame();
	tmpitem->dlg = 0;
}

// ----------------------------- CLASS URLDIALOG -------------------------end //

// --------------------------- CLASS CONFIGDIALOG ----------------------begin //

ConfigDialog::ConfigDialog()
:QDialog()
{
	setWindowTitle(__tr2qs("URL Module Configuration"));

	QGridLayout *g = new QGridLayout(this);

	KviConfig *cfg = new KviConfig(szConfigPath,KviConfig::Read);
	cfg->setGroup("ConfigDialog");

	cb[0] = new QCheckBox(__tr2qs("Save URL list on module unload"),this);
	cb[0]->setChecked(cfg->readBoolEntry("SaveUrlListOnUnload",false));
	g->addWidget(cb[0],0,0,1,2);

	cb[1] = new QCheckBox(__tr2qs("Save columns width on URL list close"),this);
	cb[1]->setChecked(cfg->readBoolEntry("SaveColumnWidthOnClose",false));
	g->addWidget(cb[1],1,0,1,2);

	bool tmp = cfg->readBoolEntry("BanEnabled",false);
	delete cfg;

	m_pBanFrame = new BanFrame(this,"banlist",tmp);
	g->addWidget(m_pBanFrame,3,0,1,2);

	QPushButton *b;
	// configure buttons
        b = new QPushButton(__tr2qs("&Cancel"),this);
	b->setObjectName("discard");

	connect(b,SIGNAL(clicked()),this,SLOT(discardbtn()));
	g->addWidget(b,4,0);

	b = new QPushButton(__tr2qs("&OK"),this);
	b->setObjectName("accept");

	connect(b,SIGNAL(clicked()),this,SLOT(acceptbtn()));
	g->addWidget(b,4,1);

	show();
}

void ConfigDialog::discardbtn()
{
	delete this;
}

void ConfigDialog::acceptbtn()
{
	KviConfig *cfg = new KviConfig(szConfigPath,KviConfig::Write);
	cfg->setGroup("ConfigDialog");

	if (m_pBanFrame) m_pBanFrame->saveBans(cfg);

	cfg->writeEntry("SaveUrlListOnUnload",cb[0]->isChecked());
	cfg->writeEntry("SaveColumnWidthOnClose",cb[1]->isChecked());
	delete cfg;

	delete this;
}

void ConfigDialog::closeEvent(QCloseEvent *)
{
	delete this;
}

ConfigDialog::~ConfigDialog()
{
	for(int i=0;i<cbnum;i++) delete cb[i];
	g_pConfigDialog = 0;
}

// --------------------------- CLASS CONFIGDIALOG ------------------------end //

// ---------------------------- CLASS BANFRAME ------------------------begin //

BanFrame::BanFrame(QWidget *parent, const char *name, bool banEnabled)
:QFrame(parent)
{
	setFrameStyle(QFrame::Panel | QFrame::Raised);
	setObjectName(name);

	QGridLayout *g = new QGridLayout(this);

	m_pEnable = new QCheckBox(__tr2qs("Enable URL ban list"),this);
	connect(m_pEnable,SIGNAL(clicked()),this,SLOT(enableClicked()));
	m_pEnable->setChecked(banEnabled);
	g->addWidget(m_pEnable,0,0,1,2);

	m_pBanList = new QListWidget(this);
	m_pBanList->setMinimumHeight(100);
	loadBanList();
	for(QString *tmp=g_pBanList->first();tmp;tmp=g_pBanList->next()) {
		m_pBanList->addItem(*tmp); // load ban list into listbox
	}

	m_pBanList->setEnabled(m_pEnable->isChecked());
	g->addWidget(m_pBanList,1,0,1,2);

	m_pAddBtn = new QPushButton(__tr2qs("&Add Ban"),this);
	m_pAddBtn->setObjectName("add");
	connect(m_pAddBtn,SIGNAL(clicked()),this,SLOT(addBan()));
	m_pAddBtn->setEnabled(m_pEnable->isChecked());
	g->addWidget(m_pAddBtn,2,0);

	m_pRemoveBtn = new QPushButton(__tr2qs("&Remove Selected"),this);
	m_pRemoveBtn->setObjectName("remove");
	connect(m_pRemoveBtn,SIGNAL(clicked()),this,SLOT(removeBan()));
	m_pRemoveBtn->setEnabled(m_pEnable->isChecked());
	g->addWidget(m_pRemoveBtn,2,1);
}

void BanFrame::enableClicked()
{
	m_pBanList->setEnabled(m_pEnable->isChecked());
	m_pAddBtn->setEnabled(m_pEnable->isChecked());
	m_pRemoveBtn->setEnabled(m_pEnable->isChecked());
}

void BanFrame::addBan()
{
	bool ok = false;
	QString *pText = new QString(QInputDialog::getText(this, __tr2qs("URL Ban List"),__tr2qs("Add"),QLineEdit::Normal,QString(),&ok));
	if (ok && !pText->isEmpty()) {
		g_pBanList->append(pText);
		m_pBanList->addItem(*pText);
	}
}

void BanFrame::removeBan()
{
	if(!m_pBanList->currentItem())
		return;

	if (!m_pBanList->currentItem()->isSelected()) {
		QMessageBox::warning(0,__tr2qs("Warning - KVIrc"),__tr2qs("Select a ban."),QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	QString item(m_pBanList->currentItem()->text());
	for(QString *tmp=g_pBanList->first();tmp;tmp=g_pBanList->next())
	{
		if (tmp->compare(item)==0)
		{
			g_pBanList->removeCurrent();
			delete m_pBanList->currentItem();
			return;
		}
	}

	m_pBanList->removeItemWidget(m_pBanList->currentItem());

}

void BanFrame::saveBans(KviConfig *cfg)
{
	cfg->writeEntry("BanEnabled",m_pEnable->isChecked());
	if (m_pEnable->isChecked()) saveBanList();
}

BanFrame::~BanFrame()
{

}

// ---------------------------- CLASS URLTOOLBAR -------------------------end //

void saveUrlList()
{
	QString urllist;
	g_pApp->getLocalKvircDirectory(urllist,KviApp::ConfigPlugins);
	urllist += g_pUrlListFilename;
	QFile file;
	file.setFileName(urllist);
	file.open(QIODevice::WriteOnly);

	QTextStream stream(&file);

	stream << g_pList->count() << endl;

	for(KviUrl *tmp=g_pList->first();tmp;tmp=g_pList->next())
	{
		stream << tmp->url << endl;
		stream << tmp->window << endl;
		stream << tmp->count << endl;
		stream << tmp->timestamp << endl;
	}
	file.flush();
	file.close();
}

void loadUrlList()
{
	QString urllist;
	g_pApp->getLocalKvircDirectory(urllist,KviApp::ConfigPlugins);
	urllist += g_pUrlListFilename;
	QFile file;
	file.setFileName(urllist);
	if (!file.open(QIODevice::ReadOnly))return;

	QTextStream stream(&file);

	g_pList->clear();

	for (UrlDlgList *tmpitem=g_pUrlDlgList->first();tmpitem;tmpitem=g_pUrlDlgList->next()) {
		if (tmpitem->dlg) tmpitem->dlg->m_pUrlList->clear();
	}
	KviUrl *tmp;
	int i=0;
	int num = stream.readLine().toInt();
	while ((!stream.atEnd()) && (i<num)){
		tmp = new KviUrl();
		tmp->url = stream.readLine();
		tmp->window = stream.readLine();
		tmp->count = stream.readLine().toInt();
		tmp->timestamp = stream.readLine();

		g_pList->append(tmp);

		for (UrlDlgList *tmpitem=g_pUrlDlgList->first();tmpitem;tmpitem=g_pUrlDlgList->next()) {
			if (tmpitem->dlg) {
				QString tmpCount;
				tmpCount.setNum(tmp->count);
				tmpitem->dlg->addUrl(QString(tmp->url), QString(tmp->window), tmpCount, QString(tmp->timestamp));
			}
		}
		i++;
	}
	file.close();
}

void saveBanList()
{
	QString banlist;
	g_pApp->getLocalKvircDirectory(banlist,KviApp::ConfigPlugins);
	banlist += g_pBanListFilename;
	QFile file;
	file.setFileName(banlist);
	file.open(QIODevice::WriteOnly);

	QTextStream stream(&file);

	stream << g_pBanList->count() << endl;
	for(QString *tmp=g_pBanList->first();tmp;tmp=g_pBanList->next())
	{
		stream << *tmp << endl;
	}
	file.flush();
	file.close();
}

void loadBanList()
{
	QString banlist;
	g_pApp->getLocalKvircDirectory(banlist,KviApp::ConfigPlugins);
	banlist += g_pBanListFilename;
	QFile file;
	file.setFileName(banlist);
	if (!file.open(QIODevice::ReadOnly))return;

	QTextStream stream(&file);

	g_pBanList->clear();

	int i=0;
	int num = stream.readLine().toInt();
	while ((!stream.atEnd()) && (i<num)){
		QString *tmp = new QString(stream.readLine());
		g_pBanList->append(tmp);
		i++;
	}
	file.close();
}


/*
 	@doc: url.list
	@type:
		command
	@title:
		url.list
	@short:
		Opens url list
	@syntax:
		url.list
	@description:
		This command opens a window containing the urls' list.
		In the list there is other information:[br]
		[U]Window[/U] : window where the url has been shown last[br]
		[U]Count[/U] : number of urls shown[br]
		[U]Timestamp[/U] : date/time when the url has been shown first[br]
		Clicking right on the url column of the list a menu will popup, through it
		you can remove the selected item, find the url in the window it appeared last, and
		say it to: [I]@Console, Channels, Querys, DCC Chats[/I] and [I]User windows[/I].[br]
		The list is saved to file when you click on the menu item or when you unload the plugin
		on condition that you have checked the relative checkbox in configuration dialog.[br]
		You can also open the url in your web browser double clicking on it in the url list window.[br][br]

  */

static bool url_kvs_cmd_list(KviKvsModuleCommandCall *)
{
	urllist();
	return true;
}

UrlDlgList *findFrame()
{
	UrlDlgList *tmpitem = g_pUrlDlgList->first();
	if (!tmpitem) {
		UrlDlgList *udl = new UrlDlgList();
		udl->dlg = 0;
		udl->menu_id = -1;
		g_pUrlDlgList->append(udl);
		tmpitem = g_pUrlDlgList->current();
	}
	return tmpitem;
}

bool urllist()
{
	UrlDlgList *tmpitem = findFrame();
	if (tmpitem->dlg) return false;

	tmpitem->dlg = new UrlDialog(g_pList);
	g_pFrame->addWindow(tmpitem->dlg);

	for(KviUrl *tmp=g_pList->first();tmp;tmp=g_pList->next())
	{
		QString tmpCount;
		tmpCount.setNum(tmp->count);
		tmpitem->dlg->addUrl(QString(tmp->url), QString(tmp->window), tmpCount, QString(tmp->timestamp));
	}
	return true;
}


/*
 	@doc: url.config
	@type:
		command
	@title:
		url.config
	@short:
		Loads URL list module
	@syntax:
		url.config
	@description:
		This command opens a configuration window where it is possible
		to setup plugin's parameters. You can also open this window by
		using popup menu in the url list window<BR><BR>
		<H3>Configure dialog options:</H3>
		There is also a ban list widget, which allows to have a list of words that plugin mustn't catch.<BR><BR>
		<I>E.g.<BR>
		<blockquote>if the word "ftp" is inserted in the ban list and if in a window there is an output like "ftp.kvirc.net",
		the url will not be catched.</blockquote></I>
		<HR>
*/

static bool url_kvs_cmd_config(KviKvsModuleCommandCall *)
{
	if (!g_pConfigDialog) g_pConfigDialog = new ConfigDialog();
	return true;
}

int check_url(KviWindow *w,const QString &szUrl) // return 0 if no occurence of the url were found
{
	int tmp = 0;

	for(QString *tmpi=g_pBanList->first();tmpi;tmpi=g_pBanList->next())
	{
		if (szUrl.indexOf(*tmpi,Qt::CaseInsensitive) != -1) tmp++;
	}
	if (tmp > 0) return tmp;

	for(KviUrl *u = g_pList->first();u;u=g_pList->next())
	{
		if (u->url == szUrl) {
			u->window = w->plainTextCaption();
			u->count++;
			tmp++;
		}
	}

	for (UrlDlgList *tmpitem=g_pUrlDlgList->first();tmpitem;tmpitem=g_pUrlDlgList->next()) {
		if (tmpitem->dlg) {
			int count=tmpitem->dlg->m_pUrlList->topLevelItemCount();
			for(int i=0;i<count;i++)
			{
				QTreeWidgetItem* it=tmpitem->dlg->m_pUrlList->topLevelItem(i);
				if (it->text(0) == szUrl) {
					int tmpCount = it->text(2).toInt();
					tmpCount++;
					QString tmpStr;
					tmpStr.setNum(tmpCount);
					it->setText(2,tmpStr);
					it->setText(1,w->plainTextCaption());
				}
			}
		}
	}
	return tmp;
}

bool urllist_module_event_onUrl(KviKvsModuleEventCall * c)
{
	KviKvsVariant * vUrl = c->firstParam();
	QString szUrl;
	if(vUrl)vUrl->asString(szUrl);

	if (check_url(c->window(),szUrl) == 0)
	{

		KviUrl *tmp = new KviUrl;
		QString tmpTimestamp;
		QDate d = QDate::currentDate();
		QString date;
		date.sprintf("%d-%d%d-%d%d",d.year(),d.month() / 10,d.month() % 10,d.day() / 10,d.day() % 10);
		tmpTimestamp = "["+date+"]"+" [";
		tmpTimestamp += QTime::currentTime().toString()+"]";
		tmp->url = szUrl;
		tmp->window = c->window()->plainTextCaption();
		tmp->count = 1;
		tmp->timestamp = tmpTimestamp;

		g_pList->append(tmp);
		for (UrlDlgList *tmpitem=g_pUrlDlgList->first();tmpitem;tmpitem=g_pUrlDlgList->next()) {
			if (tmpitem->dlg) {
				QString tmpCount;
				tmpCount.setNum(tmp->count);
				tmpitem->dlg->addUrl(QString(tmp->url), QString(tmp->window), tmpCount, QString(tmp->timestamp));
				tmpitem->dlg->windowListItem()->highlight(false);
			}
		}
	}
	return true;
}


/*	@doc: url.load
	@type:
		command
	@title:
		url.load
	@short:
		Loads URL list module
	@syntax:
		url.load
	@description:
		Loads the URL list module which keeps track of all urls shown in kvirc windows.
*/


static bool url_module_init(KviModule *m)
{
	KviModuleExtensionDescriptor * d = m->registerExtension("tool",
							KVI_URL_EXTENSION_NAME,
							__tr2qs("View URL list"),
							url_extension_alloc);
	if(d)d->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_URL)));

	g_pList = new KviPointerList<KviUrl>;
	g_pList->setAutoDelete(true);

	g_pUrlDlgList = new KviPointerList<UrlDlgList>;
	g_pUrlDlgList->setAutoDelete(true);

	g_pBanList = new KviPointerList<QString>;
	g_pBanList->setAutoDelete(true);

	KVSM_REGISTER_SIMPLE_COMMAND(m,"list",url_kvs_cmd_list);
	KVSM_REGISTER_SIMPLE_COMMAND(m,"config",url_kvs_cmd_config);
	g_pUrlAction = new KviUrlAction(KviActionManager::instance());
	KviActionManager::instance()->registerAction(g_pUrlAction);

	m->kvsRegisterAppEventHandler(KviEvent_OnURL,urllist_module_event_onUrl);

	g_pApp->getLocalKvircDirectory(szConfigPath,KviApp::ConfigPlugins,"url.conf");

	loadUrlList();
	loadBanList();

	UrlDlgList *udl = new UrlDlgList();
	udl->dlg = 0;
	g_pUrlDlgList->append(udl);

	return true;
}

static bool url_module_cleanup(KviModule *)
{
	KviConfig cfg(szConfigPath,KviConfig::Read);
	cfg.setGroup("ConfigDialog");
	if (cfg.readBoolEntry("SaveUrlListOnUnload",false) == true) saveUrlList();
	for (UrlDlgList *tmpitem=g_pUrlDlgList->first();tmpitem;tmpitem=g_pUrlDlgList->next()) {
		if (tmpitem->dlg) tmpitem->dlg->close();
	}

	delete g_pList;
	g_pList = 0;
	delete g_pBanList;
	g_pBanList = 0;

	delete g_pUrlDlgList;
	g_pUrlDlgList = 0;
	delete g_pUrlAction;
	g_pUrlAction = 0;

	return true;
}

static bool url_module_can_unload(KviModule *)
{
	// FIXME: really ugly :/
	return false;
}

void url_module_config()
{
	if (!g_pConfigDialog) g_pConfigDialog = new ConfigDialog();
}

/*
	@doc: url
	@type:
		module
	@short:
		The URL list module: keeps track of all urls shown in kvirc windows
	@title:
		The URL list module
	@body:
		This plugin keeps track of all urls shown in kvirc windows.
		<H3>Exported commands:</H3>
		<B>/url.list</B> : this command opens a window containing the urls' list.
		In the list there is other information:<BR>
		<U>Window</U> : window where the url has been shown last<BR>
		<U>Count</U> : number of urls shown<BR>
		<U>Timestamp</U> : date/time when the url has been shown first<BR>
		Clicking right on the url column of the list a menu will popup, through it
		you can remove the selected item, find the url in the window it appeared last, and
		say it to: <I>@Console, Channels, Querys, DCC Chats</I> and <I>User windows</I>.<BR>
		The list is saved to file when you click on the menu item or when you unload the plugin
		on condition that you have checked the relative checkbox in configuration dialog.<BR>
		You can also open the url in your web browser double clicking on it in the url list window.<BR><BR>

		Mail me if you have any suggestion or you want to notice a bug.<BR>
		<B>Andrea 'YaP' Parrella</B> &lt;anandrea@iname.com&gt;<BR><BR>
		Thanks to:<BR>
		<B>Szymon Stefanek</B> and <B>Till Bush</B> for their help.<BR>
		<B>Ted Podgurski</B> for toolbar's icons.
*/


KVIRC_MODULE(
	"URL",
	"4.0.0" ,
	"Copyright (C) 2002 Andrea Parrella <yap@yapsoft.it>",
	"url list module for KVIrc",
	url_module_init,
	url_module_can_unload,
	0,
	url_module_cleanup,
	0
)

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "libkviurl.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
