//
//   File : optw_texticons.cpp
//   Creation date : Fri May 24 2002 00:16:13 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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

#include "optw_texticons.h"

#include "kvi_iconmanager.h"
#include "kvi_texticonmanager.h"
#include "kvi_locale.h"
#include "kvi_filedialog.h"
#include "kvi_tal_hbox.h"

#include <QLayout>
#include <QCursor>
#include <QTableWidget>
#include <QTableWidgetItem>



QWidget * KviTextIconQt4ModelViewParadigmIsRidiculouslyComplexAndUglyTableItemDelegate::createEditor(QWidget * parent,const QStyleOptionViewItem & option,const QModelIndex & index) const
{
	if(index.column() != 1)
		return QItemDelegate::createEditor(parent,option,index);
	return new KviTextIconEditor(parent);
}

void KviTextIconQt4ModelViewParadigmIsRidiculouslyComplexAndUglyTableItemDelegate::setEditorData(QWidget * editor,const QModelIndex & index) const
{
	if(index.column() != 1)
	{
		QItemDelegate::setEditorData(editor,index);
		return;
	}

	if(!editor)
		return;
	if(!editor->inherits("KviTextIconEditor"))
		return;

	KviTextIconEditor * pEditor = (KviTextIconEditor *)editor;


	// FIXME: Should use dynamic_cast<> here (once we know it's really safe to use it on all the platforms)
	KviTextIconTableItem * pItem = static_cast<KviTextIconTableItem *>(m_pTableWidget->item(index.row(),index.column()));
	if(!pItem)
		return;

	pEditor->setIcon(pItem->icon());
	pEditor->updateIcon();
}

void KviTextIconQt4ModelViewParadigmIsRidiculouslyComplexAndUglyTableItemDelegate::setModelData(QWidget * editor,QAbstractItemModel * model,const QModelIndex & index) const
{
	if(index.column() != 1)
	{
		QItemDelegate::setModelData(editor,model,index);
		return;
	}

	if(!editor)
		return;
	if(!editor->inherits("KviTextIconEditor"))
		return;

	KviTextIconEditor * pEditor = (KviTextIconEditor *)editor;

	// FIXME: Should use dynamic_cast<> here (once we know it's really safe to use it on all the platforms)
	KviTextIconTableItem * pItem = static_cast<KviTextIconTableItem *>(m_pTableWidget->item(index.row(),index.column()));
	if(!pItem)
		return;
		
	pItem->setContentFromEditor(pEditor);
}

void KviTextIconQt4ModelViewParadigmIsRidiculouslyComplexAndUglyTableItemDelegate::updateEditorGeometry(QWidget * editor,const QStyleOptionViewItem & option,const QModelIndex & index) const
{
	if(index.column() != 1)
	{
		QItemDelegate::updateEditorGeometry(editor,option,index);
		return;
	}

	if(!editor)
		return;
	if(!editor->inherits("KviTextIconEditor"))
		return;

	KviTextIconEditor * pEditor = (KviTextIconEditor *)editor;

	// FIXME: Should use dynamic_cast<> here (once we know it's really safe to use it on all the platforms)
	KviTextIconTableItem * pItem = static_cast<KviTextIconTableItem *>(m_pTableWidget->item(index.row(),index.column()));
	if(!pItem)
		return;

	pEditor->setGeometry(m_pTableWidget->visualItemRect(pItem));
}

KviTextIconEditor::KviTextIconEditor(QWidget * par)
: KviTalHBox(par)
{
	m_pIconButton=new QToolButton(this);
	m_pBrowseButton=new QToolButton(this);
	m_pBrowseButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	m_pBrowseButton->setText("...");
	m_pIcon = NULL;
	m_pPopup = 0;
	updateIcon();
	connect(m_pIconButton,SIGNAL(clicked()),this,SLOT(doPopup()));
	connect(m_pBrowseButton,SIGNAL(clicked()),this,SLOT(chooseFromFile()));
	setMargin(0);
	setSpacing(0);

	// I've spend more than 1 hour to find out the following line.
	// Without this the editor disappears on the first click and no clicked() events are forwarded to the buttons.
	// It's very ugly.
	setFocusPolicy(Qt::ClickFocus);
}


KviTextIconEditor::~KviTextIconEditor()
{
	
}

void KviTextIconEditor::doPopup()
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

void KviTextIconEditor::iconSelected(int id)
{
	if(!m_pIcon)
		return;
	m_pIcon->setId(id);
	updateIcon();
}

void KviTextIconEditor::chooseFromFile()
{
	QString szFile;
	KviFileDialog::askForOpenFileName(szFile,"Choose icon filename",QString::null,"*.png","options");
	if(!szFile.isEmpty())
	{
		if(g_pIconManager->getPixmap(szFile))
		{
			m_pIcon->setFilename(szFile);
//			qDebug("%s %s %i |%s| %p",__FILE__,__FUNCTION__,__LINE__,m_pIcon->filename().utf8().data(),m_pIcon);
			updateIcon();
		}
	}
}

