#ifndef _ABOUTDIALOG_H_
#define _ABOUTDIALOG_H_
//=============================================================================
//
//   File : AboutDialog.h
//   Creation date : Sun Jun 23 17:59:13 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviTalTabDialog.h"

class AboutDialog : public KviTalTabDialog
{
	Q_OBJECT
public:
	AboutDialog();
	~AboutDialog();

protected:
	void closeEvent(QCloseEvent * e) override;
protected slots:
	void closeButtonPressed();
};

#endif //_ABOUTDIALOG_H_
