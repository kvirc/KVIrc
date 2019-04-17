//=============================================================================
//
//   File : libkviurl.cpp
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

#include "libkviurl.h"

#include "KviModule.h"
#include "KviApplication.h"
#include "KviMainWindow.h"
#include "KviMenuBar.h"
#include "KviWindowStack.h"
#include "KviInternalCommand.h"
#include "KviIconManager.h"
#include "KviAction.h"
#include "KviActionManager.h"
#include "KviWindowListBase.h"
#include "KviKvsEventManager.h"
#include "KviWindow.h"
#include "KviOptions.h"
#include "KviQString.h"
#include "kvi_out.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCursor>
#include <QDateTime>
#include <QHeaderView>
#include <QTextStream>
#include <QMouseEvent>
#include <QPainter>
#include <QMenu>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
extern KVIRC_API QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

struct UrlDlgList
{
	UrlDialog * dlg;
	int menu_id;
};

const char * g_pUrlListFilename = "/list.kviurl";
const char * g_pBanListFilename = "/list.kviban";

std::unordered_set<KviUrl *> g_List;
std::vector<UrlDlgList *> g_UrlDlgList;
std::unordered_set<QString *> g_BanList;
ConfigDialog * g_pConfigDialog;

QString szConfigPath;

void saveUrlList();
void loadUrlList();
void saveBanList();
void loadBanList();
UrlDlgList * findFrame();
void url_module_help();

#define KVI_URL_EXTENSION_NAME "URL module extension"

UrlDialogTreeWidget::UrlDialogTreeWidget(QWidget * par)
    : QTreeWidget(par)
{
}

void UrlDialogTreeWidget::paintEvent(QPaintEvent * event)
{
	QPainter * p = new QPainter(viewport());
	QStyleOptionViewItem option = viewOptions();
	QRect rect = event->rect();

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p->save();
		p->setCompositionMode(QPainter::CompositionMode_Source);
		QColor col = KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p->fillRect(rect, col);
		p->restore();
	}
	else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = ((KviWindow *)parent())->isDocked() ? viewport()->mapTo(g_pMainWindow, rect.topLeft()) : viewport()->mapTo((KviWindow *)parent(), rect.topLeft());
		p->drawTiledPixmap(rect, *(g_pShadedChildGlobalDesktopBackground), pnt);
	}
	else
	{
#endif
		//FIXME this is not the treewindowlist
		p->fillRect(rect, KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground));
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	delete p;

	//call paint on all children
	QTreeWidget::paintEvent(event);
}

// ---------------------------- CLASS URLDIALOG ------------------------begin //

UrlDialog::UrlDialog(std::unordered_set<KviUrl *>)
    : KviWindow(KviWindow::Tool, "URL List")
{
	setAutoFillBackground(false);

	m_pUrlList = new UrlDialogTreeWidget(this);

	m_pMenuBar = new KviTalMenuBar(this, "URL menu");

	KviConfigurationFile cfg(szConfigPath, KviConfigurationFile::Read);

	m_pUrlList->header()->setSortIndicatorShown(true);
	m_pUrlList->setColumnCount(4);

	QStringList labels;
	labels << __tr2qs("URL") << __tr2qs("Window") << __tr2qs("Count") << __tr2qs("Timestamp");
	m_pUrlList->setHeaderLabels(labels);

	connect(m_pUrlList, SIGNAL(itemActivated(QTreeWidgetItem *, int)), SLOT(activate(QTreeWidgetItem *, int)));
	connect(m_pUrlList, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(contextMenu(const QPoint &)));
	m_pUrlList->setContextMenuPolicy(Qt::CustomContextMenu);
	m_pUrlList->setFocusPolicy(Qt::StrongFocus);
	m_pUrlList->setFocus();
}

void UrlDialog::config()
{
	if(!g_pConfigDialog)
		g_pConfigDialog = new ConfigDialog();
	else
		g_pConfigDialog->show();
}

void UrlDialog::help()
{
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
	g_List.clear();
	for(auto & tmpitem : g_UrlDlgList)
	{
		if(tmpitem->dlg)
			tmpitem->dlg->m_pUrlList->clear();
	}
}

