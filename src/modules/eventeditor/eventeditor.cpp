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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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
#include "kvi_app.h"
#include "kvi_cmdformatter.h"
#include "kvi_fileextensions.h"
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

KviEventTreeWidgetItem::KviEventTreeWidgetItem(QTreeWidget * par,unsigned int uEvIdx,const QString &name,const QString &params)
: QTreeWidgetItem(par), m_uEventIdx(uEvIdx), m_szParams(params)
{
	setName(name);
}

void KviEventTreeWidgetItem::setName(const QString &szName)
{
	m_szName = szName;
	setText(0,m_szName);
}

KviEventHandlerTreeWidgetItem::KviEventHandlerTreeWidgetItem(QTreeWidgetItem * par,const QString &name,const QString &buffer,bool bEnabled)
: QTreeWidgetItem(par), m_szBuffer(buffer)
{
	m_cPos=0;
	setEnabled(bEnabled); //this updates the icon too
	setName(name);
}

void KviEventHandlerTreeWidgetItem::setName(const QString &szName)
{
	m_szName = szName;
	setText(0,m_szName);
}

void KviEventHandlerTreeWidgetItem::setEnabled(const bool bEnabled)
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

	KviTalVBox * vbox = new KviTalVBox(spl);
	vbox->setSpacing(0);
	vbox->setMargin(0);

	m_pTreeWidget = new KviEventEditorTreeWidget(vbox);

	QPushButton * pb = new QPushButton(__tr2qs_ctx("&Export All To...","editor"),vbox);
	connect(pb,SIGNAL(clicked()),this,SLOT(exportAllEvents()));

	KviTalVBox * box = new KviTalVBox(spl);
	spl->setStretchFactor (0,20);
	spl->setStretchFactor (1,80);

	box->setSpacing(0);
	box->setMargin(0);

	m_pNameEditor = new QLineEdit(box);
	m_pNameEditor->setToolTip(__tr2qs_ctx("Edit the event handler name.","editor"));

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

	KviEventTreeWidgetItem * it;
	KviEventHandlerTreeWidgetItem * ch;

	for(unsigned int i = 0;i < KVI_KVS_NUM_APP_EVENTS;i++)
	{
		KviKvsEvent * e = KviKvsEventManager::instance()->appEvent(i);

		it = new KviEventTreeWidgetItem(m_pTreeWidget,i,e->name(),e->parameterDescription());
		if(KviPointerList<KviKvsEventHandler> * l = e->handlers())
		{
			for(KviKvsEventHandler * s = l->first();s;s = l->next())
			{
				if(s->type() == KviKvsEventHandler::Script)
				{
					ch = new KviEventHandlerTreeWidgetItem(it,((KviKvsScriptEventHandler *)s)->name(),
					((KviKvsScriptEventHandler *)s)->code(),((KviKvsScriptEventHandler *)s)->isEnabled());
				}
			}
		}
		it->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(it->childCount() ? KVI_SMALLICON_EVENT : KVI_SMALLICON_EVENTNOHANDLERS))));
	}

	connect(m_pTreeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
	connect(m_pTreeWidget,SIGNAL(rightButtonPressed(QTreeWidgetItem *,QPoint)),
		this,SLOT(itemPressed(QTreeWidgetItem *,QPoint)));

	m_pContextPopup = new KviTalPopupMenu(this);
	m_pTreeWidget->sortItems(0,Qt::AscendingOrder);
}

KviEventEditorTreeWidget::KviEventEditorTreeWidget(QWidget * par)
: QTreeWidget(par)
{
	setColumnCount (1);
	setHeaderLabel(__tr2qs_ctx("Event","editor"));
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSortingEnabled(true);
	setRootIsDecorated(true);
	setAnimated(true);
}

void KviEventEditorTreeWidget::mousePressEvent (QMouseEvent *e)
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
			if(!(((KviEventHandlerTreeWidgetItem *)it)->m_bEnabled))
				m_pContextPopup->insertItem(
					*(g_pIconManager->getSmallIcon(KVI_SMALLICON_HANDLER)),
					__tr2qs_ctx("&Enable Handler","editor"),this,SLOT(toggleCurrentHandlerEnabled()));
			else
				m_pContextPopup->insertItem(
					*(g_pIconManager->getSmallIcon(KVI_SMALLICON_HANDLERDISABLED)),
					__tr2qs_ctx("&Disable Handler","editor"),this,SLOT(toggleCurrentHandlerEnabled()));

			m_pContextPopup->insertItem(
					*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),
					__tr2qs_ctx("Re&move Handler","editor"),
					this,SLOT(removeCurrentHandler()));
			m_pContextPopup->insertItem(
					*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
					__tr2qs_ctx("&Export Handler To...","editor"),
					this,SLOT(exportCurrentHandler()));
		} else {
			m_pContextPopup->insertItem(
				*(g_pIconManager->getSmallIcon(KVI_SMALLICON_HANDLER)),
				__tr2qs_ctx("&New Handler","editor"),
				this,SLOT(addHandlerForCurrentEvent()));
		}

		m_pContextPopup->popup(pnt);
	}
}

