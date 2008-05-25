///////////////////////////////////////////////////////////////////////////////
//
//   File : kvi_listview.cpp
//   Creation date : 19  Jan 2006 GMT by Alexey Uzhva
//
//   This toolbar is part of the KVirc irc client distribution
//   Copyright (C) 2006 Alexey Uzhva
//   Copyright (C) 2006 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVIRC__

#include "kvi_listview.h"

#include <qpainter.h>
#include <qpixmap.h>

#include "kvi_doublebuffer.h"

KviListView::KviListView( QWidget * parent, const char * name)
:KviTalListView(parent)
{
	m_pBackgroundOverlayPixmap=0;
	m_iBackgroundOverlayAlignment=Qt::AlignAuto;
}

KviListView::~KviListView()
{
	if(m_pBackgroundOverlayPixmap)
		delete m_pBackgroundOverlayPixmap;
}

void KviListView::setBackgroundOverlayPixmap(QPixmap * pix,int iAlignmentFlags)
{
	setStaticBackground(TRUE);
	viewport()->setAutoFillBackground(false);
	m_pBackgroundOverlayPixmap=new QPixmap(*pix);
	m_iBackgroundOverlayAlignment= Qt::AlignRight | Qt::AlignBottom;
	repaintContents();
}



void KviListView::drawContentsOffset(QPainter * p,int ox,int oy,int cx,int cy,int cw,int ch)
{
	if(!m_pBackgroundOverlayPixmap)
	{
		KviTalListView::drawContentsOffset(p,ox,oy,cx,cy,cw,ch);
		return;
	}

	if(cw <= 0)return; // this does happen
	if(ch <= 0)return; // this does happen

	KviDoubleBuffer pix(viewport()->width(),viewport()->height());

	QPixmap * pMemPixmap = pix.pixmap();

	QPainter pa(pMemPixmap);

	int xx = cx - ox;
	int yy = cy - oy;

	pa.fillRect(QRect(xx,yy,cw,ch),viewport()->backgroundColor());

	//KviTalListView::paintEmptyArea(&pa,);

	// compute the pixmap position
	int x,y;
	if(m_iBackgroundOverlayAlignment == Qt::AlignAuto)
		x=y=0;
	else {
		if(m_iBackgroundOverlayAlignment & Qt::AlignLeft)
			x=0;
		else if ( m_iBackgroundOverlayAlignment & Qt::AlignRight )
			x=viewport()->width() - m_pBackgroundOverlayPixmap->width();
		else if( m_iBackgroundOverlayAlignment & Qt::AlignHCenter )
			x=(viewport()->width() - m_pBackgroundOverlayPixmap->width())/2;
		else
			x=0;
		
		if( m_iBackgroundOverlayAlignment & Qt::AlignTop )
			y=0;
		else if ( m_iBackgroundOverlayAlignment & Qt::AlignBottom )
			y=viewport()->height() - m_pBackgroundOverlayPixmap->height();
		else if ( m_iBackgroundOverlayAlignment & Qt::AlignVCenter )
			y=(viewport()->height() - m_pBackgroundOverlayPixmap->height())/2;
		else
			y=0;
	}

	pa.drawPixmap(xx,yy,*m_pBackgroundOverlayPixmap,xx-x,yy-y,cw,ch);

	// Qt's auto double buffering is buggy and can't be disabled... too bad :/
	//
	// The Qt source for the listview painting is also totally unreadable
	// (maybe that's why they have thrown this cool widget away in Qt4: unmantainable)
	//
	// Anyway, I've noticed that when double buffering is choosen (and
	// Qt seems to have a really complex logic to choose when to enable it
	// and when not) then the painter passed to paintCell() of the
	// list view items is NOT this painter. It's the internal painter
	// of the QSharedDoubleBuffer private Qt class. It's screwed
	// because of the multiple coordinate translations. With this
	// widget we screw it even more just because our paintEmptyArea()
	// does nothing and we do double buffering ourselves.
	
	KviTalListView::drawContentsOffset(&pa,ox,oy,cx,cy,cw,ch);

	p->drawPixmap(xx,yy,*pMemPixmap,xx,yy,cw,ch);

	//p->drawPixmap(cx-ox,cy-oy,*pMemPixmap,0,0,cw,ch);
	//KviTalListView::drawContentsOffset(p,ox,oy,cx,cy,cw,ch);
}


void KviListView::paintEmptyArea(QPainter * p,const QRect & rect)
{
	if(!m_pBackgroundOverlayPixmap)
	{
		KviTalListView::paintEmptyArea(p,rect);
		return;
	}

	// otherwise just do nothing (we're filling the background in drawContentsOffset)

	/*
	KviDoubleBuffer pix(rect.right()+1,rect.bottom()+1);

	QPixmap * pMemPixmap = pix.pixmap();

	QPainter pa(pMemPixmap);

	pa.fillRect(rect,viewport()->backgroundColor());

	KviTalListView::paintEmptyArea(&pa,rect);
	
	QPoint realTopLeft = p->xForm(rect.topLeft());

	// compute the pixmap position
	int x,y;
	if(m_iBackgroundOverlayAlignment == Qt::AlignAuto)
		x=y=0;
	else {
		if(m_iBackgroundOverlayAlignment & Qt::AlignLeft)
			x=0;
		else if ( m_iBackgroundOverlayAlignment & Qt::AlignRight )
			x=viewport()->width() - m_pBackgroundOverlayPixmap->width();
		else if( m_iBackgroundOverlayAlignment & Qt::AlignHCenter )
			x=(viewport()->width() - m_pBackgroundOverlayPixmap->width())/2;
		else
			x=0;
		
		if( m_iBackgroundOverlayAlignment & Qt::AlignTop )
			y=0;
		else if ( m_iBackgroundOverlayAlignment & Qt::AlignBottom )
			y=viewport()->height() - m_pBackgroundOverlayPixmap->height();
		else if ( m_iBackgroundOverlayAlignment & Qt::AlignVCenter )
			y=(viewport()->height() - m_pBackgroundOverlayPixmap->height())/2;
		else
			y=0;
	}

	pa.drawPixmap(rect.x(),rect.y(),*m_pBackgroundOverlayPixmap,realTopLeft.x()-x,realTopLeft.y()-y,rect.width(),rect.height());

	p->drawPixmap(rect.x(),rect.y(),*pMemPixmap,rect.x(),rect.y(),rect.width(),rect.height());
	*/
}

void KviListView::resizeEvent(QResizeEvent * e)
{
	KviTalListView::resizeEvent(e);
	if(m_pBackgroundOverlayPixmap)
	repaintContents(); // force a full repaint (otherwise qt does not honor static background here)
}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

void KviListView::focusInEvent(QFocusEvent * e)
{
	KviTalListView::focusInEvent(e);
	if(m_pBackgroundOverlayPixmap)
		repaintContents(); 
}

void KviListView::focusOutEvent(QFocusEvent * e)
{
	KviTalListView::focusOutEvent(e);
	if(m_pBackgroundOverlayPixmap)
		repaintContents(); 
}

#endif
