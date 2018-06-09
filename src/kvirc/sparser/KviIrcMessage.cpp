//=============================================================================
//
//   File : KviIrcMessage.cpp
//   Creation date : Fri Aug  2 23:08:57 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviIrcMessage.h"
#include "KviIrcConnection.h"
#include "KviKvsHash.h"

KviIrcMessage::KviIrcMessage(const char * message, KviIrcConnection * pConnection)
{
	m_pConnection = pConnection;
	m_pConsole = pConnection->console();
	m_iFlags = 0;

	const char * aux;
	m_ptr = message;

	while(*m_ptr == ' ')
		++m_ptr;
	const char * allParams = m_ptr; // just to be sure
	if(*m_ptr)
	{
		if(*m_ptr == '@')
		{
			aux = ++m_ptr;
			while(*m_ptr && (*m_ptr != ' '))
				++m_ptr;
			m_szMessageTags.extractFromString(aux, m_ptr);
			parseMessageTags();
			while(*m_ptr == ' ')
				++m_ptr;
		}

		if(*m_ptr == ':')
		{
			aux = ++m_ptr;
			while(*m_ptr && (*m_ptr != ' '))
				++m_ptr;
			m_szPrefix.extractFromString(aux, m_ptr);
			while(*m_ptr == ' ')
				++m_ptr;
		}
		aux = m_ptr;
		while(*m_ptr && (*m_ptr != ' '))
			++m_ptr;
		m_szCommand.extractFromString(aux, m_ptr);
		while(*m_ptr == ' ')
			++m_ptr;
		allParams = m_ptr;
		while(*m_ptr)
		{
			if(*m_ptr == ':')
			{
				++m_ptr;
				m_pParams.emplace_back(m_ptr);
				break; // this was the last
			}
			else
			{
				aux = m_ptr;
				while(*m_ptr && (*m_ptr != ' '))
					++m_ptr;
				m_pParams.emplace_back(aux, m_ptr);
				while(*m_ptr == ' ')
					++m_ptr;
			}
		}
	}
	m_ptr = allParams;

	m_iNumericCommand = (*m_szCommand.ptr() - '0') * 100;

	if((m_szCommand.len() == 3) && (m_iNumericCommand <= 900) && (m_iNumericCommand >= 0))
	{
		aux = m_szCommand.ptr();
		aux++;
		if((*aux >= '0') && (*aux <= '9'))
		{
			m_iNumericCommand += (*aux - '0') * 10;
			aux++;
			if((*aux >= '0') && (*aux <= '9'))
			{
				m_iNumericCommand += (*aux - '0');
			}
			else
			{
				m_iNumericCommand = -1;
				m_szCommand.toUpper();
			}
		}
		else
		{
			m_iNumericCommand = -1;
			m_szCommand.toUpper();
		}
	}
	else
	{
		m_iNumericCommand = -1;
		m_szCommand.toUpper();
	}
}

KviIrcMessage::~KviIrcMessage()
    = default;

void KviIrcMessage::decodeAndSplitMask(char * b, QString & szNick, QString & szUser, QString & szHost)
{
	static QString szWild("*");
	char * p = b;

	while(*p && *p != '!')
		p++;
	if(*p)
	{
		if(p != b)
		{
			*p = 0;
			szNick = m_pConnection->decodeText(b);
			*p = '!';
		}
		else
		{
			szNick = szWild; // ???
		}
	}
	else
	{
		if(p != b)
			szNick = m_pConnection->decodeText(b);
		else
			szNick = szWild; // ???
		szUser = szWild;
		szHost = szWild;
		return;
	}
	p++;
	b = p;
	while(*p && *p != '@')
		p++;
	if(*p)
	{
		if(p != b)
		{
			*p = 0;
			szUser = m_pConnection->decodeText(b);
			*p = '@';
		}
		else
		{
			szUser = szWild; // ???
		}
	}
	else
	{
		if(p != b)
			szUser = m_pConnection->decodeText(b);
		else
			szUser = szWild; // ???
		szHost = szWild;
		return;
	}
	p++;
	// IDN hostnames can be encoded (see ticket #1215)
	szHost = m_pConnection->decodeText(p);
}

void KviIrcMessage::decodeAndSplitPrefix(QString & szNick, QString & szUser, QString & szHost)
{
	char * b;
	if(m_szPrefix.hasData())
		b = m_szPrefix.ptr();
	else
	{
		m_szPrefix = connection()->currentServerName();
		b = m_szPrefix.ptr();
	}
	decodeAndSplitMask(b, szNick, szUser, szHost);
}

const char * KviIrcMessage::safePrefix()
{
	if(m_szPrefix.hasData())
		return m_szPrefix.ptr();
	m_szPrefix = connection()->currentServerName();
	return m_szPrefix.ptr();
}

void KviIrcMessage::parseMessageTags()
{
	if(m_szMessageTags.isEmpty())
		return;
	KviCString szKey;
	KviCString szValue;
	for(int i = 0; i < m_szMessageTags.len(); ++i)
	{
		if(m_szMessageTags.at(i) == '=')
		{
			for(++i; i < m_szMessageTags.len(); ++i)
			{
				if(m_szMessageTags.at(i) == ';')
				{
					m_ParsedMessageTags[connection()->decodeText(szKey)] = connection()->decodeText(szValue);
					szKey.clear();
					szValue.clear();
					break;
				}
				else if(m_szMessageTags.at(i) == '\\')
				{
					if(++i >= m_szMessageTags.len())
						break;
					switch(m_szMessageTags.at(i))
					{
						case ':':
							szValue += ';';
							break;
						case 's':
							szValue += ' ';
							break;
						case '0':
							szValue += '\0';
							break;
						case 'r':
							szValue += '\r';
							break;
						case 'n':
							szValue += '\n';
							break;
						default:
							szValue += m_szMessageTags.at(i);
					}
				}
				else
				{
					szValue += m_szMessageTags.at(i);
				}
			}
		}
		else if(m_szMessageTags.at(i) == ';')
		{
			// Insert key without value
			m_ParsedMessageTags[connection()->decodeText(szKey)].clear();
			szKey.clear();
		}
		else
		{
			szKey += m_szMessageTags.at(i);
		}
	}
	m_ParsedMessageTags[connection()->decodeText(szKey)] = connection()->decodeText(szValue);

	m_time = QDateTime::fromString(m_ParsedMessageTags.value("time"), Qt::ISODate); // empty value will be invalid time
}

QString * KviIrcMessage::messageTagPtr(const QString & szTag)
{
	QHash<QString, QString>::iterator i = m_ParsedMessageTags.find(szTag);
	if(i == m_ParsedMessageTags.end())
		return nullptr;
	return &*i;
}

KviKvsHash * KviIrcMessage::messageTagsKvsHash()
{
	KviKvsHash * hList = new KviKvsHash;
	const auto & m = this->messageTagsMap();
	for(auto it = m.begin(); it != m.end(); ++it)
		hList->set(it.key(), new KviKvsVariant(it.value()));
	return hList;
}
