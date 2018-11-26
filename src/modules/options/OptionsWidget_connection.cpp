//=============================================================================
//
//   File : OptionsWidget_connection.cpp
//   Creation date : Sat Nov 24 04:25:16 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
//   copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#include "OptionsWidget_connection.h"

#include "kvi_settings.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "KviIconManager.h"

#include <QRadioButton>

OptionsWidget_connection::OptionsWidget_connection(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("connection_options_widget");
	createLayout();

	KviBoolSelector * b1;
	KviBoolSelector * b;

	KviTalGroupBox * gbox = addGroupBox(0, 0, 0, 0, Qt::Horizontal, __tr2qs_ctx("On Disconnect", "options"));

	b1 = addBoolSelector(gbox, __tr2qs_ctx("Keep channels open", "options"), KviOption_boolKeepChannelsOpenOnDisconnect, true);
	mergeTip(b1, __tr2qs_ctx("This option will cause KVIrc to keep channels open after disconnect.", "options"));

	b1 = addBoolSelector(gbox, __tr2qs_ctx("Keep queries open", "options"), KviOption_boolKeepQueriesOpenOnDisconnect, true);
	mergeTip(b1, __tr2qs_ctx("This option will cause KVIrc to keep queries open after disconnect.", "options"));

	gbox = addGroupBox(0, 1, 0, 1, Qt::Horizontal, __tr2qs_ctx("On Unexpected Disconnect", "options"));

	b1 = addBoolSelector(gbox, __tr2qs_ctx("Keep channels open", "options"), KviOption_boolKeepChannelsOpenOnUnexpectedDisconnect, true);
	mergeTip(b1, __tr2qs_ctx("This option will cause KVIrc to keep channels open after an unexpected disconnect.", "options"));

	b1 = addBoolSelector(gbox, __tr2qs_ctx("Keep queries open", "options"), KviOption_boolKeepQueriesOpenOnUnexpectedDisconnect, true);
	mergeTip(b1, __tr2qs_ctx("This option will cause KVIrc to keep queries open after an unexpected disconnect.", "options"));

	b1 = addBoolSelector(gbox, __tr2qs_ctx("Rejoin channels after reconnect", "options"), KviOption_boolRejoinChannelsAfterReconnect, true);
	mergeTip(b1, __tr2qs_ctx("This option will cause KVIrc to rejoin channels after a successful reconnect attempt.", "options"));

	b1 = addBoolSelector(gbox, __tr2qs_ctx("Reopen queries after reconnect", "options"), KviOption_boolReopenQueriesAfterReconnect, true);
	mergeTip(b1, __tr2qs_ctx("This option will cause KVIrc to reopen query windows after a successful reconnect attempt.", "options"));

	b = addBoolSelector(gbox, __tr2qs_ctx("Automatically reconnect", "options"), KviOption_boolAutoReconnectOnUnexpectedDisconnect);
	mergeTip(b, __tr2qs_ctx("This option will enable auto-reconnecting after an unexpected disconnect. "
	                        "An unexpected disconnect is the <b>termination</b> of a <b>fully connected IRC session</b> "
	                        "that was <b>not requested by the user</b> by the means of the QUIT message."
	                        "<p><b>Warning:</b> If you use /RAW to send a QUIT message to the server, "
	                        "this option will not behave correctly, since does not detect the outgoing "
	                        "QUIT message and will attempt to reconnect after the server has closed the connection. "
	                        "For this reason, always use the /QUIT command to close your connections. "
	                        "This option may also behave incorrectly with bouncers that support "
	                        "detaching, in this case a solution could be to prepare an alias that sends the "
	                        "bouncer \"detach\" command immediately before the \"quit\" command.<br>"
	                        "<tt>alias(bncdetach){ raw bouncer detach; quit; }</tt></p>",
	                "options"));

	KviUIntSelector * u = addUIntSelector(gbox, __tr2qs_ctx("Maximum attempts (0 = unlimited):", "options"),
	    KviOption_uintMaxAutoReconnectAttempts, 0, 100, 5, KVI_OPTION_BOOL(KviOption_boolAutoReconnectOnUnexpectedDisconnect));
	connect(b, SIGNAL(toggled(bool)), u, SLOT(setEnabled(bool)));

	u = addUIntSelector(gbox, __tr2qs_ctx("Delay between attempts:", "options"), KviOption_uintAutoReconnectDelay, 0, 86400, 5,
	    KVI_OPTION_BOOL(KviOption_boolAutoReconnectOnUnexpectedDisconnect));
	u->setSuffix(__tr2qs_ctx(" sec", "options"));
	connect(b, SIGNAL(toggled(bool)), u, SLOT(setEnabled(bool)));

	mergeTip(u, __tr2qs_ctx("Minimum value: <b>0 sec</b><br>Maximum value: <b>86400 sec</b>", "options"));

	addRowSpacer(0, 3, 4, 3);
}

