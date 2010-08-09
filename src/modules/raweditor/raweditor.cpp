//=============================================================================
//
//   File : raweditor.cpp
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

#include "raweditor.h"

#include "kvi_iconmanager.h"
#include "kvi_iconmanager.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_imagedialog.h"
#include "kvi_config.h"
#include "kvi_filedialog.h"
#include "kvi_fileutils.h"
#include "kvi_scripteditor.h"
#include "kvi_debug.h"
#include "kvi_fileextensions.h"
#include "kvi_app.h"
#include "kvi_cmdformatter.h"
#include "kvi_kvs_eventmanager.h"
#include "kvi_kvs_eventhandler.h"
#include "kvi_tal_vbox.h"
#include "kvi_tal_popupmenu.h"

#include <QMessageBox>
#include <QSplitter>
#include <QLayout>
#include <QToolTip>
#include <QInputDialog>
#include <QPushButton>

extern KviRawEditorWindow * g_pRawEditorWindow;


KviRawTreeWidgetItem::KviRawTreeWidgetItem(QTreeWidget *par,int idx,bool bEnabled)
: QTreeWidgetItem(par)
{
	m_iIdx = idx;
 	QString szName;
	szName.sprintf("%03d", idx);
	setText(0,szName);
	setEnabled(bEnabled);
}

void KviRawHandlerTreeWidgetItem::setName(const QString &szName)
{
	setText(0,szName);
}

KviRawEditor::KviRawEditor(QWidget * par)
: QWidget(par)
{
	setObjectName("raw_event_editor");
	QGridLayout * l = new QGridLayout(this);
	QSplitter * spl = new QSplitter(Qt::Horizontal,this);
	spl->setObjectName("raweditor_splitter");
	spl->setChildrenCollapsible(false);

	l->addWidget(spl,0,0);

	KviTalVBox * boxi = new KviTalVBox(spl);
	boxi->setMaximumWidth(200);
	m_pTreeWidget = new KviRawTreeWidget(boxi);



	m_pTreeWidget->setColumnCount(1);
	m_pTreeWidget->setHeaderLabel(__tr2qs_ctx("Raw Event","editor"));

//	m_pTreeWidget->setMultiSelection(false);
	m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
//	m_pTreeWidget->setShowSortIndicator(true);
	m_pTreeWidget->setRootIsDecorated(true);
	m_pContextPopup = new KviTalPopupMenu(this);
	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pTreeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));

	connect(m_pTreeWidget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(customContextMenuRequested(const QPoint &)));

	QPushButton * pb = new QPushButton(__tr2qs_ctx("&Export All To...","editor"),boxi);
	connect(pb,SIGNAL(clicked()),this,SLOT(exportAllEvents()));

	KviTalVBox * box = new KviTalVBox(spl);
	m_pNameEditor = new QLineEdit(box);
	m_pNameEditor->setToolTip(__tr2qs_ctx("Edit the raw event handler name.","editor"));
	m_pEditor = KviScriptEditor::createInstance(box);

	m_bOneTimeSetupDone = false;
	m_pLastEditedItem = 0;
}

KviRawEditor::~KviRawEditor()
{
	KviScriptEditor::destroyInstance(m_pEditor);
}

void KviRawEditor::oneTimeSetup()
{
	if(m_bOneTimeSetupDone)return;
	m_bOneTimeSetupDone = true;

	KviRawTreeWidgetItem * it;
	KviRawHandlerTreeWidgetItem * ch;

	for(unsigned int i = 0;i < 999;i++)
	{
		KviPointerList<KviKvsEventHandler> * l = KviKvsEventManager::instance()->rawHandlers(i);
		if(l)
		{
			it = new KviRawTreeWidgetItem(m_pTreeWidget,i,true);
			for(KviKvsEventHandler * s = l->first();s;s = l->next())
			{
				if(s->type() == KviKvsEventHandler::Script)
				{
					ch = new KviRawHandlerTreeWidgetItem(it,((KviKvsScriptEventHandler *)s)->name(),
					((KviKvsScriptEventHandler *)s)->code(),((KviKvsScriptEventHandler *)s)->isEnabled());
				}
			}
			it->setExpanded(true);
		}
	}
}

