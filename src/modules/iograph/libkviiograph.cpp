//=============================================================================
//
//   File : kvi_iograph.cpp
//   Creation date : Tue Oct 31 2000 00:14:12 CEST by Szymon Stefanek
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

#include "libkviiograph.h"
#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_locale.h"
#include "kvi_module.h"
#include "kvi_options.h"
#include "kvi_socket.h"

#include <QPainter>
#include <QPaintEvent>

KviIOGraphWindow* g_pIOGraphWindow = 0;

KviIOGraphWindow::KviIOGraphWindow(KviModuleExtensionDescriptor * d,KviFrame * lpFrm,const char * name)
: KviWindow(KVI_WINDOW_TYPE_IOGRAPH,lpFrm,name), KviModuleExtension(d)
{
	m_pIOGraph = new KviIOGraphWidget(this);
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
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_SAYICON);
}

void KviIOGraphWindow::resizeEvent(QResizeEvent *e)
{
	m_pIOGraph->setGeometry(0,0,width(),height());
}

void KviIOGraphWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs("I/O Traffic Graph");

	m_szHtmlActiveCaption = "<nobr><font color=\"";
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name();
	m_szHtmlActiveCaption += "\"><b>";
	m_szHtmlActiveCaption += m_szPlainTextCaption;
	m_szHtmlActiveCaption += "</b></font></nobr>";

	m_szHtmlInactiveCaption = "<nobr><font color=\"";
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name();
	m_szHtmlInactiveCaption += "\"><b>";
	m_szHtmlInactiveCaption += m_szPlainTextCaption;
	m_szHtmlInactiveCaption += "</b></font></nobr>";
}

void KviIOGraphWindow::die()
{
	close();
}



KviIOGraphWidget::KviIOGraphWidget(QWidget * par)
: QWidget(par)
{
	for(int i=0;i < KVI_IOGRAPH_NUMBER_POINTS;i++)
	{
		m_sendRates[i] = 0;
		m_recvRates[i] = 0;
	}
	m_iNextPoint = 1;
	m_uLastSentBytes = g_uOutgoingTraffic;
	m_uLastRecvBytes = g_uIncomingTraffic;

	QString tip(__tr("Outgoing traffic"));
	tip.append("\n");
	tip.append(__tr("Incoming traffic"));

	this->setToolTip(tip);

	startTimer(1000);
}

void KviIOGraphWidget::timerEvent(QTimerEvent *e)
{
	kvi_u64_t sB = g_uOutgoingTraffic;
	kvi_u64_t rB = g_uIncomingTraffic;

	int sDiff = (sB - m_uLastSentBytes) / 8;
	int rDiff = (rB - m_uLastRecvBytes) / 32;

	if(sDiff < 0)
		sDiff = 0;
	else if(sDiff > 30)
		sDiff = 30;
	if(rDiff < 0)
		rDiff = 0;
	else if(rDiff > 30)
		rDiff = 30;
	m_uLastSentBytes = sB;
	m_uLastRecvBytes = rB;
	m_sendRates[m_iNextPoint] = sDiff;
	m_recvRates[m_iNextPoint] = rDiff;
	m_iNextPoint++;
	if(m_iNextPoint >= KVI_IOGRAPH_NUMBER_POINTS)
		m_iNextPoint = 0;
	update();
}

void KviIOGraphWidget::paintEvent(QPaintEvent * e)
{
	QPainter p(this);

	p.fillRect(e->rect(), QColor("#000000"));

	p.setRenderHint(QPainter::Antialiasing);
	p.setPen(QColor("#c0c0c0"));

	int c=0;
	int sw = e->rect().width() / KVI_IOGRAPH_HORIZ_SEGMENTS;
	int sh = e->rect().height() / KVI_IOGRAPH_VERT_SEGMENTS;

	for(int i=0;i<KVI_IOGRAPH_HORIZ_SEGMENTS;i++)
	{
		p.drawLine(0, c, e->rect().width(), c);
		c+=sw;
	}

	c=0;
	for(int i=0;i<KVI_IOGRAPH_VERT_SEGMENTS;i++)
	{
		p.drawLine(c, 0, c, e->rect().height());
		c+=sh;
	}
// 	p.setPen(QColor("#c0c0c0"));

	// the first point to draw is "m_iNextPoint"

	int leftPart = KVI_IOGRAPH_NUMBER_POINTS - m_iNextPoint;

	int i;

	p.setPen(QColor("#FF0000"));
	for(i = 1;i < leftPart;i++)
	{
		p.drawLine(i * sw,33 - m_sendRates[m_iNextPoint + i - 1],i * sw,33 - m_sendRates[m_iNextPoint + i]);
	}

	p.drawLine(i * sw,33 - m_sendRates[m_iNextPoint + i - 1],i * sw,33 - m_sendRates[0]);

	for(i = 1;i < m_iNextPoint;i++)
	{
		p.drawLine(leftPart + i * sw,33 - m_sendRates[i - 1],leftPart + i * sw,33 - m_sendRates[i]);
	}

	p.setPen(QColor("#0000FF"));

	for(i = 1;i < leftPart;i++)
	{
		p.drawLine(i * sw,33 - m_recvRates[m_iNextPoint + i - 1],i * sw,33 - m_recvRates[m_iNextPoint + i]);
	}

	p.drawLine(i * sw,33 - m_recvRates[m_iNextPoint + i - 1],i * sw,33 - m_recvRates[0]);

	for(i = 1;i < m_iNextPoint;i++)
	{
		p.drawLine(leftPart + i * sw,33 - m_recvRates[i - 1],leftPart + i * sw,33 - m_recvRates[i]);
	}
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

		g_pIOGraphWindow = new KviIOGraphWindow(s->pDescriptor,g_pFrame,"IOGraph Window");
		g_pFrame->addWindow(g_pIOGraphWindow,!bCreateMinimized);

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
							__tr2qs_ctx("Show I/O &Traffic graph","iograph"),
							iograph_extension_alloc);

	if(d)d->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SAYICON)));

	return true;
}

static bool iograph_module_cleanup(KviModule *m)
{
	if(g_pIOGraphWindow)
	{
		delete g_pIOGraphWindow;
		g_pIOGraphWindow = 0;
	}
	return true;
}

static bool iograph_module_can_unload(KviModule *m)
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
	iograph_module_cleanup
)

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "libkviiograph.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
