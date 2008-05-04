//=============================================================================
//
//   File : kvi_maskeditor.cpp
//   Creation date : Tue Aug 30 2000 12:24:59 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

#define __KVIRC__
#include "kvi_maskeditor.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_qstring.h"
#include "kvi_channel.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_toolwindows_container.h"
#include "kvi_channel.h"

#include <QLayout>
#include <QLabel>
#include <QValidator>

KviMaskItem::KviMaskItem(KviTalListView* parent,KviMaskEntry* entry)
:KviTalListViewItem(parent), m_Mask(*entry)
{
	QDateTime date;
	date.setTime_t(mask()->uSetAt);
	setText(0,mask()->szMask);
	setText(1,mask()->szSetBy);
	setText(2,date.toString());
}

KviMaskItem::~KviMaskItem()
{
}

int KviMaskItem::compare ( KviTalListViewItem * i, int col, bool ascending ) const
{
	if(col==2)
	{
		if( ((KviMaskItem*)i)->mask()->uSetAt > m_Mask.uSetAt )  return -1;
		if( ((KviMaskItem*)i)->mask()->uSetAt == m_Mask.uSetAt ) return  0;
		if( ((KviMaskItem*)i)->mask()->uSetAt < m_Mask.uSetAt )  return  1;
	}
	return KviTalListViewItem::compare(i,col,ascending);
}

KviMaskInputDialog::KviMaskInputDialog(const QString &szMask,KviMaskEditor* pEditor,KviChannel * pChannel)
:QDialog(pEditor)
{
	m_pChannel=pChannel;
	m_pEditor=pEditor;
	setModal(1);
	m_szOldMask=szMask;
	
	setWindowTitle(__tr2qs("Mask editor - KVirc"));
	
	QGridLayout * g = new QGridLayout(this);
	
	QLabel * tl = new QLabel(__tr2qs("New mask must match an *!*@* expression"),this);
	g->addMultiCellWidget(tl,0,0,0,3);
	
	m_pEdit=new QLineEdit(szMask,this);
	g->addMultiCellWidget(m_pEdit,1,1,0,3);
	
	m_pOkButton= new QPushButton(__tr2qs("Ok"),this);
	connect(m_pOkButton,SIGNAL(clicked()), this, SLOT(accept()));
	g->addWidget(m_pOkButton,2,1);
	m_pOkButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	
	m_pChancelButton= new QPushButton(__tr2qs("Cancel"),this);
	connect(m_pChancelButton,SIGNAL(clicked()), this, SLOT(reject()));
	g->addWidget(m_pChancelButton,2,2);
	m_pChancelButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
	
	QRegExp rx( "*!*@*", false,true );
	QValidator* validator = new QRegExpValidator( rx, this );
	
	m_pEdit->setValidator( validator );
}

KviMaskInputDialog::~KviMaskInputDialog()
{
}

void KviMaskInputDialog::accept()
{
	if(m_szOldMask.isEmpty()) 
	{
		m_pChannel->connection()->sendFmtData("MODE %s +%c %s",
			m_pChannel->connection()->encodeText(m_pChannel->name()).data(),
			m_pEditor->flag(),
			m_pChannel->connection()->encodeText(m_pEdit->text()).data()
			);
	} else {
		m_pChannel->connection()->sendFmtData("MODE %s -%c+%c %s %s",
			m_pChannel->connection()->encodeText(m_pChannel->name()).data(),
			m_pEditor->flag(),
			m_pEditor->flag(),
			m_pChannel->connection()->encodeText(m_szOldMask).data(),
			m_pChannel->connection()->encodeText(m_pEdit->text()).data()
			);
	}
	QDialog::accept();
}

