#ifndef _KVI_TAL_VBOX_H_
#define _KVI_TAL_VBOX_H_
//=============================================================================
//
//   File : KviTalVBox.h
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

/**
* \file KviTalVBox.h
* \author Szymon Stefanek
* \brief Class for vertical box
*/

#include "kvi_settings.h"

#include <QWidget>
#include <QVBoxLayout>

/**
* \class KviTalVBox
* \brief Toolkit Abstraction Layer: vbox class
*/
class KVILIB_API KviTalVBox : public QWidget
{
	Q_OBJECT
public:
	/**
	* \brief Constructs a vertical box object
	* \param pParent The parent object
	* \param pcName The name of the box
	* \return KviTalVBox
	*/
	KviTalVBox(QWidget * pParent, char * pcName = nullptr);

	/**
	* \brief Destroys a vertical box object
	*/
	~KviTalVBox();

private:
	QVBoxLayout * m_pLayout;

public:
	/**
	* \brief Sets the stretch factor for an object
	* \param pChild The object to stretch
	* \param iStretch The stretch factor to use
	* \return void
	*/
	void setStretchFactor(QWidget * pChild, int iStretch);

	/**
	* \brief Sets the spacing of the box
	* \param iSpacing The space in pixels
	* \return void
	*/
	void setSpacing(int iSpacing);

	/**
	* \brief Sets the margin of the box
	* \param iMargin The margin in pixels
	* \return void
	*/
	void setMargin(int iMargin);

	/**
	* \brief Sets the alignment of the box
	* \param alignment The alignment
	* \return void
	*/
	void setAlignment(Qt::Alignment alignment);

	/**
	* \brief Sets the alignment of the box for a child object
	* \param pChild The object to align
	* \param alignment The alignment
	* \return void
	*/
	void setAlignment(QWidget * pChild, Qt::Alignment alignment);

	/**
	* \brief Adds the stretch factor
	* \param iStretch The stretch factor
	* \return void
	*/
	void addStretch(int iStretch);

protected:
	void childEvent(QChildEvent * e) override;
};

#endif // _KVI_TAL_VBOX_H_
