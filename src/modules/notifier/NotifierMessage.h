#ifndef _NOTIFIERMESSAGE_H_
#define _NOTIFIERMESSAGE_H_
//=============================================================================
//
//   File : NotifierMessage.h
//   Creation date : Mar 02 Nov 2004 02:41:18 by Iacopo Palazzi
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2004-2008 Iacopo Palazzi < iakko(at)siena(dot)linux(dot)it >
//   Copyright (C) 2009 Fabio Bas < ctrlaltca at gmail dot com >
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

#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QString>
#include <QWidget>

/**
* \class NotifierMessage
* \brief A single message in a notifier window
*
* This is basically a single message that can appear in a notifier window
* It's made up of an icon and a rich text content
* This class is basically a QLabel with a method that translates a raw irc message to html code.
*/
class NotifierMessage : public QWidget
{
	friend class NotifierWindow;

	Q_OBJECT
public:
	/**
	* \brief Constructs the NotifierMessage object
	* \param pPixmap pointer to a message icon, can be null
	* \param szText const reference to message text in irc format
	* \return NotifierMessage
	*/
	NotifierMessage(QPixmap * pPixmap, QString szText);
	/**
	* \brief Destroys the NotifierMessage object
	*/
	~NotifierMessage();

private:
	/// The message text
	QString m_szText;
	/// The message icon (can be null)
	QPixmap * m_pPixmap = nullptr;
	/// Layout for the labels
	QHBoxLayout * m_pHBox = nullptr;
	/// Label for the message icon
	QLabel * m_pLabel0 = nullptr;
	/// Label for the message text
	QLabel * m_pLabel1 = nullptr;

public:
	/**
	* \brief Returns the original irc message
	* \return const QString &
	*/
	const QString & text() const { return m_szText; }
	/**
	* \brief Returns the message icon
	* \return QPixmap *
	*/
	QPixmap * pixmap() const { return m_pPixmap; }
	/**
	* \brief Updates the aspect of this message
	* \return void
	*/
	void updateGui();
};

#endif //!_NOTIFIERMESSAGE_H_
