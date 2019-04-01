#ifndef _OPTW_MESSAGES_H_
#define _OPTW_MESSAGES_H_
//=============================================================================
//
//   File : OptionsWidget_message.h
//   Creation date : Sat Aug 11 2001 18:11:21 CEST by Szymon Stefanek
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

#include "KviOptionsWidget.h"
#include "KviMessageTypeSettings.h"
#include "KviIconManager.h"

#include <QTreeWidget>
#include <QItemDelegate>
#include <QCheckBox>
#include <QToolButton>
#include <QPainter>
#include <QMenu>

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_message KviIconManager::Messages
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_message __tr2qs_no_lookup("Text")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_message __tr2qs_no_lookup("theme,colors,output")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_message "theme"
#define KVI_OPTIONS_WIDGET_CONTAINER_OptionsWidget_message true

class OptionsWidget_message : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_message(QWidget * pParent);
	~OptionsWidget_message();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_privmsg KviIconManager::Raw
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_privmsg __tr2qs_no_lookup("Message Style")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_privmsg OptionsWidget_message
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_privmsg 40
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_privmsg "theme"
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_privmsg __tr2qs_no_lookup("theme,privmsg,output,format")

class OptionsWidget_privmsg : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_privmsg(QWidget * parent);
	~OptionsWidget_privmsg();

public:
	KviBoolSelector * m_pUseSmartColorSelector;
	KviBoolSelector * m_pSpecialSmartColorSelector;
	KviBoolSelector * m_pUseSmartColorWithBackgroundSelector;
	KviMircTextColorSelector * m_pSmartColorSelector;
protected slots:
	void enableDisableSmartColorSelector(bool);
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_timestamp KviIconManager::Time
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_timestamp __tr2qs_no_lookup("Timestamp")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_timestamp OptionsWidget_message
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_timestamp 20
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_timestamp "theme"
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_timestamp __tr2qs_no_lookup("time,timestamp")

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

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_messageColors KviIconManager::IconManager
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_messageColors __tr2qs_no_lookup("Message Decorations")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_messageColors __tr2qs_no_lookup("theme,icons,colors,messages,output")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_messageColors "theme"
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_messageColors OptionsWidget_message
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_messageColors 30

class MessageListWidget : public KviTalListWidget
{
	Q_OBJECT
public:
	MessageListWidget(QWidget * par) : KviTalListWidget(par){};
	~MessageListWidget(){};
};

class MessageListWidgetItemDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	MessageListWidgetItemDelegate(QAbstractItemView * pWidget = nullptr)
	    : QItemDelegate(pWidget){};
	~MessageListWidgetItemDelegate(){};
	void paint(QPainter * p, const QStyleOptionViewItem & opt, const QModelIndex & index) const;
};

class MessageListWidgetItem : public KviTalListWidgetItem
{
public:
	MessageListWidgetItem(MessageListWidget * l, int optId);
	~MessageListWidgetItem();

private:
	int m_iOptId;
	KviMessageTypeSettings * m_pMsgType;

public:
	int optionId() const { return m_iOptId; }
	KviMessageTypeSettings * msgType() const { return m_pMsgType; }
};

class MessageColorListWidgetItem : public KviTalListWidgetText
{
public:
	MessageColorListWidgetItem(KviTalListWidget * b, int idx);
	~MessageColorListWidgetItem();

public:
	int m_iClrIdx;

public:
	int clrIdx() const { return m_iClrIdx; }
};

class MessageColorListWidgetItemDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	MessageColorListWidgetItemDelegate(QAbstractItemView * pWidget = nullptr)
	    : QItemDelegate(pWidget){};
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
	MessageListWidget * m_pListView;
	MessageListWidgetItemDelegate * m_pListViewItemDelegate;
	KviTalListWidget * m_pForeListWidget;
	MessageColorListWidgetItemDelegate * m_pForeListWidgetDelegate;
	KviTalListWidget * m_pBackListWidget;
	MessageColorListWidgetItemDelegate * m_pBackListWidgetDelegate;
	KviTalListWidget * m_pLevelListWidget;
	MessageColorListWidgetItem * m_pForeItems[16];
	MessageColorListWidgetItem * m_pBackItems[17];
	MessageListWidgetItem * m_pLastItem;
	QCheckBox * m_pEnableLogging;
	QToolButton * m_pIconButton;
	QMenu * m_pIconPopup;

public:
	void saveLastItem();
protected slots:
	void itemChanged();
	void colorChanged();
	void iconButtonClicked();
	void newIconSelected(KviIconManager::SmallIcon eIcon);
	virtual void commit();
	virtual void save();
	virtual void load();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_standardColors KviIconManager::Colors
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_standardColors __tr2qs_no_lookup("Color Set")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_standardColors __tr2qs_no_lookup("theme,colors,messages,output")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_standardColors "theme"
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_standardColors 20
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_standardColors OptionsWidget_message

class OptionsWidget_standardColors : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_standardColors(QWidget * par);
	~OptionsWidget_standardColors();

private:
	KviColorSelector * m_pColorSelector[16];
private slots:
	void resetToDefaults();
};

#endif //!_OPTW_MESSAGES_H_
