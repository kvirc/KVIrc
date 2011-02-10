//=============================================================================
//
//   File : KviHtmlGenerator.cpp
//   Creation date : Mon 04 Jan 2010 22:09:18 by Fabio Bas
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2010 Fabio Bas < ctrlaltca at gmail dot com >
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

#include "KviHtmlGenerator.h"

#include "KviIconManager.h"
#include "KviControlCodes.h"
#include "KviOptions.h"
#include "KviTextIconManager.h"

#define KVI_LABEL_DEF_BACK 100
#define KVI_LABEL_DEF_FORE 101

namespace KviHtmlGenerator
{

	QString convertToHtml(const QString &szText)
	{
		QString szResult="<qt>";
		bool bCurBold      = false;
		bool bCurUnderline = false;
		bool bIgnoreIcons = false;
		bool bShowIcons = KVI_OPTION_BOOL(KviOption_boolDrawEmoticons);

		unsigned char uCurFore      = KVI_LABEL_DEF_FORE; //default fore
		unsigned char uCurBack      = KVI_LABEL_DEF_BACK; //default back

		unsigned int uIdx = 0;

		while(uIdx < (unsigned int)szText.length())
		{
			unsigned short c = szText[(int)uIdx].unicode();
			unsigned int uStart = uIdx;

			while(
					(c != KviControlCodes::Color) &&
					(c != KviControlCodes::Bold) &&
					(c != KviControlCodes::Underline) &&
					(c != KviControlCodes::Reverse) &&
					(c != KviControlCodes::Reset) &&
					(c != KviControlCodes::Icon) &&
					((c != ':') || bIgnoreIcons) &&
					((c != ';') || bIgnoreIcons) &&
					((c != '=') || bIgnoreIcons)
				)
			{
				bIgnoreIcons=FALSE;
				if(c == '&')
				{
					//look for an html entity
					QString szEntity = szText.mid((int)uIdx,6);
					if(szEntity=="&quot;")
					{
						uIdx+=5;
					} else {
						szEntity.truncate(5);
						if(szEntity=="&amp;")
						{
							 uIdx+=4;
						} else {
							szEntity.truncate(4);
							if(szEntity=="&lt;" || szEntity=="&gt;") uIdx+=3;
						}
					}
				}

				uIdx++;
				if(uIdx >= (unsigned int)szText.length()) break;
				else c = szText[(int)uIdx].unicode();
			}

			bIgnoreIcons=FALSE;
			int iLen = uIdx - uStart;

			if(iLen > 0)
			{
				bool bOpened = FALSE;

				if(uCurFore != KVI_LABEL_DEF_FORE)
				{
					szResult.append("<span style=\"color:");
					szResult.append(KVI_OPTION_MIRCCOLOR(uCurFore).name());
					bOpened = TRUE;
				}

				if(uCurBack != KVI_LABEL_DEF_BACK)
				{
					if(!bOpened)
					{
						szResult.append("<span style=\"background-color:");
						bOpened = TRUE;
					} else {
						szResult.append(";background-color:");
					}
					szResult.append(KVI_OPTION_MIRCCOLOR(uCurBack).name());
				}

				if(bCurUnderline)
				{
					if(!bOpened)
					{
						szResult.append("<span style=\"text-decoration:underline");
						bOpened = TRUE;
					} else {
						szResult.append(";text-decoration:underline");
					}
				}

				if(bCurBold)
				{
					if(!bOpened)
					{
						szResult.append("<span style=\"font-weight:bold");
						bOpened = TRUE;
					} else {
						szResult.append(";font-weight:bold");
					}
				}

				if(bOpened) szResult.append(";\">");

				szResult.append(szText.mid(uStart,iLen));

				if( bOpened )
					szResult.append("</span>");
			}

			switch(c)
			{
				case KviControlCodes::Bold:
					bCurBold = !bCurBold; ++uIdx;
				break;
				case KviControlCodes::Underline:
					bCurUnderline = !bCurUnderline; ++uIdx;
				break;
				case KviControlCodes::Reverse:
					{
						char auxBack = uCurBack;
						uCurBack = uCurFore;
						uCurFore = auxBack;
					}
					++uIdx;
				break;
				case KviControlCodes::Reset:
					uCurFore = KVI_LABEL_DEF_FORE;
					uCurBack = KVI_LABEL_DEF_BACK;
					bCurBold = false;
					bCurUnderline = false;
					++uIdx;
				break;
				case KviControlCodes::Color:
				{
					++uIdx;
					unsigned char fore;
					unsigned char back;
					uIdx = KviControlCodes::getUnicodeColorBytes(szText,uIdx,&fore,&back);
					if(fore != KviControlCodes::NoChange)
					{
						uCurFore = fore;
						if(back != KviControlCodes::NoChange)
							uCurBack = back;
					} else {
						// only a CTRL+K
						uCurBack = KVI_LABEL_DEF_BACK;
						uCurFore = KVI_LABEL_DEF_FORE;
					}
				}
				break;
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
						unsigned short uIsEmoticon=0;
						unsigned int uIcoStart = uIdx;

						if(uIdx < (unsigned int)szText.length())
						{
							//look up for a nose
							if(szText[(int)uIdx] == '-')
							{
								szLookup.append('-');
								uIdx++;
							}
						}

						if(uIdx < (unsigned int)szText.length())
						{
							//look up for a mouth
							unsigned short m = szText[(int)uIdx].unicode();
							switch(m)
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
									szLookup+=QChar(m);
									uIsEmoticon++;
									uIdx++;
									break;
								default:
									break;
							}
						}

						if(uIdx < (unsigned int)szText.length())
						{
							//look up for a space
							if(szText[(int)uIdx]== ' ')
							{
								uIsEmoticon++;
							}
						} else {
							//got a smile at the end of the szText
							uIsEmoticon++;
						}

						if(uIsEmoticon>1)
						{
							KviTextIcon * pIcon  = g_pTextIconManager->lookupTextIcon(szLookup);
							// do we have that emoticon-icon association ?
							if(pIcon)
							{
								szResult.append("<img src=\"");
								szResult.append(g_pIconManager->getSmallIconResourceName(pIcon->id()));
								if(uCurBack != KVI_LABEL_DEF_BACK)
								{
									szResult.append("\" style=\"background-color:");
									szResult.append(KVI_OPTION_MIRCCOLOR(uCurBack).name());
								}
								szResult.append("\">");
							} else {
								bIgnoreIcons=TRUE;
								uIdx = uIcoStart-1;
							}
						} else {
							bIgnoreIcons=TRUE;
							uIdx = uIcoStart-1;
						}
					} else {
						bIgnoreIcons=TRUE;
					}
				}
				break;
				case KviControlCodes::Icon:
				{
					++uIdx;
					if(bShowIcons)
					{
						unsigned int uIcoStart = uIdx;
						while((uIdx < (unsigned int)szText.length()) && (szText[(int)uIdx].unicode() > 32))uIdx++;

						QString szLookup = szText.mid(uIcoStart,uIdx - uIcoStart);

						KviTextIcon * pIcon = g_pTextIconManager->lookupTextIcon(szLookup);
						if(pIcon)
						{
							szResult.append("<img src=\"");
							szResult.append(g_pIconManager->getSmallIconResourceName(pIcon->id()));
							if(uCurBack != KVI_LABEL_DEF_BACK)
							{
								szResult.append("\" style=\"background-color:");
								szResult.append(KVI_OPTION_MIRCCOLOR(uCurBack).name());
							}
							szResult.append("\">");
						} else {
							uIdx = uIcoStart;
						}
					}
				}
				break;
			}
		}
		szResult.append("</qt>");
		//qDebug("%s",szResult.toUtf8().data());
		return szResult;
	}
}
