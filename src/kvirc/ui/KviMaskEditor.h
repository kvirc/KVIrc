#ifndef _KVI_MASKEDITOR_H_
#define _KVI_MASKEDITOR_H_
//=============================================================================
//
//   File : KviMaskEditor.h
//   Creation date : Tue Aug 30 2000 12:20:10 by Szymon Stefanek
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
//=============================================================================

#include "kvi_settings.h"
#include "KviWindowToolWidget.h"
#include "KviIconManager.h"

#include <QDialog>
#include <QString>
#include <QTreeWidget>
#include <QWidget>

#include <vector>

class KviChannelWindow;
class KviMaskEditor;
class QLineEdit;
class QPushButton;

struct KviMaskEntry
{
	QString szMask;
	QString szSetBy;
	unsigned int uSetAt;
};

class KviMaskItem : public QTreeWidgetItem
{
public:
	KviMaskItem(QTreeWidget * parent, KviMaskEntry * entry);
	~KviMaskItem();

	KviMaskEntry * mask() { return &m_Mask; };
protected:
	KviMaskEntry m_Mask;
	bool operator<(const QTreeWidgetItem & other) const
	{
		// This is the sorting function for KviMaskItem
		switch(treeWidget()->sortColumn())
		{
			case 0:
				return m_Mask.szMask < ((KviMaskItem *)&other)->mask()->szMask;
				break;
			case 1:
				return m_Mask.szSetBy < ((KviMaskItem *)&other)->mask()->szSetBy;
				break;
			case 2:
				return m_Mask.uSetAt < ((KviMaskItem *)&other)->mask()->uSetAt;
				break;
		}
		return false; //make compiler happy
	}
};

class KviMaskInputDialog : public QDialog
{
	Q_OBJECT
public:
	KviMaskInputDialog(const QString & szMask, KviMaskEditor * pEditor, KviChannelWindow * pChannel);
	~KviMaskInputDialog();

protected:
	QLineEdit * m_pEdit;
	QPushButton * m_pOkButton;
	QPushButton * m_pChancelButton;
	QString m_szOldMask;
	KviChannelWindow * m_pChannel;
	KviMaskEditor * m_pEditor;
protected slots:
	void accept() override;
};

class KVIRC_API KviMaskEditor : public KviWindowToolWidget
{
	Q_OBJECT
public:
	KviMaskEditor(QWidget * par, KviChannelWindow * pChannel, KviWindowToolPageButton * button, std::vector<KviMaskEntry *> maskList,
	    char cMode, const char * name);
	~KviMaskEditor();

protected:
	KviChannelWindow * m_pChannel;
	QTreeWidget * m_pMaskBox;
	QPushButton * m_pRemoveMask;
	QPushButton * m_pAddButton;
	QLineEdit * m_pSearch;
	char m_cFlag;
	KviIconManager::SmallIcon m_eIcon;

public:
	char flag() { return m_cFlag; };
	void addMask(KviMaskEntry * e);
	void removeMask(KviMaskEntry * e);
	void clear();
protected slots:
	void removeClicked();
	void addClicked();
	void itemDoubleClicked(QTreeWidgetItem *, int);
	void searchTextChanged(const QString &);
	void updateOpStatus();
signals:
	void removeMasks(KviMaskEditor *, std::vector<KviMaskEntry *>);
};

#endif //_KVI_MASKEDITOR_H_
