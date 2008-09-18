//=============================================================================
//
//   File : eventeditor.cpp
//   Creation date : Mon Dec 23 2002 20:28:18 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the linkss of the GNU General Public License
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

#include "eventeditor.h"

#include "kvi_iconmanager.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_imagedialog.h"
#include "kvi_config.h"
#include "kvi_filedialog.h"
#include "kvi_fileutils.h"
#include "kvi_scripteditor.h"
#include "kvi_debug.h"
//#include "kvi_event.h"
#include "kvi_app.h"
#include "kvi_cmdformatter.h"
//#include "kvi_eventhandler.h"
#include "kvi_qstring.h"
#include "kvi_filedialog.h"
#include "kvi_kvs_eventmanager.h"
#include "kvi_tal_vbox.h"
#include "kvi_tal_popupmenu.h"

#include <QMessageBox>
#include <QSplitter>
#include <QLayout>
#include <QToolTip>
#include <QPushButton>
#include <QMouseEvent>

extern KviEventEditorWindow * g_pEventEditorWindow;

KviEventListViewItem::KviEventListViewItem(QTreeWidget * par,unsigned int uEvIdx,const QString &name,const QString &params)
: QTreeWidgetItem(par), m_uEventIdx(uEvIdx), m_szParams(params)
{
	setName(name);
}

void KviEventListViewItem::setName(const QString &szName)
{
	m_szName = szName;
	setText(0,m_szName);
}

KviEventHandlerListViewItem::KviEventHandlerListViewItem(QTreeWidgetItem * par,const QString &name,const QString &buffer,bool bEnabled)
: QTreeWidgetItem(par), m_szBuffer(buffer)
{
	setEnabled(bEnabled); //this updates the icon too
	setName(name);
}

void KviEventHandlerListViewItem::setName(const QString &szName)
{
	m_szName = szName;
	setText(0,m_szName);
}

void KviEventHandlerListViewItem::setEnabled(const bool bEnabled)
{
	m_bEnabled = bEnabled;
	setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(m_bEnabled ? KVI_SMALLICON_HANDLER : KVI_SMALLICON_HANDLERDISABLED))));
}

KviEventEditor::KviEventEditor(QWidget * par)
: QWidget(par)
{
	setObjectName("event_editor");

	QGridLayout * l = new QGridLayout(this);

	QSplitter * spl = new QSplitter(Qt::Horizontal,this);
	spl->setOpaqueResize(false);

	l->addWidget(spl,0,0);

	KviTalVBox * boxi = new KviTalVBox(spl);
	boxi->setSpacing(0);
	boxi->setMargin(0);

	m_pListView = new KviEventEditorListView(boxi);

	QPushButton * pb = new QPushButton(__tr2qs("&Export All To..."),boxi);
	connect(pb,SIGNAL(clicked()),this,SLOT(exportAllEvents()));

	KviTalVBox * box = new KviTalVBox(spl);
	box->setSpacing(0);
	box->setMargin(0);

	m_pNameEditor = new QLineEdit(box);
	m_pNameEditor->setToolTip(__tr2qs("Edit the event handler name."));

	m_pEditor = KviScriptEditor::createInstance(box);
	m_pEditor->setFocus();

	m_bOneTimeSetupDone = false;
	m_pLastEditedItem = 0;
}

KviEventEditor::~KviEventEditor()
{
	KviScriptEditor::destroyInstance(m_pEditor);
}

