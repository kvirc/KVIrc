///////////////////////////////////////////////////////////////////////////////
//
//   File : kvi_styled_controls.cpp
//   Creation date : 19  Jan 2006 GMT by Alexey Uzhva
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


#define __KVIRC__

#include "kvi_styled_controls.h"
#include "kvi_iconmanager.h"
#include "kvi_app.h"
#include "kvi_options.h"
#include "kvi_doublebuffer.h"
#include "kvi_tal_toolbar.h"

#include "kvi_styled_controls.h"
#include <QPainter>
#include <QImage>
#include <QEvent>
#include <QTimer>
#include <QPaintEvent>


KviStyledControlInternal::KviStyledControlInternal( KviStyledControl* control)
:QObject(0)
{
	m_pControl=control;
}

KviStyledControlInternal::~KviStyledControlInternal()
{
	
}

bool KviStyledControlInternal::eventFilter( QObject *obj, QEvent *ev )
{
	if(ev->type()==QEvent::Enter)
	{
		m_pControl->enterEvent(ev);
	} else if(ev->type()==QEvent::Leave) {
		m_pControl->leaveEvent(ev);
	}
	return FALSE;
}

void KviStyledControlInternal::paintTimerShot ()
{
//	debug("%s %s %i",__FILE__,__FUNCTION__,__LINE__);
//	debug("%s %i",__FUNCTION__,m_pControl->m_iStepNumber);
	if(m_pControl->m_bMouseEnter)
	{
		m_pControl->m_iStepNumber++;
		if(m_pControl->m_iStepNumber>=KVI_STYLE_NUM_STEPS)
		{
			m_pControl->m_iStepNumber=KVI_STYLE_NUM_STEPS;
			m_pControl->m_pTimer->stop();
		}
	} else {
		m_pControl->m_iStepNumber--;
		if(m_pControl->m_iStepNumber<=0) 
		{
			m_pControl->m_iStepNumber=0;
			m_pControl->m_pTimer->stop();
		}
	}
	m_pControl->m_pWidget->repaint();
}

KviStyledControl::KviStyledControl(QWidget* w)
{
	m_pWidget=w;
	m_iStepNumber=0;
	m_bMouseEnter=0;
	m_pInternal=new KviStyledControlInternal(this);
	m_pTimer = new QTimer(m_pInternal);
	m_pWidget->installEventFilter(m_pInternal);
}

KviStyledControl::~KviStyledControl()
{
	delete m_pTimer;
	delete m_pInternal;
}

void KviStyledControl::enterEvent ( QEvent * )
{
//	debug("%s %s %i",__FILE__,__FUNCTION__,__LINE__);
	if(m_pWidget->isEnabled() && KVI_OPTION_BOOL(KviOption_boolEnableVisualEffects))
	{
		if(m_iStepNumber<KVI_STYLE_NUM_STEPS)
		{
			if(!m_pTimer->isActive()) 
			{
				m_pTimer->connect( m_pTimer, SIGNAL(timeout()), m_pInternal, SLOT(paintTimerShot()) );
				m_pTimer->start(KVI_STYLE_TIMER_STEP);
			}
		}
		m_bMouseEnter=1;
		m_iStepNumber++;
		m_pWidget->repaint();
	}
}

void KviStyledControl::leaveEvent ( QEvent * )
{
//	debug("%s %s %i",__FILE__,__FUNCTION__,__LINE__);
	if(m_pWidget->isEnabled() && KVI_OPTION_BOOL(KviOption_boolEnableVisualEffects))
	{
		if(m_iStepNumber>0) 
		{
			if(!m_pTimer->isActive())
			{
				m_pTimer->connect( m_pTimer, SIGNAL(timeout()), m_pInternal, SLOT(paintTimerShot()) );
				m_pTimer->start(KVI_STYLE_TIMER_STEP);
			}
		}
		m_bMouseEnter=0;
		m_iStepNumber--;
		m_pWidget->repaint();
	}
}


KviStyledCheckBox::KviStyledCheckBox ( QWidget * parent, const char * name )
: QCheckBox(parent), KviStyledControl(this)
{
	setObjectName(name);
	//setWFlags(WNoAutoErase);

}

KviStyledCheckBox::KviStyledCheckBox ( const QString & text, QWidget * parent, const char * name )
: QCheckBox(text,parent), KviStyledControl(this)
{
	setObjectName(name);
	//setWFlags(WNoAutoErase);
}

