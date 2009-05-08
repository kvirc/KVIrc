//=============================================================================
//
//   File : optw_texticons.cpp
//   Creation date : Fri May 24 2002 00:16:13 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "optw_texticons.h"

#include "kvi_iconmanager.h"
#include "kvi_texticonmanager.h"
#include "kvi_locale.h"
#include "kvi_filedialog.h"
#include "kvi_fileextensions.h"
#include "kvi_tal_hbox.h"

#include <QLayout>
#include <QCursor>
#include <QHeaderView>

KviTextIconTableItem::KviTextIconTableItem(QTableWidget *,KviTextIcon * icon)
: QTableWidgetItem(QString(),Qt::ItemIsEditable)
{
	if(icon)
		m_pIcon=icon;
	else
		m_pIcon=new KviTextIcon(0);
	QPixmap* pix=m_pIcon->pixmap();
	if(pix) setIcon(QIcon(*pix));
}

KviTextIconTableItem::~KviTextIconTableItem()
{
	delete m_pIcon;

}

void KviTextIconTableItem::setId(int id)
{
	m_pIcon->setId(id);
	QPixmap* pix=m_pIcon->pixmap();
	if(pix) setIcon(QIcon(*pix));
}


KviTextIconsOptionsWidget::KviTextIconsOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	m_pBox=0;
	m_pIconButton=0;
	m_pBrowseButton=0;
	m_pPopup=0;
	m_iLastEditedRow=-1;

	setObjectName("texticons_options_widget");
	createLayout();

	KviPointerHashTableIterator<QString,KviTextIcon> it(*(g_pTextIconManager->textIconDict()));

	m_pTable = new QTableWidget(g_pTextIconManager->textIconDict()->count(),2,this);

	QStringList header;
	header.append(__tr2qs("Text"));
	header.append(__tr2qs("Emoticon"));

	m_pTable->setColumnWidth(0,300);
	m_pTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	m_pTable->horizontalHeader()->stretchLastSection();
	m_pTable->setHorizontalHeaderLabels(header);
	m_pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	mergeTip(m_pTable->viewport(),__tr2qs_ctx("This table contains the text icon associations.<br>" \
			"KVirc will use them to display the CTRL+I escape sequences and eventually the " \
			"emoticons.","options"));

	int idx = 0;
	QTableWidgetItem *item0;
	KviTextIconTableItem *item1;
	while(KviTextIcon * i = it.current())
	{
		if(!m_pTable->item(idx,0))
		{
			item0 = new QTableWidgetItem(it.currentKey());
			m_pTable->setItem(idx,0,item0);
		}

		item1 = new KviTextIconTableItem(m_pTable,new KviTextIcon(i));
		//remove from the item the ability to be edited as text
		item1->setFlags(item1->flags() ^ Qt::ItemIsEditable);
		m_pTable->setItem(idx,1,item1);

		++idx;
		++it;
	}

	layout()->addWidget(m_pTable,0,0,1,2);

	m_pAdd = new QPushButton(__tr2qs_ctx("Add","options"),this);
	layout()->addWidget(m_pAdd,1,0);
	connect(m_pAdd,SIGNAL(clicked()),this,SLOT(addClicked()));

	m_pDel = new QPushButton(__tr2qs_ctx("Delete","options"),this);
	layout()->addWidget(m_pDel,1,1);
	connect(m_pDel,SIGNAL(clicked()),this,SLOT(delClicked()));

	m_pDel->setEnabled(false);

	connect(m_pTable,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged()));
	connect(m_pTable,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(itemClicked(QTableWidgetItem *)));
}

KviTextIconsOptionsWidget::~KviTextIconsOptionsWidget()
{
	if(m_pIconButton)
		delete m_pIconButton;
	if(m_pBrowseButton)
		delete m_pBrowseButton;
	if (m_pBox)
		delete m_pBox;
}


void KviTextIconsOptionsWidget::doPopup()
{
	if(!m_pPopup)
	{
		m_pPopup = new KviTalPopupMenu(this);
		KviIconWidget * iw = new KviIconWidget(m_pPopup);
		connect(iw,SIGNAL(selected(int)),this,SLOT(iconSelected(int)));
		m_pPopup->insertItem(iw);
	}
	m_pPopup->popup(QCursor::pos());
}

