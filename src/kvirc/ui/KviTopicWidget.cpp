//=============================================================================
//
//   File : KviTopicWidget.cpp
//   Creation date : Fri Aug 4 2000 12:09:21 by Szymon Stefanek
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



#include "KviTopicWidget.h"
#include "KviOptions.h"
#include "KviControlCodes.h"
#include "KviLocale.h"
#include "kvi_defaults.h"
#include "kvi_settings.h"
#include "KviIconManager.h"
#include "KviTextIconManager.h"
#include "KviApplication.h"
#include "KviColorSelectionWindow.h"
#include "KviTextIconWindow.h"
#include "KviWindow.h"
#include "KviChannelWindow.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviHtmlGenerator.h"
#include "KviTalToolTip.h"
#include "KviThemedLabel.h"

#include <QLineEdit>
#include <QPainter>
#include <QFontMetrics>
#include <QRegExp>
#include <QClipboard>
#include <QEvent>
#include <QMouseEvent>
#include <QTextDocument> // for Qt::escape
#include <QMenu>

extern KviColorWindow * g_pColorWindow;

#define KVI_LABEL_DEF_BACK 100
#define KVI_LABEL_DEF_FORE 101

extern QStringList  * g_pRecentTopicList;

KviTopicListBoxItemDelegate::KviTopicListBoxItemDelegate(QAbstractItemView * pWidget)
: KviTalIconAndRichTextItemDelegate(pWidget)
{
}

KviTopicListBoxItemDelegate::~KviTopicListBoxItemDelegate()
{
}

QSize KviTopicListBoxItemDelegate::sizeHint(const QStyleOptionViewItem &,const QModelIndex &index) const
{
	KviTalListWidget* listWidget = (KviTalListWidget*)parent();
	KviTopicListBoxItem* item = (KviTopicListBoxItem*) listWidget->item(index.row());

	return listWidget->fontMetrics().size(Qt::TextSingleLine, KviControlCodes::stripControlBytes(item->text()));
}

void KviTopicListBoxItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	KviTalListWidget* listWidget = (KviTalListWidget*)parent();
	KviTopicListBoxItem* item = (KviTopicListBoxItem*) listWidget->item(index.row());

	if(item) KviTopicWidget::paintColoredText(p,item->text(),option.palette,option.rect);
}


KviTopicListBoxItem::KviTopicListBoxItem(KviTalListWidget * listbox, const QString & text)
: KviTalListWidgetText(listbox,text)
{
}

KviTopicListBoxItem::~KviTopicListBoxItem()
{
}

int KviTopicListBoxItem::width ( const KviTalListWidget * lb ) const
{
	return lb->fontMetrics().width(KviControlCodes::stripControlBytes(text()));
}


KviTopicWidget::KviTopicWidget(QWidget * par, KviChannelWindow * pChannel, const char * name)
: QWidget(par)
{
	setObjectName(name);
	m_pKviChannelWindow = pChannel;
	m_pHistory = 0;
	m_pAccept = 0;
	m_pDiscard = 0;
	m_pContextPopup = 0;
	m_iCursorPosition = 0;
	m_pInput = 0;
	m_pLabel = 0;
	setAutoFillBackground(false);

	m_pLabel = new KviThemedLabel(this, pChannel, "topic_label");
	m_pLabel->setTextFormat(Qt::RichText);
	connect(m_pLabel,SIGNAL(doubleClicked()),this,SLOT(switchMode()));

	reset();

	m_pCompletionBox=new KviTalListWidget(this,"topic_completion_box",Qt::Popup);
	m_pCompletionBox->setFont( font() );
	m_pCompletionBox->setPalette( palette() );
	m_pCompletionBox->setFrameStyle( QFrame::Box | QFrame::Plain );
	m_pCompletionBox->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pCompletionBox->setSelectionMode(QAbstractItemView::SingleSelection);

	m_pItemDelegate = new KviTopicListBoxItemDelegate(m_pCompletionBox);
	m_pCompletionBox->setItemDelegate(m_pItemDelegate);

	connect(m_pCompletionBox,SIGNAL(itemSelectionChanged()),this,SLOT(complete()));
	m_pCompletionBox->hide();

	setContentsMargins(0,0,0,0);
	setMinimumWidth(80);
	applyOptions();
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
	m_szTopic = "";
	m_pLabel->setText(KviHtmlGenerator::convertToHtml(__tr2qs("Unknown")));
	KviTalToolTip::add(this,__tr2qs("No topic message has been received from the server yet"));
	m_szSetAt = "";
	m_szSetBy = "";
}

