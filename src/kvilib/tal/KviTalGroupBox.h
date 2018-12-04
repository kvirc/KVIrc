#ifndef _KVI_TAL_GROUPBOX_H_
#define _KVI_TAL_GROUPBOX_H_
//=============================================================================
//
//   File : KviTalGroupBox.h
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviTalGroupBox.h
* \author Szymon Stefanek
* \brief Class for groupbox
*/

#include "kvi_settings.h"

#include <QGroupBox>
#include <QLayout>

class QChildEvent;

/**
* \class KviTalGroupBox
* \brief Toolkit Abstraction Layer: groupbox class
*/
class KVILIB_API KviTalGroupBox : public QGroupBox
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the groupbox object
	* \param pParent The parent object
	* \param pcName the name of the groupbox
	* \return KviTalGroupBox
	*/
	KviTalGroupBox(QWidget * pParent, char * pcName = nullptr);

	/**
	* \brief Constructs the groupbox object
	* \param pParent The parent object
	* \return KviTalGroupBox
	*/
	KviTalGroupBox(QWidget * pParent = nullptr);

	/**
	* \brief Constructs the groupbox object
	* \param szTitle The title of the groupbox
	* \param pParent The parent object
	* \return KviTalGroupBox
	*/
	KviTalGroupBox(const QString & szTitle, QWidget * pParent = nullptr);

	/**
	* \brief Constructs the groupbox object
	* \param orientation The orientation of the groupbox
	* \param pParent The parent object
	* \return KviTalGroupBox
	*/
	KviTalGroupBox(Qt::Orientation orientation, QWidget * pParent = nullptr);

	/**
	* \brief Constructs the groupbox object
	* \param orientation The orientation of the groupbox
	* \param szTitle The title of the groupbox
	* \param pParent The parent object
	* \return KviTalGroupBox
	*/
	KviTalGroupBox(Qt::Orientation orientation, const QString & szTitle, QWidget * pParent = nullptr);

	/**
	* \brief Destroys the groupbox object
	*/
	~KviTalGroupBox();

protected:
	Qt::Orientation mOrientation;
	QLayout * m_pLayout;

public:
	/**
	* \brief Sets the inside margin
	* \param iMargin The margin in pixels
	* \return void
	*/
	void setInsideMargin(int iMargin)
	{
		if(layout())
			layout()->setMargin(iMargin);
	};

	/**
	* \brief Sets the inside spacing
	* \param iSpacing The spacing in pixels
	* \return void
	*/
	void setInsideSpacing(int iSpacing)
	{
		if(layout())
			layout()->setSpacing(iSpacing);
	};

	/**
	* \brief Returns the inside margin
	* \return int
	*/
	int insideMargin()
	{
		if(layout())
			return layout()->margin();
		return 0;
	};

	/**
	* \brief Returns the inside spacing
	* \return int
	*/
	int insideSpacing()
	{
		if(layout())
			return layout()->spacing();
		return 0;
	};

	/**
	* \brief Adds a space
	* \param iSpace The space in pixels
	* \return void
	*/
	void addSpace(int iSpace);

	/**
	* \brief Sets the orientation
	* \param orientation The orientation :)
	* \return void
	*/
	void setOrientation(Qt::Orientation orientation);

	/**
	* \brief Sets a new layout
	* \param newLayout The new layout
	* \return void
	*/
	void setLayout(QLayout * newLayout);

protected:
	void childEvent(QChildEvent * e) override;
};

#endif // _KVI_TAL_GROUPBOX_H_
