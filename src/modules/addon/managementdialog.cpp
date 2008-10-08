//=============================================================================
//
//   File : managementdialog.cpp
//   Created on Fri 08 Apr 2005 14:54:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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

#include "managementdialog.h"

#include "kvi_listview.h"
#include "kvi_locale.h"
#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_kvs_scriptaddonmanager.h"
#include "kvi_window.h"

#include "kvi_filedialog.h"
#include "kvi_fileutils.h"
#include "kvi_kvs_script.h"
#include "kvi_sourcesdate.h"

#include <qpushbutton.h>
#include <qlayout.h>
#include <qapplication.h>
#include <qtooltip.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qframe.h>
#include "kvi_tal_scrollview.h"
#ifdef COMPILE_USE_QT4
	#include <q3header.h>

#else
	#include <qheader.h>
#endif
#include "kvi_draganddrop.h"
#include <qpainter.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qevent.h>

KviScriptManagementDialog * KviScriptManagementDialog::m_pInstance = 0;
extern QRect g_rectManagementDialogGeometry;


#define LVI_ICON_SIZE 32
#define LVI_BORDER 4
#define LVI_SPACING 8
#define LVI_MINIMUM_TEXT_WIDTH 300
#define LVI_MINIMUM_CELL_WIDTH (LVI_MINIMUM_TEXT_WIDTH + LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER)

KviScriptAddonListViewItem::KviScriptAddonListViewItem(KviTalListView * v,KviKvsScriptAddon * a)
: KviTalListViewItem(v,"")
{
	m_pAddon = new KviKvsScriptAddon(*a);
	m_pListView = v;
	QString t = "<nobr><b>";
	t += a->visibleName();
	t += "</b> [";
	t += a->version();
	t += "]";
	t += " <font color=\"#a0a0a0\">[";
	t += a->name();
	t += "]</font></nobr>";
	t += "<br><nobr><font size=\"-1\">";
	t += a->description();
	t += "</font></nobr>";
	m_szKey = a->visibleName().upper();
#ifdef COMPILE_USE_QT4
	m_pText = new QTextDocument();
	m_pText->setHtml(t);
	m_pText->setDefaultFont(v->font());
#else
	m_pText = new QSimpleRichText(t,v->font());
#endif

	QPixmap * p = a->icon();
	m_pIcon = p ? new QPixmap(*p) : new QPixmap(LVI_ICON_SIZE,LVI_ICON_SIZE);
}

KviScriptAddonListViewItem::~KviScriptAddonListViewItem()
{
	delete m_pIcon;
	delete m_pText;
	delete m_pAddon;
}

QString KviScriptAddonListViewItem::key(int,bool) const
{
	return m_szKey;
}

void KviScriptAddonListViewItem::setup()
{
	KviTalListViewItem::setup();
	int iWidth = m_pListView->visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	iWidth -= LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER;
	
	#ifdef COMPILE_USE_QT4 
	int iHeight = m_pText->size().height() + (2 * LVI_BORDER);
	#else
	m_pText->setWidth(iWidth);
	int iHeight = m_pText->height() + (2 * LVI_BORDER);
	#endif
	if(iHeight < (LVI_ICON_SIZE + (2 * LVI_BORDER)))iHeight = LVI_ICON_SIZE + (2 * LVI_BORDER);
	setHeight(iHeight+2);
}

void KviScriptAddonListViewItem::paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align)
{
	#ifdef COMPILE_USE_QT4
	if (isSelected())
	{
		QColor col(m_pListView->palette().highlight());
		col.setAlpha(127);
		p->setBrush(col);
		p->drawRect(0, 0, m_pListView->visibleWidth(), height());
	}
	p->drawPixmap(LVI_BORDER,LVI_BORDER,*m_pIcon);
	int afterIcon = LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING;
	int www = m_pListView->visibleWidth() - (afterIcon + LVI_BORDER);
	p->translate(afterIcon,LVI_BORDER);
	m_pText->setPageSize(QSizeF(www,height() - (LVI_BORDER * 2)));
	m_pText->drawContents(p);
	#else
	p->drawPixmap(LVI_BORDER,LVI_BORDER,*m_pIcon);
	int afterIcon = LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING;
	int www = m_pListView->visibleWidth() - (afterIcon + LVI_BORDER);
	m_pText->setWidth(www);
	if(isSelected())
	{
		QColorGroup cg2(cg);
		cg2.setColor(QColorGroup::HighlightedText,cg.text());
	
		m_pText->draw(p,afterIcon,LVI_BORDER,QRect(afterIcon,LVI_BORDER,www,height() - (LVI_BORDER * 2)),cg2);
	} else {
		m_pText->draw(p,afterIcon,LVI_BORDER,QRect(afterIcon,LVI_BORDER,www,height() - (LVI_BORDER * 2)),cg);
	}
	#endif
}







