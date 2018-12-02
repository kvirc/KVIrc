//=============================================================================
//
//   File : OptionsWidget_message.cpp
//   Creation date : Sat Aug 11 2001 18:12:00 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _WANT_OPTION_FLAGS_

#include "OptionsWidget_message.h"

#include "kvi_settings.h"
#include "KviOptions.h"
#include "KviApplication.h"
#include "KviLocale.h"
#include "KviControlCodes.h"
#include "KviConfigurationFile.h"
#include "KviFileDialog.h"
#include "KviTalVBox.h"

#include <QLayout>
#include <QPushButton>
#include <QCursor>
#include <QPainter>
#include <QWidgetAction>

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
#include <unistd.h> // symlink()
#endif

#ifdef COMPILE_PSEUDO_TRANSPARENCY
extern KVIRC_API QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

OptionsWidget_message::OptionsWidget_message(QWidget * pParent)
    : KviOptionsWidget(pParent)
{
}

OptionsWidget_message::~OptionsWidget_message()
    = default;

OptionsWidget_privmsg::OptionsWidget_privmsg(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("privmsg_options_widget");
	createLayout();

	KviTalGroupBox * g = addGroupBox(0, 0, 0, 0, Qt::Horizontal, __tr2qs_ctx("General", "options"));

	addBoolSelector(g, __tr2qs_ctx("Show message icons", "options"), KviOption_boolIrcViewShowImages);
	addBoolSelector(g, __tr2qs_ctx("Draw some emoticons (smileys) as pictures", "options"), KviOption_boolDrawEmoticons);
	addBoolSelector(g, __tr2qs_ctx("Don't show colors in user messages", "options"), KviOption_boolStripMircColorsInUserMessages);

	g = addGroupBox(0, 1, 0, 1, Qt::Horizontal, __tr2qs_ctx("Nicknames", "options"));

	m_pUseSmartColorSelector = addBoolSelector(g, __tr2qs_ctx("Smart nickname colors", "options"), KviOption_boolColorNicks);

	connect(m_pUseSmartColorSelector, SIGNAL(toggled(bool)), this, SLOT(enableDisableSmartColorSelector(bool)));

	m_pUseSmartColorWithBackgroundSelector = addBoolSelector(g, __tr2qs_ctx("Use a background color for smart nickname colors", "options"), KviOption_boolColorNicksWithBackground, KVI_OPTION_BOOL(KviOption_boolColorNicks));

	KviTalHBox * hb = new KviTalHBox(g);
	hb->setSpacing(4);
	m_pSpecialSmartColorSelector = addBoolSelector(hb, __tr2qs_ctx("Use specified smart colors for own nick:", "options"), KviOption_boolUseSpecifiedSmartColorForOwnNick, KVI_OPTION_BOOL(KviOption_boolColorNicks));

	m_pSmartColorSelector = addMircTextColorSelector(hb, "", KviOption_uintUserIrcViewOwnForeground, KviOption_uintUserIrcViewOwnBackground, KVI_OPTION_BOOL(KviOption_boolColorNicks) && KVI_OPTION_BOOL(KviOption_boolUseSpecifiedSmartColorForOwnNick));

	connect(m_pSpecialSmartColorSelector, SIGNAL(toggled(bool)), this, SLOT(enableDisableSmartColorSelector(bool)));
	connect(m_pUseSmartColorSelector, SIGNAL(toggled(bool)), m_pSpecialSmartColorSelector, SLOT(setEnabled(bool)));
	connect(m_pUseSmartColorSelector, SIGNAL(toggled(bool)), m_pUseSmartColorWithBackgroundSelector, SLOT(setEnabled(bool)));

	enableDisableSmartColorSelector(true);

	KviBoolSelector * b2 = addBoolSelector(g, __tr2qs_ctx("Use same colors as in the userlist", "options"), KviOption_boolUseUserListColorsAsNickColors, !KVI_OPTION_BOOL(KviOption_boolColorNicks));
	connect(m_pUseSmartColorSelector, SIGNAL(toggled(bool)), b2, SLOT(setNotEnabled(bool)));

	addBoolSelector(g, __tr2qs_ctx("Show nicknames in bold", "options"), KviOption_boolBoldedNicks);
	addBoolSelector(g, __tr2qs_ctx("Show user and host", "options"), KviOption_boolShowUserAndHostInPrivmsgView);
	addBoolSelector(g, __tr2qs_ctx("Show channel mode prefix", "options"), KviOption_boolShowChannelUserFlagInPrivmsgView);
	KviBoolSelector * b = addBoolSelector(g, __tr2qs_ctx("User-defined prefix and postfix", "options"), KviOption_boolUseExtendedPrivmsgView);

	QLabel * l = addLabel(g, __tr2qs_ctx("[PREFIX]nickname[!user@host][POSTFIX] message", "options"));

	l->setEnabled(KVI_OPTION_BOOL(KviOption_boolUseExtendedPrivmsgView));
	connect(b,
	    SIGNAL(toggled(bool)),
	    l,
	    SLOT(setEnabled(bool)));

	KviTalVBox * vb = new KviTalVBox(g);
	vb->setSpacing(5);

	connect(
	    b,
	    SIGNAL(toggled(bool)),
	    addStringSelector(vb, __tr2qs_ctx("Prefix:", "options"), KviOption_stringExtendedPrivmsgPrefix, KVI_OPTION_BOOL(KviOption_boolUseExtendedPrivmsgView)),
	    SLOT(setEnabled(bool)));
	connect(
	    b,
	    SIGNAL(toggled(bool)),
	    addStringSelector(vb, __tr2qs_ctx("Postfix:", "options"), KviOption_stringExtendedPrivmsgPostfix, KVI_OPTION_BOOL(KviOption_boolUseExtendedPrivmsgView)),
	    SLOT(setEnabled(bool)));
	addRowSpacer(0, 3, 0, 3);
}

