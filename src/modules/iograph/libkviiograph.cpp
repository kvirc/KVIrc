//
//   File : kvi_iograph.cpp
//   Creation date : Tue Oct 31 2000 00:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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


#include "libkviiograph.h"
#include "kvi_module.h"

#if 0


#include "kvi_console.h"
#include "kvi_options.h"
#include "kvi_ircsocket.h"
#include "kvi_frame.h"
#include "kvi_locale.h"

#include "kvi_list.h"
#include <qtooltip.h>
#include <qpointarray.h>
#include <qpainter.h>


extern QPixmap                 * g_pIccMemBuffer;


static KviPtrList<KviIOGraphDisplay> * g_pIOGraphWidgetList;

KviIOGraphDisplay::KviIOGraphDisplay(KviIrcContextController * pController,bool sentGraph,bool recvGraph)
: KviIrcContextGraphicalApplet(pController,"iograph_display")
{
	g_pIOGraphWidgetList->append(this);
	for(int i=0;i < KVI_IOGRAPH_NUMBER_POINTS;i++)
	{
		m_sendRates[i] = 0;
		m_recvRates[i] = 0;
	}
	m_iNextPoint = 1;
	m_uLastSentBytes = pController->console()->socket()->sentBytes();
	m_uLastRecvBytes = pController->console()->socket()->readBytes();
	m_bShowSentGraph = sentGraph;
	m_bShowRecvGraph = recvGraph;

	KviStr tip;
	if(sentGraph)tip = __tr("Outgoing traffic");
	if(recvGraph)
	{
		if(tip.hasData())tip.append("\n");
		tip.append(__tr("Incoming traffic"));
	}

	QToolTip::add(this,tip.ptr());

	startTimer(1000);
}

KviIOGraphDisplay::~KviIOGraphDisplay()
{
	g_pIOGraphWidgetList->removeRef(this);
}

void KviIOGraphDisplay::timerEvent(QTimerEvent *e)
{
	unsigned int sB = console()->socket()->sentBytes();
	unsigned int rB = console()->socket()->readBytes();
	int sDiff = (sB - m_uLastSentBytes) / 8;
	int rDiff = (rB - m_uLastRecvBytes) / 32;
//	debug("s:%d,r:%d",sDiff,rDiff);
	if(sDiff < 0)sDiff = 0;
	else if(sDiff > 30)sDiff = 30;
	if(rDiff < 0)rDiff = 0;
	else if(rDiff > 30)rDiff = 30;
	m_uLastSentBytes = sB;
	m_uLastRecvBytes = rB;
	m_sendRates[m_iNextPoint] = sDiff;
	m_recvRates[m_iNextPoint] = rDiff;
	m_iNextPoint++;
	if(m_iNextPoint >= KVI_IOGRAPH_NUMBER_POINTS)m_iNextPoint = 0;
	update();
}

static QCOORD horizSegments[6 * 4]=
{
	5 , 3  , KVI_IRCTOOLBARAPPLET_MAXIMUM_WIDTH - 4 , 3  ,
	5 , 8  , KVI_IRCTOOLBARAPPLET_MAXIMUM_WIDTH - 4 , 8  ,
	5 , 13 , KVI_IRCTOOLBARAPPLET_MAXIMUM_WIDTH - 4 , 13 ,
	5 , 18 , KVI_IRCTOOLBARAPPLET_MAXIMUM_WIDTH - 4 , 18 ,
	5 , 23 , KVI_IRCTOOLBARAPPLET_MAXIMUM_WIDTH - 4 , 23 ,
	5 , 28 , KVI_IRCTOOLBARAPPLET_MAXIMUM_WIDTH - 4 , 28
};

static QCOORD vertSegments[29 * 4]=
{
	 9   , 4  , 9   , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 14  , 4  , 14  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 19  , 4  , 19  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 24  , 4  , 24  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 29  , 4  , 29  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 34  , 4  , 34  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 39  , 4  , 39  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 44  , 4  , 44  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 49  , 4  , 49  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 54  , 4  , 54  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 59  , 4  , 59  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 64  , 4  , 64  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 69  , 4  , 69  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 74  , 4  , 74  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 79  , 4  , 79  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 84  , 4  , 84  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 89  , 4  , 89  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 94  , 4  , 94  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 99  , 4  , 99  , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 104 , 4  , 104 , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 109 , 4  , 109 , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 114 , 4  , 114 , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 119 , 4  , 119 , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 124 , 4  , 124 , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 129 , 4  , 129 , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 134 , 4  , 134 , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 139 , 4  , 139 , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 144 , 4  , 144 , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 ,
	 149 , 4  , 149 , KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5 
};

