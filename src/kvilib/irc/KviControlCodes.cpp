//=============================================================================
//
//   File : KviControlCodes.cpp
//   Creation date : Sat Aug 31 17:07:36 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_CONTROLCODE_CPP_

#include "KviControlCodes.h"

namespace KviControlCodes
{
	QString stripControlBytes(const QString & szData)
	{
		QString szRet;

		int i = 0;
		int iBegin = 0;
		unsigned char c1;
		unsigned char c2;
		while(i < szData.length())
		{
			switch(szData[i].unicode())
			{
				case KviControlCodes::Underline:
				case KviControlCodes::Bold:
				case KviControlCodes::Reset:
				case KviControlCodes::Reverse:
				case KviControlCodes::CryptEscape:
				case KviControlCodes::CTCP:
					if(i != iBegin)
						szRet += szData.mid(iBegin,i - iBegin);
					i++;
					iBegin = i;
				break;
				case KviControlCodes::Color:
					if(i != iBegin)
						szRet += szData.mid(iBegin,i - iBegin);
					i++;
					i = getUnicodeColorBytes(szData,i,&c1,&c2);
					iBegin = i;
				break;
				case KviControlCodes::Icon:
					if(i != iBegin)
						szRet += szData.mid(iBegin,i - iBegin);
					i++;
					/*
					* These lines will strip out the first word following the icon escape character
					while(i < l)
					{
						if(szData[i].unicode() == ' ')
							break;
						else i++;
					}
					*/
					iBegin = i;
				break;
				default:
					i++;
				break;
			}
		}
		if(i != iBegin)
			szRet += szData.mid(iBegin,i - iBegin);
		return szRet;
	}
	
	const kvi_wchar_t * getColorBytesW(const kvi_wchar_t * pwData, unsigned char * pcByte1, unsigned char * pcByte2)
	{
		//
		// Scans the pwData for a mIrc color code XX,XX
		// and fills the color values in the two bytes
		//

		//First we can have a digit or a comma
		if(((*pwData >= '0') && (*pwData <='9')))
		{
			//Something interesting ok.
			(*pcByte1) = ((*pwData)-'0'); //store the code
			pwData++;     //and check the next
			if(((*pwData >= '0') && (*pwData <= '9')) || (*pwData==','))
			{
				//Yes we can understand it
				if(*pwData == ',')
				{
					//A comma, need to check for background
					pwData++;
				} else {
					//A number
					//(*pcByte1)=((((*pcByte1)*10)+((*pwData)-'0'))%16);
					(*pcByte1) = ((*pcByte1)*10) + ((*pwData)-'0');
					pwData++;
					if(*pwData == ',')
					{
						//A comma, need to check for background
						pwData++;
					} else {
						//Senseless return
						(*pcByte2) = KviControlCodes::NoChange;
						return pwData;
					}
				}
			} else {
				//Senseless character control code OK and return
				(*pcByte2) = KviControlCodes::NoChange;
				return pwData;
			}
		} else {
			//Senseless character : only a CTRL+K code
			(*pcByte1) = KviControlCodes::NoChange;
			(*pcByte2) = KviControlCodes::NoChange;
			return pwData;
		}

		if((*pwData >= '0') && (*pwData <='9'))
		{
			//Background, a color code
			(*pcByte2) = (*pwData)-'0';
			pwData++;
			if((*pwData >= '0') && (*pwData <='9'))
			{
				//(*pcByte2)=((((*pcByte2)*10)+((*pwData)-'0'))%16);
				(*pcByte2) = ((*pcByte2)*10)+((*pwData)-'0');
				pwData++;
			}
			return pwData;
		} else {
			(*pcByte2) = KviControlCodes::NoChange;
			return pwData - 1;
		}
	}

