//=============================================================================
//
//   File : KviTopicWidget.cpp
//   Creation date : Fri Aug 4 2000 12:09:21 by Szymon Stefanek
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

#include "KviTopicWidget.h"
#include "KviOptions.h"
#include "KviControlCodes.h"
#include "KviLocale.h"
#include "kvi_defaults.h"
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

#include <QClipboard>
#include <QEvent>
#include <QFontMetrics>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QRegExp>

extern KviColorWindow * g_pColorWindow;

#define KVI_LABEL_DEF_BACK 100
#define KVI_LABEL_DEF_FORE 101

extern QStringList * g_pRecentTopicList;

KviTopicListBoxItemDelegate::KviTopicListBoxItemDelegate(QAbstractItemView * pWidget)
    : KviTalIconAndRichTextItemDelegate(pWidget)
{
}

KviTopicListBoxItemDelegate::~KviTopicListBoxItemDelegate()
    = default;

QSize KviTopicListBoxItemDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex & index) const
{
	KviTalListWidget * listWidget = (KviTalListWidget *)parent();
	KviTopicListBoxItem * item = (KviTopicListBoxItem *)listWidget->item(index.row());

	return listWidget->fontMetrics().size(Qt::TextSingleLine, KviControlCodes::stripControlBytes(item->text()));
}

void KviTopicListBoxItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	KviTalListWidget * listWidget = (KviTalListWidget *)parent();
	KviTopicListBoxItem * item = (KviTopicListBoxItem *)listWidget->item(index.row());

	if(item)
		KviTopicWidget::paintColoredText(p, item->text(), option.palette, option.rect);
}

KviTopicListBoxItem::KviTopicListBoxItem(KviTalListWidget * listbox, const QString & text)
    : KviTalListWidgetText(listbox, text)
{
}

KviTopicListBoxItem::~KviTopicListBoxItem()
    = default;

int KviTopicListBoxItem::width(const KviTalListWidget * lb) const
{
	return lb->fontMetrics().width(KviControlCodes::stripControlBytes(text()));
}

KviTopicWidget::KviTopicWidget(QWidget * par, KviChannelWindow * pChannel, const char * name)
    : QWidget(par)
{
	setObjectName(name);
	m_pKviChannelWindow = pChannel;
	m_pHistory = nullptr;
	m_pAccept = nullptr;
	m_pDiscard = nullptr;
	m_pContextPopup = nullptr;
	m_iCursorPosition = 0;
	m_pInput = nullptr;

	m_pLabel = new KviThemedLabel(this, pChannel, "topic_label");
	m_pLabel->setTextFormat(Qt::RichText);
	connect(m_pLabel, SIGNAL(doubleClicked()), this, SLOT(switchMode()));

	reset();

	m_pCompletionBox = new KviTalListWidget(this, "topic_completion_box", Qt::Popup);
	m_pCompletionBox->setFont(font());
	m_pCompletionBox->setPalette(palette());
	m_pCompletionBox->setFrameStyle(QFrame::Box | QFrame::Plain);
	m_pCompletionBox->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pCompletionBox->setSelectionMode(QAbstractItemView::SingleSelection);

	m_pItemDelegate = new KviTopicListBoxItemDelegate(m_pCompletionBox);
	m_pCompletionBox->setItemDelegate(m_pItemDelegate);

	connect(m_pCompletionBox, SIGNAL(itemSelectionChanged()), this, SLOT(complete()));
	m_pCompletionBox->hide();

	setAutoFillBackground(false);
	setContentsMargins(0, 0, 0, 0);
	applyOptions();
}

KviTopicWidget::~KviTopicWidget()
{
	delete m_pCompletionBox;
}

void KviTopicWidget::popDownListBox()
{
	m_pCompletionBox->removeEventFilter(this);
	m_pCompletionBox->hide();
}

