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

#include "kvi_app.h"
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
#include "kvi_draganddrop.h"
#include "kvi_tal_scrollview.h"

#include <QPainter>
#include <QPixmap>
#include <QMessageBox>
#include <QEvent>
#include <QCloseEvent>
#include <QPushButton>
#include <QLayout>
#include <QApplication>
#include <QToolTip>
#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QAbstractTextDocumentLayout>

// FIXME: Qt4 #include <QHeaderView>
#include <q3header.h>




KviScriptManagementDialog * KviScriptManagementDialog::m_pInstance = 0;
extern QRect g_rectManagementDialogGeometry;


#define LVI_ICON_SIZE 32
#define LVI_BORDER 4
#define LVI_SPACING 8
#define LVI_MINIMUM_TEXT_WIDTH 300
#define LVI_MINIMUM_CELL_WIDTH (LVI_MINIMUM_TEXT_WIDTH + LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER)


void KviScriptAddonDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	painter->save();
	if (option.state & QStyle::State_Selected)
	{
		QPalette pal=option.palette;
		QBrush brush=pal.highlight();
		QColor col=brush.color();
		col.setAlpha(127);
		brush.setColor(col);
		painter->fillRect(option.rect,brush);
	}
	QString text=index.data(Qt::DisplayRole).toString();
	QPixmap pixmap;
	QRect decorationRect;
	QVariant value = index.data(Qt::DecorationRole);
	QStyle::State state=option.state;
	QRect rect=option.rect;
	int afterIcon = LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING;
	QIcon ico=QIcon(value.value<QIcon>());
	painter->drawPixmap(option.rect.x()+LVI_BORDER,option.rect.y()+LVI_BORDER,ico.pixmap(LVI_ICON_SIZE,LVI_ICON_SIZE));
	QTextDocument doc;
	doc.setHtml( text );
	painter->translate(option.rect.x()+afterIcon,option.rect.y()+LVI_BORDER);
	doc.setTextWidth(option.rect.width()-10);
	QRect cliprect=QRect(QPoint(0,0),QSize(option.rect.width()-afterIcon,option.rect.height()-(LVI_BORDER*2)-4));
	doc.drawContents(painter, cliprect);	
	painter->restore();
}

QSize KviScriptAddonDelegate::sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	return QSize(300,40);		
}

KviScriptAddonListViewItem::KviScriptAddonListViewItem(QListWidget * v,KviKvsScriptAddon * a)
: QListWidgetItem(v)
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

	setText(t);
	QPixmap * p = a->icon();
	if (p) setIcon(*p); 
	else setIcon(QPixmap(LVI_ICON_SIZE,LVI_ICON_SIZE));
}

KviScriptAddonListViewItem::~KviScriptAddonListViewItem()
{
	
	delete m_pAddon;
}

/*
QString KviScriptAddonListViewItem::key(int,bool) const
{
	return m_szKey;
}
*/

void KviScriptAddonListViewItem::setup()
{
	/*
	KviTalListViewItem::setup();
	int iWidth = m_pListView->visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	iWidth -= LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER;
	int iHeight = m_pText->size().height() + (2 * LVI_BORDER);
	if(iHeight < (LVI_ICON_SIZE + (2 * LVI_BORDER)))iHeight = LVI_ICON_SIZE + (2 * LVI_BORDER);
	setHeight(iHeight+2);
	*/
}

void KviScriptAddonListViewItem::paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align)
{
	/*
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
	*/
}

KviScriptAddonListView::KviScriptAddonListView(QWidget * pParent)
: QListWidget (pParent)
{
/*	QPixmap * p = g_pIconManager->getImage("kvi_dialog_addons.png");
	if(p)setBackgroundOverlayPixmap(p,Qt::AlignRight | Qt::AlignBottom);
*/
	setSelectionMode(QAbstractItemView::SingleSelection);
	int iWidth = width();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	
	// FIXME: doesn't work in Qt4
	/*
	int iWidth = visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	addColumn("",iWidth);
	*/
	setSortingEnabled(true);
}

KviScriptAddonListView::~KviScriptAddonListView()
{
}

void KviScriptAddonListView::resizeEvent(QResizeEvent * e)
{
	QListWidget::resizeEvent(e);
//	int iWidth = visibleWidth();
//	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
//	setColumnWidth(0,iWidth);
}


KviScriptManagementDialog::KviScriptManagementDialog(QWidget * p)
: QDialog(p,"" /*,WType_TopLevel | WStyle_Customize | WStyle_Title | WStyle_StaysOnTop | WStyle_DialogBorder*/)
{
	setWindowTitle(__tr2qs("Manage Script-Based Addons"));
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ADDONS)));
	setModal(true);

	m_pInstance = this;

	QGridLayout * g = new QGridLayout(this);
	
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
	m_pListView->setItemDelegate(new KviScriptAddonDelegate(m_pListView));

	QString szPic;
	g_pApp->getGlobalKvircDirectory(szPic,KviApp::Pics);
	szPic += "/kvi_dialog_addons.png";
	QString style("QListWidget {background-image: url(" + szPic + ");  background-attachment: fixed;}");
	debug("pic: %s",szPic.toUtf8().data());
	debug("style: %s",style.toUtf8().data());
	m_pListView->setStyleSheet(style);
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
	g->setColumnStretch(1,1);

	fillListView();
	
	currentChanged(0,0);
	connect(m_pListView,SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),this,SLOT(currentChanged(QListWidgetItem *,QListWidgetItem *)));
	//currentToolBarChanged();
	m_pListView->setCurrentItem(m_pListView->item(0));
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

void KviScriptManagementDialog::currentChanged(QListWidgetItem *item,QListWidgetItem *)
{
	KviScriptAddonListViewItem * it = (KviScriptAddonListViewItem *)item;
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
	currentChanged(0,0);
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
	currentChanged(0,0);

	//m_pListView->publicUpdateContents();
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
