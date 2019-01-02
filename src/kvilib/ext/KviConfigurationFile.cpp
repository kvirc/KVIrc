//=============================================================================
//
//   File : KviConfigurationFile.cpp
//   Creation date : Thu Jan 14 1999 18:03:59 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviConfigurationFile.h"
#include "KviFileUtils.h"
#include "KviPixmap.h"
#include "KviMessageTypeSettings.h"
#include "KviStringConversion.h"
#include "KviMemory.h"
#include "KviFile.h"

#include <QColor>
#include <QRect>
#include <QSaveFile>

KviConfigurationFile::KviConfigurationFile(const QString & filename, FileMode f, bool bLocal8Bit)
{
	m_bLocal8Bit = bLocal8Bit;
	m_szFileName = filename;
	m_bDirty = false;
	m_szGroup = KVI_CONFIG_DEFAULT_GROUP;
	m_bPreserveEmptyGroups = false;
	m_bReadOnly = (f == KviConfigurationFile::Read);
	m_pDict = new KviPointerHashTable<QString, KviConfigurationFileGroup>(17, false);
	m_pDict->setAutoDelete(true);
	if(f != KviConfigurationFile::Write)
		load();
}

KviConfigurationFile::KviConfigurationFile(const char * filename, FileMode f, bool bLocal8Bit)
{
	m_bLocal8Bit = bLocal8Bit;
	m_szFileName = QString::fromUtf8(filename);
	m_bDirty = false;
	m_szGroup = KVI_CONFIG_DEFAULT_GROUP;
	m_bPreserveEmptyGroups = false;
	m_bReadOnly = (f == KviConfigurationFile::Read);
	m_pDict = new KviPointerHashTable<QString, KviConfigurationFileGroup>(17, false);
	m_pDict->setAutoDelete(true);
	if(f != KviConfigurationFile::Write)
		load();
}

KviConfigurationFile::~KviConfigurationFile()
{
	if(m_bDirty)
		save();
	delete m_pDict;
}

void KviConfigurationFile::clear()
{
	delete m_pDict;
	m_pDict = new KviPointerHashTable<QString, KviConfigurationFileGroup>(17, false);
	m_pDict->setAutoDelete(true);
	m_bDirty = true;
	m_szGroup = KVI_CONFIG_DEFAULT_GROUP;
}

void KviConfigurationFile::clearGroup(const QString & szGroup)
{
	m_bDirty = true;
	m_pDict->remove(szGroup);
	if(!m_pDict->find(m_szGroup))
		m_szGroup = KVI_CONFIG_DEFAULT_GROUP; //removed the current one
}

void KviConfigurationFile::clearKey(const QString & szKey)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	p_group->remove(szKey);
	if(p_group->count() == 0)
		clearGroup(m_szGroup);
}

#define LOAD_BLOCK_SIZE 32768

