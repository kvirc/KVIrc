//=============================================================================
//
//   File : dialog.cpp
//   Created on Fri 10 Feb 2006 18:52:18 by Szymon Stefanek
//   Based on dlg_options.cpp: Tue Jun 11 2000 02:39:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "dialog.h"

#include "kvi_app.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_module.h"
#include "kvi_pointerhashtable.h"
#include "kvi_tal_vbox.h"
#include "kvi_tal_tooltip.h"
#include "kvi_tal_popupmenu.h"
#include "kvi_tal_groupbox.h"

#include <QLayout>
#include <QLabel>
#include <QSplitter>
#include <QPushButton>
#include <QPainter>
#include <QFont>
#include <QEvent>
#include <QCloseEvent>
#include <QShortcut>
#include <QHeaderView>
#include <QDesktopWidget>
#include <QStackedWidget>

//FIXME: some headers must be hidden


//extern KviModule * g_pOptionsModule;
extern KviPointerHashTable<QString,KviOptionsDialog> * g_pOptionsDialogDict;

extern KVIRC_API KviApp * g_pApp;
extern KviOptionsInstanceManager * g_pOptionsInstanceManager;

KviGeneralOptionsFrontWidget::KviGeneralOptionsFrontWidget(QWidget *parent,const QString &szText)
:KviOptionsWidget(parent)
{
	setObjectName("general_options_front_widget");
	createLayout();
	QLabel * l = new QLabel(szText,this);
	l->setWordWrap(true);
	l->setAlignment(Qt::AlignTop);
	layout()->addWidget(l,0,0);
}

KviGeneralOptionsFrontWidget::~KviGeneralOptionsFrontWidget()
{
}


KviOptionsTreeWidgetItem::KviOptionsTreeWidgetItem(QTreeWidget *parent,KviOptionsWidgetInstanceEntry * e)
:QTreeWidgetItem()
{
	m_pInstanceEntry = e;
	m_pOptionsWidget = 0;
	setText(0,e->szName);
	setIcon(0,*(g_pIconManager->getSmallIcon(e->iIcon)));
	parent->insertTopLevelItem(0, this);
}

KviOptionsTreeWidgetItem::KviOptionsTreeWidgetItem(QTreeWidgetItem *parent,KviOptionsWidgetInstanceEntry * e)
:QTreeWidgetItem()
{
	m_pInstanceEntry = e;
	m_pOptionsWidget = 0;
	setText(0,e->szName);
	setIcon(0,*(g_pIconManager->getSmallIcon(e->iIcon)));
	parent->insertChild(0, this);
}

KviOptionsTreeWidgetItem::~KviOptionsTreeWidgetItem()
{
}

