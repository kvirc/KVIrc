//=============================================================================
//
//   File : kvi_iograph.cpp
//   Creation date : Tue Oct 31 2000 00:14:12 CEST by Szymon Stefanek
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

#include <math.h>

#include "libkviiograph.h"
#include "KviMainWindow.h"
#include "KviIconManager.h"
#include "KviLocale.h"
#include "KviMdiManager.h"
#include "KviModule.h"
#include "KviOptions.h"
#include "kvi_socket.h"

#include <QPainter>
#include <QPaintEvent>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern KVIRC_API QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

KviIOGraphWindow* g_pIOGraphWindow = 0;

KviIOGraphWindow::KviIOGraphWindow(KviModuleExtensionDescriptor * d,KviMainWindow * lpFrm,const char * name)
: KviWindow(KviWindow::IOGraph,lpFrm,name), KviModuleExtension(d)
{
	m_pIOGraph = new KviIOGraphWidget(this);
	setAutoFillBackground(false);
}

KviIOGraphWindow::~KviIOGraphWindow()
{
	if(m_pIOGraph)
		delete m_pIOGraph;
	m_pIOGraph=0;

	g_pIOGraphWindow = 0;
}

QPixmap * KviIOGraphWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::SayIcon);
}

void KviIOGraphWindow::resizeEvent(QResizeEvent *)
{
	m_pIOGraph->setGeometry(0,0,width(),height());
}

void KviIOGraphWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs("I/O Traffic Graph");
}

void KviIOGraphWindow::die()
{
	close();
}

void KviIOGraphWindow::updatePseudoTransparency()
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	update();
#endif
}

void KviIOGraphWindow::moveEvent(QMoveEvent *)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	updatePseudoTransparency();
#endif
}

void KviIOGraphWindow::paintEvent(QPaintEvent * e)
{
	QPainter p(this);

	QRect rect = e->rect();
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p.save();
		p.setCompositionMode(QPainter::CompositionMode_Source);
		QColor col=KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p.fillRect(rect, col);
		p.restore();
	} else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = mdiParent() ? mapTo(g_pMainWindow, rect.topLeft() + g_pMainWindow->mdiManager()->scrollBarsOffset()) : rect.topLeft();
		p.drawTiledPixmap(rect,*(g_pShadedChildGlobalDesktopBackground), pnt);
	} else {
#endif
		//FIXME this is not the treewindowlist
		p.fillRect(rect,KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground));
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif
}

KviIOGraphWidget::KviIOGraphWidget(QWidget * par)
: QWidget(par)
{
	m_uLastSentBytes = g_uOutgoingTraffic;
	m_uLastRecvBytes = g_uIncomingTraffic;

	m_maxRate = 1;

	unsigned int iMax = qMax(m_uLastSentBytes, m_uLastRecvBytes);
	while(iMax > m_maxRate)
		m_maxRate*=2;

	m_sendRates.prepend(0);
	m_recvRates.prepend(0);

	QString tip("<font color=\"#FF0000\">");
	tip.append(__tr("Outgoing traffic"));
	tip.append("</font><br/><font color=\"#0000FF\">");
	tip.append(__tr("Incoming traffic"));
	tip.append("</font>");

	this->setToolTip(tip);

	startTimer(1000);
}

void KviIOGraphWidget::timerEvent(QTimerEvent *)
{
	static int uLastResize=0;

	kvi_u64_t sB = g_uOutgoingTraffic;
	kvi_u64_t rB = g_uIncomingTraffic;

	unsigned int sDiff = sB > m_uLastSentBytes ? (sB - m_uLastSentBytes) : 0;
	unsigned int rDiff = rB > m_uLastRecvBytes ? (rB - m_uLastRecvBytes) : 0;

	unsigned int iMax = qMax(sDiff, rDiff);

	if(uLastResize == 0)
	{
		if(m_maxRate > 1)
		{
			m_maxRate=1;
			for(int i=0;i<m_sendRates.count();++i)
				while(m_sendRates.at(i) > m_maxRate) m_maxRate*=2;
			for(int i=0;i<m_recvRates.count();++i)
				while(m_recvRates.at(i) > m_maxRate) m_maxRate*=2;
		}
	} else uLastResize--;

	if(iMax > m_maxRate)
	{
		while(iMax > m_maxRate)
			m_maxRate*=2;
		uLastResize=KVI_IOGRAPH_NUMBER_POINTS;
	}

	m_uLastSentBytes = sB;
	m_uLastRecvBytes = rB;


	m_sendRates.prepend(sDiff);
	if(m_sendRates.count()>(KVI_IOGRAPH_NUMBER_POINTS+1))
		m_sendRates.removeLast();
	m_recvRates.prepend(rDiff);
	if(m_recvRates.count()>(KVI_IOGRAPH_NUMBER_POINTS+1))
		m_recvRates.removeLast();

	update();
}

