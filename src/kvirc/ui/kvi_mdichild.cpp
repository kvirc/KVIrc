//=============================================================================
//
//   File : kvi_mdichild.cpp
//   Creation date : Wed Jun 21 2000 17:35:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2007 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================
#define __KVIRC__

#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"
#include "kvi_mdichild.h"
#include "kvi_mdimanager.h"
#include "kvi_string.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_settings.h"
#include "kvi_iconmanager.h"
#include "kvi_window.h"
#include "kvi_mdicaption.h"

#include <qcursor.h>
#include <qnamespace.h>
#include <qapplication.h>
#include <qfontmetrics.h>
#include "kvi_pointerlist.h"
#include <qpixmap.h>
#include <qstyle.h>
#include <qpainter.h>
#include "kvi_tal_popupmenu.h"
#ifndef COMPILE_USE_QT4
	#include <qobjectlist.h>
#endif
#include <qevent.h>

#ifdef Q_OS_MACX
#include "kvi_app.h"  //Needed for g_pApp
#ifdef COMPILE_USE_QT4
	#include <QDesktopWidget>
#endif
#endif

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif


#define KVI_MDI_NORESIZE 0
#define KVI_MDI_RESIZE_TOP 1
#define KVI_MDI_RESIZE_LEFT 2
#define KVI_MDI_RESIZE_RIGHT 4
#define KVI_MDI_RESIZE_BOTTOM 8
#define KVI_MDI_RESIZE_TOPLEFT (1|2)
#define KVI_MDI_RESIZE_TOPRIGHT (1|4)
#define KVI_MDI_RESIZE_BOTTOMLEFT (8|2)
#define KVI_MDI_RESIZE_BOTTOMRIGHT (8|4)


KviMdiChild::KviMdiChild(KviMdiManager * par,const char * name)
: QFrame(par->viewport(),name ? name : "mdi_child")
{
	setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	setFrameShape(NoFrame);
	m_pManager = par;

	m_pCaption      = new KviMdiCaption(this,"mdi_caption");

	m_bResizeMode        = false;
	m_iResizeCorner      = KVI_MDI_NORESIZE;
	m_iLastCursorCorner  = KVI_MDI_NORESIZE;

	m_pClient            = 0;
	m_state              = Normal;
	m_restoredGeometry   = QRect(10,10,100,100);

	setMouseTracking(true);
	setMinimumSize(KVI_MDICHILD_MIN_WIDTH,KVI_MDICHILD_MIN_HEIGHT);

#ifdef COMPILE_USE_QT4
	setAutoFillBackground(true);
#endif
	
}

KviMdiChild::~KviMdiChild()
{
	if(m_pClient)delete m_pClient;
}

void KviMdiChild::reloadImages()
{
	m_pCaption->reloadImages();
}

QRect KviMdiChild::restoredGeometry()
{
	if(m_state == Maximized)return m_restoredGeometry;
	else return QRect(x(),y(),width(),height());
//	else return geometry();

}

#ifdef COMPILE_USE_QT4
void KviMdiChild::setBackgroundRole(QPalette::ColorRole)
{
	// hack
	QFrame::setBackgroundRole(QPalette::Window);
}
#else
void KviMdiChild::setBackgroundMode(QWidget::BackgroundMode)
{
	// hack
	QFrame::setBackgroundMode(QWidget::PaletteBackground);
}
#endif

void KviMdiChild::setIcon(const QPixmap &pix)
{
	m_pCaption->setSystemIcon(pix);

	if((m_state == Maximized) && (m_pManager->topChild() == this))
	{
		m_pManager->updateSDIMode();
	}
}

const QPixmap * KviMdiChild::icon()
{
	return m_pCaption->systemIcon();
}

void KviMdiChild::enableClose(bool bEnable)
{
	m_pCaption->enableClose(bEnable);
	if((m_state == Maximized) && (m_pManager->topChild() == this))
	{
		m_pManager->updateSDIMode();
	}

}

bool KviMdiChild::closeEnabled()
{
	return m_pCaption->closeEnabled();
}

void KviMdiChild::setCaption(const QString & plain,const QString & xmlActive,const QString & xmlInactive)
{
	m_szPlainCaption = plain;
	m_szXmlActiveCaption = xmlActive;
	m_szXmlInactiveCaption = xmlInactive;
	//m_pCaptionLabel->setActive(m_pCaptionLabel->active());
	m_pCaption->update();
}

