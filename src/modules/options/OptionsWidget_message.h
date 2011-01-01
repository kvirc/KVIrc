#ifndef _OPTW_MESSAGES_H_
#define _OPTW_MESSAGES_H_
//=============================================================================
//
//   File : OptionsWidget_message.h
//   Creation date : Sat Aug 11 2001 18:11:21 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviOptionsWidget.h"
#include "KviMessageTypeSettings.h"
#include "KviTalPopupMenu.h"

#include <QTreeWidget>
#include <QItemDelegate>
#include <QCheckBox>
#include <QToolButton>
#include <QPainter>
#include <QColorGroup>

#define KVI_OPTIONS_WIDGET_ICON_KviMessageOptionsWidget KVI_SMALLICON_MESSAGES
#define KVI_OPTIONS_WIDGET_NAME_KviMessageOptionsWidget __tr2qs_no_lookup("Text")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviMessageOptionsWidget __tr2qs_no_lookup("theme,colors,output")
#define KVI_OPTIONS_WIDGET_GROUP_KviMessageOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_CONTAINER_KviMessageOptionsWidget true

class OptionsWidget_message : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_message(QWidget * pParent);
	~OptionsWidget_message();
};


#define KVI_OPTIONS_WIDGET_ICON_KviPrivmsgOptionsWidget KVI_SMALLICON_RAW
#define KVI_OPTIONS_WIDGET_NAME_KviPrivmsgOptionsWidget __tr2qs_no_lookup("Message Style")
#define KVI_OPTIONS_WIDGET_PARENT_KviPrivmsgOptionsWidget OptionsWidget_message
#define KVI_OPTIONS_WIDGET_PRIORITY_KviPrivmsgOptionsWidget 40
#define KVI_OPTIONS_WIDGET_GROUP_KviPrivmsgOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviPrivmsgOptionsWidget __tr2qs_no_lookup("theme,privmsg,output,format")

class OptionsWidget_privmsg : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_privmsg(QWidget * parent);
	~OptionsWidget_privmsg();
public:
	KviBoolSelector * m_pUseSmartColorSelector;
	KviBoolSelector * m_pSpecialSmartColorSelector;
	KviMircTextColorSelector * m_pSmartColorSelector;
protected slots:
	void enableDisableSmartColorSelector(bool);
};

#define KVI_OPTIONS_WIDGET_ICON_KviTimestampOptionsWidget KVI_SMALLICON_TIME
#define KVI_OPTIONS_WIDGET_NAME_KviTimestampOptionsWidget __tr2qs_no_lookup("Timestamp")
#define KVI_OPTIONS_WIDGET_PARENT_KviTimestampOptionsWidget OptionsWidget_message
#define KVI_OPTIONS_WIDGET_PRIORITY_KviTimestampOptionsWidget 20
#define KVI_OPTIONS_WIDGET_GROUP_KviTimestampOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTimestampOptionsWidget __tr2qs_no_lookup("time,timestamp")

class OptionsWidget_timestamp : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_timestamp(QWidget * parent);
	~OptionsWidget_timestamp();
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
#define KVI_OPTIONS_WIDGET_PARENT_KviMessageColorsOptionsWidget OptionsWidget_message
#define KVI_OPTIONS_WIDGET_PRIORITY_KviMessageColorsOptionsWidget 30

class MessageListWidget : public KviTalListWidget
{
	Q_OBJECT
public:
	MessageListWidget(QWidget * par): KviTalListWidget(par) {};
	~MessageListWidget() {};
};

class  MessageListWidgetItemDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	MessageListWidgetItemDelegate(QAbstractItemView * pWidget=0)
		: QItemDelegate(pWidget) {};
	~MessageListWidgetItemDelegate(){};
	void paint(QPainter * p, const QStyleOptionViewItem & opt, const QModelIndex & index) const;
};

class MessageListWidgetItem : public KviTalListWidgetItem
{
public:
	MessageListWidgetItem(MessageListWidget* l,int optId);
	~MessageListWidgetItem();
private:
	int          m_iOptId;
	KviMessageTypeSettings * m_pMsgType;
public:
	inline int optionId(){ return m_iOptId; };
	inline KviMessageTypeSettings * msgType(){ return m_pMsgType; };
};


class MessageColorListWidgetItem : public KviTalListWidgetText
{
public:
	MessageColorListWidgetItem(KviTalListWidget * b, int idx);
	~MessageColorListWidgetItem();
public:
	int m_iClrIdx;
public:
	inline int clrIdx() { return m_iClrIdx; };
};

class  MessageColorListWidgetItemDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	MessageColorListWidgetItemDelegate(QAbstractItemView * pWidget=0)
		: QItemDelegate(pWidget) {};
	~MessageColorListWidgetItemDelegate(){};
	void paint(QPainter * p, const QStyleOptionViewItem & opt, const QModelIndex & index) const;
};

class OptionsWidget_messageColors : public KviOptionsWidget
{
	Q_OBJECT
	friend class KviTalListWidget;
public:
	OptionsWidget_messageColors(QWidget * parent);
	~OptionsWidget_messageColors();
public:
	MessageListWidget         * m_pListView;
	MessageListWidgetItemDelegate * m_pListViewItemDelegate;
	KviTalListWidget           * m_pForeListWidget;
	MessageColorListWidgetItemDelegate * m_pForeListWidgetDelegate;
	KviTalListWidget           * m_pBackListWidget;
	MessageColorListWidgetItemDelegate * m_pBackListWidgetDelegate;
	KviTalListWidget           * m_pLevelListWidget;
	MessageColorListWidgetItem * m_pForeItems[16];
	MessageColorListWidgetItem * m_pBackItems[17];
	MessageListWidgetItem     * m_pLastItem;
	QCheckBox                  * m_pEnableLogging;
	QToolButton                * m_pIconButton;
	KviTalPopupMenu            * m_pIconPopup;
public:
	void saveLastItem();
protected slots:
	void itemChanged();
	void colorChanged();
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
#define KVI_OPTIONS_WIDGET_PARENT_KviStandardColorsOptionsWidget OptionsWidget_message

class OptionsWidget_standardColors : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_standardColors(QWidget * par);
	~OptionsWidget_standardColors();

};


#endif //!_OPTW_MESSAGES_H_
