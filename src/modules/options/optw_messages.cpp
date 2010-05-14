//=============================================================================
//
//   File : optw_messages.cpp
//   Creation date : Sat Aug 11 2001 18:12:00 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#define _WANT_OPTION_FLAGS_

#include "optw_messages.h"

#include "kvi_settings.h"
#include "kvi_options.h"
#include "kvi_app.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_mirccntrl.h"
#include "kvi_config.h"
#include "kvi_filedialog.h"
#include "kvi_tal_vbox.h"

#include <QLayout>
#include <QPushButton>
#include <QCursor>
#include <QPainter>

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	#include <unistd.h> // symlink()
#endif


#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern KVIRC_API QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif


KviMessageOptionsWidget::KviMessageOptionsWidget(QWidget * pParent)
: KviOptionsWidget(pParent)
{
}

KviMessageOptionsWidget::~KviMessageOptionsWidget()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviPrivmsgOptionsWidget::KviPrivmsgOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("privmsg_options_widget");
	createLayout();

	KviTalGroupBox * g = addGroupBox(0,0,0,0,Qt::Horizontal,__tr2qs_ctx("General","options"));

	addBoolSelector(g,__tr2qs_ctx("Show message icons","options"),KviOption_boolIrcViewShowImages);
	addBoolSelector(g,__tr2qs_ctx("Draw some emoticons (smileys) as pictures","options"),KviOption_boolDrawEmoticons);
	addBoolSelector(g,__tr2qs_ctx("Don't show colors in user messages","options"),KviOption_boolStripMircColorsInUserMessages);

	g = addGroupBox(0,1,0,1,Qt::Horizontal,__tr2qs_ctx("Nicknames","options"));

	m_pUseSmartColorSelector = addBoolSelector(g,__tr2qs_ctx("\"Smart\" nickname colors","options"),KviOption_boolColorNicks);

	KviTalHBox * hb = new KviTalHBox(g);
	hb->setSpacing(4);
	m_pSpecialSmartColorSelector = addBoolSelector(hb,__tr2qs_ctx("Use specified colors for own nick:","options"),KviOption_boolUseSpecifiedSmartColorForOwnNick,KVI_OPTION_BOOL(KviOption_boolColorNicks));

	m_pSmartColorSelector = addMircTextColorSelector(hb,"",KviOption_uintUserIrcViewOwnForeground,KviOption_uintUserIrcViewOwnBackground,KVI_OPTION_BOOL(KviOption_boolColorNicks) && KVI_OPTION_BOOL(KviOption_boolUseSpecifiedSmartColorForOwnNick));

	connect(m_pSpecialSmartColorSelector,SIGNAL(toggled(bool)),this,SLOT(enableDisableSmartColorSelector(bool)));
	connect(m_pUseSmartColorSelector,SIGNAL(toggled(bool)),m_pSpecialSmartColorSelector,SLOT(setEnabled(bool)));

	KviBoolSelector * b2 = addBoolSelector(g,__tr2qs_ctx("Use same colors as in the userlist","options"),KviOption_boolUseUserListColorsAsNickColors,!KVI_OPTION_BOOL(KviOption_boolColorNicks));
	connect(m_pUseSmartColorSelector,SIGNAL(toggled(bool)),b2,SLOT(setNotEnabled(bool)));

	addBoolSelector(g,__tr2qs_ctx("Show nicknames in bold","options"),KviOption_boolBoldedNicks);
	addBoolSelector(g,__tr2qs_ctx("Show user and host","options"),KviOption_boolShowUserAndHostInPrivmsgView);
	addBoolSelector(g,__tr2qs_ctx("Show channel mode prefix","options"),KviOption_boolShowChannelUserFlagInPrivmsgView);
	KviBoolSelector * b = addBoolSelector(g,__tr2qs_ctx("User-defined prefix and postfix","options"),KviOption_boolUseExtendedPrivmsgView);

	QLabel * l = addLabel(g,__tr2qs_ctx("[PREFIX]nickname[!user@host][POSTFIX] message","options"));

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
		addStringSelector(vb,__tr2qs_ctx("Prefix:","options"),KviOption_stringExtendedPrivmsgPrefix,KVI_OPTION_BOOL(KviOption_boolUseExtendedPrivmsgView)),
		SLOT(setEnabled(bool)));
	connect(
		b,
		SIGNAL(toggled(bool)),
		addStringSelector(vb,__tr2qs_ctx("Postfix:","options"),KviOption_stringExtendedPrivmsgPostfix,KVI_OPTION_BOOL(KviOption_boolUseExtendedPrivmsgView)),
		SLOT(setEnabled(bool)));
	addRowSpacer(0,3,0,3);
}

