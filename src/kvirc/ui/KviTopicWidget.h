#ifndef _KVI_TOPICW_H_
#define _KVI_TOPICW_H_
//============================================================================
//
//   File : KviTopicWidget.h
//   Creation date : Fri Aug 4 2000 12:03:12 by Szymon Stefanek
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
//============================================================================

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviApplication.h"
#include "KviInputEditor.h"
#include "KviTalListWidget.h"
#include "KviTalIconAndRichTextItemDelegate.h"

#include <QString>

class KviChannelWindow;
class KviIrcConnection;
class KviThemedLabel;
class QComboBox;
class QMenu;
class QPushButton;

class KVIRC_API KviTopicListBoxItemDelegate : public KviTalIconAndRichTextItemDelegate
{
	Q_OBJECT
public:
	KviTopicListBoxItemDelegate(QAbstractItemView * pWidget = nullptr);
	~KviTopicListBoxItemDelegate();

public:
	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

class KVIRC_API KviTopicListBoxItem : public KviTalListWidgetText
{
public:
	KviTopicListBoxItem(KviTalListWidget * pListBox = nullptr, const QString & text = QString());
	~KviTopicListBoxItem();

public:
	virtual int width(const KviTalListWidget * pList) const;
};

class KVIRC_API KviTopicWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(int TransparencyCapable READ dummyRead)
	friend class KviChannelWindow;
	friend class KviTalListWidget;

public:
	KviTopicWidget(QWidget * pParent, KviChannelWindow * pChannel, const char * name);
	~KviTopicWidget();

private:
	QString m_szTopic;
	QString m_szSetBy;
	QString m_szSetAt;
	QPushButton * m_pAccept;
	QPushButton * m_pDiscard;
	QPushButton * m_pHistory;
	QMenu * m_pContextPopup;
	QAbstractItemDelegate * m_pItemDelegate;
	KviThemedLabel * m_pLabel;
	KviInputEditor * m_pInput;
	KviTalListWidget * m_pCompletionBox;
	KviChannelWindow * m_pKviChannelWindow;

protected:
	int m_iCursorPosition;

private:
	QChar getSubstituteChar(unsigned short uControlCode);

protected:
	void updateToolTip();
	void deactivate();
	void iconButtonClicked();
	bool eventFilter(QObject * o, QEvent * e) override;
	void mousePressEvent(QMouseEvent * e) override;
	void keyPressEvent(QKeyEvent * e) override;
	void resizeEvent(QResizeEvent * e) override;

public:
	void insertChar(QChar c);
	void insertText(const QString & szText);
	int dummyRead() const { return 0; };
	void reset();

	void setTopic(const QString & szTopic);
	void setTopicSetBy(const QString & szSetBy);
	void setTopicSetAt(const QString & szSetAt);

	const QString & topic() { return m_szTopic; };
	const QString & topicSetBy() { return m_szSetBy; };
	const QString & topicSetAt() { return m_szSetAt; };
	QSize sizeHint() const override;
	void applyOptions();

	static void paintColoredText(QPainter * p, QString szText, const QPalette & palette, const QRect & rect);
protected slots:
	void acceptClicked();
	void discardClicked();
	void historyClicked();
	void contextPopupAboutToShow();
	void copy();
	void complete();
	void popDownListBox();
	void switchMode();
signals:
	void topicSelected(const QString & szTopic);
};

#endif //_KVI_TOPICW_H_
