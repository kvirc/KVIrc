//=============================================================================
//
//   File : dialog.cpp
//   Created on Fri 10 Feb 2006 18:52:18 by Szymon Stefanek
//   Based on dlg_options.cpp: Tue Jun 11 2000 02:39:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2000-2006 Szymon Stefanek <pragma at kvirc dot net>
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

#include "dialog.h"

#include "kvi_app.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_module.h"
#include "kvi_styled_controls.h"
#include "kvi_ptrdict.h"
#include <qlayout.h>
#include "kvi_accel.h"
#include <qlabel.h>
#include "kvi_tal_vbox.h"
#include <qsplitter.h>
#include "kvi_tal_widgetstack.h"
#include <qpushbutton.h>
#include "kvi_tal_tooltip.h"

#ifdef COMPILE_USE_QT4
	#include <q3header.h>
#else
	#include <qheader.h>
	#include <qobjectlist.h>
#endif
#include "kvi_tal_popupmenu.h"
#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <kvi_tal_groupbox.h>
#include <qpainter.h>
#include <qfont.h>
#include <qevent.h>

#include <iostream>

//extern KviModule * g_pOptionsModule;
extern KviDict<KviOptionsDialog> * g_pOptionsDialogDict;

extern KVIRC_API KviApp * g_pApp;
extern KviOptionsInstanceManager * g_pOptionsInstanceManager;

KviGeneralOptionsFrontWidget::KviGeneralOptionsFrontWidget(QWidget *parent,const QString &szText)
:KviOptionsWidget(parent,"general_options_front_widget")
{
	createLayout(1,1);
	QLabel * l = new QLabel(szText,this);
#ifdef COMPILE_USE_QT4
	l->setWordWrap(true);
#endif
	l->setAlignment(Qt::AlignTop);
	layout()->addWidget(l,0,0);
}

KviGeneralOptionsFrontWidget::~KviGeneralOptionsFrontWidget()
{
}




KviOptionsListViewItem::KviOptionsListViewItem(KviTalListView *parent,KviOptionsWidgetInstanceEntry * e)
:KviTalListViewItem(parent,e->szName)
{
	m_pInstanceEntry = e;
	m_pOptionsWidget = 0;
	m_bHighlighted = false;
	setPixmap(0,*(g_pIconManager->getSmallIcon(e->iIcon)));
}

KviOptionsListViewItem::KviOptionsListViewItem(KviTalListViewItem *parent,KviOptionsWidgetInstanceEntry * e)
:KviTalListViewItem(parent,e->szName)
{
	m_pInstanceEntry = e;
	m_pOptionsWidget = 0;
	m_bHighlighted = false;
	setPixmap(0,*(g_pIconManager->getSmallIcon(e->iIcon)));
}

KviOptionsListViewItem::~KviOptionsListViewItem()
{
}

void KviOptionsListViewItem::paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align)
{
	if(m_bHighlighted)
	{
		QColorGroup tmp(cg);
		tmp.setColor(QColorGroup::Base,Qt::red);
		tmp.setColor(QColorGroup::Background,Qt::red);
		tmp.setColor(QColorGroup::Text,Qt::yellow);
		KviTalListViewItem::paintCell(p,tmp,column,width,align);
	} else {
		KviTalListViewItem::paintCell(p,cg,column,width,align);
	}
}


KviOptionsDialog::KviOptionsDialog(QWidget * par,const QString &szGroup)
: QDialog(par,"general_options_dialog")
{
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_KVIRC)));

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
#ifdef COMPILE_INFO_TIPS
	szInfoTips = __tr2qs_ctx("Many settings have tooltips that can be shown by holding " \
							"the cursor over their label for a few seconds.","options"); 
#else
	szInfoTips = "";
#endif
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
		).arg(szDialogTitle).arg(szDialog).arg(szInfoTips).arg(szOkCancelButtons);

	QString szCaption = szDialogTitle + " - KVIrc";
	setCaption(szCaption);

	QGridLayout * g1 = new QGridLayout(this,2,5,8,8);
	QSplitter * spl = new QSplitter(Qt::Horizontal,this);

	g1->addMultiCellWidget(spl,0,0,0,4);

	KviTalVBox * vbox = new KviTalVBox(spl);
	vbox->setSpacing(2);
	vbox->setMargin(0);

	// Controlling list view
	m_pListView = new KviTalListView(vbox);
	m_pListView->addColumn("");
	m_pListView->header()->hide();
	m_pListView->setRootIsDecorated(true);
	m_pListView->setSorting(-1);
	connect(m_pListView,SIGNAL(selectionChanged(KviTalListViewItem *)),this,SLOT(listViewItemSelectionChanged(KviTalListViewItem *)));

	KviTalHBox * hbox = new KviTalHBox(vbox);
	vbox->setSpacing(2);
	vbox->setMargin(0);

	m_pSearchLineEdit = new QLineEdit(hbox);
	connect(m_pSearchLineEdit,SIGNAL(returnPressed()),this,SLOT(searchClicked()));
	m_pSearchButton = new KviStyledToolButton(hbox);
	m_pSearchButton->setUsesBigPixmap(false);
	m_pSearchButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SEARCH)));
	connect(m_pSearchButton,SIGNAL(clicked()),this,SLOT(searchClicked()));
	connect(m_pSearchLineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(searchLineEditTextChanged(const QString &)));

