//=============================================================================
//
//   File : KviIrcToolBar.cpp
//   Creation date : Thu Oct 29 2000 14:13:13 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
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
#include "KviApplication.h"
#include "KviDynamicToolTip.h"
#include "KviIrcUrl.h"
#include "KviInternalCommand.h"
#include "KviPointerList.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcContext.h"
#include "KviLagMeter.h"
#include "KviWindowStack.h"
#include "KviPixmapUtils.h"

#include <QStyle>
#include <QPainter>
#include <QLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QMenu>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

static KviPointerList<KviToolBarGraphicalApplet> * g_pToolBarGraphicalAppletList = nullptr;

KviToolBarGraphicalApplet::KviToolBarGraphicalApplet(QWidget * par, const char * name)
    : QWidget(par)
{
	setObjectName(name);
	if(!g_pToolBarGraphicalAppletList)
	{
		g_pToolBarGraphicalAppletList = new KviPointerList<KviToolBarGraphicalApplet>();
		g_pToolBarGraphicalAppletList->setAutoDelete(false);
	}

	g_pToolBarGraphicalAppletList->append(this);

	setAutoFillBackground(false);
	setMouseTracking(true);
	m_bResizeMode = false;

	setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

	setMinimumWidth(32);
	setMaximumWidth(580);

	if(KVI_OPTION_UINT(KviOption_uintIrcContextAppletWidth) < 32)
		KVI_OPTION_UINT(KviOption_uintIrcContextAppletWidth) = 32;

	resize(KVI_OPTION_UINT(KviOption_uintIrcContextAppletWidth),
	    KVI_OPTION_UINT(KviOption_uintToolBarIconSize));
}

KviToolBarGraphicalApplet::~KviToolBarGraphicalApplet()
{
	KVI_OPTION_UINT(KviOption_uintIrcContextAppletWidth) = width();

	g_pToolBarGraphicalAppletList->removeRef(this);
	if(g_pToolBarGraphicalAppletList->isEmpty())
	{
		delete g_pToolBarGraphicalAppletList;
		g_pToolBarGraphicalAppletList = nullptr;
	}
}

void KviToolBarGraphicalApplet::mouseMoveEvent(QMouseEvent * e)
{
	if(e->buttons() & Qt::LeftButton)
	{
		if(m_bResizeMode)
		{
			int w = e->pos().x();
			resize(w, height());
			updateGeometry();
		}
	}
	else
	{
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

void KviToolBarGraphicalApplet::paintEvent(QPaintEvent *)
{
	if(!isVisible())
		return;

	QPainter pa(this);

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		pa.save();
		pa.setCompositionMode(QPainter::CompositionMode_Source);
		QColor col = KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		pa.fillRect(rect(), col);
		pa.restore();
	}
	else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = mapTo(g_pMainWindow, rect().topLeft());
		pa.drawTiledPixmap(rect(), *(g_pShadedChildGlobalDesktopBackground), pnt);
	}
	else
	{
#endif
		QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapIrcToolBarAppletBackground).pixmap();
		if(pix)
		{
			KviPixmapUtils::drawPixmapWithPainter(&pa, pix, KVI_OPTION_UINT(KviOption_pixmapIrcToolBarAppletBackground), rect(), rect().width(), rect().height());
		}
		else
		{
			pa.fillRect(rect(), KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletBackground));
		}
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	drawContents(&pa);

	//Need to draw the sunken rect around the view now...
	pa.setPen(palette().dark().color());
	pa.drawLine(0, 0, width(), 0);
	pa.drawLine(0, 0, 0, width());
	pa.setPen(palette().light().color());
	pa.drawLine(1, height() - 1, width() - 1, height() - 1);
	pa.drawLine(width() - 1, 1, width() - 1, height());
}

QSize KviToolBarGraphicalApplet::sizeHint() const
{
	return QSize(width(), KVI_OPTION_UINT(KviOption_uintToolBarIconSize));
}

void KviToolBarGraphicalApplet::drawContents(QPainter *)
{
	// nothing here
}