void KviPrivmsgOptionsWidget::enableDisableSmartColorSelector(bool)
{
	m_pSmartColorSelector->setEnabled(m_pUseSmartColorSelector->isChecked() && m_pUseSmartColorSelector->isChecked());
}

KviPrivmsgOptionsWidget::~KviPrivmsgOptionsWidget()
{
}

KviTimestampOptionsWidget::KviTimestampOptionsWidget(QWidget * pParent)
: KviOptionsWidget(pParent)
{
	createLayout();
	m_pUseTimestampSelector = addBoolSelector(0,0,0,0,__tr2qs_ctx("Show timestamp","options"),KviOption_boolIrcViewTimestamp);

	connect(m_pUseTimestampSelector,SIGNAL(toggled(bool)),this,SLOT(enableDisableTimestampSelector(bool)));
	KviBoolSelector* b = addBoolSelector(0,1,0,1,__tr2qs_ctx("Use UTC time for timestamp","options"),KviOption_boolIrcViewTimestampUTC,KVI_OPTION_BOOL(KviOption_boolIrcViewTimestamp));

	connect(m_pUseTimestampSelector,SIGNAL(toggled(bool)),b,SLOT(setEnabled(bool)));

	KviTalHBox *hb = new KviTalHBox(this);
	addWidgetToLayout(hb,0,2,0,2);

	m_pSpecialTimestampColorSelector = addBoolSelector(hb,__tr2qs_ctx("Use special color for timestamps","options"),KviOption_boolUseSpecialColorForTimestamp,KVI_OPTION_BOOL(KviOption_boolIrcViewTimestamp));
	connect(m_pSpecialTimestampColorSelector,SIGNAL(toggled(bool)),this,SLOT(enableDisableTimestampSelector(bool)));
	connect(m_pUseTimestampSelector,SIGNAL(toggled(bool)),m_pSpecialTimestampColorSelector,SLOT(setEnabled(bool)));

	m_pTimestampColorSelector = addMircTextColorSelector(hb,"",KviOption_uintTimeStampForeground,KviOption_uintTimeStampBackground,KVI_OPTION_BOOL(KviOption_boolIrcViewTimestamp) && KVI_OPTION_BOOL(KviOption_boolUseSpecialColorForTimestamp));

	KviStringSelector * st=addStringSelector(0,3,0,3,__tr2qs_ctx("Timestamp format:","options"),KviOption_stringIrcViewTimestampFormat);
	connect(m_pUseTimestampSelector,SIGNAL(toggled(bool)),st,SLOT(setEnabled(bool)));

	connect(m_pUseTimestampSelector,SIGNAL(toggled(bool)),m_pSpecialTimestampColorSelector,SLOT(setEnabled(bool)));
	addRowSpacer(0,4,0,4);
}

KviTimestampOptionsWidget::~KviTimestampOptionsWidget()
{
}

