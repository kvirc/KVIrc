#ifndef _KVI_NETWORKACCESSMANAGER_H_
#define _KVI_NETWORKACCESSMANAGER_H_
//=============================================================================
//
//   File : KviNetworkAccessManager.h
//   Creation date : Sat Sep 29 2012 17:15:12 CEST by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2012 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#include <QNetworkAccessManager>
// useful for users of this singleton
#include <QNetworkRequest>
#include <QNetworkReply>

class KVILIB_API KviNetworkAccessManager
{
private:
	KviNetworkAccessManager();
	~KviNetworkAccessManager();

public:
	static QNetworkAccessManager * getInstance()
	{
		static QNetworkAccessManager * pInstance = nullptr;
		if(!pInstance)
			pInstance = new QNetworkAccessManager();
		return pInstance;
	}
};

#endif //_KVI_NETWORKACCESSMANAGER_H_
