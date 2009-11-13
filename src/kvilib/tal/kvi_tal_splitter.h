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

/**
* \file kvi_tal_splitter.h
* \author fabio bas
* \brief Class used to workaround a strange behaviour in QSplitter
*/

#include "kvi_settings.h"
#include <QSplitter>

/**
* \class KviTalSplitter
* \brief Toolkit Abstraction Layer: splitter class
*/

class KVILIB_API KviTalSplitter : public QSplitter
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the splitter object
	* \param orientation The orientation of the splitter: horizontal or vertical
	* \param pParent the parent object
	* \return KviTalSplitter
	*/
	KviTalSplitter(Qt::Orientation orientation, QWidget * pParent = 0);

	/**
	* \brief Destroys the splitter object
	*/
	virtual ~KviTalSplitter();
private:
	bool bHasValidSizes;
	QList<int> oldSizes;
public:
	/**
	* \brief Returns the list of sizes of the items contained inside the splitter
	* \return QList<int>
	*/
	QList<int> sizes();

	/**
	* \brief Sets the initial sizes for items contained inside the splitter
	* \param sizes The list of sizes
	* \return void
	*/
	void setSizes(QList<int> sizes);
protected slots:
	void splitterHasMoved(int pos, int index);

};

#endif // _KVI_TAL_SPLITTER_H_
