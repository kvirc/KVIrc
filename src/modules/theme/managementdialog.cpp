//=============================================================================
//
//   File : managementdialog.cpp
//   Created on Sat 30 Dec 2006 14:54:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2006 Szymon Stefanek <pragma at kvirc dot net>
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

#define LVI_ICON_SIZE 32
#define LVI_BORDER 4
#define LVI_SPACING 8
#define LVI_MINIMUM_TEXT_WIDTH 300
#define LVI_MINIMUM_CELL_WIDTH (LVI_MINIMUM_TEXT_WIDTH + LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER)

#include "managementdialog.h"
#include "packthemedialog.h"
#include "savethemedialog.h"
#include "themefunctions.h"

#include "kvi_iconmanager.h"
#include "kvi_msgbox.h"


#include <qlayout.h>
#include <qpushbutton.h>
#include "kvi_tal_listbox.h"
#include <qlineedit.h>
#include <kvi_tal_textedit.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qfiledialog.h>
#include "kvi_tal_popupmenu.h"
#include <qcombobox.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qimage.h>
#include <kvi_tal_textedit.h>
//#include <qmultilineedit.h>
#ifdef COMPILE_USE_QT4
#include <QCloseEvent>
#endif
#include <qbuffer.h>

#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_config.h"
#include "kvi_fileutils.h"
#include "kvi_app.h"
#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_internalcmd.h"
#include "kvi_styled_controls.h"
#include "kvi_doublebuffer.h"
#include "kvi_packagefile.h"
#include "kvi_fileextensions.h"
#include "kvi_filedialog.h"
#include "kvi_dynamictooltip.h"

#include <stdlib.h> // rand & srand

extern QRect g_rectManagementDialogGeometry;



KviThemeListBoxItem::KviThemeListBoxItem(KviTalListBox * box,KviThemeInfo * inf)
: KviTalListBoxText(box)
{
	m_pThemeInfo = inf;
	QString t;
	t = "<nobr><b>";
	t += inf->name();
	t += "</b>";
	
	if(!inf->version().isEmpty()) {
		t += "[";
		t += inf->version();
		t += "]";
	}
	
	if(!inf->author().isEmpty()) {
		t += " <font color=\"#a0a0a0\"> ";
		t += __tr2qs_ctx("by","theme");
		t += " ";
		t += inf->author();
		t += "</font>";
	}

	t += "</nobr>";
	t += "<br><nobr><font size=\"-1\">";
	t += inf->description();
	t += "</font></nobr>";
	m_pText = new KviTalSimpleRichText(t,box->font());
	int iWidth = box->visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	iWidth -= LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER;
	m_pText->setWidth(iWidth);
}

KviThemeListBoxItem::~KviThemeListBoxItem()
{
	delete m_pThemeInfo;
	delete m_pText;
}

void KviThemeListBoxItem::paint(QPainter * p)
{
	KviTalListBoxText::paint(p);
	p->drawPixmap(LVI_BORDER,LVI_BORDER, *(g_pIconManager->getBigIcon(QString(KVI_BIGICON_THEME))) );
	int afterIcon = LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING;
	int www = p->window().width() - (afterIcon + LVI_BORDER);
	m_pText->setWidth(www);
	m_pText->draw(p,afterIcon,LVI_BORDER,QRect(afterIcon,LVI_BORDER,www,p->window().height() - (LVI_BORDER * 2)),listBox()->viewport()->colorGroup());
}

int KviThemeListBoxItem::height(const KviTalListBox * lb) const 
{
	int iHeight = m_pText->height() + (2 * LVI_BORDER);
	if(iHeight < (LVI_ICON_SIZE + (2 * LVI_BORDER)))iHeight = LVI_ICON_SIZE + (2 * LVI_BORDER);
	return iHeight;
}

KviThemeManagementDialog * KviThemeManagementDialog::m_pInstance = 0;


