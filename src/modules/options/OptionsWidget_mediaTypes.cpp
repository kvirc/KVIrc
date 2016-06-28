//=============================================================================
//
//   File : OptionsWidget_mediaTypes.cpp
//   Creation date : Sat Jan 20 2001 16:14:53 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidget_mediaTypes.h"

#include "KviOptions.h"
#include "KviLocale.h"
#include "KviTalToolTip.h"

#include <QLayout>
#include <QLabel>

// KviApplication.cpp
extern KVIRC_API KviMediaManager * g_pMediaManager;

static void copyMediaType(KviMediaType * dst, const KviMediaType * src)
{
	dst->szFileMask = src->szFileMask;
	dst->szIanaType = src->szIanaType;
	dst->szDescription = src->szDescription;
	dst->szMagicBytes = src->szMagicBytes;
	dst->szSavePath = src->szSavePath;
	dst->szCommandline = src->szCommandline;
	dst->szRemoteExecCommandline = src->szRemoteExecCommandline;
	dst->szIcon = src->szIcon;
}

MediaTypeTreeWidgetItem::MediaTypeTreeWidgetItem(QTreeWidget * w, KviMediaType * t)
    : QTreeWidgetItem(w)
{
	copyData(t);
}

MediaTypeTreeWidgetItem::~MediaTypeTreeWidgetItem()
    = default;

void MediaTypeTreeWidgetItem::copyData(KviMediaType * t)
{
	copyMediaType(&m_data, t);
	setText(0, m_data.szFileMask.ptr());
	setText(1, m_data.szIanaType.ptr());
	setText(2, m_data.szDescription.ptr());
}