void KviTimestampOptionsWidget::enableDisableTimestampSelector(bool)
{
	m_pTimestampColorSelector->setEnabled(m_pUseTimestampSelector->isChecked() && m_pSpecialTimestampColorSelector->isChecked());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviStandardColorsOptionsWidget::KviStandardColorsOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("stdcolors");
	createLayout();

	addColorSelector(0,0,0,0,"0:",&(KVI_OPTION_MIRCCOLOR(0)));
	addColorSelector(1,0,1,0,"1:",&(KVI_OPTION_MIRCCOLOR(1)));
	addColorSelector(2,0,2,0,"2:",&(KVI_OPTION_MIRCCOLOR(2)));
	addColorSelector(3,0,3,0,"3:",&(KVI_OPTION_MIRCCOLOR(3)));
	addColorSelector(0,1,0,1,"4:",&(KVI_OPTION_MIRCCOLOR(4)));
	addColorSelector(1,1,1,1,"5:",&(KVI_OPTION_MIRCCOLOR(5)));
	addColorSelector(2,1,2,1,"6:",&(KVI_OPTION_MIRCCOLOR(6)));
	addColorSelector(3,1,3,1,"7:",&(KVI_OPTION_MIRCCOLOR(7)));
	addColorSelector(0,2,0,2,"8:",&(KVI_OPTION_MIRCCOLOR(8)));
	addColorSelector(1,2,1,2,"9:",&(KVI_OPTION_MIRCCOLOR(9)));
	addColorSelector(2,2,2,2,"10:",&(KVI_OPTION_MIRCCOLOR(10)));
	addColorSelector(3,2,3,2,"11:",&(KVI_OPTION_MIRCCOLOR(11)));
	addColorSelector(0,3,0,3,"12:",&(KVI_OPTION_MIRCCOLOR(12)));
	addColorSelector(1,3,1,3,"13:",&(KVI_OPTION_MIRCCOLOR(13)));
	addColorSelector(2,3,2,3,"14:",&(KVI_OPTION_MIRCCOLOR(14)));
	addColorSelector(3,3,3,3,"15:",&(KVI_OPTION_MIRCCOLOR(15)));

	addRowSpacer(0,4,3,4);

	layout()->setRowStretch(4,1);
}

KviStandardColorsOptionsWidget::~KviStandardColorsOptionsWidget()
{
}

KviMessageListWidgetItem::KviMessageListWidgetItem(KviMessageListWidget* l,int optId)
: KviTalListWidgetItem(l)
{
	m_iOptId = optId;
	m_pMsgType = new KviMsgType(KVI_OPTION_MSGTYPE(optId));

	QString txt = g_msgtypeOptionsTable[m_iOptId].name;
	// skip the msgtype prefix
	txt.remove(0,KVI_MSGTYPE_OPTIONS_PREFIX_LEN);
	txt += " (";
	txt += __tr2qs_no_xgettext(m_pMsgType->type());
	txt += ")";

	setText(txt);
	setIcon(*(g_pIconManager->getSmallIcon(m_pMsgType->pixId())));
}

KviMessageListWidgetItem::~KviMessageListWidgetItem()
{
	delete m_pMsgType;
}

void KviMessageListWidgetItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & opt, const QModelIndex & index) const
{

	const KviTalListWidget *tb = (const KviTalListWidget *)parent();
	KviMessageListWidgetItem *it = static_cast<KviMessageListWidgetItem*>(index.internalPointer());
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p->save();
		p->setCompositionMode(QPainter::CompositionMode_Source);
		QColor col=KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p->fillRect(opt.rect, col);
		p->restore();
	} else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = tb->viewport()->mapToGlobal(opt.rect.topLeft());
		p->drawTiledPixmap(opt.rect,*g_pShadedChildGlobalDesktopBackground, pnt);
	} else {
#endif
		QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapIrcViewBackground).pixmap();
		if(pix)
		{
			QPoint pnt = tb->viewport()->mapToGlobal(opt.rect.topLeft());
			p->drawTiledPixmap(opt.rect,*pix,pnt);
		} else {
			p->fillRect(opt.rect,KVI_OPTION_COLOR(KviOption_colorIrcViewBackground));
		}
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	QString szText=index.data(Qt::DisplayRole).toString();
	QPoint pt = opt.rect.topLeft();

	pt.setX(pt.x() + 4);
	p->drawPixmap(pt,*(g_pIconManager->getSmallIcon(it->msgType()->pixId())));
	pt.setX(pt.x() + 18);
	// draw the background
	if(it->msgType()->back() < 16)
	{
		QColor bColor = KVI_OPTION_MIRCCOLOR(it->msgType()->back());
		p->fillRect(pt.x(), pt.y(), opt.rect.width() - pt.x(), opt.rect.height(), bColor);
	}
	unsigned char ucFore = it->msgType()->fore();
	if(ucFore > 15)ucFore = 0;
	p->setPen(QPen(KVI_OPTION_MIRCCOLOR(ucFore)));
	pt.setX(pt.x() + 2);

	p->drawText(pt.x(), pt.y(), opt.rect.width() - pt.x(), opt.rect.height(),Qt::AlignLeft | Qt::AlignVCenter,szText);

	if(opt.state & QStyle::State_Selected)
	{
		QPen pen(Qt::white);
		p->setPen(pen);
		p->drawRect(opt.rect.adjusted(0,0,-1,-1));
		p->drawRect(opt.rect.adjusted (1,1,-2,-2));

		pen.setColor(Qt::black);
		pen.setStyle(Qt::DashLine);
		p->setPen(pen);
		p->drawRect(opt.rect.adjusted(0,0,-1,-1));
		p->drawRect(opt.rect.adjusted (1,1,-2,-2));
	}
}


KviMessageColorListWidgetItem::KviMessageColorListWidgetItem(KviTalListWidget * b,int idx)
: KviTalListWidgetText(b,QString())
{

	m_iClrIdx = idx;
	if((idx < 0) || (idx > 15))
	{
		setText(__tr2qs_ctx("Transparent","options"));
		setBackground(listWidget()->isEnabled() ? Qt::transparent : Qt::gray);
	} else {
		setBackground(QColor(KVI_OPTION_MIRCCOLOR(m_iClrIdx)));
		setText(" ");
	}
}

KviMessageColorListWidgetItem::~KviMessageColorListWidgetItem()
{
}

void KviMessageColorListWidgetItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & opt, const QModelIndex & index) const
{

	if(opt.state & QStyle::State_Selected)
	{
		QColor clr;
		const KviTalListWidget *lb = (const KviTalListWidget *)parent();
		KviMessageColorListWidgetItem *it = static_cast<KviMessageColorListWidgetItem*>(index.internalPointer());

		if((it->clrIdx() >= 0) && (it->clrIdx() <= 15))
		{
			clr = KVI_OPTION_MIRCCOLOR(it->clrIdx());
		} else {
			clr =lb->palette().color(QPalette::Window);
		}

		QStyleOptionViewItem optz(opt);
		optz.state = opt.state ^ QStyle::State_Selected;
		QItemDelegate::paint(p, optz, index);

		QPen pen(QColor(255-clr.red(), 255-clr.green(), 255-clr.blue()));
		pen.setStyle(Qt::DashLine);
		p->setPen(pen);
		p->drawRect(opt.rect.adjusted(0,0,-1,-1));
		p->drawRect(opt.rect.adjusted (1,1,-2,-2));
		p->drawRect(opt.rect.adjusted (2,2,-3,-3));
	} else {
		QItemDelegate::paint(p, opt, index);
	}
}