void KviMdiChild::maximize()
{
	if(m_state == Minimized)restore(); // restore first
	if(m_state == Normal)m_restoredGeometry = geometry();
	m_state = Maximized;
	manager()->maximizeChild(this);
}

void KviMdiChild::restore()
{
	if(m_restoredGeometry.x() < 0)m_restoredGeometry.setX(0);
	if(m_restoredGeometry.y() < 0)m_restoredGeometry.setY(0);
	// ensure coherency
	if(m_restoredGeometry.width() < 5)m_restoredGeometry.setWidth(5);
	if(m_restoredGeometry.height() < 5)m_restoredGeometry.setHeight(5);

	/*
	if((m_restoredGeometry.x() + m_restoredGeometry.width()) > m_pManager->width())
		m_restoredGeometry.setWidth(m_pManager->width() - m_restoredGeometry.x());
	if((m_restoredGeometry.y() + m_restoredGeometry.height()) > m_pManager->height())
		m_restoredGeometry.setHeight(m_pManager->height() - m_restoredGeometry.y());
	*/
	
	switch(m_state)
	{
		case Maximized:
			m_pManager->moveChild(this,m_restoredGeometry.x(),m_restoredGeometry.y());
			resize(m_restoredGeometry.width(),m_restoredGeometry.height());
			m_state = Normal;
			m_pManager->childRestored(this,true);
		break;
		case Minimized:
			m_pManager->moveChild(this,m_restoredGeometry.x(),m_restoredGeometry.y());
			resize(m_restoredGeometry.width(),m_restoredGeometry.height());
			show();
			m_state = Normal;
			m_pManager->childRestored(this,false);
		break;
		case Normal:
			m_state = Normal;
			if(!isVisible())show();
			return;
		break;
	}
}

void KviMdiChild::minimize()
{
	switch(m_state)
	{
		case Maximized:
			hide();
			m_state = Minimized;
			m_pManager->childMinimized(this,true);
		break;
		case Normal:
			m_restoredGeometry = geometry();
			hide();
			m_state = Minimized;
			m_pManager->childMinimized(this,false);
		break;
		case Minimized:
			m_state = Minimized;
			if(isVisible())hide();
			return;
		break;
	}
}

void KviMdiChild::closeRequest()
{
	if(closeEnabled())if(m_pClient)m_pClient->close();
}
/*
void KviMdiChild::systemPopupAboutToShow()
{
	m_pSystemPopup->clear();
	if(m_state != Maximized)m_pSystemPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MAXIMIZE)),__tr("&Maximize"),this,SLOT(maximize()));
	if(m_state != Minimized)m_pSystemPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MINIMIZE)),__tr("M&inimize"),this,SLOT(minimize()));
	if(m_state != Normal)m_pSystemPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_RESTORE)),__tr("&Restore"),this,SLOT(restore()));
	if(closeEnabled())
	{
		m_pSystemPopup->insertSeparator();
		m_pSystemPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLOSE)),__tr("&Close"),this,SLOT(closeRequest()));
	}
}
*/
void KviMdiChild::moveEvent(QMoveEvent *e)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(m_pClient && g_pShadedChildGlobalDesktopBackground)
	{
		if(m_pClient->inherits("KviWindow")) // actually this is always the case
		{
			((KviWindow *)m_pClient)->updateBackgrounds();
		}
	}
#endif
	QFrame::moveEvent(e);
}

void KviMdiChild::systemPopupSlot()
{
	if(sender()->inherits("QToolButton"))
	{
		emit systemPopupRequest(((QToolButton *)sender())->mapToGlobal(QPoint(0,((QToolButton *)sender())->height())));
	} else {
		emit systemPopupRequest(m_pCaption->mapToGlobal(QPoint(5,5)));
	}
}

