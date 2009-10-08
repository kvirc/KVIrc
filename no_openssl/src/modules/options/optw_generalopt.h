#ifndef _OPTW_GENERALOPT_H_
#define _OPTW_GENERALOPT_H_
//=============================================================================
//
//   File : optw_generalopt.h
//   Creation date : Wed Nov 19 09:25:16 2005 GMT by Grifisx & Noldor
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#define KVI_OPTIONS_WIDGET_ICON_KviGeneralOptOptionsWidget KVI_SMALLICON_OPTIONS
#define KVI_OPTIONS_WIDGET_NAME_KviGeneralOptOptionsWidget __tr2qs_no_lookup("General options")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviGeneralOptOptionsWidget 40000

class KviGeneralOptOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviGeneralOptOptionsWidget(QWidget * parent);
	~KviGeneralOptOptionsWidget();
};

#endif //_OPTW_CONNECTION_H_
