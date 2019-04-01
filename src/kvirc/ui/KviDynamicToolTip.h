#ifndef _KVI_DYNAMICTOOLTIP_H_
#define _KVI_DYNAMICTOOLTIP_H_
//=============================================================================
//
//   File : KviDynamicToolTip.h
//   Creation date : Wed Nov 01 2000 15:23:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviTalToolTip.h"

#include <QObject>

class KviDynamicToolTip;

class KVIRC_API KviDynamicToolTipHelper : public KviTalToolTip
{
	friend class KviDynamicToolTip;

protected:
	KviDynamicToolTipHelper(QWidget * parent, KviDynamicToolTip * parToolTip);
	virtual ~KviDynamicToolTipHelper();

protected:
	KviDynamicToolTip * m_pParentToolTip;

protected:
	virtual void maybeTip(const QPoint & pnt);
};

class KVIRC_API KviDynamicToolTip : public QObject
{
	friend class KviDynamicToolTipHelper;
	Q_OBJECT
public:
	KviDynamicToolTip(QWidget * parent, const char * name = nullptr);
	virtual ~KviDynamicToolTip();

protected:
	KviDynamicToolTipHelper * m_pHelper;

public:
	void tip(const QRect & rct, const QString & text);

protected:
	void maybeTip(const QPoint & pnt);
signals:
	void tipRequest(KviDynamicToolTip * tip, const QPoint & pnt);
};

#endif //!_KVI_DYNAMICTOOLTIP_H_