KviThemeManagementDialog::KviThemeManagementDialog(QWidget * parent)
: QDialog(parent,"theme_options_widget")
{
	setCaption(__tr2qs_ctx("Manage Themes - KVIrc","theme"));
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_THEME)));

	setModal(true);

	m_pInstance = this;

	QGridLayout * g = new QGridLayout(this,3,2,4,4);

	KviTalHBox *hb = new KviTalHBox(this);
	g->addMultiCellWidget(hb,0,0,0,1);

	KviStyledToolButton * tb;
	QFrame * sep;

	tb = new KviStyledToolButton(hb);
	tb->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_SAVE)));
	tb->setUsesBigPixmap(true);
	QToolTip::add(tb,__tr2qs_ctx("Save Current Theme...","theme"));
	connect(tb,SIGNAL(clicked()),this,SLOT(saveCurrentTheme()));

	sep = new QFrame(hb);
	sep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	sep->setMinimumWidth(12);
	
	m_pPackThemeButton = new KviStyledToolButton(hb);
	m_pPackThemeButton->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_PACK)));
	m_pPackThemeButton->setUsesBigPixmap(true);
	QToolTip::add(m_pPackThemeButton,__tr2qs_ctx("Export Selected Themes to a Distributable Package","theme"));
	connect(m_pPackThemeButton,SIGNAL(clicked()),this,SLOT(packTheme()));

	m_pDeleteThemeButton = new KviStyledToolButton(hb);
	m_pDeleteThemeButton->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_REMOVE)));
	m_pDeleteThemeButton->setUsesBigPixmap(true);
	QToolTip::add(m_pDeleteThemeButton,__tr2qs_ctx("Delete Selected Themes","theme"));
	connect(m_pDeleteThemeButton,SIGNAL(clicked()),this,SLOT(deleteTheme()));

	sep = new QFrame(hb);
	sep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	sep->setMinimumWidth(12);

	tb = new KviStyledToolButton(hb);
	tb->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_OPEN)));
	tb->setUsesBigPixmap(true);
	QToolTip::add(tb,__tr2qs_ctx("Install Theme Package From Disk","theme"));
	connect(tb,SIGNAL(clicked()),this,SLOT(installFromFile()));

	tb = new KviStyledToolButton(hb);
	tb->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_WWW)));
	tb->setUsesBigPixmap(true);
	QToolTip::add(tb,__tr2qs_ctx("Get More Themes...","theme"));
	connect(tb,SIGNAL(clicked()),this,SLOT(getMoreThemes()));

	QWidget *w= new QWidget(hb);
	w->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

	m_pListBox = new KviTalListBox(this);
	m_pListBox->setMinimumHeight(400);
	m_pListBox->setMinimumWidth(400);
	m_pListBox->setSelectionMode(KviTalListBox::Extended);
	connect(m_pListBox,SIGNAL(doubleClicked(KviTalListBoxItem *)),this,SLOT(applyTheme(KviTalListBoxItem *)));
	connect(m_pListBox,SIGNAL(contextMenuRequested(KviTalListBoxItem *,const QPoint &)),
		this,SLOT(contextMenuRequested(KviTalListBoxItem *,const QPoint &)));
	connect(m_pListBox,SIGNAL(selectionChanged()),this,SLOT(enableDisableButtons()));
	g->addMultiCellWidget(m_pListBox,1,1,0,1);

	KviDynamicToolTip * tip = new KviDynamicToolTip(m_pListBox); 
	connect(tip,SIGNAL(tipRequest(KviDynamicToolTip *,const QPoint &)),this,SLOT(tipRequest(KviDynamicToolTip *,const QPoint &))); 

	QPushButton * b = new QPushButton(__tr2qs("Close"),this);
	connect(b,SIGNAL(clicked()),this,SLOT(closeClicked()));
	g->addWidget(b,2,1);

	g->setRowStretch(1,0);
	g->setColStretch(0,1);

	fillThemeBox();
	m_pContextPopup = new KviTalPopupMenu(this);

	if(g_rectManagementDialogGeometry.y() < 5)
	{
		g_rectManagementDialogGeometry.setY(5);
	}
	resize(g_rectManagementDialogGeometry.width(),
		g_rectManagementDialogGeometry.height());
	move(g_rectManagementDialogGeometry.x(),
		g_rectManagementDialogGeometry.y());
}