void OptionsWidget_privmsg::enableDisableSmartColorSelector(bool)
{
	m_pSmartColorSelector->setEnabled(m_pSpecialSmartColorSelector->isChecked() && m_pUseSmartColorSelector->isChecked());
}

OptionsWidget_privmsg::~OptionsWidget_privmsg()
    = default;

OptionsWidget_timestamp::OptionsWidget_timestamp(QWidget * pParent)
    : KviOptionsWidget(pParent)
{
	createLayout();
	m_pUseTimestampSelector = addBoolSelector(0, 0, 0, 0, __tr2qs_ctx("Show timestamp", "options"), KviOption_boolIrcViewTimestamp);

	connect(m_pUseTimestampSelector, SIGNAL(toggled(bool)), this, SLOT(enableDisableTimestampSelector(bool)));
	KviBoolSelector * b = addBoolSelector(0, 1, 0, 1, __tr2qs_ctx("Use UTC time for timestamp", "options"), KviOption_boolIrcViewTimestampUTC, KVI_OPTION_BOOL(KviOption_boolIrcViewTimestamp));

	connect(m_pUseTimestampSelector, SIGNAL(toggled(bool)), b, SLOT(setEnabled(bool)));

	KviTalHBox * hb = new KviTalHBox(this);
	addWidgetToLayout(hb, 0, 2, 0, 2);

	m_pSpecialTimestampColorSelector = addBoolSelector(hb, __tr2qs_ctx("Use special color for timestamps:", "options"), KviOption_boolUseSpecialColorForTimestamp, KVI_OPTION_BOOL(KviOption_boolIrcViewTimestamp));
	connect(m_pSpecialTimestampColorSelector, SIGNAL(toggled(bool)), this, SLOT(enableDisableTimestampSelector(bool)));
	connect(m_pUseTimestampSelector, SIGNAL(toggled(bool)), m_pSpecialTimestampColorSelector, SLOT(setEnabled(bool)));

	m_pTimestampColorSelector = addMircTextColorSelector(hb, "", KviOption_uintTimeStampForeground, KviOption_uintTimeStampBackground, KVI_OPTION_BOOL(KviOption_boolIrcViewTimestamp) && KVI_OPTION_BOOL(KviOption_boolUseSpecialColorForTimestamp));

	KviStringSelector * st = addStringSelector(0, 3, 0, 3, __tr2qs_ctx("Timestamp format:", "options"), KviOption_stringIrcViewTimestampFormat);
	connect(m_pUseTimestampSelector, SIGNAL(toggled(bool)), st, SLOT(setEnabled(bool)));

	connect(m_pUseTimestampSelector, SIGNAL(toggled(bool)), m_pSpecialTimestampColorSelector, SLOT(setEnabled(bool)));
	addRowSpacer(0, 4, 0, 4);
}

