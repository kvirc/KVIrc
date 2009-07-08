#ifndef _OPTW_TEXTICONS_H_
#define _OPTW_TEXTICONS_H_
//=============================================================================
//
//   File : optw_texticons.h
//   Creation date : Fri May 24 2002 00:14:02 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

};

class KviTextIconsOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviTextIconsOptionsWidget(QWidget * parent);
	~KviTextIconsOptionsWidget();
protected:
	QTableWidget * m_pTable;
	int m_iLastEditedRow;
	KviTextIconTableItem *m_pItem;
	QPushButton  * m_pAdd;
	QPushButton  * m_pDel;
	KviTalPopupMenu *m_pPopup;
	KviTalHBox *m_pBox;
	QToolButton *m_pIconButton;
	QToolButton *m_pBrowseButton;
public:
	virtual void commit();
protected slots:
	void itemSelectionChanged();
	void itemClicked(QTableWidgetItem *i);
	void addClicked();
	void delClicked();

	void iconSelected(int);
	void doPopup();
	void chooseFromFile();
};

#endif //!_OPTW_TEXTICONS_H_