void KviIOGraphDisplay::drawContents(QPainter * p)
{
	static QPointArray hp(6 * 4,horizSegments);
	static QPointArray vp(29 * 4,vertSegments);

	p->setPen(KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletForegroundLowContrast));
	p->drawLineSegments(hp,0,6);
	p->drawLineSegments(vp,0,29);
	p->setPen(KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletForegroundMidContrast));

	p->drawLine(4,33,KVI_IRCTOOLBARAPPLET_MAXIMUM_WIDTH - 4,33);
	p->drawLine(4,3,4,KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT - 5);

	p->setPen(isActiveContext() ?
			KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletForegroundHighContrastActive1) : 
			KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletForegroundHighContrastInactive1));

	// the first point to draw is "m_iNextPoint"
	// it will be drawn at X = 4

	int leftPart = KVI_IOGRAPH_NUMBER_POINTS - m_iNextPoint;

	int i;

	if(m_bShowSentGraph)
	{
		for(i = 1;i < leftPart;i++)
		{
			p->drawLine(3 + i,33 - m_sendRates[m_iNextPoint + i - 1],4 + i,33 - m_sendRates[m_iNextPoint + i]);
		}
	
		p->drawLine(3 + i,33 - m_sendRates[m_iNextPoint + i - 1],4 + i,33 - m_sendRates[0]);
	
		for(i = 1;i < m_iNextPoint;i++)
		{
			p->drawLine(leftPart + 3 + i,33 - m_sendRates[i - 1],leftPart + 4 + i,33 - m_sendRates[i]);
		}
	}

	if(m_bShowRecvGraph)
	{
		p->setPen(isActiveContext() ?
				KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletForegroundHighContrastActive2) : 
				KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletForegroundHighContrastInactive2));
	
		for(i = 1;i < leftPart;i++)
		{
			p->drawLine(3 + i,33 - m_recvRates[m_iNextPoint + i - 1],4 + i,33 - m_recvRates[m_iNextPoint + i]);
		}
	
		p->drawLine(3 + i,33 - m_recvRates[m_iNextPoint + i - 1],4 + i,33 - m_recvRates[0]);

		for(i = 1;i < m_iNextPoint;i++)
		{
			p->drawLine(leftPart + 3 + i,33 - m_recvRates[i - 1],leftPart + 4 + i,33 - m_recvRates[i]);
		}
	}
}

QSize KviIOGraphDisplay::sizeHint() const
{
	return QSize(KVI_IRCTOOLBARAPPLET_MAXIMUM_WIDTH,KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT);
}


/*
	@doc: iograph.add
	@type:
		command
	@title:
		iograph.add
	@short:
		Adds an IOGraph applet
	@syntax:
		iograph.add [-i] [-o]
	@description:
		Adds an IOGraph applet to the current irc-context toolbar. It will
		monitor the Incoming and Outgoing socket traffic.[br]
		Since both graphs often are somewhat unreadable,
		the -i switch will cause the IOGraph applet to show only
		the incoming traffic, and the -o switch will cause it to show
		only the outgoing traffic.[br]
		Well...I agree...this is an amazing misuse of resources :)
*/



static bool iograph_module_cmd_add(KviModule *m,KviCommand *c)
{
	ENTER_STACK_FRAME(c,"iograph_module_cmd_add");

	KviStr dummy;
	if(!g_pUserParser->parseCmdFinalPart(c,dummy))return false;

	if(!c->window()->console())return c->noIrcContext();

	bool bSentGraph = !(c->hasSwitch('i'));
	bool bRecvGraph = !(c->hasSwitch('o'));

	if(!(bSentGraph || bRecvGraph))bRecvGraph = true;

	KviIOGraphDisplay * dpy = new KviIOGraphDisplay(c->window()->console()->icController(),bSentGraph,bRecvGraph);
	c->window()->console()->icController()->addApplet(dpy);
	dpy->show();

	return c->leaveStackFrame();
}
#endif

static bool iograph_module_init(KviModule *m)
{
#if 0
	g_pIOGraphWidgetList = new KviPtrList<KviIOGraphDisplay>;
	g_pIOGraphWidgetList->setAutoDelete(false);
	m->registerCommand("add",iograph_module_cmd_add);
#endif
	return true;
}

static bool iograph_module_cleanup(KviModule *m)
{
#if 0
	while(g_pIOGraphWidgetList->first())
	{
		KviIOGraphDisplay * dpy = g_pIOGraphWidgetList->first();
		dpy->controller()->removeApplet(dpy); // deleted path
	}
	delete g_pIOGraphWidgetList;
#endif
	return true;
}

static bool iograph_module_can_unload(KviModule *m)
{
#if 0
	return g_pIOGraphWidgetList->isEmpty();
#else
	return true;
#endif
}

KVIRC_MODULE(
	"IOGraph",                                              // module name
	"1.0.0",                                                // module version
	"Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"IRC socket traffic monitor",
	iograph_module_init,
	iograph_module_can_unload,
	0,
	iograph_module_cleanup
)

#if 0
#include "libkviiograph.moc"
#endif
