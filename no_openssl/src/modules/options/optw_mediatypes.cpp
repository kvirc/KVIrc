//=============================================================================
//
//   File : optw_mediatypes.cpp
//   Creation date : Sat Jan 20 2001 16:14:53 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include "optw_mediatypes.h"

#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_tal_tooltip.h"

#include <QLayout>
#include <QLabel>

// kvi_app.cpp
extern KVIRC_API KviMediaManager * g_pMediaManager;

static void copyMediaType(KviMediaType * dst,const KviMediaType * src)
{
	dst->szFileMask              = src->szFileMask;
	dst->szIanaType              = src->szIanaType;
	dst->szDescription           = src->szDescription;
	dst->szMagicBytes            = src->szMagicBytes;
	dst->szSavePath              = src->szSavePath;
	dst->szCommandline           = src->szCommandline;
	dst->szRemoteExecCommandline = src->szRemoteExecCommandline;
	dst->szIcon                  = src->szIcon;
}

KviMediaTypeTreeWidgetItem::KviMediaTypeTreeWidgetItem(QTreeWidget * w,KviMediaType * t)
: QTreeWidgetItem(w)
{
	copyData(t);
}

KviMediaTypeTreeWidgetItem::~KviMediaTypeTreeWidgetItem()
{
}

void KviMediaTypeTreeWidgetItem::copyData(KviMediaType * t)
{
	copyMediaType(&m_data,t);
	setText(0,m_data.szFileMask.ptr());
	setText(1,m_data.szIanaType.ptr());
	setText(2,m_data.szDescription.ptr());
}

KviMediaTypesOptionsWidget::KviMediaTypesOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("mediatypes_options_widget");

	createLayout();

	m_pTreeWidget = new QTreeWidget(this);
	m_pTreeWidget->setColumnCount(3);
	QStringList columnLabels;
	columnLabels.append(__tr2qs_ctx("Pattern","options"));
	columnLabels.append(__tr2qs_ctx("MIME Type","options"));
	columnLabels.append(__tr2qs_ctx("Description","options"));
	m_pTreeWidget->setHeaderLabels(columnLabels);
	m_pTreeWidget->setAllColumnsShowFocus(true);
	m_pTreeWidget->setRootIsDecorated(false);
	connect(m_pTreeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));

	layout()->addWidget(m_pTreeWidget,0,0,1,3);
//	layout()->addMultiCellWidget(m_pTreeWidget,0,0,0,2);

	QLabel * l = new QLabel(__tr2qs_ctx("Description:","options"),this);
	layout()->addWidget(l,1,0);
	m_pDescription = new QLineEdit(this);
	layout()->addWidget(m_pDescription,1,1,1,2);
//	layout()->addMultiCellWidget(m_pDescription,1,1,1,2);

	l = new QLabel(__tr2qs_ctx("MIME type:","options"),this);
	layout()->addWidget(l,2,0);
	m_pIanaType = new QLineEdit(this);
	layout()->addWidget(m_pIanaType,2,1,1,2);
//	layout()->addMultiCellWidget(m_pIanaType,2,2,1,2);

	l = new QLabel(__tr2qs_ctx("File pattern:","options"),this);
	layout()->addWidget(l,3,0);
	m_pFileMask = new QLineEdit(this);
	layout()->addWidget(m_pFileMask,3,1,1,2);
//	layout()->addMultiCellWidget(m_pFileMask,3,3,1,2);

	l = new QLabel(__tr2qs_ctx("Magic bytes:","options"),this);
	layout()->addWidget(l,4,0);
	m_pMagicBytes = new QLineEdit(this);
	layout()->addWidget(m_pMagicBytes,4,1,1,2);
//	layout()->addMultiCellWidget(m_pMagicBytes,4,4,1,2);

	l = new QLabel(__tr2qs_ctx("Save path:","options"),this);
	layout()->addWidget(l,5,0);
	m_pSavePath = new QLineEdit(this);
	layout()->addWidget(m_pSavePath,5,1,1,2);
//	layout()->addMultiCellWidget(m_pSavePath,5,5,1,2);

	l = new QLabel(__tr2qs_ctx("Local open command:","options"),this);
	layout()->addWidget(l,6,0);
	m_pCommandline = new QLineEdit(this);
	layout()->addWidget(m_pCommandline,6,1,1,2);
//	layout()->addMultiCellWidget(m_pCommandline,6,6,1,2);
	mergeTip(m_pCommandline,__tr2qs_ctx("<center>This field contains the command to execute to open a local file.<br>" \
		"<tt>$0</tt> is used in place of the filename</center>","options"));

	l = new QLabel(__tr2qs_ctx("Remote open command:","options"),this);
	layout()->addWidget(l,7,0);
	m_pRemoteExecCommandline = new QLineEdit(this);
	layout()->addWidget(m_pRemoteExecCommandline,7,1,1,2);
//	layout()->addMultiCellWidget(m_pRemoteExecCommandline,7,7,1,2);
	mergeTip(m_pRemoteExecCommandline,__tr2qs_ctx("<center>This field contains the command to execute when automatically opening a received file.<br>" \
		"<tt>$0</tt> is used in place of the filename</center>","options"));

	l = new QLabel(__tr2qs_ctx("Icon","options"),this);
	layout()->addWidget(l,8,0);
	m_pIcon = new QLineEdit(this);
	layout()->addWidget(m_pIcon,8,1,1,2);
