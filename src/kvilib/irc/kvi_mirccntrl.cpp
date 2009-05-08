//=============================================================================
//
//   File : kvi_mirccntrl.cpp
//   Creation date : Sat Aug 31 17:07:36 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_MIRCCNTRL_CPP_

#include "kvi_mirccntrl.h"

KVILIB_API const char * getColorBytes(const char *data_ptr,unsigned char *byte_1,unsigned char *byte_2)
{
	//
	// Scans the data_ptr for a mIrc color code XX,XX
	// and fills the color values in the two bytes
	//

	//First we can have a digit or a coma
	if(((*data_ptr >= '0') && (*data_ptr <='9')))
	{
		//Something interesting ok.
		(*byte_1)=(*data_ptr)-'0'; //store the code
		data_ptr++;     //and check the next
		if(((*data_ptr >= '0') && (*data_ptr <= '9'))||(*data_ptr==','))
		{
			//Yes we can understand it
			if(*data_ptr==',')
			{
				//A coma , need to check for background
				data_ptr++;
			} else {
				//A number
				(*byte_1)=((((*byte_1)*10)+((*data_ptr)-'0'))%16);
				data_ptr++;
				if(*data_ptr==',')
				{
					//A coma , need to check for background
					data_ptr++;
				} else {
					//Senseless return
					(*byte_2)=KVI_NOCHANGE;
					return data_ptr;
				}
			}
		} else {
			//Senseless character control code OK and return
			(*byte_2)=KVI_NOCHANGE;
			return data_ptr;
		}
	} else {
		//Senseless character : only a CTRL+K code
		(*byte_1)=KVI_NOCHANGE;
		(*byte_2)=KVI_NOCHANGE;
		return data_ptr;
	}

	if((*data_ptr >= '0') && (*data_ptr <='9'))
	{
		//Background , a color code
		(*byte_2)=(*data_ptr)-'0';
		data_ptr++;
		if((*data_ptr >= '0') && (*data_ptr <='9'))
		{
			(*byte_2)=((((*byte_2)*10)+((*data_ptr)-'0'))%16);
			data_ptr++;
		}
		return data_ptr;
	} else {
		(*byte_2)=KVI_NOCHANGE;
		return data_ptr-1;
	}
}


KVILIB_API const kvi_wchar_t * getColorBytesW(const kvi_wchar_t *data_ptr,unsigned char *byte_1,unsigned char *byte_2)
{
	//
	// Scans the data_ptr for a mIrc color code XX,XX
	// and fills the color values in the two bytes
	//

	//First we can have a digit or a coma
	if(((*data_ptr >= '0') && (*data_ptr <='9')))
	{
		//Something interesting ok.
		(*byte_1)=((*data_ptr)-'0'); //store the code
		data_ptr++;     //and check the next
		if(((*data_ptr >= '0') && (*data_ptr <= '9'))||(*data_ptr==','))
		{
			//Yes we can understand it
			if(*data_ptr==',')
			{
				//A coma , need to check for background
				data_ptr++;
			} else {
				//A number
				//(*byte_1)=((((*byte_1)*10)+((*data_ptr)-'0'))%16);
				(*byte_1)=((*byte_1)*10)+((*data_ptr)-'0');
				data_ptr++;
				if(*data_ptr==',')
				{
					//A coma , need to check for background
					data_ptr++;
				} else {
					//Senseless return
					(*byte_2)=KVI_NOCHANGE;
					return data_ptr;
				}
			}
		} else {
			//Senseless character control code OK and return
			(*byte_2)=KVI_NOCHANGE;
			return data_ptr;
		}
	} else {
		//Senseless character : only a CTRL+K code
		(*byte_1)=KVI_NOCHANGE;
		(*byte_2)=KVI_NOCHANGE;
		return data_ptr;
	}

	if((*data_ptr >= '0') && (*data_ptr <='9'))
	{
		//Background , a color code
		(*byte_2)=(*data_ptr)-'0';
		data_ptr++;
		if((*data_ptr >= '0') && (*data_ptr <='9'))
		{
			//(*byte_2)=((((*byte_2)*10)+((*data_ptr)-'0'))%16);
			(*byte_2)=((*byte_2)*10)+((*data_ptr)-'0');
			data_ptr++;
		}
		return data_ptr;
	} else {
		(*byte_2)=KVI_NOCHANGE;
		return data_ptr-1;
	}
}