void KviTopicWidget::applyOptions()
{
	//set the font
	m_pLabel->applyOptions();
	QFont newFont(KVI_OPTION_FONT(KviOption_fontLabel));
	newFont.setKerning(false);
	newFont.setStyleStrategy(QFont::StyleStrategy(newFont.styleStrategy() | QFont::ForceIntegerMetrics));
	setFont(newFont);
	if(m_pCompletionBox)
		m_pCompletionBox->setFont(newFont);
}

void KviTopicWidget::paintColoredText(QPainter *p, QString text,const QPalette& cg,const QRect & rect)
{
	QFontMetrics fm(p->fontMetrics());
	bool curBold      = false;
	bool curUnderline = false;
	unsigned char curFore      = KVI_LABEL_DEF_FORE; //default fore
	unsigned char curBack      = KVI_LABEL_DEF_BACK; //default back
	int baseline = rect.top() + rect.height() - fm.descent() -1;

	int curX = rect.x() + 1; // is the margin
	int maxX = rect.width() - 2; // 2 is 1*margin
	unsigned int idx = 0;

	while((idx < (unsigned int)text.length()) && (curX < maxX))
	{
		unsigned short c = text[(int)idx].unicode();

		unsigned int start = idx;

		while((idx < (unsigned int)text.length()) &&
				(c != KviControlCodes::Color) &&
				(c != KviControlCodes::Bold) &&
				(c != KviControlCodes::Underline) &&
				(c != KviControlCodes::Reverse) &&
				(c != KviControlCodes::Reset) &&
				(c != KviControlCodes::Icon)
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
				p->setPen(cg.text().color());
			} else {
				if(curFore > 16)p->setPen(cg.background().color());
				else p->setPen(KVI_OPTION_MIRCCOLOR(curFore));
			}

			if(curBack != KVI_LABEL_DEF_BACK)
			{
				if(curBack > 16)
				{
					p->fillRect(curX,rect.y() + 2,wdth,rect.height() - 4,
						cg.text());
				} else {
					p->fillRect(curX,rect.y() + 2,wdth,rect.height() - 4,
						KVI_OPTION_MIRCCOLOR(curBack));
				}
			}

			p->drawText(curX,baseline,szText.left(len));

			if(curBold)p->drawText(curX+1,baseline,szText.left(len));
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
			case KviControlCodes::Bold: curBold = !curBold; ++idx; break;
			case KviControlCodes::Underline: curUnderline = !curUnderline; ++idx; break;
			case KviControlCodes::Reverse:
				{
					char auxBack = curBack;
					curBack = curFore;
					curFore = auxBack;
				}
				++idx;
			break;
			case KviControlCodes::Reset:
				curFore = KVI_LABEL_DEF_FORE;
				curBack = KVI_LABEL_DEF_BACK;
				curBold = false;
				curUnderline = false;
				++idx;
			break;
			case KviControlCodes::Color:
			{
				++idx;
				unsigned char fore;
				unsigned char back;
				idx = KviControlCodes::getUnicodeColorBytes(text,idx,&fore,&back);
				if(fore != KviControlCodes::NoChange)
				{
					curFore = fore;
					if(back != KviControlCodes::NoChange)
						curBack = back;
				} else {
					// only a CTRL+K
					curBack = KVI_LABEL_DEF_BACK;
					curFore = KVI_LABEL_DEF_FORE;
				}
			}
			break;
			case KviControlCodes::Icon:
			{
				++idx;

				unsigned int icoStart = idx;
				while((idx < (unsigned int)text.length()) && (text[(int)idx].unicode() > 32))idx++;

				KviCString lookupString = text.mid(icoStart,idx - icoStart);

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

void KviTopicWidget::setTopic(const QString & topic)
{
	m_szTopic = topic;
	m_pLabel->setText(KviHtmlGenerator::convertToHtml(Qt::escape(m_szTopic)));

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
		if(g_pRecentTopicList->count() >= KVI_MAX_RECENT_TOPICS)
		{
			g_pRecentTopicList->erase(g_pRecentTopicList->begin());
		}
		g_pRecentTopicList->append(m_szTopic);
	}
	updateToolTip();
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

		txt += KviHtmlGenerator::convertToHtml(Qt::escape(m_szTopic));
		txt +=          "</center></td></tr>";

		//FIXME hardcoding styles sucks
		if(!m_szSetBy.isEmpty())
		{
			txt +=      "<tr><td bgcolor=\"#D0D0D0\"><font color=\"#000000\">";
			txt +=       __tr2qs("Set by") + " <b>" + m_szSetBy + "</b>";
			txt +=      "</font></td></tr>";

			if(!m_szSetAt.isEmpty())
			{
				txt +=      "<tr><td bgcolor=\"#D0D0D0\"><font color=\"#000000\">";
				txt +=       __tr2qs("Set on") + " <b>" + m_szSetAt + "</b>";
				txt +=      "</font></td></tr>";
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
	int h = qMax(fm.height(), 14) + 2*(KVI_INPUT_MARGIN + KVI_INPUT_PADDING + KVI_INPUT_XTRAPADDING);
	int w = fm.width(QLatin1Char('x')) * 17 + 2*(KVI_INPUT_MARGIN + KVI_INPUT_PADDING + KVI_INPUT_XTRAPADDING);
	QStyleOptionFrameV2 option;
	option.initFrom(this);
	option.rect = rect();
	option.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, this);
	option.midLineWidth = 0;

	option.state |= QStyle::State_Sunken;
	option.features = QStyleOptionFrameV2::None;

	return (style()->sizeFromContents(QStyle::CT_LineEdit, &option, QSize(w, h).
		expandedTo(QApplication::globalStrut()), this));
}

void KviTopicWidget::switchMode()
{
	int iMaxLen=-1;
	QObject * w = parent();
	QString szModes;
	bool bCanEdit = TRUE;
	while(w)
	{
		if(w->inherits("KviChannelWindow"))
		{
			KviChannelWindow *chan = ((KviChannelWindow *)w);
			iMaxLen = chan->connection()->serverInfo()->maxTopicLen();
			chan->getChannelModeString(szModes);
			if(chan->plainChannelMode().contains('t') &&
				!( chan->isMeHalfOp() ||
					chan->isMeOp() ||
					chan->isMeChanOwner() ||
					chan->isMeChanAdmin() ||
					chan->connection()->userInfo()->hasUserMode('o') ||
					chan->connection()->userInfo()->hasUserMode('O')
				)
			)
			{
				bCanEdit=false;
			}
			break;
		}
		w = w->parent();
	}
	if(m_pInput == 0)
	{
		m_pInput=new KviInputEditor(this,m_pKviChannelWindow);
		m_pInput->setObjectName("topicw_inputeditor");
		m_pInput->setReadOnly(!bCanEdit);
		if(iMaxLen>0)
			m_pInput->setMaxBufferSize(iMaxLen);
		m_pInput->setGeometry(0,0,width() - (height() << 2)+height(),height());
		m_pInput->setText(m_szTopic);
		connect(m_pInput,SIGNAL(enterPressed()),this,SLOT(acceptClicked()));
		connect(m_pInput,SIGNAL(escapePressed()),this,SLOT(discardClicked()));
		m_pInput->installEventFilter(this);

		m_pHistory = new QPushButton(this);
		m_pHistory->setObjectName("topicw_historybutton");
		m_pHistory->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Time))));
		m_pHistory->setGeometry(width() - (height() << 2)+height(),0,height(),height());
		KviTalToolTip::add(m_pHistory,__tr2qs("History"));
		m_pHistory->show();
		connect(m_pHistory,SIGNAL(clicked()),this,SLOT(historyClicked()));

		m_pAccept = new QPushButton(this);
		m_pAccept->setObjectName("topicw_acceptbutton");
		m_pAccept->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept))));
		m_pAccept->setGeometry(width() - (height() << 1),0,height(),height());
		m_pAccept->setEnabled(bCanEdit);
		m_pAccept->show();
		KviTalToolTip::add(m_pAccept,__tr2qs("Commit Changes"));
		connect(m_pAccept,SIGNAL(clicked()),this,SLOT(acceptClicked()));

		m_pDiscard = new QPushButton(this);
		m_pDiscard->setObjectName("topicw_discardbutton");
		m_pDiscard->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard))));
		m_pDiscard->setGeometry(width() - height(),0,height(),height());
		KviTalToolTip::add(m_pDiscard,__tr2qs("Discard Changes"));
		m_pDiscard->show();
		connect(m_pDiscard,SIGNAL(clicked()),this,SLOT(discardClicked()));

		m_pInput->home();
		m_pInput->show();
		m_pInput->setFocus();

		m_pLabel->hide();
	} else {
		deactivate();
	}
}

