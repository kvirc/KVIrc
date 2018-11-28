//=============================================================================
//
//   File : RawEditorWindow.cpp
//   Creation date : Mon Dec 23 2002 20:28:18 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "RawEditorWindow.h"

#include "KviIconManager.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviImageDialog.h"
#include "KviConfigurationFile.h"
#include "KviFileDialog.h"
#include "KviFileUtils.h"
#include "KviScriptEditor.h"
#include "kvi_debug.h"
#include "kvi_fileextensions.h"
#include "KviApplication.h"
#include "KviCommandFormatter.h"
#include "KviKvsEventManager.h"
#include "KviTalVBox.h"

#include <QMessageBox>
#include <QSplitter>
#include <QLayout>
#include <QToolTip>
#include <QInputDialog>
#include <QPushButton>
#include <QMenu>

extern RawEditorWindow * g_pRawEditorWindow;

RawTreeWidgetItem::RawTreeWidgetItem(QTreeWidget * par, int idx, bool bEnabled)
    : QTreeWidgetItem(par)
{
	m_iIdx = idx;
	QString szName;
	szName.sprintf("%03d", idx);
	setText(0, szName);
	setEnabled(bEnabled);
}

void RawHandlerTreeWidgetItem::setName(const QString & szName)
{
	setText(0, szName);
}

RawEditorWidget::RawEditorWidget(QWidget * par)
    : QWidget(par)
{
	setObjectName("raw_event_editor");
	QGridLayout * l = new QGridLayout(this);
	QSplitter * spl = new QSplitter(Qt::Horizontal, this);
	spl->setObjectName("raweditor_splitter");
	spl->setChildrenCollapsible(false);

	l->addWidget(spl, 0, 0);

	KviTalVBox * boxi = new KviTalVBox(spl);
	boxi->setMaximumWidth(200);
	m_pTreeWidget = new RawTreeWidget(boxi);

	m_pTreeWidget->setColumnCount(1);
	m_pTreeWidget->setHeaderLabel(__tr2qs_ctx("RAW Event", "editor"));

	//	m_pTreeWidget->setMultiSelection(false);
	m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	//	m_pTreeWidget->setShowSortIndicator(true);
	m_pTreeWidget->setRootIsDecorated(true);
	m_pContextPopup = new QMenu(this);
	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

	connect(m_pTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));

	QPushButton * pb = new QPushButton(__tr2qs_ctx("&Export All to...", "editor"), boxi);
	connect(pb, SIGNAL(clicked()), this, SLOT(exportAllEvents()));

	KviTalVBox * box = new KviTalVBox(spl);
	m_pNameEditor = new QLineEdit(box);
	m_pNameEditor->setToolTip(__tr2qs_ctx("Edit the RAW event handler name.", "editor"));
	m_pEditor = KviScriptEditor::createInstance(box);

	m_bOneTimeSetupDone = false;
	m_pLastEditedItem = nullptr;
}

RawEditorWidget::~RawEditorWidget()
{
	KviScriptEditor::destroyInstance(m_pEditor);
}

void RawEditorWidget::oneTimeSetup()
{
	if(m_bOneTimeSetupDone)
		return;
	m_bOneTimeSetupDone = true;

	RawTreeWidgetItem * it;

	for(unsigned int i = 0; i < 999; i++)
	{
		KviPointerList<KviKvsEventHandler> * l = KviKvsEventManager::instance()->rawHandlers(i);
		if(l)
		{
			it = new RawTreeWidgetItem(m_pTreeWidget, i, true);
			for(KviKvsEventHandler * s = l->first(); s; s = l->next())
			{
				if(s->type() == KviKvsEventHandler::Script)
				{
					new RawHandlerTreeWidgetItem(it, ((KviKvsScriptEventHandler *)s)->name(),
					    ((KviKvsScriptEventHandler *)s)->code(), ((KviKvsScriptEventHandler *)s)->isEnabled());
				}
			}
			it->setExpanded(true);
		}
	}
}

