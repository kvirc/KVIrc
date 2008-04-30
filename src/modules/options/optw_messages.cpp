//=============================================================================
//
//   File : optw_messages.cpp
//   Creation date : Sat Aug 11 2001 18:12:00 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2004 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_styled_controls.h"
#include "kvi_tal_vbox.h"

#include <QLayout>
#include <QPushButton>
#include <QCursor>
#include <QPainter>

#ifndef COMPILE_ON_WINDOWS
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviPrivmsgOptionsWidget::KviPrivmsgOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"privmsg_options_widget")
{
	createLayout();

	KviBoolSelector * b1;
	KviBoolSelector * b2;

	KviTalGroupBox * g = addGroupBox(0,0,0,0,1,Qt::Horizontal,__tr2qs_ctx("General","options"));
	
	addBoolSelector(g,__tr2qs_ctx("Show message icons","options"),KviOption_boolIrcViewShowImages);
	addBoolSelector(g,__tr2qs_ctx("Draw some emoticons (smileys) as pictures","options"),KviOption_boolDrawEmoticons);
	addBoolSelector(g,__tr2qs_ctx("Don't show colors in user messages","options"),KviOption_boolStripMircColorsInUserMessages);

	g = addGroupBox(0,1,0,1,1,Qt::Horizontal,__tr2qs_ctx("Nicknames","options"));

	b1 = addBoolSelector(g,__tr2qs_ctx("\"Smart\" nickname colors","options"),KviOption_boolColorNicks);
	b2 = addBoolSelector(g,__tr2qs_ctx("Use same colors as in the userlist","options"),KviOption_boolUseUserListColorsAsNickColors,!KVI_OPTION_BOOL(KviOption_boolColorNicks));
	connect(b1,SIGNAL(toggled(bool)),b2,SLOT(setNotEnabled(bool)));
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
: KviOptionsWidget(parent,"stdcolors")
{
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


KviMessageListView::KviMessageListView(QWidget * par)
: KviTalListView(par)
{
	setItemMargin(2);
}

KviMessageListView::~KviMessageListView()
{
}


void KviMessageListView::paintEmptyAreaInternal(QPainter * p,const QRect &viewportRect,const QRect &painterRect)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = viewport()->mapToGlobal(viewportRect.topLeft());
		p->drawTiledPixmap(painterRect.x(),painterRect.y(),painterRect.width(),painterRect.height(),*g_pShadedChildGlobalDesktopBackground,pnt.x(),pnt.y());
	} else {
#endif
		QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapIrcViewBackground).pixmap();
		if(pix)
		{
			QPoint pnt = viewportToContents(viewportRect.topLeft());
			p->fillRect(painterRect.x(),painterRect.y(),painterRect.width(),painterRect.height(),KVI_OPTION_COLOR(KviOption_colorIrcViewBackground));
			p->drawTiledPixmap(painterRect.x(),painterRect.y(),painterRect.width(),painterRect.height(),*pix,pnt.x(),pnt.y());
		} else {
			p->fillRect(painterRect.x(),painterRect.y(),painterRect.width(),painterRect.height(),KVI_OPTION_COLOR(KviOption_colorIrcViewBackground));
		}
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif
}

void KviMessageListView::paintEmptyArea(QPainter * p,const QRect &rct)
{
	paintEmptyAreaInternal(p,rct,rct);
	KviTalListView::paintEmptyArea(p,rct);
}


KviMessageListViewItem::KviMessageListViewItem(KviTalListView * l,int optId)
: KviTalListViewItem(l)
{
	m_iOptId = optId;
	setText(0,"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW");
	m_pMsgType = new KviMsgType(KVI_OPTION_MSGTYPE(optId));
	setPixmap(0,*(g_pIconManager->getSmallIcon(0))); // just a dummy one to ensure that the size of the item is at least 20 pixels (16 + 2 * margin)
}

KviMessageListViewItem::~KviMessageListViewItem()
{
	delete m_pMsgType;
}

