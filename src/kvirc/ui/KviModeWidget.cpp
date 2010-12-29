//============================================================================
//
//   File : KviModeWidget.cpp
//   Creation date : Sat Nov 12 2005 23:50:12 by Alexey Uzhva
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2005-2008 Alexey Uzhva (wizard at opendoor dot ru)
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
//============================================================================

#include "KviChannelWindow.h"
#include "KviOptions.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviTalHBox.h"

#include <QEvent>
#include <QResizeEvent>
#include <QByteArray>

KviModeWidget::KviModeWidget(QWidget * par,KviChannelWindow* chan,const char * name)
:KviThemedLineEdit(par, chan, name)
{
	m_pChannel=chan;
	reset();
}

KviModeWidget::~KviModeWidget()
{
}

void KviModeWidget::reset()
{
	setReadOnly(true);
	refreshModes();

	if(m_pChannel->input()) m_pChannel->setFocus();
}

void KviModeWidget::refreshModes()
{
	QString szMode;
	m_pChannel->getChannelModeStringWithEmbeddedParams(szMode);
	setText(szMode);
}

void KviModeWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	if(m_pChannel->isMeHalfOp() ||
		m_pChannel->isMeOp() ||
		m_pChannel->isMeChanOwner() ||
		m_pChannel->isMeChanAdmin() ||
		m_pChannel->connection()->userInfo()->hasUserMode('o') ||
		m_pChannel->connection()->userInfo()->hasUserMode('O'))
	{
		setReadOnly(false);
	}
}

void KviModeWidget::keyReleaseEvent (QKeyEvent * e)
{
	switch(e->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
			editorReturnPressed();
			break;
		case Qt::Key_Escape:
			reset();
			break;
		default:
			KviThemedLineEdit::keyReleaseEvent(e);
			break;
	}
}

void KviModeWidget::editorReturnPressed()
{
	QMap<char, QString> szPlusModes;
	QMap<char, QString> szMinusModes;

	QString szTmpMode;
	m_pChannel->getChannelModeStringWithEmbeddedParams(szTmpMode);
	QStringList szOldModes=szTmpMode.split(QChar(' '), QString::SkipEmptyParts);
	QStringList szNewModes=text().split(QChar(' '), QString::SkipEmptyParts);

	//add new modes and modified ones
	for(int i=0; i<szNewModes.count(); ++i)
	{
		QString szSubstring=szNewModes.at(i);
		if(i)
		{
			// not first part: mode with parameter
			if(szSubstring.size() < 3)
				continue;
			if(szSubstring.at(1) != QChar(':'))
				continue;
			char cMode = szSubstring.at(0).unicode();
			szSubstring.remove(0,2);

			if(!m_pChannel->hasChannelMode(cMode) || 
				(szSubstring != m_pChannel->channelModeParam(cMode)))
			{
				// mode was not set before, or the parameter has changed
				szPlusModes.insert(cMode, szSubstring);
			}
		} else {
			// first part: parameterless modes
			QString szCurModes = szOldModes.count() ? szOldModes.at(0) : "";
			for(int j=0; j<szSubstring.length(); ++j)
			{
				char cMode = szSubstring.at(j).unicode();
				if(!szCurModes.contains(cMode))
				{
					// was not set, has to be inserted
					szPlusModes.insert(cMode,QString());
				}
			}
		}
	}
	
	// check for any mode that has been unset
	for(int i=0; i<szOldModes.count(); ++i)
	{
		QString szSubstring=szOldModes.at(i);
		if(i)
		{
			// not first part: mode with parameter
			if(szSubstring.size() < 3)
				continue;
			if(szSubstring.at(1) != QChar(':'))
				continue;
			char cMode = szSubstring.at(0).unicode();
			szSubstring.remove(0,2);

			// we skip parameterless modes (j=0)
			bool bStillSet=false;
			for(int j=1; j<szNewModes.length(); ++j)
			{
				if(szNewModes.at(j).at(0) == cMode)
					bStillSet=true;
			}
			if(!bStillSet)
			{
				// checks if this specific mode does not need a parameter when set
				if(modeNeedsParameterOnlyWhenSet(cMode))
				{
					szMinusModes.insert(cMode,QString());
				} else {
					szMinusModes.insert(cMode, szSubstring);
				}
			}
		} else {
			// first part: parameterless modes
			QString szNewParameterLessModes = szNewModes.count() ? szNewModes.at(0) : "";
			for(int j=0; j<szSubstring.length(); ++j)
			{
				char cMode = szSubstring.at(j).unicode();
				if(!szNewParameterLessModes.contains(cMode))
				{
					// was set, has to be unset
					szMinusModes.insert(cMode,QString());
				}
			}
		}
	}

	// now flush out mode changes
	int iModesPerLine=3; // a good default
	KviIrcConnectionServerInfo * pServerInfo = 0;
	if(m_pChannel) pServerInfo = m_pChannel->serverInfo();
	if(pServerInfo)
	{
		iModesPerLine = pServerInfo->maxModeChanges();
		if(iModesPerLine < 1) iModesPerLine = 1;
	}

	QString szModes;
	QStringList szParameters;
	int iModes=0;

	QMap<char, QString>::const_iterator iter = szMinusModes.constBegin();
	while (iter != szMinusModes.constEnd())
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
				szModes="-";
			if(szParameters.count())
			{
				szCommitModes.append(QChar(' '));
				szCommitModes.append(szParameters.join(QString(" ")));
				szParameters.clear();
			}
			iModes=0;
			
			emit setMode(szCommitModes);
		}
	}

	iter = szPlusModes.constBegin();
	while (iter != szPlusModes.constEnd())
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
				szModes="+";
			if(szParameters.count())
			{
				szCommitModes.append(QChar(' '));
				szCommitModes.append(szParameters.join(QString(" ")));
				szParameters.clear();
			}
			iModes=0;
			
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

	reset();
}

inline bool KviModeWidget::modeNeedsParameterOnlyWhenSet(char cMode)
{
	KviIrcConnectionServerInfo * pServerInfo = 0;
	if(m_pChannel) pServerInfo = m_pChannel->serverInfo();
	if(pServerInfo)
		return pServerInfo->supportedParameterWhenSetModes().contains(cMode);
	return false;
}
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviModeWidget.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