KviStyledCheckBox::~KviStyledCheckBox()
{
}

void KviStyledCheckBox::paintEvent ( QPaintEvent * event)
{
	//debug("%s %s %i %i %i",__FILE__,__FUNCTION__,__LINE__,m_bMouseEnter,m_iStepNumber);
	if(KVI_OPTION_BOOL(KviOption_boolEnableVisualEffects))
	{
		KviDoubleBuffer doublebuffer(event->rect().width(),event->rect().height());
		QPixmap * pDoubleBufferPixmap = doublebuffer.pixmap();
		
		QRect rect=event->rect();
		//pDoubleBufferPixmap->fill(this, rect.topLeft()); 
		
		QPainter p( this); 
		//p.drawRect(rect.topLeft()); 
		//pDoubleBufferPixmap,
		p.translate(-rect.x(), -rect.y()); 
		
		QPixmap*  pStoredPix = 0;
		
		if(isChecked())
			pStoredPix=g_pIconManager->getBigIcon("kvi_checkbox_selected.png");
		else
			pStoredPix=g_pIconManager->getBigIcon("kvi_checkbox_unselected.png");
		//debug("%s %s %i %i %i",__FILE__,__FUNCTION__,__LINE__,m_bMouseEnter,m_iStepNumber);
		if(pStoredPix)
		{
			//debug("%s %s %i %i %i",__FILE__,__FUNCTION__,__LINE__,m_bMouseEnter,m_iStepNumber);
			QPixmap pix=*pStoredPix;
			if(m_iStepNumber && isEnabled())
			{
				QImage image = pix.toImage().convertToFormat(QImage::Format_ARGB32);;
				for(int x=0; x<image.width(); x++)
					for(int y=0; y<image.height(); y++)
					{
						QRgb rgb=image.pixel(x,y);
						rgb=qRgba(
							qRed(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber < 255 ? qRed(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber : 255 ,
							qGreen(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber < 255 ? qGreen(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber : 255,
							qBlue(rgb),
							qAlpha(rgb)
							);
						image.setPixel(x,y,rgb);
					}
				p.drawImage(0,0,image);
				} else if( !isEnabled()) {
				QImage image = pix.toImage().convertToFormat(QImage::Format_ARGB32);
				for(int x=0; x<image.width(); x++)
					for(int y=0; y<image.height(); y++)
					{
						//- From Black to Gray conversion for disabled mode.
						QRgb rgb=image.pixel(x,y);
						rgb=qRgba(
							qRed(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber == 0 ? 180 : qRed(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber,
							qGreen(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber == 0 ? 180 : qGreen(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber,
							qBlue(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber == 0 ? 180 : qBlue(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber,
							qAlpha(rgb)
							);
						image.setPixel(x,y,rgb);
					}
					p.drawImage(0,0,image);
			} else {
			p.drawPixmap(0,0,pix);
			}
			//debug("%s %s %i %i %i",__FILE__,__FUNCTION__,__LINE__,m_bMouseEnter,m_iStepNumber);
			QString szText=text();
			szText=szText.remove("&");
			p.drawText(pix.width()+3,0,width(),height(),0,szText);
	//		bitBlt(this, rect.x(), rect.y(), pDoubleBufferPixmap, 0, 0, rect.width(), rect.height());
			//debug("%s %s %i %i %i",__FILE__,__FUNCTION__,__LINE__,m_bMouseEnter,m_iStepNumber);
		} else	{
			QCheckBox::paintEvent(event);
		}
	} else
		QCheckBox::paintEvent(event);
}

KviStyledToolButton::KviStyledToolButton( QWidget * parent, const char * name )
: QToolButton(parent), KviStyledControl(this), bShowSubmenuIndicator(false)
{
	//setWFlags(WNoAutoErase);
	setObjectName(name);
	resizeEvent(0);
}

KviStyledToolButton::KviStyledToolButton ( const QIcon & iconSet, const QString & textLabel, const QString & grouptext, QObject * receiver, const char * slot, KviTalToolBar * parent, const QString &name)
: QToolButton(iconSet,textLabel,grouptext,receiver,slot,parent,name.toUtf8().data()), KviStyledControl(this), bShowSubmenuIndicator(false)
{
//	setWFlags(WNoAutoErase);
	resizeEvent(0);
}

KviStyledToolButton::~KviStyledToolButton()
{
}

void KviStyledToolButton::setShowSubmenuIndicator(bool bShow)
{
	bShowSubmenuIndicator=bShow;
	resizeEvent(0);
}

void KviStyledToolButton::paintEvent ( QPaintEvent * event)
{
	if(KVI_OPTION_BOOL(KviOption_boolEnableVisualEffects))
	{
		QPixmap*  pArrowPix = 0;
		int iWidth=width();
		int iPixWidth=0;
		if(bShowSubmenuIndicator)
		{
			if(pArrowPix=g_pIconManager->getBigIcon("kvi_toolbutton_menuindicator.png"))
				iPixWidth=pArrowPix->width();
		}
		bool bActive= isChecked() || m_bMouseEnter;
		//KviDoubleBuffer doublebuffer(event->rect().width(),event->rect().height());
		//QPixmap * pDoubleBufferPixmap = doublebuffer.pixmap();

		QRect rect=event->rect();
		
		/*pDoubleBufferPixmap->fill(
			bActive ? QColor(206,215,223) :
			colorGroup().background()
			);
		*/
		QPainter p(this); 
			p.setPen(bActive ? QColor(206,215,223) :palette().background().color());
			p.setBrush(bActive ? QColor(206,215,223) :palette().background());

		p.drawRect(event->rect());
		if(bActive)
		{
			p.setPen(QColor(185,190,195));
			p.drawRect(0,0,iWidth,height());
		}

		p.translate(-rect.x(), -rect.y());
		QPixmap pix=iconSet().pixmap(
                                usesBigPixmap() ? QIconSet::Large : QIconSet::Small,
                                isEnabled() ? QIcon::Normal :
                                              QIconSet::Disabled,
                                isChecked() ? QIcon::On :
                                              QIcon::Off);
		QPoint pos((iWidth-iPixWidth-pix.width())/2,(height()-pix.height())/2);
		if(!pix.isNull())
		{
			if(m_iStepNumber && isEnabled())
			{
				QImage image = pix.toImage().convertToFormat(QImage::Format_ARGB32);
				for(int x=0; x<image.width(); x++)
					for(int y=0; y<image.height(); y++)
					{
						QRgb rgb=image.pixel(x,y);
						rgb=qRgba(
							qRed(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber < 255 ? qRed(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber : 255 ,
							qGreen(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber < 255 ? qGreen(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber : 255,
							qBlue(rgb),
							qAlpha(rgb)
							);
						image.setPixel(x,y,rgb);
					}
				p.drawImage(pos,image);
				} else if( !isEnabled()) {
				QImage image = pix.toImage().convertToFormat(QImage::Format_ARGB32);
				for(int x=0; x<image.width(); x++)
					for(int y=0; y<image.height(); y++)
					{
						//- From Black to Gray conversion for disabled mode.
						QRgb rgb=image.pixel(x,y);
						rgb=qRgba(
							qRed(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber == 0 ? 180 : qRed(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber,
							qGreen(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber == 0 ? 180 : qGreen(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber,
							qBlue(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber == 0 ? 180 : qBlue(rgb)+KVI_STYLE_COLOR_DIFF/KVI_STYLE_NUM_STEPS*m_iStepNumber,
							qAlpha(rgb)
							);
						image.setPixel(x,y,rgb);
					}
					p.drawImage(pos,image);
			} else {
			p.drawPixmap(pos,pix);
			}
		}
		
		if(pArrowPix)
		{
			pos.setX(pix.width()+4);
			pos.setY((height()-pArrowPix->height())/2);
			p.drawPixmap(pos,*pArrowPix);
		}

	//	bitBlt(this, rect.x(), rect.y(), pDoubleBufferPixmap, 0, 0, rect.width(), rect.height());
		
	} else {
		QToolButton::paintEvent(event);
	}
}

void KviStyledToolButton::resizeEvent ( QResizeEvent * e)
{
	QPixmap* pStoredPix = 0;
	if(pStoredPix=g_pIconManager->getBigIcon("kvi_toolbutton_menuindicator.png"))
	{
		QPixmap pix=iconSet().pixmap(
			iconSize().height() > 22 ? QIconSet::Large : QIconSet::Small,
			isEnabled() ? QIcon::Normal : QIconSet::Disabled,
			isChecked() ? QIcon::On : QIcon::Off);
		
		setMinimumWidth(bShowSubmenuIndicator ? pix.width()+8+pStoredPix->width() : pix.width());
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_styled_controls.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
