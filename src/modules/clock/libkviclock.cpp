//
//   File : kvi_clock.cpp
//   Creation date : Mon Aug 13 2001 16:34:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include "libkviclock.h"

#include "kvi_module.h"
#include "kvi_console.h"
#include "kvi_options.h"
#include "kvi_frame.h"
#include "kvi_locale.h"
#include "kvi_pointerlist.h"

#include <QBitmap>
#include <QToolTip>
#include <QPainter>

//#include <X11/Xlib.h>
#include <time.h>

#if 0


#warning "Change this XPM to bitmap bits! (And thus use the QBitmap(w,h,bits) constructor)"
#warning "Support GMT , LOCAL , DATE , ALARM..."

/* XPM */
static const char * kvi_clock_digits_xpm[] = {
"234 25 3 1",
" 	c None",
".	c #000000",
"+	c #FFFFFF",
"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++",
"+................+++++++++++.......++................++................+...+++++++.......++................++................++................++................++................+++++++++++++++++++++++++++++++++++++++++++++...+++++++",
"+................+++++++++++.......++...+++++++......++...+++++++......+...+++++++.......++....++++++++++++++....++++++......++...+++++++......++....++++++......++...+++++++......++++++++++++++++++++++++++++++++++++++++++++.....++++++",
"+....++++++......+++++++++++.......++...+++++++......++...+++++++......+...+++++++.......++....++++++++++++++....++++++......++...+++++++......++....++++++......++...+++++++......++++++++++++++++++++++++++++++++++++++++++++.....++++++",
"+....++++++......+++++++++++.......++...+++++++......++...+++++++......+...+++++++.......++....++++++++++++++....++++++......++...+++++++......++....++++++......++...+++++++......++++...+++++++++++++++++++++++++++++++++++++.....++++++",
"+....++++++......+++++++++++.......++...+++++++......++...+++++++......+...+++++++.......++....++++++++++++++....++++++......++...+++++++......++....++++++......++...+++++++......++++...++++++++++++++++++++++++++++++++++++.......+++++",
"+....++++++......+++++++++++.......++++++++++++......++...+++++++......+...+++++++.......++....++++++++++++++....++++++++++++++...+++++++......++....++++++......++...+++++++......++++...++++..++++++++++++++++++++++++++++++++++++++++++",
"+....++++++......+++++++++++.......++++++++++++......++++++++++++......+...+++++++.......++....++++++++++++++....++++++++++++++++++++++++......++....++++++......++...+++++++......+++++++++++..+++++++++++++++++++++++++++++++++.++++++++",
"+....++++++......+++++++++++.......++................+++...............+.................++................++................++++++++++++......++................++...+++++++......+++++++++++..++++++++++++++++++++++++++++++++++++++++++",
"+....++++++......+++++++++++.......++................+++...............+.................++................++................++++++++++++......++................++................+++++++++++..++++++++++++++++++++++++++++++++++++++++++",
"+....++++++......+++++++++++.......++...+++++++++++++++++++++++++......+++++++++++.......++++++++++++......++....++++++......++++++++++++......++....++++++......++++++++++++......+++++++++++..++++++++++++++++++++++++++++++++++++++++++",
"+....++++++......+++++++++++.......++...+++++++++++++++++++++++++......+++++++++++.......++++++++++++......++....++++++......++++++++++++......++....++++++......++++++++++++......++++...++++.......++.......++.......+++++.+++++++++.+++",
"+....++++++......+++++++++++.......++...+++++++++++++++...+++++++......+++++++++++.......++....++++++......++....++++++......++++++++++++......++....++++++......++++++++++++......++++...++++..+++..++..+.+..++..+++++++++.++.++.++.++.++",
"+....++++++......+++++++++++.......++...+++++++++++++++...+++++++......+++++++++++.......++....++++++......++....++++++......++++++++++++......++....++++++......++++++++++++......++++...++++..+++..++..+.+..++.......++++.+.++...++.+.++",
"+....++++++......+++++++++++.......++...+++++++++++++++...+++++++......+++++++++++.......++....++++++......++....++++++......++++++++++++......++....++++++......++++++++++++......+++++++++++..+++..++..+.+..+++++++..++++.+.++...++.+.++",
"+................+++++++++++.......++................++................+++++++++++.......++................++................++++++++++++......++................++++++++++++......+++++++++++..+++..++..+.+..++..+++..++++.++.++.++.++.++",
"+................+++++++++++.......++................++................+++++++++++.......++................++................++++++++++++......++................++++++++++++......+++++++++++..+++..++..+.+..++.......+++++.+++++++++.+++",
"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++",
"+................++................++................++................++................+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++",
"+..+...+++.++++..++..+.+.+++.++++..++..+++.+++.+.....++..+++.+++.+.....++..+++.+++.+.....+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++",
"+..+...+.+.+.....++..+.+..+..+.....++..+...+.+.+.....++..+...+.+.+.....++..+...+.+.+.....+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++",
"+..+...+.+.+.....++..+.+..+..+.....++..+...+++.+.....++..+...+++.+.....++..+...+++.+.....+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++",
"+..+...+.+.+.....++..+.+..+..+.....++..+...+.+.+.....++..+...+.+.+.....++..+...+.+.+.....+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++",
"+..+++.+++.++++..++..+++..+..++++..++..+++.+.+.++++..++..+++.+.+.++++..++..+++.+.+.++++..+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++",
"+................++................++................++................++................+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"};


