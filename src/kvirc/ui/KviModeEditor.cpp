//=============================================================================
//
//   File : KviModeEditor.cpp
//   Creation date : Sat Apr 14 2001 13:52:11 by Szymon Stefanek
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

#include "KviModeEditor.h"
#include "KviChannelWindow.h"
#include "KviIconManager.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviLocale.h"

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QString>

KviModeEditor::KviModeEditor(QWidget * par, KviWindowToolPageButton * button, const char * name, KviChannelWindow * pChan)
    : KviWindowToolWidget(par, button)
{
	setObjectName(name);
	m_pChannel = pChan;

	QGridLayout * pMasterLayout = new QGridLayout(this);

	setFocusPolicy(Qt::ClickFocus);

	QScrollArea * pScrollArea = new QScrollArea(this);
	pScrollArea->viewport()->setBackgroundRole(QPalette::Background);
	pMasterLayout->addWidget(pScrollArea, 0, 0);

	pMasterLayout->setRowStretch(1, 1);

	m_pButton = new QPushButton("", this);
	pMasterLayout->addWidget(m_pButton, 1, 0);
	connect(m_pButton, SIGNAL(clicked()), this, SLOT(commit()));

	if(!m_pChannel || !m_pChannel->connection())
		m_pButton->setText(__tr2qs("Close"));

	QWidget * pBackground = new QWidget(pScrollArea->viewport());

	QGridLayout * g = new QGridLayout(pBackground);

	QLabel * l = new QLabel("", pBackground);
	l->setPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::Mode)));
	g->addWidget(l, 0, 0);

	l = new QLabel(__tr2qs("Channel modes"), pBackground);

	g->addWidget(l, 0, 1, 1, 1);

	QFrame * f = new QFrame(pBackground);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	g->addWidget(f, 1, 0, 1, 3);

	QCheckBox * pCheckBox = nullptr;
	QLineEdit * pLineEdit = nullptr;
	int iRow = 1;
	QString szTmp;
	QString cDesc;
	char cMode = 0;

	//NOTE: this is a fallback is for some reason we don't have a serverInfo() struct available fot this connection

	// The connection is dead and the context was destroyed, don't guess on what was there
	if(!m_pChannel || !m_pChannel->connection())
		return;

	// first, the basic checkable modes pstnmi
	QString szModes = "pstnmi";

	while(!szModes.isEmpty())
	{
		cMode = szModes[0].unicode();
		szModes.remove(0, 1);

		szTmp = QString("%1: %2").arg(cMode).arg(*(getModeDescription(cMode)));
		pCheckBox = new QCheckBox(szTmp, pBackground);
		m_pCheckBoxes.insert(cMode, pCheckBox);
		if(pChan)
			pCheckBox->setChecked(pChan->plainChannelMode().contains(cMode));
		iRow++;
		g->addWidget(pCheckBox, iRow, 0, 1, 3);
	}

	// second, che basic modes with parameter lk
	szModes = "lk";

	while(!szModes.isEmpty())
	{
		cMode = szModes[0].unicode();
		szModes.remove(0, 1);

		szTmp = QString("%1: %2").arg(cMode).arg(*(getModeDescription(cMode)));
		pCheckBox = new QCheckBox(szTmp, pBackground);
		m_pCheckBoxes.insert(cMode, pCheckBox);
		iRow++;
		g->addWidget(pCheckBox, iRow, 0, 1, 3);

		connect(pCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxToggled(bool)));
		pLineEdit = new QLineEdit(pBackground);
		m_pLineEdits.insert(cMode, pLineEdit);
		iRow++;
		g->addWidget(pLineEdit, iRow, 1, 1, 2);

		if(pChan)
		{
			if(pChan->hasChannelMode(cMode))
			{
				pCheckBox->setChecked(true);
				pLineEdit->setText(pChan->channelModeParam(cMode));
			}
			else
			{
				pLineEdit->setEnabled(false);
			}
		}
	}

	// third, check if the we have any info about other modes supported by the server
	KviIrcConnectionServerInfo * pServerInfo = nullptr;
	if(m_pChannel)
		pServerInfo = m_pChannel->serverInfo();
	if(!pServerInfo)
	{
		g->setRowStretch(++iRow, 1);
		g->setColumnStretch(2, 1);
		pScrollArea->setWidget(pBackground);
		return;
	}

	// 4th, more plain modes supported by the server
	szModes = pServerInfo->supportedPlainModes();

	// remove modes that we already implemented
	szModes.remove("p");
	szModes.remove("s");
	szModes.remove("t");
	szModes.remove("n");
	szModes.remove("m");
	szModes.remove("i");

	while(!szModes.isEmpty())
	{
		cMode = szModes[0].unicode();
		szModes.remove(0, 1);

		cDesc = *(getModeDescription(cMode));

		szTmp = QString("%1: %2").arg(cMode).arg(!cDesc.isEmpty() ? cDesc : "Unknown");
		pCheckBox = new QCheckBox(szTmp, pBackground);
		m_pCheckBoxes.insert(cMode, pCheckBox);
		if(pChan)
			pCheckBox->setChecked(pChan->plainChannelMode().contains(cMode));

		iRow++;
		g->addWidget(pCheckBox, iRow, 0, 1, 3);
	}

	iRow++;

	// parameterized modes
	szModes = pServerInfo->supportedParameterModes();
	szModes.append(pServerInfo->supportedParameterWhenSetModes());

	// remove modes that we already implemented
	szModes.remove("k");
	szModes.remove("l");

	while(!szModes.isEmpty())
	{
		cMode = szModes[0].unicode();
		szModes.remove(0, 1);

		cDesc = *(getModeDescription(cMode));

		szTmp = QString("%1: %2").arg(cMode).arg(!cDesc.isEmpty() ? cDesc : "Unknown");
		pCheckBox = new QCheckBox(szTmp, pBackground);
		m_pCheckBoxes.insert(cMode, pCheckBox);
		iRow++;
		g->addWidget(pCheckBox, iRow, 0, 1, 3);

		connect(pCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxToggled(bool)));
		pLineEdit = new QLineEdit(pBackground);
		m_pLineEdits.insert(cMode, pLineEdit);
		iRow++;
		g->addWidget(pLineEdit, iRow, 1, 1, 2);

		if(pChan->hasChannelMode(cMode))
		{
			pCheckBox->setChecked(true);
			pLineEdit->setText(pChan->channelModeParam(cMode));
		}
		else
		{
			pLineEdit->setEnabled(false);
		}
	}

	g->setRowStretch(++iRow, 1);
	g->setColumnStretch(2, 1);
	pScrollArea->setWidget(pBackground);

	updateOpStatus();

	if(m_pChannel)
		connect(m_pChannel, SIGNAL(opStatusChanged()), this, SLOT(updateOpStatus()));
}

