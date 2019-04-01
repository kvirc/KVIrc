//=============================================================================
//
//   File : OptionsDialog.cpp
//   Created on Fri 10 Feb 2006 18:52:18 by Szymon Stefanek
//   Based on dlg_options.cpp: Tue Jun 11 2000 02:39:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "OptionsDialog.h"

#include "KviApplication.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviIconManager.h"
#include "KviMainWindow.h"
#include "KviModule.h"
#include "KviPointerHashTable.h"
#include "KviTalVBox.h"
#include "KviTalToolTip.h"
#include "KviTalGroupBox.h"

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
#include <QMenu>

//FIXME: some headers must be hidden

//extern KviModule * g_pOptionsModule;
extern KviPointerHashTable<QString, OptionsDialog> * g_pOptionsDialogDict;

extern KVIRC_API KviApplication * g_pApp;
extern OptionsInstanceManager * g_pOptionsInstanceManager;

OptionsDialogFrontWidget::OptionsDialogFrontWidget(QWidget * parent, const QString & szText)
    : KviOptionsWidget(parent)
{
	setObjectName("general_options_front_widget");
	createLayout();
	QLabel * l = new QLabel(szText, this);
	l->setWordWrap(true);
	l->setAlignment(Qt::AlignTop);
	layout()->addWidget(l, 0, 0);
}

OptionsDialogFrontWidget::~OptionsDialogFrontWidget()
    = default;

OptionsDialogTreeWidgetItem::OptionsDialogTreeWidgetItem(QTreeWidget * parent, OptionsWidgetInstanceEntry * e)
    : QTreeWidgetItem()
{
	m_pInstanceEntry = e;
	m_pOptionsWidget = nullptr;
	setText(0, e->szName);
	setIcon(0, *(g_pIconManager->getSmallIcon(e->eIcon)));
	parent->insertTopLevelItem(0, this);
}

OptionsDialogTreeWidgetItem::OptionsDialogTreeWidgetItem(QTreeWidgetItem * parent, OptionsWidgetInstanceEntry * e)
    : QTreeWidgetItem()
{
	m_pInstanceEntry = e;
	m_pOptionsWidget = nullptr;
	setText(0, e->szName);
	setIcon(0, *(g_pIconManager->getSmallIcon(e->eIcon)));
	parent->insertChild(0, this);
}

OptionsDialogTreeWidgetItem::~OptionsDialogTreeWidgetItem()
    = default;

