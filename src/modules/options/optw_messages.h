#ifndef _OPTW_MESSAGES_H_
#define _OPTW_MESSAGES_H_
//=============================================================================
//
//   File : optw_messages.h
//   Creation date : Sat Aug 11 2001 18:11:21 CEST by Szymon Stefanek
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

#include "kvi_optionswidget.h"
#include "kvi_msgtype.h"
#include "kvi_tal_popupmenu.h"

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
	KviBoolSelector * m_pUseSmartColorSelector;
	KviBoolSelector * m_pSpecialSmartColorSelector;
	KviMircTextColorSelector * m_pSmartColorSelector;
protected slots:
	void enableDisableSmartColorSelector(bool);
};

#define KVI_OPTIONS_WIDGET_ICON_KviTimestampOptionsWidget KVI_SMALLICON_TIME
#define KVI_OPTIONS_WIDGET_NAME_KviTimestampOptionsWidget __tr2qs_no_lookup("Timestamp")
#define KVI_OPTIONS_WIDGET_PARENT_KviTimestampOptionsWidget KviMessageOptionsWidget
#define KVI_OPTIONS_WIDGET_PRIORITY_KviTimestampOptionsWidget 20
#define KVI_OPTIONS_WIDGET_GROUP_KviTimestampOptionsWidget "theme"
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTimestampOptionsWidget __tr2qs_no_lookup("time,timestamp")

class KviTimestampOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviTimestampOptionsWidget(QWidget * parent);
	~KviTimestampOptionsWidget();
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

class KviMessageListWidget : public KviTalListWidget
{
	Q_OBJECT
public:
	KviMessageListWidget(QWidget * par): KviTalListWidget(par) {};
	~KviMessageListWidget() {};
};

class  KviMessageListWidgetItemDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	KviMessageListWidgetItemDelegate(QAbstractItemView * pWidget=0)
		: QItemDelegate(pWidget) {};
	~KviMessageListWidgetItemDelegate(){};
	void paint(QPainter * p, const QStyleOptionViewItem & opt, const QModelIndex & index) const;
};

class KviMessageListWidgetItem : public KviTalListWidgetItem
{
public:
	KviMessageListWidgetItem(KviMessageListWidget* l,int optId);
	~KviMessageListWidgetItem();
private:
	int          m_iOptId;
	KviMsgType * m_pMsgType;
public:
	inline int optionId(){ return m_iOptId; };
	inline KviMsgType * msgType(){ return m_pMsgType; };
};


class KviMessageColorListWidgetItem : public KviTalListWidgetText
{
public:
	KviMessageColorListWidgetItem(KviTalListWidget * b, int idx);
	~KviMessageColorListWidgetItem();
public:
	int m_iClrIdx;
public:
	inline int clrIdx() { return m_iClrIdx; };
};

class  KviMessageColorListWidgetItemDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	KviMessageColorListWidgetItemDelegate(QAbstractItemView * pWidget=0)
		: QItemDelegate(pWidget) {};
	~KviMessageColorListWidgetItemDelegate(){};
	void paint(QPainter * p, const QStyleOptionViewItem & opt, const QModelIndex & index) const;
};

class KviMessageColorsOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
	friend class KviTalListWidget;
public:
	KviMessageColorsOptionsWidget(QWidget * parent);
	~KviMessageColorsOptionsWidget();
public:
	KviMessageListWidget         * m_pListView;
	KviMessageListWidgetItemDelegate * m_pListViewItemDelegate;
	KviTalListWidget           * m_pForeListWidget;
	KviMessageColorListWidgetItemDelegate * m_pForeListWidgetDelegate;
	KviTalListWidget           * m_pBackListWidget;
	KviMessageColorListWidgetItemDelegate * m_pBackListWidgetDelegate;
	KviTalListWidget           * m_pLevelListWidget;
	KviMessageColorListWidgetItem * m_pForeItems[16];
	KviMessageColorListWidgetItem * m_pBackItems[17];
	KviMessageListWidgetItem     * m_pLastItem;
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
#define KVI_OPTIONS_WIDGET_PARENT_KviStandardColorsOptionsWidget KviMessageOptionsWidget

class KviStandardColorsOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviStandardColorsOptionsWidget(QWidget * par);
	~KviStandardColorsOptionsWidget();

};


#endif //!_OPTW_MESSAGES_H_