void KviRawEditor::customContextMenuRequested(const QPoint &pos)
{
	QTreeWidgetItem *it;
	it=(QTreeWidgetItem *)m_pTreeWidget->itemAt(pos);
	__range_valid(m_bOneTimeSetupDone);
	m_pContextPopup->clear();
	if(it)
	{
		if(it->parent())
		{
			if(!(((KviRawHandlerTreeWidgetItem *)it)->m_bEnabled))
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
				this,SLOT(addHandlerForCurrentRaw()));
		}
	}

	m_pContextPopup->insertSeparator();
	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_RAWEVENT)),
			__tr2qs_ctx("&Add Raw Event...","editor"),
			this,SLOT(addRaw()));
        m_pContextPopup->popup(mapToGlobal(QPoint(pos.x()+15,pos.y())));
}

void KviRawEditor::getUniqueHandlerName(KviRawTreeWidgetItem *it,QString &buffer)
{
	__range_valid(m_bOneTimeSetupDone);

	QString newName = buffer;
	if(newName.isEmpty())newName = __tr2qs_ctx("unnamed","editor");

	bool bFound = true;
	int idx = 1;

	while(bFound)
	{
		bFound = false;

		for (int i=0;i<it->childCount();i++)
		{
			KviRawHandlerTreeWidgetItem * ch =(KviRawHandlerTreeWidgetItem *) it->child(i);
			if(KviQString::equalCI(newName,ch->text(0)))
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

void KviRawEditor::addRaw()
{
	bool bOk = false;

	int iIdx = QInputDialog::getInteger(this,__tr2qs_ctx("New Raw Event","editor"),__tr2qs_ctx("Enter the numeric code of the message (0-999)","editor"),0,0,999,1,&bOk);

	if(!bOk)return;

	KviRawTreeWidgetItem * it;
	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		it=(KviRawTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if(((KviRawTreeWidgetItem *)it)->m_iIdx == iIdx)
		{
			m_pTreeWidget->setCurrentItem(it);
			m_pTreeWidget->clearSelection();
			it->setSelected(true);
			goto add_handler;
		}
	}

	it = new KviRawTreeWidgetItem(m_pTreeWidget,iIdx,true);
	m_pTreeWidget->setCurrentItem(it);
	m_pTreeWidget->clearSelection();
	it->setSelected(true);

add_handler:
	addHandlerForCurrentRaw();

}

void KviRawEditor::addHandlerForCurrentRaw()
{
	__range_valid(m_pOneTimeSetupDone);

	KviRawTreeWidgetItem * it = (KviRawTreeWidgetItem *) m_pTreeWidget->currentItem();
	if(it)
	{
		if(it->parent() == 0)
		{
			QString buffer = __tr2qs_ctx("default","editor");
			getUniqueHandlerName((KviRawTreeWidgetItem *)it,buffer);
			QTreeWidgetItem * ch = new KviRawHandlerTreeWidgetItem(it,buffer,"",true);
			it->setEnabled(true);
			it->setExpanded(true);
			m_pTreeWidget->setCurrentItem(ch);
			m_pTreeWidget->clearSelection();
			ch->setSelected(true);
		}
	}
}

void KviRawEditor::removeCurrentHandler()
{
	__range_valid(m_pOneTimeSetupDone);
	if(m_pLastEditedItem)
	{
		QTreeWidgetItem * it = m_pLastEditedItem;
		KviRawTreeWidgetItem * parent=(KviRawTreeWidgetItem *)it->parent();
		m_pLastEditedItem = 0;
		delete it;
		m_pEditor->setEnabled(false);
		m_pNameEditor->setEnabled(false);
		if (!parent->childCount()) parent->setEnabled(false);
	}
}

void KviRawEditor::toggleCurrentHandlerEnabled()
{
	__range_valid(m_pOneTimeSetupDone);
	if(m_pLastEditedItem)
	{
		m_pLastEditedItem->m_bEnabled = !(m_pLastEditedItem->m_bEnabled);
		m_pLastEditedItem->setEnabled(m_pLastEditedItem->m_bEnabled);
		currentItemChanged(m_pLastEditedItem,m_pLastEditedItem);
	}
}

void KviRawEditor::commit()
{
	if(!m_bOneTimeSetupDone)return; // nothing to commit

	saveLastEditedItem();
	KviKvsEventManager::instance()->removeAllScriptRawHandlers();
	QTreeWidgetItem * it;
	for(int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		it=(QTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if(it->childCount())
		{
			QString szContext;
			QTreeWidgetItem * ch;
			for (int j=0;j<it->childCount();j++)
			{
				ch=(QTreeWidgetItem *)it->child(j);
				debug("Commit handler %s",((KviRawHandlerTreeWidgetItem *)ch)->text(0).toUtf8().data());
				//int a=(KviRawTreeWidgetItem *)it)->m_iIdx;
				KviQString::sprintf(szContext,"RawEvent%d::%s",&(((KviRawTreeWidgetItem *)it)->m_iIdx),(((KviRawHandlerTreeWidgetItem *)ch)->text(0)).toUtf8().data());
				KviKvsScriptEventHandler * s = new KviKvsScriptEventHandler(
						((KviRawHandlerTreeWidgetItem *)ch)->text(0),
						szContext,
						((KviRawHandlerTreeWidgetItem *)ch)->m_szBuffer,
						((KviRawHandlerTreeWidgetItem *)ch)->m_bEnabled
					);

				KviKvsEventManager::instance()->addRawHandler(((KviRawTreeWidgetItem *)it)->m_iIdx,s);
			}
		}
	}

	g_pApp->saveRawEvents(); // otherwise this page has never been shown
}

void KviRawEditor::saveLastEditedItem()
{
	__range_valid(m_bOneTimeSetupDone);
	if(!m_pLastEditedItem)return;

	QString buffer = m_pNameEditor->text();
	debug("Check lineedit name %s and internal %s",buffer.toUtf8().data(),m_pLastEditedItem->text(0).toUtf8().data());
	if(!KviQString::equalCI(buffer,m_pLastEditedItem->text(0)))
	{
		getUniqueHandlerName((KviRawTreeWidgetItem *)(m_pLastEditedItem->parent()),buffer);
		debug("Change name %s",buffer.toUtf8().data());
	}

	m_pLastEditedItem->setName(buffer);
	QString tmp;
	m_pEditor->getText(tmp);

	m_pLastEditedItem->m_szBuffer = tmp;
}

void KviRawEditor::currentItemChanged(QTreeWidgetItem * it,QTreeWidgetItem *)
{
	__range_valid(m_bOneTimeSetupDone);
	saveLastEditedItem();
	if(it->parent())
	{
		m_pLastEditedItem = (KviRawHandlerTreeWidgetItem *)it;
		m_pNameEditor->setEnabled(true);
		m_pNameEditor->setText(it->text(0));
		m_pEditor->setEnabled(true);
		m_pEditor->setText(((KviRawHandlerTreeWidgetItem *)it)->m_szBuffer);
	} else {
		m_pLastEditedItem = 0;
		m_pNameEditor->setEnabled(false);
		m_pNameEditor->setText("");
		m_pEditor->setEnabled(false);
		QString tmp;
		KviQString::sprintf(tmp,__tr2qs_ctx("\n\nRaw Event:\n%s","editor"),
			((KviRawHandlerTreeWidgetItem *)it)->text(0).toUtf8().data());
		m_pEditor->setText(tmp);
	}
}

void KviRawEditor::showEvent(QShowEvent *e)
{
	oneTimeSetup();
	QWidget::showEvent(e);
}

void KviRawEditor::getExportEventBuffer(QString &buffer,KviRawHandlerTreeWidgetItem * it)
{
	if(!it->parent())return;

	QString szBuf = it->m_szBuffer;

	KviCommandFormatter::blockFromBuffer(szBuf);

	buffer = "event(";
	buffer += ((KviRawTreeWidgetItem *)(it->parent()))->text(0);
	buffer += ",";
	buffer += it->text(0);
	buffer += ")\n";
	buffer += szBuf;
	buffer += "\n";

	if(!it->m_bEnabled)
	{
		buffer += "\n";
		buffer += "eventctl -d ";
		buffer += ((KviRawTreeWidgetItem *)(it->parent()))->text(0);
		buffer += " ";
		buffer += it->text(0);
	}
}

void KviRawEditor::exportCurrentHandler()
{
	if(!m_pLastEditedItem)return;
	saveLastEditedItem();
	if(!m_pLastEditedItem)return;

	QString szName = QDir::homePath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	szName += "raw";
	szName += ((KviRawTreeWidgetItem *)(m_pLastEditedItem->parent()))->text(0);
	szName += ".";
	szName += m_pLastEditedItem->text(0);
	szName += ".kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs_ctx("Choose a Filename - KVIrc","editor"),szName,KVI_FILTER_SCRIPT,true,true,true))return;

	QString szOut;
	getExportEventBuffer(szOut,m_pLastEditedItem);

	if(!KviFileUtils::writeFile(szFile,szOut))
	{
		QMessageBox::warning(this,__tr2qs_ctx("Write Failed - KVIrc","editor"),__tr2qs_ctx("Unable to write to the raw event file.","editor"),__tr2qs_ctx("&OK","editor"));
	}
}

void KviRawEditor::exportAllEvents()
{
	saveLastEditedItem();

	KviRawTreeWidgetItem * it;
	QString out;

	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		it=(KviRawTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		KviRawHandlerTreeWidgetItem * item;
		for (int j=0;j<it->childCount();j++)
		{
			QString tmp;
			item=(KviRawHandlerTreeWidgetItem *)it->child(j);
			getExportEventBuffer(tmp,item);
			out += tmp;
			out += "\n";
		}
	}

	QString szName = QDir::homePath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	szName += "rawevents.kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs_ctx("Choose a Filename - KVIrc","editor"),szName,KVI_FILTER_SCRIPT,true,true,true))return;

	if(!KviFileUtils::writeFile(szFile,out))
	{
		QMessageBox::warning(this,__tr2qs_ctx("Write Failed - KVIrc","editor"),__tr2qs_ctx("Unable to write to the raw events file.","editor"),__tr2qs_ctx("OK","editor"));
	}
}


KviRawEditorWindow::KviRawEditorWindow(KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_SCRIPTEDITOR,lpFrm,"raweditor",0)
{
	g_pRawEditorWindow = this;

	QGridLayout * g = new QGridLayout();

	m_pEditor = new KviRawEditor(this);
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

KviRawEditorWindow::~KviRawEditorWindow()
{
	g_pRawEditorWindow = 0;
}

void KviRawEditorWindow::okClicked()
{
	m_pEditor->commit();
	close();
}

void KviRawEditorWindow::applyClicked()
{
	m_pEditor->commit();
}

void KviRawEditorWindow::cancelClicked()
{
	close();
}

QPixmap * KviRawEditorWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_RAWEVENT);
}

void KviRawEditorWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Raw Editor","editor");
}

void KviRawEditorWindow::getConfigGroupName(QString &szName)
{
	szName = "raweditor";
}

void KviRawEditorWindow::saveProperties(KviConfig *)
{
	/*
	#ifdef COMPILE_SCRIPTTOOLBAR
		cfg->writeEntry("Sizes",m_pEditor->sizes());
		cfg->writeEntry("LastRaw",m_pEditor->lastEditedRaw().ptr());
		//debug("LAST EDITED=%s",m_pEditor->lastEditedRaw().ptr());
	#endif // COMPILE_SCRIPTTOOLBAR
	*/
}

void KviRawEditorWindow::loadProperties(KviConfig *)
{
	/*
	#ifdef COMPILE_SCRIPTTOOLBAR
		QValueList<int> def;
		def.append(20);
		def.append(80);
		m_pEditor->setSizes(cfg->readIntListEntry("Sizes",def));
		KviStr tmp = cfg->readEntry("LastRaw","");
		m_pEditor->editRaw(tmp);
		//debug("LAST EDITED WAS %s",tmp.ptr());
	#endif // COMPILE_SCRIPTTOOLBAR
	*/
}
