#ifndef _KVI_TAL_GROUPBOX_H_
#define _KVI_TAL_GROUPBOX_H_

//=============================================================================
//
//   File : kvi_tal_groupbox.h
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

/**
* \file kvi_tal_groupbox.h
* \author Szymon Stefanek
* \brief Class for groupbox
*/

#include "kvi_settings.h"

#include <QGroupBox>
#include <QLayout>

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
	KviTalGroupBox(QWidget * pParent, char * pcName=0);

	/**
	* \brief Constructs the groupbox object
	* \param pParent The parent object
	* \return KviTalGroupBox
	*/
	KviTalGroupBox(QWidget * pParent = 0);

	/**
	* \brief Constructs the groupbox object
	* \param szTitle The title of the groupbox
	* \param pParent The parent object
	* \return KviTalGroupBox
	*/
	KviTalGroupBox(const QString & szTitle, QWidget * pParent = 0);

	/**
	* \brief Constructs the groupbox object
	* \param orientation The orientation of the groupbox
	* \param pParent The parent object
	* \return KviTalGroupBox
	*/
	KviTalGroupBox(Qt::Orientation orientation, QWidget * parent = 0);

	/**
	* \brief Constructs the groupbox object
	* \param orientation The orientation of the groupbox
	* \param szTitle The title of the groupbox
	* \param pParent The parent object
	* \return KviTalGroupBox
	*/
	KviTalGroupBox(Qt::Orientation orientation, const QString & szTitle, QWidget * parent = 0);

	/**
	* \brief Destroys the groupbox object
	*/
	~KviTalGroupBox();
protected:
	Qt::Orientation mOrientation;
private:
	QLayout * m_pLayout;
public:
	/**
	* \brief Sets the inside margin
	* \param iMargin The margin in pixels
	* \return void
	*/
	void setInsideMargin(int iMargin)
		{ if(m_pLayout) m_pLayout->setMargin(iMargin); };

	/**
	* \brief Sets the inside spacing
	* \param iSpacing The spacing in pixels
	* \return void
	*/
	void setInsideSpacing(int iSpacing)
		{ if(m_pLayout) m_pLayout->setSpacing(iSpacing); };

	/**
	* \brief Returns the inside margin
	* \return int
	*/
	int insideMargin()
		{ if(m_pLayout) return m_pLayout->margin(); return 0; };

	/**
	* \brief Returns the inside spacing
	* \return int
	*/
	int insideSpacing()
		{ if(m_pLayout) return m_pLayout->spacing(); return 0; };

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
protected:
	virtual void childEvent(QChildEvent * e);
};

#endif // _KVI_TAL_GROUPBOX_H_