void KviTextIconsOptionsWidget::iconSelected(int id)
{
	m_pItem->icon()->setId(id);
	m_pItem->setIcon(QIcon(*m_pItem->icon()->pixmap()));
	m_pIconButton->setIcon(QIcon(*m_pItem->icon()->pixmap()));
}

void KviTextIconsOptionsWidget::chooseFromFile()
{
	QString szFile;
	KviFileDialog::askForOpenFileName(szFile,"Choose icon filename",QString(),KVI_FILTER_IMAGE,"options");
	if(!szFile.isEmpty())
	{
		if(g_pIconManager->getPixmap(szFile))
		{
			m_pItem->icon()->setFilename(szFile);
			m_pItem->setIcon(QIcon(*m_pItem->icon()->pixmap()));
			m_pIconButton->setIcon(QIcon(*m_pItem->icon()->pixmap()));
		}
	}
}
void KviTextIconsOptionsWidget::itemSelectionChanged()
{
	int i = m_pTable->currentRow();
	m_pDel->setEnabled(i >= 0 && i < m_pTable->rowCount());
}
void KviTextIconsOptionsWidget::itemClicked(QTableWidgetItem *i)
{
	if (i->column()!=1) return;
	if (m_iLastEditedRow==i->row()) return;

	m_pItem=(KviTextIconTableItem *)i;

	if(m_pIconButton)
		delete m_pIconButton;
	if(m_pBrowseButton)
		delete m_pBrowseButton;
	if (m_pBox)
		delete m_pBox;

	m_pBox=new KviTalHBox(0);
	m_pBox->setSpacing(0);
	m_pBox->setMargin(0);

	m_pIconButton=new QToolButton(m_pBox);
	m_pIconButton->setMinimumWidth(150);
	m_pIconButton->setIcon(QIcon(i->icon()));
	connect(m_pIconButton,SIGNAL(clicked()),this,SLOT(doPopup()));

	m_pBrowseButton=new QToolButton(m_pBox);
	m_pBrowseButton->setText("...");
	connect(m_pBrowseButton,SIGNAL(clicked()),this,SLOT(chooseFromFile()));

	m_pTable->setCellWidget(i->row(),1,m_pBox);
	m_iLastEditedRow=i->row();
}

void KviTextIconsOptionsWidget::addClicked()
{
	m_pTable->setRowCount(m_pTable->rowCount() + 1);
	m_pTable->setItem(m_pTable->rowCount() - 1,0,new QTableWidgetItem(__tr2qs_ctx("unnamed","options")));
	m_pTable->setItem(m_pTable->rowCount() - 1,1,new KviTextIconTableItem(m_pTable,0));
	m_pTable->scrollToBottom();
	m_pDel->setEnabled(true);
}

void KviTextIconsOptionsWidget::delClicked()
{
	int i = m_pTable->currentRow();

	if((i > -1) && (i < m_pTable->rowCount()))
	{
		if(m_pIconButton)
		{
			delete m_pIconButton;
			m_pIconButton=0;
		}
		if(m_pBrowseButton)
		{
			delete m_pBrowseButton;
			m_pBrowseButton=0;
		}
		if (m_pBox)
		{
			delete m_pBox;
			m_pBox=0;
		}
		m_pTable->removeRow(i);
		if(m_pTable->rowCount() == 0) m_pDel->setEnabled(false);
	}
}

void KviTextIconsOptionsWidget::commit()
{

	KviOptionsWidget::commit();
	g_pTextIconManager->clear();
	int n = m_pTable->rowCount();
	for(int i=0;i < n;i++)
	{
		QString szVal = m_pTable->item(i,0)->text();
		if(!szVal.isEmpty())
		{
			KviTextIconTableItem * it = (KviTextIconTableItem *)m_pTable->item(i,1);
			if(it)
			{
				g_pTextIconManager->insert(szVal,*(it->icon()));
			}
		}
	}
	g_pTextIconManager->checkDefaultAssociations();

	for(int i=0; i<n; i++)
		for (int j=0; j<m_pTable->columnCount(); j++)
			if (m_pTable->item(i,j)) m_pTable->takeItem(i,j);

}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_texticons.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