void KviTextIconEditor::updateIcon()
{
	if(!m_pIcon)
		return;
	QPixmap* pix=m_pIcon->pixmap();
	if(pix) m_pIconButton->setPixmap(*pix);
}

KviTextIconTableItem::KviTextIconTableItem(QTableWidget * t,KviTextIcon * icon)
: QTableWidgetItem(QString::null,Qt::ItemIsEditable)
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

/*QWidget * KviTextIconTableItem::createEditor() const
{
	return new KviTextIconEditor(tableWidget()->viewport(),m_pIcon,(KviTextIconTableItem*)this);
}*/

void KviTextIconTableItem::setContentFromEditor(QWidget * w)
{
	if(w->inherits("KviTextIconEditor"))
	{
		QPixmap* pix=m_pIcon->pixmap();
		if(pix) setIcon(QIcon(*pix));
	}
}

KviTextIconsOptionsWidget::KviTextIconsOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("texticons_options_widget");
	createLayout(2,2);

	KviPointerHashTableIterator<QString,KviTextIcon> it(*(g_pTextIconManager->textIconDict()));

	m_pTable = new QTableWidget(g_pTextIconManager->textIconDict()->count(),2,this);

	mergeTip(m_pTable->viewport(),__tr2qs_ctx("This table contains the text icon associations.<br>" \
			"KVirc will use them to display the CTRL+I escape sequences and eventually the " \
			"emoticons.","options"));


	m_pTable->setItemDelegateForColumn(1,new KviTextIconQt4ModelViewParadigmIsRidiculouslyComplexAndUglyTableItemDelegate(m_pTable));

	int idx = 0;
	while(KviTextIcon * i = it.current())
	{
		if(!m_pTable->item(idx,0)){
			m_pTable->setItem(idx,0,new QTableWidgetItem(it.currentKey()));
		}

		m_pTable->setItem(idx,1,new KviTextIconTableItem(m_pTable,new KviTextIcon(i)));
		++idx;
		++it;
	}

	layout()->addMultiCellWidget(m_pTable,0,0,0,1);

	m_pAdd = new QPushButton(__tr2qs_ctx("Add","options"),this);
	layout()->addWidget(m_pAdd,1,0);
	connect(m_pAdd,SIGNAL(clicked()),this,SLOT(addClicked()));

	m_pDel = new QPushButton(__tr2qs_ctx("Delete","options"),this);
	layout()->addWidget(m_pDel,1,1);
	connect(m_pDel,SIGNAL(clicked()),this,SLOT(delClicked()));

	m_pDel->setEnabled(false);

	connect(m_pTable,SIGNAL(itemSelectionChanged()),this,SLOT(itemSelectionChanged()));
}

KviTextIconsOptionsWidget::~KviTextIconsOptionsWidget()
{
}

void KviTextIconsOptionsWidget::itemSelectionChanged()
{
	int i = m_pTable->currentRow();
	m_pDel->setEnabled(i >= 0 && i < m_pTable->rowCount());
}

void KviTextIconsOptionsWidget::addClicked()
{
	m_pTable->setRowCount(m_pTable->rowCount() + 1);
	//m_pTable->item(m_pTable->rowCount() - 1,0)->setText(__tr2qs_ctx("unnamed","options"));
	m_pTable->setItem(m_pTable->rowCount() - 1,0,new QTableWidgetItem(__tr2qs_ctx("unnamed","options")));
	m_pTable->setItem(m_pTable->rowCount() - 1,1,new KviTextIconTableItem(m_pTable,0));
	m_pDel->setEnabled(true);
}

void KviTextIconsOptionsWidget::delClicked()
{
	int i = m_pTable->currentRow();

	if((i > -1) && (i < m_pTable->rowCount()))
	{
		// remove row i
		m_pTable->takeItem(i,0);
		m_pTable->takeItem(i,1);
		m_pTable->takeItem(i,2);

		for(;i < (m_pTable->rowCount() - 1);i++)
		{
			// Get first line data
			QString cellData1 = m_pTable->item(i,0)->text();
			QString cellData2 = m_pTable->item(i,1)->text();

			// Fill in first line with second line data
			m_pTable->item(i,0)->setText(m_pTable->item(i+1,0)->text());
			m_pTable->item(i,1)->setText(m_pTable->item(i+1,1)->text());

			// Put first line data in second line
			m_pTable->item(i+1,0)->setText(cellData1);
			m_pTable->item(i+1,1)->setText(cellData2);
		}

		m_pTable->setRowCount(m_pTable->rowCount() - 1);
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