void KviEventEditor::oneTimeSetup()
{
	if(m_bOneTimeSetupDone)return;
	m_bOneTimeSetupDone = true;

	KviEventListViewItem * it;
	KviEventHandlerListViewItem * ch;

	for(unsigned int i = 0;i < KVI_KVS_NUM_APP_EVENTS;i++)
	{
		KviKvsEvent * e = KviKvsEventManager::instance()->appEvent(i);
		it = new KviEventListViewItem(m_pListView,i,e->name(),e->parameterDescription());
		if(KviPointerList<KviKvsEventHandler> * l = e->handlers())
		{
			for(KviKvsEventHandler * s = l->first();s;s = l->next())
			{
				if(s->type() == KviKvsEventHandler::Script)
				{
					ch = new KviEventHandlerListViewItem(it,((KviKvsScriptEventHandler *)s)->name(),
					((KviKvsScriptEventHandler *)s)->code(),((KviKvsScriptEventHandler *)s)->isEnabled());
				}
			}
			it->setExpanded(true);
		}
		it->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(it->childCount() ? KVI_SMALLICON_EVENT : KVI_SMALLICON_EVENTNOHANDLERS))));
	}

	connect(m_pListView,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
	connect(m_pListView,SIGNAL(rightButtonPressed(QTreeWidgetItem *,QPoint)),
		this,SLOT(itemPressed(QTreeWidgetItem *,QPoint)));

	m_pContextPopup = new KviTalPopupMenu(this);
	m_pListView->sortItems(0,Qt::AscendingOrder);
}

KviEventEditorListView::KviEventEditorListView(QWidget * par)
: QTreeWidget(par)
{
	setColumnCount (1);
	setHeaderLabel(__tr2qs("Event"));
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSortingEnabled(true);
	setRootIsDecorated(true);
	setAnimated(true);
}

void KviEventEditorListView::mousePressEvent (QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
	{
		QTreeWidgetItem *i=itemAt(e->pos());
		if (i) emit rightButtonPressed(i,QCursor::pos());
	}
	QTreeWidget::mousePressEvent(e);
}

void KviEventEditor::itemPressed(QTreeWidgetItem *it,const QPoint &pnt)
{
	__range_valid(m_bOneTimeSetupDone);

	if(it)
	{
		m_pContextPopup->clear();
		if(it->parent())
		{
			QString tmp;
			if(!(((KviEventHandlerListViewItem *)it)->m_bEnabled))
				m_pContextPopup->insertItem(
					*(g_pIconManager->getSmallIcon(KVI_SMALLICON_HANDLER)),
					__tr2qs("&Enable Handler"),this,SLOT(toggleCurrentHandlerEnabled()));
			else
				m_pContextPopup->insertItem(
					*(g_pIconManager->getSmallIcon(KVI_SMALLICON_HANDLERDISABLED)),
					__tr2qs("&Disable Handler"),this,SLOT(toggleCurrentHandlerEnabled()));

			m_pContextPopup->insertItem(
					*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),
					__tr2qs("Re&move Handler"),
					this,SLOT(removeCurrentHandler()));
			m_pContextPopup->insertItem(
					*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
					__tr2qs("&Export Handler To..."),
					this,SLOT(exportCurrentHandler()));
		} else {
			m_pContextPopup->insertItem(
				*(g_pIconManager->getSmallIcon(KVI_SMALLICON_HANDLER)),
				__tr2qs("&New Handler"),
				this,SLOT(addHandlerForCurrentEvent()));
		}

		m_pContextPopup->popup(pnt);
	}
}

void KviEventEditor::getUniqueHandlerName(KviEventListViewItem *it,QString &buffer)
{
	__range_valid(m_bOneTimeSetupDone);

	QString newName = buffer;
	if(newName.isEmpty())newName = __tr2qs("unnamed");

	bool bFound = true;
	int idx = 1;

	while(bFound)
	{
		bFound = false;

		for(int i=0;i<it->childCount();i++)
		//for(KviEventHandlerListViewItem * ch = (KviEventHandlerListViewItem *)(it->firstChild());ch;ch = (KviEventHandlerListViewItem *)ch->nextSibling())
		{
			if(KviQString::equalCI(newName,((KviEventHandlerListViewItem *)it->child(i))->m_szName))
			{
				bFound = true;
				KviQString::sprintf(newName,"%Q_%d",&buffer,idx);
				idx++;
				break;
			}
		}
	}

	buffer = newName;
}