void RawEditorWidget::customContextMenuRequested(const QPoint & pos)
{
	QTreeWidgetItem * it;
	it = (QTreeWidgetItem *)m_pTreeWidget->itemAt(pos);
	KVI_ASSERT(m_bOneTimeSetupDone);
	m_pContextPopup->clear();
	if(it)
	{
		if(it->parent())
		{
			if(!(((RawHandlerTreeWidgetItem *)it)->m_bEnabled))
				m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Handler)),
				    __tr2qs_ctx("&Enable Handler", "editor"), this, SLOT(toggleCurrentHandlerEnabled()));
			else
				m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::HandlerDisabled)),
				    __tr2qs_ctx("&Disable Handler", "editor"), this, SLOT(toggleCurrentHandlerEnabled()));

			m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)),
			    __tr2qs_ctx("Re&move Handler", "editor"), this, SLOT(removeCurrentHandler()));

			m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Save)),
			    __tr2qs_ctx("&Export Handler to...", "editor"), this, SLOT(exportCurrentHandler()));
		}
		else
		{
			m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Handler)),
			    __tr2qs_ctx("&New Handler", "editor"), this, SLOT(addHandlerForCurrentRaw()));
		}
	}

	m_pContextPopup->addSeparator();

	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::RawEvent)),
	    __tr2qs_ctx("&Add RAW Event...", "editor"), this, SLOT(addRaw()));
	m_pContextPopup->popup(mapToGlobal(QPoint(pos.x() + 15, pos.y())));
}

void RawEditorWidget::getUniqueHandlerName(RawTreeWidgetItem * it, QString & buffer)
{
	KVI_ASSERT(m_bOneTimeSetupDone);

	QString newName = buffer;
	if(newName.isEmpty())
		newName = __tr2qs_ctx("unnamed", "editor");

	bool bFound = true;
	int idx = 1;

	while(bFound)
	{
		bFound = false;

		for(int i = 0; i < it->childCount(); i++)
		{
			RawHandlerTreeWidgetItem * ch = (RawHandlerTreeWidgetItem *)it->child(i);
			if(KviQString::equalCI(newName, ch->text(0)))
			{
				bFound = true;
				newName = QString("%1_%2").arg(buffer).arg(idx);
				idx++;
				break;
			}
		}
	}

	buffer = newName;
}

void RawEditorWidget::addRaw()
{
	bool bOk = false;

	int iIdx = QInputDialog::getInt(this, __tr2qs_ctx("New RAW Event", "editor"), __tr2qs_ctx("Enter the numeric code of the message (0-999)", "editor"), 0, 0, 999, 1, &bOk);

	if(!bOk)
		return;

	RawTreeWidgetItem * it;
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		it = (RawTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if(((RawTreeWidgetItem *)it)->m_iIdx == iIdx)
		{
			m_pTreeWidget->setCurrentItem(it);
			m_pTreeWidget->clearSelection();
			it->setSelected(true);
			goto add_handler;
		}
	}

	it = new RawTreeWidgetItem(m_pTreeWidget, iIdx, true);
	m_pTreeWidget->setCurrentItem(it);
	m_pTreeWidget->clearSelection();
	it->setSelected(true);

add_handler:
	addHandlerForCurrentRaw();
}

void RawEditorWidget::addHandlerForCurrentRaw()
{
	KVI_ASSERT(m_bOneTimeSetupDone);

	RawTreeWidgetItem * it = (RawTreeWidgetItem *)m_pTreeWidget->currentItem();
	if(it)
	{
		if(it->parent() == nullptr)
		{
			QString buffer = __tr2qs_ctx("default", "editor");
			getUniqueHandlerName((RawTreeWidgetItem *)it, buffer);
			QTreeWidgetItem * ch = new RawHandlerTreeWidgetItem(it, buffer, "", true);
			it->setEnabled(true);
			it->setExpanded(true);
			m_pTreeWidget->setCurrentItem(ch);
			m_pTreeWidget->clearSelection();
			ch->setSelected(true);
		}
	}
}

void RawEditorWidget::removeCurrentHandler()
{
	KVI_ASSERT(m_bOneTimeSetupDone);
	if(m_pLastEditedItem)
	{
		QTreeWidgetItem * it = m_pLastEditedItem;
		RawTreeWidgetItem * parent = (RawTreeWidgetItem *)it->parent();
		m_pLastEditedItem = nullptr;
		delete it;
		m_pEditor->setEnabled(false);
		m_pNameEditor->setEnabled(false);
		if(!parent->childCount())
			parent->setEnabled(false);
	}
}