//	layout()->addMultiCellWidget(m_pIcon,8,8,1,2);

	QFrame * f = new QFrame(this);
	f->setFrameStyle(QFrame::Sunken | QFrame::HLine);
	layout()->addWidget(f,9,0,1,3);
//	layout()->addMultiCellWidget(f,9,9,0,2);

	QPushButton * b = new QPushButton(__tr2qs_ctx("&New","options"),this);
	connect(b,SIGNAL(clicked()),this,SLOT(newMediaType()));
	layout()->addWidget(b,10,1);

	m_pDelete = new QPushButton(__tr2qs_ctx("Re&move","options"),this);
	connect(m_pDelete,SIGNAL(clicked()),this,SLOT(delMediaType()));
	layout()->addWidget(m_pDelete,10,2);

	layout()->setColumnStretch(1,1);
	layout()->setColumnStretch(2,1);
	layout()->setRowStretch(0,1);

	m_pLastItem = 0;

	fillTreeWidget();
}

KviMediaTypesOptionsWidget::~KviMediaTypesOptionsWidget()
{
}

void KviMediaTypesOptionsWidget::fillTreeWidget()
{
	m_pTreeWidget->clear();
	g_pMediaManager->lock();
	KviPointerList<KviMediaType> * l = g_pMediaManager->mediaTypeList();
	KviMediaTypeTreeWidgetItem * it;
	for(KviMediaType * t = l->first();t;t = l->next())
	{
		it = new KviMediaTypeTreeWidgetItem(m_pTreeWidget,t);
	}
	g_pMediaManager->unlock();
	enableOrDisable();
}

void KviMediaTypesOptionsWidget::saveLastItem()
{
	if(m_pLastItem)
	{
		KviMediaType t;
		t.szDescription           = m_pDescription->text();
		t.szIanaType              = m_pIanaType->text();
		t.szFileMask              = m_pFileMask->text();
		t.szSavePath              = m_pSavePath->text();
		t.szCommandline           = m_pCommandline->text();
		t.szMagicBytes            = m_pMagicBytes->text();
		t.szRemoteExecCommandline = m_pRemoteExecCommandline->text();
		t.szIcon                  = m_pIcon->text();

		if(t.szDescription.isEmpty())t.szDescription = __tr2qs_ctx("[Unknown Media Type]","options");

		m_pLastItem->copyData(&t);
	}
}

void KviMediaTypesOptionsWidget::enableOrDisable()
{
	m_pDescription->setEnabled(m_pLastItem);
	m_pIanaType->setEnabled(m_pLastItem);
	m_pFileMask->setEnabled(m_pLastItem);
	m_pSavePath->setEnabled(m_pLastItem);
	m_pCommandline->setEnabled(m_pLastItem);
	m_pMagicBytes->setEnabled(m_pLastItem);
	m_pRemoteExecCommandline->setEnabled(m_pLastItem);
	m_pDelete->setEnabled(m_pLastItem);
	m_pIcon->setEnabled(m_pLastItem);
}

void KviMediaTypesOptionsWidget::setLineEdits()
{
	m_pDescription->setText(m_pLastItem ? m_pLastItem->mydata()->szDescription.ptr() : "");
	m_pIanaType->setText(m_pLastItem ? m_pLastItem->mydata()->szIanaType.ptr() : "");
	m_pFileMask->setText(m_pLastItem ? m_pLastItem->mydata()->szFileMask.ptr() : "");
	m_pSavePath->setText(m_pLastItem ? m_pLastItem->mydata()->szSavePath.ptr() : "");
	m_pCommandline->setText(m_pLastItem ? m_pLastItem->mydata()->szCommandline.ptr() : "");
	m_pMagicBytes->setText(m_pLastItem ? m_pLastItem->mydata()->szMagicBytes.ptr() : "");
	m_pRemoteExecCommandline->setText(m_pLastItem ? m_pLastItem->mydata()->szRemoteExecCommandline.ptr() : "");
	m_pIcon->setText(m_pLastItem ? m_pLastItem->mydata()->szIcon.ptr() : "");
}

void KviMediaTypesOptionsWidget::currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *)
{
	saveLastItem();
	m_pLastItem = (KviMediaTypeTreeWidgetItem *)it;
	if(it)
	{
		if(!it->isSelected())it->setSelected(true);
	}
	setLineEdits();
	enableOrDisable();
}

void KviMediaTypesOptionsWidget::newMediaType()
{
	KviMediaType empty;
	empty.szDescription = __tr2qs_ctx("New Media Type","options");
	KviMediaTypeTreeWidgetItem * it = new KviMediaTypeTreeWidgetItem(m_pTreeWidget,&empty);
	m_pTreeWidget->setCurrentItem(it);
	it->setSelected(true);
}

void KviMediaTypesOptionsWidget::delMediaType()
{
	if(m_pLastItem)
	{
		KviMediaTypeTreeWidgetItem * it = m_pLastItem;
		m_pLastItem = 0;
		delete it;
	}
}

void KviMediaTypesOptionsWidget::commit()
{
	saveLastItem();
	KviMediaTypeTreeWidgetItem * it;// = (KviMediaTypeTreeWidgetItem *)m_pTreeWidget->firstChild();
	g_pMediaManager->lock();
	g_pMediaManager->clear();
	//while(it)
	for(int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		it=(KviMediaTypeTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		KviMediaType * t = new KviMediaType;
		copyMediaType(t,it->mydata());
		g_pMediaManager->insertMediaType(t);
	//	it = (KviMediaTypeTreeWidgetItem *)it->nextSibling();
	}
	g_pMediaManager->unlock();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_mediatypes.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
