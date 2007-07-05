//=============================================================================
//
//   File : kvi_topicw.cpp
//   Creation date : Fri Aug 4 2000 12:09:21 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2005 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVIRC__

#include "kvi_topicw.h"
#include "kvi_options.h"
#include "kvi_mirccntrl.h"
#include "kvi_locale.h"
#include "kvi_defaults.h"
#include "kvi_settings.h"
#include "kvi_iconmanager.h"
#include "kvi_texticonmanager.h"
#include "kvi_app.h"
#include "kvi_colorwin.h"
#include "kvi_texticonwin.h"
#include "kvi_window.h"
#include "kvi_channel.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_mirccntrl.h"

extern KviTextIconWindow * g_pTextIconWindow;
extern KviColorWindow * g_pColorWindow;
static int g_iInputFontCharWidth[256];

#include <qpainter.h>
#include <qfontmetrics.h>
#include "kvi_tal_tooltip.h"
#include <qlineedit.h>
#include "kvi_tal_listbox.h"
#include <qregexp.h>
#include <qclipboard.h>
#include "kvi_tal_popupmenu.h"
#include <qevent.h>

// FIXME: #warning "The combo should disappear when it looses focus!...(how to do it ?)"

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;

#endif


extern QStringList  * g_pRecentTopicList;

int KviListBoxTopicItem::width ( const KviTalListBox * lb ) const
{
	QFontMetrics fm(lb->font());
	return fm.width(KviMircCntrl::stripControlBytes(text()));
}

void KviListBoxTopicItem::paint ( QPainter * p )
{
	KviTopicWidget::paintColoredText(p,text(),listBox()->colorGroup(),height(listBox()));
}


KviTopicWidget::KviTopicWidget(QWidget * par,const char * name)
: QFrame(par,name)
{
	setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
	setFont(KVI_OPTION_FONT(KviOption_fontLabel));
	m_pHistory = 0;
	m_pAccept = 0;
	m_pDiscard = 0;
	m_pContextPopup = 0;
	m_iCursorPosition = 0;
	m_pInput = 0;
#ifdef COMPILE_USE_QT4
	setAutoFillBackground(false);
#else
	setBackgroundMode(QWidget::NoBackground);
#endif
	reset();
#ifdef COMPILE_USE_QT4
	m_pCompletionBox=new KviTalListBox(this,Qt::Popup);
#else
	m_pCompletionBox=new KviTalListBox(this,Qt::WType_Popup);
#endif
	m_pCompletionBox->setFont( font() );
	m_pCompletionBox->setPalette( palette() );
//	m_pCompletionBox->setVScrollBarMode( KviTalListBox::AlwaysOff );
//	m_pCompletionBox->setHScrollBarMode( KviTalListBox::AlwaysOff );
	m_pCompletionBox->setFrameStyle( QFrame::Box | QFrame::Plain );
	m_pCompletionBox->setLineWidth( 1 );
	connect(m_pCompletionBox,SIGNAL(selected(int)),this,SLOT(complete(int)));
	m_pCompletionBox->hide();
}

KviTopicWidget::~KviTopicWidget()
{
	delete m_pCompletionBox;
}

void KviTopicWidget::popDownListBox()
{
	m_pCompletionBox->removeEventFilter( this );
	m_pCompletionBox->hide();
}

void KviTopicWidget::reset()
{
	KviTalToolTip::remove(this);
	m_szTopic = __tr2qs("Unknown");
	KviTalToolTip::add(this,__tr2qs("No topic message has been received from the server yet"));
	m_szSetAt = "";
	m_szSetBy = "";
	update();
}

void KviTopicWidget::applyOptions()
{
//	setFont(KVI_OPTION_FONT(KviOption_fontLabel));
//	if(m_pComboBox)m_pComboBox->setFont(KVI_OPTION_FONT(KviOption_fontLabel));
	resizeEvent(0);
}

#define KVI_LABEL_DEF_BACK 100
#define KVI_LABEL_DEF_FORE 101