KviModeEditor::~KviModeEditor()
{
	qDeleteAll(m_pLineEdits);
	qDeleteAll(m_pCheckBoxes);
}

void KviModeEditor::updateOpStatus()
{
	bool isEnabled = true;
	if(m_pChannel)
	{
		if(!(m_pChannel->isMeHalfOp(true) || m_pChannel->connection()->userInfo()->hasUserMode('o') || m_pChannel->connection()->userInfo()->hasUserMode('O')))
			isEnabled = false;
	}

	if(m_pButton)
		m_pButton->setText(isEnabled ? __tr2qs("&Apply") : __tr2qs("Close"));

	foreach(QLineEdit * pLineEdit, m_pLineEdits)
		pLineEdit->setEnabled(isEnabled);

	foreach(QCheckBox * pCheckBox, m_pCheckBoxes)
		pCheckBox->setEnabled(isEnabled);
}

void KviModeEditor::checkBoxToggled(bool bChecked)
{
	// enable/disable the lineedit if any
	if(!sender())
		return;

	QChar c = m_pCheckBoxes.key((QCheckBox *)sender());
	if(c.isNull())
		return;

	QLineEdit * pLineEdit = m_pLineEdits.value(c.unicode());
	if(!pLineEdit)
		return;

	pLineEdit->setEnabled(bChecked);
}