void KviMessageListViewItem::paintCell(QPainter * p,const QColorGroup &,int,int w,int)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = listView()->viewport()->mapToGlobal(QPoint(int(p->worldMatrix().dx()),int(p->worldMatrix().dy())));
		p->drawTiledPixmap(0,0,w,height(),*g_pShadedChildGlobalDesktopBackground,pnt.x(),pnt.y());
	} else {
#endif
		QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapIrcViewBackground).pixmap();
		if(pix)
		{
			QPoint pnt = listView()->viewportToContents(QPoint(int(p->worldMatrix().dx()),int(p->worldMatrix().dy())));
			p->fillRect(0,0,w,height(),KVI_OPTION_COLOR(KviOption_colorIrcViewBackground));
			p->drawTiledPixmap(0,0,w,height(),*pix,pnt.x(),pnt.y());
		} else {
			p->fillRect(0,0,w,height(),KVI_OPTION_COLOR(KviOption_colorIrcViewBackground));
		}
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif


	p->drawPixmap(listView()->itemMargin(),(height() - 16) / 2,*(g_pIconManager->getSmallIcon(m_pMsgType->pixId())));
	// draw the background
	if(m_pMsgType->back() < 16)
	{
		QColor bColor = KVI_OPTION_MIRCCOLOR(m_pMsgType->back());
		p->fillRect(22,listView()->itemMargin(),w - 24,height() - (listView()->itemMargin() * 2),bColor);
	}
	unsigned char ucFore = m_pMsgType->fore();
	if(ucFore > 15)ucFore = 0;
	p->setPen(QPen(KVI_OPTION_MIRCCOLOR(ucFore)));
	// skip the msgtype prefix
	QString txt = g_msgtypeOptionsTable[m_iOptId].name.remove(0,7);
	txt += " (";
	txt += __tr2qs_no_xgettext(m_pMsgType->type());
	txt += ")";
	p->drawText(24,listView()->itemMargin(),w - 24,height() - (listView()->itemMargin() * 2),Qt::AlignLeft | Qt::AlignVCenter,txt);
	if(isSelected())
	{
		QPen pen(KVI_OPTION_COLOR(KviOption_colorIrcViewBackground));
		pen.setStyle(Qt::DashLine);
		p->setPen( pen);
		p->drawRect(0,0,w,height());
		p->drawRect(1,1,w - 2,height() - 2);
	}
}


KviMessageColorListBoxItem::KviMessageColorListBoxItem(KviTalListBox * b,const QColor &clr,int idx)
: KviTalListBoxText(b,QString::null)
{
	m_clr = clr;
	m_iClrIdx = idx;
	if((idx < 0) || (idx > 15))setText(__tr2qs_ctx("Transparent","options"));
}

KviMessageColorListBoxItem::~KviMessageColorListBoxItem()
{
}

void KviMessageColorListBoxItem::paint(QPainter * p)
{
	QColor clr;

	const KviTalListBox * lb = (const KviTalListBox *)listBox();

	if((m_iClrIdx >= 0) && (m_iClrIdx <= 15))
	{
		clr = lb->isEnabled() ? KVI_OPTION_MIRCCOLOR(m_iClrIdx) : Qt::gray;
		p->fillRect(0,0,width(lb),height(lb),clr);
	} else {
		clr = listBox()->colorGroup().background();
		KviTalListBoxText::paint(p);
	}

	if(isSelected())
	{
		QPen pen(clr);
		pen.setStyle(Qt::DashLine);
		p->setPen( pen);
		p->drawRect(0,0,width(lb),height(lb));
		p->drawRect(1,1,width(lb) - 2,height(lb) - 2);
		p->drawRect(2,2,width(lb) - 4,height(lb) - 4);
	}
}
/*
int KviMessageColorListBoxItem::width(const KviTalListBox * lb) const
{
	int w = lb->width();
	if(w < 30)w = 30;
	if(w > 150)w = 150;
	return w;
}

int KviMessageColorListBoxItem::height(const KviTalListBox *) const 
{
	return 30;
}
*/

/*
KviIrcColorsOptionsWidget::KviIrcColorsOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"irccolors","options")
{
	createLayout();

	addRowSpacer(0,4,3,4);
}

KviIrcColorsOptionsWidget::~KviIrcColorsOptionsWidget()
{
}
*/

