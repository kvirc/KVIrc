//============================================================================
//
//   File : KviModeWidget.cpp
//   Creation date : Sat Nov 12 2005 23:50:12 by Alexey Uzhva
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alexey Uzhva (wizard at opendoor dot ru)
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
//============================================================================

#include "KviModeWidget.h"
#include "KviChannelWindow.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviOptions.h"
#include "KviTalHBox.h"

#include <QKeyEvent>

KviModeWidget::KviModeWidget(QWidget * par, KviChannelWindow & chan, const char * name)
    : KviThemedLineEdit(par, &chan, name), m_Channel(chan)
{
	reset();
}

KviModeWidget::~KviModeWidget()
    = default;

void KviModeWidget::reset()
{
	setReadOnly(true);
	refreshModes();

	if(m_Channel.input())
		m_Channel.setFocus();
}

void KviModeWidget::refreshModes()
{
	setText(m_Channel.getChannelModeStringWithEmbeddedParams());
}

void KviModeWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	if(m_Channel.isMeHalfOp(true) || m_Channel.connection()->userInfo()->hasUserMode('o') || m_Channel.connection()->userInfo()->hasUserMode('O'))
	{
		setReadOnly(false);
	}
}

void KviModeWidget::keyReleaseEvent(QKeyEvent * e)
{
	switch(e->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
			processModeChanges();
			break;
		case Qt::Key_Escape:
			reset();
			break;
		default:
			KviThemedLineEdit::keyReleaseEvent(e);
			break;
	}
}

void KviModeWidget::processModeChanges()
{
	auto szOldModes = m_Channel.getChannelModeStringWithEmbeddedParams();
	auto newModesDict = parseChannelModeString(text());
	auto oldModesDict = parseChannelModeString(szOldModes);

	std::vector<std::pair<QChar, QString>> removeModes;
	std::vector<std::pair<QChar, QString>> addModes;

	for(const auto & modeIter : oldModesDict)
	{
		const QChar & cMode = modeIter.first;
		const QString & szParam = modeIter.second;
		if(!newModesDict.count(cMode))
		{
			if(!isParameterOnlyNeededWhenModeIsSet(cMode))
				removeModes.emplace_back(cMode, szParam);
			else
				removeModes.emplace_back(cMode, QString{});
		}
	}

	for(const auto & modeIter : newModesDict)
	{
		const QChar & cMode = modeIter.first;
		const QString & szParam = modeIter.second;
		if(!oldModesDict.count(cMode))
			addModes.emplace_back(cMode, szParam);
		else if(oldModesDict[cMode] != newModesDict[cMode])
		{
			if(!isParameterOnlyNeededWhenModeIsSet(cMode))
				removeModes.emplace_back(cMode, oldModesDict[cMode]);
			addModes.emplace_back(cMode, szParam);
		}
	}

	int iModesPerLine = 3;
	KviIrcConnectionServerInfo * pServerInfo = m_Channel.serverInfo();
	if(pServerInfo)
		iModesPerLine = std::max(1, pServerInfo->maxModeChanges());

	int iModes = 0;
	QString szModeChanges;
	QStringList params;
	for(const auto & modeChange : removeModes)
	{
		if(szModeChanges.isEmpty())
			szModeChanges += QChar('-');
		szModeChanges += modeChange.first;
		if(!modeChange.second.isEmpty())
			params << modeChange.second;
		++iModes;
		if(iModes == iModesPerLine)
		{
			sendModeChanges(std::move(szModeChanges), std::move(params));
			iModes = 0;
		}
	}

	if(!addModes.empty())
		szModeChanges += QChar('+');

	for(const auto & modeChange : addModes)
	{
		if(szModeChanges.isEmpty())
			szModeChanges += QChar('+');
		szModeChanges += modeChange.first;
		if(!modeChange.second.isEmpty())
			params << modeChange.second;
		++iModes;
		if(iModes == iModesPerLine)
		{
			sendModeChanges(std::move(szModeChanges), std::move(params));
			iModes = 0;
		}
	}

	if(iModes != 0)
		sendModeChanges(std::move(szModeChanges), std::move(params));

	reset();
}

bool KviModeWidget::isParameterOnlyNeededWhenModeIsSet(const QChar & cMode)
{
	KviIrcConnectionServerInfo * pServerInfo = m_Channel.serverInfo();
	if(pServerInfo)
		return pServerInfo->supportedParameterWhenSetModes().contains(cMode);
	return false;
}

std::map<QChar, QString> KviModeWidget::parseChannelModeString(const QString& szModes)
{
	std::map<QChar, QString> modeDict;

	for(const auto & szSubstring : szModes.split(QChar(' '), QString::SkipEmptyParts))
	{
		if(szSubstring.size() >= 3 && szSubstring.at(1) == QChar(':'))
		{
			QChar cMode = szSubstring.at(0);
			modeDict[cMode] = szSubstring.mid(2);
		}
                else
		{
			for(const auto & cMode : szSubstring)
				modeDict[cMode] = QString{};
		}
	}

	return modeDict;
}

void KviModeWidget::sendModeChanges(const QString szModeString, const QStringList params)
{
	if(!params.isEmpty())
		emit setMode(szModeString + QChar(' ') + params.join(QChar(' ')));
	else
		emit setMode(szModeString);
}