KviMaskEditor::KviMaskEditor(QWidget * par,KviWindowToolPageButton* button,KviPointerList<KviMaskEntry> * maskList,char flag,const char * nam)
: KviWindowToolWidget(par,button)
{
	bool isEnabled=1;
	
	QObject * w = parent();
	while(w)
	{
		if(w->inherits("KviChannel"))
		{
			KviChannel *chan = ((KviChannel *)w);
			if(!( chan->isMeHalfOp() || chan->isMeOp() || chan->isMeChanOwner() || chan->isMeChanAdmin() || chan->connection()->userInfo()->hasUserMode('o') || chan->connection()->userInfo()->hasUserMode('O') ) )  isEnabled=0;
			break;
		}
	w = w->parent();
	}

	setFocusPolicy(Qt::ClickFocus);

	QGridLayout *g = new QGridLayout(this);

	m_cFlag = flag;

	QString txt;
	switch(flag)
	{
		case 'b':
			txt = __tr2qs("Active Bans");
			m_iIconId = KVI_SMALLICON_BAN;
		break;
		case 'I':
			txt = __tr2qs("Active Invite Exceptions");
			m_iIconId = KVI_SMALLICON_INVITEEXCEPT;
		break;
		case 'e':
			txt = __tr2qs("Active Ban Exceptions");
			m_iIconId = KVI_SMALLICON_BANEXCEPT;
		break;
		default:
			txt = "?";
			m_iIconId = KVI_SMALLICON_UNHANDLED;
		break;
	}

	QLabel * l = new QLabel("",this);
	l->setPixmap(*(g_pIconManager->getSmallIcon(m_iIconId)));
	g->addWidget(l,0,0);

	l = new QLabel(txt,this);
	g->addWidget(l,0,1);

	KviTalHBox * hb = new KviTalHBox(this);
	g->addMultiCellWidget(hb,1,1,0,1);

	new QLabel(__tr2qs("Filter:"),hb);
	m_pSearch = new QLineEdit(hb);
	connect(m_pSearch,SIGNAL(textChanged ( const QString & ) ),this,SLOT(searchTextChanged ( const QString & )));

	l = new QLabel(__tr2qs("Use doubleclick to edit item"),this);
	g->addWidget(l,1,1);
	g->addMultiCellWidget(l,2,2,0,1);
	
	m_pMaskBox = new KviTalListView(this);
	m_pMaskBox->setFocusPolicy(Qt::ClickFocus);
	m_pMaskBox->setFocusProxy(this);
	m_pMaskBox->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
	m_pMaskBox->addColumn(__tr2qs("Mask"));
	m_pMaskBox->addColumn(__tr2qs("Set by"));
	m_pMaskBox->addColumn(__tr2qs("Set at"));
	m_pMaskBox->setMultiSelection(true);
	m_pMaskBox->setAllColumnsShowFocus(true);
	m_pMaskBox->setShowSortIndicator(true);
	m_pMaskBox->setSorting(2,false);
	connect(m_pMaskBox,SIGNAL(doubleClicked ( KviTalListViewItem * )),this,SLOT(listViewDoubleClicked( KviTalListViewItem * )));
	g->addMultiCellWidget(m_pMaskBox,3,3,0,1);

	m_pRemoveMask  = new QPushButton(__tr2qs("Re&move"),this);
	m_pRemoveMask->setEnabled(isEnabled);
	m_pRemoveMask->setFocusPolicy(Qt::ClickFocus);
	m_pRemoveMask->setFocusProxy(this);
	g->addWidget(m_pRemoveMask,4,1);
	connect(m_pRemoveMask,SIGNAL(clicked()),this,SLOT(removeClicked()));
	m_pRemoveMask->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DELETEITEM)));
	
	m_pAddButton = new QPushButton(__tr2qs("Add"),this);
	m_pAddButton->setEnabled(isEnabled);
	m_pAddButton->setFocusPolicy(Qt::ClickFocus);
	m_pAddButton->setFocusProxy(this);
	g->addWidget(m_pAddButton,4,0);
	connect(m_pAddButton,SIGNAL(clicked()),this,SLOT(addClicked()));
	m_pAddButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NEWITEM)));
	
	g->setColumnStretch(3,1);

	for(KviMaskEntry * e = maskList->first();e;e = maskList->next()) addMask(e);
	registerSelf();
}

KviMaskEditor::~KviMaskEditor()
{

}

void KviMaskEditor::searchTextChanged ( const QString & text)
{
	KviTalListViewItem *pItem=m_pMaskBox->firstChild();
	KviMaskItem *pMaskItem;
	bool bEmpty = text.isEmpty();
	while(pItem)
	{
		pMaskItem = (KviMaskItem *)pItem;
		if(bEmpty)
		{
			pMaskItem->setVisible(true);
		} else {
			if(pMaskItem->mask()->szMask.contains(text))
				pMaskItem->setVisible(true);
			else 
				pMaskItem->setVisible(false);
		}
		pItem=pItem->nextSibling();
	}
}

void KviMaskEditor::removeClicked()
{
	KviPointerList<KviMaskEntry>  * l = new KviPointerList<KviMaskEntry>;
	l->setAutoDelete(true);
	KviMaskItem * it = (KviMaskItem *)(m_pMaskBox->firstChild());
	while(it)
	{
		if(it->isSelected())
		{
			KviMaskEntry * e = new KviMaskEntry;
			e->szMask  = it->mask()->szMask;
			e->szSetBy = it->mask()->szSetBy;
			e->uSetAt =  it->mask()->uSetAt;
			l->append(e);
		}
		it = (KviMaskItem *)(it->nextSibling());
	}
	if(l->count() > 0)emit removeMasks(this,l);
	delete l;
}

void KviMaskEditor::addClicked()
{
	QObject * w = parent();
	while(w)
	{
		if(w->inherits("KviChannel"))
		{
			KviChannel *chan = ((KviChannel *)w);
			if(chan->isMeHalfOp() || chan->isMeOp() || chan->isMeChanAdmin() || chan->isMeChanOwner() || chan->connection()->userInfo()->hasUserMode('o') || chan->connection()->userInfo()->hasUserMode('O'))
			{
				KviMaskInputDialog* pDialog=new KviMaskInputDialog("",this,chan);
				pDialog->show();
			}
			break;
		}
	w = w->parent();
	}
}

void KviMaskEditor::addMask(KviMaskEntry *e)
{
//	debug("%s %s %i",__FILE__,__FUNCTION__,__LINE__);
	KviMaskItem *it;
	it = new KviMaskItem(m_pMaskBox,e);
	it->setPixmap(0,*(g_pIconManager->getSmallIcon(m_iIconId)));
}

void KviMaskEditor::removeMask(KviMaskEntry *e)
{
	KviMaskItem * it =(KviMaskItem *)(m_pMaskBox->firstChild());
	while(it)
	{
		if(KviQString::equalCI(it->mask()->szMask,e->szMask))
		{
			delete it;
			return;
		}
		it = (KviMaskItem *)(it->nextSibling());
	}
}

void KviMaskEditor::listViewDoubleClicked( KviTalListViewItem * pItem)
{
	if(pItem)
	{
		QObject * w = parent();
		while(w)
		{
			if(w->inherits("KviChannel"))
			{
				KviChannel *chan = ((KviChannel *)w);
				if(chan->isMeHalfOp() || chan->isMeOp() || chan->isMeChanAdmin() || chan->isMeChanOwner() || chan->connection()->userInfo()->hasUserMode('o') || chan->connection()->userInfo()->hasUserMode('O'))
				{
					KviMaskInputDialog* pDialog=new KviMaskInputDialog(pItem->text(0),this,chan);
					pDialog->show();
				}
				break;
			}
		w = w->parent();
		}
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_maskeditor.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
