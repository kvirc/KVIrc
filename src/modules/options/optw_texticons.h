#ifndef _OPTW_TEXTICONS_H_
#define _OPTW_TEXTICONS_H_

//
//   File : optw_texticons.h
//   Creation date : Fri May 24 2002 00:14:02 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_texticonmanager.h"
#include "kvi_tal_popupmenu.h"
#include "kvi_tal_hbox.h"

#include <QToolButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QItemDelegate>

class KviTextIconTableItem;

#define KVI_OPTIONS_WIDGET_ICON_KviTextIconsOptionsWidget KVI_SMALLICON_BIGGRIN
#define KVI_OPTIONS_WIDGET_NAME_KviTextIconsOptionsWidget __tr2qs_no_lookup("Text icons")
#define KVI_OPTIONS_WIDGET_PARENT_KviTextIconsOptionsWidget KviToolsOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTextIconsOptionsWidget __tr2qs_no_lookup("smileys,emoticons")


class KviTextIconEditor : public KviTalHBox
{
	Q_OBJECT
public:
	KviTextIconEditor(QWidget * par);
	~KviTextIconEditor();
protected:
	KviTextIcon     *m_pIcon;
	KviTalPopupMenu *m_pPopup;
	QToolButton     *m_pIconButton;
	QToolButton     *m_pBrowseButton;
public:
	void updateIcon();
	void setIcon(KviTextIcon * icon)
		{ m_pIcon = icon; };
	KviTextIcon* icon()
		{ return m_pIcon; };
protected slots:
	void iconSelected(int);
	void doPopup();
	void chooseFromFile();
};

class KviTextIconTableItem : public QTableWidgetItem
{
	friend class KviTextIconsOptionsWidget;
public:
	KviTextIconTableItem(QTableWidget * t,KviTextIcon * icon);
	~KviTextIconTableItem();
protected:
	KviTextIcon * m_pIcon;
public:
	KviTextIcon * icon() { return m_pIcon; };
	void setId(int id);
	//virtual QWidget * createEditor() const;
	void setContentFromEditor(QWidget * w);
};

//
// Yes. It's true. You spend more time to find the function to reimplement in order to get the
// desired behaviour than to think out your REAL algorithm.
// Usually the function to reimplement is somewhere in a very abstract class that you're
// forced to understand and then inherit. IMHO the Qt3 item views were *really* better.
// Let's hope that Trolls improve this.
// 
class KviTextIconQt4ModelViewParadigmIsRidiculouslyComplexAndUglyTableItemDelegate : public QItemDelegate
{
protected:
	QTableWidget * m_pTableWidget;
public:
	KviTextIconQt4ModelViewParadigmIsRidiculouslyComplexAndUglyTableItemDelegate(QTableWidget * pTableWidget)
		: QItemDelegate(pTableWidget), m_pTableWidget(pTableWidget) {};
	~KviTextIconQt4ModelViewParadigmIsRidiculouslyComplexAndUglyTableItemDelegate()
		{};
protected:
	virtual QWidget * createEditor(QWidget * parent,const QStyleOptionViewItem & option,const QModelIndex & index) const;
	virtual void setEditorData(QWidget * editor,const QModelIndex & index) const;
	virtual void setModelData(QWidget * editor,QAbstractItemModel * model,const QModelIndex & index) const;
	virtual void updateEditorGeometry(QWidget * editor,const QStyleOptionViewItem & option,const QModelIndex & index) const;
};

class KviTextIconsOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviTextIconsOptionsWidget(QWidget * parent);
	~KviTextIconsOptionsWidget();
protected:
	QTableWidget * m_pTable;
	QPushButton  * m_pAdd;
	QPushButton  * m_pDel;
public:
	virtual void commit();
protected slots:
	void itemSelectionChanged();
	void addClicked();
	void delClicked();
};

#endif //!_OPTW_TEXTICONS_H_
