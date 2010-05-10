//=============================================================================
//
//   File : kvi_maskeditor.cpp
//   Creation date : Tue Aug 30 2000 12:24:59 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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


#include "kvi_maskeditor.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_qstring.h"
#include "kvi_channel.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_toolwindows_container.h"
#include "kvi_channel.h"
#include "kvi_options.h"

#include <QLayout>
#include <QLabel>
#include <QValidator>

KviMaskItem::KviMaskItem(QTreeWidget * pParent, KviMaskEntry * pEntry)
:QTreeWidgetItem(pParent), m_Mask(*pEntry)
{
	QString szDate;
	QDateTime date;
	date.setTime_t(mask()->uSetAt);
	switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
	{
		case 0:
			szDate = date.toString();
			break;
		case 1:
			szDate = date.toString(Qt::ISODate);
			break;
		case 2:
			szDate = date.toString(Qt::SystemLocaleDate);
			break;
	}
	setText(0,mask()->szMask);
	setText(1,mask()->szSetBy);
	setText(2,szDate);
}

KviMaskItem::~KviMaskItem()
{
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
	g->addWidget(tl,0,0,1,4);

	m_pEdit=new QLineEdit(szMask,this);
	g->addWidget(m_pEdit,1,0,1,4);

	m_pOkButton= new QPushButton(__tr2qs("Ok"),this);
	connect(m_pOkButton,SIGNAL(clicked()), this, SLOT(accept()));
	g->addWidget(m_pOkButton,2,1);
	m_pOkButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));

	m_pChancelButton= new QPushButton(__tr2qs("Cancel"),this);
	connect(m_pChancelButton,SIGNAL(clicked()), this, SLOT(reject()));
	g->addWidget(m_pChancelButton,2,2);
	m_pChancelButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));

	QRegExp rx( "*!*@*", Qt::CaseInsensitive,QRegExp::Wildcard );
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
			m_pChannel->connection()->encodeText(m_pChannel->target()).data(),
			m_pEditor->flag(),
			m_pChannel->connection()->encodeText(m_pEdit->text()).data()
			);
	} else {
		m_pChannel->connection()->sendFmtData("MODE %s -%c+%c %s %s",
			m_pChannel->connection()->encodeText(m_pChannel->target()).data(),
			m_pEditor->flag(),
			m_pEditor->flag(),
			m_pChannel->connection()->encodeText(m_szOldMask).data(),
			m_pChannel->connection()->encodeText(m_pEdit->text()).data()
			);
	}
	QDialog::accept();
}