KviScriptAddonListView::KviScriptAddonListView(QWidget * pParent)
: KviListView(pParent)
{
	QPixmap * p = g_pIconManager->getImage("kvi_dialog_addons.png");
	if(p)setBackgroundOverlayPixmap(p,Qt::AlignRight | Qt::AlignBottom);

	setSelectionMode(Single);
	header()->hide();
	int iWidth = visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	addColumn("",iWidth);
	setSorting(0,true);
}

KviScriptAddonListView::~KviScriptAddonListView()
{
}

void KviScriptAddonListView::resizeEvent(QResizeEvent * e)
{
	KviListView::resizeEvent(e);
	int iWidth = visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	setColumnWidth(0,iWidth);
}


KviScriptManagementDialog::KviScriptManagementDialog(QWidget * p)
: QDialog(p,"" /*,WType_TopLevel | WStyle_Customize | WStyle_Title | WStyle_StaysOnTop | WStyle_DialogBorder*/)
{
	setCaption(__tr2qs("Manage Script-Based Addons"));
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ADDONS)));
	setModal(true);

	m_pInstance = this;

	QGridLayout * g = new QGridLayout(this,11,3,8,8);
	
	/*QLabel * lb = new QLabel(this);
	lb->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

	g->addMultiCellWidget(lb,0,10,0,0);
	QPixmap * pix = g_pIconManager->getImage("kvi_dialog_addons.png");
	if(pix)
	{
		lb->setPixmap(*pix);
		lb->setFixedWidth(pix->width());
	}
	lb->setBackgroundColor(Qt::black);
	lb->setAlignment(Qt::AlignBottom | Qt::AlignRight);
	*/
	m_pListView = new KviScriptAddonListView(this);
	g->addMultiCellWidget(m_pListView,0,10,1,1);

	m_pConfigureButton = new QPushButton(__tr2qs("Configure"),this);
	connect(m_pConfigureButton,SIGNAL(clicked()),this,SLOT(configureScript()));
	g->addWidget(m_pConfigureButton,0,2);

	m_pHelpButton = new QPushButton(__tr2qs("Show Help"),this);
	connect(m_pHelpButton,SIGNAL(clicked()),this,SLOT(showScriptHelp()));
	g->addWidget(m_pHelpButton,1,2);

	g->addRowSpacing(2,40);

	m_pUninstallButton = new QPushButton(__tr2qs("Uninstall"),this);
	connect(m_pUninstallButton,SIGNAL(clicked()),this,SLOT(uninstallScript()));
	g->addWidget(m_pUninstallButton,3,2);

	g->addRowSpacing(4,15);

	QFrame *f = new QFrame(this);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	g->addWidget(f,5,2);
	
	g->addRowSpacing(6,15);

	m_pInstallButton = new QPushButton(__tr2qs("Install Addon..."),this);
	connect(m_pInstallButton,SIGNAL(clicked()),this,SLOT(installScript()));
	g->addWidget(m_pInstallButton,7,2);

	m_pGetScriptsButton = new QPushButton(__tr2qs("More Addons..."),this);
	connect(m_pGetScriptsButton,SIGNAL(clicked()),this,SLOT(getMoreScripts()));
	g->addWidget(m_pGetScriptsButton,8,2);

	QPushButton * b = new QPushButton(__tr2qs("Close"),this);
	connect(b,SIGNAL(clicked()),this,SLOT(closeClicked()));
	g->addWidget(b,10,2);

	g->setRowStretch(9,1);
	g->setColStretch(1,1);

	fillListView();

	currentChanged(0);
	connect(m_pListView,SIGNAL(currentChanged(KviTalListViewItem *)),this,SLOT(currentChanged(KviTalListViewItem *)));
	//currentToolBarChanged();

	if(g_rectManagementDialogGeometry.y() < 5)
	{
		g_rectManagementDialogGeometry.setY(5);
	}
	resize(g_rectManagementDialogGeometry.width(),
		g_rectManagementDialogGeometry.height());
	move(g_rectManagementDialogGeometry.x(),
		g_rectManagementDialogGeometry.y());
}

