//=============================================================================
//
//   File : kvi_toolwindows_container.cpp
//   Creation date : Tue Aug 1 2000 21:05:22 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_toolwindows_container.h"
#include "kvi_iconmanager.h"
#include "kvi_tal_vbox.h"

#include <QPainter>
#include <QApplication>
#include <QLayout>
#include <QObjectCleanupHandler>
#include <QIcon>


KviWindowToolWidget::KviWindowToolWidget(QWidget * parent, KviWindowToolPageButton* button/*, const char * name, WFlags f*/ )
:QWidget(parent/*,name,f*/)
{
//	m_pContainer=parent;
	m_pButton=button;
	m_bAutoDelete=1;
	m_bHidden=0;
	if(m_pButton) m_pButton->setChecked(!isHidden());
//	m_ObjectHandler.add(button);
//	setMinimumWidth(0);
}

KviWindowToolWidget::~KviWindowToolWidget()
{
//	unregisterSelf();
//	m_ObjectHandler.remove(m_pButton);

}

void KviWindowToolWidget::registerSelf()
{
/*	m_pContainer->registerWidget(this);
	if(!m_ObjectHandler.isEmpty())
	{
		if(m_pButton)
			m_pButton->setOn(true);
	} else {
		m_pButton=0;
	}*/
}

void KviWindowToolWidget::unregisterSelf()
{
/*	if(!m_bHidden)
		m_pContainer->unregisterWidget(this);
	if(!m_ObjectHandler.isEmpty())
	{
		if(m_pButton)
			m_pButton->setOn(false);
	} else {
		m_pButton=0;
	}*/
}

/*void KviWindowToolWidget::hide()
{
	if(m_bAutoDelete==0)
	{
		unregisterSelf();
		m_bHidden=1;
	}
	QWidget::hide();
}*/

/*void KviWindowToolWidget::show()
{
	if(m_bHidden)
	{
		registerSelf();
	}
	QWidget::show();
	m_bHidden=0;
}*/

KviWindowToolPageButton::KviWindowToolPageButton ( int pixon,int pixoff, const QString & text, QWidget * parent,bool bOn, const char * name )
:QToolButton(parent)
{
//	setFlat(true);
	setToolTip(text);
	setObjectName("kvi_window_tool_button");
	setIcon(QIcon(*(g_pIconManager->getSmallIcon(pixon))));
	setCheckable(true);
	setChecked(bOn);

//	setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Ignored));
}

KviWindowToolPageButton::~KviWindowToolPageButton()
{
}

/*void KviWindowToolPageButton::drawButton ( QPainter * p)
{
	QPixmap pixmap( height(), width() );
	if(isOn())
		pixmap.fill(colorGroup().mid());
	else
		pixmap.fill(colorGroup().button()); 
	QPainter painter( &pixmap );

	QPixmap icon;
	if(isOn())
		icon=iconSet()->pixmap(QIcon::Normal,QIcon::On);
	else
		icon=iconSet()->pixmap(QIcon::Normal,QIcon::Off);
		
	// Draw the frame
	//painter.setPen( colorGroup().mid() );
	//if ( m_id != NUM_TABS - 1 ) painter.drawLine( 0, 0, 0, pixmap.height() - 1 );
	//painter.drawLine( 0, pixmap.height() - 1, pixmap.width() - 1, pixmap.height() - 1 );
	
	// Draw the text
	QFont font;
	QString str = text();
	str.remove("&");
	const int textX = pixmap.width() / 2 - QFontMetrics( font ).width( str ) / 2;
	painter.setPen( colorGroup().buttonText() );
	const QRect rect( textX + icon.width() / 2 + 2, 0, pixmap.width(), pixmap.height() );
	painter.drawText( rect, Qt::AlignLeft | Qt::AlignVCenter, str );
	
	// Draw the icon
	painter.drawPixmap( textX - icon.width() / 2 - 2, pixmap.height() / 2 - icon.height() / 2, icon );
	
	// Paint to widget
	p->rotate( -90 );
	p->drawPixmap( 1 - pixmap.width(), 0, pixmap );
}*/