void KviEventEditor::getUniqueHandlerName(KviEventTreeWidgetItem *it,QString &buffer)
{
	__range_valid(m_bOneTimeSetupDone);

	QString newName = buffer;
	if(newName.isEmpty())newName = __tr2qs_ctx("unnamed","editor");

	bool bFound = true;
	int idx = 1;

	while(bFound)
	{
		bFound = false;

		for(int i=0;i<it->childCount();i++)
		//for(KviEventHandlerTreeWidgetItem * ch = (KviEventHandlerTreeWidgetItem *)(it->firstChild());ch;ch = (KviEventHandlerTreeWidgetItem *)ch->nextSibling())
		{
			if(KviQString::equalCI(newName,((KviEventHandlerTreeWidgetItem *)it->child(i))->m_szName))
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

	if(!m_pTreeWidget->selectedItems().isEmpty())
	{
		QTreeWidgetItem * it = m_pTreeWidget->selectedItems().first();

		if(it->parent() == 0)
		{
			if(it->childCount()==0)
				it->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_EVENT))));

			QString buffer = __tr2qs_ctx("default","editor");
			getUniqueHandlerName((KviEventTreeWidgetItem *)it,buffer);
			QTreeWidgetItem * ch = new KviEventHandlerTreeWidgetItem(it,buffer,"",true);
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
		m_pTreeWidget->repaint(m_pTreeWidget->visualItemRect(m_pLastEditedItem));
		currentItemChanged(m_pLastEditedItem,0);
	}
}

void KviEventEditor::commit()
{
	if(!m_bOneTimeSetupDone)return; // nothing to commit

	saveLastEditedItem();
	KviKvsEventManager::instance()->removeAllScriptAppHandlers();

	int count=m_pTreeWidget->topLevelItemCount();
	for (int i=0;i<count;i++)
	{
		QTreeWidgetItem * it = m_pTreeWidget->topLevelItem(i);
		int ccount = it->childCount();
		if(ccount > 0)
		{
			QString szContext;

			for(int j=0;j<ccount;j++)
			{
				QTreeWidgetItem * ch = it->child(j);
				KviQString::sprintf(szContext,"%Q::%Q",&(((KviEventTreeWidgetItem *)it)->m_szName),&(((KviEventHandlerTreeWidgetItem *)ch)->m_szName));
				KviKvsScriptEventHandler * s = KviKvsScriptEventHandler::createInstance( // msvc workaround
						((KviEventHandlerTreeWidgetItem *)ch)->m_szName,
						szContext,
						((KviEventHandlerTreeWidgetItem *)ch)->m_szBuffer,
						((KviEventHandlerTreeWidgetItem *)ch)->m_bEnabled
					);

				KviKvsEventManager::instance()->addAppHandler(((KviEventTreeWidgetItem *)it)->m_uEventIdx,s);
			}
		}
	}

	g_pApp->saveAppEvents();
}

void KviEventEditor::saveLastEditedItem()
{
	__range_valid(m_bOneTimeSetupDone);
	if(!m_pLastEditedItem)return;
	((KviEventHandlerTreeWidgetItem *)m_pLastEditedItem)->setCursorPosition(m_pEditor->getCursor());
	QString buffer = m_pNameEditor->text();
	//not-so elaborate fix for #218, we'd better rework this
	buffer.replace(QRegExp("[^A-Za-z0-9_]"), "");
	if(!KviQString::equalCI(buffer,m_pLastEditedItem->m_szName))
	{
		getUniqueHandlerName((KviEventTreeWidgetItem *)(m_pLastEditedItem->parent()),buffer);
	}

	m_pLastEditedItem->setName(buffer);
	QString tmp;
	m_pEditor->getText(tmp);

	m_pLastEditedItem->m_szBuffer = tmp;
}

void KviEventEditor::currentItemChanged(QTreeWidgetItem * it,QTreeWidgetItem *)
{
	__range_valid(m_bOneTimeSetupDone);
	saveLastEditedItem();

	if(!it)
	{
		m_pNameEditor->setText(__tr2qs_ctx("No item selected","editor"));
		m_pEditor->setText("");
		m_pEditor->setEnabled(false);
		return;
	}

	if(it->parent())
	{
		m_pLastEditedItem = (KviEventHandlerTreeWidgetItem *)it;
		m_pNameEditor->setEnabled(true);
		m_pNameEditor->setText(m_pLastEditedItem->name());
		m_pEditor->setEnabled(true);
		m_pEditor->setFocus();
		m_pEditor->setText(m_pLastEditedItem->m_szBuffer);
		m_pEditor->setCursorPosition(((KviEventHandlerTreeWidgetItem *)it)->cursorPosition());
	} else {
		m_pLastEditedItem = 0;
		m_pNameEditor->setEnabled(false);
		m_pNameEditor->setText("");
		m_pEditor->setEnabled(false);
		QString parms = ((KviEventTreeWidgetItem *)it)->m_szParams;
		if(parms.isEmpty())parms = __tr2qs_ctx("none","editor");
		KviCommandFormatter::indent(parms);
		KviCommandFormatter::indent(parms);
		QString tmp;
		KviQString::sprintf(tmp,__tr2qs_ctx("\n\nEvent:\n%s\n\nParameters:\n%s","editor"),
			((KviEventTreeWidgetItem *)it)->m_szName.toUtf8().data(),parms.toUtf8().data());
		m_pEditor->setText(tmp);
	}
}

