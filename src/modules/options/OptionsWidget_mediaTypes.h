#ifndef _OPTW_MEDIATYPES_H_
#define _OPTW_MEDIATYPES_H_
//=============================================================================
//
//   File : OptionsWidget_mediaTypes.h
//   Creation date : Sat Jan 20 2001 16:12:10 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviMediaManager.h"

#include <QTreeWidget>
#include <QLineEdit>
#include <QPushButton>

class MediaTypeTreeWidgetItem : public QTreeWidgetItem
{
public:
	MediaTypeTreeWidgetItem(QTreeWidget * w, KviMediaType * t);
	~MediaTypeTreeWidgetItem();

private:
	KviMediaType m_data;

public:
	void copyData(KviMediaType * t);
	const KviMediaType * mydata() { return &m_data; };
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_mediaTypes KviIconManager::File
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_mediaTypes __tr2qs_no_lookup("Media Types")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_mediaTypes OptionsWidget_general
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_mediaTypes __tr2qs_no_lookup("file,audio,video,programs")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_mediaTypes 60000
//#define KVI_OPTIONS_WIDGET_HIDDEN_OptionsWidget_mediaTypes true

class OptionsWidget_mediaTypes : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_mediaTypes(QWidget * parent);
	~OptionsWidget_mediaTypes();

private:
	QTreeWidget * m_pTreeWidget;
	QLineEdit * m_pDescription;
	QLineEdit * m_pIanaType;
	QLineEdit * m_pFileMask;
	QLineEdit * m_pSavePath;
	QLineEdit * m_pCommandline;
	QLineEdit * m_pMagicBytes;
	QLineEdit * m_pRemoteExecCommandline;
	QLineEdit * m_pIcon;
	QPushButton * m_pDelete;
	MediaTypeTreeWidgetItem * m_pLastItem;

private:
	void fillTreeWidget();
	void saveLastItem();
	void enableOrDisable();
	void setLineEdits();
private slots:
	void currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem * prev);
	void newMediaType();
	void delMediaType();

public:
	virtual void commit();
};

#endif //!_OPTW_IDENTITY_H_