OptionsWidget_mediaTypes::OptionsWidget_mediaTypes(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("mediatypes_options_widget");

	createLayout();

	m_pTreeWidget = new QTreeWidget(this);
	m_pTreeWidget->setColumnCount(3);
	QStringList columnLabels;
	columnLabels.append(__tr2qs_ctx("Pattern", "options"));
	columnLabels.append(__tr2qs_ctx("MIME Type", "options"));
	columnLabels.append(__tr2qs_ctx("Description", "options"));
	m_pTreeWidget->setHeaderLabels(columnLabels);
	m_pTreeWidget->setColumnWidth(0, 150);
	m_pTreeWidget->setColumnWidth(1, 180);
	m_pTreeWidget->setColumnWidth(2, 150);
	m_pTreeWidget->setAllColumnsShowFocus(true);
	m_pTreeWidget->setRootIsDecorated(false);
	connect(m_pTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

	layout()->addWidget(m_pTreeWidget, 0, 0, 1, 3);

	QLabel * l = new QLabel(__tr2qs_ctx("Description:", "options"), this);
	layout()->addWidget(l, 1, 0);
	m_pDescription = new QLineEdit(this);
	layout()->addWidget(m_pDescription, 1, 1, 1, 2);

	l = new QLabel(__tr2qs_ctx("MIME type:", "options"), this);
	layout()->addWidget(l, 2, 0);
	m_pIanaType = new QLineEdit(this);
	layout()->addWidget(m_pIanaType, 2, 1, 1, 2);

	l = new QLabel(__tr2qs_ctx("File pattern:", "options"), this);
	layout()->addWidget(l, 3, 0);
	m_pFileMask = new QLineEdit(this);
	layout()->addWidget(m_pFileMask, 3, 1, 1, 2);

	l = new QLabel(__tr2qs_ctx("Magic bytes:", "options"), this);
	layout()->addWidget(l, 4, 0);
	m_pMagicBytes = new QLineEdit(this);
	layout()->addWidget(m_pMagicBytes, 4, 1, 1, 2);

	l = new QLabel(__tr2qs_ctx("Save path:", "options"), this);
	layout()->addWidget(l, 5, 0);
	m_pSavePath = new QLineEdit(this);
	layout()->addWidget(m_pSavePath, 5, 1, 1, 2);

	l = new QLabel(__tr2qs_ctx("Local open command:", "options"), this);
	layout()->addWidget(l, 6, 0);
	m_pCommandline = new QLineEdit(this);
	layout()->addWidget(m_pCommandline, 6, 1, 1, 2);
	mergeTip(m_pCommandline, __tr2qs_ctx("This field contains the command to execute to open a local file.<br>"
	                                     "<tt>$0</tt> is used in place of the filename",
	                             "options"));

	l = new QLabel(__tr2qs_ctx("Remote open command:", "options"), this);
	layout()->addWidget(l, 7, 0);
	m_pRemoteExecCommandline = new QLineEdit(this);
	layout()->addWidget(m_pRemoteExecCommandline, 7, 1, 1, 2);
	mergeTip(m_pRemoteExecCommandline, __tr2qs_ctx("This field contains the command to execute when automatically opening a received file.<br>"
	                                               "<tt>$0</tt> is used in place of the filename",
	                                       "options"));

	l = new QLabel(__tr2qs_ctx("Icon:", "options"), this);
	layout()->addWidget(l, 8, 0);
	m_pIcon = new QLineEdit(this);
	layout()->addWidget(m_pIcon, 8, 1, 1, 2);

	QFrame * f = new QFrame(this);
	f->setFrameStyle(QFrame::Sunken | QFrame::HLine);
	layout()->addWidget(f, 9, 0, 1, 3);

	QPushButton * b = new QPushButton(__tr2qs_ctx("&New", "options"), this);
	connect(b, SIGNAL(clicked()), this, SLOT(newMediaType()));
	layout()->addWidget(b, 10, 1);

	m_pDelete = new QPushButton(__tr2qs_ctx("Re&move", "options"), this);
	connect(m_pDelete, SIGNAL(clicked()), this, SLOT(delMediaType()));
	layout()->addWidget(m_pDelete, 10, 2);

	layout()->setColumnStretch(1, 1);
	layout()->setColumnStretch(2, 1);
	layout()->setRowStretch(0, 1);

	m_pLastItem = nullptr;

	fillTreeWidget();
}

OptionsWidget_mediaTypes::~OptionsWidget_mediaTypes()
    = default;

void OptionsWidget_mediaTypes::fillTreeWidget()
{
	m_pTreeWidget->clear();
	g_pMediaManager->lock();
	KviPointerList<KviMediaType> * l = g_pMediaManager->mediaTypeList();

	for(KviMediaType * t = l->first(); t; t = l->next())
		new MediaTypeTreeWidgetItem(m_pTreeWidget, t);

	g_pMediaManager->unlock();
	enableOrDisable();
}

void OptionsWidget_mediaTypes::saveLastItem()
{
	if(m_pLastItem)
	{
		KviMediaType t;
		t.szDescription = m_pDescription->text();
		t.szIanaType = m_pIanaType->text();
		t.szFileMask = m_pFileMask->text();
		t.szSavePath = m_pSavePath->text();
		t.szCommandline = m_pCommandline->text();
		t.szMagicBytes = m_pMagicBytes->text();
		t.szRemoteExecCommandline = m_pRemoteExecCommandline->text();
		t.szIcon = m_pIcon->text();

		if(t.szDescription.isEmpty())
			t.szDescription = __tr2qs_ctx("[Unknown Media Type]", "options");

		m_pLastItem->copyData(&t);
	}
}

void OptionsWidget_mediaTypes::enableOrDisable()
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

void OptionsWidget_mediaTypes::setLineEdits()
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

void OptionsWidget_mediaTypes::currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem *)
{
	saveLastItem();
	m_pLastItem = (MediaTypeTreeWidgetItem *)it;
	if(it)
	{
		if(!it->isSelected())
			it->setSelected(true);
	}
	setLineEdits();
	enableOrDisable();
}

void OptionsWidget_mediaTypes::newMediaType()
{
	KviMediaType empty;
	empty.szDescription = __tr2qs_ctx("New media type", "options");
	MediaTypeTreeWidgetItem * it = new MediaTypeTreeWidgetItem(m_pTreeWidget, &empty);
	m_pTreeWidget->setCurrentItem(it);
	it->setSelected(true);
}

void OptionsWidget_mediaTypes::delMediaType()
{
	if(m_pLastItem)
	{
		MediaTypeTreeWidgetItem * it = m_pLastItem;
		m_pLastItem = nullptr;
		delete it;
	}
}

void OptionsWidget_mediaTypes::commit()
{
	saveLastItem();
	MediaTypeTreeWidgetItem * it; // = (MediaTypeTreeWidgetItem *)m_pTreeWidget->firstChild();
	g_pMediaManager->lock();
	g_pMediaManager->clear();
	//while(it)
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		it = (MediaTypeTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		KviMediaType * t = new KviMediaType;
		copyMediaType(t, it->mydata());
		g_pMediaManager->insertMediaType(t);
		//	it = (MediaTypeTreeWidgetItem *)it->nextSibling();
	}
	g_pMediaManager->unlock();
}