KviMessageColorsOptionsWidget::KviMessageColorsOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("messages");

	createLayout();

	int i;
	m_pLastItem = 0;

	m_pListView = new KviMessageListWidget(this);
	m_pListViewItemDelegate = new KviMessageListWidgetItemDelegate(m_pListView);
	m_pListView->setItemDelegate(m_pListViewItemDelegate);
	m_pListView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pListView->setFont(KVI_OPTION_FONT(KviOption_fontIrcView));
	m_pListView->viewport()->setAutoFillBackground(false);

	addWidgetToLayout(m_pListView,0,0,2,0);

	KviTalVBox * box = new KviTalVBox(this);
	addWidgetToLayout(box,3,0,3,0);

	QLabel * l = new QLabel(__tr2qs_ctx("Background:","options"),box);

	m_pBackListWidget = new KviTalListWidget(box);
	m_pBackListWidget->setMaximumWidth(150);
	m_pBackListWidgetDelegate = new KviMessageColorListWidgetItemDelegate(m_pBackListWidget);
	m_pBackListWidget->setItemDelegate(m_pBackListWidgetDelegate);

	m_pBackItems[16] = new KviMessageColorListWidgetItem(m_pBackListWidget,KVI_TRANSPARENT);
	for(i=0;i<16;i++)
	{
		m_pBackItems[i] = new KviMessageColorListWidgetItem(m_pBackListWidget,i);
	}

	l = new QLabel(__tr2qs_ctx("Foreground:","options"),box);

	m_pForeListWidget = new KviTalListWidget(box);
	m_pForeListWidget->setMaximumWidth(150);
	m_pForeListWidgetDelegate = new KviMessageColorListWidgetItemDelegate(m_pForeListWidget);
	m_pForeListWidget->setItemDelegate(m_pForeListWidgetDelegate);

	for(i=0;i<16;i++)
	{
		m_pForeItems[i] = new KviMessageColorListWidgetItem(m_pForeListWidget,i);
	}

	l = new QLabel(__tr2qs_ctx("Alert level:","options"),box);

	m_pLevelListWidget = new KviTalListWidget(box);
	m_pLevelListWidget->setMaximumWidth(150);
	KviTalListWidgetText * lbt;
	for(i=0;i<6;i++)
	{
		QString tmpn;
		tmpn.setNum(i);
		lbt = new KviTalListWidgetText(m_pLevelListWidget,tmpn);
	}

	m_pIconButton = new QToolButton(box);
	connect(m_pIconButton,SIGNAL(clicked()),this,SLOT(iconButtonClicked()));

	m_pIconPopup = new KviTalPopupMenu(this);
	KviIconWidget * iw = new KviIconWidget(m_pIconPopup);
	connect(iw,SIGNAL(selected(int)),this,SLOT(newIconSelected(int)));
	m_pIconPopup->insertItem(iw);


	m_pEnableLogging = new QCheckBox(__tr2qs_ctx("Log this","options"),box);

	KviTalHBox * h = new KviTalHBox(this);
	addWidgetToLayout(h,0,1,3,1);
	QPushButton * b = new QPushButton(__tr2qs_ctx("Load From...","options"),h);
	connect(b,SIGNAL(clicked()),this,SLOT(load()));
	b = new QPushButton(__tr2qs_ctx("Save As...","options"),h);
	connect(b,SIGNAL(clicked()),this,SLOT(save()));


	KviMessageListWidgetItem * it;

	for(i=0;i<KVI_NUM_MSGTYPE_OPTIONS;i++)
	{
		it = new KviMessageListWidgetItem(m_pListView,i);
	}

	layout()->setRowStretch(0,1);
	layout()->setColumnStretch(0,1);

	connect(m_pListView,SIGNAL(itemSelectionChanged ()),this,SLOT(itemChanged()));
	connect(m_pForeListWidget,SIGNAL(itemSelectionChanged ()),this,SLOT(colorChanged()));
	connect(m_pBackListWidget,SIGNAL(itemSelectionChanged ()),this,SLOT(colorChanged()));

	itemChanged();
}

