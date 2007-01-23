#define __KVIRC__


#include <qlayout.h> 
#include <qtoolbutton.h> 
#include <qobjectcleanuphandler.h> 
#include <qpushbutton.h> 
#include "kvi_tal_vbox.h"
#include <qpainter.h>
#include <qapplication.h>

#include "kvi_toolwindows_container.h"
#include "kvi_iconmanager.h"


KviWindowToolWidget::KviWindowToolWidget(QWidget * parent, KviWindowToolPageButton* button/*, const char * name, WFlags f*/ )
:QWidget(parent/*,name,f*/)
{
//	m_pContainer=parent;
	m_pButton=button;
	m_bAutoDelete=1;
	m_bHidden=0;
	if(m_pButton) m_pButton->setOn(!isHidden());
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
:KviStyledToolButton(parent,name)
{
	setOn(bOn);
	QIconSet is1;
	is1.setPixmap(*(g_pIconManager->getSmallIcon(pixon)),QIconSet::Small,QIconSet::Normal,QIconSet::On);
	is1.setPixmap(*(g_pIconManager->getSmallIcon(pixoff)),QIconSet::Small,QIconSet::Normal,QIconSet::Off);
	setIconSet(is1);
	setToggleButton(true);
	setUsesBigPixmap(false);
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
		icon=iconSet()->pixmap(QIconSet::Small,QIconSet::Normal,QIconSet::On);
	else
		icon=iconSet()->pixmap(QIconSet::Small,QIconSet::Normal,QIconSet::Off);
		
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