QString convertToHtml(const QString &text)
{
	QString result;
	bool curBold      = false;
	bool curUnderline = false;
	unsigned char curFore      = KVI_LABEL_DEF_FORE; //default fore
	unsigned char curBack      = KVI_LABEL_DEF_BACK; //default back

	unsigned int idx = 0;

	while(idx < text.length())
	{
		unsigned short c = text[(int)idx].unicode();

		unsigned int start = idx;

		while(
				(c != KVI_TEXT_COLOR) &&
				(c != KVI_TEXT_BOLD) &&
				(c != KVI_TEXT_UNDERLINE) &&
				(c != KVI_TEXT_REVERSE) &&
				(c != KVI_TEXT_RESET) &&
				(c != KVI_TEXT_ICON)
			)
		{
			idx++;
			if(idx >= text.length())break;
			else c = text[(int)idx].unicode();
		}
		
		int len = idx - start;

		if(len > 0)
		{
			bool bOpened = FALSE;
			QString szText = text.mid(start,len);
			
			if(curBold) result.append("<b>");
			if(curUnderline) result.append("<u>");
			
			if(curFore != KVI_LABEL_DEF_FORE)
			{
				result.append("<font color=\"");
				result.append(KVI_OPTION_MIRCCOLOR(curFore).name());
				result.append('"');
				bOpened = TRUE;
			}
		
/*			if(curBack != KVI_LABEL_DEF_BACK)
			{
				if(!bOpened) 
					result.append("<font bgcolor=");
				else
					result.append(" bgcolor=");
				result.append(KVI_OPTION_MIRCCOLOR(curBack).name());
			}*/

			if(bOpened) result.append(">");

			result.append(text.mid(start,len));

			if( (curFore != KVI_LABEL_DEF_FORE) /*|| (curBack != KVI_LABEL_DEF_BACK)*/ )
				result.append("</font>");

			if(curUnderline) result.append("</u>");
			if(curBold) result.append("</b>");

		}

		switch(c)
		{
			case KVI_TEXT_BOLD: curBold = !curBold; ++idx; break;
			case KVI_TEXT_UNDERLINE: curUnderline = !curUnderline; ++idx; break;
			case KVI_TEXT_REVERSE:
				{
					char auxBack = curBack;
					curBack = curFore;
					curFore = auxBack;
				}
				++idx;
			break;
			case KVI_TEXT_RESET:
				curFore = KVI_LABEL_DEF_FORE;
				curBack = KVI_LABEL_DEF_BACK;
				curBold = false;
				curUnderline = false;
				++idx;
			break;
			case KVI_TEXT_COLOR:
			{
				++idx;
				unsigned char fore;
				unsigned char back;
				idx = getUnicodeColorBytes(text,idx,&fore,&back);
				if(fore != KVI_NOCHANGE)
				{
					curFore = fore;
					if(back != KVI_NOCHANGE)curBack = back;
				} else {
					// only a CTRL+K
					curBack = KVI_LABEL_DEF_BACK;
					curFore = KVI_LABEL_DEF_FORE;
				}
			}
			break;
			case KVI_TEXT_ICON:
			{
				++idx;

				unsigned int icoStart = idx;
				while((idx < text.length()) && (text[(int)idx].unicode() > 32))idx++;

				KviStr lookupString = text.mid(icoStart,idx - icoStart);

				KviTextIcon * icon = g_pTextIconManager->lookupTextIcon(lookupString.ptr());
				if(icon)
				{
					//TODO: icons
/*					QPixmap * pigzmap = icon->pixmap();
					p->drawPixmap(curX,(baseline + 2) - pigzmap->height(),*(pigzmap));
					curX += pigzmap->width();*/
				} else {
					idx = icoStart;
				}
			}
			break;
		}
	}
	return result;
}

