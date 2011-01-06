//=============================================================================
//
//   File : KviIrcToolBar.cpp
//   Creation date : Thu Oct 29 2000 14:13:13 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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


#include "KviIrcToolBar.h"
#include "KviConsoleWindow.h"
#include "KviMainWindow.h"
#include "KviLocale.h"
#include "KviIconManager.h"
#include "kvi_settings.h"
#include "KviOptions.h"
#include "KviMainWindow.h"
#include "KviApplication.h"
#include "KviDynamicToolTip.h"
#include "KviIrcUrl.h"
#include "KviInternalCommand.h"
#include "KviPointerList.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcContext.h"
#include "KviLagMeter.h"
#include "KviTalPopupMenu.h"
#include "KviMdiManager.h"
#include "KviPixmapUtils.h"

#include <QStyle>
#include <QPainter>
#include <QLayout>
#include <QEvent>
#include <QMouseEvent>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

static QPixmap                                  * g_pIccMemBuffer               = 0;
static KviPointerList<KviToolBarGraphicalApplet>    * g_pToolBarGraphicalAppletList = 0;

KviToolBarGraphicalApplet::KviToolBarGraphicalApplet(QWidget * par,const char * name)
: QToolButton(par)
{
	setObjectName(name);
	if(!g_pToolBarGraphicalAppletList)
	{
		g_pToolBarGraphicalAppletList = new KviPointerList<KviToolBarGraphicalApplet>();
		g_pToolBarGraphicalAppletList->setAutoDelete(false);
		g_pIccMemBuffer = new QPixmap(1,1);
	}

	g_pToolBarGraphicalAppletList->append(this);

	setAutoFillBackground(false);
	setMouseTracking(true);
	m_bResizeMode = false;

	m_sizeHint = QSize(180,32);
	m_bSizeLoaded = false;
}

unsigned int KviToolBarGraphicalApplet::loadAppletWidth()
{
	if(KVI_OPTION_UINT(KviOption_uintIrcContextAppletWidth) < 32)
		KVI_OPTION_UINT(KviOption_uintIrcContextAppletWidth) = 32;
	return KVI_OPTION_UINT(KviOption_uintIrcContextAppletWidth);
}

void KviToolBarGraphicalApplet::saveAppletWidth(unsigned int uWidth)
{
	KVI_OPTION_UINT(KviOption_uintIrcContextAppletWidth) = uWidth;
}

void KviToolBarGraphicalApplet::setupSizeHint()
{
	m_sizeHint = QSize(loadAppletWidth(),22);
	m_bSizeLoaded = true;
}

QSize KviToolBarGraphicalApplet::sizeHint() const
{
	// forget constness :(
	KviToolBarGraphicalApplet * that = (KviToolBarGraphicalApplet *)this;
	if(!m_bSizeLoaded)that->setupSizeHint();
	return m_sizeHint;
}

/*
toolbar.define(default)
{
	applet(thisandthat);
	applet(thisandthat);
	applet(thisandthat);
	applet(thisandthat);
}
*/


void KviToolBarGraphicalApplet::mouseMoveEvent(QMouseEvent * e)
{
	if(e->modifiers() & Qt::LeftButton)
	{
		if(m_bResizeMode)
		{
			int w = e->pos().x();
			if(w < 32)w = 32;
			if(w > 480)w = 480;
			m_sizeHint = QSize(w,22);
			resize(w,height());
			g_pApp->postEvent(parentWidget(),new QEvent(QEvent::LayoutRequest));
		}
	} else {
		if(e->pos().x() > width() - 4)
			setCursor(Qt::SizeHorCursor);
		else
			setCursor(Qt::ArrowCursor);
	}
}

void KviToolBarGraphicalApplet::mousePressEvent(QMouseEvent * e)
{
	if(e->button() & Qt::LeftButton)
	{
		m_bResizeMode = (e->pos().x() > (width() - 4));
	}
}

void KviToolBarGraphicalApplet::mouseReleaseEvent(QMouseEvent *)
{
	m_bResizeMode = false;
}


KviToolBarGraphicalApplet::~KviToolBarGraphicalApplet()
{
	saveAppletWidth(m_sizeHint.width());
	g_pToolBarGraphicalAppletList->removeRef(this);
	if(g_pToolBarGraphicalAppletList->isEmpty())
	{
		delete g_pToolBarGraphicalAppletList;
		g_pToolBarGraphicalAppletList = 0;
		delete g_pIccMemBuffer;
		g_pIccMemBuffer = 0;
	} else {
		// resize the mem buffer to match the maximum width / height of the applets
		resizeMemBuffer();
	}
}

