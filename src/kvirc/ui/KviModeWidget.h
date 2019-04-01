#ifndef _KVI_MODEWIDGET_H_
#define _KVI_MODEWIDGET_H_
//============================================================================
//
//   File : KviModeWidget.h
//   Creation date : Sat Nov 12 2005 23:50:12 by Alexey Uzhva
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alexey Uzhva (wizard at opendoor dot ru)
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
//============================================================================

#include "KviThemedLineEdit.h"

#include <map>

class KviChannelWindow;
class KviIrcConnectionServerInfo;

class KVIRC_API KviModeWidget : public KviThemedLineEdit
{
	Q_OBJECT
public:
	KviModeWidget(QWidget * par, KviChannelWindow & chan, const char * name = nullptr);
	~KviModeWidget();

	void reset();
	void refreshModes();

private:
	KviChannelWindow & m_Channel;

	std::map<QChar, QString> parseChannelModeString(const QString& szModes);
	void sendModeChanges(const QString szModeString, const QStringList params);

protected:
	void mouseDoubleClickEvent(QMouseEvent * e) override;
	void keyReleaseEvent(QKeyEvent * e) override;

	bool isParameterOnlyNeededWhenModeIsSet(const QChar & cMode);

public slots:
	void processModeChanges();

signals:
	void setMode(const QString & szMode);
};

#endif //_KVI_MODEWIDGET_H_