//
// KviIrcContextDisplay
//
//      Main applet of all IRC contexts
//      Displays the server connection status, server name
//      nickname, user mode and the graphical indication of the context
//

KviIrcContextDisplay::KviIrcContextDisplay(QWidget * par, const char * name)
    : KviToolBarGraphicalApplet(par, name)
{
	KviDynamicToolTip * tip = new KviDynamicToolTip(this);
	connect(tip, SIGNAL(tipRequest(KviDynamicToolTip *, const QPoint &)), this, SLOT(tipRequest(KviDynamicToolTip *, const QPoint &)));
}

KviIrcContextDisplay::~KviIrcContextDisplay()
    = default;

void KviIrcContextDisplay::tipRequest(KviDynamicToolTip * tip, const QPoint &)
{
	QString txt;

	KviConsoleWindow * c = g_pActiveWindow->console();

	static QString br("<br>");
	static QString bb("<b>");
	static QString be("</b>");
	static QString cln(":");
	static QString space(" ");
	static QString nbspc("&nbsp;");

	static QString ths = "<html><body><table style=\"white-space: pre\">";
	static QString sbr = "<tr><td style=\"background-color: rgb(48,48,48); white-space: nowrap; font-weight: bold; color: rgb(255,255,255); padding-left: 5px; padding-right: 5px;\">";
	static QString ebr = "</td></font></color></tr>";
	static QString snr = "<tr><td style=\"white-space: pre\">";
	static QString enr = "</td></tr>";
	static QString the = "</table></body></html>";

	txt = ths;

	if(c)
	{
		KviIrcConnection * ic = c->connection();

		QString szNum;
		szNum.setNum(c->context()->id());

		QString szIrcContext;
		szIrcContext += __tr2qs("IRC context");
		szIrcContext += ":" + space + bb;
		szIrcContext += szNum + be;

		if(!ic)
		{
			txt += sbr;
			txt += __tr2qs("Not connected");
			txt += ebr;
		}
		else
		{
			if(!c->isConnected())
			{
				txt += sbr;
				txt += __tr2qs("Connection in progress...");
				txt += ebr;
			}
			else if(ic)
			{
				txt += sbr;
				txt += __tr2qs("Using server") + ":" + space;;
				txt += ic->currentServerName();
				txt += ebr;

				txt += snr;
				KviCString nickAndMode = ic->userInfo()->nickName();
				if(!(ic->userInfo()->userMode().isEmpty()))
				{
					nickAndMode.append(KviCString::Format, ": <b>+%s</b>", ic->userInfo()->userMode().toUtf8().data());

					txt += nbspc;
					txt += __tr2qs("Modes for") + space;
					txt += nickAndMode.ptr();
				}
				else if(ic->userInfo()->userMode().isEmpty())
					txt += nbspc + ic->currentNickName();

				if(ic->userInfo()->isAway())
				{
					txt += br;
					txt += nbspc;
					txt += ic->currentNickName();
					txt += space + bb;
					txt += __tr2qs("is away") + be;
				}
			}

			txt += enr;
		}

		if(ic && ic->lagMeter() && (KVI_OPTION_BOOL(KviOption_boolShowLagOnContextDisplay)))
		{
			txt += snr;

			int lll;
			if((lll = ic->lagMeter()->lag()) > 0)
			{
				int llls = lll / 1000;
				int llld = (lll % 1000) / 100;
				int lllc = (lll % 100) / 10;
				txt += nbspc;
				KviQString::appendFormatted(txt, __tr2qs("Lag: <b>%d.%d%d secs</b>"), llls, llld, lllc);
				txt += enr;
				int vss = ic->lagMeter()->secondsSinceLastCompleted();
				int vmm = vss / 60;
				vss = vss % 60;
				txt += snr;
				KviQString::appendFormatted(txt, nbspc + __tr2qs("Last checked: <b>%d mins %d secs ago</b>"), vmm, vss);
				txt += enr;
			}
			else
				txt += nbspc + __tr2qs("Lag: <b>?.?\?</b>"); //escaped a ? due to compiler trigraphs warning
		}

		txt += R"(<tr><td bgcolor="#E0E0E0"><font color="#000000">)";
		txt += szIrcContext;
		txt += "</font>" + enr;
	}
	else
	{
		txt =  sbr;
		txt += __tr2qs("No IRC context");
		txt += ebr;
	}

	txt += the;
	tip->tip(rect(), txt);
}