void RawEditorWidget::toggleCurrentHandlerEnabled()
{
	KVI_ASSERT(m_bOneTimeSetupDone);
	if(m_pLastEditedItem)
	{
		m_pLastEditedItem->m_bEnabled = !(m_pLastEditedItem->m_bEnabled);
		m_pLastEditedItem->setEnabled(m_pLastEditedItem->m_bEnabled);
		currentItemChanged(m_pLastEditedItem, m_pLastEditedItem);
	}
}

void RawEditorWidget::commit()
{
	if(!m_bOneTimeSetupDone)
		return; // nothing to commit

	saveLastEditedItem();
	KviKvsEventManager::instance()->removeAllScriptRawHandlers();
	QTreeWidgetItem * it;
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		it = (QTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if(it->childCount())
		{
			QString szContext;
			QTreeWidgetItem * ch;
			for(int j = 0; j < it->childCount(); j++)
			{
				ch = (QTreeWidgetItem *)it->child(j);
				qDebug("Commit handler %s", ((RawHandlerTreeWidgetItem *)ch)->text(0).toUtf8().data());
				//int a=(RawTreeWidgetItem *)it)->m_iIdx;
				szContext = QString("RawEvent%1::%2").arg(((RawTreeWidgetItem *)it)->m_iIdx).arg(((RawHandlerTreeWidgetItem *)ch)->text(0));
				auto * s = new KviKvsScriptEventHandler(
				    ((RawHandlerTreeWidgetItem *)ch)->text(0),
				    szContext,
				    ((RawHandlerTreeWidgetItem *)ch)->m_szBuffer,
				    ((RawHandlerTreeWidgetItem *)ch)->m_bEnabled);

				if(!KviKvsEventManager::instance()->addRawHandler(((RawTreeWidgetItem *)it)->m_iIdx, s))
					delete s;
			}
		}
	}

	g_pApp->saveRawEvents(); // otherwise this page has never been shown
}

void RawEditorWidget::saveLastEditedItem()
{
	KVI_ASSERT(m_bOneTimeSetupDone);
	if(!m_pLastEditedItem)
		return;

	QString buffer = m_pNameEditor->text();
	qDebug("Check lineedit name %s and internal %s", buffer.toUtf8().data(), m_pLastEditedItem->text(0).toUtf8().data());
	if(!KviQString::equalCI(buffer, m_pLastEditedItem->text(0)))
	{
		getUniqueHandlerName((RawTreeWidgetItem *)(m_pLastEditedItem->parent()), buffer);
		qDebug("Change name %s", buffer.toUtf8().data());
	}

	m_pLastEditedItem->setName(buffer);
	QString tmp;
	m_pEditor->getText(tmp);

	m_pLastEditedItem->m_szBuffer = tmp;
}

void RawEditorWidget::currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem *)
{
	KVI_ASSERT(m_bOneTimeSetupDone);
	saveLastEditedItem();
	if(it->parent())
	{
		m_pLastEditedItem = (RawHandlerTreeWidgetItem *)it;
		m_pNameEditor->setEnabled(true);
		m_pNameEditor->setText(it->text(0));
		m_pEditor->setEnabled(true);
		m_pEditor->setText(((RawHandlerTreeWidgetItem *)it)->m_szBuffer);
	}
	else
	{
		m_pLastEditedItem = nullptr;
		m_pNameEditor->setEnabled(false);
		m_pNameEditor->setText("");
		m_pEditor->setEnabled(false);
		QString szTmp = QString(__tr2qs_ctx("\n\nRAW Event:\n%1", "editor")).arg(((RawHandlerTreeWidgetItem *)it)->text(0));
		m_pEditor->setText(szTmp);
	}
}

void RawEditorWidget::showEvent(QShowEvent * e)
{
	oneTimeSetup();
	QWidget::showEvent(e);
}

void RawEditorWidget::getExportEventBuffer(QString & buffer, RawHandlerTreeWidgetItem * it)
{
	if(!it->parent())
		return;

	QString szBuf = it->m_szBuffer;

	KviCommandFormatter::blockFromBuffer(szBuf);

	buffer = "event(";
	buffer += ((RawTreeWidgetItem *)(it->parent()))->text(0);
	buffer += ",";
	buffer += it->text(0);
	buffer += ")\n";
	buffer += szBuf;
	buffer += "\n";

	if(!it->m_bEnabled)
	{
		buffer += "\n";
		buffer += "eventctl -d ";
		buffer += ((RawTreeWidgetItem *)(it->parent()))->text(0);
		buffer += " ";
		buffer += it->text(0);
	}
}