bool KviConfigurationFile::load()
{
	// this is really faster than the old version :)
	// open the file
	KviFile f(m_szFileName);
	if(!f.open(QFile::ReadOnly))
		return false;

	KviCString tmp;
	KviConfigurationFileGroup * p_group = nullptr;

	int iLoadBlockSize = LOAD_BLOCK_SIZE;

	char * buffer = (char *)KviMemory::allocate(iLoadBlockSize * sizeof(char));

	int toRead;
	int readedLen;
	int remainingLen = 0;

	char * p = buffer; // start writing to the beginning of the buffer

	do
	{
		// compute the length to read
		toRead = iLoadBlockSize - remainingLen;
		if(toRead < 1)
		{
			// ops... a string longer than iLoadBlockSize - 1 chars
			iLoadBlockSize += LOAD_BLOCK_SIZE;
			int iOffset = p - buffer;
			buffer = (char *)KviMemory::reallocate(buffer, iLoadBlockSize * sizeof(char));
			p = buffer + iOffset;
			toRead += LOAD_BLOCK_SIZE;
		}

		// do read
		readedLen = f.read(p, toRead);
		if(readedLen < toRead)
		{
			// check for errors
			if(readedLen <= 0)
			{
				if(readedLen < 0)
				{
					// error at all
					f.close();
					KviMemory::free(buffer);
					return true; // nothing more to parse anyway
				}
				else
				{
					// just a zero byte read
					if(remainingLen == 0)
					{
						// there was nothing in the buffer
						f.close(); // nothing to parse anyway
						KviMemory::free(buffer);
						return true;
					}
					// there is something in the buffer but we have readed 0 bytes
					// this usually means that the last line in the file has no trailing newline
					// ...we just fake it :)
					*p = '\n';
					readedLen = 1;
				}
			}
			else
			{
				// just readed something but less than expected
				// check if the last readed char is a newline
				// if it isn't, fake it
				if(*(p + readedLen - 1) != '\n')
				{
					*(p + readedLen) = '\n';
					readedLen++;
				}
			}
		}
		// compute the end pointer
		char * endp = p + readedLen;

		p = buffer; // start from beginning of the data buffer at all
		// begin of the current string
		char * begin = p;

		// and loop
		while(p < endp)
		{
			// find a newline
			if(*p != '\n')
			{
				p++;
				continue;
			}
			// newline!
			*p = 0;
			// now begin points to the string that terminates in p
			// skip leading whitespace
			while((*begin == '\t') || (*begin == ' '))
				begin++;

			if(p == begin)
			{
				// empty line
				p++;
				begin = p;
				continue;
			}
			// now p > begin
			// check if there are trailing spaces (include CR so CRLF is trimmed too)
			char * trail = p - 1;

			p++;

			while(trail >= begin)
			{
				if((*trail == '\r') || (*trail == '\t') || (*trail == ' '))
					*trail = 0;
				else
					break;
				trail--;
			}

			// yeah, have some data in this line :D
			switch(*begin)
			{
				case 0:
					// empty line
					break;
				case '#':
					// comment: just skip it
					break;
				case '[':
					// group ?
					begin++;
					if(*begin && (*begin != ']'))
					{
						char * z = begin;
#define COMPAT_WITH_OLD_CONFIGS
#ifdef COMPAT_WITH_OLD_CONFIGS
						// run to the end of the string
						while(*z)
							z++;
						// run back to the trailing ']'
						while((z > begin) && (*z != ']'))
							z--;
						// if it is not there just run back to the end of the string
						if(*z != ']')
							while(*z)
								z++;
#else
						// new configs have it always encoded properly
						while(*z && (*z != ']'))
							z++;
#endif
						*z = 0;
						tmp.hexDecode(begin);
						tmp.stripRightWhiteSpace(); // no external spaces in group names

						if(!tmp.isEmpty())
						{
							QString szGroup = m_bLocal8Bit ? QString::fromLocal8Bit(tmp.ptr(), tmp.len()) : QString::fromUtf8(tmp.ptr(), tmp.len());
							p_group = m_pDict->find(szGroup);
							if(!p_group)
							{
								p_group = new KviConfigurationFileGroup(17, false);
								p_group->setAutoDelete(true);
								m_pDict->insert(szGroup, p_group);
							}
						}
					}
					break;
				default:
				{
					// real data ?
					char * z = begin;
					while(*z && (*z != '='))
						z++;
					if(*z && (z != begin))
					{
						*z = 0;
						tmp.hexDecode(begin);
						tmp.stripRightWhiteSpace(); // No external spaces at all in keys
						if(!tmp.isEmpty())
						{
							QString szKey = m_bLocal8Bit ? QString::fromLocal8Bit(tmp.ptr(), tmp.len()) : QString::fromUtf8(tmp.ptr(), tmp.len());
							z++;
							while(*z && ((*z == ' ') || (*z == '\t')))
								z++;
							if(*z)
							{
								tmp.hexDecode(z);
								QString * pVal = new QString(m_bLocal8Bit ? QString::fromLocal8Bit(tmp.ptr(), tmp.len()) : QString::fromUtf8(tmp.ptr(), tmp.len()));
								if(!p_group)
								{
									// ops...we're missing a group
									// use the default one
									p_group = new KviConfigurationFileGroup(17, false);
									p_group->setAutoDelete(true);
									m_pDict->insert(KVI_CONFIG_DEFAULT_GROUP, p_group);
								}
								p_group->replace(szKey, pVal);
							}
							else
							{
								// we in fact need this (mercy :D)
								// otherwise the empty options will be treated as non-existing ones
								// and will get the defaults (which is bad)
								QString * pVal = new QString(QString());
								p_group->replace(szKey, pVal);
							}
						}
					}
				}
				break;
			}
			begin = p;
		}
		if(begin != endp)
		{
			// there is data with no trailing newline in the buffer
			remainingLen = endp - begin;
			if(buffer != begin)
			{
				KviMemory::move(buffer, begin, remainingLen);
				p = buffer + remainingLen;
			} // else p remains where it is
		}
		else
		{
			p = buffer;
		}
	} while(readedLen == toRead);

	f.close();
	KviMemory::free(buffer);
	return true;
}