void KviEventEditor::showEvent(QShowEvent *e)
{
	oneTimeSetup();
	QWidget::showEvent(e);
}

void KviEventEditor::getExportEventBuffer(QString &buffer,KviEventHandlerTreeWidgetItem * it)
{
	if(!it->parent())return;

	QString szBuf = it->m_szBuffer;

	KviCommandFormatter::blockFromBuffer(szBuf);

	buffer = "event(";
	buffer += ((KviEventTreeWidgetItem *)(it->parent()))->m_szName;
	buffer += ",";
	buffer += it->m_szName;
	buffer += ")\n";
	buffer += szBuf;
	buffer += "\n";

	if(!it->m_bEnabled)
	{
		buffer += "\n";
		buffer += "eventctl -d ";
		buffer += ((KviEventTreeWidgetItem *)(it->parent()))->m_szName;
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
	szName += ((KviEventTreeWidgetItem *)(m_pLastEditedItem->parent()))->m_szName;
	szName += ".";
	szName += m_pLastEditedItem->m_szName;
	szName += ".kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs_ctx("Choose a Filename - KVIrc","editor"),szName,KVI_FILTER_SCRIPT,true,true,true))return;

	QString szOut;
	getExportEventBuffer(szOut,m_pLastEditedItem);

	if(!KviFileUtils::writeFile(szFile,szOut))
	{
		QMessageBox::warning(this,__tr2qs_ctx("Write Failed - KVIrc","editor"),__tr2qs_ctx("Unable to write to the event file.","editor"),__tr2qs_ctx("&OK","editor"));
	}
}

void KviEventEditor::exportAllEvents()
{
	saveLastEditedItem();

	QString out;

	int count=m_pTreeWidget->topLevelItemCount();
	for (int i=0;i<count;i++)
	{
		KviEventTreeWidgetItem * it = (KviEventTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);

		int ccount = it->childCount();

		for(int j=0;j<ccount;j++)
		{
			KviEventHandlerTreeWidgetItem * item = (KviEventHandlerTreeWidgetItem *) it->child(j);

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

	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs_ctx("Choose a Filename - KVIrc","editor"),szName,QString(),true,true))return;

	if(!KviFileUtils::writeFile(szFile,out))
	{
		QMessageBox::warning(this,__tr2qs_ctx("Write Failed - KVIrc","editor"),__tr2qs_ctx("Unable to write to the events file.","editor"),__tr2qs_ctx("OK","editor"));
	}
}


KviEventEditorWindow::KviEventEditorWindow(KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_SCRIPTEDITOR,lpFrm,"eventeditor",0)
{
	g_pEventEditorWindow = this;

	QGridLayout * g = new QGridLayout();
	m_pEditor = new KviEventEditor(this);
	g->addWidget(m_pEditor,0,0,1,4);

	QPushButton * btn = new QPushButton(__tr2qs_ctx("&OK","editor"),this);
	connect(btn,SIGNAL(clicked()),this,SLOT(okClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g->addWidget(btn,1,1);

	btn = new QPushButton(__tr2qs_ctx("&Apply","editor"),this);
	connect(btn,SIGNAL(clicked()),this,SLOT(applyClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g->addWidget(btn,1,2);

	btn = new QPushButton(__tr2qs_ctx("Cancel","editor"),this);
	connect(btn,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
	g->addWidget(btn,1,3);

	g->setRowStretch(0,1);
	g->setColumnStretch(0,1);
	setLayout(g);
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

void KviEventEditorWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Event Editor","editor");
}

void KviEventEditorWindow::getConfigGroupName(QString &szName)
{
	szName = "eventeditor";
}

void KviEventEditorWindow::saveProperties(KviConfig *) //cfg
{
/*
#ifdef COMPILE_SCRIPTTOOLBAR
	cfg->writeEntry("Sizes",m_pEditor->sizes());
	cfg->writeEntry("LastEvent",m_pEditor->lastEditedEvent().ptr());
	//debug("LAST EDITED=%s",m_pEditor->lastEditedEvent().ptr());
#endif // COMPILE_SCRIPTTOOLBAR
*/
}

void KviEventEditorWindow::loadProperties(KviConfig *) //cfg
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