void KviTopicWidget::paintColoredText(QPainter *p, QString text,const QColorGroup& cg,int height, int width)
{
	QFontMetrics fm(p->font());
	
	if(height<0) height=p->window().height();
	if(width<0) width=p->window().width();
	
	bool curBold      = false;
	bool curUnderline = false;
	unsigned char curFore      = KVI_LABEL_DEF_FORE; //default fore
	unsigned char curBack      = KVI_LABEL_DEF_BACK; //default back
	int baseline = ((height + fm.ascent() - fm.descent() + 1) >> 1);

	int curX = p->window().x() + 2; //2 is the margin

	unsigned int idx = 0;

	while((idx < text.length()) && (curX < width))
	{
		unsigned short c = text[(int)idx].unicode();

		unsigned int start = idx;

		while((idx < text.length()) &&
				(c != KVI_TEXT_COLOR) &&
				(c != KVI_TEXT_BOLD) &&
				(c != KVI_TEXT_UNDERLINE) &&
				(c != KVI_TEXT_REVERSE) &&
				(c != KVI_TEXT_RESET) &&
				(c != KVI_TEXT_ICON)
			)
		{
			idx++;
			c = text[(int)idx].unicode();
		}

		int len = idx - start;
		int wdth;

		if(len > 0)
		{
			QString szText = text.mid(start,len);

			wdth = fm.width(szText);
	
			if(curFore == KVI_LABEL_DEF_FORE)
			{
				p->setPen(cg.text());
			} else {
				if(curFore > 16)p->setPen(cg.background());
				else p->setPen(KVI_OPTION_MIRCCOLOR(curFore));
			}
		
			if(curBack != KVI_LABEL_DEF_BACK)
			{
				if(curBack > 16)
				{
					p->fillRect(curX,p->window().y() + 2,wdth,height - 4,
						cg.text());
				} else {
					p->fillRect(curX,p->window().y() + 2,wdth,height - 4,
						KVI_OPTION_MIRCCOLOR(curBack));
				}
			}

			p->drawText(curX,baseline,szText,0,len);
	
			if(curBold)p->drawText(curX+1,baseline,szText,0,len);
			if(curUnderline)
			{
				p->drawLine(curX,baseline + 1,curX+wdth,baseline + 1);
			}
		} else {
			wdth = 0;
		}


		curX += wdth;

		switch(c)
		{
			case KVI_TEXT_BOLD: curBold = !curBold; ++idx; break;
			case KVI_TEXT_UNDERLINE: curUnderline = !curUnderline; ++idx; break;
			case KVI_TEXT_REVERSE:
				{
					char auxBack = curBack;
					curBack = curFore;
					curFore = auxBack;
				}
				++idx;
			break;
			case KVI_TEXT_RESET:
				curFore = KVI_LABEL_DEF_FORE;
				curBack = KVI_LABEL_DEF_BACK;
				curBold = false;
				curUnderline = false;
				++idx;
			break;
			case KVI_TEXT_COLOR:
			{
				++idx;
				unsigned char fore;
				unsigned char back;
				idx = getUnicodeColorBytes(text,idx,&fore,&back);
				if(fore != KVI_NOCHANGE)
				{
					curFore = fore;
					if(back != KVI_NOCHANGE)curBack = back;
				} else {
					// only a CTRL+K
					curBack = KVI_LABEL_DEF_BACK;
					curFore = KVI_LABEL_DEF_FORE;
				}
			}
			break;
			case KVI_TEXT_ICON:
			{
				++idx;

				unsigned int icoStart = idx;
				while((idx < text.length()) && (text[(int)idx].unicode() > 32))idx++;

				KviStr lookupString = text.mid(icoStart,idx - icoStart);

				KviTextIcon * icon = g_pTextIconManager->lookupTextIcon(lookupString.ptr());
				if(icon)
				{
					QPixmap * pigzmap = icon->pixmap();
					p->drawPixmap(curX,(baseline + 2) - pigzmap->height(),*(pigzmap));
					curX += pigzmap->width();
				} else {
					idx = icoStart;
				}
			}
			break;
		}
	}
}

#ifdef COMPILE_USE_QT4
void KviTopicWidget::paintEvent(QPaintEvent * e)
{
	QPainter pa(this);
	drawFrame(&pa);
	drawContents(&pa);
}
#endif

void KviTopicWidget::drawContents(QPainter *p)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = mapToGlobal(contentsRect().topLeft());
		p->drawTiledPixmap(contentsRect(),*g_pShadedChildGlobalDesktopBackground,pnt);
	} else {
#endif
		if(KVI_OPTION_PIXMAP(KviOption_pixmapLabelBackground).pixmap())
		{
			p->drawTiledPixmap(contentsRect(),*(KVI_OPTION_PIXMAP(KviOption_pixmapLabelBackground).pixmap()));
		} else {
			p->fillRect(contentsRect(),KVI_OPTION_COLOR(KviOption_colorLabelBackground));
		}
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif
	QColorGroup colorGroup;
	//colorGroup()
	colorGroup.setColor(QColorGroup::Text,KVI_OPTION_COLOR(KviOption_colorLabelForeground));
	colorGroup.setColor(QColorGroup::Background,KVI_OPTION_COLOR(KviOption_colorLabelBackground));
	paintColoredText(p,m_szTopic,colorGroup);
}

