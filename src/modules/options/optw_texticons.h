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

class KviTextIconTableItem;

#define KVI_OPTIONS_WIDGET_ICON_KviTextIconsOptionsWidget KVI_SMALLICON_BIGGRIN
#define KVI_OPTIONS_WIDGET_NAME_KviTextIconsOptionsWidget __tr2qs_no_lookup("Text icons")
#define KVI_OPTIONS_WIDGET_PARENT_KviTextIconsOptionsWidget KviToolsOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTextIconsOptionsWidget __tr2qs_no_lookup("smileys,emoticons")


class KviTextIconEditor : public KviTalHBox
{
	Q_OBJECT
public:
	KviTextIconEditor(QWidget * par,KviTextIcon * icon,KviTextIconTableItem* item);
	~KviTextIconEditor();
protected:
	KviTextIconTableItem *m_pTableItem;
	KviTextIcon     *m_pIcon;
	KviTalPopupMenu *m_pPopup;
	QToolButton     *m_pIconButton;
	QToolButton     *m_pBrowseButton;
public:
	void updateIcon();
	KviTextIcon*	icon() { return m_pIcon; };
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
	virtual QWidget * createEditor() const;
	virtual void setContentFromEditor(QWidget * w);
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
	void selectionChanged();
	void addClicked();
	void delClicked();
};

#endif //!_OPTW_TEXTICONS_H_