#ifdef COMPILE_INFO_TIPS
	QString szTip = __tr2qs_ctx("<p>This is the search tool for this options dialog.</p>" \
								"<p>You can enter a search term either in your native " \
								"language or in english and press the button on the right. " \
								"The pages that contain some options related to the " \
								"search term will be highlighted and you will be able " \
								"to quickly find them.</p><p>Try \"nickname\" for example.</p>","options");
	KviTalToolTip::add(m_pSearchLineEdit,szTip);
	KviTalToolTip::add(m_pSearchButton,szTip);
#endif

	vbox = new KviTalVBox(spl);
	vbox->setSpacing(2);
	vbox->setMargin(0);

	m_pCategoryLabel = new QLabel("<b>&nbsp;</b>",vbox,"labgeneraloptions");
	//m_pCategoryLabel->setMargin(3);

	QFrame * f = new QFrame(vbox);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);

	// Widget stack
	m_pWidgetStack = new KviTalWidgetStack(vbox);
	vbox->setStretchFactor(m_pWidgetStack,1);

	// First widget visible
	m_pFrontWidget = new KviGeneralOptionsFrontWidget(m_pWidgetStack,szFrontText);
	m_pWidgetStack->addWidget(m_pFrontWidget,0);
	m_pWidgetStack->raiseWidget(m_pFrontWidget);

//  Ok,Cancel,Help

	QPushButton * b = new QPushButton(__tr2qs_ctx("&OK","options"),this,"btnok");
	KviTalToolTip::add(b,__tr2qs_ctx("Close this dialog, accepting all changes.","options"));
	connect(b,SIGNAL(clicked()),this,SLOT(okClicked()));
	//b->setMinimumWidth(whatIsThisWidth);
	b->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g1->addWidget(b,1,2);	

	b = new QPushButton(__tr2qs_ctx("&Apply","options"),this,"btnapply");
	KviTalToolTip::add(b,__tr2qs_ctx("Commit all changes immediately.","options"));
	connect(b,SIGNAL(clicked()),this,SLOT(applyClicked()));
	b->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g1->addWidget(b,1,3);

	b = new QPushButton(__tr2qs_ctx("Cancel","options"),this,"btncancel");
	KviTalToolTip::add(b,__tr2qs_ctx("Close this dialog, discarding all changes.","options"));
	b->setDefault(true);
	//b->setMinimumWidth(whatIsThisWidth);
	connect(b,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	b->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
	g1->addWidget(b,1,4);

	g1->setRowStretch(0,1);
	g1->setColStretch(1,1);


	fillListView(0,g_pOptionsInstanceManager->instanceEntryTree(),szGroup);
	
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

	KviAccel *a = new KviAccel( this );
        a->connectItem( a->insertItem(Qt::Key_Escape), this,SLOT(close()) );
	
}

KviOptionsDialog::~KviOptionsDialog()
{
	if(!parent())KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry) = QRect(pos().x(),pos().y(),
			size().width(),size().height());
	if(g_pOptionsDialogDict)
		g_pOptionsDialogDict->remove(m_szGroup);
}


void KviOptionsDialog::searchLineEditTextChanged(const QString &)
{
	QString txt = m_pSearchLineEdit->text().stripWhiteSpace();
	m_pSearchButton->setEnabled(txt.length() > 0);
}

