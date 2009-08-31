#ifndef _KVI_TAL_SPLITTER_H_
#define _KVI_TAL_SPLITTER_H_

//=============================================================================
//
//   File : kvi_tal_splitter.cpp
//   Creation date : Mon Aug 31 2009 19:47:08 by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2009 Fabio Bas (ctrlaltca at libero dot it)
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

#include "kvi_settings.h"
#include <QSplitter>

class KVILIB_API KviTalSplitter : public QSplitter
{
	Q_OBJECT
public:
	KviTalSplitter(Qt::Orientation orientation, QWidget * parent = 0)
	: QSplitter(orientation, parent), bHasValidSizes(false) {};
	virtual ~KviTalSplitter() {};
	QList<int> sizes() { return bHasValidSizes ? QSplitter::sizes() : oldSizes; };
	void setSizes(QList<int> sizes) { oldSizes = sizes; QSplitter::setSizes(oldSizes); };
protected:
	virtual void showEvent(QShowEvent * event) { if(!bHasValidSizes) bHasValidSizes=true; QSplitter::showEvent(event); };
private:
	bool bHasValidSizes;
	QList<int> oldSizes;
};

#endif // _KVI_TAL_SPLITTER_H_