bool KviConfigurationFile::saveIfDirty()
{
	if(!m_bDirty)
		return true;
	return save();
}

bool KviConfigurationFile::save()
{
	static unsigned char encode_table[256] = {
		// clang-format off
		//	000 001 002 003 004 005 006 007   008 009 010 011 012 013 014 015
		//	NUL SOH STX ETX EOT ENQ ACK BEL   BS  HT  LF  VT  FF  CR  SO  SI
			1  ,1  ,1  ,1  ,1  ,1  ,1  ,1    ,1  ,1  ,1  ,1  ,1  ,1  ,1  ,1  ,
		//	016 017 018 019 020 021 022 023   024 025 026 027 028 029 030 031
		//	DLE DC1 DC2 DC3 DC4 NAK SYN ETB   CAN EM  SUB ESC FS  GS  RS  US
			1  ,1  ,1  ,1  ,1  ,1  ,1  ,1    ,1  ,1  ,1  ,1  ,1  ,1  ,1  ,1  ,
		//	032 033 034 035 036 037 038 039   040 041 042 043 044 045 046 047
		//	    !   "   #   $   %   &   '     (   )   *   +   ,   -   .   /
			1  ,0  ,0  ,1  ,0  ,1  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	048 049 050 051 052 053 054 055   056 057 058 059 060 061 062 063
		//	0   1   2   3   4   5   6   7     8   9   :   ;   <   =   >   ?
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,1  ,0  ,0  ,
		//	064 065 066 067 068 069 070 071   072 073 074 075 076 077 078 079
		//	@   A   B   C   D   E   F   G     H   I   J   K   L   M   N   O
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	080 081 082 083 084 085 086 087   088 089 090 091 092 093 094 095
		//	P   Q   R   S   T   U   V   W     X   Y   Z   [   \   ]   ^   _
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,1  ,0  ,1  ,0  ,0  ,
		//	096 097 098 099 100 101 102 103   104 105 106 107 108 109 110 111
		//	`   a   b   c   d   e   f   g     h   i   j   k   l   m   n   o
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	112 113 114 115 116 117 118 119   120 121 122 123 124 125 126 127
		//	p   q   r   s   t   u   v   w     x   y   z   {   |   }   ~   
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	128 129 130 131 132 133 134 135   136 137 138 139 140 141 142 143
		//
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	144 145 146 147 148 149 150 151   152 153 154 155 156 157 158 159
		//
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	160 161 162 163 164 165 166 167   168 169 170 171 172 173 174 175
		//
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	176 177 178 179 180 181 182 183   184 185 186 187 188 189 190 191
		//
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	192 193 194 195 196 197 198 199   200 201 202 203 204 205 206 207
		//	�  �  �  �  �  �  �  �    �  �  �  �  �  �  �  �
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	208 209 210 211 212 213 214 215   216 217 218 219 220 221 222 223
		//	�  �  �  �  �  �  �  �    �  �  �  �  �  �  �  �
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	224 225 226 227 228 229 230 231   232 233 234 235 236 237 238 239
		//	�  �  �  �  �  �  �  �    �  �  �  �  �  �  �  �
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,
		//	240 241 242 243 244 245 246 247   248 249 250 251 252 253 254 255
		//	�  �  �  �  �  �  �  �
			0  ,0  ,0  ,0  ,0  ,0  ,0  ,0    ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0
		// clang-format on
	};

	if(m_bReadOnly)
		return false;

	QSaveFile f(m_szFileName);

	if(!f.open(QFile::WriteOnly | QFile::Truncate))
		return false;

	if(f.write("# KVIrc configuration file\n", 27) != 27)
		return false;

	KviPointerHashTableIterator<QString, KviConfigurationFileGroup> it(*m_pDict);
	while(it.current())
	{
		if((it.current()->count() != 0) || (m_bPreserveEmptyGroups))
		{
			KviCString group(m_bLocal8Bit ? it.currentKey().toLocal8Bit() : it.currentKey().toUtf8());
			group.hexEncodeWithTable(encode_table);

			if(!f.putChar('['))
				return false;
			if(f.write(group.ptr(), group.len()) < (unsigned int)group.len())
				return false;
			if(f.write("]\n", 2) < 2)
				return false;

			KviConfigurationFileGroup * dict = (KviConfigurationFileGroup *)it.current();
			KviConfigurationFileGroupIterator it2(*dict);

			KviCString szName, szValue;
			while(QString * p_str = it2.current())
			{
				szName = m_bLocal8Bit ? it2.currentKey().toLocal8Bit() : it2.currentKey().toUtf8();
				szValue = m_bLocal8Bit ? (*p_str).toLocal8Bit() : (*p_str).toUtf8();
				szName.hexEncodeWithTable(encode_table);
				szValue.hexEncodeWhiteSpace();

				if(f.write(szName.ptr(), szName.len()) < (unsigned int)szName.len())
					return false;
				if(!f.putChar('='))
					return false;
				if(f.write(szValue.ptr(), szValue.len()) < (unsigned int)szValue.len())
					return false;
				if(!f.putChar('\n'))
					return false;
				++it2;
			}
		}
		++it;
	}

	if(!f.commit())
		return false;

	m_bDirty = false;
	return true;
}