KviOptionsDialog::KviOptionsDialog(QWidget * par,const QString &szGroup)
: QDialog(par)
{
	setObjectName("general_options_dialog");

	setWindowIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_KVIRC)));

	m_szGroup = szGroup;

	QString szDialogTitle;

	if(szGroup.isEmpty() || KviQString::equalCI(szGroup,"general"))
	{
		szDialogTitle = __tr2qs_ctx("General Preferences","options");
	} else if(KviQString::equalCI(szGroup,"theme"))
	{
		szDialogTitle = __tr2qs_ctx("Theme Preferences","options");
	} else {
		szDialogTitle = __tr2qs_ctx("KVIrc Preferences","options");
	}

	QString szDialog = __tr2qs_ctx("This dialog contains a set of KVIrc settings.<br> Use the icons " \
			"on the left to navigate through the option pages. The text box in the " \
			"bottom left corner is a small search engine. It will highlight the " \
			"pages that contain options related to the search term you have entered.","options");


	QString szInfoTips;
	szInfoTips = __tr2qs_ctx("Many settings have tooltips that can be shown by holding " \
			"the cursor over their label for a few seconds.","options");

	QString szOkCancelButtons = __tr2qs_ctx("When you have finished, click \"<b>OK</b>\" to accept your changes " \
			"or \"<b>Cancel</b>\" to discard them. Clicking \"<b>Apply</b>\" will commit your " \
			"changes without closing the window.","options");

	QString szFrontText = QString(
			"<table width=\"100%\" height=\"100%\" valign=\"top\" align=\"center\" cellpadding=\"4\">" \
				"<tr>" \
					"<td bgcolor=\"#303030\" valign=\"top\">" \
						"<center><h1><font color=\"#FFFFFF\">%1</font></h1></center>" \
					"</td>" \
				"</tr>" \
				"<tr>" \
					"<td valign=\"bottom\">" \
						"<br>" \
						"<br>" \
						"<p>" \
							"%2" \
						"</p>" \
						"<br>" \
						"<p>" \
							"%3" \
						"</p>" \
						"<br>" \
						"<p>" \
							"%4" \
						"</p>" \
					"</td>" \
				"</tr>" \
			"</table>"
		).arg(szDialogTitle, szDialog, szInfoTips, szOkCancelButtons);

	QString szCaption = szDialogTitle + " - KVIrc";
	setWindowTitle(szCaption);

	QGridLayout * g1 = new QGridLayout(this);
	QSplitter * spl = new QSplitter(Qt::Horizontal,this);
	spl->setChildrenCollapsible(false);

	g1->addWidget(spl,0,0,1,5);

	KviTalVBox * vbox = new KviTalVBox(spl);
	vbox->setSpacing(2);
	vbox->setMargin(3);

	// Controlling list view
	m_pTreeWidget = new QTreeWidget(vbox);

	m_pTreeWidget->header()->hide();
	m_pTreeWidget->setRootIsDecorated(true);
	m_pTreeWidget->setSortingEnabled(false);
	m_pTreeWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

	connect(m_pTreeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem *)),this,SLOT(treeWidgetItemSelectionChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

	KviTalHBox * hbox = new KviTalHBox(vbox);
	hbox->setSpacing(2);
	hbox->setMargin(3);

	m_pSearchLineEdit = new QLineEdit(hbox);
	connect(m_pSearchLineEdit,SIGNAL(returnPressed()),this,SLOT(searchClicked()));
	m_pSearchButton = new QToolButton(hbox);
	m_pSearchButton->setIconSize(QSize(16,16));
	m_pSearchButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SEARCH)));
	connect(m_pSearchButton,SIGNAL(clicked()),this,SLOT(searchClicked()));
	connect(m_pSearchLineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(searchLineEditTextChanged(const QString &)));

	QString szTip = __tr2qs_ctx("<p>This is the search tool for this options dialog.</p>" \
			"<p>You can enter a search term either in your native " \
			"language or in english and press the button on the right. " \
			"The pages that contain some options related to the " \
			"search term will be highlighted and you will be able " \
			"to quickly find them.</p><p>Try \"nickname\" for example.</p>","options");
	KviTalToolTip::add(m_pSearchLineEdit,szTip);
	KviTalToolTip::add(m_pSearchButton,szTip);

	vbox = new KviTalVBox(spl);
	vbox->setSpacing(2);
	vbox->setMargin(3);

	m_pCategoryLabel = new QLabel("<b>&nbsp;</b>",vbox);
	m_pCategoryLabel->setObjectName("labgeneraloptions");

	QFrame * f = new QFrame(vbox);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);

	// Widget stack
	m_pWidgetStack = new QStackedWidget(vbox);
	vbox->setStretchFactor(m_pWidgetStack,1);

	// First widget visible
	m_pFrontWidget = new KviGeneralOptionsFrontWidget(m_pWidgetStack,szFrontText);
	m_pWidgetStack->addWidget(m_pFrontWidget);

	int idx = m_pWidgetStack->indexOf(m_pFrontWidget);
	m_pWidgetStack->widget(idx)->raise();

	//  Ok,Cancel,Help
	QPushButton * b = new QPushButton(__tr2qs_ctx("&OK","options"),this);
	b->setObjectName("btnok");
	KviTalToolTip::add(b,__tr2qs_ctx("Close this dialog, accepting all changes.","options"));
	connect(b,SIGNAL(clicked()),this,SLOT(okClicked()));
	//b->setMinimumWidth(whatIsThisWidth);
	b->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g1->addWidget(b,1,2);

	b = new QPushButton(__tr2qs_ctx("&Apply","options"),this);
	b->setObjectName("btnapply");
	KviTalToolTip::add(b,__tr2qs_ctx("Commit all changes immediately.","options"));
	connect(b,SIGNAL(clicked()),this,SLOT(applyClicked()));
	b->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g1->addWidget(b,1,3);

	b = new QPushButton(__tr2qs_ctx("Cancel","options"),this);
	b->setObjectName("btncancel");
	KviTalToolTip::add(b,__tr2qs_ctx("Close this dialog, discarding all changes.","options"));
	b->setDefault(true);
	//b->setMinimumWidth(whatIsThisWidth);
	connect(b,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	b->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
	g1->addWidget(b,1,4);

	g1->setRowStretch(0,1);
	g1->setColumnStretch(1,1);


	fillTreeWidget(0,g_pOptionsInstanceManager->instanceEntryTree(),szGroup);

	if(!parent())
	{
		if(KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry).y() < 5)
		{
			KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry).setY(5);
		}
		//setGeometry(KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry));
		resize(KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry).width(),
			KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry).height());
		move(KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry).x(),
			KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry).y());
	}

	new QShortcut(Qt::Key_Escape,this,SLOT(close()));
}