KviThemeManagementDialog::~KviThemeManagementDialog()
{
	g_rectManagementDialogGeometry = QRect(pos().x(),pos().y(),size().width(),size().height());
	m_pInstance = 0;
}

void KviThemeManagementDialog::closeClicked()
{
	delete this;
	m_pInstance = 0;
}


void KviThemeManagementDialog::display()
{
	if(!m_pInstance)
		m_pInstance = new KviThemeManagementDialog(g_pFrame);
	m_pInstance->show();
}

void KviThemeManagementDialog::cleanup()
{
	if(!m_pInstance)return;
	delete m_pInstance;
	m_pInstance = 0;
}

void KviThemeManagementDialog::packTheme()
{
	KviPointerList<KviThemeInfo> dl;
	dl.setAutoDelete(false);
	for(KviThemeListBoxItem * it = (KviThemeListBoxItem *)m_pListBox->firstItem();it;it = (KviThemeListBoxItem *)it->next())
	{
		if(it->isSelected())
			dl.append(it->themeInfo());
	}
	if(dl.isEmpty())return;

	KviPackThemeDialog * pDialog = new KviPackThemeDialog(this,&dl);
	pDialog->exec();
	delete pDialog;

}

void KviThemeManagementDialog::contextMenuRequested(KviTalListBoxItem * it,const QPoint & pos)
{
	if(it)
	{
		m_pListBox->setCurrentItem(it);
		m_pContextPopup->clear();
		m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MINUS)),__tr2qs_ctx("&Remove Theme","theme"),this,SLOT(deleteTheme()));
		m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)),__tr2qs_ctx("&Apply Theme","theme"),this,SLOT(applyCurrentTheme()));
		m_pContextPopup->popup(pos);
	}
}

void KviThemeManagementDialog::applyTheme ( KviTalListBoxItem * it)
{
	if(it)m_pListBox->setCurrentItem(it);
	applyCurrentTheme();
}

void KviThemeManagementDialog::applyCurrentTheme()
{
	KviThemeListBoxItem * it = (KviThemeListBoxItem *)m_pListBox->item(m_pListBox->currentItem());
	if(!it)return;

	if(KviMessageBox::yesNo(__tr2qs_ctx("Apply theme - KVIrc","theme"),
		__tr2qs_ctx("Do you wish to apply theme \"%Q\" (version %Q)?","theme"),
		&(it->themeInfo()->name()),&(it->themeInfo()->version())))
	{
		QString szPath = it->themeInfo()->absoluteDirectory();
		if(szPath.isEmpty())return;

		KviThemeInfo out;
		if(!KviTheme::load(szPath,out))
		{
			QString szErr = out.lastError();
			QString szMsg;
			KviQString::sprintf(szMsg,__tr2qs_ctx("Failed to apply the specified theme: %Q","theme"),&szErr);
			QMessageBox::critical(this,__tr2qs_ctx("Apply theme - KVIrc","theme"),szMsg,
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);

		}
	}
}

void KviThemeManagementDialog::deleteTheme()
{
	for(KviThemeListBoxItem * pItem = (KviThemeListBoxItem *)m_pListBox->firstItem();pItem;pItem = (KviThemeListBoxItem *)pItem->next())
	{
		if(pItem->isSelected())
		{
			if(!KviMessageBox::yesNo(__tr2qs_ctx("Delete Theme - KVIrc","theme"),
				__tr2qs_ctx("Do you really wish to delete theme \"%Q\" (version %Q)?","theme"),
					&(pItem->themeInfo()->name()),&(pItem->themeInfo()->version())))goto jump_out;

			KviFileUtils::deleteDir(pItem->themeInfo()->absoluteDirectory());
		}
	}
jump_out:
	fillThemeBox();
}