void KviMdiChild::resizeEvent(QResizeEvent *e)
{
	int s = m_pCaption->heightHint();
	m_pCaption->setGeometry(KVI_MDICHILD_BORDER,KVI_MDICHILD_BORDER,
		width() - (KVI_MDICHILD_BORDER << 1),s);

	if(m_pClient)
	{
		int yPos = KVI_MDICHILD_BORDER + s + KVI_MDICHILD_SPACING;
		m_pClient->setGeometry(KVI_MDICHILD_BORDER,yPos,
			width() - (KVI_MDICHILD_BORDER << 1),height() - (yPos + KVI_MDICHILD_BORDER));
	}
	QFrame::resizeEvent(e);
}

void KviMdiChild::mousePressEvent(QMouseEvent *e)
{
	if(m_state == Maximized)return;
	m_iResizeCorner=getResizeCorner(e->pos().x(),e->pos().y());
	if(m_iResizeCorner != KVI_MDI_NORESIZE)
	{
		grabMouse(getResizeCursor(m_iResizeCorner));
		m_bResizeMode = true;
	}
}

void KviMdiChild::mouseReleaseEvent(QMouseEvent *)
{
	m_iResizeCorner=KVI_MDI_NORESIZE;
	m_iLastCursorCorner=KVI_MDI_NORESIZE;

	if(m_bResizeMode)
	{
		m_bResizeMode = false;
		releaseMouse();
	}

	//if(QApplication::overrideCursor())QApplication::restoreOverrideCursor();
}

QCursor KviMdiChild::getResizeCursor(int resizeCorner)
{
	switch (resizeCorner)
	{ 
		case KVI_MDI_RESIZE_LEFT: 
		case KVI_MDI_RESIZE_RIGHT: 
			return Qt::sizeHorCursor;
			break; 
		case KVI_MDI_RESIZE_TOP: 
		case KVI_MDI_RESIZE_BOTTOM: 
			return Qt::sizeVerCursor;
			break; 
		case KVI_MDI_RESIZE_TOPLEFT: 
		case KVI_MDI_RESIZE_BOTTOMRIGHT: 
			return Qt::sizeFDiagCursor;
			break; 
		case KVI_MDI_RESIZE_BOTTOMLEFT: 
		case KVI_MDI_RESIZE_TOPRIGHT: 
			return Qt::sizeBDiagCursor;
			break; 
		default:
			return Qt::arrowCursor;
			break;
	}
}

void KviMdiChild::mouseMoveEvent(QMouseEvent *e)
{
	if(e->state() & Qt::LeftButton)
	{
		if(m_iResizeCorner && (m_state != Maximized))resizeWindowOpaque(m_iResizeCorner);
	} else {
		setResizeCursor(getResizeCorner(e->pos().x(), e->pos().y()));
	}
}

void KviMdiChild::setResizeCursor(int resizeCorner)
{
	if(resizeCorner == m_iLastCursorCorner)
		return; //Don't do it twice
	m_iLastCursorCorner = resizeCorner;
	if(resizeCorner == KVI_MDI_NORESIZE)
	{
		setCursor(getResizeCursor(resizeCorner));
		//if(QApplication::overrideCursor())QApplication::restoreOverrideCursor(); 
	} else {
		if(m_state != Maximized)
		{
			setCursor(getResizeCursor(resizeCorner));
			//QApplication::setOverrideCursor(getResizeCursor(resizeCorner),true);
		}
	}
}

void KviMdiChild::leaveEvent(QEvent *)
{
	if(!m_bResizeMode)
	{
		m_iResizeCorner=KVI_MDI_NORESIZE;
		m_iLastCursorCorner=KVI_MDI_NORESIZE;
		//if(QApplication::overrideCursor())QApplication::restoreOverrideCursor();
	} else {
		if(m_iResizeCorner != KVI_MDI_NORESIZE)resizeWindowOpaque(m_iResizeCorner);
	}
}