OptionsWidget_timestamp::~OptionsWidget_timestamp()
    = default;

void OptionsWidget_timestamp::enableDisableTimestampSelector(bool)
{
	m_pTimestampColorSelector->setEnabled(m_pUseTimestampSelector->isChecked() && m_pSpecialTimestampColorSelector->isChecked());
}

OptionsWidget_standardColors::OptionsWidget_standardColors(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("stdcolors");
	createLayout();

	for(int i = 0; i < 16; i++)
		m_pColorSelector[i] = addColorSelector(i % 4, i / 4, i % 4, i / 4, QString("%1:").arg(i), &(KVI_OPTION_MIRCCOLOR(i)));

	QLabel * l = addLabel(0, 4, 3, 4, __tr2qs_ctx(
	                                      "Please note that this is a set of standard IRC colors. "
	                                      "You can adjust them a bit, make them darker or lighter but you "
	                                      "shouldn't change their fundamental tint. "
	                                      "Black should stay black, dark green should still remain a green darker than the light one.",
	                                      "options"));
	l->setAlignment(Qt::AlignLeft);

	addRowSpacer(0, 5, 3, 5);

	layout()->setRowStretch(5, 1);

	QPushButton * pReset = addPushButton(3, 6, 3, 6, __tr2qs_ctx("Reset to Default", "options"));

	QObject::connect(pReset, SIGNAL(clicked()), this, SLOT(resetToDefaults()));
}

OptionsWidget_standardColors::~OptionsWidget_standardColors()
    = default;

void OptionsWidget_standardColors::resetToDefaults()
{
	m_pColorSelector[0]->forceColor(QColor(255, 255, 255));
	m_pColorSelector[1]->forceColor(QColor(0, 0, 0));
	m_pColorSelector[2]->forceColor(QColor(0, 0, 140));
	m_pColorSelector[3]->forceColor(QColor(0, 100, 0));
	m_pColorSelector[4]->forceColor(QColor(230, 0, 0));
	m_pColorSelector[5]->forceColor(QColor(150, 0, 0));
	m_pColorSelector[6]->forceColor(QColor(80, 0, 80));
	m_pColorSelector[7]->forceColor(QColor(255, 90, 0));
	m_pColorSelector[8]->forceColor(QColor(255, 255, 0));
	m_pColorSelector[9]->forceColor(QColor(0, 255, 0));
	m_pColorSelector[10]->forceColor(QColor(0, 150, 180));
	m_pColorSelector[11]->forceColor(QColor(170, 170, 255));
	m_pColorSelector[12]->forceColor(QColor(15, 15, 255));
	m_pColorSelector[13]->forceColor(QColor(200, 0, 200));
	m_pColorSelector[14]->forceColor(QColor(80, 80, 80));
	m_pColorSelector[15]->forceColor(QColor(170, 170, 170));
}

MessageListWidgetItem::MessageListWidgetItem(MessageListWidget * l, int optId)
    : KviTalListWidgetItem(l)
{
	m_iOptId = optId;
	m_pMsgType = new KviMessageTypeSettings(KVI_OPTION_MSGTYPE(optId));

	QString txt = g_msgtypeOptionsTable[m_iOptId].name;
	// skip the msgtype prefix
	txt.remove(0, KVI_MSGTYPE_OPTIONS_PREFIX_LEN);
	txt += " (";
	txt += __tr2qs_no_xgettext(m_pMsgType->type());
	txt += ")";

	setText(txt);
	setIcon(*(g_pIconManager->getSmallIcon(m_pMsgType->pixId())));
}

MessageListWidgetItem::~MessageListWidgetItem()
{
	delete m_pMsgType;
}

void MessageListWidgetItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & opt, const QModelIndex & index) const
{

	const KviTalListWidget * tb = (const KviTalListWidget *)parent();
	MessageListWidgetItem * it = static_cast<MessageListWidgetItem *>(index.internalPointer());
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p->save();
		p->setCompositionMode(QPainter::CompositionMode_Source);
		QColor col = KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p->fillRect(opt.rect, col);
		p->restore();
	}
	else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = tb->viewport()->mapToGlobal(opt.rect.topLeft());
		p->drawTiledPixmap(opt.rect, *g_pShadedChildGlobalDesktopBackground, pnt);
	}
	else
	{
#endif
		QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapIrcViewBackground).pixmap();
		if(pix)
		{
			QPoint pnt = tb->viewport()->mapToGlobal(opt.rect.topLeft());
			p->drawTiledPixmap(opt.rect, *pix, pnt);
		}
		else
		{
			p->fillRect(opt.rect, KVI_OPTION_COLOR(KviOption_colorIrcViewBackground));
		}
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	QString szText = index.data(Qt::DisplayRole).toString();
	QPoint pt = opt.rect.topLeft();

	pt.setX(pt.x() + 4);
	p->drawPixmap(pt, *(g_pIconManager->getSmallIcon(it->msgType()->pixId())));
	pt.setX(pt.x() + 18);
	// draw the background
	if(it->msgType()->back() <= KVI_EXTCOLOR_MAX)
	{
		QColor bColor = getMircColor(it->msgType()->back());
		p->fillRect(pt.x(), pt.y(), opt.rect.width() - pt.x(), opt.rect.height(), bColor);
	}
	unsigned char ucFore = it->msgType()->fore();
	if(ucFore > KVI_EXTCOLOR_MAX)
		ucFore = 0;
	p->setPen(QPen(getMircColor(ucFore)));
	pt.setX(pt.x() + 2);

	p->drawText(pt.x(), pt.y(), opt.rect.width() - pt.x(), opt.rect.height(), Qt::AlignLeft | Qt::AlignVCenter, szText);

	if(opt.state & QStyle::State_Selected)
	{
		QPen pen(Qt::white);
		p->setPen(pen);
		p->drawRect(opt.rect.adjusted(0, 0, -1, -1));
		p->drawRect(opt.rect.adjusted(1, 1, -2, -2));

		pen.setColor(Qt::black);
		pen.setStyle(Qt::DashLine);
		p->setPen(pen);
		p->drawRect(opt.rect.adjusted(0, 0, -1, -1));
		p->drawRect(opt.rect.adjusted(1, 1, -2, -2));
	}
}

MessageColorListWidgetItem::MessageColorListWidgetItem(KviTalListWidget * b, int idx)
    : KviTalListWidgetText(b, QString())
{

	m_iClrIdx = idx;
	if((idx < 0) || (idx > KVI_EXTCOLOR_MAX))
	{
		setText(__tr2qs_ctx("Transparent", "options"));
		setBackground(listWidget()->isEnabled() ? Qt::transparent : Qt::gray);
	}
	else
	{
		setBackground(QColor(getMircColor(m_iClrIdx)));
		setText(" ");
	}
}

MessageColorListWidgetItem::~MessageColorListWidgetItem()
    = default;

void MessageColorListWidgetItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & opt, const QModelIndex & index) const
{

	if(opt.state & QStyle::State_Selected)
	{
		QColor clr;
		const KviTalListWidget * lb = (const KviTalListWidget *)parent();
		MessageColorListWidgetItem * it = static_cast<MessageColorListWidgetItem *>(index.internalPointer());

		if((it->clrIdx() >= 0) && (it->clrIdx() <= KVI_EXTCOLOR_MAX))
		{
			clr = getMircColor(it->clrIdx());
		}
		else
		{
			clr = lb->palette().color(QPalette::Window);
		}

		QStyleOptionViewItem optz(opt);
		optz.state = opt.state ^ QStyle::State_Selected;
		QItemDelegate::paint(p, optz, index);

		QPen pen(QColor(255 - clr.red(), 255 - clr.green(), 255 - clr.blue()));
		pen.setStyle(Qt::DashLine);
		p->setPen(pen);
		p->drawRect(opt.rect.adjusted(0, 0, -1, -1));
		p->drawRect(opt.rect.adjusted(1, 1, -2, -2));
		p->drawRect(opt.rect.adjusted(2, 2, -3, -3));
	}
	else
	{
		QItemDelegate::paint(p, opt, index);
	}
}

