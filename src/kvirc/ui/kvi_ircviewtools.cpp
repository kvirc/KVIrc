//=============================================================================
//
//   File : kvi_ircviewtools.cpp
//   Creation date : Sat Oct 9 2004 16:03:01 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

#define __KVIRC__

#include "kvi_ircviewtools.h"
#include "kvi_ircview.h"
#include "kvi_styled_controls.h"
#include "kvi_iconmanager.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_malloc.h"
#include "kvi_msgbox.h"
#include "kvi_filedialog.h"
#include "kvi_app.h"
#include "kvi_memmove.h"

#include <qtoolbutton.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qlineedit.h>
#include <qcursor.h>
#include <qaccel.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Tool widget implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


KviIrcMessageCheckListItem::KviIrcMessageCheckListItem(QListView * par,KviIrcViewToolWidget * w,int id)
: QCheckListItem(par,QString::null,QCheckListItem::CheckBox)
{
	m_iId = id;
	m_pToolWidget = 0;
	setPixmap(0,*(g_pIconManager->getSmallIcon(KVI_OPTION_MSGTYPE(id).pixId())));
	setOn(true);
	m_pToolWidget = w;
}

KviIrcMessageCheckListItem::~KviIrcMessageCheckListItem()
{
}

void KviIrcMessageCheckListItem::stateChange(bool bOn)
{
	QCheckListItem::stateChange(bOn);
	if(!m_pToolWidget)return;
	m_pToolWidget->forceRepaint();
}



KviIrcViewToolWidget::KviIrcViewToolWidget(KviIrcView * par)
: QFrame(par)
{
	m_pIrcView = par;
	setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

	QGridLayout * gl = new QGridLayout(this,2,2,4,2);

	QLabel * l = new QLabel(__tr2qs("<b><font color=\"#EAEAEA\" size=\"-1\">Find Text</font></b>"),this);
	l->setMaximumHeight(14);
	l->setBackgroundColor(Qt::black);
	gl->addWidget(l,0,0);

	QToolButton *tb = new QToolButton(DownArrow,this);
	tb->setFixedSize(14,14);
	tb->setAutoRepeat(false);
	connect(tb,SIGNAL(clicked()),m_pIrcView,SLOT(toggleToolWidget()));
	gl->addWidget(tb,0,1);


	QTabWidget * tw = new QTabWidget(this);



	// Find tab
	QWidget * w = new QWidget(tw);

	QGridLayout * g = new QGridLayout(w,6,2,2,1);

	m_pStringToFind = new QLineEdit(w);
	g->addMultiCellWidget(m_pStringToFind,0,0,0,2);
	connect(m_pStringToFind,SIGNAL(returnPressed()),this,SLOT(findNext()));
	
	m_pRegExp = new KviStyledCheckBox(__tr2qs("&Regular expression"),w);
	g->addMultiCellWidget(m_pRegExp,1,1,0,2);

	m_pExtendedRegExp = new KviStyledCheckBox(__tr2qs("E&xtended regexp."),w);
	g->addMultiCellWidget(m_pExtendedRegExp,2,2,0,2);
	m_pExtendedRegExp->setEnabled(false);
	connect(m_pRegExp,SIGNAL(toggled(bool)),m_pExtendedRegExp,SLOT(setEnabled(bool)));

	m_pCaseSensitive = new KviStyledCheckBox(__tr2qs("C&ase sensitive"),w);
	g->addMultiCellWidget(m_pCaseSensitive,3,3,0,2);

	QPushButton * pb = new QPushButton(__tr2qs("Find &Prev."),w);
	connect(pb,SIGNAL(clicked()),this,SLOT(findPrev()));
	g->addWidget(pb,4,0);

	pb = new QPushButton(__tr2qs("&Find Next"),w);
	pb->setDefault(true);
	connect(pb,SIGNAL(clicked()),this,SLOT(findNext()));
	g->addMultiCellWidget(pb,4,4,1,2);

	m_pFindResult = new QLabel(w);
	m_pFindResult->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	g->addMultiCellWidget(m_pFindResult,5,5,0,2);

	//g->setResizeMode(QGridLayout::Fixed);

	tw->addTab(w,__tr2qs("Find"));

	// Filter tab
	QWidget * w1 = new QWidget(tw);

	g = new QGridLayout(w1,5,2,2,1);

	m_pFilterView = new QListView(w1);
	m_pFilterView->addColumn(__tr2qs("Type"));
	m_pFilterView->header()->hide();
	m_pFilterView->setMinimumSize(QSize(10,10));

	g->addMultiCellWidget(m_pFilterView,0,4,0,0);


	m_pFilterItems = (KviIrcMessageCheckListItem **)kvi_malloc(KVI_NUM_MSGTYPE_OPTIONS * sizeof(KviIrcMessageCheckListItem *));

	for(int i=0;i<KVI_NUM_MSGTYPE_OPTIONS;i++)
	{
		m_pFilterItems[i] = new KviIrcMessageCheckListItem(m_pFilterView,this,i);
	}

	pb = new QPushButton(__tr2qs("Set &All"),w1);
	connect(pb,SIGNAL(clicked()),this,SLOT(filterEnableAll()));
	g->addWidget(pb,0,1);

	pb = new QPushButton(__tr2qs("Set &None"),w1);
	connect(pb,SIGNAL(clicked()),this,SLOT(filterEnableNone()));
	g->addWidget(pb,1,1);

	pb = new QPushButton(__tr2qs("&Load From..."),w1);
	connect(pb,SIGNAL(clicked()),this,SLOT(filterLoad()));
	g->addWidget(pb,2,1);

	pb = new QPushButton(__tr2qs("&Save As..."),w1);
	connect(pb,SIGNAL(clicked()),this,SLOT(filterSave()));
	g->addWidget(pb,3,1);

	tw->addTab(w1,__tr2qs("Filter"));

	gl->addMultiCellWidget(tw,1,1,0,1);

	gl->setResizeMode(QGridLayout::Fixed);
	m_pStringToFind->setFocus();
	tw->showPage(w);
	QAccel *a = new QAccel( this );
        a->connectItem( a->insertItem(Key_Escape), this,SLOT(close()) );
}