void KviEventEditor::addHandlerForCurrentEvent()
{
	__range_valid(m_pOneTimeSetupDone);

	if(!m_pListView->selectedItems().isEmpty())
	{
		QTreeWidgetItem * it = m_pListView->selectedItems().first();

		if(it->parent() == 0)
		{
			if(it->childCount()==0)
				it->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_EVENT))));

			QString buffer = __tr2qs("default");
			getUniqueHandlerName((KviEventListViewItem *)it,buffer);
			QTreeWidgetItem * ch = new KviEventHandlerListViewItem(it,buffer,"",true);
			it->setExpanded(true);
			ch->setSelected(true);
		}
	}
}

void KviEventEditor::removeCurrentHandler()
{
	__range_valid(m_pOneTimeSetupDone);
	if(m_pLastEditedItem)
	{
		QTreeWidgetItem * it = m_pLastEditedItem;
		QTreeWidgetItem * parent = m_pLastEditedItem->parent();
		m_pLastEditedItem = 0;
		delete it;

		if(parent)
		{
			if(parent->childCount()==0)
				parent->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_EVENTNOHANDLERS))));
		}

		m_pEditor->setEnabled(false);
		m_pNameEditor->setEnabled(false);
	}
}

void KviEventEditor::toggleCurrentHandlerEnabled()
{
	__range_valid(m_pOneTimeSetupDone);
	if(m_pLastEditedItem)
	{
		m_pLastEditedItem->setEnabled(!(m_pLastEditedItem->m_bEnabled));
		m_pListView->repaint(m_pListView->visualItemRect(m_pLastEditedItem));
		currentItemChanged(m_pLastEditedItem,0);
	}
}

void KviEventEditor::commit()
{
	if(!m_bOneTimeSetupDone)return; // nothing to commit

	saveLastEditedItem();
	KviKvsEventManager::instance()->removeAllScriptAppHandlers();

	int count=m_pListView->topLevelItemCount();
	for (int i=0;i<count;i++)
	//for(QTreeWidgetItem * it = m_pListView->firstChild();it;it = it->nextSibling())
	{
		QTreeWidgetItem * it = m_pListView->topLevelItem(i);
		int ccount = it->childCount();
		if(ccount > 0)
		{
			QString szContext;

			for(int j=0;j<ccount;j++)
			//for(QTreeWidgetItem * ch = it->firstChild();ch;ch = ch->nextSibling())
			{
				QTreeWidgetItem * ch = it->child(j);

				KviQString::sprintf(szContext,"%Q::%Q",&(((KviEventListViewItem *)it)->m_szName),&(((KviEventHandlerListViewItem *)ch)->m_szName));

				KviKvsScriptEventHandler * s = KviKvsScriptEventHandler::createInstance( // msvc workaround
						((KviEventHandlerListViewItem *)ch)->m_szName,
						szContext,
						((KviEventHandlerListViewItem *)ch)->m_szBuffer,
						((KviEventHandlerListViewItem *)ch)->m_bEnabled
					);

				KviKvsEventManager::instance()->addAppHandler(((KviEventListViewItem *)it)->m_uEventIdx,s);
			}
		}
	}

	g_pApp->saveAppEvents();
}

void KviEventEditor::saveLastEditedItem()
{
	__range_valid(m_bOneTimeSetupDone);
	if(!m_pLastEditedItem)return;

	QString buffer = m_pNameEditor->text();
	//not-so elaborate fix for #218, we'd better rework this
	buffer.replace(QRegExp("[^A-Za-z0-9_]"), "");
	if(!KviQString::equalCI(buffer,m_pLastEditedItem->m_szName))
	{
		getUniqueHandlerName((KviEventListViewItem *)(m_pLastEditedItem->parent()),buffer);
	}

	m_pLastEditedItem->setName(buffer);
	QString tmp;
	m_pEditor->getText(tmp);

	m_pLastEditedItem->m_szBuffer = tmp;
}