OptionsWidget_messageColors::OptionsWidget_messageColors(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("messages");

	createLayout();

	int i;
	m_pLastItem = nullptr;

	m_pListView = new MessageListWidget(this);
	m_pListViewItemDelegate = new MessageListWidgetItemDelegate(m_pListView);
	m_pListView->setItemDelegate(m_pListViewItemDelegate);
	m_pListView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pListView->setFont(KVI_OPTION_FONT(KviOption_fontIrcView));
	m_pListView->viewport()->setAutoFillBackground(false);

	addWidgetToLayout(m_pListView, 0, 0, 2, 0);

	KviTalVBox * box = new KviTalVBox(this);
	addWidgetToLayout(box, 3, 0, 3, 0);

	new QLabel(__tr2qs_ctx("Background:", "options"), box);

	m_pBackListWidget = new KviTalListWidget(box);
	m_pBackListWidget->setMaximumWidth(150);
	m_pBackListWidgetDelegate = new MessageColorListWidgetItemDelegate(m_pBackListWidget);
	m_pBackListWidget->setItemDelegate(m_pBackListWidgetDelegate);

	m_pBackItems[16] = new MessageColorListWidgetItem(m_pBackListWidget, KviControlCodes::Transparent);
	for(i = 0; i < 16; i++)
	{
		m_pBackItems[i] = new MessageColorListWidgetItem(m_pBackListWidget, i);
	}

	new QLabel(__tr2qs_ctx("Foreground:", "options"), box);

	m_pForeListWidget = new KviTalListWidget(box);
	m_pForeListWidget->setMaximumWidth(150);
	m_pForeListWidgetDelegate = new MessageColorListWidgetItemDelegate(m_pForeListWidget);
	m_pForeListWidget->setItemDelegate(m_pForeListWidgetDelegate);

	for(i = 0; i < 16; i++)
	{
		m_pForeItems[i] = new MessageColorListWidgetItem(m_pForeListWidget, i);
	}

	new QLabel(__tr2qs_ctx("Alert level:", "options"), box);

	m_pLevelListWidget = new KviTalListWidget(box);
	m_pLevelListWidget->setMaximumWidth(150);

	for(i = 0; i < 6; i++)
	{
		QString tmpn;
		tmpn.setNum(i);
		new KviTalListWidgetText(m_pLevelListWidget, tmpn);
	}

	m_pIconButton = new QToolButton(box);
	connect(m_pIconButton, SIGNAL(clicked()), this, SLOT(iconButtonClicked()));

	m_pIconPopup = new QMenu(this);
	KviIconWidget * iw = new KviIconWidget(m_pIconPopup);
	connect(iw, SIGNAL(selected(KviIconManager::SmallIcon)), this, SLOT(newIconSelected(KviIconManager::SmallIcon)));

	QWidgetAction * pWaction = new QWidgetAction(m_pIconPopup);
	pWaction->setDefaultWidget(iw);
	m_pIconPopup->addAction(pWaction);

	m_pEnableLogging = new QCheckBox(__tr2qs_ctx("Log this", "options"), box);

	KviTalHBox * h = new KviTalHBox(this);
	addWidgetToLayout(h, 0, 1, 3, 1);
	QPushButton * b = new QPushButton(__tr2qs_ctx("Load from...", "options"), h);
	connect(b, SIGNAL(clicked()), this, SLOT(load()));
	b = new QPushButton(__tr2qs_ctx("Save As...", "options"), h);
	connect(b, SIGNAL(clicked()), this, SLOT(save()));

	for(i = 0; i < KVI_NUM_MSGTYPE_OPTIONS; i++)
		new MessageListWidgetItem(m_pListView, i);

	layout()->setRowStretch(0, 1);
	layout()->setColumnStretch(0, 1);

	connect(m_pListView, SIGNAL(itemSelectionChanged()), this, SLOT(itemChanged()));
	connect(m_pForeListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(colorChanged()));
	connect(m_pBackListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(colorChanged()));

	itemChanged();
}