OptionsWidget_connection::~OptionsWidget_connection()
    = default;

OptionsWidget_connectionSsl::OptionsWidget_connectionSsl(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("ssl_options_widget");
	createLayout();

#ifdef COMPILE_SSL_SUPPORT

	KviBoolSelector * b;
	KviFileSelector * f;
	KviPasswordSelector * p;

	KviTalGroupBox * gbox = addGroupBox(0, 0, 0, 0, Qt::Horizontal, __tr2qs_ctx("Certificate", "options"));

	b = addBoolSelector(gbox, __tr2qs_ctx("Use SSL certificate (PEM format only)", "options"),
	    &(KVI_OPTION_BOOL(KviOption_boolUseSSLCertificate)), true);

	f = addFileSelector(gbox, __tr2qs_ctx("Certificate location:", "options"), &(KVI_OPTION_STRING(KviOption_stringSSLCertificatePath)),
	    KVI_OPTION_BOOL(KviOption_boolUseSSLCertificate));
	connect(b, SIGNAL(toggled(bool)), f, SLOT(setEnabled(bool)));

	p = new KviPasswordSelector(gbox, __tr2qs_ctx("Certificate password:", "options"), &(KVI_OPTION_STRING(KviOption_stringSSLCertificatePass)),
	    KVI_OPTION_BOOL(KviOption_boolUseSSLCertificate));
	connect(b, SIGNAL(toggled(bool)), p, SLOT(setEnabled(bool)));

	gbox = addGroupBox(0, 1, 0, 1, Qt::Horizontal, __tr2qs_ctx("Private Key", "options"));

	b = addBoolSelector(gbox, __tr2qs_ctx("Use SSL private key", "options"), &(KVI_OPTION_BOOL(KviOption_boolUseSSLPrivateKey)), true);
	f = addFileSelector(gbox, __tr2qs_ctx("Private key location:", "options"), &(KVI_OPTION_STRING(KviOption_stringSSLPrivateKeyPath)),
	    KVI_OPTION_BOOL(KviOption_boolUseSSLPrivateKey));
	connect(b, SIGNAL(toggled(bool)), f, SLOT(setEnabled(bool)));

	p = addPasswordSelector(gbox, __tr2qs_ctx("Private key password:", "options"), &(KVI_OPTION_STRING(KviOption_stringSSLPrivateKeyPass)),
	    KVI_OPTION_BOOL(KviOption_boolUseSSLPrivateKey));
	connect(b, SIGNAL(toggled(bool)), p, SLOT(setEnabled(bool)));

	addRowSpacer(0, 3, 0, 3);
#else
	addLabel(0, 0, 0, 0, __tr2qs_ctx("This executable has no SSL support.", "options"));
#endif
}

OptionsWidget_connectionSsl::~OptionsWidget_connectionSsl()
    = default;