void KviConfigurationFile::setGroup(const QString & szGroup)
{
	m_szGroup = szGroup;
	if(m_bPreserveEmptyGroups)
	{
		if(!hasGroup(szGroup))
		{
			getCurrentGroup(); // we need it to be created.
			m_bDirty = true;
		}
	}
}

bool KviConfigurationFile::hasKey(const QString & szKey)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	return (p_group->find(szKey) != nullptr);
}

bool KviConfigurationFile::hasGroup(const QString & szGroup)
{
	return (m_pDict->find(szGroup) != nullptr);
}

KviConfigurationFileGroup * KviConfigurationFile::getCurrentGroup()
{
	if(m_szGroup.isEmpty())
		m_szGroup = KVI_CONFIG_DEFAULT_GROUP;
	KviConfigurationFileGroup * p_group = m_pDict->find(m_szGroup);
	if(!p_group)
	{
		//create the group
		p_group = new KviConfigurationFileGroup(17, false);
		p_group->setAutoDelete(true);
		m_pDict->insert(m_szGroup, p_group);
	}
	return p_group;
}

//
// QString
//

void KviConfigurationFile::writeEntry(const QString & szKey, const QString & szValue)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_data = new QString(szValue);
	p_group->replace(szKey, p_data);
}

QString KviConfigurationFile::readEntry(const QString & szKey, const QString & szDefault)
{
	KviConfigurationFileGroup * pGroup = getCurrentGroup();
	QString * pStr = pGroup->find(szKey);
	if(!pStr)
	{
		m_szStrBuffer = szDefault;
	}
	else
	{
		m_szStrBuffer = *pStr;
	}
	return m_szStrBuffer;
}

//
// QStringList
//

static QString g_szConfigStringListSeparator(",\\[ITEM],");

QStringList KviConfigurationFile::readStringListEntry(const QString & szKey, const QStringList & list)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_str = p_group->find(szKey);
	if(!p_str)
		return list;

	return p_str->split(g_szConfigStringListSeparator);
}

void KviConfigurationFile::writeEntry(const QString & szKey, const QStringList & list)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_data = new QString(list.join(g_szConfigStringListSeparator));
	p_group->replace(szKey, p_data);
}

//
// QList<int>
//

