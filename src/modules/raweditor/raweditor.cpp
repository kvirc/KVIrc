//
//   File : raweditor.cpp
//   Creation date : Mon Dec 23 2002 20:28:18 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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
#include "raweditor.h"

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
#include "kvi_kvs_eventmanager.h"
#include "kvi_kvs_eventhandler.h"

#include <qmessagebox.h>
#include <qsplitter.h>
#include <qlayout.h>
#include "kvi_tal_vbox.h"
#include "kvi_tal_popupmenu.h"
#include <qtooltip.h>
#include <qinputdialog.h>
#include <qpushbutton.h>

extern KviRawEditorWindow * g_pRawEditorWindow;



KviRawListViewItem::KviRawListViewItem(KviTalListView *par,int idx)
: KviTalListViewItem(par)
{
	m_iIdx = idx;
 	m_szName.setNum(idx);
	if(idx < 100)m_szName.prepend('0');
	if(idx < 10)m_szName.prepend('0');
};

const QPixmap * KviRawListViewItem::pixmap(int col) const
{
	return g_pIconManager->getSmallIcon(firstChild() ? KVI_SMALLICON_RAWEVENT : KVI_SMALLICON_RAWEVENTNOHANDLERS);
}

const QPixmap * KviRawHandlerListViewItem::pixmap(int col) const
{
	return g_pIconManager->getSmallIcon(m_bEnabled ? KVI_SMALLICON_HANDLER : KVI_SMALLICON_HANDLERDISABLED);
}



KviRawEditor::KviRawEditor(QWidget * par)
: QWidget(par,"raw_event_editor")
{
	QGridLayout * l = new QGridLayout(this,1,1,2,2);

	QSplitter * spl = new QSplitter(QSplitter::Horizontal,this);

	l->addWidget(spl,0,0);

	KviTalVBox * boxi = new KviTalVBox(spl);
	m_pListView = new KviTalListView(boxi);
	m_pListView->addColumn(__tr2qs("Raw Event"));
	m_pListView->setMultiSelection(false);
	m_pListView->setShowSortIndicator(true);
	m_pListView->setRootIsDecorated(true);

	QPushButton * pb = new QPushButton(__tr2qs("&Export All To..."),boxi);
	connect(pb,SIGNAL(clicked()),this,SLOT(exportAllEvents()));
	
	KviTalVBox * box = new KviTalVBox(spl);
	m_pNameEditor = new QLineEdit(box);
	QToolTip::add(m_pNameEditor,__tr2qs("Edit the raw event handler name."));
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

	KviRawListViewItem * it;
	KviRawHandlerListViewItem * ch;

	for(unsigned int i = 0;i < 999;i++)
	{
		KviPtrList<KviKvsEventHandler> * l = KviKvsEventManager::instance()->rawHandlers(i);
		if(l)
		{
			it = new KviRawListViewItem(m_pListView,i);
			for(KviKvsEventHandler * s = l->first();s;s = l->next())
			{
				if(s->type() == KviKvsEventHandler::Script)
				{
					ch = new KviRawHandlerListViewItem(it,((KviKvsScriptEventHandler *)s)->name(),
					((KviKvsScriptEventHandler *)s)->code(),((KviKvsScriptEventHandler *)s)->isEnabled());
				}
			}
			it->setOpen(true);
		}
	}


	m_pContextPopup = new KviTalPopupMenu(this);

	connect(m_pListView,SIGNAL(selectionChanged(KviTalListViewItem *)),this,SLOT(selectionChanged(KviTalListViewItem *)));
	connect(m_pListView,SIGNAL(rightButtonPressed(KviTalListViewItem *,const QPoint &,int)),
		this,SLOT(itemPressed(KviTalListViewItem *,const QPoint &,int)));
}

void KviRawEditor::itemPressed(KviTalListViewItem *it,const QPoint &pnt,int col)
{
	__range_valid(m_bOneTimeSetupDone);
	m_pContextPopup->clear();
	if(it)
	{
		if(it->parent())
		{
			if(!(((KviRawHandlerListViewItem *)it)->m_bEnabled))
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
				this,SLOT(addHandlerForCurrentRaw()));
		}
	}

	m_pContextPopup->insertSeparator();
	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_RAWEVENT)),
			__tr2qs("&Add Raw Event..."),
			this,SLOT(addRaw()));

	m_pContextPopup->popup(pnt);
}