KviOptionsDialog::~KviOptionsDialog()
{
	if(!parent())KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry) = QRect(pos().x(),pos().y(),
			size().width(),size().height());
	if(g_pOptionsDialogDict)
		g_pOptionsDialogDict->remove(m_szGroup);
}

void KviOptionsDialog::showEvent(QShowEvent *e)
{
	QRect r = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());

	int w = r.width();
	int h = r.height();

	int ww = width();
	int wh = height();

	if(w < 800)
	{
		// 640x480
		if(ww < 630)ww = 630;
	} else {
		if(ww < 770)ww = 770;
	}

	setGeometry((w - ww) / 2,(h - wh) / 2,ww,wh);

	QDialog::showEvent(e);
}

void KviOptionsDialog::searchLineEditTextChanged(const QString &)
{
	QString txt = m_pSearchLineEdit->text().trimmed();
	m_pSearchButton->setEnabled(txt.length() > 0);
}
bool KviOptionsDialog::searchInSelectors(KviOptionsWidget *pOptionsWidget,const QStringList &lKeywords)
{
	KviPointerList<KviSelectorInterface> *selectors=pOptionsWidget->selectors();
	bool bFoundSomethingHere=false;
	if(selectors->count()>0)
	{
		for(unsigned int i=0;i<selectors->count();i++)
		{
			QString szText=selectors->at(i)->textForSearch();
			QWidget* pWidget=selectors->at(i)->widgetToHighlight();
			if (!pWidget) continue;
			QString szTmp=pWidget->toolTip();
			szTmp=szTmp.replace(QRegExp("<[^<>]+>"),"");
			szText.append(szTmp);
			if(!szText.isEmpty())
			{
				bool bOk = true;
				for(int j=0;j<lKeywords.count();j++)
				{
					if(szText.indexOf(lKeywords.at(j),0,Qt::CaseInsensitive) == -1)
					{
						bOk = false;
						break;
					}
				}
				if(bOk)
				{
					bFoundSomethingHere = true;
				}
				QFont font = pWidget->font();
				font.setBold(bOk);
				font.setUnderline(bOk);
				pWidget->setFont(font);
			}
		}
	}
	return bFoundSomethingHere;
}

bool KviOptionsDialog::recursiveSearch(KviOptionsTreeWidgetItem * pItem,const QStringList &lKeywords)
{
	if(!pItem)return false;
	if(!pItem->m_pOptionsWidget)
	{
		pItem->m_pOptionsWidget = g_pOptionsInstanceManager->getInstance(pItem->m_pInstanceEntry,m_pWidgetStack);
		m_pWidgetStack->addWidget(pItem->m_pOptionsWidget);
	}

	bool bFoundSomethingHere = false;
	KviOptionsWidget *pOptionsWidget=pItem->m_pOptionsWidget;
	QTabWidget *pTab= pOptionsWidget->tabWidget();
	if (pTab)
	{
		for(int i=0;i<pTab->count();i++)
		{
			QString szTxt = pTab->tabText(i);
			if(KviQString::equalCIN(szTxt,">>> ",4))
			{
				szTxt.replace(">>> ","");
				szTxt.replace(" <<<","");
			}
			if (searchInSelectors((KviOptionsWidget*)pTab->widget(i),lKeywords))
			{
				bFoundSomethingHere = true;
				szTxt.insert(0,">>> ");
				szTxt += QString(" <<<");
			}
			pTab->setTabText(i,szTxt);
		}
	}
	else
	{
		if (searchInSelectors(pOptionsWidget,lKeywords))
			bFoundSomethingHere = true;
	}

	if(bFoundSomethingHere)
	{
		pItem->setForeground(0, Qt::yellow);
		pItem->setBackground(0, Qt::red);
	} else {
		pItem->setForeground(0, Qt::black);
		pItem->setBackground(0, Qt::transparent);
	}

	bool bFoundSomethingInside = false;
	int ccount = pItem->childCount();
	if(ccount > 0)
	{
		for(int j=0;j<ccount;j++)
		{
			KviOptionsTreeWidgetItem * pChild = (KviOptionsTreeWidgetItem *)pItem->child(j);
			bool bRet = recursiveSearch(pChild,lKeywords);
			if(bRet)bFoundSomethingInside = true;
		}
	}

	pItem->setSelected(false);
	pItem->setExpanded(bFoundSomethingInside);
	return (bFoundSomethingInside || bFoundSomethingHere);
}

