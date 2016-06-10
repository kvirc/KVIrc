//=============================================================================
//
//   File : OptionsWidget_textIcons.cpp
//   Creation date : Fri May 24 2002 00:16:13 CEST by Szymon Stefanek
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

#include "OptionsWidget_textIcons.h"

#include "KviIconManager.h"

#include "KviApplication.h"
#include "KviOptions.h"
#include "KviTextIconManager.h"
#include "KviLocale.h"
#include "KviFileDialog.h"
#include "kvi_fileextensions.h"
#include "KviTalHBox.h"
#include "KviFileUtils.h"

#include <QToolButton>
#include <QLayout>
#include <QCursor>
#include <QHeaderView>
#include <QWidgetAction>
#include <QMenu>

TextIconTableItem::TextIconTableItem(QTableWidget *, KviTextIcon * icon)
    : QTableWidgetItem(QString(), Qt::ItemIsEditable)
{
	if(icon)
		m_pIcon = icon;
	else
		m_pIcon = new KviTextIcon(KviIconManager::None);
	QPixmap * pix = m_pIcon->pixmap();
	if(pix)
		setIcon(QIcon(*pix));
}

TextIconTableItem::~TextIconTableItem()
{
	delete m_pIcon;
}

void TextIconTableItem::setId(int id)
{
	m_pIcon->setId(id);
	QPixmap * pix = m_pIcon->pixmap();
	if(pix)
		setIcon(QIcon(*pix));
}

OptionsWidget_textIcons::OptionsWidget_textIcons(QWidget * parent)
    : KviOptionsWidget(parent)
{
	m_pPopup = nullptr;
	m_iLastEditedRow = -1;
	m_pCurrentIconButton = nullptr;
	setObjectName("texticons_options_widget");
	createLayout();

	m_pTable = new QTableWidget(this);

	m_pTable->setColumnCount(2);
	m_pTable->setColumnWidth(0, 300);
	m_pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	m_pTable->horizontalHeader()->stretchLastSection();
	m_pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	mergeTip(m_pTable->viewport(), __tr2qs_ctx("This table contains the text icon associations.<br>"
	                                           "KVIrc will use them to display the Alt+E escape sequences "
	                                           "and eventually the emoticons.", "options"));

	layout()->addWidget(m_pTable, 0, 0, 1, 3);

	m_pAdd = new QPushButton(__tr2qs_ctx("Add", "options"), this);
	layout()->addWidget(m_pAdd, 1, 0);
	connect(m_pAdd, SIGNAL(clicked()), this, SLOT(addClicked()));

	m_pDel = new QPushButton(__tr2qs_ctx("Delete", "options"), this);
	layout()->addWidget(m_pDel, 1, 1);
	connect(m_pDel, SIGNAL(clicked()), this, SLOT(delClicked()));

	m_pRestore = new QPushButton(__tr2qs_ctx("Restore", "options"), this);
	layout()->addWidget(m_pRestore, 1, 2);
	connect(m_pRestore, SIGNAL(clicked()), this, SLOT(restoreClicked()));

	connect(m_pTable, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
	connect(m_pTable, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)));

	fillTable();
}

OptionsWidget_textIcons::~OptionsWidget_textIcons()
    = default;

void OptionsWidget_textIcons::fillTable()
{
	KviPointerHashTableIterator<QString, KviTextIcon> it(*(g_pTextIconManager->textIconDict()));

	m_pTable->clear();
	QStringList header;
	header.append(__tr2qs("Text"));
	header.append(__tr2qs("Emoticon"));
	m_pTable->setHorizontalHeaderLabels(header);
	m_pTable->setRowCount(g_pTextIconManager->textIconDict()->count());

	int idx = 0;
	QTableWidgetItem * item0;
	TextIconTableItem * item1;
	while(KviTextIcon * i = it.current())
	{
		if(!m_pTable->item(idx, 0))
		{
			item0 = new QTableWidgetItem(it.currentKey());
			m_pTable->setItem(idx, 0, item0);
		}

		item1 = new TextIconTableItem(m_pTable, new KviTextIcon(i));
		//remove from the item the ability to be edited as text
		item1->setFlags(item1->flags() ^ Qt::ItemIsEditable);
		m_pTable->setItem(idx, 1, item1);

		++idx;
		++it;
	}

	// disable the delete button
	m_pDel->setEnabled(false);
}

void OptionsWidget_textIcons::doPopup()
{
	if(!m_pPopup)
	{
		m_pPopup = new QMenu(this);
		KviIconWidget * iw = new KviIconWidget(m_pPopup);
		connect(iw, SIGNAL(selected(KviIconManager::SmallIcon)), this, SLOT(iconSelected(KviIconManager::SmallIcon)));
		QWidgetAction * pWaction = new QWidgetAction(m_pPopup);
		pWaction->setDefaultWidget(iw);
		m_pPopup->addAction(pWaction);
	}
	m_pPopup->popup(QCursor::pos());
}