KviMaskEditor::KviMaskEditor(QWidget * par,KviWindowToolPageButton* button,KviPointerList<KviMaskEntry> * maskList,char flag,const char *)
: KviWindowToolWidget(par,button)
{
	bool isEnabled=1;

	QObject * w = parent();
	while(w)
	{
		if(w->inherits("KviChannel"))
		{
			KviChannel *chan = ((KviChannel *)w);
			if(!( chan->isMeHalfOp() || chan->isMeOp() || chan->isMeChanOwner() || chan->isMeChanAdmin() ) )
				isEnabled=0;
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
		case 'q':
			txt = __tr2qs("Active Quiet Bans");
			m_iIconId = KVI_SMALLICON_BAN;
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
	g->addWidget(hb,1,0,1,2);

	new QLabel(__tr2qs("Filter:"),hb);
	m_pSearch = new QLineEdit(hb);
	connect(m_pSearch,SIGNAL(textChanged ( const QString & ) ),this,SLOT(searchTextChanged ( const QString & )));

	l = new QLabel(__tr2qs("Use doubleclick to edit item"),this);
	g->addWidget(l,1,1);
	g->addWidget(l,2,0,1,2);

	m_pMaskBox = new QTreeWidget(this);
	m_pMaskBox->setFocusPolicy(Qt::ClickFocus);
	m_pMaskBox->setFocusProxy(this);
	m_pMaskBox->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
	m_pMaskBox->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QStringList columnLabels;
	m_pMaskBox->setColumnCount(3);

	columnLabels.append(__tr2qs("Mask"));
	columnLabels.append(__tr2qs("Set by"));
	columnLabels.append(__tr2qs("Set at"));
	m_pMaskBox->setHeaderLabels(columnLabels);
	m_pMaskBox->setAllColumnsShowFocus(true);
	m_pMaskBox->setSortingEnabled(true);
	connect(m_pMaskBox,SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)),this,SLOT(itemDoubleClicked( QTreeWidgetItem *,int)));
	g->addWidget(m_pMaskBox,3,0,1,2);

	m_pRemoveMask  = new QPushButton(__tr2qs("Re&move"),this);
	m_pRemoveMask->setEnabled(isEnabled);
	m_pRemoveMask->setFocusPolicy(Qt::ClickFocus);
	m_pRemoveMask->setFocusProxy(this);
	g->addWidget(m_pRemoveMask,4,1);
	connect(m_pRemoveMask,SIGNAL(clicked()),this,SLOT(removeClicked()));
	m_pRemoveMask->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DELETEITEM)));

	m_pAddButton = new QPushButton(__tr2qs("&Add"),this);
	m_pAddButton->setEnabled(isEnabled);
	g->addWidget(m_pAddButton,4,0);
	connect(m_pAddButton,SIGNAL(clicked()),this,SLOT(addClicked()));
	m_pAddButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NEWITEM)));

	g->setColumnStretch(1,1);

	for(KviMaskEntry * e = maskList->first();e;e = maskList->next()) addMask(e);
}

KviMaskEditor::~KviMaskEditor()
{

}

void KviMaskEditor::searchTextChanged ( const QString & text)
{
	KviMaskItem *pMaskItem;
	bool bEmpty = text.isEmpty();
	for (int i=0;i<m_pMaskBox->topLevelItemCount();i++)
	{
		pMaskItem = (KviMaskItem *)m_pMaskBox->topLevelItem(i);
		if(bEmpty)
		{
			pMaskItem->setHidden(false);
		} else {
			if(pMaskItem->mask()->szMask.contains(text))
				pMaskItem->setHidden(false);
			else
				pMaskItem->setHidden(true);
		}
	}
}

void KviMaskEditor::removeClicked()
{
	KviPointerList<KviMaskEntry>  * l = new KviPointerList<KviMaskEntry>;
	l->setAutoDelete(true);
	KviMaskItem * it;
	for (int i=0;i<m_pMaskBox->topLevelItemCount();i++)
	{
		it=(KviMaskItem *)m_pMaskBox->topLevelItem(i);
		if(it->isSelected())
		{
			KviMaskEntry * e = new KviMaskEntry;
			e->szMask  = it->mask()->szMask;
			e->szSetBy = it->mask()->szSetBy;
			e->uSetAt =  it->mask()->uSetAt;
			l->append(e);
		}
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
	KviMaskItem *it;
	it = new KviMaskItem(m_pMaskBox,e);
	it->setIcon(0,*(g_pIconManager->getSmallIcon(m_iIconId)));
}

void KviMaskEditor::removeMask(KviMaskEntry *e)
{
	KviMaskItem * it;
	for (int i=0;i<m_pMaskBox->topLevelItemCount();i++)
	{
		it=(KviMaskItem *)m_pMaskBox->topLevelItem(i);
		if(KviQString::equalCI(it->mask()->szMask,e->szMask))
		{
			delete it;
			return;
		}
	}
}

void KviMaskEditor::clear()
{
	KviMaskItem * it;
	while(m_pMaskBox->topLevelItemCount()>0)
	{
		it=(KviMaskItem *)m_pMaskBox->topLevelItem(0);
		if(it) delete it;
	}
}

void KviMaskEditor::itemDoubleClicked( QTreeWidgetItem * pItem, int )
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