OptionsWidget_messageColors::~OptionsWidget_messageColors()
{
	int i;
	for(i = 15; i >= 0; i--)
	{
		delete m_pForeItems[i];
	}
	for(i = 16; i >= 0; i--)
	{
		delete m_pBackItems[i];
	}
	delete m_pListView;
	delete m_pForeListWidget;
	delete m_pBackListWidget;

	delete m_pLevelListWidget;
	delete m_pEnableLogging;
	delete m_pIconButton;
	delete m_pIconPopup;
}

void OptionsWidget_messageColors::newIconSelected(KviIconManager::SmallIcon eIcon)
{
	if(!m_pLastItem)
		return;
	if(eIcon >= KviIconManager::IconCount)
		return;
	m_pLastItem->msgType()->setPixId(eIcon);
	m_pIconButton->setIcon(*(g_pIconManager->getSmallIcon(eIcon)));
	m_pListView->repaint(m_pListView->visualItemRect(m_pLastItem));
}

void OptionsWidget_messageColors::saveLastItem()
{
	if(!m_pLastItem)
		return;

	int curIt = m_pForeListWidget->currentRow();
	if(curIt >= 0) //quite useless
	{
		//qDebug("Setting fore %d",curIt);
		MessageColorListWidgetItem * fore = (MessageColorListWidgetItem *)m_pForeListWidget->item(curIt);
		//qDebug("And is %d",fore);
		if(fore)
			m_pLastItem->msgType()->setFore(fore->m_iClrIdx);
	}
	curIt = m_pBackListWidget->currentRow();
	if(curIt >= 0) //quite useless
	{
		MessageColorListWidgetItem * back = (MessageColorListWidgetItem *)m_pBackListWidget->item(curIt);
		if(back)
			m_pLastItem->msgType()->setBack(back->m_iClrIdx);
	}
	m_pLastItem->msgType()->enableLogging(m_pEnableLogging->isChecked());
	//qDebug("Updating","options");
	curIt = m_pLevelListWidget->currentRow();
	if(curIt < 0 || curIt > 5)
		curIt = 1;
	m_pLastItem->msgType()->setLevel(curIt);
	m_pListView->repaint(m_pListView->visualItemRect(m_pLastItem));
}

void OptionsWidget_messageColors::itemChanged()
{
	KviTalListWidgetItem * it = (KviTalListWidgetItem *)m_pListView->currentItem();
	//qDebug("Item changed","options");
	if(m_pLastItem)
		saveLastItem();

	m_pLastItem = nullptr; // do NOT save in this routine

	m_pForeListWidget->setEnabled(it);
	m_pBackListWidget->setEnabled(it);
	m_pEnableLogging->setEnabled(it);
	m_pIconButton->setEnabled(it);
	m_pLevelListWidget->setEnabled(it);

	if(it)
	{
		int back = ((MessageListWidgetItem *)it)->msgType()->back();
		int fore = ((MessageListWidgetItem *)it)->msgType()->fore();
		if(fore >= 0 && fore <= 15)
		{
			m_pForeListWidget->setCurrentItem(m_pForeItems[fore]);
		}
		if(back >= 0 && back <= 15)
		{
			m_pBackListWidget->setCurrentItem(m_pBackItems[back]);
		}
		else
		{
			m_pBackListWidget->setCurrentItem(m_pBackItems[16]);
		}
		m_pLevelListWidget->setCurrentRow(((MessageListWidgetItem *)it)->msgType()->level());
		m_pEnableLogging->setChecked(((MessageListWidgetItem *)it)->msgType()->logEnabled());
		m_pIconButton->setIcon(*(g_pIconManager->getSmallIcon(((MessageListWidgetItem *)it)->msgType()->pixId())));
	}

	// Ok...can save from now on
	m_pLastItem = (MessageListWidgetItem *)it;
}

void OptionsWidget_messageColors::colorChanged()
{
	if(m_pLastItem)
		saveLastItem();
}

void OptionsWidget_messageColors::iconButtonClicked()
{
	m_pIconPopup->popup(QCursor::pos());
}

void OptionsWidget_messageColors::commit()
{
	saveLastItem();

	mergeResetFlag(KviOption_resetUpdateGui);

	int count = m_pListView->count();
	for(int i = 0; i < count; i++)
	{
		MessageListWidgetItem * it = (MessageListWidgetItem *)m_pListView->item(i);
		KVI_OPTION_MSGTYPE(it->optionId()) = *(it->msgType());
	}

	KviOptionsWidget::commit();
}

