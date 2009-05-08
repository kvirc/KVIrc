#ifndef _KVI_TOPICW_H_
#define _KVI_TOPICW_H_

//============================================================================
//
//   File : kvi_topicw.h
//   Creation date : Fri Aug 4 2000 12:03:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//============================================================================

#include "kvi_settings.h"

#include <qframe.h>
#include <qpushbutton.h>
#include "kvi_tal_listbox.h" 

#include "kvi_string.h"
#include "kvi_app.h"
#include "kvi_input.h"

class QComboBox;
class KviChannel;
class KviTalPopupMenu;
class KviIrcConnection;
class KviTalListBox;

class KVIRC_API KviListBoxTopicItem : public KviTalListBoxText
{
public:
	KviListBoxTopicItem( KviTalListBox * listbox = 0 , const QString & text = QString::null):KviTalListBoxText(listbox,text) { ; };
	virtual int width ( const KviTalListBox * lb ) const;
protected:
	virtual void paint ( QPainter * p );

};

class KVIRC_API KviTopicWidget : public QFrame 
{
	Q_OBJECT
	Q_PROPERTY(int TransparencyCapable READ dummyRead)
	friend class KviChannel;
	
public:
	KviTopicWidget(QWidget * par,const char * name);
	~KviTopicWidget();

private:
	QString       m_szTopic;
	QString       m_szSetBy;
	QString       m_szSetAt;
	QPushButton * m_pAccept;
	QPushButton * m_pDiscard;
	QPushButton * m_pHistory;
	KviTalPopupMenu  * m_pContextPopup;
	QChar getSubstituteChar(unsigned short control_code);
	int xCursorPostionCalculation(int xInd);
	KviInputEditor* m_pInput;
	KviTalListBox* 	m_pCompletionBox;
protected:
	int  m_iCursorPosition;
	virtual void drawContents(QPainter *p);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
#ifdef COMPILE_USE_QT4
	virtual void paintEvent(QPaintEvent * e);
#endif
	void updateToolTip();
	void deactivate();
	void iconButtonClicked();
	virtual bool eventFilter(QObject *o,QEvent *e);
	bool handleKeyPressEvent(QKeyEvent * e);
public:
	void insertChar(QChar c);
	void insertText(const QString &s);
	int dummyRead() const { return 0; };
	void reset();

	void setTopic(const QString & szTopic);
	void setTopicSetBy(const QString & setBy);
	void setTopicSetAt(const QString & setAt);

	const QString & topic(){ return m_szTopic; };
	const QString & topicSetBy(){ return m_szSetBy; };
	const QString & topicSetAt(){ return m_szSetAt; };
	virtual QSize sizeHint() const;
	void applyOptions();
	
	static void paintColoredText(QPainter *p, QString text,const QColorGroup& cg, int h=-1, int w=-1);
protected slots:
	void acceptClicked();
	void discardClicked();
	void historyClicked();
	void contextPopupAboutToShow();
	void copy();
	void complete(int);
	void popDownListBox();
signals:
	void topicSelected(const QString &szTopic);
};

#endif //_KVI_TOPICW_H_
