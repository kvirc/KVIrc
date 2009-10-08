#ifndef _OPTW_MEDIATYPES_H_
#define _OPTW_MEDIATYPES_H_
//=============================================================================
//
//   File : optw_mediatypes.h
//   Creation date : Sat Jan 20 2001 16:12:10 CEST by Szymon Stefanek
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
#include "kvi_mediatype.h"
#include <QTreeWidget>

#include <QLineEdit>
#include <QPushButton>


class KviMediaTypeTreeWidgetItem : public QTreeWidgetItem
{
public:
	KviMediaTypeTreeWidgetItem(QTreeWidget * w,KviMediaType * t);
	~KviMediaTypeTreeWidgetItem();
private:
	KviMediaType m_data;
public:
	void copyData(KviMediaType * t);
	const KviMediaType * mydata(){ return &m_data; };
};

#define KVI_OPTIONS_WIDGET_ICON_KviMediaTypesOptionsWidget KVI_SMALLICON_FILE
#define KVI_OPTIONS_WIDGET_NAME_KviMediaTypesOptionsWidget __tr2qs_no_lookup("Media Types")
#define KVI_OPTIONS_WIDGET_PARENT_KviMediaTypesOptionsWidget KviGeneralOptOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviMediaTypesOptionsWidget __tr2qs_no_lookup("file,audio,video,programs")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviMediaTypesOptionsWidget 60000
//#define KVI_OPTIONS_WIDGET_HIDDEN_KviMediaTypesOptionsWidget true

class KviMediaTypesOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviMediaTypesOptionsWidget(QWidget * parent);
	~KviMediaTypesOptionsWidget();
private:
	QTreeWidget  * m_pTreeWidget;
	QLineEdit   * m_pDescription;
	QLineEdit   * m_pIanaType;
	QLineEdit   * m_pFileMask;
	QLineEdit   * m_pSavePath;
	QLineEdit   * m_pCommandline;
	QLineEdit   * m_pMagicBytes;
	QLineEdit   * m_pRemoteExecCommandline;
	QLineEdit   * m_pIcon;
	QPushButton * m_pDelete;
	KviMediaTypeTreeWidgetItem * m_pLastItem;
private:
	void fillTreeWidget();
	void saveLastItem();
	void enableOrDisable();
	void setLineEdits();
private slots:
	void currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *prev);
	void newMediaType();
	void delMediaType();
public:
	virtual void commit();
};

#endif //!_OPTW_IDENTITY_H_