void OptionsWidget_textIcons::iconSelected(KviIconManager::SmallIcon eIcon)
{
	m_pCurrentItem->icon()->setId(eIcon);
	m_pCurrentItem->setIcon(QIcon(*m_pCurrentItem->icon()->pixmap()));

	KviTalHBox * pBox = new KviTalHBox(nullptr);
	pBox->setSpacing(0);
	pBox->setMargin(0);

	m_pCurrentIconButton = new QToolButton(pBox);
	m_pCurrentIconButton->setMinimumWidth(90);
	m_pCurrentIconButton->setIcon(QIcon(*m_pCurrentItem->icon()->pixmap()));

	connect(m_pCurrentIconButton, SIGNAL(clicked()), this, SLOT(doPopup()));

	QToolButton * pBrowseButton = new QToolButton(pBox);
	pBrowseButton->setText(__tr2qs("&Browse..."));
	connect(pBrowseButton, SIGNAL(clicked()), this, SLOT(chooseFromFile()));

	m_pTable->setCellWidget(m_pCurrentItem->row(), 1, pBox);
}

void OptionsWidget_textIcons::chooseFromFile()
{
	QString szFile;
	KviFileDialog::askForOpenFileName(szFile, __tr2qs_ctx("Select a File - KVIrc", "options"), QString(), KVI_FILTER_IMAGE, false, true, this);
	if(szFile.isEmpty())
		return;

	if(!g_pIconManager->getPixmap(szFile))
		return;

	QFileInfo info(szFile);

	QString szFileName = info.fileName();

	QString szCurrentThemePath;
	g_pApp->getLocalKvircDirectory(szCurrentThemePath, KviApplication::Themes, KVI_OPTION_STRING(KviOption_stringIconThemeSubdir));

	szCurrentThemePath += KVI_PATH_SEPARATOR_CHAR;

	if(!KviFileUtils::directoryExists(szCurrentThemePath))
		KviFileUtils::makeDir(szCurrentThemePath);

	KviFileUtils::copyFile(szFile, szCurrentThemePath + szFileName);

	m_pCurrentItem->icon()->setFilename(szFileName);

	QPixmap * p = m_pCurrentItem->icon()->pixmap();
	m_pCurrentItem->setIcon(QIcon(*p));

	if(m_pCurrentIconButton)
		m_pCurrentIconButton->setIcon(QIcon(*p));
}

void OptionsWidget_textIcons::itemSelectionChanged()
{
	int i = m_pTable->currentRow();
	m_pDel->setEnabled(i >= 0 && i < m_pTable->rowCount());
}

void OptionsWidget_textIcons::currentItemChanged(QTableWidgetItem * cur, QTableWidgetItem * prev)
{
	if(prev)
	{
		QWidget * pOldWidget = m_pTable->cellWidget(prev->row(), 1);
		if(pOldWidget)
			m_pTable->setCellWidget(prev->row(), 1, nullptr);
	}

	m_pCurrentItem = nullptr;

	if(!cur)
		return;
	if(cur->column() != 1)
		return;
	if(m_iLastEditedRow == cur->row() || cur == prev)
		return;

	m_pCurrentItem = (TextIconTableItem *)cur;

	KviTalHBox * pBox = new KviTalHBox(nullptr);
	pBox->setSpacing(0);
	pBox->setMargin(0);

	m_pCurrentIconButton = new QToolButton(pBox);
	m_pCurrentIconButton->setMinimumWidth(90);
	m_pCurrentIconButton->setIcon(QIcon(cur->icon()));
	m_pCurrentIconButton->setText(__tr2qs("&Select..."));
	connect(m_pCurrentIconButton, SIGNAL(clicked()), this, SLOT(doPopup()));

	QToolButton * pBrowseButton = new QToolButton(pBox);
	pBrowseButton->setText(__tr2qs("&Browse..."));
	connect(pBrowseButton, SIGNAL(clicked()), this, SLOT(chooseFromFile()));

	m_pTable->setCellWidget(cur->row(), 1, pBox);
	m_iLastEditedRow = cur->row();
}

void OptionsWidget_textIcons::addClicked()
{
	m_pTable->setRowCount(m_pTable->rowCount() + 1);
	m_pTable->setItem(m_pTable->rowCount() - 1, 0, new QTableWidgetItem(__tr2qs_ctx("unnamed", "options")));
	m_pTable->setItem(m_pTable->rowCount() - 1, 1, new TextIconTableItem(m_pTable, nullptr));
	m_pTable->scrollToBottom();
	m_pDel->setEnabled(true);
}

void OptionsWidget_textIcons::restoreClicked()
{
	g_pTextIconManager->checkDefaultAssociations();
	fillTable();
}

void OptionsWidget_textIcons::delClicked()
{
	int i = m_pTable->currentRow();

	if((i > -1) && (i < m_pTable->rowCount()))
	{
		m_pTable->removeRow(i);
		if(m_pTable->rowCount() == 0)
			m_pDel->setEnabled(false);
	}
}

void OptionsWidget_textIcons::commit()
{
	KviOptionsWidget::commit();
	g_pTextIconManager->clear();
	int n = m_pTable->rowCount();
	for(int i = 0; i < n; i++)
	{
		QString szVal = m_pTable->item(i, 0)->text();
		if(!szVal.isEmpty())
		{
			TextIconTableItem * it = (TextIconTableItem *)m_pTable->item(i, 1);
			if(it)
			{
				g_pTextIconManager->insert(szVal, *(it->icon()));
			}
		}
	}
	g_pTextIconManager->save();

	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < m_pTable->columnCount(); j++)
		{
			if(m_pTable->item(i, j))
				m_pTable->takeItem(i, j);
		}
	}
}