void KviTopicWidget::setTopic(const QString & topic)
{
	m_szTopic = topic;
	bool bFound = false;
	for(QStringList::Iterator it=g_pRecentTopicList->begin();it != g_pRecentTopicList->end(); ++it)
	{
		if(*it == m_szTopic)
		{
			bFound = true;
			break;
		}
	}
	if(!bFound && (!m_szTopic.isEmpty()))
	{
		if(g_pRecentTopicList->count() >= KVI_RECENT_TOPIC_ENTRIES)
		{
			g_pRecentTopicList->remove(g_pRecentTopicList->begin());
		}
		g_pRecentTopicList->append(m_szTopic);
	}
	updateToolTip();
	update();
}

void KviTopicWidget::setTopicSetBy(const QString & setBy)
{
	m_szSetBy = setBy;
	updateToolTip();
}

void KviTopicWidget::setTopicSetAt(const QString & setAt)
{
	m_szSetAt = setAt;
	updateToolTip();
}

void KviTopicWidget::updateToolTip()
{
	KviTalToolTip::remove(this);

	QString txt = "<html>" \
			"<body>" \
				"<table width=\"100%\">";

	if(!m_szTopic.isEmpty())
	{
		txt +=          START_TABLE_BOLD_ROW;
		txt += __tr2qs("Channel topic:");
		txt +=              END_TABLE_BOLD_ROW;

		txt +=          "<tr><td><center>";

		QString tmp = m_szTopic;
		
		tmp.replace('&',"&amp;");
		tmp.replace('<',"&lt;");
		tmp.replace('>',"&gt;");
		tmp = convertToHtml(tmp);
		
		txt += tmp;
		txt +=          "</center></td></tr>";
		
		if(!m_szSetBy.isEmpty())
		{
			txt +=      "<tr><td bgcolor=\"#D0D0D0\">";
			txt +=       __tr2qs("Set by") + " <b>" + m_szSetBy + "</b>";
			txt +=      "</td></tr>";

			if(!m_szSetAt.isEmpty())
			{
				txt +=      "<tr><td bgcolor=\"#D0D0D0\">";
				txt +=       __tr2qs("Set on") + " <b>" + m_szSetAt + "</b>";
				txt +=      "</td></tr>";
			}
		}

		txt +=          "<tr><td>";
		txt += __tr2qs("Double-click to edit...");
		txt +=          "</td></tr>";

	} else {
		txt +=          "<tr><td>";
		txt += __tr2qs("No topic is set");
		txt +=          "</td></tr>";
		txt +=          "<tr><td>";
		txt += __tr2qs("Double-click to set...");
		txt +=          "</td></tr>";
	}

	txt += "</table>" \
		"</body>" \
	"<html>";

	KviTalToolTip::add(this,txt);
}

QSize KviTopicWidget::sizeHint() const
{
	QFontMetrics fm(font());
	int hght = fm.lineSpacing() + (frameWidth() << 1) + 4;
	int baseline = ((hght + fm.ascent() - fm.descent() + 1) >> 1);
	if(baseline < 16)hght += (16 - baseline);
	return QSize(width(),hght);
}

void KviTopicWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	int maxlen=-1;
	QObject * w = parent();
	QString szModes;
	bool bCanEdit = TRUE;
	while(w)
	{
		if(w->inherits("KviChannel"))
		{
			KviChannel *chan = ((KviChannel *)w);
			maxlen=chan->connection()->serverInfo()->maxTopicLen();
			chan->getChannelModeString(szModes);
			if(szModes.contains('t') && !( chan->isMeHalfOp() || chan->isMeOp() || chan->isMeChanOwner() || chan->isMeChanAdmin() || chan->connection()->userInfo()->hasUserMode('o') || chan->connection()->userInfo()->hasUserMode('O')) )  {
				bCanEdit=false;
				}
			break;
		}
	w = w->parent();
	}
	if(m_pInput == 0)
	{
		m_pInput=new KviInputEditor(this,0);
		m_pInput->setReadOnly(!bCanEdit);
		m_pInput->setMaxBufferSize(maxlen);
		m_pInput->setGeometry(0,0,width() - (height() << 2)+height(),height());
		m_pInput->setText(m_szTopic);
		connect(m_pInput,SIGNAL(enterPressed()),this,SLOT(acceptClicked()));
		connect(m_pInput,SIGNAL(escapePressed()),this,SLOT(discardClicked()));
		m_pInput->installEventFilter(this);

		m_pHistory = new QPushButton(this);
		m_pHistory->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)));
		m_pHistory->setGeometry(width() - (height() << 2)+height(),0,height(),height());
		KviTalToolTip::add(m_pHistory,__tr2qs("History"));
		m_pHistory->show();
		connect(m_pHistory,SIGNAL(clicked()),this,SLOT(historyClicked()));
		
		m_pAccept = new QPushButton(this);
		m_pAccept->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
		m_pAccept->setGeometry(width() - (height() << 1),0,height(),height());
		m_pAccept->setEnabled(bCanEdit);
		m_pAccept->show();
		KviTalToolTip::add(m_pAccept,__tr2qs("Commit Changes"));
		connect(m_pAccept,SIGNAL(clicked()),this,SLOT(acceptClicked()));
		
		m_pDiscard = new QPushButton(this);
		m_pDiscard->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
		m_pDiscard->setGeometry(width() - height(),0,height(),height());
		KviTalToolTip::add(m_pDiscard,__tr2qs("Discard Changes"));
		m_pDiscard->show();
		connect(m_pDiscard,SIGNAL(clicked()),this,SLOT(discardClicked()));
		
		m_pInput->show();
		m_pInput->setFocus();
	}
}

void KviTopicWidget::mousePressEvent(QMouseEvent * e)
{

	if(!(e->button() & Qt::RightButton))return;
	if(!m_pContextPopup)
	{
		m_pContextPopup = new KviTalPopupMenu(this);
		connect(m_pContextPopup,SIGNAL(aboutToShow()),this,SLOT(contextPopupAboutToShow()));
	}
	m_pContextPopup->popup(mapToGlobal(e->pos()));
}

void KviTopicWidget::contextPopupAboutToShow()
{
	if(!m_pContextPopup)return; // hm ?
	m_pContextPopup->clear();
	m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_COPY)),__tr2qs("Copy to clipboard"),this,SLOT(copy()));
}

void KviTopicWidget::copy()
{
	QClipboard * c = QApplication::clipboard();
	if(!c)return;
	if(c->supportsSelection())c->setText(m_szTopic,QClipboard::Selection);
	c->setText(m_szTopic,QClipboard::Clipboard);
}

bool KviTopicWidget::eventFilter(QObject *object,QEvent *e)
{
	if ( !e )
		return TRUE;
	if(object==m_pCompletionBox)
	{
		switch( e->type() ) {
		case QEvent::MouseButtonPress:
			if ( m_pCompletionBox->rect().contains( ((QMouseEvent*)e)->pos() ) ) {
				complete(m_pCompletionBox->index(m_pCompletionBox->itemAt(((QMouseEvent*)e)->pos())));
				return TRUE;
			}
			break;
		case QEvent::MouseButtonRelease:
			if ( m_pCompletionBox->rect().contains( ((QMouseEvent*)e)->pos() ) ) {
				QMouseEvent tmp( QEvent::MouseButtonDblClick,
						((QMouseEvent*)e)->pos(), ((QMouseEvent*)e)->button(), ((QMouseEvent*)e)->state() ) ;
				// will hide popup
				QApplication::sendEvent( object, &tmp );
				return TRUE;
			} else {
				if ( m_pCompletionBox->isVisible() )
					popDownListBox();
			}
			break;
		case QEvent::KeyPress:
			switch( ((QKeyEvent *)e)->key() ) {
			case Qt::Key_Up:
			case Qt::Key_Down:
				if ( !(((QKeyEvent *)e)->state() & Qt::AltButton) )
				break;
			case Qt::Key_F4:
			case Qt::Key_Escape:
				if ( m_pCompletionBox->isVisible() ) {
				popDownListBox();
				return TRUE;
				}
				break;
			default:
				break;
			}
			break;
		case QEvent::Hide:
			popDownListBox();
			break;
		default:
			break;
		}
	}
	return QFrame::eventFilter(object,e);
}