OptionsDialog::OptionsDialog(QWidget * par, const QString & szGroup, bool bModal)
    : QDialog(par)
{
	setObjectName("general_options_dialog");

	setWindowIcon(*(g_pIconManager->getSmallIcon(KviIconManager::KVIrc)));

	m_szGroup = szGroup;

	QString szDialogTitle;

	if(szGroup.isEmpty() || KviQString::equalCI(szGroup, "general"))
	{
		szDialogTitle = __tr2qs_ctx("General Preferences", "options");
	}
	else if(KviQString::equalCI(szGroup, "theme"))
	{
		szDialogTitle = __tr2qs_ctx("Theme Preferences", "options");
	}
	else
	{
		szDialogTitle = __tr2qs_ctx("KVIrc Preferences", "options");
	}

	QString szDialog = __tr2qs_ctx("This dialog contains a set of KVIrc settings.<br> Use the icons "
	                               "on the left to navigate through the option pages. The text box in the "
	                               "bottom left corner is a small search engine. It will highlight the "
	                               "pages that contain options related to the search term you have entered.",
	    "options");

	QString szInfoTips;
	szInfoTips = __tr2qs_ctx("Many settings have tooltips that can be shown by holding "
	                         "the cursor over their label for a few seconds.",
	    "options");

	QString szOkCancelButtons = __tr2qs_ctx("When you have finished, click \"<b>OK</b>\" to accept your changes "
	                                        "or \"<b>Cancel</b>\" to discard them. Clicking \"<b>Apply</b>\" will commit your "
	                                        "changes without closing the window.",
	    "options");

	// clang-format off
	QString szFrontText = QString(
			"<table width=\"100%\" height=\"100%\" valign=\"top\" align=\"center\" cellpadding=\"4\">" \
				"<tr>" \
					"<td bgcolor=\"#303030\" valign=\"top\">" \
						"<center><h1><font color=\"#FFFFFF\">%1</font></h1></center>" \
					"</td>" \
				"</tr>" \
				"<tr>" \
					"<td valign=\"bottom\">" \
						"<p>" \
							"%2" \
						"</p>" \
						"<br style=\"line-height:5px;\"/>" \
						"<p>" \
							"%3" \
						"</p>" \
						"<br style=\"line-height:5px;\"/>" \
						"<p>" \
							"%4" \
						"</p>" \
					"</td>" \
				"</tr>" \
			"</table>"
		).arg(szDialogTitle, szDialog, szInfoTips, szOkCancelButtons);
	// clang-format on

	QString szCaption = szDialogTitle + " - KVIrc";
	setWindowTitle(szCaption);

	QGridLayout * g1 = new QGridLayout(this);
	QSplitter * spl = new QSplitter(Qt::Horizontal, this);
	spl->setChildrenCollapsible(false);

	g1->addWidget(spl, 0, 0, 1, 5);

	KviTalVBox * vbox = new KviTalVBox(spl);
	vbox->setSpacing(2);
	vbox->setMargin(3);

	// Controlling list view
	m_pTreeWidget = new QTreeWidget(vbox);

	m_pTreeWidget->header()->hide();
	m_pTreeWidget->setRootIsDecorated(true);
	m_pTreeWidget->setSortingEnabled(false);
	m_pTreeWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	m_pTreeWidget->setCurrentItem(nullptr);

	connect(m_pTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(treeWidgetItemSelectionChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

	KviTalHBox * hbox = new KviTalHBox(vbox);
	hbox->setSpacing(2);
	hbox->setMargin(3);

	m_pSearchLineEdit = new QLineEdit(hbox);
	connect(m_pSearchLineEdit, SIGNAL(textEdited(const QString &)), this, SLOT(searchClicked()));
	m_pSearchButton = new QToolButton(hbox);
	m_pSearchButton->setIconSize(QSize(16, 16));
	m_pSearchButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Search)));
	connect(m_pSearchButton, SIGNAL(clicked()), this, SLOT(searchClicked()));
	connect(m_pSearchLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(searchLineEditTextChanged(const QString &)));

	QString szTip = __tr2qs_ctx("<p>This is the search tool for this options dialog.</p>"
	                            "<p>You can enter a search term either in your native "
	                            "language or in English and press the button on the right. "
	                            "The pages that contain some options related to the "
	                            "search term will be highlighted and you will be able "
	                            "to quickly find them.</p><p>Try \"nickname\" for example.</p>",
	    "options");
	KviTalToolTip::add(m_pSearchLineEdit, szTip);
	KviTalToolTip::add(m_pSearchButton, szTip);

	vbox = new KviTalVBox(spl);
	vbox->setSpacing(2);
	vbox->setMargin(3);

	m_pCategoryLabel = new QLabel("<b>&nbsp;</b>", vbox);
	m_pCategoryLabel->setObjectName("labgeneraloptions");

	QFrame * f = new QFrame(vbox);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);

	// Widget stack
	m_pWidgetStack = new QStackedWidget(vbox);
	vbox->setStretchFactor(m_pWidgetStack, 1);

	// First widget visible
	m_pFrontWidget = new OptionsDialogFrontWidget(m_pWidgetStack, szFrontText);
	m_pWidgetStack->addWidget(m_pFrontWidget);

	int idx = m_pWidgetStack->indexOf(m_pFrontWidget);
	m_pWidgetStack->widget(idx)->raise();

	//  Ok,Cancel,Help
	QPushButton * b = new QPushButton(__tr2qs_ctx("&OK", "options"), this);
	b->setObjectName("btnok");
	KviTalToolTip::add(b, __tr2qs_ctx("Close this dialog, accepting all changes.", "options"));
	connect(b, SIGNAL(clicked()), this, SLOT(okClicked()));
	//b->setMinimumWidth(whatIsThisWidth);
	b->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	g1->addWidget(b, 1, 2);

	b = new QPushButton(__tr2qs_ctx("&Apply", "options"), this);
	b->setObjectName("btnapply");
	KviTalToolTip::add(b, __tr2qs_ctx("Commit all changes immediately.", "options"));
	connect(b, SIGNAL(clicked()), this, SLOT(applyClicked()));
	b->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	g1->addWidget(b, 1, 3);

	b = new QPushButton(__tr2qs_ctx("Cancel", "options"), this);
	b->setObjectName("btncancel");
	KviTalToolTip::add(b, __tr2qs_ctx("Close this dialog, discarding all changes.", "options"));
	b->setDefault(true);
	//b->setMinimumWidth(whatIsThisWidth);
	connect(b, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	b->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)));
	g1->addWidget(b, 1, 4);

	g1->setRowStretch(0, 1);
	g1->setColumnStretch(1, 1);

	fillTreeWidget(nullptr, g_pOptionsInstanceManager->instanceEntryTree(), szGroup);

	if(!parent())
	{
		if(KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry).y() < 5)
			KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry)
			    .setY(5);

		//setGeometry(KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry));
		resize(KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry).width(),
		    KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry).height());
		move(KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry).x(),
		    KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry).y());
	}

	new QShortcut(Qt::Key_Escape, this, SLOT(close()));

	if(bModal)
		setWindowModality(par ? Qt::WindowModal : Qt::ApplicationModal);
}