KviMessageColorsOptionsWidget::~KviMessageColorsOptionsWidget()
{
	int i;
	for(i=15;i>=0;i--)
	{
		delete m_pForeItems[i];
	}
	for(i=16;i>=0;i--)
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

void KviMessageColorsOptionsWidget::newIconSelected(int iconId)
{
	if(!m_pLastItem)return;
	if(iconId >= KVI_NUM_SMALL_ICONS)return;
	m_pLastItem->msgType()->setPixId(iconId);
	m_pIconButton->setIcon(*(g_pIconManager->getSmallIcon(iconId)));
	m_pListView->repaint(m_pListView->visualItemRect(m_pLastItem));
}

void KviMessageColorsOptionsWidget::saveLastItem()
{
	if(!m_pLastItem)return;

	int curIt = m_pForeListWidget->currentRow();
	if(curIt >= 0) //quite useless
	{
		//qDebug("Setting fore %d",curIt);
		KviMessageColorListWidgetItem * fore = (KviMessageColorListWidgetItem *)m_pForeListWidget->item(curIt);
		//qDebug("And is %d",fore);
		if(fore)m_pLastItem->msgType()->setFore(fore->m_iClrIdx);
	}
	curIt = m_pBackListWidget->currentRow();
	if(curIt >= 0) //quite useless
	{
		KviMessageColorListWidgetItem * back = (KviMessageColorListWidgetItem *)m_pBackListWidget->item(curIt);
		if(back)m_pLastItem->msgType()->setBack(back->m_iClrIdx);
	}
	m_pLastItem->msgType()->enableLogging(m_pEnableLogging->isChecked());
	//qDebug("Updating","options");
	curIt = m_pLevelListWidget->currentRow();
	if(curIt < 0 || curIt > 5)curIt = 1;
	m_pLastItem->msgType()->setLevel(curIt);
	m_pListView->repaint(m_pListView->visualItemRect(m_pLastItem));
}

void KviMessageColorsOptionsWidget::itemChanged()
{
	KviTalListWidgetItem * it = (KviTalListWidgetItem*)m_pListView->currentItem();
	//qDebug("Item changed","options");
	if(m_pLastItem)saveLastItem();

	m_pLastItem = 0; // do NOT save in this routine

	m_pForeListWidget->setEnabled(it);
	m_pBackListWidget->setEnabled(it);
	m_pEnableLogging->setEnabled(it);
	m_pIconButton->setEnabled(it);
	m_pLevelListWidget->setEnabled(it);

	if(it)
	{
		int back = ((KviMessageListWidgetItem *)it)->msgType()->back();
		int fore = ((KviMessageListWidgetItem *)it)->msgType()->fore();
		if(fore >= 0 && fore <= 15)
		{
			m_pForeListWidget->setCurrentItem(m_pForeItems[fore]);
		}
		if(back >= 0 && back <= 15)
		{
			m_pBackListWidget->setCurrentItem(m_pBackItems[back]);
		} else {
			m_pBackListWidget->setCurrentItem(m_pBackItems[16]);
		}
		m_pLevelListWidget->setCurrentRow(((KviMessageListWidgetItem *)it)->msgType()->level());
		m_pEnableLogging->setChecked(((KviMessageListWidgetItem *)it)->msgType()->logEnabled());
		m_pIconButton->setIcon(*(g_pIconManager->getSmallIcon(((KviMessageListWidgetItem *)it)->msgType()->pixId())));
	}

	// Ok...can save from now on
	m_pLastItem = (KviMessageListWidgetItem *)it;
}

void KviMessageColorsOptionsWidget::colorChanged()
{
	if(m_pLastItem)saveLastItem();
}

void KviMessageColorsOptionsWidget::iconButtonClicked()
{
	m_pIconPopup->popup(QCursor::pos());
}

void KviMessageColorsOptionsWidget::commit()
{
	saveLastItem();

	mergeResetFlag(KviOption_resetUpdateGui);

	int count = m_pListView->count();
	for(int i=0; i<count;i++)
	{
		KviMessageListWidgetItem* it = (KviMessageListWidgetItem *)m_pListView->item(i);
		KVI_OPTION_MSGTYPE(it->optionId()) = *(it->msgType());
	}

	KviOptionsWidget::commit();
}

void KviMessageColorsOptionsWidget::save()
{
	QString szName;
	QString szInit;
	g_pApp->getLocalKvircDirectory(szInit,KviApp::MsgColors);

	if(KviFileDialog::askForSaveFileName(szName,__tr2qs_ctx("Choose a Filename - KVIrc","options"),szInit))
	{
		if(m_pLastItem)saveLastItem();

		KviConfig cfg(szName,KviConfig::Write);

		cfg.setGroup("Messages");

		KviStr tmp;


		int count = m_pListView->count();
		for(int i=0; i<count;i++)
		{
			KviMessageListWidgetItem* it = (KviMessageListWidgetItem *)m_pListView->item(i);

			tmp.sprintf("Fore%d",it->optionId());
			cfg.writeEntry(tmp.ptr(),it->msgType()->fore());
			tmp.sprintf("Back%d",it->optionId());
			cfg.writeEntry(tmp.ptr(),it->msgType()->back());
			tmp.sprintf("Icon%d",it->optionId());
			cfg.writeEntry(tmp.ptr(),it->msgType()->pixId());
			tmp.sprintf("Log%d",it->optionId());
			cfg.writeEntry(tmp.ptr(),it->msgType()->logEnabled());
			tmp.sprintf("Level%d",it->optionId());
			cfg.writeEntry(tmp.ptr(),it->msgType()->level());
		}

	}
}

//#include <errno.h>

void KviMessageColorsOptionsWidget::load()
{

	QString szName;
	QString szInit;
	g_pApp->getLocalKvircDirectory(szInit,KviApp::MsgColors);

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	// symlink local to global
	QString szGlobal;
	g_pApp->getGlobalKvircDirectory(szGlobal,KviApp::MsgColors);
	QString szLocal;
	g_pApp->getLocalKvircDirectory(szLocal,KviApp::MsgColors,"presets");
	//qDebug("SYMLINKING %s to %s",szGlobal.ptr(),szLocal.ptr());
	//qDebug("SYMLINK RETURNS %d (%d)",::symlink(szGlobal.ptr(),szLocal.ptr()));
	//qDebug("ERRNO (%d)",errno);
	int dummy; // make gcc happy
	dummy = symlink(szGlobal.toLocal8Bit().data(),szLocal.toLocal8Bit().data());
	// FIXME: Do it also on windows...
#endif

	if(KviFileDialog::askForOpenFileName(szName,__tr2qs_ctx("Choose a Filename - KVIrc ","options"),szInit))
	{
		itemChanged();

		KviConfig cfg(szName,KviConfig::Read);

		cfg.setGroup("Messages");

		//KviStr tmp;
		QString tmp;

		int count = m_pListView->count();
		for(int i=0; i<count;i++)
		{
			KviMessageListWidgetItem* it = (KviMessageListWidgetItem *)m_pListView->item(i);

			tmp.sprintf("Fore%d",it->optionId());
			int fore = cfg.readIntEntry(tmp,it->msgType()->fore());
			if(fore < 0 || fore > 15)fore = 0;
			it->msgType()->setFore(fore);

			tmp.sprintf("Back%d",it->optionId());
			int back = cfg.readIntEntry(tmp,it->msgType()->back());
			if(back < 0 || back > 15)back = KVI_TRANSPARENT;
			it->msgType()->setBack(back);

			tmp.sprintf("Icon%d",it->optionId());
			int ico = cfg.readIntEntry(tmp,it->msgType()->pixId());
			if(ico < 0 || ico >= KVI_NUM_SMALL_ICONS)ico = 0;
			it->msgType()->setPixId(ico);

			tmp.sprintf("Log%d",it->optionId());
			bool bLog = cfg.readBoolEntry(tmp,it->msgType()->logEnabled());
			it->msgType()->enableLogging(bLog);

			tmp.sprintf("Level%d",it->optionId());
			int iLevel = cfg.readIntEntry(tmp,it->msgType()->level());
			it->msgType()->setLevel(iLevel);

			m_pListView->repaint(m_pListView->visualItemRect(it));
		}
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_messages.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