bool KviOptionsDialog::recursiveSearch(KviOptionsListViewItem * pItem,const QStringList &lKeywords)
{
	debug("recursive search:");
	if(!pItem)return false;

	if(!pItem->m_pOptionsWidget)
	{
		pItem->m_pOptionsWidget = g_pOptionsInstanceManager->getInstance(pItem->m_pInstanceEntry,m_pWidgetStack);
		m_pWidgetStack->addWidget(pItem->m_pOptionsWidget,0);
	}

	bool bFoundSomethingHere = false;
	KviPtrDict<bool> lOptionWidgetsToMark;
	lOptionWidgetsToMark.setAutoDelete(true);
	QTabWidget * pTabWidgetToMark = 0;
	
	QObject * o;
#ifdef COMPILE_USE_QT4
	QObjectList ol = pItem->m_pOptionsWidget->queryList();
	if(ol.count() > 0)
	{
		for(QObjectList::Iterator it = ol.begin();it != ol.end();++it)
		{
			o = *it;
#else
	QObjectList * ol = pItem->m_pOptionsWidget->queryList();
	if(ol)
	{
		QObjectListIt it(*ol);
		while((o = it.current())/* && (!bFoundSomethingHere)*/)
		{
#endif
			QString szText;
			if(o->inherits("QLabel"))szText = ((QLabel *)o)->text();
			else if(o->inherits("QCheckBox"))szText = ((QCheckBox *)o)->text();
			else if(o->inherits("KviTalGroupBox"))szText = ((KviTalGroupBox *)o)->title();
#ifdef COMPILE_INFO_TIPS
			if(o->inherits("QWidget"))
#ifdef COMPILE_USE_QT4
				szText.append(((QWidget *)o)->toolTip());
#else
				szText.append(QToolTip::textFor((QWidget*)o));
#endif
#endif
			if(!szText.isEmpty())
			{
				bool bOk = true;
				for(int j=0;j<lKeywords.count();j++)
				{
#ifdef COMPILE_USE_QT4
					if(szText.find(lKeywords.at(j),0,false) == -1)
#else
					if(szText.find(*(lKeywords.at(j)),0,false) == -1)
#endif
					{
						bOk = false;
						break;
					}
				}
				if(bOk)
				{
					bFoundSomethingHere = true;
				}
				
				if(o->inherits("QWidget"))
				{
					QWidget* pWidget=(QWidget*)o;
					QFont font = pWidget->font();
					font.setBold(bOk);
					font.setUnderline(bOk);
					pWidget->setFont(font);

					// if there is a QTabWidget in the parent chain, signal it in the tab text
					QObject * pParent = pWidget->parent();
					while(pParent)
					{
						if(pParent->inherits("QTabWidget"))
						{
							pTabWidgetToMark = (QTabWidget *)pParent;
							break;
						}
						pParent = pParent->parent();
					}

					if(pTabWidgetToMark)
					{
						// lookup the KviOptionsWidget parent
						pParent = pWidget->parent();
						while(pParent)
						{
							if(pParent->inherits("KviOptionsWidget"))
							{
								bool * pExistingBool = lOptionWidgetsToMark.find(pParent);
								if(pExistingBool)
								{
									if(bOk)
										*pExistingBool = true;
								} else {
									lOptionWidgetsToMark.insert(pParent,new bool(bOk));
								}
								break;
							}
							pParent = pParent->parent();
						}
					}
				}
			}

#ifndef COMPILE_USE_QT4
			++it;
#endif
		}
#ifndef COMPILE_USE_QT4
		delete ol;
#endif
	}

	if(pTabWidgetToMark)
	{
		KviPtrDictIterator<bool> it(lOptionWidgetsToMark);
		while(bool * pBool = it.current())
		{
			KviOptionsWidget * pOptionsWidget = (KviOptionsWidget *)it.currentKey();
			QString szTxt = pTabWidgetToMark->tabLabel(pOptionsWidget);
			if(KviQString::equalCIN(szTxt,">>> ",4))
			{
				szTxt.replace(">>> ","");
				szTxt.replace(" <<<","");
			}
			if(*pBool)
			{
				szTxt.insert(0,">>> ");
				szTxt += QString(" <<<");
			}
			pTabWidgetToMark->setTabLabel(pOptionsWidget,szTxt);
			++it;
		}
	}

	pItem->setHighlighted(bFoundSomethingHere);

	KviOptionsListViewItem * pChild = (KviOptionsListViewItem *)pItem->firstChild();
	bool bFoundSomethingInside = false;
	while(pChild)
	{
		bool bRet = recursiveSearch(pChild,lKeywords);
		if(bRet)bFoundSomethingInside = true;
		pChild = (KviOptionsListViewItem *)(pChild->nextSibling());
	}
	pItem->setSelected(false);
	m_pListView->setOpen(pItem,bFoundSomethingInside);
	return (bFoundSomethingInside || bFoundSomethingHere);
}

void KviOptionsDialog::search(const QStringList &lKeywords)
{
	m_pListView->setUpdatesEnabled(false);

	KviOptionsListViewItem * pChild = (KviOptionsListViewItem *)(m_pListView->firstChild());
	bool bFoundSomethingInside = false;
	while(pChild)
	{
		bFoundSomethingInside = recursiveSearch(pChild,lKeywords);
		pChild = (KviOptionsListViewItem *)(pChild->nextSibling());
	}
	m_pListView->setUpdatesEnabled(true);
	m_pListView->triggerUpdate();
}

void KviOptionsDialog::search(const QString &szKeywords)
{
	QStringList lKeywords = QStringList::split(" ",szKeywords,false);
	search(lKeywords);
}

void KviOptionsDialog::searchClicked()
{
	QString szTxt = m_pSearchLineEdit->text().stripWhiteSpace();
	if(!szTxt.isEmpty())
		search(szTxt);
}

void KviOptionsDialog::fillListView(KviTalListViewItem * p,KviPtrList<KviOptionsWidgetInstanceEntry> * l,const QString &szGroup,bool bNotContainedOnly)
{
	if(!l)return;

	KviOptionsListViewItem * it;
	KviOptionsWidgetInstanceEntry * e;

	KviPtrList<KviOptionsWidgetInstanceEntry> tmp;
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
			if(p)it = new KviOptionsListViewItem(p,e);
			else it = new KviOptionsListViewItem(m_pListView,e);
			if(!it->m_pOptionsWidget)
			{
				it->m_pOptionsWidget = g_pOptionsInstanceManager->getInstance(it->m_pInstanceEntry,m_pWidgetStack);
				m_pWidgetStack->addWidget(it->m_pOptionsWidget,0);
			}
		} else {
			it = (KviOptionsListViewItem *)p;
		}
		
		if(e->pChildList)
		{
			if(e->bIsContainer)
			{
				// it's a container: add only eventual not contained children (containers or explicitly marked as not contained)
				fillListView(it,e->pChildList,szGroup,true);
			} else {
				// it's not a container, add any children
				fillListView(it,e->pChildList,szGroup,false);
			}
		}
	}
}