QList<int> KviConfigurationFile::readIntListEntry(const QString & szKey, const QList<int> & list)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_str = p_group->find(szKey);
	if(!p_str)
	{
		//qDebug("Returning default list for group %s and key %s",m_szGroup.latin1(),szKey.latin1());
		return list;
	}

	QStringList sl = p_str->split(",");
	QList<int> ret;

	//qDebug("Got option list for group %s and key %s: %s",m_szGroup.latin1(),szKey.latin1(),p_str->latin1());

	for(auto & it : sl)
	{
		bool bOk;
		int iTmp = it.toInt(&bOk);
		if(bOk)
			ret.append(iTmp);
	}

	return ret;
}

void KviConfigurationFile::writeEntry(const QString & szKey, const QList<int> & list)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	KviCString szData;
	for(int it : list)
	{
		if(szData.hasData())
			szData.append(',');
		szData.append(KviCString::Format, "%d", it);
	}
	//qDebug("Writing option list for group %s and key %s: %s",m_szGroup.latin1(),szKey.latin1(),szData.ptr());

	p_group->replace(szKey, new QString(szData.ptr()));
}

//
// KviPixmap
//

// FIXME: #warning "Spaces in image names ?"
void KviConfigurationFile::writeEntry(const QString & szKey, const KviPixmap & pixmap)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_data = new QString();
	KviStringConversion::toString(pixmap, *p_data);
	p_group->replace(szKey, p_data);
}

KviPixmap KviConfigurationFile::readPixmapEntry(const QString & szKey, const KviPixmap & pixDef)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_str = p_group->find(szKey);
	if(p_str)
	{
		KviPixmap ret("");
		return KviStringConversion::fromString(*p_str, ret) ? ret : pixDef;
	}
	else
	{
		return pixDef;
	}
}

//
// KviMessageTypeSettings
//

void KviConfigurationFile::writeEntry(const QString & szKey, const KviMessageTypeSettings & msg)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString szData;
	KviStringConversion::toString(msg, szData);
	p_group->replace(szKey, new QString(szData));
}

KviMessageTypeSettings KviConfigurationFile::readMsgTypeEntry(const QString & szKey, const KviMessageTypeSettings & msgDef)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_str = p_group->find(szKey);
	if(!p_str)
		return msgDef;
	KviMessageTypeSettings ret = msgDef;
	KviStringConversion::fromString(*p_str, ret);
	return ret;
}

//
// QColor
//

void KviConfigurationFile::writeEntry(const QString & szKey, const QColor & clr)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	KviCString szData(KviCString::Format, "%d,%d,%d,%d", clr.red(), clr.green(), clr.blue(), clr.alpha());
	p_group->replace(szKey, new QString(szData.ptr()));
}

QColor KviConfigurationFile::readColorEntry(const QString & szKey, const QColor & clr)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QColor color(clr);
	QString * pointer_that_IS_initialized = p_group->find(szKey);

	if(pointer_that_IS_initialized)
	{

		KviCString str(*pointer_that_IS_initialized);
		str.stripLeftWhiteSpace();

		KviCString red, green, blue, alpha;

		str.getToken(red, ',');
		str.getToken(green, ',');
		str.getToken(blue, ',');
		str.getToken(alpha, ',');
		if(alpha.isEmpty())
			alpha = "255";

		if((red.isUnsignedNum()) && (green.isUnsignedNum()) && (blue.isUnsignedNum()) && (alpha.isUnsignedNum()))
		{
			bool bOk;
			int r = red.toInt(&bOk) % 256;
			int g = green.toInt(&bOk) % 256;
			int b = blue.toInt(&bOk) % 256;
			int a = alpha.toInt(&bOk) % 256;
			if(r < 0)
				r = -r;
			if(g < 0)
				g = -g;
			if(b < 0)
				b = -b;
			if(a < 0)
				a = -a;

			color.setRgb(r, g, b);
			color.setAlpha(a);
		}
	}
	return color;
}

//
// QFont
//

void KviConfigurationFile::getFontProperties(KviCString & buffer, QFont * fnt)
{
	QString tmp;
	KviStringConversion::toString(*fnt, tmp);
	buffer = tmp;
}

void KviConfigurationFile::writeEntry(const QString & szKey, QFont & fnt)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_data = new QString();
	KviStringConversion::toString(fnt, *p_data);
	p_group->replace(szKey, p_data);
}