void KviOptionsDialog::search(const QStringList &lKeywords)
{
	m_pTreeWidget->setUpdatesEnabled(false);

	bool bFoundSomethingInside = false;
	int count=m_pTreeWidget->topLevelItemCount();
	for (int i=0;i<count;i++)
	{
		KviOptionsTreeWidgetItem * pChild = (KviOptionsTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		bFoundSomethingInside = recursiveSearch(pChild,lKeywords);
	}
	m_pTreeWidget->setUpdatesEnabled(true);
	m_pTreeWidget->update();
}

void KviOptionsDialog::search(const QString &szKeywords)
{
	QStringList lKeywords = szKeywords.split(" ",QString::SkipEmptyParts);
	search(lKeywords);
}

void KviOptionsDialog::searchClicked()
{
	QString szTxt = m_pSearchLineEdit->text().trimmed();
	if(!szTxt.isEmpty())
		search(szTxt);
}

void KviOptionsDialog::fillTreeWidget(QTreeWidgetItem * p,KviPointerList<KviOptionsWidgetInstanceEntry> * l,const QString &szGroup,bool bNotContainedOnly)
{
	if(!l)return;

	KviOptionsTreeWidgetItem * it;
	KviOptionsWidgetInstanceEntry * e;

	KviPointerList<KviOptionsWidgetInstanceEntry> tmp;
	tmp.setAutoDelete(false);


	for(e = l->first();e;e = l->next())
	{
		// must be in the correct group
		// if we want only containers then well.. must be one
		e->bDoInsert = KviQString::equalCI(szGroup,e->szGroup) && ((!bNotContainedOnly) || e->bIsContainer || e->bIsNotContained);
		KviOptionsWidgetInstanceEntry * ee = tmp.first();
		int idx = 0;
		while(ee)
		{
			if(ee->iPriority >= e->iPriority)break;
			idx++;
			ee = tmp.next();
		}
		tmp.insert(idx,e);
	}

	for(e = tmp.first();e;e = tmp.next())
	{
		if(e->bDoInsert)
		{
			if(p)it = new KviOptionsTreeWidgetItem(p,e);
			else it = new KviOptionsTreeWidgetItem(m_pTreeWidget,e);
			if(!it->m_pOptionsWidget)
			{
				it->m_pOptionsWidget = g_pOptionsInstanceManager->getInstance(it->m_pInstanceEntry,m_pWidgetStack);

				if(it->m_pOptionsWidget)
					m_pWidgetStack->addWidget(it->m_pOptionsWidget);
			}
		} else {
			it = (KviOptionsTreeWidgetItem *)p;
		}

		if(e->pChildList)
		{
			if(e->bIsContainer)
			{
				// it's a container: add only eventual not contained children (containers or explicitly marked as not contained)
				fillTreeWidget(it,e->pChildList,szGroup,true);
			} else {
				// it's not a container, add any children
				fillTreeWidget(it,e->pChildList,szGroup,false);
			}
		}
	}
}

void KviOptionsDialog::treeWidgetItemSelectionChanged(QTreeWidgetItem* it, QTreeWidgetItem *)
{
	if(it)
	{
		QString str = it->text(0);
		QTreeWidgetItem * par = it->parent();

		while(par)
		{
			str.prepend(" :: ");
			str.prepend(par->text(0));
			par = par->parent();
		}
		str.prepend("<b>");
		str += "</b>";

		KviOptionsTreeWidgetItem *i = (KviOptionsTreeWidgetItem *)it;
		if(!i->m_pOptionsWidget)
		{
			i->m_pOptionsWidget = g_pOptionsInstanceManager->getInstance(i->m_pInstanceEntry,m_pWidgetStack);
			m_pWidgetStack->addWidget(i->m_pOptionsWidget);
		}

		int idx = m_pWidgetStack->indexOf(i->m_pOptionsWidget);
		m_pWidgetStack->setCurrentWidget(m_pWidgetStack->widget(idx));
		m_pCategoryLabel->setText(str);
	}
}
/*
KviOptionsTreeWidgetItem * KviOptionsDialog::findItemByPage(KviOptionsTreeWidgetItem *it,KviOptionsWidget * pPage)
{
	if(!it)return 0;
	if(it->m_pOptionsWidget == pPage)return it;

	KviOptionsTreeWidgetItem *i;

	int ccount = it->childCount();

	for(int j=0;j<ccount;j++)
	{
		KviOptionsTreeWidgetItem * i = (KviOptionsTreeWidgetItem *)it->child(j);

		i = findItemByPage(i,pPage);
		if(i)return i;
	}

	// not found in the children tree.. look in the next sibling
	for(int j=0;j<ccount;j++)
	{
		KviOptionsTreeWidgetItem * pChild = (KviOptionsTreeWidgetItem *)pItem->child(j);

		i = findItemByPage(i,pPage);
		if(i)return i;
	}

	i = (KviOptionsTreeWidgetItem *)(it->nextSibling());
	if(i)
	{
		i = findItemByPage(i,pPage);
		if(i)return i;
	}
	return 0;
}
*/
void KviOptionsDialog::pageWantsToSwitchToAdvancedPage(KviOptionsWidget *)
{
	// unused
}

void KviOptionsDialog::applyClicked()
{
	apply(false);
}

void KviOptionsDialog::apply(bool bDialogAboutToClose)
{
	int count=m_pTreeWidget->topLevelItemCount();
	int curTab=-1;
	KviOptionsTreeWidgetItem * it;

	if(!bDialogAboutToClose)
	{
		// bring up the current widget again!
		it = (KviOptionsTreeWidgetItem *)m_pTreeWidget->currentItem();
		//checking if the check of the check is checkable
		if(it)
			if(it->m_pOptionsWidget)
				if(it->m_pOptionsWidget->tabWidget())
					curTab = it->m_pOptionsWidget->tabWidget()->currentIndex();
	}

	for (int i=0;i<count;i++)
	{
		it = (KviOptionsTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		recursiveCommit(it);
	}

	if(!bDialogAboutToClose)
	{
		// bring up the current widget again!
		it = (KviOptionsTreeWidgetItem *)m_pTreeWidget->currentItem();
		if(it)
		{
			treeWidgetItemSelectionChanged(it, 0);
			if(curTab > 0 && it->m_pOptionsWidget)
				if(it->m_pOptionsWidget->tabWidget())
					it->m_pOptionsWidget->tabWidget()->setCurrentIndex(curTab);
		}
	}

	g_pApp->saveConfiguration();

}

void KviOptionsDialog::okClicked()
{
	apply(true);
	deleteLater();
}

void KviOptionsDialog::cancelClicked()
{
	deleteLater();
}

void KviOptionsDialog::closeEvent(QCloseEvent *e)
{
	e->ignore();
	deleteLater();
}

void KviOptionsDialog::recursiveCommit(KviOptionsTreeWidgetItem *it)
{
	// First commit the children
	if(!it) return;
	int count=it->childCount();
	for (int i=0;i<count;i++)
	{
		KviOptionsTreeWidgetItem * pChild = (KviOptionsTreeWidgetItem *)it->child(i);
		recursiveCommit(pChild);
	}

	// then the parents
	if(it->m_pOptionsWidget)
	{
		it->m_pOptionsWidget->commit();
		delete it->m_pOptionsWidget;
		it->m_pOptionsWidget = 0;
	}
	//refreshTreeWidget(); // <-- this tends to jump into infinite recursion
}

void  KviOptionsDialog::keyPressEvent( QKeyEvent * e )
{
	if(e->key()==Qt::Key_Return)
		e->accept();
	else
		e->ignore();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_dialog.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