void KviTopicWidget::reset()
{
	KviTalToolTip::remove(this);
	QString szTip;

	static QString tdh = "<tr><td style=\"background-color: rgb(48,48,48); white-space: nowrap; font-weight: bold; color: rgb(255,255,255); text-align:center; padding-left: 5px; padding-right: 5px;\">";
	static QString nrs = "<tr><td style=\"white-space: pre;\">";
	static QString enr = "</td></tr>";

	szTip = "<table>";

	m_szTopic = "";
	m_pLabel->setText(KviHtmlGenerator::convertToHtml(__tr2qs("Unknown channel topic")));

	szTip += tdh;
	szTip += __tr2qs("Channel Topic");
	szTip += enr + nrs;
	szTip += __tr2qs("No topic message has been received from the server yet");
	szTip += enr;

	m_szSetAt = "";
	m_szSetBy = "";

	szTip += "</table>";

	KviTalToolTip::add(this, szTip);
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

	// reset topic html too (in case colors have been changed)
	m_pLabel->setText(KviHtmlGenerator::convertToHtml(m_szTopic, true));
}

void KviTopicWidget::paintColoredText(QPainter * p, QString text, const QPalette & cg, const QRect & rect)
{
	QFontMetrics fm(p->fontMetrics());
	bool curBold = false;
	bool curItalic = false;
	bool curUnderline = false;
	unsigned char curFore = KVI_LABEL_DEF_FORE; //default fore
	unsigned char curBack = KVI_LABEL_DEF_BACK; //default back
	int baseline = rect.top() + rect.height() - fm.descent() - 1;

	int curX = rect.x() + 1;     // is the margin
	int maxX = rect.width() - 2; // 2 is 1*margin
	unsigned int idx = 0;

	QFont::Style normalFontStyle = p->font().style();

	while((idx < (unsigned int)text.length()) && (curX < maxX))
	{
		unsigned short c = text[(int)idx].unicode();

		unsigned int start = idx;

		while((idx < (unsigned int)text.length()) && (c != KviControlCodes::Color) && (c != KviControlCodes::Bold) && (c != KviControlCodes::Italic) && (c != KviControlCodes::Underline) && (c != KviControlCodes::Reverse) && (c != KviControlCodes::Reset) && (c != KviControlCodes::Icon))
		{
			idx++;
			c = text[(int)idx].unicode();
		}

		int len = idx - start;
		int wdth;

		if(len > 0)
		{
			QString szText = text.mid(start, len);

			wdth = fm.width(szText);

			if(curFore == KVI_LABEL_DEF_FORE)
			{
				p->setPen(cg.text().color());
			}
			else
			{
				if(curFore > KVI_EXTCOLOR_MAX)
					p->setPen(cg.background().color());
				else
					p->setPen(getMircColor(curFore));
			}

			if(curBack != KVI_LABEL_DEF_BACK)
			{
				if(curBack > KVI_EXTCOLOR_MAX)
				{
					p->fillRect(curX, rect.y() + 2, wdth, rect.height() - 4,
					    cg.text());
				}
				else
				{
					p->fillRect(curX, rect.y() + 2, wdth, rect.height() - 4,
					    getMircColor(curBack));
				}
			}

			QFont newFont = p->font();
			newFont.setStyle((curItalic ^ (normalFontStyle != QFont::StyleNormal)) ? QFont::StyleItalic : QFont::StyleNormal);
			p->setFont(newFont);

			p->drawText(curX, baseline, szText.left(len));

			if(curBold)
				p->drawText(curX + 1, baseline, szText.left(len));
			if(curUnderline)
			{
				p->drawLine(curX, baseline + 1, curX + wdth, baseline + 1);
			}
		}
		else
		{
			wdth = 0;
		}

		curX += wdth;

		switch(c)
		{
			case KviControlCodes::Bold:
				curBold = !curBold;
				++idx;
				break;
			case KviControlCodes::Italic:
				curItalic = !curItalic;
				++idx;
				break;
			case KviControlCodes::Underline:
				curUnderline = !curUnderline;
				++idx;
				break;
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
				curItalic = false;
				curUnderline = false;
				++idx;
				break;
			case KviControlCodes::Color:
			{
				++idx;
				unsigned char fore;
				unsigned char back;
				idx = KviControlCodes::getUnicodeColorBytes(text, idx, &fore, &back);
				if(fore != KviControlCodes::NoChange)
				{
					curFore = fore;
					if(back != KviControlCodes::NoChange)
						curBack = back;
				}
				else
				{
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
				while((idx < (unsigned int)text.length()) && (text[(int)idx].unicode() > 32))
					idx++;

				KviCString lookupString = text.mid(icoStart, idx - icoStart);

				KviTextIcon * icon = g_pTextIconManager->lookupTextIcon(lookupString.ptr());
				if(icon)
				{
					QPixmap * pigzmap = icon->pixmap();
					p->drawPixmap(curX, (baseline + 2) - pigzmap->height(), *(pigzmap));
					curX += pigzmap->width();
				}
				else
				{
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
	m_pLabel->setText(KviHtmlGenerator::convertToHtml(m_szTopic, true));

	int pos = g_pRecentTopicList->indexOf(m_szTopic);

	if(pos >= 0)
	{
		if(g_pRecentTopicList->count() > 1)
			g_pRecentTopicList->move(pos, g_pRecentTopicList->count() - 1);
	}
	else if(!m_szTopic.isEmpty())
	{
		if(g_pRecentTopicList->count() >= KVI_MAX_RECENT_TOPICS)
			g_pRecentTopicList->removeFirst();
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

	static QString br("<br>");
	static QString bb("<b>");
	static QString be("</b>");
	static QString cln(":");
	static QString space(" ");
	static QString tds = "<tr><td style=\"background-color: rgb(48,48,48); white-space: nowrap; font-weight: bold; color: rgb(255,255,255); text-align:center; padding-left: 5px; padding-right: 5px;\">";
	static QString snr = "<tr><td style=\"white-space: pre; padding-left: 5px; padding-right: 5px;\">";
	static QString nrs = "<tr><td>";
	static QString enr = "</td></tr>";

	QString txt = "<html><body><table>";

	if(!m_szTopic.isEmpty())
	{
		txt += tds;
		txt += __tr2qs("Channel Topic");
		txt += enr + nrs;
		txt += KviHtmlGenerator::convertToHtml(KviControlCodes::stripControlBytes(m_szTopic), true);
		txt += enr;

		if(!m_szSetBy.isEmpty())
		{
			txt += R"(<tr><td style="white-space: pre"; bgcolor="#E0E0E0"><font color="#000000">)";
			txt += __tr2qs("Set by") + cln + space + bb + m_szSetBy + be;
			txt += "</font>" + enr;

			if(!m_szSetAt.isEmpty())
			{
				txt += R"(<tr><td bgcolor="#E0E0E0"><font color="#000000">)";
				txt += __tr2qs("Set on") + cln + space + bb + m_szSetAt + be;
				txt += "</font>" + enr;
			}
		}

		txt += nrs;
		txt += __tr2qs("Double-click to edit...");
		txt += enr;
	}
	else
	{
		txt += tds;
		txt += __tr2qs("Channel Topic");
		txt += enr + nrs;
		txt += __tr2qs("No topic is set");
		txt += enr + nrs;
		txt += __tr2qs("Double-click to set...");
		txt += enr;
	}

	txt += "</table></body></html>";

	KviTalToolTip::add(this, txt);
}

QSize KviTopicWidget::sizeHint() const
{
	QFontMetrics fm(font());
	int h = qMax(fm.height(), 14) + 2 * (KVI_INPUT_MARGIN + KVI_INPUT_XTRAPADDING);
	int w = fm.width(QLatin1Char('x')) * 17 + 2 * (KVI_INPUT_MARGIN + KVI_INPUT_XTRAPADDING);
	QStyleOptionFrame option;
	option.initFrom(this);
	option.rect = rect();
	option.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, this);
	option.midLineWidth = 0;

	option.state |= QStyle::State_Sunken;
	option.features = QStyleOptionFrame::None;

	return (style()->sizeFromContents(QStyle::CT_LineEdit, &option, QSize(w, h).expandedTo(QApplication::globalStrut()), this));
}

void KviTopicWidget::switchMode()
{
	int iMaxLen = -1;
	QObject * w = parent();
	QString szModes;
	bool bCanEdit = true;
	while(w)
	{
		if(w->inherits("KviChannelWindow"))
		{
			KviChannelWindow * chan = ((KviChannelWindow *)w);
			iMaxLen = chan->connection()->serverInfo()->maxTopicLen();
			chan->getChannelModeString(szModes);
			if(chan->plainChannelMode().contains('t') && !(chan->isMeHalfOp(true) || chan->connection()->userInfo()->hasUserMode('o') || chan->connection()->userInfo()->hasUserMode('O')))
			{
				bCanEdit = false;
			}
			break;
		}
		w = w->parent();
	}
	if(m_pInput == nullptr)
	{
		m_pInput = new KviInputEditor(this, m_pKviChannelWindow);
		m_pInput->setObjectName("topicw_inputeditor");
		m_pInput->setReadOnly(!bCanEdit);
		if(iMaxLen > 0)
			m_pInput->setMaxBufferSize(iMaxLen);
		m_pInput->setGeometry(0, 0, width() - (height() << 2) + height(), height());
		m_pInput->setText(m_szTopic);
		connect(m_pInput, SIGNAL(enterPressed()), this, SLOT(acceptClicked()));
		connect(m_pInput, SIGNAL(escapePressed()), this, SLOT(discardClicked()));
		m_pInput->installEventFilter(this);

		m_pHistory = new QPushButton(this);
		m_pHistory->setObjectName("topicw_historybutton");
		m_pHistory->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::History))));
		m_pHistory->setGeometry(width() - (height() << 2) + height(), 0, height(), height());
		KviTalToolTip::add(m_pHistory, __tr2qs("History"));
		m_pHistory->show();
		connect(m_pHistory, SIGNAL(clicked()), this, SLOT(historyClicked()));

		m_pAccept = new QPushButton(this);
		m_pAccept->setObjectName("topicw_acceptbutton");
		m_pAccept->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept))));
		m_pAccept->setGeometry(width() - (height() << 1), 0, height(), height());
		m_pAccept->setEnabled(bCanEdit);
		m_pAccept->show();
		KviTalToolTip::add(m_pAccept, __tr2qs("Commit changes"));
		connect(m_pAccept, SIGNAL(clicked()), this, SLOT(acceptClicked()));

		m_pDiscard = new QPushButton(this);
		m_pDiscard->setObjectName("topicw_discardbutton");
		m_pDiscard->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard))));
		m_pDiscard->setGeometry(width() - height(), 0, height(), height());
		KviTalToolTip::add(m_pDiscard, __tr2qs("Discard changes"));
		m_pDiscard->show();
		connect(m_pDiscard, SIGNAL(clicked()), this, SLOT(discardClicked()));

		m_pInput->home();
		m_pInput->show();
		m_pInput->setFocus();

		m_pLabel->hide();
	}
	else
	{
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
		connect(m_pContextPopup, SIGNAL(aboutToShow()), this, SLOT(contextPopupAboutToShow()));
	}
	m_pContextPopup->popup(mapToGlobal(e->pos()));
}