bool KviTopicWidget::handleKeyPressEvent(QKeyEvent * e)
{	
	return 1;
}

void KviTopicWidget::keyPressEvent(QKeyEvent * e)
{
	if(handleKeyPressEvent(e))
	{
		e->accept();
		return;
	}
}

void KviTopicWidget::resizeEvent(QResizeEvent *e)
{
	if(e)QFrame::resizeEvent(e);
	if(m_pInput)
	{
		m_pInput->setGeometry(0,0,width() - (height() << 2)+height(),height());
		m_pHistory->setGeometry(width() - (height() << 2)+height(),0,height(),height());
		m_pAccept->setGeometry(width() - (height() << 1),0,height(),height());
		m_pDiscard->setGeometry(width() - height(),0,height(),height());
	}
}

void KviTopicWidget::deactivate()
{
	popDownListBox();
	if(m_pInput)
	{
		delete m_pInput;
		m_pInput = 0;
		delete m_pHistory;
		m_pHistory = 0;
		delete m_pAccept;
		m_pAccept = 0;
		delete m_pDiscard;
		m_pDiscard = 0;
	}
	
	// try to find a KviWindow parent and give it the focus
	
	QObject * w = parent();
	while(w)
	{
		if(w->inherits("KviWindow"))
		{
			((KviWindow *)w)->setFocus();
			return;
		}
		w = w->parent();
	}

	// no KviWindow on the path
	w = parent();
	if(w)
	{
		if(w->inherits("QWidget"))
			((QWidget *)w)->setFocus();
	}
}

void KviTopicWidget::discardClicked()
{
	deactivate();
}

void KviTopicWidget::historyClicked()
{
	if(g_pRecentTopicList)
	{
		m_pCompletionBox->installEventFilter( this );
		m_pCompletionBox->clear();
		for ( QStringList::Iterator it = g_pRecentTopicList->begin(); it != g_pRecentTopicList->end(); ++it ) {
			KviListBoxTopicItem* item=new KviListBoxTopicItem(m_pCompletionBox,*it);
		}
		m_pCompletionBox->resize(m_pInput->width(),6*m_pCompletionBox->fontMetrics().height()+20);
		QPoint point=m_pInput->mapToGlobal(QPoint(0,0));
		point+=QPoint(0,m_pInput->height());
		m_pCompletionBox->move(point);
		m_pCompletionBox->show();
	}
}

void KviTopicWidget::acceptClicked()
{
	if(!m_pInput->readOnly())
	{
		QString tmp = m_pInput->text();
		if(tmp != m_szTopic)emit topicSelected(tmp);
	}
	deactivate();
}

void KviTopicWidget::insertChar(QChar c)
{
	insertText(QString(c));
}

void KviTopicWidget::insertText(const QString &c)
{
	if(m_pInput)
		m_pInput->insertText(c);
}

int KviTopicWidget::xCursorPostionCalculation(int xInd)
{
	return 0;
}
void KviTopicWidget::complete(int pos)
{
	m_pInput->setText(m_pCompletionBox->text(pos));
	popDownListBox();
}

QChar KviTopicWidget::getSubstituteChar(unsigned short control_code)
{
	switch(control_code)
	{
		case KVI_TEXT_COLOR:
			return QChar('K');
			break;
		case KVI_TEXT_BOLD:
			return QChar('B');
			break;
		case KVI_TEXT_RESET:
			return QChar('O');
			break;
		case KVI_TEXT_REVERSE:
			return QChar('R');
			break;
		case KVI_TEXT_UNDERLINE:
			return QChar('U');
			break;
		case KVI_TEXT_ICON:
			return QChar('I');
			break;
		default:
			return QChar(control_code);
			break;
	}
}

#include "kvi_topicw.moc"
