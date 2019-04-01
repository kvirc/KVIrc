#ifndef _OPTW_TEXTICONS_H_
#define _OPTW_TEXTICONS_H_
//=============================================================================
//
//   File : OptionsWidget_textIcons.h
//   Creation date : Fri May 24 2002 00:14:02 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviTextIconManager.h"

#include <QTableWidget>
#include <QToolButton>
#include <QTableWidgetItem>

class TextIconTableItem;

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_textIcons KviIconManager::BigGrin
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_textIcons __tr2qs_no_lookup("Text Icons")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_textIcons OptionsWidget_tools
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_textIcons __tr2qs_no_lookup("smileys,emoticons")

class TextIconTableItem : public QTableWidgetItem
{
	friend class OptionsWidget_textIcons;

public:
	TextIconTableItem(QTableWidget * t, KviTextIcon * icon);
	~TextIconTableItem();

protected:
	KviTextIcon * m_pIcon;

public:
	KviTextIcon * icon()
	{
		return m_pIcon;
	};
	void setId(int id);
};

class OptionsWidget_textIcons : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_textIcons(QWidget * parent);
	~OptionsWidget_textIcons();

protected:
	QTableWidget * m_pTable;
	int m_iLastEditedRow;
	TextIconTableItem * m_pCurrentItem = nullptr;
	QPushButton * m_pAdd;
	QPushButton * m_pDel;
	QPushButton * m_pRestore;
	QToolButton * m_pCurrentIconButton;
	QMenu * m_pPopup;

private:
	void fillTable();

public:
	virtual void commit();
protected slots:
	void itemSelectionChanged();
	void currentItemChanged(QTableWidgetItem * cur, QTableWidgetItem * prev);
	void addClicked();
	void delClicked();
	void restoreClicked();

	void iconSelected(KviIconManager::SmallIcon);
	void doPopup();
	void chooseFromFile();
};

#endif //!_OPTW_TEXTICONS_H_