void KviTopicWidget::mousePressEvent(QMouseEvent * e)
{

	if(!(e->button() & Qt::RightButton))
		return;
	if(!m_pContextPopup)
	{
		m_pContextPopup = new QMenu(this);
		connect(m_pContextPopup,SIGNAL(aboutToShow()),this,SLOT(contextPopupAboutToShow()));
	}
	m_pContextPopup->popup(mapToGlobal(e->pos()));
}

void KviTopicWidget::contextPopupAboutToShow()
{
	if(!m_pContextPopup)
		return; // hm ?
	m_pContextPopup->clear();
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Copy)),__tr2qs("Copy to clipboard"),this,SLOT(copy()));
}

void KviTopicWidget::copy()
{
	QClipboard * c = QApplication::clipboard();
	if(!c)
		return;
	if(c->supportsSelection())
		c->setText(m_szTopic,QClipboard::Selection);
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
				complete();
				return TRUE;
			}
			break;
		case QEvent::MouseButtonRelease:
			if ( m_pCompletionBox->rect().contains( ((QMouseEvent*)e)->pos() ) ) {
				QMouseEvent tmp( QEvent::MouseButtonDblClick,
						((QMouseEvent*)e)->pos(), ((QMouseEvent*)e)->button(), ((QMouseEvent*)e)->buttons(), ((QMouseEvent*)e)->modifiers() ) ;
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
				if ( !(((QKeyEvent *)e)->modifiers() & Qt::AltModifier) )
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
	return QWidget::eventFilter(object,e);
}