KviMessageColorsOptionsWidget::KviMessageColorsOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"messages")
{
	int i;

	createLayout();

	m_pLastItem = 0;


	m_pListView = new KviMessageListView(this);
	m_pListView->addColumn(__tr2qs_ctx("Message Type","options"));
	m_pListView->setAllColumnsShowFocus(true);
	m_pListView->setSelectionMode(KviTalListView::Single);
	m_pListView->setFont(KVI_OPTION_FONT(KviOption_fontIrcView));
	m_pListView->setStaticBackground(true);
	
	addWidgetToLayout(m_pListView,0,0,2,0);

	KviTalVBox * box = new KviTalVBox(this);
	addWidgetToLayout(box,3,0,3,0);

	QLabel * l = new QLabel(__tr2qs_ctx("Background:","options"),box);

	m_pBackListBox = new KviTalListBox(box);
	m_pBackItems[16] = new KviMessageColorListBoxItem(m_pBackListBox,Qt::gray,KVI_TRANSPARENT);
	for(i=0;i<16;i++)
	{
		m_pBackItems[i] = new KviMessageColorListBoxItem(m_pBackListBox,KVI_OPTION_MIRCCOLOR(i),i);
	}

	l = new QLabel(__tr2qs_ctx("Foreground:","options"),box);

	m_pForeListBox = new KviTalListBox(box);
	for(i=0;i<16;i++)
	{
		m_pForeItems[i] = new KviMessageColorListBoxItem(m_pForeListBox,KVI_OPTION_MIRCCOLOR(i),i);
	}

	l = new QLabel(__tr2qs_ctx("Alert level:","options"),box);

	m_pLevelListBox = new KviTalListBox(box);
	KviTalListBoxText * lbt;
	for(i=0;i<6;i++)
	{
		QString tmpn;
		tmpn.setNum(i);
		lbt = new KviTalListBoxText(m_pLevelListBox,tmpn);
	}

	m_pIconButton = new KviStyledToolButton(box);
	connect(m_pIconButton,SIGNAL(clicked()),this,SLOT(iconButtonClicked()));

	m_pIconPopup = new KviTalPopupMenu(this);
	KviIconWidget * iw = new KviIconWidget(m_pIconPopup);
	connect(iw,SIGNAL(selected(int)),this,SLOT(newIconSelected(int)));
	m_pIconPopup->insertItem(iw);


	m_pEnableLogging = new KviStyledCheckBox(__tr2qs_ctx("Log this","options"),box);

	KviTalHBox * h = new KviTalHBox(this);
	addWidgetToLayout(h,0,1,3,1);
	QPushButton * b = new QPushButton(__tr2qs_ctx("Load From...","options"),h);
	connect(b,SIGNAL(clicked()),this,SLOT(load()));
	b = new QPushButton(__tr2qs_ctx("Save As...","options"),h);
	connect(b,SIGNAL(clicked()),this,SLOT(save()));


	KviMessageListViewItem * it;

	for(i=0;i<KVI_NUM_MSGTYPE_OPTIONS;i++)
	{
		it = new KviMessageListViewItem(m_pListView,i);
	}

	layout()->setRowStretch(0,1);
	layout()->setColumnStretch(0,1);

	connect(m_pListView,SIGNAL(selectionChanged(KviTalListViewItem *)),this,SLOT(itemChanged(KviTalListViewItem *)));
	connect(m_pForeListBox,SIGNAL(selectionChanged(KviTalListBoxItem *)),this,SLOT(colorChanged(KviTalListBoxItem *)));
	connect(m_pBackListBox,SIGNAL(selectionChanged(KviTalListBoxItem *)),this,SLOT(colorChanged(KviTalListBoxItem *)));

	itemChanged(0);
}

KviMessageColorsOptionsWidget::~KviMessageColorsOptionsWidget()
{
}

void KviMessageColorsOptionsWidget::newIconSelected(int iconId)
{
	if(!m_pLastItem)return;
	if(iconId >= KVI_NUM_SMALL_ICONS)return;
	m_pLastItem->msgType()->setPixId(iconId);
	m_pIconButton->setIconSet(*(g_pIconManager->getSmallIcon(iconId)));
	m_pListView->repaintItem(m_pLastItem);
}