void KviTopicWidget::contextPopupAboutToShow()
{
	if(!m_pContextPopup)
		return; // hm ?
	m_pContextPopup->clear();
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Copy)), __tr2qs("Copy Topic to Clipboard"), this, SLOT(copy()));
}

void KviTopicWidget::copy()
{
	QClipboard * c = QApplication::clipboard();
	if(!c)
		return;
	if(c->supportsSelection())
		c->setText(m_szTopic, QClipboard::Selection);
	c->setText(m_szTopic, QClipboard::Clipboard);
}

bool KviTopicWidget::eventFilter(QObject * object, QEvent * e)
{
	if(!e)
		return true;
	if(object == m_pCompletionBox)
	{
		switch(e->type())
		{
			case QEvent::MouseButtonPress:
				if(m_pCompletionBox->rect().contains(((QMouseEvent *)e)->pos()))
				{
					complete();
					return true;
				}
				break;
			case QEvent::MouseButtonRelease:
				if(m_pCompletionBox->rect().contains(((QMouseEvent *)e)->pos()))
				{
					QMouseEvent tmp(QEvent::MouseButtonDblClick,
					    ((QMouseEvent *)e)->pos(), ((QMouseEvent *)e)->button(), ((QMouseEvent *)e)->buttons(), ((QMouseEvent *)e)->modifiers());
					// will hide popup
					QApplication::sendEvent(object, &tmp);
					return true;
				}
				else
				{
					if(m_pCompletionBox->isVisible())
						popDownListBox();
				}
				break;
			case QEvent::KeyPress:
				switch(((QKeyEvent *)e)->key())
				{
					case Qt::Key_Up:
					case Qt::Key_Down:
						if(!(((QKeyEvent *)e)->modifiers() & Qt::AltModifier))
							break;
					case Qt::Key_F4:
					case Qt::Key_Escape:
						if(m_pCompletionBox->isVisible())
						{
							popDownListBox();
							return true;
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
	return QWidget::eventFilter(object, e);
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
		m_pInput->setGeometry(0, 0, width() - (height() << 2) + height(), height());
		m_pHistory->setGeometry(width() - (height() << 2) + height(), 0, height(), height());
		m_pAccept->setGeometry(width() - (height() << 1), 0, height(), height());
		m_pDiscard->setGeometry(width() - height(), 0, height(), height());
	}
	else
	{
		m_pLabel->setGeometry(0, 0, width(), height());
	}
}

void KviTopicWidget::deactivate()
{
	popDownListBox();
	if(m_pInput)
	{
		m_pInput->deleteLater();
		m_pInput = nullptr;
		m_pHistory->deleteLater();
		m_pHistory = nullptr;
		m_pAccept->deleteLater();
		m_pAccept = nullptr;
		m_pDiscard->deleteLater();
		m_pDiscard = nullptr;
	}

	m_pLabel->show();
	resizeEvent(nullptr);

	if(g_pColorWindow && g_pColorWindow->isVisible())
		g_pColorWindow->hide();

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
		m_pCompletionBox->installEventFilter(this);
		m_pCompletionBox->clear();

		for(auto & it : *g_pRecentTopicList)
			new KviTopicListBoxItem(m_pCompletionBox, it);

		m_pCompletionBox->resize(m_pInput->width(), 6 * m_pCompletionBox->fontMetrics().height() + 20);
		QPoint point = m_pInput->mapToGlobal(QPoint(0, 0));
		point += QPoint(0, m_pInput->height());
		m_pCompletionBox->move(point);
		m_pCompletionBox->show();
	}
}

void KviTopicWidget::acceptClicked()
{
	if(!m_pInput->isReadOnly())
	{
		QString tmp = m_pInput->text();
		if(tmp != m_szTopic)
			emit topicSelected(tmp);
	}
	deactivate();
}

void KviTopicWidget::insertChar(QChar c)
{
	insertText(QString(c));
}

void KviTopicWidget::insertText(const QString & c)
{
	if(m_pInput)
		m_pInput->insertText(c);
}

void KviTopicWidget::complete()
{
	if(m_pCompletionBox->selectedItems().count() > 0)
	{
		KviTopicListBoxItem * item = (KviTopicListBoxItem *)m_pCompletionBox->selectedItems().first();
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
		case KviControlCodes::Italic:
			return QChar('I');
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
			return QChar('E');
			break;
		default:
			return { control_code };
			break;
	}
}
