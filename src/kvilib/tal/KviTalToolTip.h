#ifndef _KVI_TAL_TOOLTIP_H_
#define _KVI_TAL_TOOLTIP_H_
//=============================================================================
//
//   File : KviTalToolTip.h
//   Creation date : Mon May 05 2008 11:25:08 by Alessandro Carbone
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include <QToolTip>
#include <QObject>

class KviTalToolTip;

// This is useful only with Qt4, but we put it here anyway
// to have both a consistent API and make moc happy
class KviTalToolTipHelper : public QObject
{
	friend class KviTalToolTip;
	Q_OBJECT
public:
	KviTalToolTipHelper(KviTalToolTip * pToolTip, QWidget * pWidget);
	~KviTalToolTipHelper();

protected:
	KviTalToolTip * m_pToolTip;

protected:
	bool eventFilter(QObject * pObject, QEvent * pEvent) override;
	void toolTipDying();
};

class KVILIB_API KviTalToolTip
{
	friend class KviTalToolTipHelper;

public:
	KviTalToolTip(QWidget * pParent);
	virtual ~KviTalToolTip();

protected:
	KviTalToolTipHelper * m_pHelper;
	QWidget * m_pParent;

public:
	static void add(QWidget * widget, const QString & text);
	static void remove(QWidget * widget);
	virtual void tip(const QRect & rect, const QString & text);

protected:
	virtual void maybeTip(const QPoint & p);
	void helperDying();
};

#endif // _KVI_TAL_TOOLTIP_H_
