#ifndef _CLASS_TRAYICON_H_
#define _CLASS_TRAYICON_H_
//=============================================================================
//
//   File : KvsObject_trayIcon.h
//   Creation date : Wed Ago 25 17:28:45 2010 GMT by Carbone Alesssandro
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

#include "object_macros.h"
#include <QSystemTrayIcon>
#include <QMenu>

class KvsObject_trayIcon : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_trayIcon)
protected:
	QSystemTrayIcon * m_pTrayIcon;

protected:
	bool setIcon(KviKvsObjectFunctionCall * c);
	bool show(KviKvsObjectFunctionCall * c);
	bool hide(KviKvsObjectFunctionCall * c);
	bool isVisible(KviKvsObjectFunctionCall * c);
	bool setTooltip(KviKvsObjectFunctionCall * c);
	bool showMessage(KviKvsObjectFunctionCall * c);
	bool setContextMenu(KviKvsObjectFunctionCall * c);
	bool activatedEvent(KviKvsObjectFunctionCall * c);
	bool messageClickedEvent(KviKvsObjectFunctionCall * c);

protected slots:
	void slotActivated(QSystemTrayIcon::ActivationReason);
	void slotMessageClicked();
};
#endif //_CLASS_TRAYICON_H_