void KviMdiChild::calculateResizeRect(int resizeCorner,QPoint mousePos,QRect &resizeRect,int minWidth,int minHeight)
{
	switch(resizeCorner)
	{
		case KVI_MDI_RESIZE_LEFT:
			resizeRect.setLeft(mousePos.x() - 1);
			if(resizeRect.width() < minWidth)resizeRect.setLeft(resizeRect.right() - minWidth);
			if(resizeRect.x() < 0)resizeRect.setX(0);
			break; 
		case KVI_MDI_RESIZE_RIGHT: 
			resizeRect.setRight(mousePos.x() + 1);
			if(resizeRect.width() < minWidth)resizeRect.setRight(resizeRect.left() + minWidth);
			break; 
		case KVI_MDI_RESIZE_TOP:
			resizeRect.setTop(mousePos.y() - 1);
			if(resizeRect.height() < minHeight)resizeRect.setTop(resizeRect.bottom() - minHeight);
			if(resizeRect.y() < 0)resizeRect.setY(0);
			break; 
		case KVI_MDI_RESIZE_BOTTOM: 
			resizeRect.setBottom(mousePos.y() + 1);
			if(resizeRect.height() < minHeight)resizeRect.setBottom(resizeRect.top() + minHeight);
			break; 
		case KVI_MDI_RESIZE_BOTTOMRIGHT:
			resizeRect.setBottom(mousePos.y() + 1);
			if(resizeRect.height() < minHeight)resizeRect.setBottom(resizeRect.top() + minHeight);
			resizeRect.setRight(mousePos.x() + 1);
			if(resizeRect.width() < minWidth)resizeRect.setRight(resizeRect.left() + minWidth);
			break;
		case KVI_MDI_RESIZE_TOPRIGHT:
			resizeRect.setTop(mousePos.y() - 1);
			if(resizeRect.height() < minHeight)resizeRect.setTop(resizeRect.bottom() - minHeight);
			if(resizeRect.y() < 0)resizeRect.setY(0);
			resizeRect.setRight(mousePos.x() + 1);
			if(resizeRect.width() < minWidth)resizeRect.setRight(resizeRect.left() + minWidth);
			break;
		case KVI_MDI_RESIZE_BOTTOMLEFT:
			resizeRect.setBottom(mousePos.y() + 1);
			if(resizeRect.height() < minHeight)resizeRect.setBottom(resizeRect.top() + minHeight);
			resizeRect.setLeft(mousePos.x() - 1);
			if(resizeRect.width() < minWidth)resizeRect.setLeft(resizeRect.right() - minWidth);
			if(resizeRect.x() < 0)resizeRect.setX(0);
			break;
		case KVI_MDI_RESIZE_TOPLEFT:
			resizeRect.setTop(mousePos.y() - 1);
			if(resizeRect.height() < minHeight)resizeRect.setTop(resizeRect.bottom() - minHeight);
			if(resizeRect.y() < 0)resizeRect.setY(0);
			resizeRect.setLeft(mousePos.x() - 1);
			if(resizeRect.width() < minWidth)resizeRect.setLeft(resizeRect.right() - minWidth);
			if(resizeRect.x() < 0)resizeRect.setX(0);
			break;
	}
}

void KviMdiChild::calculateMinimumSize(int &minWidth,int &minHeight)
{
	if(m_pClient){
		minWidth  = m_pClient->minimumSize().width() + (KVI_MDICHILD_BORDER << 1);
		minHeight = m_pClient->minimumSize().height()+ (KVI_MDICHILD_BORDER << 1)+
					m_pCaption->heightHint() + KVI_MDICHILD_SPACING;
	}
	if(minWidth<KVI_MDICHILD_MIN_WIDTH)minWidth=KVI_MDICHILD_MIN_WIDTH;
	if(minHeight<KVI_MDICHILD_MIN_HEIGHT)minHeight=KVI_MDICHILD_MIN_HEIGHT;
}

void KviMdiChild::resizeWindowOpaque(int resizeCorner)
{
	int minWidth=0;
	int minHeight=0;
	QRect resizeRect(m_pManager->childX(this),m_pManager->childY(this),width(),height());
	calculateMinimumSize(minWidth,minHeight);
	QPoint mousePos = m_pManager->viewportToContents(m_pManager->viewport()->mapFromGlobal(QCursor::pos()));
	calculateResizeRect(resizeCorner,mousePos,resizeRect,minWidth,minHeight);
	m_pManager->moveChild(this,resizeRect.x(),resizeRect.y());
	resize(resizeRect.width(),resizeRect.height());
	m_pManager->childMoved(this);

	if(m_state == Maximized)
	{
		m_state=Normal;
		m_pManager->childRestored(this,true);
	}

}

