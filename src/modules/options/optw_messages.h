#ifndef _OPTW_MESSAGES_H_
#define _OPTW_MESSAGES_H_

//
//   File : optw_messages.h
//   Creation date : Sat Aug 11 2001 18:11:21 CEST by Szymon Stefanek
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

#include "kvi_optionswidget.h"

#include "kvi_msgtype.h"

#include <qlistview.h>
#include <qlistbox.h>
#include <qcheckbox.h>
#include <qtoolbutton.h>
#include "kvi_tal_popupmenu.h"

#define KVI_OPTIONS_WIDGET_ICON_KviMessageOptionsWidget KVI_SMALLICON_MESSAGES
#define KVI_OPTIONS_WIDGET_NAME_KviMessageOptionsWidget __tr2qs_no_lookup("Text")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviMessageOptionsWidget __tr2qs_no_lookup("theme,colors,output")
#define KVI_OPTIONS_WIDGET_GROUP_KviMessageOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_CONTAINER_KviMessageOptionsWidget true

class KviMessageOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviMessageOptionsWidget(QWidget * pParent);
	~KviMessageOptionsWidget();
};


#define KVI_OPTIONS_WIDGET_ICON_KviPrivmsgOptionsWidget KVI_SMALLICON_RAW
#define KVI_OPTIONS_WIDGET_NAME_KviPrivmsgOptionsWidget __tr2qs_no_lookup("Message Style")
#define KVI_OPTIONS_WIDGET_PARENT_KviPrivmsgOptionsWidget KviMessageOptionsWidget
#define KVI_OPTIONS_WIDGET_PRIORITY_KviPrivmsgOptionsWidget 40
#define KVI_OPTIONS_WIDGET_GROUP_KviPrivmsgOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviPrivmsgOptionsWidget __tr2qs_no_lookup("theme,privmsg,output,format")

class KviPrivmsgOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviPrivmsgOptionsWidget(QWidget * parent);
	~KviPrivmsgOptionsWidget();
public:
	KviBoolSelector * m_pUseTimestampSelector;
	KviBoolSelector * m_pSpecialTimestampColorSelector;
	KviMircTextColorSelector * m_pTimestampColorSelector;
protected slots:
	void enableDisableTimestampSelector(bool);
};

#define KVI_OPTIONS_WIDGET_ICON_KviMessageColorsOptionsWidget KVI_SMALLICON_MESSAGES
#define KVI_OPTIONS_WIDGET_NAME_KviMessageColorsOptionsWidget __tr2qs_no_lookup("Message Colors")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviMessageColorsOptionsWidget __tr2qs_no_lookup("theme,colors,messages,output")
#define KVI_OPTIONS_WIDGET_GROUP_KviMessageColorsOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_PARENT_KviMessageColorsOptionsWidget KviMessageOptionsWidget
#define KVI_OPTIONS_WIDGET_PRIORITY_KviMessageColorsOptionsWidget 30

class KviMessageListView : public QListView
{
	Q_OBJECT
public:
	KviMessageListView(QWidget * par);
	~KviMessageListView();
protected:
	void paintEmptyAreaInternal(QPainter * p,const QRect &viewportRect,const QRect &painterRect);
public:
	void paintEmptyArea(QPainter * p,const QRect &rct);
};


class KviMessageListViewItem : public QListViewItem
{
public:
	KviMessageListViewItem(QListView * l,int optId);
	~KviMessageListViewItem();
private:
	int          m_iOptId;
	KviMsgType * m_pMsgType;
public:
	int optionId(){ return m_iOptId; };
	KviMsgType * msgType(){ return m_pMsgType; };
	virtual void paintCell(QPainter * p,const QColorGroup &,int,int,int);
};


class KviMessageColorListBoxItem : public QListBoxText
{
public:
	KviMessageColorListBoxItem(QListBox * b,const QColor & clr,int idx);
	~KviMessageColorListBoxItem();
public:
	int m_iClrIdx;
	QColor m_clr;
public:
	virtual void paint(QPainter * p);
	virtual int width(const QListBox * lv) const { return 120; };
};




class KviMessageColorsOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviMessageColorsOptionsWidget(QWidget * parent);
	~KviMessageColorsOptionsWidget();
public:
	KviMessageListView         * m_pListView;
	QListBox                   * m_pForeListBox;
	QListBox                   * m_pBackListBox;
	QListBox                   * m_pLevelListBox;
	KviMessageColorListBoxItem * m_pForeItems[16];
	KviMessageColorListBoxItem * m_pBackItems[17];
	KviMessageListViewItem     * m_pLastItem;
	QCheckBox                  * m_pEnableLogging;
	QToolButton                * m_pIconButton;
	KviTalPopupMenu                 * m_pIconPopup;
public:
	void saveLastItem();
protected slots:
	void itemChanged(QListViewItem * it);
	void colorChanged(QListBoxItem *);
	void iconButtonClicked();
	void newIconSelected(int iconId);
	virtual void commit();
	virtual void save();
	virtual void load();
};

#define KVI_OPTIONS_WIDGET_ICON_KviStandardColorsOptionsWidget KVI_SMALLICON_COLORS
#define KVI_OPTIONS_WIDGET_NAME_KviStandardColorsOptionsWidget __tr2qs_no_lookup("Color Set")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviStandardColorsOptionsWidget __tr2qs_no_lookup("theme,colors,messages,output")
#define KVI_OPTIONS_WIDGET_GROUP_KviStandardColorsOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_PRIORITY_KviStandardColorsOptionsWidget 20
#define KVI_OPTIONS_WIDGET_PARENT_KviStandardColorsOptionsWidget KviMessageOptionsWidget

class KviStandardColorsOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviStandardColorsOptionsWidget(QWidget * par);
	~KviStandardColorsOptionsWidget();
	
};




#endif //!_OPTW_MESSAGES_H_
