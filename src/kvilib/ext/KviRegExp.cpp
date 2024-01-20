//=============================================================================
//
//   File : KviRegExp.cpp
//   Creation date : Tue Oct 3 2023 17:25:21 CEST by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2023 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviRegExp.h"

KviRegExp::KviRegExp(const QString & szPattern, const KviRegExp::CaseSensitivity cs, const KviRegExp::PatternSyntax ps)
: m_szPattern(szPattern), m_eCs(cs), m_ePs(ps), m_bMinimal(0)
{
}

QString KviRegExp::getCompletePattern() const
{
	if(m_ePs == PatternSyntax::Wildcard)
	{
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 1)
		return QRegularExpression::wildcardToRegularExpression(m_szPattern, QRegularExpression::UnanchoredWildcardConversion | QRegularExpression::NonPathWildcardConversion);
#elif QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		QString tmp = QRegularExpression::wildcardToRegularExpression(m_szPattern, QRegularExpression::UnanchoredWildcardConversion);
		// fix #2589 - permit slash chars in matched string
		tmp.replace("[^/]*", "[\\d\\D]*");
		return tmp;
#else
		QString tmp = QRegularExpression::wildcardToRegularExpression(m_szPattern);
		// Qt5 always add anchors to the converted regexp, strip them
		tmp.remove(0, 5);	// "\\A(?:"
		tmp.chop(3);		// ")\\z"
		// fix #2589 - permit slash chars in matched string
		tmp.replace("[^/]*", "[\\d\\D]*");
		return tmp;
#endif
	}
	return m_szPattern;
}

QRegularExpression::PatternOptions KviRegExp::getPatternOptions() const
{
	QRegularExpression::PatternOptions ret = QRegularExpression::NoPatternOption;
	if(m_eCs == CaseSensitivity::CaseInsensitive)
		ret |= QRegularExpression::CaseInsensitiveOption;

	if(m_bMinimal)
		ret |= QRegularExpression::InvertedGreedinessOption;

	return ret;
}

bool KviRegExp::exactMatch(const QString & szStr)
{
	QRegularExpression re(QRegularExpression::anchoredPattern(getCompletePattern()), getPatternOptions());
	m_oLastMatch = re.match(szStr);
	return m_oLastMatch.hasMatch();
}

int KviRegExp::indexIn(const QString & szStr, int offset)
{
	QRegularExpression re(getCompletePattern(), getPatternOptions());
	m_oLastMatch = re.match(szStr, offset);
	return m_oLastMatch.capturedStart(0);
}

int KviRegExp::matchedLength() const
{
	return m_oLastMatch.capturedLength(0);
}

QString KviRegExp::cap(int iNth) const
{
	return m_oLastMatch.captured(iNth);
}

QStringList KviRegExp::capturedTexts() const
{
	return m_oLastMatch.capturedTexts();
}