void KviModeEditor::commit()
{
	if(!m_pChannel)
	{
		emit done();
		return;
	}

	QMap<char, QString> szPlusModes;
	QMap<char, QString> szMinusModes;

	QCheckBox * pCheckBox;
	QLineEdit * pLineEdit;
	char cMode = 0;

	QList<char> modes = m_pCheckBoxes.keys();

	for(int i = 0; i < modes.count(); ++i)
	{
		cMode = modes.at(i);
		pCheckBox = m_pCheckBoxes.value(cMode);
		if(!pCheckBox)
			continue; // wtf!?

		pLineEdit = m_pLineEdits.value(cMode);
		if(pLineEdit)
		{
			// mode with parameter (with an exception)
			if(pCheckBox->isChecked())
			{
				// mode is checked
				if(!m_pChannel->hasChannelMode(cMode) || (pLineEdit->text().trimmed() != m_pChannel->channelModeParam(cMode)))
				{
					// mode was not checked before, or the parameter has changed
					szPlusModes.insert(cMode, pLineEdit->text().trimmed());
				}
			}
			else
			{
				// mode is not checked
				if(m_pChannel->hasChannelMode(cMode))
				{
					// but it was checked before

					// checks if this specific mode does not need a parameter when set
					if(modeNeedsParameterOnlyWhenSet(cMode))
					{
						szMinusModes.insert(cMode, QString());
					}
					else
					{
						szMinusModes.insert(cMode, m_pChannel->channelModeParam(cMode));
					}
				}
			}
		}
		else
		{
			//mode without parameter
			if(pCheckBox->isChecked())
			{
				if(!m_pChannel->plainChannelMode().contains(cMode))
					szPlusModes.insert(cMode, QString());
			}
			else
			{
				if(m_pChannel->plainChannelMode().contains(cMode))
					szMinusModes.insert(cMode, QString());
			}
		}
	}

	// now flush out mode changes
	int iModesPerLine = 3; // a good default
	KviIrcConnectionServerInfo * pServerInfo = nullptr;
	if(m_pChannel)
		pServerInfo = m_pChannel->serverInfo();
	if(pServerInfo)
	{
		iModesPerLine = pServerInfo->maxModeChanges();
		if(iModesPerLine < 1)
			iModesPerLine = 1;
	}

	QString szModes;
	QStringList szParameters;
	int iModes = 0;

	QMap<char, QString>::const_iterator iter = szMinusModes.constBegin();
	while(iter != szMinusModes.constEnd())
	{
		if(iter == szMinusModes.constBegin())
			szModes.append("-");
		szModes.append(iter.key());
		szParameters.append(iter.value());
		++iModes;
		++iter;

		//time to commit?
		if(iModes == iModesPerLine)
		{
			QString szCommitModes = szModes;
			if(iter == szMinusModes.constEnd())
				szModes.clear();
			else
				szModes = "-";
			if(szParameters.count())
			{
				szCommitModes.append(QChar(' '));
				szCommitModes.append(szParameters.join(QString(" ")));
				szParameters.clear();
			}
			iModes = 0;

			emit setMode(szCommitModes);
		}
	}

	iter = szPlusModes.constBegin();
	while(iter != szPlusModes.constEnd())
	{
		if(iter == szPlusModes.constBegin())
			szModes.append("+");
		szModes.append(iter.key());
		szParameters.append(iter.value());
		++iModes;
		++iter;

		//time to commit? this should be an ==, but includes the minus sign so "+aaa" = 4 chars
		if(iModes == iModesPerLine)
		{
			QString szCommitModes = szModes;
			if(iter == szPlusModes.constEnd())
				szModes.clear();
			else
				szModes = "+";
			if(szParameters.count())
			{
				szCommitModes.append(QChar(' '));
				szCommitModes.append(szParameters.join(QString(" ")));
				szParameters.clear();
			}
			iModes = 0;

			emit setMode(szCommitModes);
		}
	}

	if(iModes)
	{
		QString szCommitModes = szModes;
		szModes.clear();
		if(szParameters.count())
		{
			szCommitModes.append(QChar(' '));
			szCommitModes.append(szParameters.join(QString(" ")));
			szParameters.clear();
		}
		emit setMode(szCommitModes);
	}

	emit done();
}

const QString * KviModeEditor::getModeDescription(char cMode)
{
	if(!m_pChannel)
		return nullptr;
	KviIrcConnectionServerInfo * pServerInfo = m_pChannel->serverInfo();
	if(pServerInfo)
		return &(pServerInfo->getChannelModeDescription(cMode));
	return nullptr;
}

bool KviModeEditor::modeNeedsParameterOnlyWhenSet(char cMode)
{
	if(!m_pChannel)
		return false;
	KviIrcConnectionServerInfo * pServerInfo = m_pChannel->serverInfo();
	if(pServerInfo)
		return pServerInfo->supportedParameterWhenSetModes().contains(cMode);
	return false;
}