#define KVI_APPLETIRCCONTEXTINDICATORWIDTH 12

void KviIrcContextDisplay::drawContents(QPainter * p)
{
	// The context indicator
	KviWindow * wnd = g_pActiveWindow;
	KviConsoleWindow * c = wnd ? wnd->console() : nullptr;

	static QString space(" ");
	static QString sprtr("-");
	static QString cln(":");

	if(c)
	{
		QString serv, nick;
		if(!c->connection())
		{
			serv = __tr2qs("Not connected");
		}
		else
		{
			if(c->isConnected())
			{
				KviIrcConnection * ic = c->connection();
				if(!ic->userInfo()->userMode().isEmpty())
				{
					nick += __tr2qs("Modes for") + space;
					nick += ic->currentNickName() + cln;
					nick += space + "+" + ic->userInfo()->userMode();
				}
				else if(ic->userInfo()->userMode().isEmpty())
					nick += ic->currentNickName();

				if(ic->userInfo()->isAway())
				{
					nick += space + sprtr + space;
					nick += __tr2qs("is away");
				}

				serv =  __tr2qs("Using server");
				serv += cln + space;
				serv += ic->currentServerName();
				if(ic->lagMeter() && (KVI_OPTION_BOOL(KviOption_boolShowLagOnContextDisplay)))
				{
					nick += space + sprtr + space;
					int lll;
					if((lll = ic->lagMeter()->lag()) > 0)
					{
						int llls = lll / 1000;
						int llld = (lll % 1000) / 100;
						int lllc = (lll % 100) / 10;
						KviQString::appendFormatted(nick, __tr2qs("Lag: %d.%d%d secs"), llls, llld, lllc);
					}
					else
					{
						nick += __tr2qs("Lag: ?.??");
					}
				}
			}
			else
			{
				serv = __tr2qs("Connection in progress...");
			}
		}

		p->setPen(KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletFont));

		p->setClipRect(KVI_APPLETIRCCONTEXTINDICATORWIDTH + 2, 2, width() - (KVI_APPLETIRCCONTEXTINDICATORWIDTH + 4), height() - 4);

		if(height() < 30)
		{
			static QString xxx(" [");
			serv += xxx;
			serv += nick;
			serv += QChar(']');
			p->drawText(KVI_APPLETIRCCONTEXTINDICATORWIDTH + 4, 13, serv);
		}
		else
		{
			p->drawText(KVI_APPLETIRCCONTEXTINDICATORWIDTH + 4, 14, serv);
			p->drawText(KVI_APPLETIRCCONTEXTINDICATORWIDTH + 4, 27, nick);
		}

		p->setClipping(false);

		QColor base = palette().window().color();
		QColor cntx = KVI_OPTION_ICCOLOR(c->context()->id() % KVI_NUM_ICCOLOR_OPTIONS);
		base.setRgb((base.red() + cntx.red()) >> 1, (base.green() + cntx.green()) >> 1,
		    (base.blue() + cntx.blue()) >> 1);

		p->fillRect(2, 2, KVI_APPLETIRCCONTEXTINDICATORWIDTH - 2, height() - 4, base);
	}

	p->setPen(KVI_OPTION_COLOR(KviOption_colorIrcToolBarAppletBorder));
	p->drawLine(1, 1, width() - 1, 1);
	p->drawLine(1, 1, 1, height() - 1);
	p->drawLine(2, height() - 2, width() - 1, height() - 2);
	p->drawLine(width() - 2, 1, width() - 2, height());
	p->drawLine(KVI_APPLETIRCCONTEXTINDICATORWIDTH, 2, KVI_APPLETIRCCONTEXTINDICATORWIDTH, height() - 2);
}

#ifdef Bool
#undef Bool
#endif