void KviConfigurationFile::setFontProperties(KviCString & str, QFont * fnt)
{
	KviStringConversion::fromString(str.ptr(), *fnt);
}

QFont KviConfigurationFile::readFontEntry(const QString & szKey, const QFont & fnt)
{
	QFont font(fnt);
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_str = p_group->find(szKey);
	if(p_str)
	{
		//FontEntry=Arial,12,9,0,100,italic,underline,strikeout,
		KviCString str(*p_str);
		str.stripLeftWhiteSpace();
		setFontProperties(str, &font);
	}
	return font;
}

//
// bool
//

void KviConfigurationFile::writeEntry(const QString & szKey, bool bTrue)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_data = new QString(bTrue ? "true" : "false");
	p_group->replace(szKey, p_data);
}

bool KviConfigurationFile::readBoolEntry(const QString & szKey, bool bTrue)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_str = p_group->find(szKey);
	if(!p_str)
		return bTrue;
	return p_str->toLower() == "true";
}

//
// QRect
//

void KviConfigurationFile::writeEntry(const QString & szKey, const QRect & rct)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString szBuf;
	KviStringConversion::toString(rct, szBuf);
	p_group->replace(szKey, new QString(szBuf));
}

QRect KviConfigurationFile::readRectEntry(const QString & szKey, const QRect & rct)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * str = p_group->find(szKey);
	if(!str)
		return rct;
	QRect ret;
	return KviStringConversion::fromString(*str, ret) ? ret : rct;
}

//
// unsigned short
//

void KviConfigurationFile::writeEntry(const QString & szKey, unsigned short int usValue)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_data = new QString();
	p_data->setNum(usValue);
	p_group->replace(szKey, p_data);
}

unsigned short int KviConfigurationFile::readUShortEntry(const QString & szKey, unsigned short int usDefault)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_str = p_group->find(szKey);
	if(!p_str)
		return usDefault;
	bool bOk;
	unsigned short int usVal = p_str->toUShort(&bOk);
	return bOk ? usVal : usDefault;
}

//
// int
//

void KviConfigurationFile::writeEntry(const QString & szKey, int iValue)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_data = new QString();
	p_data->setNum(iValue);
	p_group->replace(szKey, p_data);
}

int KviConfigurationFile::readIntEntry(const QString & szKey, int iDefault)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_str = p_group->find(szKey);
	if(!p_str)
		return iDefault;
	bool bOk;
	int iVal = p_str->toInt(&bOk);
	return bOk ? iVal : iDefault;
}

//
// unsigned int
//

void KviConfigurationFile::writeEntry(const QString & szKey, unsigned int iValue)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_data = new QString();
	p_data->setNum(iValue);
	p_group->replace(szKey, p_data);
}

unsigned int KviConfigurationFile::readUIntEntry(const QString & szKey, unsigned int iDefault)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_str = p_group->find(szKey);
	if(!p_str)
		return iDefault;
	bool bOk;
	unsigned int iVal = p_str->toUInt(&bOk);
	return bOk ? iVal : iDefault;
}

//
// char
//

void KviConfigurationFile::writeEntry(const QString & szKey, char iValue)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_data = new QString();
	p_data->setNum(iValue);
	p_group->replace(szKey, p_data);
}

char KviConfigurationFile::readCharEntry(const QString & szKey, char iDefault)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_str = p_group->find(szKey);
	if(!p_str)
		return iDefault;
	bool bOk;
	char iVal = (char)p_str->toInt(&bOk);
	return bOk ? iVal : iDefault;
}

//
// unsigned char
//

void KviConfigurationFile::writeEntry(const QString & szKey, unsigned char iValue)
{
	m_bDirty = true;
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_data = new QString();
	p_data->setNum(iValue);
	p_group->replace(szKey, p_data);
}

unsigned char KviConfigurationFile::readUCharEntry(const QString & szKey, unsigned char iDefault)
{
	KviConfigurationFileGroup * p_group = getCurrentGroup();
	QString * p_str = p_group->find(szKey);
	if(!p_str)
		return iDefault;
	bool bOk;
	unsigned char iVal = (unsigned char)p_str->toUInt(&bOk);
	return bOk ? iVal : iDefault;
}
