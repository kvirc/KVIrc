///////////////////////////////////////////////////////////////////////////////
//
//   File : kvi_xmladdonloader.cpp
//   Creation date : 25  Jul 2006 GMT by Alexey Uzhva
//
//   This toolbar is part of the KVirc irc client distribution
//   Copyright (C) 2006 Alexey Uzhva
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
///////////////////////////////////////////////////////////////////////////////

/*
#define __KVIRC__

#include "kvi_locale.h"
#include "kvi_xmladdonloader.h"
#include "kvi_iconmanager.h"
#include "kvi_app.h"

#include <qlayout.h>
#include <qtextcodec.h>
#include <qpushbutton.h>
#include <qfile.h>
#include <zlib.h>

// FIXME: Shouldn't this go to the addon management module ? (smaller executable)

KviXmlAddonLoaderItem::KviXmlAddonLoaderItem ( QDomElement *pElement,QCheckListItem * parent )
:QCheckListItem(parent,QString::null,CheckBoxController),m_pElement(pElement)
{
	setText(0,m_pElement->attribute("Name"));
	setText(1,m_pElement->attribute("Version"));
	setState(On);
}

KviXmlAddonLoaderItem::~KviXmlAddonLoaderItem()
{
	delete m_pElement;
}

KviXmlAddonLoader::KviXmlAddonLoader ( QString szAddonFilename,QWidget * parent, const char * name, bool modal, WFlags f )
:QDialog(parent,name,modal,f)
{
	m_pDocument=0;
	m_szFilename=szAddonFilename;
	QGridLayout *pLayout = new QGridLayout(this,2,3);
	
	m_pListView = new QListView(this);
	m_pListView->addColumn(__tr2qs("Name"));
	m_pListView->addColumn(__tr2qs("Version"));
	pLayout->addMultiCellWidget(m_pListView,0,0,0,2);
	
	QPushButton *pButton;
	
	pButton = new QPushButton(__tr2qs("Install"),this);
	connect(pButton,SIGNAL(clicked()),this,SLOT(accept()));
	pButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	pLayout->addWidget(pButton,1,1);
	
	pButton = new QPushButton(__tr2qs("Cancel"),this);
	connect(pButton,SIGNAL(clicked()),this,SLOT(reject()));
	pLayout->addWidget(pButton,1,2);
	pButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
	
	pLayout->setRowStretch(0,1);
	pLayout->setColStretch(0,1);
	
	fillListView();
}

KviXmlAddonLoader::~KviXmlAddonLoader()
{
	if(m_pDocument) delete m_pDocument;
}

void KviXmlAddonLoader::accept()
{
	QListViewItemIterator it( m_pListView, QListViewItemIterator::Checked );
	while ( it.current() ) {
		g_pApp->loadXmlTheme( ( (KviXmlAddonLoaderItem*)(it.current()) )->element() );
		++it;
	}
	QDialog::accept();
}

void KviXmlAddonLoader::reject()
{
	QDialog::reject();
}

void KviXmlAddonLoader::fillListView()
{
	m_pListView->clear();
	
	QAsciiDict<QCheckListItem> dict;
	
	if(m_pDocument) delete m_pDocument;
	m_pDocument = new QDomDocument();

	QCString data;
	gzFile file=gzopen(QTextCodec::codecForLocale()->fromUnicode(m_szFilename).data(),"rb");
	if(file)
	{
		char buff[1025];
		int len;
		len=gzread(file,buff,1024);
		while(len>0)
		{
			buff[len]=0;
			data.append(buff);
			len=gzread(file,buff,1024);
		}
		gzclose(file);
	}

	if ( !m_pDocument->setContent( data ) ) {
		return;
	}
	
	QCheckListItem *pParent;
	for( QDomNode n = m_pDocument->documentElement().firstChild(); !n.isNull(); n = n.nextSibling() )
	{
		if(n.nodeName()=="Theme" && n.isElement())
		{
			pParent=dict["theme"];
			if(!pParent) {
				pParent = new QCheckListItem(m_pListView,__tr2qs("Themes"));
				pParent->setOpen(TRUE);
				dict.replace("theme",pParent);
			}
			new KviXmlAddonLoaderItem(new QDomElement(n.toElement()),pParent);
		}
	}
}

*/
