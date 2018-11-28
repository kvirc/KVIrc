#ifndef _KVI_IRCVIEWTOOLS_H_
#define _KVI_IRCVIEWTOOLS_H_
//=============================================================================
//
//   File : KviIrcView_tools.h
//   Creation date : Sat Oct 9 2004 16:03:01 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "KviTalToolTip.h"

#include <QFrame>
#include <QString>
#include <QTreeWidget>
#include <QCheckBox>

class QLabel;
class QLineEdit;
class QComboBox;
class KviIrcView;
class QPushButton;
class QMenu;

class KviIrcViewToolTip : public KviTalToolTip
{
public:
	KviIrcViewToolTip(KviIrcView * pView);
	virtual ~KviIrcViewToolTip();

protected:
	KviIrcView * m_pView;

protected:
	virtual void maybeTip(const QPoint & pnt);

public:
	void doTip(const QRect & rct, const QString & str) { tip(rct, str); };
};

//
// Item for the filter list view
//

class KviIrcViewToolWidget;

class KviIrcMessageCheckListItem : public QTreeWidgetItem
{
public:
	KviIrcMessageCheckListItem(QTreeWidget * par, KviIrcViewToolWidget * w, int id);
	~KviIrcMessageCheckListItem();

private:
	int m_iId;
	KviIrcViewToolWidget * m_pToolWidget;

public:
	//void setToolWidget(KviIrcViewToolWidget * w){ m_pToolWidget = w; };
	void setOn(bool b) { setCheckState(0, b ? Qt::Checked : Qt::Unchecked); };
	bool isOn()
	{
		if(checkState(0) == Qt::Checked)
			return true;
		else
			return false;
	}
};

//
// Search tools widget
//

class KviIrcView;

class KviIrcViewToolWidget : public QWidget
{
	friend class KviIrcView;
	Q_OBJECT
protected:
	KviIrcViewToolWidget(KviIrcView * par);
	~KviIrcViewToolWidget();

protected:
	KviIrcView * m_pIrcView;
	QLineEdit * m_pStringToFind;
	QPoint m_pressPoint;

	QCheckBox * m_pCaseSensitive;
	QComboBox * m_pSearchMode;
	QMenu * m_pOptionsWidget;
	QPushButton * m_pOptionsButton;

	// 	QLabel      * m_pFindResult;

	QTreeWidget * m_pFilterView;

	KviIrcMessageCheckListItem ** m_pFilterItems;

public:
	enum SearchMode
	{
		PlainText,
		Wildcards,
		RegExp
	};
	void setFindResult(const QString & text);
	void focusStringToFind();
	bool messageEnabled(int msg_type) { return m_pFilterItems[msg_type]->isOn(); }
	void forceRepaint();
protected slots:
	void findPrev();
	void findNext();
	void findNextHelper(QString unused);
	void filterEnableAll();
	void filterEnableNone();
	void filterSave();
	void filterLoad();
	void toggleOptions();
};

#endif //!_KVI_IRCVIEWTOOLS_H_
