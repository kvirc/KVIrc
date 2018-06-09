//=============================================================================
//
//   File : KviMaskEditor.cpp
//   Creation date : Tue Aug 30 2000 12:24:59 by Szymon Stefanek
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

#include "KviMaskEditor.h"
#include "KviChannelWindow.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "KviQString.h"
#include "KviWindowToolWidget.h"

#include <QDateTime>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>

KviMaskItem::KviMaskItem(QTreeWidget * pParent, KviMaskEntry * pEntry)
    : QTreeWidgetItem(pParent), m_Mask(*pEntry)
{
	QString szDate;
	QDateTime date;
	date.setTime_t(mask()->uSetAt);
	switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
	{
		case 0:
			// this is the equivalent to an empty date.toString() call, but it's needed
			// to ensure qt4 will use the default() locale and not the system() one
			szDate = QLocale().toString(date, "ddd MMM d hh:mm:ss yyyy");
			break;
		case 1:
			szDate = date.toString(Qt::ISODate);
			break;
		case 2:
			szDate = date.toString(Qt::SystemLocaleShortDate);
			break;
	}
	setText(0, mask()->szMask);
	setText(1, mask()->szSetBy);
	setText(2, szDate);
}

KviMaskItem::~KviMaskItem()
    = default;

KviMaskInputDialog::KviMaskInputDialog(const QString & szMask, KviMaskEditor * pEditor, KviChannelWindow * pChannel)
    : QDialog(pEditor)
{
	m_pChannel = pChannel;
	m_pEditor = pEditor;
	setModal(true);
	m_szOldMask = szMask;

	setWindowTitle(__tr2qs("Mask Editor - KVIrc"));

	QGridLayout * g = new QGridLayout(this);

	QLabel * tl = new QLabel(__tr2qs("Insert a nickname or a mask matching *!*@*"), this);
	g->addWidget(tl, 0, 0, 1, 4);

	m_pEdit = new QLineEdit(szMask, this);
	g->addWidget(m_pEdit, 1, 0, 1, 4);

	m_pOkButton = new QPushButton(__tr2qs("OK"), this);
	connect(m_pOkButton, SIGNAL(clicked()), this, SLOT(accept()));
	g->addWidget(m_pOkButton, 2, 1);
	m_pOkButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));

	m_pChancelButton = new QPushButton(__tr2qs("Cancel"), this);
	connect(m_pChancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	g->addWidget(m_pChancelButton, 2, 2);
	m_pChancelButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)));
}

KviMaskInputDialog::~KviMaskInputDialog()
    = default;

void KviMaskInputDialog::accept()
{
	if(m_szOldMask.isEmpty())
	{
		m_pChannel->connection()->sendFmtData("MODE %s +%c %s",
		    m_pChannel->connection()->encodeText(m_pChannel->target()).data(),
		    m_pEditor->flag(),
		    m_pChannel->connection()->encodeText(m_pEdit->text()).data());
	}
	else
	{
		m_pChannel->connection()->sendFmtData("MODE %s -%c+%c %s %s",
		    m_pChannel->connection()->encodeText(m_pChannel->target()).data(),
		    m_pEditor->flag(),
		    m_pEditor->flag(),
		    m_pChannel->connection()->encodeText(m_szOldMask).data(),
		    m_pChannel->connection()->encodeText(m_pEdit->text()).data());
	}
	QDialog::accept();
}

KviMaskEditor::KviMaskEditor(QWidget * par, KviChannelWindow * pChannel, KviWindowToolPageButton * button, std::vector<KviMaskEntry *> maskList, char cMode, const char * name)
    : KviWindowToolWidget(par, button)
{
	setObjectName(name);
	m_pChannel = pChannel;

	setFocusPolicy(Qt::ClickFocus);

	QGridLayout * g = new QGridLayout(this);

	m_cFlag = cMode;

	KviIrcConnectionServerInfo * pServerInfo = nullptr;
	QString szDescription = "";
	if(m_pChannel)
		pServerInfo = m_pChannel->serverInfo();
	if(pServerInfo)
		szDescription = pServerInfo->getChannelModeDescription(cMode);
	if(szDescription.isEmpty())
		szDescription = __tr2qs("Mode \"%1\" masks").arg(cMode);
	switch(cMode)
	{
		case 'b':
			m_eIcon = KviIconManager::Ban;
			break;
		case 'e':
			m_eIcon = KviIconManager::BanExcept;
			break;
		case 'I':
			m_eIcon = KviIconManager::InviteExcept;
			break;
		case 'a':
			m_eIcon = KviIconManager::ChanAdmin;
			break;
		case 'q':
			// this could also be quiet bans..
			m_eIcon = KviIconManager::Kick;
			break;
		default:
			m_eIcon = KviIconManager::Ban;
			break;
	}

	KviTalHBox * pTitleLayout = new KviTalHBox(this);
	g->addWidget(pTitleLayout, 0, 0);

	QLabel * l = new QLabel("", pTitleLayout);
	l->setPixmap(*(g_pIconManager->getSmallIcon(m_eIcon)));

	l = new QLabel(szDescription, pTitleLayout);

	QFrame * pFrame = new QFrame(this);
	pFrame->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	g->addWidget(pFrame, 1, 0, 1, -1);

	KviTalHBox * hb = new KviTalHBox(this);
	g->addWidget(hb, 2, 0, 1, 2);

	new QLabel(__tr2qs("Filter:"), hb);
	m_pSearch = new QLineEdit(hb);
	connect(m_pSearch, SIGNAL(textChanged(const QString &)), this, SLOT(searchTextChanged(const QString &)));

	l = new QLabel(__tr2qs("Use double-click to edit item"), this);
	g->addWidget(l, 2, 1);
	g->addWidget(l, 3, 0, 1, 2);

	m_pMaskBox = new QTreeWidget(this);
	m_pMaskBox->setFocusPolicy(Qt::ClickFocus);
	m_pMaskBox->setFocusProxy(this);
	m_pMaskBox->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	m_pMaskBox->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QStringList columnLabels;
	m_pMaskBox->setColumnCount(3);

	columnLabels.append(__tr2qs("Mask"));
	columnLabels.append(__tr2qs("Set by"));
	columnLabels.append(__tr2qs("Set at"));
	m_pMaskBox->setHeaderLabels(columnLabels);
	m_pMaskBox->setAllColumnsShowFocus(true);
	m_pMaskBox->setSortingEnabled(true);
	connect(m_pMaskBox, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
	g->addWidget(m_pMaskBox, 4, 0, 1, 2);

	m_pRemoveMask = new QPushButton(__tr2qs("Re&move"), this);

	m_pRemoveMask->setFocusPolicy(Qt::ClickFocus);
	m_pRemoveMask->setFocusProxy(this);
	g->addWidget(m_pRemoveMask, 5, 1);
	connect(m_pRemoveMask, SIGNAL(clicked()), this, SLOT(removeClicked()));
	m_pRemoveMask->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::DeleteItem)));

	m_pAddButton = new QPushButton(__tr2qs("&Add"), this);
	g->addWidget(m_pAddButton, 5, 0);
	connect(m_pAddButton, SIGNAL(clicked()), this, SLOT(addClicked()));
	m_pAddButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::NewItem)));

	g->setColumnStretch(1, 1);

	if(m_pChannel && (!m_pChannel->connection()))
	{
		m_pRemoveMask->setEnabled(false);
		m_pAddButton->setEnabled(false);
		return;
	}

	for(auto & e : maskList)
		addMask(e);

	updateOpStatus();

	if(m_pChannel)
		connect(m_pChannel, SIGNAL(opStatusChanged()), this, SLOT(updateOpStatus()));
}