void KviEventEditor::currentItemChanged(QTreeWidgetItem * it,QTreeWidgetItem *prev)
{
	__range_valid(m_bOneTimeSetupDone);
	saveLastEditedItem();

	if(!it)
	{
		m_pNameEditor->setText(__tr2qs("No item selected"));
		m_pEditor->setText("");
		m_pEditor->setEnabled(false);
		return;
	}

	if(it->parent())
	{
		m_pLastEditedItem = (KviEventHandlerListViewItem *)it;
		m_pNameEditor->setEnabled(true);
		m_pNameEditor->setText(m_pLastEditedItem->name());
		m_pEditor->setEnabled(true);
		m_pEditor->setText(m_pLastEditedItem->m_szBuffer);
	} else {
		m_pLastEditedItem = 0;
		m_pNameEditor->setEnabled(false);
		m_pNameEditor->setText("");
		m_pEditor->setEnabled(false);
		QString parms = ((KviEventListViewItem *)it)->m_szParams;
		if(parms.isEmpty())parms = __tr2qs("none");
		KviCommandFormatter::indent(parms);
		KviCommandFormatter::indent(parms);
		QString tmp;
		KviQString::sprintf(tmp,__tr2qs("\n\n	Event:\n		%s\n\n	Parameters:\n%s"),
			((KviEventListViewItem *)it)->m_szName.toUtf8().data(),parms.toUtf8().data());
		m_pEditor->setText(tmp);
	}
}

void KviEventEditor::showEvent(QShowEvent *e)
{
	oneTimeSetup();
	QWidget::showEvent(e);
}

void KviEventEditor::getExportEventBuffer(QString &buffer,KviEventHandlerListViewItem * it)
{
	if(!it->parent())return;

	QString szBuf = it->m_szBuffer;

	KviCommandFormatter::blockFromBuffer(szBuf);

	buffer = "event(";
	buffer += ((KviEventListViewItem *)(it->parent()))->m_szName;
	buffer += ",";
	buffer += it->m_szName;
	buffer += ")\n";
	buffer += szBuf;
	buffer += "\n";

	if(!it->m_bEnabled)
	{
		buffer += "\n";
		buffer += "eventctl -d ";
		buffer += ((KviEventListViewItem *)(it->parent()))->m_szName;
		buffer += " ";
		buffer += it->m_szName;
	}
}

void KviEventEditor::exportCurrentHandler()
{
	if(!m_pLastEditedItem)return;
	saveLastEditedItem();
	if(!m_pLastEditedItem)return;

	QString szName = QDir::homePath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	szName += ((KviEventListViewItem *)(m_pLastEditedItem->parent()))->m_szName;
	szName += ".";
	szName += m_pLastEditedItem->m_szName;
	szName += ".kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs("Choose a Filename - KVIrc"),szName,"*.kvs",true,true,true))return;

	QString szOut;
	getExportEventBuffer(szOut,m_pLastEditedItem);

	if(!KviFileUtils::writeFile(szFile,szOut))
	{
		QMessageBox::warning(this,__tr2qs("Write Failed - KVIrc"),__tr2qs("Unable to write to the event file."),__tr2qs("&OK"));
	}
}

void KviEventEditor::exportAllEvents()
{
	saveLastEditedItem();

	QString out;

	int count=m_pListView->topLevelItemCount();
	for (int i=0;i<count;i++)
	{
		KviEventListViewItem * it = (KviEventListViewItem *)m_pListView->topLevelItem(i);

		int ccount = it->childCount();

		for(int j=0;j<ccount;j++)
		{
			KviEventHandlerListViewItem * item = (KviEventHandlerListViewItem *) it->child(j);

			QString tmp;
			getExportEventBuffer(tmp,item);
			out += tmp;
			out += "\n";
		}
	}

	QString szName = QDir::homePath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	szName += "events.kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs("Choose a Filename - KVIrc"),szName,QString::null,true,true))return;

	if(!KviFileUtils::writeFile(szFile,out))
	{
		QMessageBox::warning(this,__tr2qs("Write Failed - KVIrc"),__tr2qs("Unable to write to the events file."),__tr2qs("Ok"));
	}
}


