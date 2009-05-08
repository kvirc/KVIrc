#ifndef _OPTW_CTCP_H_
#define _OPTW_CTCP_H_
//=============================================================================
//
//   File : optw_ctcp.h
//   Creation date : Sun Dec  2 20:40:43 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_optionswidget.h"

#define KVI_OPTIONS_WIDGET_ICON_KviCtcpOptionsWidget KVI_SMALLICON_CTCPREQUESTREPLIED
#define KVI_OPTIONS_WIDGET_NAME_KviCtcpOptionsWidget __tr2qs_no_lookup("CTCP")
#define KVI_OPTIONS_WIDGET_PARENT_KviCtcpOptionsWidget KviToolsOptionsWidget
#define KVI_OPTIONS_WIDGET_PRIORITY_KviCtcpOptionsWidget 70000

class KviCtcpOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviCtcpOptionsWidget(QWidget * parent);
	~KviCtcpOptionsWidget();
};


#endif //_OPTW_CTCP_H_
