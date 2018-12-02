//=============================================================================
//
//   File : KviControlCodes.cpp
//   Creation date : Sat Aug 31 17:07:36 2002 GMT by Szymon Stefanek
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

#define _KVI_CONTROLCODE_CPP_

#include "KviControlCodes.h"

#include <QString>

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
				case KviControlCodes::Italic:
				case KviControlCodes::Reset:
				case KviControlCodes::Reverse:
				case KviControlCodes::CryptEscape:
				case KviControlCodes::CTCP:
				case KviControlCodes::Icon:
					if(i != iBegin)
						szRet += szData.mid(iBegin, i - iBegin);
					i++;
					iBegin = i;
					break;
				case KviControlCodes::Color:
					if(i != iBegin)
						szRet += szData.mid(iBegin, i - iBegin);
					i++;
					i = getUnicodeColorBytes(szData, i, &c1, &c2);
					iBegin = i;
					break;
				default:
					i++;
					break;
			}
		}
		if(i != iBegin)
			szRet += szData.mid(iBegin, i - iBegin);
		return szRet;
	}

	const kvi_wchar_t * getColorBytesW(const kvi_wchar_t * pwData, unsigned char * pcByte1, unsigned char * pcByte2)
	{
		//
		// Scans the pwData for a mIRC color code XX,XX
		// and fills the color values in the two bytes
		//

		//First we can have a digit or a comma
		if(((*pwData >= '0') && (*pwData <= '9')))
		{
			//Something interesting ok.
			(*pcByte1) = ((*pwData) - '0'); //store the code
			pwData++;                       //and check the next
			if(((*pwData >= '0') && (*pwData <= '9')) || (*pwData == ','))
			{
				//Yes we can understand it
				if(*pwData == ',')
				{
					//A comma, need to check for background
					pwData++;
				}
				else
				{
					//A number
					//(*pcByte1)=((((*pcByte1)*10)+((*pwData)-'0'))%16);
					(*pcByte1) = ((*pcByte1) * 10) + ((*pwData) - '0');
					pwData++;
					if(*pwData == ',')
					{
						//A comma, need to check for background
						pwData++;
					}
					else
					{
						//Senseless return
						(*pcByte2) = KviControlCodes::NoChange;
						return pwData;
					}
				}
			}
			else
			{
				//Senseless character control code OK and return
				(*pcByte2) = KviControlCodes::NoChange;
				return pwData;
			}
		}
		else
		{
			//Senseless character : only a Ctrl+K code
			(*pcByte1) = KviControlCodes::NoChange;
			(*pcByte2) = KviControlCodes::NoChange;
			return pwData;
		}

		if((*pwData >= '0') && (*pwData <= '9'))
		{
			//Background, a color code
			(*pcByte2) = (*pwData) - '0';
			pwData++;
			if((*pwData >= '0') && (*pwData <= '9'))
			{
				//(*pcByte2)=((((*pcByte2)*10)+((*pwData)-'0'))%16);
				(*pcByte2) = ((*pcByte2) * 10) + ((*pwData) - '0');
				pwData++;
			}
			return pwData;
		}
		else
		{
			(*pcByte2) = KviControlCodes::NoChange;
			return pwData - 1;
		}
	}

	unsigned int getUnicodeColorBytes(const QString & szData, unsigned int iChar, unsigned char * pcByte1, unsigned char * pcByte2)
	{
		//
		// Scans the szData for a mIRC color code XX,XX
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
			//Senseless : only a Ctrl+K code
			(*pcByte1) = KviControlCodes::NoChange;
			(*pcByte2) = KviControlCodes::NoChange;
			return iChar;
		}

		//Something interesting OK.
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
			(*pcByte1) = (((*pcByte1) * 10) + (c - '0'));
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
		}
		else
		{
			(*pcByte2) = KviControlCodes::NoChange;
			return iChar;
		}

		if((c < '0') || (c > '9'))
		{
			(*pcByte2) = KviControlCodes::NoChange;
			if(szData[(int)(iChar - 1)].unicode() == ',')
				return iChar - 1;
			else
				return iChar;
		}

		//Background, a color code
		(*pcByte2) = c - '0';
		iChar++;
		if(iChar >= (unsigned int)szData.length())
			return iChar;
		c = szData[(int)iChar].unicode();

		if((c >= '0') && (c <= '9'))
		{
			(*pcByte2) = (((*pcByte2) * 10) + (c - '0'));
			iChar++;
		}

		return iChar;
	}

#if 0
	const char * getColorBytes(const char * pcData, unsigned char * pcByte1, unsigned char * pcByte2)
	{
		//
		// Scans the pcData for a mIRC color code XX,XX
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
					(*pcByte1)=((((*pcByte1)*10)+((*pcData)-'0')));
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
			//Senseless character : only a Ctrl+K code
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
				(*pcByte2)=((((*pcByte2)*10)+((*pcData)-'0')));
				pcData++;
			}
			return pcData;
		} else {
			(*pcByte2)=KviControlCodes::NoChange;
			return pcData-1;
		}
	}
#endif

	// Get extended (16-98) mIRC color.
	// Unlike the 0-15 ones, these are not configurable.
	// https://modern.ircdocs.horse/formatting.html#colors-16-98
	// Returns (kvi_u32_t)-1 if index is out of bounds.
	kvi_u32_t getExtendedColor(int index)
	{
		const int minColor = KVI_MIRCCOLOR_MAX + 1; // 16
		const int maxColor = KVI_EXTCOLOR_MAX;
		static const kvi_u32_t colors[maxColor - minColor + 1] = {
			0x470000,
			0x472100,
			0x474700,
			0x324700,
			0x004700,
			0x00472c,
			0x004747,
			0x002747,
			0x000047,
			0x2e0047,
			0x470047,
			0x47002a,
			0x740000,
			0x743a00,
			0x747400,
			0x517400,
			0x007400,
			0x007449,
			0x007474,
			0x004074,
			0x000074,
			0x4b0074,
			0x740074,
			0x740045,
			0xb50000,
			0xb56300,
			0xb5b500,
			0x7db500,
			0x00b500,
			0x00b571,
			0x00b5b5,
			0x0063b5,
			0x0000b5,
			0x7500b5,
			0xb500b5,
			0xb5006b,
			0xff0000,
			0xff8c00,
			0xffff00,
			0xb2ff00,
			0x00ff00,
			0x00ffa0,
			0x00ffff,
			0x008cff,
			0x0000ff,
			0xa500ff,
			0xff00ff,
			0xff0098,
			0xff5959,
			0xffb459,
			0xffff71,
			0xcfff60,
			0x6fff6f,
			0x65ffc9,
			0x6dffff,
			0x59b4ff,
			0x5959ff,
			0xc459ff,
			0xff66ff,
			0xff59bc,
			0xff9c9c,
			0xffd39c,
			0xffff9c,
			0xe2ff9c,
			0x9cff9c,
			0x9cffdb,
			0x9cffff,
			0x9cd3ff,
			0x9c9cff,
			0xdc9cff,
			0xff9cff,
			0xff94d3,
			0x000000,
			0x131313,
			0x282828,
			0x363636,
			0x4d4d4d,
			0x656565,
			0x818181,
			0x9f9f9f,
			0xbcbcbc,
			0xe2e2e2,
			0xffffff,
		};
		if (index < minColor || index > maxColor)
			return (kvi_u32_t)-1;
		return colors[index - minColor];
	}
}