void KviOptionsDialog::listViewItemSelectionChanged(KviTalListViewItem *it)
{
	if(it)
	{
		QString str = it->text(0);
		KviTalListViewItem * par = it->parent();

		while(par)
		{
			str.prepend(" :: ");
			str.prepend(par->text(0));
			par = par->parent();
		}
		str.prepend("<b>");
		str += "</b>";

		KviOptionsListViewItem *i = (KviOptionsListViewItem *)it;
		if(!i->m_pOptionsWidget)
		{
			i->m_pOptionsWidget = g_pOptionsInstanceManager->getInstance(i->m_pInstanceEntry,m_pWidgetStack);
			m_pWidgetStack->addWidget(i->m_pOptionsWidget,0);
		}

		m_pWidgetStack->raiseWidget(i->m_pOptionsWidget);
		m_pCategoryLabel->setText(str);
	}
}

KviOptionsListViewItem * KviOptionsDialog::findItemByPage(KviOptionsListViewItem *it,KviOptionsWidget * pPage)
{
	if(!it)return 0;
	if(it->m_pOptionsWidget == pPage)return it;

	KviOptionsListViewItem *i;
	// run through the children
	i = (KviOptionsListViewItem *)(it->firstChild());
	if(i)
	{
		i = findItemByPage(i,pPage);
		if(i)return i;
	}
	
	// not found in the children tree.. look in the next sibling
	i = (KviOptionsListViewItem *)(it->nextSibling());
	if(i)
	{
		i = findItemByPage(i,pPage);
		if(i)return i;
	}
	return 0;
}


void KviOptionsDialog::pageWantsToSwitchToAdvancedPage(KviOptionsWidget * pPage)
{
	// unused
}

void KviOptionsDialog::applyClicked()
{
	apply(false);
}

void KviOptionsDialog::apply(bool bDialogAboutToClose)
{

	KviOptionsListViewItem *it = (KviOptionsListViewItem *)m_pListView->firstChild();
	while(it)
	{
		recursiveCommit(it);
		it = (KviOptionsListViewItem *)it->nextSibling();
	}
	
	if(!bDialogAboutToClose)
	{
		// bring up the current widget again!
		it = (KviOptionsListViewItem *)m_pListView->currentItem();
		if(it)listViewItemSelectionChanged(it);
	}

	g_pApp->saveConfiguration();

}

void KviOptionsDialog::okClicked()
{
	apply(true);
	delete this;
}

void KviOptionsDialog::cancelClicked()
{
	delete this;
}

void KviOptionsDialog::closeEvent(QCloseEvent *e)
{
	e->ignore();
	delete this;
}

void KviOptionsDialog::recursiveCommit(KviOptionsListViewItem *it)
{
	// First commit the children
	if(!it) return;
	KviOptionsListViewItem *it2 = (KviOptionsListViewItem *)it->firstChild();
	while(it2)
	{
		recursiveCommit(it2);
		it2 = (KviOptionsListViewItem *)it2->nextSibling();
	}
	// then the parents
	if(it->m_pOptionsWidget)
	{
		it->m_pOptionsWidget->commit();
		delete it->m_pOptionsWidget;
		it->m_pOptionsWidget = 0;
	}
	//refreshListView(); // <-- this tends to jump into infinite recursion
}
void  KviOptionsDialog::keyPressEvent( QKeyEvent * e )
{
	if(e->key()==Qt::Key_Return)
		e->accept();
	else
		e->ignore();
}

#include "m_dialog.moc"
