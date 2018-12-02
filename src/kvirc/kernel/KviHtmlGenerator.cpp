//=============================================================================
//
//   File : KviHtmlGenerator.cpp
//   Creation date : Mon 04 Jan 2010 22:09:18 by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Fabio Bas < ctrlaltca at gmail dot com >
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

#include "KviHtmlGenerator.h"

#include "KviIconManager.h"
#include "KviControlCodes.h"
#include "KviOptions.h"
#include "KviTextIconManager.h"

namespace KviHtmlGenerator
{
	QString convertToHtml(const QString & szText, bool bEscape)
	{
		QString szResult = "<qt>";
		bool bCurBold = false;
		bool bCurItalic = false;
		bool bCurUnderline = false;
		bool bIgnoreIcons = false;
		bool bShowIcons = KVI_OPTION_BOOL(KviOption_boolDrawEmoticons);
		unsigned char uCurFore = Foreground;
		unsigned char uCurBack = Background;
		unsigned int uIdx = 0;

		QString szTxt = bEscape ? KviQString::toHtmlEscaped(szText) : szText;

		while(uIdx < (unsigned int)szTxt.length())
		{
			unsigned short c = szTxt[(int)uIdx].unicode();
			unsigned int uStart = uIdx;

			while(
			    (c != KviControlCodes::Color) && (c != KviControlCodes::Bold) && (c != KviControlCodes::Italic) && (c != KviControlCodes::Underline) && (c != KviControlCodes::Reverse) && (c != KviControlCodes::Reset) && (c != KviControlCodes::Icon) && ((c != ':') || bIgnoreIcons) && ((c != ';') || bIgnoreIcons) && ((c != '=') || bIgnoreIcons))
			{
				bIgnoreIcons = false;
				if(c == '&')
				{
					//look for an html entity
					QString szEntity = szTxt.mid((int)uIdx, 6);
					if(szEntity == "&quot;")
					{
						uIdx += 5;
					}
					else
					{
						szEntity.truncate(5);
						if(szEntity == "&amp;")
						{
							uIdx += 4;
						}
						else
						{
							szEntity.truncate(4);
							if(szEntity == "&lt;" || szEntity == "&gt;")
								uIdx += 3;
						}
					}
				}

				uIdx++;
				if(uIdx >= (unsigned int)szTxt.length())
					break;

				c = szTxt[(int)uIdx].unicode();
			}

			bIgnoreIcons = false;
			int iLen = uIdx - uStart;

			if(iLen > 0)
			{
				bool bOpened = false;

				if(uCurFore != Foreground)
				{
					szResult.append("<span style=\"color:");
					if(uCurFore == Background) // this is the result of reverse
						szResult.append(getMircColor(KviControlCodes::White).name());
					else
						szResult.append(getMircColor(uCurFore).name());
					bOpened = true;
				}

				if(uCurBack != Background)
				{
					if(!bOpened)
					{
						szResult.append("<span style=\"background-color:");
						bOpened = true;
					}
					else
					{
						szResult.append(";background-color:");
					}
					if(uCurBack == Foreground) // this is the result of reverse
						szResult.append(getMircColor(KviControlCodes::Black).name());
					else
						szResult.append(getMircColor(uCurBack).name());
				}

				if(bCurUnderline)
				{
					if(!bOpened)
					{
						szResult.append("<span style=\"text-decoration:underline");
						bOpened = true;
					}
					else
					{
						szResult.append(";text-decoration:underline");
					}
				}

				if(bCurBold)
				{
					if(!bOpened)
					{
						szResult.append("<span style=\"font-weight:bold");
						bOpened = true;
					}
					else
					{
						szResult.append(";font-weight:bold");
					}
				}

				if(bCurItalic)
				{
					if(!bOpened)
					{
						szResult.append("<span style=\"font-style:italic");
						bOpened = true;
					}
					else
					{
						szResult.append(";font-style:italic");
					}
				}

				if(bOpened)
					szResult.append(";\">");

				szResult.append(szTxt.mid(uStart, iLen));

				if(bOpened)
					szResult.append("</span>");
			}

			switch(c)
			{
				case KviControlCodes::Bold:
				{
					bCurBold = !bCurBold;
					++uIdx;
					break;
				}
				case KviControlCodes::Italic:
				{
					bCurItalic = !bCurItalic;
					++uIdx;
					break;
				}
				case KviControlCodes::Underline:
				{
					bCurUnderline = !bCurUnderline;
					++uIdx;
					break;
				}
				case KviControlCodes::Reverse:
				{
					std::swap(uCurFore, uCurBack);
					++uIdx;
					break;
				}
				case KviControlCodes::Reset:
				{
					uCurFore = Foreground;
					uCurBack = Background;
					bCurBold = false;
					bCurItalic = false;
					bCurUnderline = false;
					++uIdx;
					break;
				}
				case KviControlCodes::Color:
				{
					++uIdx;
					unsigned char ucFore;
					unsigned char ucBack;
					uIdx = KviControlCodes::getUnicodeColorBytes(szTxt, uIdx, &ucFore, &ucBack);
					if(ucFore != KviControlCodes::NoChange)
					{
						uCurFore = ucFore;
						if(ucBack != KviControlCodes::NoChange)
							uCurBack = ucBack;
					}
					else
					{
						// only a Ctrl+K
						uCurBack = Background;
						uCurFore = Foreground;
					}
					break;
				}
				case ':':
				case ';':
				case '=':
				{
					//potential emoticon, got eyes
					if(bShowIcons)
					{
						++uIdx;
						QString szLookup;
						szLookup.append(QChar(c));
						unsigned short uIsEmoticon = 0;
						unsigned int uIcoStart = uIdx;

						if(uIdx < (unsigned int)szTxt.length())
						{
							//look up for a nose
							if(szTxt[(int)uIdx] == '-')
							{
								szLookup.append('-');
								uIdx++;
							}
						}

						if(uIdx < (unsigned int)szTxt.length())
						{
							//look up for a mouth
							unsigned short uMouth = szTxt[(int)uIdx].unicode();
							switch(uMouth)
							{
								case ')':
								case '(':
								case '/':
								case 'D':
								case 'P':
								case 'S':
								case 'O':
								case '*':
								case '|':
									szLookup += QChar(uMouth);
									uIsEmoticon++;
									uIdx++;
									break;
								default:
									break;
							}
						}

						if(uIdx < (unsigned int)szTxt.length())
						{
							//look up for a space
							if(szTxt[(int)uIdx] == ' ')
								uIsEmoticon++;
						}
						else
						{
							//got a smile at the end of the szTxt
							uIsEmoticon++;
						}

						if(uIsEmoticon > 1)
						{
							KviTextIcon * pIcon = g_pTextIconManager->lookupTextIcon(szLookup);
							// do we have that emoticon-icon association ?
							if(pIcon)
							{
								szResult.append("<img src=\"");
								szResult.append(g_pIconManager->getSmallIconResourceName(pIcon->id()));
								if(uCurBack != Background)
								{
									szResult.append("\" style=\"background-color:");
									szResult.append(getMircColor(uCurBack).name());
								}
								szResult.append("\" />");
							}
							else
							{
								bIgnoreIcons = true;
								uIdx = uIcoStart - 1;
							}
						}
						else
						{
							bIgnoreIcons = true;
							uIdx = uIcoStart - 1;
						}
					}
					else
					{
						bIgnoreIcons = true;
					}

					break;
				}
				case KviControlCodes::Icon:
				{
					++uIdx;
					if(bShowIcons)
					{
						unsigned int uIcoStart = uIdx;
						while((uIdx < (unsigned int)szTxt.length()) && (szTxt[(int)uIdx].unicode() > 32))
							uIdx++;

						QString szLookup = szTxt.mid(uIcoStart, uIdx - uIcoStart);

						KviTextIcon * pIcon = g_pTextIconManager->lookupTextIcon(szLookup);
						if(pIcon)
						{
							szResult.append("<img src=\"");
							szResult.append(g_pIconManager->getSmallIconResourceName(pIcon->id()));
							if(uCurBack != Background)
							{
								szResult.append("\" style=\"background-color:");
								szResult.append(getMircColor(uCurBack).name());
							}
							szResult.append("\" />");
						}
						else
						{
							uIdx = uIcoStart;
						}
					}
					break;
				}
			}
		}
		szResult.append("</qt>");
		//qDebug("Results %s",szResult.toUtf8().data());
		return szResult;
	}
}