void KviThemeManagementDialog::installFromFile()
{
	int iThemeCount=0;
	QString szFileName;
	QString szError;
	
	if(!KviFileDialog::askForOpenFileName(szFileName,__tr2qs_ctx("Open Theme - KVIrc","theme"),QString::null,"*.kvt"))
		return;

	if(!KviThemeFunctions::installThemePackage(szFileName,szError,this))
	{
		KviMessageBox::information(szError);
		return;
	}
	fillThemeBox();
}

void KviThemeManagementDialog::getMoreThemes()
{
	if(!g_pFrame)return;
	g_pFrame->executeInternalCommand(KVI_INTERNALCOMMAND_OPENURL_KVIRC_THEMES);
}

void KviThemeManagementDialog::saveCurrentTheme()
{
	KviSaveThemeDialog * pSaveThemeDialog = new KviSaveThemeDialog(this);
	pSaveThemeDialog->exec();
	delete pSaveThemeDialog;
	fillThemeBox();
}

void KviThemeManagementDialog::fillThemeBox(const QString &szDir)
{
	QDir d(szDir);

	QStringList sl = d.entryList(QDir::Dirs);

	for(QStringList::Iterator it = sl.begin();it != sl.end();++it)
	{
		if(*it == ".")continue;
		if(*it == "..")continue;

		QString szTest = szDir;
		szTest += KVI_PATH_SEPARATOR_CHAR;
		szTest += *it;

		KviThemeInfo * inf = new KviThemeInfo();
		if(inf->loadFromDirectory(szTest))
		{
			inf->setSubdirectory(*it);
			KviThemeListBoxItem * item = 0;
			item = new KviThemeListBoxItem(m_pListBox,inf);
		} else {
			delete inf;
		}
	}
}

void KviThemeManagementDialog::fillThemeBox()
{
	m_pListBox->clear();

	QString szDir;
	g_pApp->getGlobalKvircDirectory(szDir,KviApp::Themes);
	fillThemeBox(szDir);
	g_pApp->getLocalKvircDirectory(szDir,KviApp::Themes);
	fillThemeBox(szDir);

	enableDisableButtons();
}

bool KviThemeManagementDialog::hasSelectedItems()
{
	for(KviTalListBoxItem * it = m_pListBox->firstItem();it;it = it->next())
	{
		if(it->isSelected())return true;
	}
	return false;
}


void KviThemeManagementDialog::enableDisableButtons()
{
	bool b = hasSelectedItems();
	m_pPackThemeButton->setEnabled(b);
	m_pDeleteThemeButton->setEnabled(b);
}

void KviThemeManagementDialog::closeEvent(QCloseEvent * e)
{
	e->ignore();
	delete this;
}

void KviThemeManagementDialog::tipRequest(KviDynamicToolTip *pTip,const QPoint &pnt)
{
	KviThemeListBoxItem * it = (KviThemeListBoxItem *)(m_pListBox->itemAt(pnt));

	if(!it)return;

	KviThemeInfo * pThemeInfo = it->themeInfo();

	QString szThemeDescription;

	KviThemeFunctions::getThemeHtmlDescription(
		szThemeDescription,
		pThemeInfo->name(),
		pThemeInfo->version(),
		pThemeInfo->description(),
		pThemeInfo->subdirectory(),
		pThemeInfo->application(),
		pThemeInfo->author(),
		pThemeInfo->date(),
		pThemeInfo->themeEngineVersion(),
		pThemeInfo->mediumScreenshot(),
		0
	);

	pTip->tip(m_pListBox->itemRect(it),szThemeDescription);
}