KviIrcViewToolWidget::~KviIrcViewToolWidget()
{
	kvi_free((void *)m_pFilterItems);
}

void KviIrcViewToolWidget::filterEnableAll()
{
	for(int i=0;i<KVI_NUM_MSGTYPE_OPTIONS;i++)
	{
		m_pFilterItems[i]->setToolWidget(0);
		m_pFilterItems[i]->setOn(true);
		m_pFilterItems[i]->setToolWidget(this);
	}
	forceRepaint();
}

void KviIrcViewToolWidget::filterEnableNone()
{
	for(int i=0;i<KVI_NUM_MSGTYPE_OPTIONS;i++)
	{
		m_pFilterItems[i]->setToolWidget(0);
		m_pFilterItems[i]->setOn(false);
		m_pFilterItems[i]->setToolWidget(this);
	}
	forceRepaint();
}

void KviIrcViewToolWidget::filterLoad()
{
	QString szFile;
	QString szInit;
	g_pApp->getLocalKvircDirectory(szInit,KviApp::Filters);

	if(KviFileDialog::askForOpenFileName(szFile,__tr2qs("Select a Filter File"),szInit))
	{
		QFile f(szFile);
		if(f.open(IO_ReadOnly))
		{
			char buffer[KVI_NUM_MSGTYPE_OPTIONS];
			kvi_memset(buffer,0,KVI_NUM_MSGTYPE_OPTIONS);
			f.readBlock(buffer,KVI_NUM_MSGTYPE_OPTIONS);
			f.close();
			for(int i=0;i<KVI_NUM_MSGTYPE_OPTIONS;i++)
			{
				m_pFilterItems[i]->setToolWidget(0);
				m_pFilterItems[i]->setOn(buffer[i]);
				m_pFilterItems[i]->setToolWidget(this);
			}
			forceRepaint();
		} else {
			KviMessageBox::warning(__tr2qs("Can't open the filter file %s for reading."),&szFile);
		}
	}
}

void KviIrcViewToolWidget::filterSave()
{
	QString szFile;
	QString szInit;
	g_pApp->getLocalKvircDirectory(szInit,KviApp::Filters,"filter.kvf");
	if(KviFileDialog::askForSaveFileName(szFile,__tr2qs("Select a Name for the Filter File"),szInit,0,false,true))
	{
		QFile f(szFile);
		if(f.open(IO_WriteOnly))
		{
			char buffer[KVI_NUM_MSGTYPE_OPTIONS];
			for(int i=0;i<KVI_NUM_MSGTYPE_OPTIONS;i++)
			{
				buffer[i] = messageEnabled(i) ? 1 : 0;
			}
			if(f.writeBlock(buffer,KVI_NUM_MSGTYPE_OPTIONS) < KVI_NUM_MSGTYPE_OPTIONS)
				KviMessageBox::warning(__tr2qs("Failed to write the filter file %Q (IO Error)"),&szFile);
			f.close();
		} else KviMessageBox::warning(__tr2qs("Can't open the filter file %Q for writing"),&szFile);
	}
}

void KviIrcViewToolWidget::forceRepaint()
{
	m_pIrcView->paintEvent(0);
}

void KviIrcViewToolWidget::setFindResult(const QString & text)
{
	m_pFindResult->setText(text);
}

void KviIrcViewToolWidget::findPrev()
{
	bool bRegExp = m_pRegExp->isChecked();
	m_pIrcView->findPrev(m_pStringToFind->text(),m_pCaseSensitive->isChecked(),bRegExp,bRegExp && m_pExtendedRegExp->isChecked());
}

void KviIrcViewToolWidget::findNext()
{
	bool bRegExp = m_pRegExp->isChecked();
	m_pIrcView->findNext(m_pStringToFind->text(),m_pCaseSensitive->isChecked(),bRegExp,bRegExp && m_pExtendedRegExp->isChecked());
}


void KviIrcViewToolWidget::mousePressEvent(QMouseEvent *e)
{
	m_pressPoint = e->pos();
}

void KviIrcViewToolWidget::mouseMoveEvent(QMouseEvent *)
{
	QPoint p=m_pIrcView->mapFromGlobal(QCursor::pos());
	p-=m_pressPoint;
	if(p.x() < 1)p.setX(1);
	else {
		int www = (m_pIrcView->width() - (m_pIrcView->m_pScrollBar->width() + 1));
		if((p.x() + width()) > www)p.setX(www - width());
	}
	if(p.y() < 1)p.setY(1);
	else {
		int hhh = (m_pIrcView->height() - 1);
		if((p.y() + height()) > hhh)p.setY(hhh - height());
	}
	move(p);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Link tip label implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviIrcViewToolTip::KviIrcViewToolTip(KviIrcView * pView)
: QToolTip(pView,0)
{
	m_pView = pView;
}

KviIrcViewToolTip::~KviIrcViewToolTip()
{
}

void KviIrcViewToolTip::maybeTip(const QPoint &pnt)
{
	m_pView->maybeTip(pnt);
}