KviMaskEditor::~KviMaskEditor()
    = default;

void KviMaskEditor::updateOpStatus()
{
	bool isEnabled = true;
	if(m_pChannel)
	{
		if(!(m_pChannel->isMeHalfOp(true) || m_pChannel->connection()->userInfo()->hasUserMode('o') || m_pChannel->connection()->userInfo()->hasUserMode('O')))
			isEnabled = false;
	}

	if(m_pRemoveMask)
		m_pRemoveMask->setEnabled(isEnabled);
	if(m_pAddButton)
		m_pAddButton->setEnabled(isEnabled);
}

void KviMaskEditor::searchTextChanged(const QString & text)
{
	KviMaskItem * pMaskItem;
	bool bEmpty = text.isEmpty();
	for(int i = 0; i < m_pMaskBox->topLevelItemCount(); i++)
	{
		pMaskItem = (KviMaskItem *)m_pMaskBox->topLevelItem(i);
		if(bEmpty)
		{
			pMaskItem->setHidden(false);
		}
		else
		{
			if(pMaskItem->mask()->szMask.contains(text))
				pMaskItem->setHidden(false);
			else
				pMaskItem->setHidden(true);
		}
	}
}

void KviMaskEditor::removeClicked()
{
	std::vector<KviMaskEntry *> l;

	KviMaskItem * it;
	for(int i = 0; i < m_pMaskBox->topLevelItemCount(); i++)
	{
		it = (KviMaskItem *)m_pMaskBox->topLevelItem(i);
		if(it->isSelected())
		{
			KviMaskEntry * e = new KviMaskEntry;
			e->szMask = it->mask()->szMask;
			e->szSetBy = it->mask()->szSetBy;
			e->uSetAt = it->mask()->uSetAt;
			l.push_back(e);
		}
	}
	if(l.size() > 0)
		emit removeMasks(this, l);
}

void KviMaskEditor::addClicked()
{
	if(m_pChannel && m_pChannel->connection())
	{
		if(m_pChannel->isMeHalfOp(true) || m_pChannel->connection()->userInfo()->hasUserMode('o') || m_pChannel->connection()->userInfo()->hasUserMode('O'))
		{
			KviMaskInputDialog * pDialog = new KviMaskInputDialog("", this, m_pChannel);
			pDialog->show();
		}
	}
}

void KviMaskEditor::addMask(KviMaskEntry * e)
{
	KviMaskItem * it;
	it = new KviMaskItem(m_pMaskBox, e);
	it->setIcon(0, *(g_pIconManager->getSmallIcon(m_eIcon)));
}

void KviMaskEditor::removeMask(KviMaskEntry * e)
{
	KviMaskItem * it;
	for(int i = 0; i < m_pMaskBox->topLevelItemCount(); i++)
	{
		it = (KviMaskItem *)m_pMaskBox->topLevelItem(i);
		if(KviQString::equalCI(it->mask()->szMask, e->szMask))
		{
			delete it;
			return;
		}
	}
}

void KviMaskEditor::clear()
{
	KviMaskItem * it;
	while(m_pMaskBox->topLevelItemCount() > 0)
	{
		it = (KviMaskItem *)m_pMaskBox->topLevelItem(0);
		if(it)
			delete it;
	}
}

void KviMaskEditor::itemDoubleClicked(QTreeWidgetItem * pItem, int)
{
	if(pItem && m_pChannel)
	{
		if(m_pChannel->isMeHalfOp(true) || m_pChannel->connection()->userInfo()->hasUserMode('o') || m_pChannel->connection()->userInfo()->hasUserMode('O'))
		{
			KviMaskInputDialog * pDialog = new KviMaskInputDialog(pItem->text(0), this, m_pChannel);
			pDialog->show();
		}
	}
}