void KviToolBarGraphicalApplet::resizeMemBuffer()
{
	int uMaxW = 0;
	int uMaxH = 0;
	for(KviToolBarGraphicalApplet * a = g_pToolBarGraphicalAppletList->first();a;a = g_pToolBarGraphicalAppletList->next())
	{
		if(uMaxW < a->width())uMaxW = a->width();
		if(uMaxH < a->height())uMaxH = a->height();
	}
	delete g_pIccMemBuffer;
	g_pIccMemBuffer=new QPixmap(uMaxW,uMaxH);
}

void KviToolBarGraphicalApplet::paintEvent(QPaintEvent *e)
{
	if(!isVisible())return;

	QPainter pa(g_pIccMemBuffer);

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		pa.save();
		pa.setCompositionMode(QPainter::CompositionMode_Source);
		QColor col=KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		pa.fillRect(rect(), col);
		pa.restore();
	} else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = mapTo(g_pMainWindow, rect().topLeft() + g_pMainWindow->mdiManager()->scrollBarsOffset());
		pa.drawTiledPixmap(rect(),*(g_pShadedChildGlobalDesktopBackground), pnt);
	} else {
#endif
		QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapIrcToolBarAppletBackground).pixmap();
		if(pix)
		{
			KviPixmapUtils::drawPixmapWithPainter(&pa,pix,KVI_OPTION_UINT(KviOption_pixmapIrcToolBarAppletBackground),rect(),rect().width(),rect().height());
		} else {
			pa.fillRect(rect(),KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletBackground));
		}
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	drawContents(&pa);

	//Need to draw the sunken rect around the view now...
	pa.setPen(palette().dark().color());
	pa.drawLine(0,0,width(),0);
	pa.drawLine(0,0,0,width());
	pa.setPen(palette().light().color());
	pa.drawLine(1,height() - 1,width() - 1,height() - 1);
	pa.drawLine(width() - 1,1,width() - 1,height());

	QPainter qt4SucksBecauseItNeedsAnAdditionalQPainter(this);
	qt4SucksBecauseItNeedsAnAdditionalQPainter.drawPixmap(e->rect().left(),e->rect().top(),e->rect().width(),e->rect().height(),*g_pIccMemBuffer,e->rect().left(),e->rect().top(),e->rect().width(),e->rect().height());
}

void KviToolBarGraphicalApplet::drawContents(QPainter *)
{
	// nothing here
}

void KviToolBarGraphicalApplet::resizeEvent(QResizeEvent *)
{
	unsigned int uBufferW = g_pIccMemBuffer->width();
	unsigned int uBufferH = g_pIccMemBuffer->height();
	unsigned int uW = width();
	unsigned int uH = height();
	if((uBufferW != uW) || (uBufferH != uH))
	{
		if((uBufferW < uW) && (uBufferH < uH)){
				delete g_pIccMemBuffer;
				g_pIccMemBuffer=new QPixmap(uW,uH);
			//			g_pIccMemBuffer->resize(uW,uH);
		}
		else resizeMemBuffer();
	}
}


//////////////////////////////////////////////////////////////////////////////
//
// KviIrcContextDisplay
//
//      Main applet of all irc contexts
//      Displays the server connection status, server name
//      nickname, user mode and the graphical indication of the context
//
//////////////////////////////////////////////////////////////////////////////

KviIrcContextDisplay::KviIrcContextDisplay(QWidget * par,const char * name)
: KviToolBarGraphicalApplet(par,name)
{
	KviDynamicToolTip * tip = new KviDynamicToolTip(this);
	connect(tip,SIGNAL(tipRequest(KviDynamicToolTip *,const QPoint &)),this,SLOT(tipRequest(KviDynamicToolTip *,const QPoint &)));
}


KviIrcContextDisplay::~KviIrcContextDisplay()
{
}