void RawEditorWidget::exportCurrentHandler()
{
	if(!m_pLastEditedItem)
		return;
	saveLastEditedItem();
	if(!m_pLastEditedItem)
		return;

	QString szName = QDir::homePath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))
		szName += KVI_PATH_SEPARATOR;
	szName += "raw";
	szName += ((RawTreeWidgetItem *)(m_pLastEditedItem->parent()))->text(0);
	szName += ".";
	szName += m_pLastEditedItem->text(0);
	szName += ".kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(szFile, __tr2qs_ctx("Choose a Filename - KVIrc", "editor"), szName, KVI_FILTER_SCRIPT, true, true, true, this))
		return;

	QString szOut;
	getExportEventBuffer(szOut, m_pLastEditedItem);

	if(!KviFileUtils::writeFile(szFile, szOut))
	{
		QMessageBox::warning(this, __tr2qs_ctx("Writing to File Failed - KVIrc", "editor"), __tr2qs_ctx("Unable to write to the RAW events file.", "editor"), __tr2qs_ctx("&OK", "editor"));
	}
}

void RawEditorWidget::exportAllEvents()
{
	saveLastEditedItem();

	RawTreeWidgetItem * it;
	QString out;

	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		it = (RawTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		RawHandlerTreeWidgetItem * item;
		for(int j = 0; j < it->childCount(); j++)
		{
			QString tmp;
			item = (RawHandlerTreeWidgetItem *)it->child(j);
			getExportEventBuffer(tmp, item);
			out += tmp;
			out += "\n";
		}
	}

	QString szName = QDir::homePath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))
		szName += KVI_PATH_SEPARATOR;
	szName += "rawevents.kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(szFile, __tr2qs_ctx("Choose a Filename - KVIrc", "editor"), szName, KVI_FILTER_SCRIPT, true, true, true, this))
		return;

	if(!KviFileUtils::writeFile(szFile, out))
	{
		QMessageBox::warning(this, __tr2qs_ctx("Writing to File Failed - KVIrc", "editor"), __tr2qs_ctx("Unable to write to the RAW events file.", "editor"), __tr2qs_ctx("OK", "editor"));
	}
}

RawEditorWindow::RawEditorWindow()
    : KviWindow(KviWindow::ScriptEditor, "raweditor", nullptr)
{
	g_pRawEditorWindow = this;

	QGridLayout * g = new QGridLayout();

	m_pEditor = new RawEditorWidget(this);
	g->addWidget(m_pEditor, 0, 0, 1, 4);

	QPushButton * btn = new QPushButton(__tr2qs_ctx("&OK", "editor"), this);
	connect(btn, SIGNAL(clicked()), this, SLOT(okClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	g->addWidget(btn, 1, 1);

	btn = new QPushButton(__tr2qs_ctx("&Apply", "editor"), this);
	connect(btn, SIGNAL(clicked()), this, SLOT(applyClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	g->addWidget(btn, 1, 2);

	btn = new QPushButton(__tr2qs_ctx("Cancel", "editor"), this);
	connect(btn, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)));
	g->addWidget(btn, 1, 3);

	g->setRowStretch(0, 1);
	g->setColumnStretch(0, 1);
	setLayout(g);
}

RawEditorWindow::~RawEditorWindow()
{
	g_pRawEditorWindow = nullptr;
}

void RawEditorWindow::okClicked()
{
	m_pEditor->commit();
	close();
}

void RawEditorWindow::applyClicked()
{
	m_pEditor->commit();
}

void RawEditorWindow::cancelClicked()
{
	close();
}

QPixmap * RawEditorWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::RawEventEditor);
}

void RawEditorWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("RAW Editor", "editor");
}

void RawEditorWindow::getConfigGroupName(QString & szName)
{
	szName = "raweditor";
}

void RawEditorWindow::saveProperties(KviConfigurationFile *)
{
}

void RawEditorWindow::loadProperties(KviConfigurationFile *)
{
}