KVILIB_API unsigned int getUnicodeColorBytes(const QString &szData,unsigned int charIdx,unsigned char *byte_1,unsigned char *byte_2)
{
	//
	// Scans the szData for a mIrc color code XX,XX
	// and fills the color values in the two bytes
	//

	if(charIdx >= (unsigned int) szData.length())
	{
		(*byte_1)=KVI_NOCHANGE;
		(*byte_2)=KVI_NOCHANGE;
		return charIdx;
	}

	unsigned short c = szData[(int)charIdx].unicode();

	//First we can have a digit or a coma
	if(((c < '0') || (c > '9')))
	{
		// senseless : only a CTRL+K code
		(*byte_1)=KVI_NOCHANGE;
		(*byte_2)=KVI_NOCHANGE;
		return charIdx;
	}

	//Something interesting ok.
	(*byte_1)=c - '0'; //store the code
	charIdx++;
	if(charIdx >= (unsigned int) szData.length())
	{
		(*byte_2)=KVI_NOCHANGE;
		return charIdx;
	}

	c = szData[(int)charIdx].unicode();

	if(((c < '0') || (c > '9')) && (c != ','))
	{
		(*byte_2)=KVI_NOCHANGE;
		return charIdx;
	}


	if((c >= '0') && (c <= '9'))
	{
		(*byte_1)=(((*byte_1)*10)+(c-'0'))%16;
		charIdx++;
		if(charIdx >= (unsigned int) szData.length())
		{
			(*byte_2)=KVI_NOCHANGE;
			return charIdx;
		}
		c = szData[(int)charIdx].unicode();
	}

	if(c == ',')
	{
		charIdx++;
		if(charIdx >= (unsigned int) szData.length())
		{
			(*byte_2)=KVI_NOCHANGE;
			return charIdx;
		}
		c = szData[(int)charIdx].unicode();
	} else {
		(*byte_2)=KVI_NOCHANGE;
		return charIdx;
	}

	if((c < '0') || (c > '9'))
	{
		(*byte_2)=KVI_NOCHANGE;
		if(szData[(int)(charIdx-1)].unicode()==',')
			return charIdx-1;
		else
			return charIdx;
	}

	//Background , a color code
	(*byte_2)=c-'0';
	charIdx++;
	if(charIdx >= (unsigned int) szData.length())return charIdx;
	c = szData[(int)charIdx].unicode();

	if((c >= '0') && (c <='9'))
	{
		(*byte_2)=(((*byte_2)*10)+(c-'0'))%16;
		charIdx++;
	}

	return charIdx;
}


namespace KviMircCntrl
{
	QString stripControlBytes(const QString &szData)
	{
		QString ret;

		int i = 0;
		int l = szData.length();
		int begin = 0;
		unsigned char b1;
		unsigned char b2;
		while(i < l)
		{
			switch(szData[i].unicode())
			{
				case KVI_TEXT_UNDERLINE:
				case KVI_TEXT_BOLD:
				case KVI_TEXT_RESET:
				case KVI_TEXT_REVERSE:
				case KVI_TEXT_CRYPTESCAPE:
				case KVI_TEXT_CTCP:
					if(i != begin)
						ret += szData.mid(begin,i - begin);
					i++;
					begin = i;
				break;
				case KVI_TEXT_COLOR:
					if(i != begin)
						ret += szData.mid(begin,i - begin);
					i++;
					i = getUnicodeColorBytes(szData,i,&b1,&b2);
					begin = i;
				break;
				case KVI_TEXT_ICON:
					if(i != begin)
						ret += szData.mid(begin,i - begin);
					i++;
					while(i < l)
					{
						if(szData[i].unicode() == ' ')break;
						else i++;
					}
					begin = i;
				break;
				default:
					i++;
				break;
			}
		}
		if(i != begin)
			ret += szData.mid(begin,i - begin);
		return ret;
	}
}