KviEventEditorWindow::KviEventEditorWindow(KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_SCRIPTEDITOR,lpFrm,"eventeditor",0)
{
	g_pEventEditorWindow = this;

	m_pEditor = new KviEventEditor(this);

	m_pBase = new QWidget(this);
	QGridLayout * g = new QGridLayout(m_pBase);

	QPushButton * btn = new QPushButton(__tr2qs("&OK"),m_pBase);
	connect(btn,SIGNAL(clicked()),this,SLOT(okClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g->addWidget(btn,0,1);

	btn = new QPushButton(__tr2qs("&Apply"),m_pBase);
	connect(btn,SIGNAL(clicked()),this,SLOT(applyClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g->addWidget(btn,0,2);

	btn = new QPushButton(__tr2qs("Cancel"),m_pBase);
	connect(btn,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
	g->addWidget(btn,0,3);

	g->setColumnStretch(0,1);
}

KviEventEditorWindow::~KviEventEditorWindow()
{
	g_pEventEditorWindow = 0;
}

void KviEventEditorWindow::okClicked()
{
	m_pEditor->commit();
	close();
}

void KviEventEditorWindow::applyClicked()
{
	m_pEditor->commit();
}

void KviEventEditorWindow::cancelClicked()
{
	close();
}


QPixmap * KviEventEditorWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_EVENT);
}

void KviEventEditorWindow::resizeEvent(QResizeEvent *e)
{
	int hght = m_pBase->sizeHint().height();
	m_pEditor->setGeometry(0,0,width(),height()- hght);
	m_pBase->setGeometry(0,height() - hght,width(),hght);
}

void KviEventEditorWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs("Event Editor");

	static QString p1("<nobr><font color=\"");
	static QString p2("\"><b>");
	static QString p3("</b></font></nobr>");

	m_szHtmlActiveCaption = p1;
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name();
	m_szHtmlActiveCaption += p2;
	m_szHtmlActiveCaption += m_szPlainTextCaption;
	m_szHtmlActiveCaption += p3;

	m_szHtmlInactiveCaption = p1;
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name();
	m_szHtmlInactiveCaption += p2;
	m_szHtmlInactiveCaption += m_szPlainTextCaption;
	m_szHtmlInactiveCaption += p3;
}

void KviEventEditorWindow::getConfigGroupName(KviStr &szName)
{
	szName = "eventeditor";
}

void KviEventEditorWindow::saveProperties(KviConfig *cfg)
{
/*
#ifdef COMPILE_SCRIPTTOOLBAR
	cfg->writeEntry("Sizes",m_pEditor->sizes());
	cfg->writeEntry("LastEvent",m_pEditor->lastEditedEvent().ptr());
	//debug("LAST EDITED=%s",m_pEditor->lastEditedEvent().ptr());
#endif // COMPILE_SCRIPTTOOLBAR
*/
}

void KviEventEditorWindow::loadProperties(KviConfig *cfg)
{
/*
#ifdef COMPILE_SCRIPTTOOLBAR
	QValueList<int> def;
	def.append(20);
	def.append(80);
	m_pEditor->setSizes(cfg->readIntListEntry("Sizes",def));
	KviStr tmp = cfg->readEntry("LastEvent","");
	m_pEditor->editEvent(tmp);
	//debug("LAST EDITED WAS %s",tmp.ptr());
#endif // COMPILE_SCRIPTTOOLBAR
*/
}
