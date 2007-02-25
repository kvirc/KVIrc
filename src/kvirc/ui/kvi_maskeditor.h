#ifndef _KVI_MASKEDITOR_H_
#define _KVI_MASKEDITOR_H_

//
//   File : kvi_maskeditor.h
//   Creation date : Tue Aug 30 2000 12:20:10 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include <qdialog.h> 
#include <qlineedit.h> 
#include <qwidget.h>
#include "kvi_tal_listview.h"
#include <qpushbutton.h>
#include "kvi_list.h"

#include "kvi_string.h"
#include "kvi_toolwindows_container.h"

class KviMaskEditor;
class KviChannel;

typedef struct _KviMaskEntry
{
	QString szMask;
	QString szSetBy;
	unsigned int uSetAt;
} KviMaskEntry;

class KviMaskItem: public KviTalListViewItem
{
public:
	KviMaskItem(KviTalListView* parent,KviMaskEntry* entry);
	~KviMaskItem();
	
	KviMaskEntry* mask() { return &m_Mask; };
	
	virtual int compare ( KviTalListViewItem * i, int col, bool ascending ) const;
protected:
	KviMaskEntry m_Mask;
	
};

class KviMaskInputDialog : public QDialog
{
	Q_OBJECT
public:
	KviMaskInputDialog(const QString &szMask,KviMaskEditor* pEditor,KviChannel * pChannel);
	~KviMaskInputDialog();
protected:
	QLineEdit     * m_pEdit;
	QPushButton   * m_pOkButton;
	QPushButton   * m_pChancelButton;
	QString 	m_szOldMask;
	KviChannel    * m_pChannel;
	KviMaskEditor * m_pEditor;
protected slots:
	virtual void accept();
};

class KVIRC_API KviMaskEditor : public KviWindowToolWidget
{
	Q_OBJECT
public:
	KviMaskEditor(QWidget * par,KviWindowToolPageButton* button,KviPtrList<KviMaskEntry> * maskList,
		char flag,const char * nam);
	~KviMaskEditor();
protected:
	KviTalListView      * m_pMaskBox;
	QPushButton         * m_pRemoveMask;
	QPushButton   	    * m_pAddButton;
	QLineEdit           * m_pSearch;
	char                  m_cFlag;
	int		              m_iIconId;
public:
	char flag(){ return m_cFlag; };
	void addMask(KviMaskEntry *e);
	void removeMask(KviMaskEntry *e);
protected slots:
	void removeClicked();
	void addClicked();
	void listViewDoubleClicked( KviTalListViewItem * );
	void searchTextChanged ( const QString & );
signals:
	void removeMasks(KviMaskEditor *,KviPtrList<KviMaskEntry> *);
};

#endif //_KVI_MASKEDITOR_H_
