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

#include <qlayout.h>
#include <qcursor.h>
#include <qhbox.h>


KviTextIconEditor::KviTextIconEditor(QWidget * par,KviTextIcon * icon,KviTextIconTableItem* item)
: QHBox(par),m_pTableItem(item)
{
	m_pIconButton=new QToolButton(this);
	m_pBrowseButton=new QToolButton(this);
	m_pBrowseButton->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum));
	m_pBrowseButton->setText("...");
	m_pIcon = icon;
	m_pPopup = 0;
	updateIcon();
	connect(m_pIconButton,SIGNAL(clicked()),this,SLOT(doPopup()));
	connect(m_pBrowseButton,SIGNAL(clicked()),this,SLOT(chooseFromFile()));
}


KviTextIconEditor::~KviTextIconEditor()
{
	
}

void KviTextIconEditor::doPopup()
{
	if(!m_pPopup)
	{
		m_pPopup = new QPopupMenu(this);
		KviIconWidget * iw = new KviIconWidget(m_pPopup);
		connect(iw,SIGNAL(selected(int)),this,SLOT(iconSelected(int)));
		m_pPopup->insertItem(iw);
	}
	m_pPopup->popup(QCursor::pos());
}

void KviTextIconEditor::iconSelected(int id)
{
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
//			debug("%s %s %i |%s| %p",__FILE__,__FUNCTION__,__LINE__,m_pIcon->filename().utf8().data(),m_pIcon);
			updateIcon();
		}
	}
}

void KviTextIconEditor::updateIcon()
{
	if(m_pIcon)
	{
		QPixmap* pix=m_pIcon->pixmap();
		if(pix)
			m_pIconButton->setPixmap(*pix);
	}
}

KviTextIconTableItem::KviTextIconTableItem(QTable * t,KviTextIcon * icon)
: QTableItem(t,QTableItem::WhenCurrent,QString::null)
{
	if(icon)
		m_pIcon=icon;
	else
		m_pIcon=new KviTextIcon(0);
	QPixmap* pix=m_pIcon->pixmap();
	if(pix)
		setPixmap(*pix);
}

KviTextIconTableItem::~KviTextIconTableItem()
{
	delete m_pIcon;
}

void KviTextIconTableItem::setId(int id)
{
	m_pIcon->setId(id);
	QPixmap* pix=m_pIcon->pixmap();
	if(pix)
		setPixmap(*pix);
}

QWidget * KviTextIconTableItem::createEditor() const
{
	return new KviTextIconEditor(table()->viewport(),m_pIcon,(KviTextIconTableItem*)this);
}


void KviTextIconTableItem::setContentFromEditor(QWidget * w)
{
	if(w->inherits("KviTextIconEditor"))
	{
		QPixmap* pix=m_pIcon->pixmap();
		if(pix)
			setPixmap(*pix);
	}
}

KviTextIconsOptionsWidget::KviTextIconsOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"texticons_options_widget")
{
	createLayout(2,2);

	KviDictIterator<KviTextIcon> it(*(g_pTextIconManager->textIconDict()));

	m_pTable = new QTable(g_pTextIconManager->textIconDict()->count(),2,this);

#ifdef COMPILE_INFO_TIPS
	mergeTip(m_pTable->viewport(),__tr2qs_ctx("This table contains the text icon associations.<br>" \
			"KVirc will use them to display the CTRL+I escape sequences and eventually the " \
			"emoticons.","options"));
#endif

	int idx = 0;
	while(KviTextIcon * i = it.current())
	{
		m_pTable->setText(idx,0,it.currentKey());
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

	connect(m_pTable,SIGNAL(selectionChanged()),this,SLOT(selectionChanged()));
}

KviTextIconsOptionsWidget::~KviTextIconsOptionsWidget()
{
}

void KviTextIconsOptionsWidget::selectionChanged()
{
	int i = m_pTable->currentRow();
	m_pDel->setEnabled(i >= 0 && i < m_pTable->numRows());
}

void KviTextIconsOptionsWidget::addClicked()
{
	m_pTable->setNumRows(m_pTable->numRows() + 1);
	m_pTable->setText(m_pTable->numRows() - 1,0,__tr2qs_ctx("unnamed","options"));
	m_pTable->setItem(m_pTable->numRows() - 1,1,new KviTextIconTableItem(m_pTable,0));
	m_pDel->setEnabled(true);
}

void KviTextIconsOptionsWidget::delClicked()
{
	int i = m_pTable->currentRow();

	if((i > -1) && (i < m_pTable->numRows()))
	{
		// remove row i
		m_pTable->clearCell(i,0);
		m_pTable->clearCell(i,1);
		m_pTable->clearCell(i,2);

		for(;i < (m_pTable->numRows() - 1);i++)
		{
			m_pTable->swapRows(i,i+1);
		}

		m_pTable->setNumRows(m_pTable->numRows() - 1);
		if(m_pTable->numRows() == 0)m_pDel->setEnabled(false);
	}
}

void KviTextIconsOptionsWidget::commit()
{
	KviOptionsWidget::commit();
	g_pTextIconManager->clear();
	int n = m_pTable->numRows();
	for(int i=0;i < n;i++)
	{
		QString szVal = m_pTable->text(i,0);
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
	
	for(int i=0;i < n;i++)
	{
		m_pTable->clearCell(i,0);
		m_pTable->clearCell(i,1);
		m_pTable->clearCell(i,2);
	}
}

#include "m_optw_texticons.moc"
