#ifndef _IDLE_H_
#define _IDLE_H_
/*
 * Idle.h - detect desktop idle time
 * Copyright (C) 2003  Justin Karneges
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "kvi_settings.h"

#include <QObject>
#include <QCursor>
#include <QDateTime>
#include <QTimer>

class IdlePlatform;

class Idle : public QObject
{
	Q_OBJECT
public:
	Idle();
	~Idle();

	bool isActive() const;
	bool usingPlatform() const;
	void start();
	void stop();
	int secondsIdle();

signals:
	void secondsIdle(int);

	private slots:
	void doCheck();

private:
	class Private;
	Private * d;
};

class IdlePlatform
{
public:
	IdlePlatform();
	~IdlePlatform();

	bool init() const;
	int secondsIdle() const;

private:
	class Private;
	Private * d;
};

class Idle::Private
{
public:
	Private() {}

	QPoint lastMousePos;
	QDateTime idleSince;

	bool active;
	int idleTime;
	QDateTime startTime;
	QTimer checkTimer;
};

#endif
