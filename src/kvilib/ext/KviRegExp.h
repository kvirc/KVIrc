#ifndef _KVI_REGEXP_H_
#define _KVI_REGEXP_H_
//=============================================================================
//
//   File : KviRegExp.h
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

#include "kvi_settings.h"

#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

class KVILIB_API KviRegExp
{
public:
	enum CaseSensitivity
	{
		CaseInsensitive = 0,
		CaseSensitive = 1,
	};

	enum PatternSyntax
	{
		RegExp = 0,
		Wildcard = 1,
	};

	KviRegExp(const QString & szPattern = QString(), const KviRegExp::CaseSensitivity cs = CaseSensitive, const KviRegExp::PatternSyntax ps = RegExp);

	operator QRegularExpression() const {
		QRegularExpression re(getCompletePattern(), getPatternOptions());
		return re;
	}

private:
	QString m_szPattern;
	QRegularExpressionMatch m_oLastMatch;
	KviRegExp::CaseSensitivity m_eCs;
	KviRegExp::PatternSyntax m_ePs;
	bool m_bMinimal;

	QString getCompletePattern() const;
	QRegularExpression::PatternOptions getPatternOptions() const;

public:
	const QString & pattern() const { return m_szPattern; };
	void setPattern(const QString & szPattern) { m_szPattern = szPattern; };
	void setCaseSensitivity(KviRegExp::CaseSensitivity cs) { m_eCs = cs; };
	void setMinimal(bool bMinimal) { m_bMinimal = bMinimal; };
	bool exactMatch(const QString & szStr);
	int indexIn(const QString & szStr, int offset = 0);
	int matchedLength() const;
	QString cap(int iNth = 0) const;
	QStringList capturedTexts() const;
	static QString escape(const QString &szStr) { return QRegularExpression::escape(szStr); };
};

#endif //!_KVI_REGEXP_H_