KviScriptManagementDialog::~KviScriptManagementDialog()
{
	g_rectManagementDialogGeometry = QRect(pos().x(),pos().y(),size().width(),size().height());

	//KviActionManager::instance()->customizeToolBarsDialogDestroyed();
	m_pInstance = 0;
}

void KviScriptManagementDialog::fillListView()
{
	m_pListView->clear();
	KviPointerHashTable<QString,KviKvsScriptAddon> * d = KviKvsScriptAddonManager::instance()->addonDict();
	if(!d)return;
	KviPointerHashTableIterator<QString,KviKvsScriptAddon> it(*d);
	KviScriptAddonListViewItem * item;
	while(KviKvsScriptAddon * a = it.current())
	{
		item = new KviScriptAddonListViewItem(m_pListView,a);
		++it;
	}
}

void KviScriptManagementDialog::currentChanged(KviTalListViewItem *)
{
	KviScriptAddonListViewItem * it = (KviScriptAddonListViewItem *)m_pListView->currentItem();
	if(!it)
	{
		m_pConfigureButton->setEnabled(false);
		m_pUninstallButton->setEnabled(false);
		m_pHelpButton->setEnabled(false);
	} else {
		m_pConfigureButton->setEnabled(!(it->addon()->configureCallbackCode().isEmpty()));
		m_pHelpButton->setEnabled(!(it->addon()->helpCallbackCode().isEmpty()));
		m_pUninstallButton->setEnabled(true);
	}
}

void KviScriptManagementDialog::showScriptHelp()
{
	KviScriptAddonListViewItem * it = (KviScriptAddonListViewItem *)m_pListView->currentItem();
	if(!it)return;
	if(it->addon()->helpCallbackCode().isEmpty())return;
	it->addon()->executeHelpCallback(g_pActiveWindow);
}

void KviScriptManagementDialog::configureScript()
{
	KviScriptAddonListViewItem * it = (KviScriptAddonListViewItem *)m_pListView->currentItem();
	if(!it)return;
	if(it->addon()->configureCallbackCode().isEmpty())return;
	it->addon()->executeConfigureCallback(g_pActiveWindow);
}

void KviScriptManagementDialog::uninstallScript()
{
	KviScriptAddonListViewItem * it = (KviScriptAddonListViewItem *)m_pListView->currentItem();
	if(!it)return;

	QString txt = "<p>";
	txt += __tr2qs("Do you really want to uninstall the addon \"%1\" ?").arg(it->addon()->visibleName());
	txt += "</p>";
	
	if(QMessageBox::question(this,
		__tr2qs("Confirm addon uninstallation"),txt,__tr2qs("&Yes"),__tr2qs("&No"),0,1) != 0)return;

	KviKvsScriptAddonManager::instance()->unregisterAddon(it->addon()->name(),g_pActiveWindow);
	
	fillListView();
	currentChanged(0);
}

void KviScriptManagementDialog::getMoreScripts()
{
	KviKvsScript::run("openurl http://www.kvirc.net/?id=addons&version=" KVI_VERSION "." KVI_SOURCES_DATE,g_pActiveWindow);
}

void KviScriptManagementDialog::installScript()
{
	QString buffer;

	if(!KviFileDialog::askForOpenFileName(buffer,__tr2qs("Please select the addon installation file"),QString::null,"install.kvs",false,true))return;

	buffer.replace("\\","\\\\");

	QString szCmd = "parse \"";
	szCmd += buffer;
	szCmd += "\"";

	KviKvsScript::run(szCmd,g_pActiveWindow);

	fillListView();
	currentChanged(0);

	m_pListView->publicUpdateContents();
	//m_pListView->triggerUpdate();
}

void KviScriptManagementDialog::showEvent(QShowEvent * e)
{
//	QRect r = parentWidget() ? parentWidget()->rect() : QApplication::desktop()->rect();
//	int x = (r.width() - width()) / 2;
//	int y = (r.height() - height()) / 2;
//	move(x,y);
}

void KviScriptManagementDialog::closeClicked()
{
	delete this;
}

void KviScriptManagementDialog::cleanup()
{
	if(!m_pInstance)return;
	delete m_pInstance;
	m_pInstance = 0;
}

void KviScriptManagementDialog::display()
{
	if(m_pInstance)return;
	m_pInstance = new KviScriptManagementDialog(g_pFrame);
	m_pInstance->show();
}

void KviScriptManagementDialog::closeEvent(QCloseEvent * e)
{
	e->ignore();
	delete this;
}

void KviScriptManagementDialog::reject()
{
	cleanup();
}