void KviRawEditor::getUniqueHandlerName(KviRawListViewItem *it,QString &buffer)
{
	__range_valid(m_bOneTimeSetupDone);

	QString newName = buffer;
	if(newName.isEmpty())newName = __tr2qs("unnamed");

	bool bFound = true;
	int idx = 1;

	while(bFound)
	{
		bFound = false;

		for(KviRawHandlerListViewItem * ch = (KviRawHandlerListViewItem *)(it->firstChild());ch;ch = (KviRawHandlerListViewItem *)ch->nextSibling())
		{
			if(KviQString::equalCI(newName,ch->m_szName))
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

	int iIdx = QInputDialog::getInteger(__tr2qs("New Raw Event"),__tr2qs("Enter the numeric code of the message (0-999)"),0,0,999,1,&bOk,this);

	if(!bOk)return;

	KviRawListViewItem * it;
	for(it = (KviRawListViewItem *)m_pListView->firstChild();it;it = (KviRawListViewItem *)it->nextSibling())
	{
		if(it->m_iIdx == iIdx)
		{
			m_pListView->setSelected(it,true);
			goto add_handler;
		}
	}

	it = new KviRawListViewItem(m_pListView,iIdx);
	m_pListView->setSelected(it,true);

add_handler:
	addHandlerForCurrentRaw();

}

void KviRawEditor::addHandlerForCurrentRaw()
{
	__range_valid(m_pOneTimeSetupDone);

	KviTalListViewItem * it = m_pListView->selectedItem();
	if(it)
	{
		if(it->parent() == 0)
		{
			QString buffer = __tr2qs("default");
			getUniqueHandlerName((KviRawListViewItem *)it,buffer);
			KviTalListViewItem * ch = new KviRawHandlerListViewItem(it,buffer,"",true);
			it->setOpen(true);
			m_pListView->setSelected(ch,true);
		}
	}
}

void KviRawEditor::removeCurrentHandler()
{
	__range_valid(m_pOneTimeSetupDone);
	if(m_pLastEditedItem)
	{
		KviTalListViewItem * it = m_pLastEditedItem;
		m_pLastEditedItem = 0;
		delete it;
		m_pEditor->setEnabled(false);
		m_pNameEditor->setEnabled(false);
	}
}

void KviRawEditor::toggleCurrentHandlerEnabled()
{
	__range_valid(m_pOneTimeSetupDone);
	if(m_pLastEditedItem)
	{
		m_pLastEditedItem->m_bEnabled = !(m_pLastEditedItem->m_bEnabled);
		m_pListView->repaintItem(m_pLastEditedItem);
		selectionChanged(m_pLastEditedItem);
	}
}

void KviRawEditor::commit()
{
	if(!m_bOneTimeSetupDone)return; // nothing to commit

	saveLastEditedItem();
	KviKvsEventManager::instance()->removeAllScriptRawHandlers();
	for(KviTalListViewItem * it = m_pListView->firstChild();it;it = it->nextSibling())
	{
		if(it->firstChild())
		{
			QString szContext;
			for(KviTalListViewItem * ch = it->firstChild();ch;ch = ch->nextSibling())
			{

				KviQString::sprintf(szContext,"RawEvent%d::%Q",&(((KviRawListViewItem *)it)->m_iIdx),&(((KviRawHandlerListViewItem *)ch)->m_szName));
				KviKvsScriptEventHandler * s = new KviKvsScriptEventHandler(
						((KviRawHandlerListViewItem *)ch)->m_szName,
						szContext,
						((KviRawHandlerListViewItem *)ch)->m_szBuffer,
						((KviRawHandlerListViewItem *)ch)->m_bEnabled
					);
			
				KviKvsEventManager::instance()->addRawHandler(((KviRawListViewItem *)it)->m_iIdx,s);
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
	if(!KviQString::equalCI(buffer,m_pLastEditedItem->m_szName))
	{
		getUniqueHandlerName((KviRawListViewItem *)(m_pLastEditedItem->parent()),buffer);
	}

	m_pLastEditedItem->m_szName = buffer;
	QString tmp;
	m_pEditor->getText(tmp);

	m_pLastEditedItem->m_szBuffer = tmp;
}

void KviRawEditor::selectionChanged(KviTalListViewItem * it)
{
	__range_valid(m_bOneTimeSetupDone);
	saveLastEditedItem();
	if(it->parent())
	{
		m_pLastEditedItem = (KviRawHandlerListViewItem *)it;
		m_pNameEditor->setEnabled(true);
		m_pNameEditor->setText(it->text(0));
		m_pEditor->setEnabled(true);
		m_pEditor->setText(((KviRawHandlerListViewItem *)it)->m_szBuffer);
	} else {
		m_pLastEditedItem = 0;
		m_pNameEditor->setEnabled(false);
		m_pNameEditor->setText("");
		m_pEditor->setEnabled(false);
	}
}

void KviRawEditor::showEvent(QShowEvent *e)
{
	oneTimeSetup();
	QWidget::showEvent(e);
}

void KviRawEditor::getExportEventBuffer(QString &buffer,KviRawHandlerListViewItem * it)
{
	if(!it->parent())return;

	QString szBuf = it->m_szBuffer;

	KviCommandFormatter::blockFromBuffer(szBuf);
	
	buffer = "event(";
	buffer += ((KviRawListViewItem *)(it->parent()))->m_szName;
	buffer += ",";
	buffer += it->m_szName;
	buffer += ")\n";
	buffer += szBuf;
	buffer += "\n";

	if(!it->m_bEnabled)
	{
		buffer += "\n";
		buffer += "eventctl -d ";
		buffer += ((KviRawListViewItem *)(it->parent()))->m_szName;
		buffer += " ";
		buffer += it->m_szName;
	}
}

void KviRawEditor::exportCurrentHandler()
{
	if(!m_pLastEditedItem)return;
	saveLastEditedItem();
	if(!m_pLastEditedItem)return;

	QString szName = QDir::homeDirPath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	szName += "raw";
	szName += ((KviRawListViewItem *)(m_pLastEditedItem->parent()))->m_szName;
	szName += ".";
	szName += m_pLastEditedItem->m_szName;
	szName += ".kvs";
	
	QString szFile;
	
	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs("Choose a Filename - KVIrc"),szName,"*.kvs",true,true,true))return;

	QString szOut;
	getExportEventBuffer(szOut,m_pLastEditedItem);
	
	if(!KviFileUtils::writeFile(szFile,szOut))
	{
		QMessageBox::warning(this,__tr2qs("Write Failed - KVIrc"),__tr2qs("Unable to write to the raw event file."),__tr2qs("&OK"));
	}
}

void KviRawEditor::exportAllEvents()
{
	saveLastEditedItem();

	KviRawListViewItem * it = (KviRawListViewItem *)m_pListView->firstChild();

	QString out;

	while(it)
	{
		KviRawHandlerListViewItem * item = (KviRawHandlerListViewItem *)it->firstChild();
		while(item)
		{
			QString tmp;
			getExportEventBuffer(tmp,item);
			out += tmp;
			out += "\n";
			item = (KviRawHandlerListViewItem *)item->nextSibling();
		}
		it = (KviRawListViewItem *)it->nextSibling();
	}

	QString szName = QDir::homeDirPath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	szName += "rawevents.kvs";
	
	QString szFile;
	
	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs("Choose a Filename - KVIrc"),szName,"*.kvs",true,true,true))return;

	if(!KviFileUtils::writeFile(szFile,out))
	{
		QMessageBox::warning(this,__tr2qs("Write Failed - KVIrc"),__tr2qs("Unable to write to the raw events file."),__tr2qs("Ok"));
	}
}




KviRawEditorWindow::KviRawEditorWindow(KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_SCRIPTEDITOR,lpFrm,"raweditor",0)
{
	g_pRawEditorWindow = this;

	m_pEditor = new KviRawEditor(this);

	m_pBase = new QWidget(this);
	QGridLayout * g = new QGridLayout(m_pBase,1,4,4,4);

	QPushButton * btn = new QPushButton(__tr2qs("&OK"),m_pBase);
	connect(btn,SIGNAL(clicked()),this,SLOT(okClicked()));
	btn->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g->addWidget(btn,0,1);

	btn = new QPushButton(__tr2qs("&Apply"),m_pBase);
	connect(btn,SIGNAL(clicked()),this,SLOT(applyClicked()));
	btn->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g->addWidget(btn,0,2);

	btn = new QPushButton(__tr2qs("Cancel"),m_pBase);
	connect(btn,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	btn->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
	g->addWidget(btn,0,3);

	g->setColStretch(0,1);
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

void KviRawEditorWindow::resizeEvent(QResizeEvent *e)
{
	int hght = m_pBase->sizeHint().height();
	m_pEditor->setGeometry(0,0,width(),height()- hght);
	m_pBase->setGeometry(0,height() - hght,width(),hght);
}

void KviRawEditorWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs("Raw Editor");

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


void KviRawEditorWindow::getConfigGroupName(KviStr &szName)
{
	szName = "raweditor";
}


void KviRawEditorWindow::saveProperties(KviConfig *cfg)
{
/*
#ifdef COMPILE_SCRIPTTOOLBAR
	cfg->writeEntry("Sizes",m_pEditor->sizes());
	cfg->writeEntry("LastRaw",m_pEditor->lastEditedRaw().ptr());
	//debug("LAST EDITED=%s",m_pEditor->lastEditedRaw().ptr());
#endif // COMPILE_SCRIPTTOOLBAR
*/
}

void KviRawEditorWindow::loadProperties(KviConfig *cfg)
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