int KviMdiChild::getResizeCorner(int ax,int ay)
{
	int ret = KVI_MDI_NORESIZE; 
	if((ax>0)&&(ax<(KVI_MDICHILD_BORDER+2))) ret |= KVI_MDI_RESIZE_LEFT; 
	if((ax<width())&&(ax>(width()-(KVI_MDICHILD_BORDER+2)))) ret |= KVI_MDI_RESIZE_RIGHT; 
	if((ay>0)&&(ay<(KVI_MDICHILD_BORDER+2))) ret |= KVI_MDI_RESIZE_TOP;
	if((ay<(height()))&&(ay>(height()-(KVI_MDICHILD_BORDER+2)))) ret |= KVI_MDI_RESIZE_BOTTOM; 
	return ret; 
}

void KviMdiChild::setClient(QWidget *w)
{
	__range_valid(m_pClient==0);
	__range_valid(w!=0);

	m_pClient = w;
	//resize to match the client
	int clientYPos=m_pCaption->heightHint()+KVI_MDICHILD_SPACING+KVI_MDICHILD_BORDER;
	resize(w->width()+(KVI_MDICHILD_BORDER << 1),w->height()+KVI_MDICHILD_BORDER+clientYPos);

	//Reparent if needed
	if(w->parent()!=this){
		//reparent to this widget , no flags , point , show it
		QPoint pnt2(KVI_MDICHILD_BORDER,clientYPos);
		w->reparent(this,pnt2,true);
	} else w->move(KVI_MDICHILD_BORDER,clientYPos);

	setFocusProxy(w);
	m_pCaption->setFocusProxy(w);

/*
	m_pMinimizeButton->setFocusProxy(w);
	m_pMaximizeButton->setFocusProxy(w);
	m_pCloseButton->setFocusProxy(w);
	m_pIconButton->setFocusProxy(w);
*/
	//linkChildren(w);

	if(m_pClient->minimumSize().width() > KVI_MDICHILD_MIN_WIDTH && 
		m_pClient->minimumSize().height() > KVI_MDICHILD_MIN_HEIGHT){
		setMinimumWidth(m_pClient->minimumSize().width() + (KVI_MDICHILD_BORDER << 1));
		setMinimumHeight(m_pClient->minimumSize().height()+ (KVI_MDICHILD_BORDER << 1) +
					m_pCaption->heightHint() + KVI_MDICHILD_SPACING);
	}

	KviStr tmp(KviStr::Format,"mdi_child_%s",w->name());
	setName(tmp.ptr());
}

void KviMdiChild::unsetClient()
{
	__range_valid(m_pClient!=0);
	if(!m_pClient)return;
	//reparent to desktop widget , no flags , point , show it
	//unlinkChildren(m_pClient);
	setFocusProxy(0); //remove the focus proxy...
	//Kewl...the reparent function has a small prob now..
	//the new toplelvel widgets gets not reenabled for dnd
#ifndef Q_OS_MACX
	m_pClient->reparent(0,m_pClient->mapToGlobal(QPoint(0,0)),true);
#else
	QRect r = g_pApp->desktop()->availableGeometry(m_pClient); 
	r.moveBy(0, 22);
	m_pClient->reparent(0,r.topLeft(),true);
#endif
	m_pClient=0;
	setName("mdi_child");
}



void KviMdiChild::activate(bool bSetFocus)
{
	if(!m_pCaption->active())m_pCaption->setActive(true);
	if(m_pManager->topChild() != this)
		m_pManager->setTopChild(this,bSetFocus);
	else if(bSetFocus)setFocus();
}

void KviMdiChild::focusInEvent(QFocusEvent *)
{
	// We gained focus by click , tab or from the caption label
	// Bring this child to top
	m_pCaption->setActive(true);
	m_pManager->setTopChild(this,false); //Do not focus by now...
	/*The client is our focusProxy ! it should be focused by Qt !*/
#ifdef _KVI_DEBUG_CLASS_NAME_
	__range_valid(focusProxy() == m_pClient);
#endif
}

QSize KviMdiChild::sizeHint()
{
	if(m_pClient)
	{
		QSize s = m_pClient->sizeHint();
		QSize ret(s.width() + (KVI_MDICHILD_BORDER << 1),
				s.height() + (KVI_MDICHILD_BORDER << 1) + KVI_MDICHILD_SPACING + m_pCaption->heightHint());
		return ret;
	}
	return QFrame::sizeHint();
}