void KviMessageColorsOptionsWidget::saveLastItem()
{
	if(!m_pLastItem)return;

	int curIt = m_pForeListBox->currentItem();
	if(curIt != -1)
	{
		//qDebug("Setting fore %d",curIt);
		KviMessageColorListBoxItem * fore = (KviMessageColorListBoxItem *)m_pForeListBox->item(curIt);
		//qDebug("And is %d",fore);
		if(fore)m_pLastItem->msgType()->setFore(fore->m_iClrIdx);
	}
	curIt = m_pBackListBox->currentItem();
	if(curIt != -1)
	{
		KviMessageColorListBoxItem * back = (KviMessageColorListBoxItem *)m_pBackListBox->item(curIt);
		if(back)m_pLastItem->msgType()->setBack(back->m_iClrIdx);
	}
	m_pLastItem->msgType()->enableLogging(m_pEnableLogging->isChecked());
	//qDebug("Updating","options");
	curIt = m_pLevelListBox->currentItem();
	if(curIt < 0 || curIt > 5)curIt = 1;
	m_pLastItem->msgType()->setLevel(curIt);
	m_pListView->repaintItem(m_pLastItem);
}

void KviMessageColorsOptionsWidget::itemChanged(KviTalListViewItem * it)
{
	//qDebug("Item changed","options");
	if(m_pLastItem)saveLastItem();

	m_pLastItem = 0; // do NOT save in this routine

	m_pForeListBox->setEnabled(it);
	m_pBackListBox->setEnabled(it);
	m_pEnableLogging->setEnabled(it);
	m_pIconButton->setEnabled(it);
	m_pLevelListBox->setEnabled(it);

	if(it)
	{
		int back = ((KviMessageListViewItem *)it)->msgType()->back();
		int fore = ((KviMessageListViewItem *)it)->msgType()->fore();
		if(fore >= 0 && fore <= 15)
		{
			m_pForeListBox->setCurrentItem(m_pForeItems[fore]);
		}
		if(back >= 0 && back <= 15)
		{
			m_pBackListBox->setCurrentItem(m_pBackItems[back]);
		} else {
			m_pBackListBox->setCurrentItem(m_pBackItems[16]);
		}
		m_pLevelListBox->setCurrentItem(((KviMessageListViewItem *)it)->msgType()->level());
		m_pEnableLogging->setChecked(((KviMessageListViewItem *)it)->msgType()->logEnabled());
		m_pIconButton->setIconSet(*(g_pIconManager->getSmallIcon(((KviMessageListViewItem *)it)->msgType()->pixId())));
	}

	// Ok...can save from now on
	m_pLastItem = (KviMessageListViewItem *)it;
}

void KviMessageColorsOptionsWidget::colorChanged(KviTalListBoxItem *)
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

	KviMessageListViewItem * it = (KviMessageListViewItem *)m_pListView->firstChild();
	while(it)
	{
		KVI_OPTION_MSGTYPE(it->optionId()) = *(it->msgType());
		it = (KviMessageListViewItem *)(it->nextSibling());
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

	
		KviMessageListViewItem * it = (KviMessageListViewItem *)m_pListView->firstChild();

		while(it)
		{
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
			it = (KviMessageListViewItem *)(it->nextSibling());
		}

	}
}

//#include <errno.h>

void KviMessageColorsOptionsWidget::load()
{

	//KviStr szName;
	QString szName;
	//KviStr szInit;
	QString szInit;
	g_pApp->getLocalKvircDirectory(szInit,KviApp::MsgColors);

#ifndef COMPILE_ON_WINDOWS
	// symlink local to global
	//KviStr szGlobal;
	QString szGlobal;
	g_pApp->getGlobalKvircDirectory(szGlobal,KviApp::MsgColors);
	//KviStr szLocal;
	QString szLocal;
	g_pApp->getLocalKvircDirectory(szLocal,KviApp::MsgColors,"presets");
	//qDebug("SYMLINKING %s to %s",szGlobal.ptr(),szLocal.ptr());
	//qDebug("SYMLINK RETURNS %d (%d)",::symlink(szGlobal.ptr(),szLocal.ptr()));
	//qDebug("ERRNO (%d)",errno);
	symlink(szGlobal.toLocal8Bit().data(),szLocal.toLocal8Bit().data());
	// FIXME: Do it also on windows...
#endif

	if(KviFileDialog::askForOpenFileName(szName,__tr2qs_ctx("Choose a Filename - KVIrc ","options"),szInit))
	{
		itemChanged(0);

		KviConfig cfg(szName,KviConfig::Read);

		cfg.setGroup("Messages");

		//KviStr tmp;
		QString tmp;
		KviMessageListViewItem * it = (KviMessageListViewItem *)m_pListView->firstChild();

		while(it)
		{
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

			m_pListView->repaintItem(it);

			it = (KviMessageListViewItem *)(it->nextSibling());
		}
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_messages.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
