#ifndef _CLASS_TABWIDGET_H_
#define _CLASS_TABWIDGET_H_
//=============================================================================
//
//   File : KvsObject_tabWidget.h
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KvsObject_widget.h"
#include "object_macros.h"

class KvsObject_tabWidget : public KvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_tabWidget)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;
	QList<kvs_hobject_t> tabsList;

	bool addTab(KviKvsObjectFunctionCall * c);
	bool indexOf(KviKvsObjectFunctionCall * c);
	bool insertTab(KviKvsObjectFunctionCall * c);
	bool setTabToolTip(KviKvsObjectFunctionCall * c);
	bool removeTabToolTip(KviKvsObjectFunctionCall * c);
	bool setTabLabel(KviKvsObjectFunctionCall * c);
	bool setCurrentPage(KviKvsObjectFunctionCall * c);
	bool currentPageIndex(KviKvsObjectFunctionCall * c);
	bool tabLabel(KviKvsObjectFunctionCall * c);
	bool currentTabLabel(KviKvsObjectFunctionCall * c);
	bool count(KviKvsObjectFunctionCall * c);
	bool changeTab(KviKvsObjectFunctionCall * c);
	bool removePage(KviKvsObjectFunctionCall * c);
	bool setTabPosition(KviKvsObjectFunctionCall * c);
	bool setTabsClosable(KviKvsObjectFunctionCall * c);
	bool widgetAt(KviKvsObjectFunctionCall * c);

	// events
	bool currentChangedEvent(KviKvsObjectFunctionCall * c);
	bool tabCloseRequestEvent(KviKvsObjectFunctionCall * c);

protected slots:
	void slotCurrentChanged(int);
	void slotTabCloseRequest(int);
};

#endif //!_CLASS_TABWIDGET_H_