void OptionsWidget_messageColors::save()
{
	QString szName;
	QString szInit;
	g_pApp->getLocalKvircDirectory(szInit, KviApplication::MsgColors);

	if(KviFileDialog::askForSaveFileName(szName, __tr2qs_ctx("Enter a Filename - KVIrc", "options"), szInit, QString(), false, false, true, this))
	{
		if(m_pLastItem)
			saveLastItem();

		KviConfigurationFile cfg(szName, KviConfigurationFile::Write);

		cfg.setGroup("Messages");

		KviCString tmp;

		int count = m_pListView->count();
		for(int i = 0; i < count; i++)
		{
			MessageListWidgetItem * it = (MessageListWidgetItem *)m_pListView->item(i);

			tmp.sprintf("Fore%d", it->optionId());
			cfg.writeEntry(tmp.ptr(), it->msgType()->fore());
			tmp.sprintf("Back%d", it->optionId());
			cfg.writeEntry(tmp.ptr(), it->msgType()->back());
			tmp.sprintf("Icon%d", it->optionId());
			cfg.writeEntry(tmp.ptr(), it->msgType()->pixId());
			tmp.sprintf("Log%d", it->optionId());
			cfg.writeEntry(tmp.ptr(), it->msgType()->logEnabled());
			tmp.sprintf("Level%d", it->optionId());
			cfg.writeEntry(tmp.ptr(), it->msgType()->level());
		}
	}
}

//#include <errno.h>

void OptionsWidget_messageColors::load()
{

	QString szName;
	QString szInit;
	g_pApp->getLocalKvircDirectory(szInit, KviApplication::MsgColors);

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	// symlink local to global
	QString szGlobal;
	g_pApp->getGlobalKvircDirectory(szGlobal, KviApplication::MsgColors);
	QString szLocal;
	g_pApp->getLocalKvircDirectory(szLocal, KviApplication::MsgColors, "presets");
	//qDebug("SYMLINKING %s to %s",szGlobal.ptr(),szLocal.ptr());
	//qDebug("SYMLINK RETURNS %d (%d)",::symlink(szGlobal.ptr(),szLocal.ptr()));
	//qDebug("ERRNO (%d)",errno);
	(void)symlink(szGlobal.toLocal8Bit().data(), szLocal.toLocal8Bit().data());
// FIXME: Do it also on windows...
#endif

	if(KviFileDialog::askForOpenFileName(szName, __tr2qs_ctx("Select a File - KVIrc", "options"), szInit, QString(), false, true, this))
	{
		itemChanged();

		KviConfigurationFile cfg(szName, KviConfigurationFile::Read);

		cfg.setGroup("Messages");

		//KviCString tmp;
		QString tmp;

		int count = m_pListView->count();
		for(int i = 0; i < count; i++)
		{
			MessageListWidgetItem * it = (MessageListWidgetItem *)m_pListView->item(i);

			tmp.sprintf("Fore%d", it->optionId());
			int fore = cfg.readIntEntry(tmp, it->msgType()->fore());
			if(fore < 0 || fore > 15)
				fore = 0;
			it->msgType()->setFore(fore);

			tmp.sprintf("Back%d", it->optionId());
			int back = cfg.readIntEntry(tmp, it->msgType()->back());
			if(back < 0 || back > 15)
				back = KviControlCodes::Transparent;
			it->msgType()->setBack(back);

			tmp.sprintf("Icon%d", it->optionId());
			int ico = cfg.readIntEntry(tmp, it->msgType()->pixId());
			if(ico < 0 || ico >= KviIconManager::IconCount)
				ico = 0;
			it->msgType()->setPixId(ico);

			tmp.sprintf("Log%d", it->optionId());
			bool bLog = cfg.readBoolEntry(tmp, it->msgType()->logEnabled());
			it->msgType()->enableLogging(bLog);

			tmp.sprintf("Level%d", it->optionId());
			int iLevel = cfg.readIntEntry(tmp, it->msgType()->level());
			it->msgType()->setLevel(iLevel);

			m_pListView->repaint(m_pListView->visualItemRect(it));
		}
	}
}