OptionsDialog::~OptionsDialog()
{
	if(!parent())
		KVI_OPTION_RECT(KviOption_rectGeneralOptionsDialogGeometry) = QRect(pos().x(), pos().y(), size().width(), size().height());
	if(g_pOptionsDialogDict)
		g_pOptionsDialogDict->remove(m_szGroup);
}

void OptionsDialog::showEvent(QShowEvent * e)
{
	// setup a minimum size and move to the screen the main kvirc window is
	QRect r = g_pApp->desktop()->screenGeometry(g_pMainWindow);

	int w = r.width();
	int h = r.height();

	int ww = width();
	int wh = height();

	if(w < 800)
	{
		// 640x480
		if(ww < 630)
			ww = 630;
	}
	else
	{
		if(ww < 770)
			ww = 770;
	}

	setGeometry(r.x() + ((w - ww) / 2), r.y() + ((h - wh) / 2), ww, wh);

	QWidget::showEvent(e);
}

void OptionsDialog::searchLineEditTextChanged(const QString &)
{
	QString txt = m_pSearchLineEdit->text().trimmed();
	m_pSearchButton->setEnabled(txt.length() > 0);
}
bool OptionsDialog::searchInSelectors(KviOptionsWidget * pOptionsWidget, const QStringList & lKeywords)
{
	KviPointerList<KviSelectorInterface> * selectors = pOptionsWidget->selectors();
	bool bCleaningUp = lKeywords.isEmpty();
	bool bFoundSomethingHere = false;
	if(selectors->count() > 0)
	{
		for(unsigned int i = 0; i < selectors->count(); i++)
		{
			QString szText = selectors->at(i)->textForSearch();
			QWidget * pWidget = selectors->at(i)->widgetToHighlight();
			if(!pWidget)
				continue;
			if(bCleaningUp)
			{
				QFont font = pWidget->font();
				font.setBold(false);
				font.setUnderline(false);
				pWidget->setFont(font);
			}
			else
			{
				QString szTmp = pWidget->toolTip();
				szTmp = szTmp.replace(QRegExp("<[^<>]+>"), "");
				szText.append(szTmp);
				if(!szText.isEmpty())
				{
					bool bOk = true;
					for(int j = 0; j < lKeywords.count(); j++)
					{
						if(szText.indexOf(lKeywords.at(j), 0, Qt::CaseInsensitive) == -1)
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
	}
	return bFoundSomethingHere;
}

bool OptionsDialog::recursiveSearch(OptionsDialogTreeWidgetItem * pItem, const QStringList & lKeywords)
{
	if(!pItem)
		return false;
	if(!pItem->m_pOptionsWidget)
	{
		pItem->m_pOptionsWidget = g_pOptionsInstanceManager->getInstance(pItem->m_pInstanceEntry, m_pWidgetStack);
		m_pWidgetStack->addWidget(pItem->m_pOptionsWidget);
	}

	bool bCleaningUp = lKeywords.isEmpty();
	bool bFoundSomethingHere = false;
	KviOptionsWidget * pOptionsWidget = pItem->m_pOptionsWidget;
	QTabWidget * pTab = pOptionsWidget->tabWidget();
	if(pTab)
	{
		for(int i = 0; i < pTab->count(); i++)
		{
			QString szTxt = pTab->tabText(i);
			if(bCleaningUp || KviQString::equalCIN(szTxt, ">>> ", 4))
			{
				szTxt.replace(">>> ", "");
				szTxt.replace(" <<<", "");
			}
			if(searchInSelectors((KviOptionsWidget *)pTab->widget(i), lKeywords) && !bCleaningUp)
			{
				bFoundSomethingHere = true;
				szTxt.insert(0, ">>> ");
				szTxt += QString(" <<<");
			}
			pTab->setTabText(i, szTxt);
		}
	}
	else
	{
		if(searchInSelectors(pOptionsWidget, lKeywords) && !bCleaningUp)
			bFoundSomethingHere = true;
	}

	if (!bCleaningUp)
	{
		QStringList szInstanceKeywords = pItem->m_pInstanceEntry->szKeywords.split(QChar(','));
		// debug all the "search keywords" for each entry in the options tree
		// qDebug("OPT %s",pItem->m_pInstanceEntry->szKeywords.toUtf8().data());

		for(int i = 0; i < szInstanceKeywords.count() && !bFoundSomethingHere; i++)
			for(int j = 0; j < lKeywords.count() && !bFoundSomethingHere; j++)
				if(szInstanceKeywords.at(i).contains(lKeywords.at(j), Qt::CaseInsensitive))
					bFoundSomethingHere = true;
	}

	if(bFoundSomethingHere)
	{
		pItem->setForeground(0, Qt::yellow);
		pItem->setBackground(0, Qt::red);
	}
	else
	{
		pItem->setForeground(0, Qt::black);
		pItem->setBackground(0, Qt::transparent);
	}

	bool bFoundSomethingInside = false;
	int ccount = pItem->childCount();
	if(ccount > 0)
	{
		for(int j = 0; j < ccount; j++)
		{
			OptionsDialogTreeWidgetItem * pChild = (OptionsDialogTreeWidgetItem *)pItem->child(j);
			bool bRet = recursiveSearch(pChild, lKeywords) && !bCleaningUp;
			if(bRet)
				bFoundSomethingInside = true;
		}
	}

	pItem->setSelected(false);
	pItem->setExpanded(bFoundSomethingInside);
	return (bFoundSomethingInside || bFoundSomethingHere);
}

void OptionsDialog::search(const QStringList & lKeywords)
{
	m_pTreeWidget->setUpdatesEnabled(false);

	int count = m_pTreeWidget->topLevelItemCount();
	for(int i = 0; i < count; i++)
	{
		OptionsDialogTreeWidgetItem * pChild = (OptionsDialogTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		recursiveSearch(pChild, lKeywords);
	}
	m_pTreeWidget->setUpdatesEnabled(true);
	m_pTreeWidget->update();
}

void OptionsDialog::search(const QString & szKeywords)
{
	QStringList lKeywords = szKeywords.split(" ", QString::SkipEmptyParts);
	search(lKeywords);
}

void OptionsDialog::clearSearch()
{
	m_pTreeWidget->setUpdatesEnabled(false);

	QTreeWidgetItemIterator it(m_pTreeWidget);
	while (*it) {
		recursiveSearch(((OptionsDialogTreeWidgetItem *)* it), QStringList());
		++it;
	}

	m_pTreeWidget->setUpdatesEnabled(true);
	m_pTreeWidget->update();
}

void OptionsDialog::searchClicked()
{
	QString szTxt = m_pSearchLineEdit->text().trimmed();
	if(szTxt.length() > 1)
		search(szTxt);
	else
		clearSearch();
}

void OptionsDialog::fillTreeWidget(QTreeWidgetItem * p, KviPointerList<OptionsWidgetInstanceEntry> * l, const QString & szGroup, bool bNotContainedOnly)
{
	if(!l)
		return;

	OptionsDialogTreeWidgetItem * it;
	OptionsWidgetInstanceEntry * e;

	KviPointerList<OptionsWidgetInstanceEntry> tmp;
	tmp.setAutoDelete(false);

	for(e = l->first(); e; e = l->next())
	{
		// must be in the correct group
		// if we want only containers then well.. must be one
		e->bDoInsert = KviQString::equalCI(szGroup, e->szGroup) && ((!bNotContainedOnly) || e->bIsContainer || e->bIsNotContained);
		OptionsWidgetInstanceEntry * ee = tmp.first();
		int idx = 0;
		while(ee)
		{
			if(ee->iPriority >= e->iPriority)
				break;
			idx++;
			ee = tmp.next();
		}
		tmp.insert(idx, e);
	}

	for(e = tmp.first(); e; e = tmp.next())
	{
		if(e->bDoInsert)
		{
			if(p)
				it = new OptionsDialogTreeWidgetItem(p, e);
			else
				it = new OptionsDialogTreeWidgetItem(m_pTreeWidget, e);
			if(!it->m_pOptionsWidget)
			{
				it->m_pOptionsWidget = g_pOptionsInstanceManager->getInstance(it->m_pInstanceEntry, m_pWidgetStack);

				if(it->m_pOptionsWidget)
					m_pWidgetStack->addWidget(it->m_pOptionsWidget);
			}
		}
		else
		{
			it = (OptionsDialogTreeWidgetItem *)p;
		}

		if(e->pChildList)
		{
			if(e->bIsContainer)
			{
				// it's a container: add only eventual not contained children (containers or explicitly marked as not contained)
				fillTreeWidget(it, e->pChildList, szGroup, true);
			}
			else
			{
				// it's not a container, add any children
				fillTreeWidget(it, e->pChildList, szGroup, false);
			}
		}
	}
}

void OptionsDialog::treeWidgetItemSelectionChanged(QTreeWidgetItem * it, QTreeWidgetItem *)
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

		OptionsDialogTreeWidgetItem * i = (OptionsDialogTreeWidgetItem *)it;
		if(!i->m_pOptionsWidget)
		{
			i->m_pOptionsWidget = g_pOptionsInstanceManager->getInstance(i->m_pInstanceEntry, m_pWidgetStack);
			m_pWidgetStack->addWidget(i->m_pOptionsWidget);
		}

		//int idx = m_pWidgetStack->indexOf(i->m_pOptionsWidget);
		m_pWidgetStack->setCurrentWidget(i->m_pOptionsWidget);
		m_pCategoryLabel->setText(str);
	}
}

void OptionsDialog::pageWantsToSwitchToAdvancedPage(KviOptionsWidget *)
{
	// unused
}

void OptionsDialog::applyClicked()
{
	apply(false);
}

void OptionsDialog::apply(bool bDialogAboutToClose)
{
	int count = m_pTreeWidget->topLevelItemCount();
	int curTab = -1;
	OptionsDialogTreeWidgetItem * it;

	if(!bDialogAboutToClose)
	{
		// bring up the current widget again!
		it = (OptionsDialogTreeWidgetItem *)m_pTreeWidget->currentItem();
		//checking if the check of the check is checkable
		if(it)
			if(it->m_pOptionsWidget)
				if(it->m_pOptionsWidget->tabWidget())
					curTab = it->m_pOptionsWidget->tabWidget()->currentIndex();
	}

	for(int i = 0; i < count; i++)
	{
		it = (OptionsDialogTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		recursiveCommit(it);
	}

	if(!bDialogAboutToClose)
	{
		// bring up the current widget again!
		it = (OptionsDialogTreeWidgetItem *)m_pTreeWidget->currentItem();
		if(it)
		{
			treeWidgetItemSelectionChanged(it, nullptr);
			if(curTab > 0 && it->m_pOptionsWidget)
				if(it->m_pOptionsWidget->tabWidget())
					it->m_pOptionsWidget->tabWidget()->setCurrentIndex(curTab);
		}
	}

	g_pApp->saveConfiguration();
}

void OptionsDialog::okClicked()
{
	apply(true);
	deleteLater();
}

void OptionsDialog::cancelClicked()
{
	deleteLater();
}

void OptionsDialog::closeEvent(QCloseEvent * e)
{
	e->ignore();
	deleteLater();
}

void OptionsDialog::recursiveCommit(OptionsDialogTreeWidgetItem * it)
{
	// First commit the children
	if(!it)
		return;
	int count = it->childCount();
	for(int i = 0; i < count; i++)
	{
		OptionsDialogTreeWidgetItem * pChild = (OptionsDialogTreeWidgetItem *)it->child(i);
		recursiveCommit(pChild);
	}

	// then the parents
	if(it->m_pOptionsWidget)
	{
		it->m_pOptionsWidget->commit();
		delete it->m_pOptionsWidget;
		it->m_pOptionsWidget = nullptr;
	}
	//refreshTreeWidget(); // <-- this tends to jump into infinite recursion
}

void OptionsDialog::keyPressEvent(QKeyEvent * e)
{
	if(e->key() == Qt::Key_Return)
		e->accept();
	else
		e->ignore();
}