// kvi_app.cpp
//extern Display                   * g_display;
//extern GC                          g_iccGC;
extern QPixmap                 * g_pIccMemBuffer;
//extern HANDLE                      g_hIccMemBuffer;


static KviPointerList<KviClockDisplay> * g_pClockWidgetList;

static QBitmap * g_pClockBitmap = 0;


KviClockDisplay::KviClockDisplay(KviIrcToolBarAppletContainer * pContainer)
: KviIrcToolBarGraphicalApplet(pContainer,"clock_display")
{
	g_pClockWidgetList->append(this);

	for(int i=0;i<6;i++)
	{
		m_uDigit[i] = 0;
	}

	startTimer(1000);
}

KviClockDisplay::~KviClockDisplay()
{
	g_pClockWidgetList->removeRef(this);
}

void KviClockDisplay::timerEvent(QTimerEvent *e)
{
	time_t t = time(0);
	struct tm * pTm = gmtime(&t);

	m_uDigit[0] = pTm->tm_hour / 10;
	m_uDigit[1] = pTm->tm_hour % 10;
	m_uDigit[2] = pTm->tm_min / 10;
	m_uDigit[3] = pTm->tm_min % 10;
	m_uDigit[4] = pTm->tm_sec / 10;
	m_uDigit[5] = pTm->tm_sec % 10;

	update();
}



void KviClockDisplay::drawContents(QPainter *p)
{
	//XSetLineAttributes(g_display,g_iccGC,1,LineSolid,CapButt,JoinMiter); //need this for line size


	//XSetClipMask(g_display,g_iccGC,g_pClockBitmap->handle());

	#define LOW_ROW_Y 6



#define DRAW_DIGIT(_x,_num,_br) \
	p->setBrushOrigin(- (m_uDigit[_num] * 18) + _x,LOW_ROW_Y); \
	p->fillRect(_x,LOW_ROW_Y,18,18,_br);

#define DRAW_DOTS(_x,_br) \
	p->setBrushOrigin(- 180 + _x,LOW_ROW_Y); \
	p->fillRect(_x,LOW_ROW_Y,9,18,_br);

	QBrush br1(KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletForegroundLowContrast),*g_pClockBitmap);

	p->setBrushOrigin(- 216,LOW_ROW_Y);
	p->fillRect(0,LOW_ROW_Y,18,18,br1);

	p->setBrushOrigin(22,LOW_ROW_Y + 2 /*-18 + LOW_ROW_Y + 20 */);
	p->fillRect(22,LOW_ROW_Y + 20,18 * 5,7,br1);

	QBrush br2(KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletForegroundHighContrastActive1),*g_pClockBitmap);

	DRAW_DIGIT(22,0,br2)
	DRAW_DIGIT(40,1,br2)

	DRAW_DOTS(58,br2)

	DRAW_DIGIT(67,2,br2)
	DRAW_DIGIT(85,3,br2)

	DRAW_DOTS(103,br2)

	DRAW_DIGIT(112,4,br2)
	DRAW_DIGIT(130,5,br2)

}

QSize KviClockDisplay::sizeHint() const
{
	return QSize(KVI_IRCTOOLBARAPPLET_MAXIMUM_WIDTH,KVI_IRCTOOLBARAPPLET_MAXIMUM_HEIGHT);
}


/*
	@doc: clock.add
	@type:
		command
	@title:
		clock.add
	@short:
		Adds a Clock applet
	@syntax:
		clock.add
	@description:
*/



static bool clock_module_cmd_add(KviModule *m,KviCommand *c)
{
	ENTER_STACK_FRAME(c,"clock_module_cmd_add");

	KviStr dummy;
	if(!g_pUserParser->parseCmdFinalPart(c,dummy))return false;

	KviClockDisplay * dpy = new KviClockDisplay(c->window()->frame()->frameAppletContainer());
	c->window()->frame()->frameAppletContainer()->addApplet(dpy);
	dpy->show();

	return c->leaveStackFrame();
}
#endif


static bool clock_module_init(KviModule *m)
{
#if 0
	g_pClockBitmap = new QBitmap();
	QPixmap pix(kvi_clock_digits_xpm);
	*g_pClockBitmap = pix;
	g_pClockWidgetList = new KviPointerList<KviClockDisplay>;
	g_pClockWidgetList->setAutoDelete(false);
	m->registerCommand("add",clock_module_cmd_add);
#endif
	return true;
}

static bool clock_module_cleanup(KviModule *m)
{
#if 0
	while(g_pClockWidgetList->first())
	{
		KviClockDisplay * dpy = g_pClockWidgetList->first();
		dpy->container()->removeApplet(dpy); // deleted path
	}
	delete g_pClockWidgetList;
	delete g_pClockBitmap;
#endif
	return true;
}

static bool clock_module_can_unload(KviModule *m)
{
#if 0
	return g_pClockWidgetList->isEmpty();
#else
	return true;
#endif
}

KVIRC_MODULE(
	"Clock",                                              // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Clock toolbar applet",
	clock_module_init,
	clock_module_can_unload,
	0,
	clock_module_cleanup
)
#if 0
#include "libkviclock.moc"
#endif