OptionsWidget_connectionSocket::OptionsWidget_connectionSocket(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("transport_options_widget");
	createLayout();

	KviUIntSelector * u;
	KviTalGroupBox * g;
	KviBoolSelector * b;
	KviStringSelector * s;

	g = addGroupBox(0, 0, 0, 0, Qt::Horizontal, __tr2qs_ctx("Timeout Values", "options"));
	u = addUIntSelector(g, __tr2qs_ctx("Connect timeout:", "options"), KviOption_uintIrcSocketTimeout, 5, 6000, 60);
	u->setSuffix(__tr2qs_ctx(" sec", "options"));
	u = addUIntSelector(g, __tr2qs_ctx("Outgoing data queue flush timeout:", "options"), KviOption_uintSocketQueueFlushTimeout, 100, 2000, 500);
	u->setSuffix(__tr2qs_ctx(" msec", "options"));
	b = addBoolSelector(0, 1, 0, 1, __tr2qs_ctx("Limit outgoing traffic per connection", "options"), KviOption_boolLimitOutgoingTraffic);
	u = addUIntSelector(0, 2, 0, 2, __tr2qs_ctx("Limit to 1 message every:", "options"),
	    KviOption_uintOutgoingTrafficLimitUSeconds, 10000, 2000000, 10000001, KVI_OPTION_BOOL(KviOption_boolLimitOutgoingTraffic));
	u->setSuffix(__tr2qs_ctx(" usec", "options"));
	mergeTip(u, __tr2qs_ctx("Minimum value: <b>10000 usec</b><br>Maximum value: <b>10000000 usec</b>", "options"));
	connect(b, SIGNAL(toggled(bool)), u, SLOT(setEnabled(bool)));

	g = addGroupBox(0, 3, 0, 3, Qt::Horizontal, __tr2qs_ctx("Network Interfaces", "options"));

	b = addBoolSelector(g, __tr2qs_ctx("Bind IPv4 connections to:", "options"), KviOption_boolBindIrcIPv4ConnectionsToSpecifiedAddress);
	s = addStringSelector(g, "", KviOption_stringIPv4ConnectionBindAddress, KVI_OPTION_BOOL(KviOption_boolBindIrcIPv4ConnectionsToSpecifiedAddress));
	connect(b, SIGNAL(toggled(bool)), s, SLOT(setEnabled(bool)));
#ifdef COMPILE_IPV6_SUPPORT
	b = addBoolSelector(g, __tr2qs_ctx("Bind IPv6 connections to:", "options"), KviOption_boolBindIrcIPv6ConnectionsToSpecifiedAddress);
	s = addStringSelector(g, "", KviOption_stringIPv6ConnectionBindAddress, KVI_OPTION_BOOL(KviOption_boolBindIrcIPv6ConnectionsToSpecifiedAddress));
	connect(b, SIGNAL(toggled(bool)), s, SLOT(setEnabled(bool)));
#endif //!COMPILE_IPV6_SUPPORT

	b = addBoolSelector(0, 4, 0, 4, __tr2qs_ctx("Pick random IP address for round-robin servers", "options"), KviOption_boolPickRandomIpAddressForRoundRobinServers);
	mergeTip(b, __tr2qs_ctx("This option will cause the KVIrc networking stack to pick up "
	                        "a random entry when multiple IP address are retrieved for a server "
	                        "DNS lookup. This is harmless and can fix some problems with caching "
	                        "DNS servers that do not properly rotate the records as the authoritative "
	                        "ones would do. On the other hand, you might want to disable it if "
	                        "you want to rely on the DNS server to provide the best choice.",
	                "options"));

	b = addBoolSelector(0, 5, 0, 5, __tr2qs_ctx("Drop connection on SASL authentication failure", "options"), KviOption_boolDropConnectionOnSaslFailure);
	mergeTip(b, __tr2qs_ctx("This option will close the socket if no SASL authentication or any SASL fallback had succeeded.", "options"));

	addRowSpacer(0, 6, 0, 6);
}

OptionsWidget_connectionSocket::~OptionsWidget_connectionSocket()
    = default;