void KviIOGraphWidget::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	p.setRenderHint(QPainter::Antialiasing);
	p.setPen(QColor("#c0c0c0"));

	float c = 1.0;
	float sw = (width() - 2.0) / KVI_IOGRAPH_VERT_SEGMENTS;
	float sh = (height() - 2.0) / KVI_IOGRAPH_HORIZ_SEGMENTS;

	for(int i=0;i<=KVI_IOGRAPH_HORIZ_SEGMENTS;i++)
	{
		p.drawLine(0, (int)c, width(), (int)c);
		if(i>0)
			p.drawText(2,(int)c,KviQString::makeSizeReadable(m_maxRate * (KVI_IOGRAPH_HORIZ_SEGMENTS - i) / KVI_IOGRAPH_HORIZ_SEGMENTS));
		c+=sh;
	}

	c=1;
	for(int i=0;i<=KVI_IOGRAPH_VERT_SEGMENTS;i++)
	{
		p.drawLine((int)c, 0, (int)c, height());
		c+=sw;
	}

	QPainterPath sP, rP;
	float wStep=(width() - 2.0) / KVI_IOGRAPH_NUMBER_POINTS;


	sP.moveTo(QPointF(width(), height()));
	c = 1.0;
	for(int i = 0;(i <= (KVI_IOGRAPH_NUMBER_POINTS + 1)) && (i < m_sendRates.count());i++)
	{
		sP.lineTo(QPointF(width()- c, height() - (height() * m_sendRates.at(i) / m_maxRate)));
		c+=wStep;
	}
	sP.lineTo(QPointF(0, height()));

	rP.moveTo(QPointF(width(), height()));
	c = 1.0;
	for(int i = 0;(i <= (KVI_IOGRAPH_NUMBER_POINTS + 1)) && (i < m_recvRates.count());i++)
	{
		rP.lineTo(QPointF(width()-c,  height() - (height() * m_recvRates.at(i) / m_maxRate)));
		c+=wStep;
	}
	rP.lineTo(QPointF(0, height()));

	p.setPen(QColor(0, 0, 255));
	p.setBrush(QColor(0, 0, 255, 128));
	p.drawPath(rP);

	p.setPen(QColor(255, 0, 0));
	p.setBrush(QColor(255, 0, 0, 128));
	p.drawPath(sP);
}

/*
	@doc: iograph.open
	@type:
		command
	@title:
		iograph.open
	@short:
		Opens the IOGraph chart
	@description:
		Opens the IOGraph chart. It will
		monitor the Incoming and Outgoing socket traffic.[br]
*/

static bool iograph_module_cmd_open(KviKvsModuleCommandCall * c)
{
	KviModuleExtensionDescriptor * d = c->module()->findExtensionDescriptor("tool",IOGRAPH_MODULE_EXTENSION_NAME);

	if(d)
	{
		KviPointerHashTable<QString,QVariant> dict(17,true);
		dict.setAutoDelete(true);
		QString dummy;
		dict.replace("bCreateMinimized",new QVariant(c->hasSwitch('m',dummy)));
		dict.replace("bNoRaise",new QVariant(c->hasSwitch('n',dummy)));

		d->allocate(c->window(),&dict,0);
	} else {
		c->warning(__tr("Ops.. internal error"));
	}
	return true;
}

static KviModuleExtension * iograph_extension_alloc(KviModuleExtensionAllocStruct * s)
{
	bool bCreateMinimized = false;
	bool bNoRaise = false;

	if(!g_pIOGraphWindow)
	{
		if(s->pParams)
		{
			if(QVariant * v = s->pParams->find("bCreateMinimized"))
			{
				if(v->isValid())
				{
					if(v->type() == QVariant::Bool)
					{
						bCreateMinimized = v->toBool();
					}
				}
			}
		}

		g_pIOGraphWindow = new KviIOGraphWindow(s->pDescriptor,g_pMainWindow,"IOGraph Window");
		g_pMainWindow->addWindow(g_pIOGraphWindow,!bCreateMinimized);

		if(bCreateMinimized)g_pIOGraphWindow->minimize();
		return g_pIOGraphWindow;
	}

	if(s->pParams)
	{
		if(QVariant * v = s->pParams->find("bNoRaise"))
		{
			if(v)
			{
				if(v->isValid() && v->type() == QVariant::Bool)
					bNoRaise = v->toBool();
			}
		}
	}

	if(!bNoRaise)
		g_pIOGraphWindow->delayedAutoRaise();
	return g_pIOGraphWindow;
}

static bool iograph_module_init(KviModule *m)
{
	g_pIOGraphWindow = 0;

	KVSM_REGISTER_SIMPLE_COMMAND(m,"open",iograph_module_cmd_open);

	KviModuleExtensionDescriptor * d = m->registerExtension("tool",
							IOGRAPH_MODULE_EXTENSION_NAME,
							__tr2qs("Show I/O &Traffic graph"),
							iograph_extension_alloc);

	if(d)d->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::SayIcon)));

	return true;
}

static bool iograph_module_cleanup(KviModule *)
{
	if(g_pIOGraphWindow && g_pMainWindow)
		g_pMainWindow->closeWindow(g_pIOGraphWindow);
	g_pIOGraphWindow = 0;
	return true;
}

static bool iograph_module_can_unload(KviModule *)
{
	return (!g_pIOGraphWindow);
}

KVIRC_MODULE(
	"IOGraph",                                              // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2008 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"IRC socket traffic monitor",
	iograph_module_init,
	iograph_module_can_unload,
	0,
	iograph_module_cleanup,
	0
)

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "libkviiograph.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
