//=============================================================================
//
//   File : OptionsWidget_ctcpFloodProtection.cpp
//   Creation date : Sun Jan 21 2001 14:35:06 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidget_ctcpFloodProtection.h"

#include "KviOptions.h"
#include "KviLocale.h"

#include <QLayout>
#include <QLabel>

/*
	@doc: ctcpfloodprotectionoptions
	@type:
		generic
	@title:
		CTCP flood protection options
	@short:
		Protection against CTCP flooding
	@keyterms:
		ctcp flood
	@body:
		Here, you can configure the CTCP flood protection system.[br]
		The CTCP engine in IRC clients may be subject to an attack, called "CTCP flooding".[br]
		Basically, the attackers (usually there is more than one offending client) sends a stream
		of CTCP requests to the victim's client, and the client attempts to respond to all requests
		by sending a stream of replies to the server, which causes the server to disconnect the user
		due to excess flood for sending too many messages before the server can process them.[br]
		Many user defined scripts choose to completely ignore all the CTCP requests in order to prevent such attacks.[br]
		KVIrc offers an alternative method that allows you to take advantage of the CTCP protocol
		and at the same time prevent the flood attacks.[br]
		The method is really simple: the client will only respond to a limited number of CTCP requests within a
		specified time interval.[br]
		The "Use flood protection" option enables this protection method.[br]
		In the "Allowed up to" field you should enter the maximum number of CTCP requests that KVIrc should
		respond to within the interval of time defined in the second field.[br]
		A strong protection against floods would be replying to 4 requests every 20 seconds.[br]
		The minimum effective setting is about 3 messages every 6 seconds.[br]
		Please note that 3 messages every 6 seconds is [b]not[/b] the same as 10 messages every 20 seconds;
		the first setting would allow 3 requests within one second, but the client would
		ignore all the requests for the next 5 seconds, the second setting would allow 10 messages within one second
		and ignore everything for the next 19 seconds. Therefore, 3/6 is a safer setting.[br]
		The maximum protection you can set is 0 messages within any number of seconds; in that case any
		CTCP request will be considered a flood and ignored.[br]
		[br][br]
		You can also decide to ignore specific CTCP requests. For example, if you don't want to reply to PING
		requests, just select "PING" under the "Ignored Requests" section.[br]
		[br][br]
		Some specific protection options can be set also in other CTCP options sections:
		see the [widget:ctcpavatarprotooptions]CTCP AVATAR protocol options[/widget] and [widget:ctcpdccprotooptions]CTCP DCC protocol options[/widget].[br]
*/

OptionsWidget_ctcpFloodProtection::OptionsWidget_ctcpFloodProtection(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("ctcpfloodprotection_options_widget");

	createLayout();

	KviBoolSelector * b = addBoolSelector(0, 0, 1, 0, __tr2qs_ctx("Use flood protection for CTCP requests (recommended)", "options"), KviOption_boolUseCtcpFloodProtection);
	mergeTip(b, __tr2qs_ctx("This option makes KVIrc only respond to a limited number of CTCP requests within a specified time interval, to prevent \"flooding\" CTCP messages.", "options"));

	KviUIntSelector * u = addUIntSelector(0, 1, 0, 1, __tr2qs_ctx("Allow up to:", "options"), KviOption_uintMaxCtcpRequests, 0, 10000, 3, KVI_OPTION_BOOL(KviOption_boolUseCtcpFloodProtection));
	u->setSuffix(__tr2qs_ctx(" requests", "options"));
	mergeTip(u, __tr2qs_ctx("Minimum value: <b>0 requests</b><br>Maximum value: <b>10000 requests</b>", "options"));
	connect(b, SIGNAL(toggled(bool)), u, SLOT(setEnabled(bool)));

	u = addUIntSelector(1, 1, 1, 1, __tr2qs_ctx("within:", "options"), KviOption_uintCtcpFloodCheckInterval, 1, 3600, 6, KVI_OPTION_BOOL(KviOption_boolUseCtcpFloodProtection));
	u->setSuffix(__tr2qs_ctx(" sec", "options"));
	mergeTip(u, __tr2qs_ctx("Minimum value: <b>1 sec</b><br>Maximum value: <b>3600 sec</b>", "options"));
	connect(b, SIGNAL(toggled(bool)), u, SLOT(setEnabled(bool)));

	KviTalGroupBox * g = addGroupBox(0, 2, 1, 2, Qt::Horizontal, __tr2qs_ctx("Ignored CTCP Requests", "options"));
	addBoolSelector(g, __tr2qs_ctx("PING", "options"), KviOption_boolIgnoreCtcpPing);
	addBoolSelector(g, __tr2qs_ctx("FINGER", "options"), KviOption_boolIgnoreCtcpFinger);
	addBoolSelector(g, __tr2qs_ctx("CLIENTINFO", "options"), KviOption_boolIgnoreCtcpClientinfo);
	addBoolSelector(g, __tr2qs_ctx("USERINFO", "options"), KviOption_boolIgnoreCtcpUserinfo);
	addBoolSelector(g, __tr2qs_ctx("VERSION", "options"), KviOption_boolIgnoreCtcpVersion);
	addBoolSelector(g, __tr2qs_ctx("SOURCE", "options"), KviOption_boolIgnoreCtcpSource);
	addBoolSelector(g, __tr2qs_ctx("TIME", "options"), KviOption_boolIgnoreCtcpTime);
	addBoolSelector(g, __tr2qs_ctx("PAGE", "options"), KviOption_boolIgnoreCtcpPage);
	addBoolSelector(g, __tr2qs_ctx("AVATAR", "options"), KviOption_boolIgnoreCtcpAvatar);
	addBoolSelector(g, __tr2qs_ctx("DCC/TDCC", "options"), KviOption_boolIgnoreCtcpDcc);

	addRowSpacer(0, 3, 1, 3);
}

OptionsWidget_ctcpFloodProtection::~OptionsWidget_ctcpFloodProtection()
    = default;