OptionsWidget_identService::OptionsWidget_identService(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("ident_options_widget");
	createLayout();

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	m_pEnableIdent = addBoolSelector(0, 0, 0, 0, __tr2qs_ctx("Enable Ident service", "options"), KviOption_boolUseIdentService);
#else
	m_pEnableIdent = addBoolSelector(0, 0, 0, 0, __tr2qs_ctx("Enable Ident service (bad practice on UNIX!)", "options"), KviOption_boolUseIdentService);
#endif
	connect(m_pEnableIdent, SIGNAL(toggled(bool)), this, SLOT(enableIpv4InIpv6(bool)));

	KviTalGroupBox * gbox = addGroupBox(0, 1, 0, 1, Qt::Horizontal, __tr2qs_ctx("Output Verbosity", "options"), KVI_OPTION_BOOL(KviOption_boolUseIdentService));
	connect(m_pEnableIdent, SIGNAL(toggled(bool)), gbox, SLOT(setEnabled(bool)));

	addLabel(gbox, __tr2qs_ctx("Output Ident service messages to:", "options"));

	m_pActiveRadio = new QRadioButton(__tr2qs_ctx("Active window", "options"), gbox);
	m_pConsoleRadio = new QRadioButton(__tr2qs_ctx("Console", "options"), gbox);
	m_pQuietRadio = new QRadioButton(__tr2qs_ctx("Do not show any Ident service messages", "options"), gbox);

	switch(KVI_OPTION_UINT(KviOption_uintIdentdOutputMode))
	{
		case KviIdentdOutputMode::Quiet:
			m_pQuietRadio->setChecked(true);
			break;
		case KviIdentdOutputMode::ToConsole:
			m_pConsoleRadio->setChecked(true);
			break;
		case KviIdentdOutputMode::ToActiveWindow:
			m_pActiveRadio->setChecked(true);
			break;
	}

	KviBoolSelector * b;
	KviStringSelector * s;
	KviUIntSelector * u;

	gbox = addGroupBox(0, 2, 0, 2, Qt::Horizontal, __tr2qs_ctx("Configuration", "options"), KVI_OPTION_BOOL(KviOption_boolUseIdentService));

	b = addBoolSelector(gbox, __tr2qs_ctx("Enable Ident service only while connecting to server", "options"), KviOption_boolUseIdentServiceOnlyOnConnect);
	connect(m_pEnableIdent, SIGNAL(toggled(bool)), b, SLOT(setEnabled(bool)));

	s = addStringSelector(gbox, __tr2qs_ctx("Ident username:", "options"), KviOption_stringIdentdUser, KVI_OPTION_BOOL(KviOption_boolUseIdentService));
	connect(m_pEnableIdent, SIGNAL(toggled(bool)), s, SLOT(setEnabled(bool)));

	u = addUIntSelector(gbox, __tr2qs_ctx("Service port:", "options"), KviOption_uintIdentdPort, 0, 65535, 113, KVI_OPTION_BOOL(KviOption_boolUseIdentService));
	connect(m_pEnableIdent, SIGNAL(toggled(bool)), u, SLOT(setEnabled(bool)));
	connect(m_pEnableIdent, SIGNAL(toggled(bool)), gbox, SLOT(setEnabled(bool)));

	gbox = addGroupBox(0, 3, 0, 3, Qt::Horizontal, __tr2qs_ctx("IPv6 Settings", "options"), KVI_OPTION_BOOL(KviOption_boolUseIdentService));

	m_pEnableIpv6 = addBoolSelector(gbox, __tr2qs_ctx("Enable service for IPv6", "options"), KviOption_boolIdentdEnableIPv6,
	    KVI_OPTION_BOOL(KviOption_boolUseIdentService));
#ifdef COMPILE_IPV6_SUPPORT
	connect(m_pEnableIdent, SIGNAL(toggled(bool)), m_pEnableIpv6, SLOT(setEnabled(bool)));
	connect(m_pEnableIpv6, SIGNAL(toggled(bool)), this, SLOT(enableIpv4InIpv6(bool)));
#else
	m_pEnableIpv6->setEnabled(false);
#endif
	m_pIpv4InIpv6 = addBoolSelector(gbox, __tr2qs_ctx("IP stack treats IPv4 as part of IPv6 namespace", "options"), KviOption_boolIdentdIPv6ContainsIPv4,
	    KVI_OPTION_BOOL(KviOption_boolUseIdentService) && KVI_OPTION_BOOL(KviOption_boolIdentdEnableIPv6));
	connect(m_pEnableIdent, SIGNAL(toggled(bool)), gbox, SLOT(setEnabled(bool)));

	addLabel(0, 4, 0, 4,
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	    __tr2qs_ctx("<p><b>Warning:</b><br>"
	                "This is a <b>non RFC 1413 compliant</b> Ident daemon that implements "
	                "only a limited subset of the Identification Protocol specifications. If it is possible, install a "
	                "real Ident daemon.</p>",
	             "options")
#else
	    __tr2qs_ctx("<p><b>Warning:</b><br>"
	                "This is a <b>non RFC 1413 compliant</b> Ident daemon that implements "
	                "only a limited subset of the <b>Identification Protocol</b> specifications.<br>"
	                "On UNIX, you may also need root privileges to bind to the auth port (113).<br>"
	                "It is <b>highly recommended</b> that a <b>real</b> system-wide Ident daemon be used instead, "
	                "or none at all if Ident is not required.</p>",
	             "options")
#endif
	        );

	addRowSpacer(0, 5, 0, 5);
}

OptionsWidget_identService::~OptionsWidget_identService()
    = default;

void OptionsWidget_identService::commit()
{
	KviOptionsWidget::commit();
	if(m_pConsoleRadio->isChecked())
		KVI_OPTION_UINT(KviOption_uintIdentdOutputMode) = KviIdentdOutputMode::ToConsole;
	if(m_pActiveRadio->isChecked())
		KVI_OPTION_UINT(KviOption_uintIdentdOutputMode) = KviIdentdOutputMode::ToActiveWindow;
	if(m_pQuietRadio->isChecked())
		KVI_OPTION_UINT(KviOption_uintIdentdOutputMode) = KviIdentdOutputMode::Quiet;
}

void OptionsWidget_identService::enableIpv4InIpv6(bool)
{
#ifdef COMPILE_IPV6_SUPPORT
	m_pIpv4InIpv6->setEnabled(m_pEnableIdent->isChecked() && m_pEnableIpv6->isChecked());
#endif
}

OptionsWidget_connectionAdvanced::OptionsWidget_connectionAdvanced(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("connection_advanced_options_widget");
}

OptionsWidget_connectionAdvanced::~OptionsWidget_connectionAdvanced()
    = default;