void UrlDialog::close_slot()
{
	close();
}

void UrlDialog::open()
{
	open_url(m_szUrl);
}

void UrlDialog::remove()
{
	if(!m_pUrlList->currentItem())
	{
		QMessageBox::warning(nullptr, __tr2qs("Entry Selection - KVIrc"), __tr2qs("Must select a URL entry from the list to remove it."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		return;
	}

	for(auto & tmp : g_List)
	{
		if(tmp->url == m_pUrlList->currentItem()->text(0))
		{
			g_List.erase(tmp);
			delete m_pUrlList->currentItem();
			return;
		}
	}
}

void UrlDialog::findtext()
{
}

void UrlDialog::open_url(QString szUrl)
{
	QString cmd = "openurl ";
	KviQString::escapeKvs(&szUrl);
	cmd.append(szUrl);
	KviKvsScript::run(cmd, this);
}

void UrlDialog::activate(QTreeWidgetItem * item, int)
{
	open_url(item->text(0));
}

void UrlDialog::contextMenu(const QPoint & point)
{
	QTreeWidgetItem * item = m_pUrlList->itemAt(point);

	QMenu p("contextmenu", nullptr);
	if (item)
	{
		m_szUrl = item->text(0);
		p.setDefaultAction(p.addAction(__tr2qs("&Open"), this, SLOT(open())));
		p.addAction(__tr2qs("&Remove"), this, SLOT(remove()));

		p.addSeparator();
		m_pListPopup = new QMenu("list", nullptr);

		for(auto & w : g_pMainWindow->windowList())
		{
			if((w->type() == KviWindow::Channel) || (w->type() == KviWindow::Query) || (w->type() == KviWindow::DccChat))
			{
				m_pListPopup->addAction(w->plainTextCaption());
			}
		}
		p.addAction(__tr2qs("&Say to Window"))->setMenu(m_pListPopup);
		connect(m_pListPopup, SIGNAL(triggered(QAction *)), this, SLOT(sayToWin(QAction *)));

		p.addSeparator();
	}

	p.addAction(__tr2qs("Configure"), this, SLOT(config()));
	p.exec(m_pUrlList->viewport()->mapToGlobal(point));
}

void UrlDialog::sayToWin(QAction * act)
{
	KviWindow * wnd = g_pApp->findWindowByCaption(act->text());
	if(wnd)
	{
		QString szUrl = m_szUrl;
		QString szWindow = wnd->windowName();
		KviQString::escapeKvs(&szUrl);
		KviQString::escapeKvs(&szWindow);
		QString say = QString("PRIVMSG %1 %2").arg(szWindow, szUrl);
		KviKvsScript::run(say, wnd);
		g_pMainWindow->setActiveWindow(wnd);
	}
	else
	{
		QMessageBox::warning(nullptr, __tr2qs("Window Not Found - KVIrc"), __tr2qs("Window not found."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
	}
}

QPixmap * UrlDialog::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::Url);
}

void UrlDialog::addUrl(QString url, QString window, QString count, QString timestamp)
{
	QTreeWidgetItem * UrlItem = new QTreeWidgetItem(m_pUrlList);

	UrlItem->setText(0, url);
	UrlItem->setText(1, window);
	UrlItem->setText(2, count);
	UrlItem->setText(3, timestamp);

	UrlItem->setForeground(0, getMircColor(KVI_OPTION_MSGTYPE(KVI_OUT_URL).fore()));
	UrlItem->setForeground(1, getMircColor(KVI_OPTION_MSGTYPE(KVI_OUT_NONE).fore()));
	UrlItem->setForeground(2, getMircColor(KVI_OPTION_MSGTYPE(KVI_OUT_NONE).fore()));
	UrlItem->setForeground(3, getMircColor(KVI_OPTION_MSGTYPE(KVI_OUT_NONE).fore()));

	m_pUrlList->resizeColumnToContents(0);
	m_pUrlList->resizeColumnToContents(3);
	m_pUrlList->resizeColumnToContents(2);
	m_pUrlList->resizeColumnToContents(1);
}

void UrlDialog::resizeEvent(QResizeEvent *)
{
	int hght = m_pMenuBar->heightForWidth(width());
	m_pMenuBar->setGeometry(0, 0, width(), hght);
	m_pUrlList->setGeometry(0, hght, width(), height() - hght);
}

UrlDialog::~UrlDialog()
{

	delete m_pUrlList;
	UrlDlgList * tmpitem = findFrame();
	tmpitem->dlg = nullptr;
}

// ----------------------------- CLASS URLDIALOG -------------------------end //

// --------------------------- CLASS CONFIGDIALOG ----------------------begin //

ConfigDialog::ConfigDialog()
    : QDialog()
{
	setWindowTitle(__tr2qs("URL Module Configuration - KVIrc"));

	QGridLayout * g = new QGridLayout(this);

	KviConfigurationFile * cfg = new KviConfigurationFile(szConfigPath, KviConfigurationFile::Read);
	cfg->setGroup("ConfigDialog");

	cb[0] = new QCheckBox(__tr2qs("Save URL list on module unload"), this);
	cb[0]->setChecked(cfg->readBoolEntry("SaveUrlListOnUnload", false));
	g->addWidget(cb[0], 0, 0, 1, 2);

	cb[1] = new QCheckBox(__tr2qs("Save columns width on URL list close"), this);
	cb[1]->setChecked(cfg->readBoolEntry("SaveColumnWidthOnClose", false));
	g->addWidget(cb[1], 1, 0, 1, 2);

	bool tmp = cfg->readBoolEntry("BanEnabled", false);
	delete cfg;

	m_pBanFrame = new BanFrame(this, "banlist", tmp);
	g->addWidget(m_pBanFrame, 3, 0, 1, 2);

	QPushButton * b;
	// configure buttons
	b = new QPushButton(__tr2qs("&Cancel"), this);
	b->setObjectName("discard");

	connect(b, SIGNAL(clicked()), this, SLOT(discardbtn()));
	g->addWidget(b, 4, 0);

	b = new QPushButton(__tr2qs("&OK"), this);
	b->setObjectName("accept");

	connect(b, SIGNAL(clicked()), this, SLOT(acceptbtn()));
	g->addWidget(b, 4, 1);

	show();
}

void ConfigDialog::discardbtn()
{
	delete this;
}

void ConfigDialog::acceptbtn()
{
	KviConfigurationFile * cfg = new KviConfigurationFile(szConfigPath, KviConfigurationFile::Write);
	cfg->setGroup("ConfigDialog");

	if(m_pBanFrame)
		m_pBanFrame->saveBans(cfg);

	cfg->writeEntry("SaveUrlListOnUnload", cb[0]->isChecked());
	cfg->writeEntry("SaveColumnWidthOnClose", cb[1]->isChecked());
	delete cfg;

	delete this;
}

void ConfigDialog::closeEvent(QCloseEvent *)
{
	delete this;
}

ConfigDialog::~ConfigDialog()
{
	for(auto & i : cb)
		delete i;
	g_pConfigDialog = nullptr;
}

// --------------------------- CLASS CONFIGDIALOG ------------------------end //

// ---------------------------- CLASS BANFRAME ------------------------begin //

BanFrame::BanFrame(QWidget * parent, const char * name, bool banEnabled)
    : QFrame(parent)
{
	setFrameStyle(QFrame::Panel | QFrame::Raised);
	setObjectName(name);

	QGridLayout * g = new QGridLayout(this);

	m_pEnable = new QCheckBox(__tr2qs("Enable URL ban list"), this);
	connect(m_pEnable, SIGNAL(clicked()), this, SLOT(enableClicked()));
	m_pEnable->setChecked(banEnabled);
	g->addWidget(m_pEnable, 0, 0, 1, 2);

	m_pBanList = new QListWidget(this);
	m_pBanList->setMinimumHeight(100);
	loadBanList();
	for(auto tmp : g_BanList)
	{
		m_pBanList->addItem(*tmp); // load ban list into listbox
	}

	m_pBanList->setEnabled(m_pEnable->isChecked());
	g->addWidget(m_pBanList, 1, 0, 1, 2);

	m_pAddBtn = new QPushButton(__tr2qs("&Add Ban"), this);
	m_pAddBtn->setObjectName("add");
	connect(m_pAddBtn, SIGNAL(clicked()), this, SLOT(addBan()));
	m_pAddBtn->setEnabled(m_pEnable->isChecked());
	g->addWidget(m_pAddBtn, 2, 0);

	m_pRemoveBtn = new QPushButton(__tr2qs("&Remove Selected"), this);
	m_pRemoveBtn->setObjectName("remove");
	connect(m_pRemoveBtn, SIGNAL(clicked()), this, SLOT(removeBan()));
	m_pRemoveBtn->setEnabled(m_pEnable->isChecked());
	g->addWidget(m_pRemoveBtn, 2, 1);
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
	QString text = QInputDialog::getText(this, __tr2qs("URL Ban List - KVIrc"), __tr2qs("Enter a URL to ban."), QLineEdit::Normal, QString(), &ok);
	if(ok && !text.isEmpty())
	{
		QString * pText = new QString(std::move(text));
		g_BanList.insert(pText);
		m_pBanList->addItem(*pText);
	}
}

void BanFrame::removeBan()
{
	if(!m_pBanList->currentItem())
		return;

	if(!m_pBanList->currentItem()->isSelected())
	{
		QMessageBox::warning(nullptr, __tr2qs("Entry Selection - KVIrc"), __tr2qs("Must select a ban entry from the list to remove it."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		return;
	}
	QString item(m_pBanList->currentItem()->text());
	for(auto tmp : g_BanList)
	{
		if(tmp->compare(item) == 0)
		{
			g_BanList.erase(tmp);
			delete m_pBanList->currentItem();
			return;
		}
	}

	m_pBanList->removeItemWidget(m_pBanList->currentItem());
}

void BanFrame::saveBans(KviConfigurationFile * cfg)
{
	cfg->writeEntry("BanEnabled", m_pEnable->isChecked());
	if(m_pEnable->isChecked())
		saveBanList();
}

BanFrame::~BanFrame()
    = default;

// ---------------------------- CLASS URLTOOLBAR -------------------------end //

void saveUrlList()
{
	QString urllist;
	g_pApp->getLocalKvircDirectory(urllist, KviApplication::ConfigPlugins);
	urllist += g_pUrlListFilename;
	QFile file;
	file.setFileName(urllist);
	file.open(QIODevice::WriteOnly);

	QTextStream stream(&file);

	stream << g_List.size() << endl;

	for(auto tmp : g_List)
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
	g_pApp->getLocalKvircDirectory(urllist, KviApplication::ConfigPlugins);
	urllist += g_pUrlListFilename;
	QFile file;
	file.setFileName(urllist);
	if(!file.open(QIODevice::ReadOnly))
		return;

	QTextStream stream(&file);

	g_List.clear();

	for(auto tmpitem : g_UrlDlgList)
	{
		if(tmpitem->dlg)
			tmpitem->dlg->m_pUrlList->clear();
	}
	KviUrl * tmp;
	int i = 0;
	int num = stream.readLine().toInt();
	while((!stream.atEnd()) && (i < num))
	{
		tmp = new KviUrl();
		tmp->url = stream.readLine();
		tmp->window = stream.readLine();
		tmp->count = stream.readLine().toInt();
		tmp->timestamp = stream.readLine();

		g_List.insert(tmp);

		for(auto tmpitem : g_UrlDlgList)
		{
			if(tmpitem->dlg)
			{
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
	g_pApp->getLocalKvircDirectory(banlist, KviApplication::ConfigPlugins);
	banlist += g_pBanListFilename;
	QFile file;
	file.setFileName(banlist);
	file.open(QIODevice::WriteOnly);

	QTextStream stream(&file);

	stream << g_BanList.size() << endl;
	for(auto tmp : g_BanList)
	{
		stream << *tmp << endl;
	}
	file.flush();
	file.close();
}

void loadBanList()
{
	QString banlist;
	g_pApp->getLocalKvircDirectory(banlist, KviApplication::ConfigPlugins);
	banlist += g_pBanListFilename;
	QFile file;
	file.setFileName(banlist);
	if(!file.open(QIODevice::ReadOnly))
		return;

	QTextStream stream(&file);

	g_BanList.clear();

	int i = 0;
	int num = stream.readLine().toInt();
	while((!stream.atEnd()) && (i < num))
	{
		QString * tmp = new QString(stream.readLine());
		g_BanList.insert(tmp);
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
		Opens URL list
	@syntax:
		url.list
	@description:
		This command opens a window containing the URLs' list.
		In the list there is other information:[br]
		[b]Window[/b]: window where the URL has been shown last[br]
		[b]Count[/b]: number of URLs shown[br]
		[b]Timestamp[/b]: date/time when the URL has been shown first[br][br]

		Clicking right on the URL column of the list a menu will popup, through it
		you can remove the selected item, find the URL in the window it appeared last, and
		say it to: [i]@Console, Channels, Queries, DCC Chats[/i] and [i]User windows[/i].[br]
		The list is saved to file when you click on the menu item or when you unload the plugin
		on condition that you have checked the relative checkbox in configuration dialog.[br][br]

		You can also open the URL in your web browser double clicking on it in the URL list window.
  */

static bool url_kvs_cmd_list(KviKvsModuleCommandCall *)
{
	UrlDlgList * tmpitem = findFrame();
	if(tmpitem->dlg) {
		tmpitem->dlg->delayedAutoRaise();
		return false;
	}

	tmpitem->dlg = new UrlDialog(g_List);
	g_pMainWindow->addWindow(tmpitem->dlg);

	for(auto tmp : g_List)
	{
		QString tmpCount;
		tmpCount.setNum(tmp->count);
		tmpitem->dlg->addUrl(QString(tmp->url), QString(tmp->window), tmpCount, QString(tmp->timestamp));
	}
	return true;
}

UrlDlgList * findFrame()
{
	UrlDlgList * tmpitem = g_UrlDlgList.front();
	if(!tmpitem)
	{
		UrlDlgList * udl = new UrlDlgList();
		udl->dlg = nullptr;
		udl->menu_id = -1;
		g_UrlDlgList.push_back(udl);
		tmpitem = g_UrlDlgList.back();
	}
	return tmpitem;
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
		to setup plugin's parameters.[br]
		You can also open this window by using popup menu in the URL list window
		[big]Configure dialog options:[/big]
		There is also a ban list widget, which allows to have a list of words that plugin must not catch.[br][br]
		[i]e.g. if the word "ftp" is inserted in the ban list and if in a window there is an output like "ftp.kvirc.net",
		the URL will not be caught.[/i]
*/

static bool url_kvs_cmd_config(KviKvsModuleCommandCall *)
{
	if(!g_pConfigDialog)
		g_pConfigDialog = new ConfigDialog();
	return true;
}

int check_url(KviWindow * w, const QString & szUrl) // return 0 if no occurrence of the URL were found
{
	int tmp = 0;

	for(auto tmpi : g_BanList)
	{
		if(szUrl.indexOf(*tmpi, Qt::CaseInsensitive) != -1)
			tmp++;
	}
	if(tmp > 0)
		return tmp;

	for(auto u : g_List)
	{
		if(u->url == szUrl)
		{
			u->window = w->plainTextCaption();
			u->count++;
			tmp++;
		}
	}

	for(auto tmpitem : g_UrlDlgList)
	{
		if(tmpitem->dlg)
		{
			int count = tmpitem->dlg->m_pUrlList->topLevelItemCount();
			for(int i = 0; i < count; i++)
			{
				QTreeWidgetItem * it = tmpitem->dlg->m_pUrlList->topLevelItem(i);
				if(it->text(0) == szUrl)
				{
					int tmpCount = it->text(2).toInt();
					tmpCount++;
					QString tmpStr;
					tmpStr.setNum(tmpCount);
					it->setText(2, tmpStr);
					it->setText(1, w->plainTextCaption());
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
	if(vUrl)
		vUrl->asString(szUrl);

	if(check_url(c->window(), szUrl) == 0)
	{

		KviUrl * tmp = new KviUrl;
		QString tmpTimestamp;
		QDate d = QDate::currentDate();
		QString date;
		date.sprintf("%d-%d%d-%d%d", d.year(), d.month() / 10, d.month() % 10, d.day() / 10, d.day() % 10);
		tmpTimestamp = "[" + date + "]" + " [";
		tmpTimestamp += QTime::currentTime().toString() + "]";
		tmp->url = szUrl;
		tmp->window = c->window()->plainTextCaption();
		tmp->count = 1;
		tmp->timestamp = tmpTimestamp;

		g_List.insert(tmp);
		for(auto tmpitem : g_UrlDlgList)
		{
			if(tmpitem->dlg)
			{
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
		Loads the URL list module which keeps track of all URLs shown in KVIrc windows.
*/

static bool url_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m, "list", url_kvs_cmd_list);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "config", url_kvs_cmd_config);

	m->kvsRegisterAppEventHandler(KviEvent_OnURL, urllist_module_event_onUrl);

	g_pApp->getLocalKvircDirectory(szConfigPath, KviApplication::ConfigPlugins, "url.conf");

	loadUrlList();
	loadBanList();

	UrlDlgList * udl = new UrlDlgList();
	udl->dlg = nullptr;
	g_UrlDlgList.push_back(udl);

	return true;
}

static bool url_module_cleanup(KviModule *)
{
	KviConfigurationFile cfg(szConfigPath, KviConfigurationFile::Read);
	cfg.setGroup("ConfigDialog");
	if(cfg.readBoolEntry("SaveUrlListOnUnload", false) == true)
		saveUrlList();
	for(auto tmpitem : g_UrlDlgList)
	{
		if(tmpitem->dlg)
			tmpitem->dlg->close();
	}

	g_List.clear();
	g_BanList.clear();
	g_UrlDlgList.clear();

	return true;
}

static bool url_module_can_unload(KviModule *)
{
	// FIXME: really ugly :/
	return false;
}

void url_module_config()
{
	if(!g_pConfigDialog)
		g_pConfigDialog = new ConfigDialog();
}

/*
	@doc: url
	@type:
		module
	@short:
		The URL list module: keeps track of all URLs shown in KVIrc windows
	@title:
		The URL list module
	@body:
		This plugin keeps track of all URLs shown in KVIrc windows.
		[big]Exported commands:[/big]
		[b]/url.list[/b]: this command opens a window containing the URLs' list.
		In the list there is other information:[br]
		[b]Window[/b]: window where the URL has been shown last[br]
		[b]Count[/b]: number of URLs shown[br]
		[b]Timestamp[/b]: date/time when the URL has been shown first[br]

		Clicking right on the URL column of the list a menu will popup, through it
		you can remove the selected item, find the URL in the window it appeared last, and
		say it to: [i]@Console, Channels, Queries, DCC Chats[/i] and [i]User windows[/i].[br]
		The list is saved to file when you click on the menu item or when you unload the plugin
		on condition that you have checked the relative checkbox in configuration dialog.[br]

		You can also open the URL in your web browser double clicking on it in the URL list window.[br][br]

		Mail me if you have any suggestion or you want to notice a bug.[br]
		[b]Andrea 'YaP' Parrella[/b] &lt;anandrea@iname.com&gt;[br][br]
		Thanks to:[br]
		[b]Szymon Stefanek[/b] and [b]Till Bush[/b] for their help.[br]
		[b]Ted Podgurski[/b] for toolbar's icons.
*/

KVIRC_MODULE(
    "URL",
    "4.0.0",
    "Copyright (C) 2002 Andrea Parrella <yap@yapsoft.it>",
    "URL list module for KVIrc",
    url_module_init,
    url_module_can_unload,
    0,
    url_module_cleanup,
    0)