void KviTopicWidget::keyPressEvent(QKeyEvent * e)
{
	e->accept();
	return;
}

void KviTopicWidget::resizeEvent(QResizeEvent *)
{
	if(m_pInput)
	{
		m_pInput->setGeometry(0,0,width() - (height() << 2)+height(),height());
		m_pHistory->setGeometry(width() - (height() << 2)+height(),0,height(),height());
		m_pAccept->setGeometry(width() - (height() << 1),0,height(),height());
		m_pDiscard->setGeometry(width() - height(),0,height(),height());
	} else {
		m_pLabel->setGeometry(0,0,width(),height());
	}
}

void KviTopicWidget::deactivate()
{
	popDownListBox();
	if(m_pInput)
	{
		m_pInput->deleteLater();
		m_pInput = 0;
		m_pHistory->deleteLater();
		m_pHistory = 0;
		m_pAccept->deleteLater();
		m_pAccept = 0;
		m_pDiscard->deleteLater();
		m_pDiscard = 0;
	}

	m_pLabel->show();
	resizeEvent(0);
	// try to find a KviWindow parent and give it the focus

	m_pKviChannelWindow->setFocus();
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

		for(QStringList::Iterator it = g_pRecentTopicList->begin(); it != g_pRecentTopicList->end(); ++it)
			new KviTopicListBoxItem(m_pCompletionBox,*it);

		m_pCompletionBox->resize(m_pInput->width(),6*m_pCompletionBox->fontMetrics().height()+20);
		QPoint point=m_pInput->mapToGlobal(QPoint(0,0));
		point+=QPoint(0,m_pInput->height());
		m_pCompletionBox->move(point);
		m_pCompletionBox->show();
	}
}

void KviTopicWidget::acceptClicked()
{
	if(!m_pInput->isReadOnly())
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

void KviTopicWidget::complete()
{
	if(m_pCompletionBox->selectedItems().count() >0)
	{
		KviTopicListBoxItem* item = (KviTopicListBoxItem*) m_pCompletionBox->selectedItems().first();
		m_pInput->setText(item->text());
		popDownListBox();
	}
}

QChar KviTopicWidget::getSubstituteChar(unsigned short control_code)
{
	switch(control_code)
	{
		case KviControlCodes::Color:
			return QChar('K');
			break;
		case KviControlCodes::Bold:
			return QChar('B');
			break;
		case KviControlCodes::Reset:
			return QChar('O');
			break;
		case KviControlCodes::Reverse:
			return QChar('R');
			break;
		case KviControlCodes::Underline:
			return QChar('U');
			break;
		case KviControlCodes::Icon:
			return QChar('I');
			break;
		default:
			return QChar(control_code);
			break;
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviTopicWidget.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