void KviIrcContextDisplay::tipRequest(KviDynamicToolTip * tip,const QPoint &)
{
	QString txt;

	KviConsoleWindow * c = g_pActiveWindow->console();

	static QString b = "<b>";
	static QString nb = "</b>";
	static QString br = "<br>";

	if(c)
	{
		KviIrcConnection * ic = c->connection();

		txt = b;

		if(!ic)
		{
			txt += __tr2qs("No connection");
			txt += nb;
			txt += br;
		} else {
			KviCString nickAndMode = ic->userInfo()->nickName();
			if(!(ic->userInfo()->userMode().isEmpty()))nickAndMode.append(KviCString::Format," (+%s)",ic->userInfo()->userMode().toUtf8().data());

			txt += ic->currentServerName();
			txt += nb;
			txt += br;
			txt += nickAndMode.ptr();
			txt += br;
		}

		QString szNum;
		szNum.setNum(c->context()->id());

		QString szIrcContext = QChar('(');
		szIrcContext += __tr2qs("IRC Context");
		szIrcContext += QChar(' ');
		szIrcContext += szNum;
		szIrcContext += QChar(')');
		txt += szIrcContext;

		if(ic && ic->lagMeter() && (KVI_OPTION_BOOL(KviOption_boolShowLagOnContextDisplay)))
		{
			txt += br;
			int lll;
			if((lll = ic->lagMeter()->lag()) > 0)
			{
				int llls = lll / 1000;
				int llld = (lll % 1000) / 100;
				int lllc = (lll % 100) / 10;
				KviQString::appendFormatted(txt,__tr2qs("Lag: %d.%d%d"),llls,llld,lllc);
			} else {
				txt += __tr2qs("Lag: ?.??");
			}
		}
	} else {
		txt = b;
		txt += __tr2qs("No IRC context");
		txt += nb;
	}

	tip->tip(rect(),txt);
}


/*
QSize KviIrcContextDisplay::sizeHint() const
{
	return QSize(160,22);
}
*/

#define KVI_APPLETIRCCONTEXTINDICATORWIDTH 12

void KviIrcContextDisplay::drawContents(QPainter * p)
{
	// The context indicator
	KviWindow * wnd = g_pActiveWindow;
	KviConsoleWindow * c = wnd ? wnd->console() : 0;

	if(c)
	{
		QString serv,nick;
        QString tmp;
		if(!c->connection())
		{
			serv = __tr2qs("Not connected");
		} else {
			if(c->isConnected())
			{
				KviIrcConnection * ic = c->connection();
				nick = ic->currentNickName();
				if(!ic->userInfo()->userMode().isEmpty())
				{
					static QString spp(" (+");
					nick += spp;
					nick += ic->userInfo()->userMode();
					if(ic->userInfo()->isAway())
					{
						nick += QChar(' ');
						nick += __tr2qs("away");
					}
					nick += QChar(')');
				} else {
					if(ic->userInfo()->isAway())
					{
						static QString ugly(" (");
						nick += ugly;
						nick += __tr2qs("away");
						nick += QChar(')');
					}
				}
				serv = ic->currentServerName();
				if(ic->lagMeter() && (KVI_OPTION_BOOL(KviOption_boolShowLagOnContextDisplay)))
				{
					nick += " ";
					int lll;
					if((lll = ic->lagMeter()->lag()) > 0)
					{
						int llls = lll / 1000;
						int llld = (lll % 1000) / 100;
						int lllc = (lll % 100) / 10;
						KviQString::appendFormatted(nick,__tr2qs("Lag: %d.%d%d"),llls,llld,lllc);
					} else {
						nick += __tr2qs("Lag: ?.??");
					}
				}
			} else {
				serv = __tr2qs("In progress...");
			}
		}

		p->setPen(KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletForegroundHighContrastActive1));

		p->setClipRect(KVI_APPLETIRCCONTEXTINDICATORWIDTH + 2,2,width() - (KVI_APPLETIRCCONTEXTINDICATORWIDTH + 4),height() - 4);

		if(height() < 30)
		{
			static QString xxx(" [");
			serv += xxx;
			serv += nick;
			serv += QChar(']');
			p->drawText(KVI_APPLETIRCCONTEXTINDICATORWIDTH + 4,16,serv);
		} else {
			p->drawText(KVI_APPLETIRCCONTEXTINDICATORWIDTH + 4,16,serv);
			p->drawText(KVI_APPLETIRCCONTEXTINDICATORWIDTH + 4,30,nick);
		}

		p->setClipping(false);

		QColor base = palette().background().color();
		QColor cntx = KVI_OPTION_ICCOLOR(c->context()->id() % KVI_NUM_ICCOLOR_OPTIONS);
		base.setRgb((base.red() + cntx.red()) >> 1,(base.green() + cntx.green()) >> 1,
			(base.blue() + cntx.blue()) >> 1);

		p->fillRect(2,2, KVI_APPLETIRCCONTEXTINDICATORWIDTH - 2,height() - 4,base);
	}

	p->setPen(KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletForegroundMidContrast));
	p->drawLine(1,1,width() - 1,1);
	p->drawLine(1,1,1,height() - 1);
	p->drawLine(2,height() - 2,width() - 1,height() - 2);
	p->drawLine(width() - 2,1,width() - 2,height());
	p->drawLine(KVI_APPLETIRCCONTEXTINDICATORWIDTH,2,KVI_APPLETIRCCONTEXTINDICATORWIDTH,height() - 2);

}


#ifdef Bool
	#undef Bool
#endif
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviIrcToolBar.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