	unsigned int getUnicodeColorBytes(const QString & szData, unsigned int iChar, unsigned char * pcByte1, unsigned char * pcByte2)
	{
		//
		// Scans the szData for a mIrc color code XX,XX
		// and fills the color values in the two bytes
		//

		if(iChar >= (unsigned int)szData.length())
		{
			(*pcByte1) = KviControlCodes::NoChange;
			(*pcByte2) = KviControlCodes::NoChange;
			return iChar;
		}

		unsigned short c = szData[(int)iChar].unicode();

		//First we can have a digit or a comma
		if(((c < '0') || (c > '9')))
		{
			// senseless : only a CTRL+K code
			(*pcByte1) = KviControlCodes::NoChange;
			(*pcByte2) = KviControlCodes::NoChange;
			return iChar;
		}

		//Something interesting ok.
		(*pcByte1) = c - '0'; //store the code
		iChar++;
		if(iChar >= (unsigned int)szData.length())
		{
			(*pcByte2) = KviControlCodes::NoChange;
			return iChar;
		}

		c = szData[(int)iChar].unicode();

		if(((c < '0') || (c > '9')) && (c != ','))
		{
			(*pcByte2) = KviControlCodes::NoChange;
			return iChar;
		}

		if((c >= '0') && (c <= '9'))
		{
			(*pcByte1) = (((*pcByte1)*10)+(c-'0'))%16;
			iChar++;
			if(iChar >= (unsigned int)szData.length())
			{
				(*pcByte2) = KviControlCodes::NoChange;
				return iChar;
			}
			c = szData[(int)iChar].unicode();
		}

		if(c == ',')
		{
			iChar++;
			if(iChar >= (unsigned int)szData.length())
			{
				(*pcByte2) = KviControlCodes::NoChange;
				return iChar;
			}
			c = szData[(int)iChar].unicode();
		} else {
			(*pcByte2) = KviControlCodes::NoChange;
			return iChar;
		}

		if((c < '0') || (c > '9'))
		{
			(*pcByte2) = KviControlCodes::NoChange;
			if(szData[(int)(iChar-1)].unicode()==',')
				return iChar-1;
			else
				return iChar;
		}

		//Background, a color code
		(*pcByte2) = c-'0';
		iChar++;
		if(iChar >= (unsigned int)szData.length())
			return iChar;
		c = szData[(int)iChar].unicode();

		if((c >= '0') && (c <='9'))
		{
			(*pcByte2) = (((*pcByte2)*10) + (c-'0')) % 16;
			iChar++;
		}

		return iChar;
	}

#if 0
	const char * getColorBytes(const char * pcData, unsigned char * pcByte1, unsigned char * pcByte2)
	{
		//
		// Scans the pcData for a mIrc color code XX,XX
		// and fills the color values in the two bytes
		//

		//First we can have a digit or a comma
		if(((*pcData >= '0') && (*pcData <='9')))
		{
			//Something interesting ok.
			(*pcByte1)=(*pcData)-'0'; //store the code
			pcData++;     //and check the next
			if(((*pcData >= '0') && (*pcData <= '9'))||(*pcData==','))
			{
				//Yes we can understand it
				if(*pcData==',')
				{
					//A comma, need to check for background
					pcData++;
				} else {
					//A number
					(*pcByte1)=((((*pcByte1)*10)+((*pcData)-'0'))%16);
					pcData++;
					if(*pcData==',')
					{
						//A comma, need to check for background
						pcData++;
					} else {
						//Senseless return
						(*pcByte2)=KviControlCodes::NoChange;
						return pcData;
					}
				}
			} else {
				//Senseless character control code OK and return
				(*pcByte2)=KviControlCodes::NoChange;
				return pcData;
			}
		} else {
			//Senseless character : only a CTRL+K code
			(*pcByte1)=KviControlCodes::NoChange;
			(*pcByte2)=KviControlCodes::NoChange;
			return pcData;
		}

		if((*pcData >= '0') && (*pcData <='9'))
		{
			//Background, a color code
			(*pcByte2)=(*pcData)-'0';
			pcData++;
			if((*pcData >= '0') && (*pcData <='9'))
			{
				(*pcByte2)=((((*pcByte2)*10)+((*pcData)-'0'))%16);
				pcData++;
			}
			return pcData;
		} else {
			(*pcByte2)=KviControlCodes::NoChange;
			return pcData-1;
		}
	}
#endif
}
