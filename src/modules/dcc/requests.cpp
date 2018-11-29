//=============================================================================
//
//   File : requests.cpp
//   Creation date : Tue Jul 23 02:44:38 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_DEBUG_CHECK_RANGE_

#include "DccVoiceGsmCodec.h"
#include "DccBroker.h"

#ifndef COMPILE_DISABLE_DCC_VIDEO
#include "DccVideoWindow.h"
#endif

#include "DccVoiceWindow.h"
#include "DccUtils.h"
#include "DccFileTransfer.h"

#include "kvi_debug.h"
#include "kvi_settings.h"
#include "KviCString.h"
#include "KviModule.h"
#include "KviIrcServerParser.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviConsoleWindow.h"
#include "KviNetUtils.h"
#include "KviMainWindow.h"
#include "KviConsoleWindow.h"
#include "KviError.h"
#include "KviOptions.h"
#include "kvi_defaults.h"
#include "KviSharedFilesManager.h"
#include "KviControlCodes.h"
#include "KviApplication.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcMessage.h"

#include <QFileInfo>

#ifdef COMPILE_ON_WINDOWS
// Ugly Windoze compiler...
#include "DccDialog.h"
#endif

//#warning "KviOption_boolIgnoreDccChat and other types too"

extern KVIRC_API KviSharedFilesManager * g_pSharedFilesManager;
extern DccBroker * g_pDccBroker;

static void dcc_module_reply_errmsg(KviDccRequest * dcc, const QString & errText)
{
	dcc->ctcpMsg->msg->console()->connection()->sendFmtData(
	    "NOTICE %s :%cERRMSG %s%c",
	    dcc->ctcpMsg->msg->console()->connection()->encodeText(dcc->ctcpMsg->pSource->nick()).data(), 0x01,
	    dcc->ctcpMsg->msg->console()->connection()->encodeText(errText).data(), 0x01);
}

static void dcc_module_request_error(KviDccRequest * dcc, const QString & errText)
{
	dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCERROR,
	    __tr2qs_ctx("Unable to process the above request: %Q, %Q", "dcc"),
	    &errText,
	    KVI_OPTION_BOOL(KviOption_boolNotifyFailedDccHandshakes) ? &(__tr2qs_ctx("Ignoring and notifying failure", "dcc")) : &(__tr2qs_ctx("Ignoring", "dcc")));

	if(KVI_OPTION_BOOL(KviOption_boolNotifyFailedDccHandshakes))
	{
		QString szError = QString("Sorry, your DCC %1 request can't be satisfied: %2").arg(dcc->szType.ptr(), errText);
		//since szError contains a user-suppplied string, we simplify it to avoid any kind of injection (bug #858)
		dcc_module_reply_errmsg(dcc, szError.simplified());
	}
}

static bool dcc_module_check_concurrent_transfers_limit(KviDccRequest * dcc)
{
	if(KVI_OPTION_UINT(KviOption_uintMaxDccSendTransfers) > 0)
	{
		unsigned int uTransfers = DccFileTransfer::runningTransfersCount();
		if(uTransfers >= KVI_OPTION_UINT(KviOption_uintMaxDccSendTransfers))
		{
			QString szError = QString(__tr2qs_ctx("Concurrent transfer limit reached (%1 of %2 transfers running)", "dcc")).arg(uTransfers).arg(KVI_OPTION_UINT(KviOption_uintMaxDccSendTransfers));
			dcc_module_request_error(dcc, szError);
			return false;
		}
	}
	return true;
}

static bool dcc_module_check_limits(KviDccRequest * dcc)
{
	if(KVI_OPTION_UINT(KviOption_uintMaxDccSlots) > 0)
	{
		unsigned int uWindows = g_pDccBroker->dccWindowsCount();
		if(uWindows >= KVI_OPTION_UINT(KviOption_uintMaxDccSlots))
		{
			QString szError = QString(__tr2qs_ctx("Slot limit reached (%1 slots of %2)", "dcc")).arg(uWindows).arg(KVI_OPTION_UINT(KviOption_uintMaxDccSlots));
			dcc_module_request_error(dcc, szError);
			return false;
		}
	}
	if(g_pDccBroker->dccBoxCount() >= 32)
	{
		// there are too many pending dcc requests: the user isn't watching....
		dcc_module_request_error(dcc, __tr2qs_ctx("Too many pending connections", "dcc"));
		return false;
	}
	return true;
}

static void dcc_fill_local_nick_user_host(DccDescriptor * d, KviDccRequest * dcc)
{
	if(dcc->pConsole->connection())
	{
		d->szLocalNick = dcc->pConsole->connection()->userInfo()->nickName();
		d->szLocalUser = dcc->pConsole->connection()->userInfo()->userName();
		d->szLocalHost = dcc->pConsole->connection()->userInfo()->hostName();
	}
	else
	{
		d->szLocalNick = __tr_ctx("unknown", "dcc");
		d->szLocalUser = __tr2qs_ctx("unknown", "dcc");
		d->szLocalHost = __tr2qs_ctx("unknown", "dcc");
	}
}

static void dcc_module_set_dcc_type(DccDescriptor * d, const char * szBaseType)
{
	d->szType = szBaseType;
#ifdef COMPILE_SSL_SUPPORT
	if(d->bIsSSL)
		d->szType.prepend('S');
#endif
	if(d->bIsTdcc)
		d->szType.prepend('T');
}

static bool dcc_module_normalize_target_data(KviDccRequest * dcc, KviCString & ipaddr, KviCString & port)
{
	if(!port.isUnsignedNum())
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			QString szError = QString(__tr2qs_ctx("Invalid port number %1", "dcc")).arg(port.ptr());
			dcc_module_request_error(dcc, szError);
		}
		return false;
	}

	struct in_addr addr;

	if(ipaddr.isUnsignedNum())
	{
		addr.s_addr = htonl((unsigned long)ipaddr.toULong());
		QString tmp;
		if(!KviNetUtils::binaryIpToStringIp(addr, tmp))
		{
			if(!dcc->ctcpMsg->msg->haltOutput())
			{
				QString szError = QString(__tr2qs_ctx("Invalid IP address in old format %1", "dcc")).arg(ipaddr.ptr());
				dcc_module_request_error(dcc, szError);
			}
			return false;
		}
		ipaddr = tmp;
	}
	else
	{
		//FIXME: KviCString -> QString
		if(!KviNetUtils::stringIpToBinaryIp(QString(ipaddr), &addr))
		{
#ifdef COMPILE_IPV6_SUPPORT
			struct in6_addr addr6;
			if(KviNetUtils::stringIpToBinaryIp_V6(QString(ipaddr), &addr6))
			{
				dcc->bIPv6 = true;
				return true; // IPV6 address.
			}
#endif
			if(!dcc->ctcpMsg->msg->haltOutput())
			{
				QString szError = QString(__tr2qs_ctx("Invalid IP address %1", "dcc")).arg(ipaddr.ptr());
				dcc_module_request_error(dcc, szError);
			}
			return false;
		}
	}
	return true;
}

//
// CHAT
//

static void dccModuleParseDccChat(KviDccRequest * dcc)
{
	//
	// We have received a DCC CHAT request in the following form:
	//
	//     DCC CHAT chat <ipaddress> <port>
	//
	// This means that we're requested to setup an ACTIVE chat connection
	// ... Easy task :)
	//
	// Anybody understands the meaning of the secondo "chat" in there ?
	// It was meant to simplify the parsing ? :DDD
	//
	// There is a mIrc extension that allows <port> to be 0
	// and adds a last parameter that seems to be a random number (thnx YaP :)
	// that is used to keep track of the connection.
	// This extension is used by firewalled machines to initiate a DCC CHAT:
	// the receiving side should respond with a DCC CHAT offer
	// with the same random number appended, and then should listen for a connection.
	//
	// when a zero port request is initiated by another party we get
	//
	//      DCC CHAT chat <fakeipaddress> 0 <tag>
	//
	// and we reply with
	//
	//      DCC CHAT chat <ourip> <ourport> <tag>
	//
	// when a zero port request is initiated by us we send out
	//
	//      DCC CHAT chat <fakeipaddress> 0 <tag>
	//
	// and we get
	//
	//      DCC CHAT chat <remoteip> <remoteport> <tag>
	//
	// Thus if there is a <tag> and the port is 0, then the remote party
	// wanted to estabilish a dcc with us and wants us to listen, but if the port is nonzero then
	// we have sent out a zero port request and the remote party acked it
	// thus we have to connect instead!
	//

	// First of all we check the dcc slot limits
	if(!dcc_module_check_limits(dcc))
		return;

	// Then we check the target host data
	if(!dcc_module_normalize_target_data(dcc, dcc->szParam2, dcc->szParam3))
		return;

	if(!kvi_strEqualCI(dcc->szParam1.ptr(), "chat"))
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
			    __tr2qs_ctx("The above request is broken: the second parameter is '%s' and should be 'chat', trying to continue", "dcc"), dcc->szParam1.ptr());
		}
	}

	KviCString szExtensions = dcc->szType;
	szExtensions.cutRight(4); // cut off CHAT

#ifdef COMPILE_SSL_SUPPORT
	bool bSSLExtension = szExtensions.contains('S', false);
#else  //!COMPILE_SSL_SUPPORT
	if(szExtensions.contains('S', false))
	{
		dcc_module_request_error(dcc, __tr2qs_ctx("This executable has been compiled without SSL support, the SSL extension to DCC CHAT is not available", "dcc"));
		return;
	}
#endif //!COMPILE_SSL_SUPPORT

	DccDescriptor * d = new DccDescriptor(dcc->pConsole);

	d->szNick = dcc->ctcpMsg->pSource->nick();
	d->szUser = dcc->ctcpMsg->pSource->user();
	d->szHost = dcc->ctcpMsg->pSource->host();

	dcc_fill_local_nick_user_host(d, dcc);

	d->szIp = dcc->szParam2.ptr();
	d->szPort = dcc->szParam3.ptr();

	if(dcc->szParam4.hasData())
	{
		// zero port tag ?
		if(d->szPort == "0")
		{
			// zero port request
			if(KVI_OPTION_BOOL(KviOption_boolDccSendFakeAddressByDefault))
			{
				d->szFakeIp = KVI_OPTION_STRING(KviOption_stringDefaultDccFakeAddress);
				if(d->szFakeIp.isEmpty())
					KVI_OPTION_BOOL(KviOption_boolDccSendFakeAddressByDefault) = false;
			}
			d->setZeroPortRequestTag(dcc->szParam4.ptr());
			QString tmp;
			if(!dcc_kvs_get_listen_ip_address(nullptr, d->console(), tmp))
				d->szListenIp = "0.0.0.0";
			else
				d->szListenIp = tmp;
			d->szListenPort = "0"; // any port is OK
			d->bAutoAccept = KVI_OPTION_BOOL(KviOption_boolAutoAcceptDccChat);
			d->bActive = false; // we must listen then...
		}
		else
		{
			// zero port acknowledge
			// check if this is a tag that we have sent out
			QString szTag = QString(dcc->szParam4.ptr());
			KviDccZeroPortTag * t = g_pDccBroker->findZeroPortTag(szTag);
			if(!t)
			{
				// hum.. not our tag

				// FIXME: As signalled by PRAEDO, ezbounce seems to send a fourth parameter in response to /quote ezb log
				// Pragma: That's a bug in ezbounce, it sends the filesize of the log as a DCC CHAT parameter...
				//         The author probably copied and pasted the CTCP line from DCC SEND and forgot to remove the filesize.
				//         We *could* add an option to ignore the last parameter and treat it as a standard DCC chat
				//         request, but since we don't encourage bugs, we don't do it :D
				//         Mail me at pragma at kvirc dot net if you really think it's necessary.
				dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
				    __tr2qs_ctx("The above request is broken: it looks like a zero port tag acknowledge but I have either never seen this tag or it was sent more than 120 seconds ago", "dcc"));
				dcc_module_request_error(dcc, __tr2qs_ctx("It seems that I haven't requested this DCC chat", "dcc"));
				delete d;
				return;
			}
			else
			{
				g_pDccBroker->removeZeroPortTag(szTag);
			}

			d->bAutoAccept = true; // auto-accept it (we have sent it out)
			d->bActive = true;
		}
	}
	else
	{
		d->bAutoAccept = KVI_OPTION_BOOL(KviOption_boolAutoAcceptDccChat);
		d->bActive = true; // we have to connect (standard active chat)
	}

#ifdef COMPILE_SSL_SUPPORT
	d->bIsSSL = bSSLExtension;
#endif

	dcc_module_set_dcc_type(d, "CHAT");
	d->triggerCreationEvent();

	g_pDccBroker->handleChatRequest(d);
}

//
// SEND
//

static void dccModuleParseDccRecv(KviDccRequest * dcc);

static void dccModuleParseDccSend(KviDccRequest * dcc)
{
	//#warning "Ignore files depending on file type ? (MediaType ?)"
	//
	// We have received a DCC SEND request in the following form
	//
	//      DCC [ST]SEND <filename> <ipaddress> <port> <filesize>
	//
	// Now the things are a bit tricky... we eventually can
	// reply with a DCC RESUME and receive a DCC ACCEPT then
	// The format of these requests is:
	//
	//      DCC RESUME <filename> <port> <resumepos>
	//      ACCEPT <filename> <port> <resumepos>
	//
	// There is a mIrc extension that allows <port> to be 0
	// and adds a last parameter that seems to be a random number (thnx YaP :)
	// that is used to keep track of the connection.
	// This extension is used by firewalled machines to initiate a DCC SEND:
	// the receiving side should respond with a DCC SEND offer
	// with the same random number appended, listen for a connection, and receive the file
	// instead of sending it.
	//
	// when a zero port request is initiated by another party we get
	//      DCC SEND <filename> <fakeipaddress> 0 <filesize> <tag>
	// if (and only if) we want to resume we reply with
	//      DCC RESUME <filename> 0 <resumesize> <tag>
	// in this case the remote part replies again with
	//      DCC ACCEPT <filename> 0 <resumesize> <tag>
	// and we finally reply with
	//      DCC SEND <filename> <ourip> <ourport> <filesize> <tag>
	//
	// when a zero port request is initiated by us we send out
	//      DCC SEND <filename> <fakeipaddress> 0 <filesize> <tag>
	// and if the remote party wants to resume then we get
	//      DCC RESUME <filename> 0 <resumesize> <tag>
	// and we eventually reply with
	//      DCC ACCEPT <filename> 0 <resumesize> <tag>
	// and we finally get
	//      DCC SEND <filename> <remoteip> <remoteport> <filesize> <tag>
	//
	// Thus if there is a <tag> and the port is 0, then the remote party
	// is trying to send a file to us, but if the port is nonzero then
	// we have sent out a zero port request and the remote party acked it
	//

	if((!kvi_strEqualCS(dcc->szParam3.ptr(), "0")) && dcc->szParam5.hasData())
	{
		// DCC SEND <filename> <remoteip> <remoteport> <filesize> <tag>
		// zero port acknowledge: treat as a RECV that should look like
		// DCC [TS]RECV <filename> <remoteip> <remoteport> <resume-filesize>
		// but since we have stored the sharedfile with the name <tag>
		// we do exchange the params :)

		KviDccZeroPortTag * t = g_pDccBroker->findZeroPortTag(dcc->szParam5.ptr());
		if(t)
		{
			// FIXME: #warning "%u will not support 64-bit numbers and won't work as intended here"
			dcc->szParam4.sprintf("%u", t->m_uResumePosition);
			g_pDccBroker->removeZeroPortTag(dcc->szParam5.ptr());
		}
		else
		{
			// this should never happen since we always add
			// a zero port tag for out outgoing requests
			// but well... maybe the user did something behind our back...
			dcc->szParam4 = "0"; // no resume possible in this case
		}

		// swap the tag and the filename (we have added a fileoffer with this tag)
		dcc->szParam1 = dcc->szParam5;
		dcc->szParam5 = "";

		dccModuleParseDccRecv(dcc);
		return;
	}

	// First of all we check the transfer limits
	dcc->szParam1 = dcc->pConsole->decodeText(dcc->szParam1);
	if(!dcc_module_check_limits(dcc))
		return;
	if(!dcc_module_check_concurrent_transfers_limit(dcc))
		return;

	// Then we ensure that the data that the remote end has sent are valid
	if(!dcc_module_normalize_target_data(dcc, dcc->szParam2, dcc->szParam3))
		return;

	if(!(dcc->szParam4.isUnsignedNum()))
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
			    __tr2qs_ctx("The above request is broken: the fourth parameter should be the file size but doesn't appear to be an unsigned number; trying to continue", "dcc"), dcc->szParam4.ptr());
		}
		dcc->szParam4 = __tr2qs_ctx("<unknown size>", "dcc");
	}

	if(dcc->szParam1.contains('/'))
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
			    __tr2qs_ctx("The above request is broken: the filename contains path components, stripping the leading path and trying to continue", "dcc"), dcc->szParam1.ptr());
		}
		dcc->szParam1.cutToLast('/');
	}

	if(dcc->szParam1.contains("%2F"))
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
			    __tr2qs_ctx("The above request is broken: the filename contains path components, stripping the leading path and trying to continue", "dcc"), dcc->szParam1.ptr());
		}
		dcc->szParam1.cutToLast("%2F");
	}

	KviCString szExtensions = dcc->szType;
	szExtensions.cutRight(4); // cut off SEND

	bool bTurboExtension = szExtensions.contains('T', false);
#ifdef COMPILE_SSL_SUPPORT
	bool bSSLExtension = szExtensions.contains('S', false);
#else  //!COMPILE_SSL_SUPPORT
	if(szExtensions.contains('S', false))
	{
		dcc_module_request_error(dcc, __tr2qs_ctx("This executable has been compiled without SSL support, the SSL extension to DCC SEND is not available", "dcc"));
		return;
	}
#endif //!COMPILE_SSL_SUPPORT

	DccDescriptor * d = new DccDescriptor(dcc->pConsole);
	d->szNick = dcc->ctcpMsg->pSource->nick();
	d->szUser = dcc->ctcpMsg->pSource->user();
	d->szHost = dcc->ctcpMsg->pSource->host();
	dcc_fill_local_nick_user_host(d, dcc);

	d->szIp = dcc->szParam2.ptr();
	d->szPort = dcc->szParam3.ptr();
	d->szFileName = dcc->szParam1.ptr();
	d->szFileSize = dcc->szParam4.ptr();

	if(d->szPort == "0" && dcc->szParam5.hasData())
	{
		if(KVI_OPTION_BOOL(KviOption_boolDccSendFakeAddressByDefault))
		{
			d->szFakeIp = KVI_OPTION_STRING(KviOption_stringDefaultDccFakeAddress);
			if(d->szFakeIp.isEmpty())
				KVI_OPTION_BOOL(KviOption_boolDccSendFakeAddressByDefault) = false;
		}
		d->setZeroPortRequestTag(dcc->szParam5.ptr());
		QString tmp;
		if(!dcc_kvs_get_listen_ip_address(nullptr, d->console(), tmp))
			d->szListenIp = "0.0.0.0";
		else
			d->szListenIp = QString(tmp);
		d->szListenPort = "0"; // any port is OK
		d->bSendRequest = true;
		d->szLocalFileSize = d->szFileSize;
	}

	d->bActive = !d->isZeroPortRequest(); // we have to connect unless it is a zero port request

	d->bResume = false;
	d->bRecvFile = true;
	d->bIsTdcc = bTurboExtension;
	d->bNoAcks = d->bIsTdcc;
#ifdef COMPILE_SSL_SUPPORT
	d->bIsSSL = bSSLExtension;
#endif
	d->bOverrideMinimize = false;
	d->bAutoAccept = KVI_OPTION_BOOL(KviOption_boolAutoAcceptDccSend);

	d->bIsIncomingAvatar = g_pApp->findPendingAvatarChange(dcc->pConsole, d->szNick, d->szFileName);
	dcc_module_set_dcc_type(d, "RECV");
	if(KVI_OPTION_BOOL(KviOption_boolAutoAcceptIncomingAvatars))
		d->bAutoAccept = d->bAutoAccept || d->bIsIncomingAvatar;
	d->triggerCreationEvent();

	g_pDccBroker->recvFileManage(d);
}

static void dccModuleParseDccAccept(KviDccRequest * dcc)
{
	// this is usually DCC ACCEPT <filename> <port> <resumesize>
	// but may be also
	// DCC ACCEPT <filename> 0 <resumesize> <tag>
	if(!g_pDccBroker->handleResumeAccepted(dcc->szParam1.ptr(), dcc->szParam2.ptr(), dcc->szParam4.ptr()))
	{
		//#warning "IF KviOption_boolReplyCtcpErrmsgOnInvalidAccept..."
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			QString szError = QString(__tr2qs_ctx("Can't proceed with DCC RECV: transfer not initiated for file %1 on port %2", "dcc")).arg(dcc->szParam1.ptr()).arg(dcc->szParam2.ptr());
			dcc_module_request_error(dcc, szError);
		}
	}
}

static void dccModuleParseDccResume(KviDccRequest * dcc)
{
	// This is usually RESUME <filename> <port> <resumesize>

	// when a zero port request is initiated by us we send out
	//      DCC SEND <filename> <fakeipaddress> 0 <filesize> <tag>
	// and if the remote party wants to resume then we get
	//      DCC RESUME <filename> 0 <resumesize> <tag>
	// and we eventually reply with
	//      DCC ACCEPT <filename> 0 <resumesize> <tag>
	// and we finally get
	//      DCC SEND <filename> <remoteip> <remoteport> <filesize> <tag>

	bool bOk;
	quint64 filePos = (quint64)(dcc->szParam3.toULongLong(&bOk));
	if(!bOk)
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			QString szError = QString(__tr2qs_ctx("Invalid resume position argument '%1'", "dcc")).arg(dcc->szParam3.ptr());
			dcc_module_request_error(dcc, szError);
		}
		return;
	}

	if(!g_pDccBroker->handleResumeRequest(dcc, dcc->szParam1.ptr(), dcc->szParam2.ptr(), filePos, dcc->szParam4.ptr()))
	{
		//#warning "IF KviOption_boolReplyCtcpErrmsgOnInvalidResume..."
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			QString szError = QString(__tr2qs_ctx("Can't proceed with DCC SEND: transfer not initiated for file %1 on port %2, or invalid resume size", "dcc")).arg(dcc->szParam1.ptr()).arg(dcc->szParam2.ptr());
			dcc_module_request_error(dcc, szError);
		}
	}
}

//
// RECV
//

static void dccModuleParseDccRecv(KviDccRequest * dcc)
{
	// DCC [TS]RECV <filename> <ipaddr> <port> <resume-filesize>
	if(!dcc_module_check_limits(dcc))
		return;
	if(!dcc_module_check_concurrent_transfers_limit(dcc))
		return;

	if(!dcc_module_normalize_target_data(dcc, dcc->szParam2, dcc->szParam3))
		return;

	if(!(dcc->szParam4.isUnsignedNum()))
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			dcc->ctcpMsg->msg->console()->outputNoFmt(KVI_OUT_DCCMSG,
			    __tr2qs_ctx("The above request has resume file size missing, assuming a resume file size of 0", "dcc"));
		}
		dcc->szParam4 = "0";
	}

	if(dcc->szParam1.contains('/'))
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
			    __tr2qs_ctx("The above request is broken: the filename contains path components, stripping the leading path and trying to continue", "dcc"), dcc->szParam1.ptr());
		}
		dcc->szParam1.cutToLast('/');
	}

	KviCString szExtensions = dcc->szType;
	szExtensions.cutRight(4); // cut off RECV

	bool bTurboExtension = szExtensions.contains('T', false);
#ifdef COMPILE_SSL_SUPPORT
	bool bSSLExtension = szExtensions.contains('S', false);
#else  //!COMPILE_SSL_SUPPORT
	if(szExtensions.contains('S', false))
	{
		dcc_module_request_error(dcc, __tr2qs_ctx("This executable has been compiled without SSL support, the SSL extension to DCC RECV is not available", "dcc"));
		return;
	}
#endif //!COMPILE_SSL_SUPPORT

	// If we have a file offer for this...do it automatically
	KviSharedFile * o = g_pSharedFilesManager->lookupSharedFile(dcc->szParam1.ptr(), dcc->ctcpMsg->pSource, 0);
	if(o)
	{

		unsigned int uResumeSize = dcc->szParam4.toUInt(); // this will NEVER fail
		if(uResumeSize >= o->fileSize())
		{
			// senseless request
			QString szError = QString(__tr2qs_ctx("Invalid RECV request: position %1 is larger than file size", "dcc")).arg(uResumeSize);
			dcc_module_request_error(dcc, szError);
			return;
		}

		// ok...we have requested this send
		//		#warning "Maybe remove this file offer now ?"
		DccDescriptor * d = new DccDescriptor(dcc->pConsole);

		d->szNick = dcc->ctcpMsg->pSource->nick();
		d->szUser = dcc->ctcpMsg->pSource->user();
		d->szHost = dcc->ctcpMsg->pSource->host();

		d->szFileName = dcc->szParam1.ptr();
		d->szFileSize = dcc->szParam4.ptr();

		//d->bResume           = false; // This is actually useless

		d->szLocalFileName = o->absFilePath();
		d->szLocalFileSize.setNum(o->fileSize()); // Should we look it up again ?

		d->bRecvFile = false;
		d->bNoAcks = bTurboExtension;

		d->bAutoAccept = true;
		d->bIsIncomingAvatar = false;

		d->bIsTdcc = bTurboExtension;
#ifdef COMPILE_SSL_SUPPORT
		d->bIsSSL = bSSLExtension;
#endif

		d->bOverrideMinimize = false;

		// We know everything
		dcc_fill_local_nick_user_host(d, dcc);

		d->bDoTimeout = true;

		d->szIp = dcc->szParam2.ptr();
		d->szPort = dcc->szParam3.ptr();

		d->bActive = true;
		dcc_module_set_dcc_type(d, "SEND");
		d->triggerCreationEvent();
		g_pDccBroker->sendFileExecute(nullptr, d);

		return;
	}
	else
	{

		dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
		    __tr2qs_ctx("%Q [%Q@%Q] is ready to receive the file \"%s\"", "dcc"),
		    &(dcc->ctcpMsg->pSource->nick()),
		    &(dcc->ctcpMsg->pSource->user()),
		    &(dcc->ctcpMsg->pSource->host()),
		    dcc->szParam1.ptr());
		dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
		    __tr2qs_ctx("The remote client is listening on interface %s and port %s", "dcc"), dcc->szParam2.ptr(), dcc->szParam3.ptr());
		KviCString szSwitches = "-c";
		if(bTurboExtension)
			szSwitches.prepend("-t ");
#ifdef COMPILE_SSL_SUPPORT
		if(bSSLExtension)
			szSwitches.prepend("-s ");
#endif
		dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
		    __tr2qs_ctx("Use %c\r![!dbl]dcc.send %s -i=%s -p=%s %Q\r/dcc.send %s -i=%s -p=%s %Q\r%c to send the file (or double-click on the socket)", "dcc"),
		    KviControlCodes::Bold,
		    szSwitches.ptr(),
		    dcc->szParam2.ptr(), dcc->szParam3.ptr(), &(dcc->ctcpMsg->pSource->nick()),
		    szSwitches.ptr(),
		    dcc->szParam2.ptr(), dcc->szParam3.ptr(), &(dcc->ctcpMsg->pSource->nick()),
		    KviControlCodes::Bold);
	}
}

//
// RSEND
//

static void dccModuleParseDccRSend(KviDccRequest * dcc)
{
	// DCC RSEND <filename> <filesize>
	//#warning "Ignore files depending on file type ? (MediaType ?)"
	//
	// We have received a DCC RSEND request in the following form
	//
	//      DCC [ST]RSEND <filename> <filesize>
	//
	dcc->szParam1 = dcc->pConsole->decodeText(dcc->szParam1);
	if(!dcc_module_check_limits(dcc))
		return;
	if(!dcc_module_check_concurrent_transfers_limit(dcc))
		return;

	if(!(dcc->szParam2.isUnsignedNum()))
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
			    __tr2qs_ctx("The above request is broken: the fourth parameter should be the file size but doesn't appear to be an unsigned number; trying to continue", "dcc"), dcc->szParam2.ptr());
		}
		dcc->szParam2 = __tr_ctx("<unknown size>", "dcc");
	}

	if(dcc->szParam1.contains('/'))
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
			    __tr2qs_ctx("The above request is broken: the filename contains path components, stripping the leading path and trying to continue", "dcc"), dcc->szParam1.ptr());
		}
		dcc->szParam1.cutToLast('/');
	}

	KviCString szExtensions = dcc->szType;
	szExtensions.cutRight(4); // cut off SEND

	bool bTurboExtension = szExtensions.contains('T', false);
#ifdef COMPILE_SSL_SUPPORT
	bool bSSLExtension = szExtensions.contains('S', false);
#else  //!COMPILE_SSL_SUPPORT
	if(szExtensions.contains('S', false))
	{
		dcc_module_request_error(dcc, __tr2qs_ctx("This executable has been compiled without SSL support, the SSL extension to DCC RSEND is not available", "dcc"));
		return;
	}
#endif //!COMPILE_SSL_SUPPORT

	//#warning "When behind a firewall, we should reply an error message and avoid setting up the listening connection"

	DccDescriptor * d = new DccDescriptor(dcc->pConsole);
	d->szNick = dcc->ctcpMsg->pSource->nick();
	d->szUser = dcc->ctcpMsg->pSource->user();
	d->szHost = dcc->ctcpMsg->pSource->host();
	d->szIp = __tr2qs_ctx("(unknown)", "dcc");
	d->szPort = d->szIp;
	QString tmp;
	if(!dcc_kvs_get_listen_ip_address(nullptr, d->console(), tmp))
	{
		d->console()->output(KVI_OUT_DCCMSG,
		    __tr2qs_ctx("No suitable interface to listen on, trying to continue anyway...", "dcc"));
		d->szListenIp = "0.0.0.0";
	}
	else
		d->szListenIp = QString(tmp);

	d->szListenPort = "0";
	dcc_fill_local_nick_user_host(d, dcc);

	d->szFileName = dcc->szParam1.ptr();
	d->szFileSize = dcc->szParam2.ptr();
	d->bActive = false; // we have to listen!
	d->bResume = false;
	d->bRecvFile = true; // we have to receive the file!

#ifdef COMPILE_SSL_SUPPORT
	d->bIsSSL = bSSLExtension;
#endif
	d->bIsTdcc = bTurboExtension;
	d->bSendRequest = true; // we have to send the [ST]RECV request back
	d->bNoAcks = d->bIsTdcc;
	d->bOverrideMinimize = false;
	d->bAutoAccept = KVI_OPTION_BOOL(KviOption_boolAutoAcceptDccSend);
	d->bIsIncomingAvatar = g_pApp->findPendingAvatarChange(dcc->pConsole, d->szNick.toUtf8().data(), d->szFileName.toUtf8().data());

	if(KVI_OPTION_BOOL(KviOption_boolDccSendFakeAddressByDefault))
	{
		d->szFakeIp = KVI_OPTION_STRING(KviOption_stringDefaultDccFakeAddress);
		if(d->szFakeIp.isEmpty())
			KVI_OPTION_BOOL(KviOption_boolDccSendFakeAddressByDefault) = false;
	}

	if(KVI_OPTION_BOOL(KviOption_boolAutoAcceptIncomingAvatars))
		d->bAutoAccept = d->bAutoAccept || d->bIsIncomingAvatar;

	dcc_module_set_dcc_type(d, "RECV");
	d->triggerCreationEvent();
	g_pDccBroker->recvFileManage(d);
}

//
// GET
//

static void dccModuleParseDccGet(KviDccRequest * dcc)
{
	// DCC [TS]GET <filename> [filesize]
	// -> DCC [TS]SEND <filename> <ipaddr> <port> <filesize>
	// ...
	dcc->szParam1 = dcc->pConsole->decodeText(dcc->szParam1);
	bool bOk;
	unsigned int uSize = dcc->szParam2.toUInt(&bOk);
	if(!bOk)
		uSize = 0;

	if(!dcc_module_check_limits(dcc))
		return;
	if(!dcc_module_check_concurrent_transfers_limit(dcc))
		return;

	KviCString szExtensions = dcc->szType;
	szExtensions.cutRight(3); // cut off GET

	bool bTurboExtension = szExtensions.contains('T', false);
#ifdef COMPILE_SSL_SUPPORT
	bool bSSLExtension = szExtensions.contains('S', false);
#else  //!COMPILE_SSL_SUPPORT
	if(szExtensions.contains('S', false))
	{
		dcc_module_request_error(dcc, __tr2qs_ctx("This executable has been compiled without SSL support, the SSL extension to DCC GET is not available", "dcc"));
		return;
	}
#endif //!COMPILE_SSL_SUPPORT

	KviSharedFile * o = g_pSharedFilesManager->lookupSharedFile(dcc->szParam1.ptr(), dcc->ctcpMsg->pSource, uSize);
	if(!o)
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			QString szError = QString(__tr2qs_ctx("No file offer named '%1' (with size %2) available for %3 [%4@%5]", "dcc")).arg(dcc->szParam1.ptr()).arg(uSize > 0 ? dcc->szParam2.ptr() : __tr_ctx("\"any\"", "dcc")).arg(dcc->ctcpMsg->pSource->nick(), dcc->ctcpMsg->pSource->user(), dcc->ctcpMsg->pSource->host());
			dcc_module_request_error(dcc, szError);
		}
		return;
	}

	//#warning "IF NOT IGNORE DCC GET!"
	//#warning "CREATE IT MINIMIZED ETC..."
	//#warning "MAYBE USE A DIALOG TO ACCEPT THE REQUEST ?"
	//#warning "DO NOT ACCEPT /etc/* requests..."

	if(KVI_OPTION_BOOL(KviOption_boolCantAcceptIncomingDccConnections))
	{
		// we have to use DCC RSEND, otherwise it will not work
		KviCString szSubproto("RSEND");
		szSubproto.prepend(szExtensions);

		QString szFileName = QFileInfo(o->absFilePath()).fileName();
		if(o->name() != szFileName)
		{
			// BUG
			//   If the file offer was added with a name that is senseless (like "mediaXYZ" for an *.mp3 file)
			//   then we would be going to RSEND that name here: the remote user woulnd't be
			//   able to recognize the file.
			//   Here we add another temporary offer with the right filename.

			// now add a file offer, so he we will accept it automatically
			// 120 secs is a reasonable timeout
			QString szMask;
			dcc->ctcpMsg->pSource->mask(szMask, KviIrcMask::NickUserHost);

			KviSharedFile * pOld = o;
			o = g_pSharedFilesManager->addSharedFile(szFileName, o->absFilePath(), szMask, 120);
			if(!o)
				o = pOld; // give up (FIXME: should we notify that ?)
		}

		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
			    __tr2qs_ctx("Accepting file request from %Q [%Q@%Q] for '%s' (real file: %Q), offering DCC %s since we can't accept incoming connections (user option)", "dcc"),
			    &(dcc->ctcpMsg->pSource->nick()),
			    &(dcc->ctcpMsg->pSource->user()),
			    &(dcc->ctcpMsg->pSource->host()), dcc->szParam1.ptr(),
			    &(o->absFilePath()), szSubproto.ptr());
		}

		dcc->pConsole->connection()->sendFmtData("PRIVMSG %s :%cDCC %s %s %s%c",
		    dcc->pConsole->connection()->encodeText(dcc->ctcpMsg->pSource->nick()).data(),
		    0x01, szSubproto.ptr(),
		    dcc->pConsole->connection()->encodeText(dcc->szParam1.ptr()).data(),
		    dcc->pConsole->connection()->encodeText(QString::number(o->fileSize())).data(),
		    0x01);
		return;
	}

	DccDescriptor * d = new DccDescriptor(dcc->pConsole);
	d->szNick = dcc->ctcpMsg->pSource->nick();
	d->szLocalFileName = o->absFilePath();
	d->szUser = dcc->ctcpMsg->pSource->user();
	d->szHost = dcc->ctcpMsg->pSource->host();
	d->bRecvFile = false;
	dcc_fill_local_nick_user_host(d, dcc);

	QString tmp;
	if(!dcc_kvs_get_listen_ip_address(nullptr, d->console(), tmp))
	{
		d->console()->output(KVI_OUT_DCCMSG,
		    __tr2qs_ctx("No suitable interface to listen on, trying to continue anyway...", "dcc"));
		d->szListenIp = "0.0.0.0";
	}
	else
		d->szListenIp = QString(tmp);
	//#warning "DO STH WITH THIS PORT (HOW TO SPECIFY IT ?)"
	d->szListenPort = "0"; // any port is ok

	if(KVI_OPTION_BOOL(KviOption_boolDccSendFakeAddressByDefault))
	{
		d->szFakeIp = KVI_OPTION_STRING(KviOption_stringDefaultDccFakeAddress);
		if(d->szFakeIp.isEmpty())
			KVI_OPTION_BOOL(KviOption_boolDccSendFakeAddressByDefault) = false;
	}

	d->bDoTimeout = true;
	d->szIp = __tr2qs_ctx("(unknown)", "dcc");
	d->szPort = d->szIp;
	d->bActive = false;
	d->bSendRequest = true;
	d->bIsTdcc = bTurboExtension;
#ifdef COMPILE_SSL_SUPPORT
	d->bIsSSL = bSSLExtension;
#endif
	d->bNoAcks = d->bIsTdcc;
	d->bOverrideMinimize = false;

	dcc_module_set_dcc_type(d, "SEND");

	if(!dcc->ctcpMsg->msg->haltOutput())
	{
		dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
		    __tr2qs_ctx("Accepting file request from %Q [%Q@%Q] for '%s' (real file: %Q), offering DCC %Q", "dcc"),
		    &(dcc->ctcpMsg->pSource->nick()),
		    &(dcc->ctcpMsg->pSource->user()),
		    &(dcc->ctcpMsg->pSource->host()),
		    dcc->szParam1.ptr(),
		    &(o->absFilePath()), &(d->szType));
	}
	d->triggerCreationEvent();
	g_pDccBroker->sendFileExecute(nullptr, d);
}

//
// VOICE
//

static void dccModuleParseDccVoice(KviDccRequest * dcc)
{
	//
	// We have received a DCC VOICE request in the following form:
	//
	//     DCC VOICE codec <ipaddress> <port> <sample-rate>
	//
	// This means that we're requested to setup an ACTIVE voice connection
	// ... Easy task :)
	//

	if(!dcc_module_check_limits(dcc))
		return;

	if(!dcc_module_normalize_target_data(dcc, dcc->szParam2, dcc->szParam3))
		return;

#ifdef COMPILE_DISABLE_DCC_VOICE
	if(!dcc->ctcpMsg->msg->haltOutput())
	{
		dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCERROR,
		    __tr2qs_ctx("The above request can't be accepted: DCC VOICE support not enabled at compilation time ", "dcc"));
		return;
	}
#endif
	//  Actually unused parameter
	if(!kvi_dcc_voice_is_valid_codec(dcc->szParam1.ptr()))
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCERROR,
			    __tr2qs_ctx("The above request can't be accepted: unsupported codec '%s'", "dcc"), dcc->szParam1.ptr());
			return;
		}
	}

	bool bOk;

	int iSampleRate = dcc->szParam4.toInt(&bOk);
	if(!bOk)
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCMSG,
			    __tr2qs_ctx("The above request appears to be broken: invalid sample-rate '%s', defaulting to 8000", "dcc"), dcc->szParam4.ptr());
		}
		iSampleRate = 8000;
	}

	DccDescriptor * d = new DccDescriptor(dcc->pConsole);
	d->szNick = dcc->ctcpMsg->pSource->nick();
	d->szUser = dcc->ctcpMsg->pSource->user();
	d->szHost = dcc->ctcpMsg->pSource->host();
	dcc_fill_local_nick_user_host(d, dcc);

	d->szIp = dcc->szParam2.ptr();
	d->szPort = dcc->szParam3.ptr();
	d->bActive = true; // we have to connect
	d->bIsTdcc = false;
	d->bNoAcks = false; // this has no meaning in voice
	d->szCodec = dcc->szParam1;
	d->iSampleRate = iSampleRate;
	d->bOverrideMinimize = false;
	d->bAutoAccept = KVI_OPTION_BOOL(KviOption_boolAutoAcceptDccVoice);
	dcc_module_set_dcc_type(d, "VOICE");
	d->triggerCreationEvent();
	g_pDccBroker->activeVoiceManage(d);
}

//
// VIDEO
//

static void dccModuleParseDccVideo(KviDccRequest * dcc)
{
	//
	// We have received a DCC VIDEO request in the following form:
	//
	//     DCC VIDEO codec <ipaddress> <port>
	//
	// This means that we're requested to setup an ACTIVE voice connection
	// ... Easy task :)
	//

	if(!dcc_module_check_limits(dcc))
		return;

	if(!dcc_module_normalize_target_data(dcc, dcc->szParam2, dcc->szParam3))
		return;

#ifdef COMPILE_DISABLE_DCC_VIDEO
	if(!dcc->ctcpMsg->msg->haltOutput())
	{
		dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCERROR,
		    __tr2qs_ctx("The above request can't be accepted: DCC VIDEO support not enabled at compilation time ", "dcc"));
		return;
	}
#else
	//  Actually unused parameter
	if(!kvi_dcc_video_is_valid_codec(dcc->szParam1.ptr()))
	{
		if(!dcc->ctcpMsg->msg->haltOutput())
		{
			dcc->ctcpMsg->msg->console()->output(KVI_OUT_DCCERROR,
			    __tr2qs_ctx("The above request can't be accepted: unsupported codec '%s'", "dcc"), dcc->szParam1.ptr());
			return;
		}
	}

	DccDescriptor * d = new DccDescriptor(dcc->pConsole);
	d->szNick = dcc->ctcpMsg->pSource->nick();
	d->szUser = dcc->ctcpMsg->pSource->user();
	d->szHost = dcc->ctcpMsg->pSource->host();
	dcc_fill_local_nick_user_host(d, dcc);

	d->szIp = dcc->szParam2.ptr();
	d->szPort = dcc->szParam3.ptr();
	d->bActive = true; // we have to connect
	d->bIsTdcc = false;
	d->bNoAcks = false; // this has no meaning in voice
	d->szCodec = dcc->szParam1;
	// 	d->iSampleRate       = iSampleRate;
	d->bOverrideMinimize = false;
	d->bAutoAccept = KVI_OPTION_BOOL(KviOption_boolAutoAcceptDccVideo);
	dcc_module_set_dcc_type(d, "VIDEO");
	d->triggerCreationEvent();
	g_pDccBroker->activeVideoManage(d);
#endif
}

//
// CANVAS
//

static void dccModuleParseDccCanvas(KviDccRequest * dcc)
{
	//
	// We have received a DCC CANVAS request in the following form:
	//
	//     DCC CANVAS unused <ipaddress> <port>
	//
	// This means that we're requested to setup an ACTIVE canvas connection
	// ... Easy task :)
	//
	if(!dcc_module_check_limits(dcc))
		return;

	if(!dcc_module_normalize_target_data(dcc, dcc->szParam2, dcc->szParam3))
		return;

#ifdef COMPILE_DCC_CANVAS
	DccDescriptor * d = new DccDescriptor(dcc->pConsole);
	d->szNick = dcc->ctcpMsg->pSource->nick();
	d->szUser = dcc->ctcpMsg->pSource->username();
	d->szHost = dcc->ctcpMsg->pSource->host();
	dcc_fill_local_nick_user_host(d, dcc);

	d->szIp = dcc->szParam2.ptr();
	d->szPort = dcc->szParam3.ptr();
	d->bActive = true; // we have to connect
	d->bIsTdcc = false;
	d->bNoAcks = false; // this has no meaning in canvas
	d->bOverrideMinimize = false;
	d->bAutoAccept = KVI_OPTION_BOOL(KviOption_boolAutoAcceptDccCanvas);
	dcc_module_set_dcc_type(d, "CANVAS");
	d->triggerCreationEvent();
	g_pDccBroker->activeCanvasManage(d);
#endif
}

static void dccModuleParseDccList(KviDccRequest *)
{
	// DCC LIST <mask> <ipaddr> <port>
	// FIXME!
}

using dccParseProc = void (*)(KviDccRequest *);
struct dccParseProcEntry
{
	const char * type;
	dccParseProc proc;
};

#define KVI_NUM_KNOWN_DCC_TYPES 28

static dccParseProcEntry dccParseProcTable[KVI_NUM_KNOWN_DCC_TYPES] = {
	// clang-format off
	{ "CHAT"   , dccModuleParseDccChat   },
	{ "SCHAT"  , dccModuleParseDccChat   },
	{ "SEND"   , dccModuleParseDccSend   },
	{ "TSEND"  , dccModuleParseDccSend   },
	{ "SSEND"  , dccModuleParseDccSend   },
	{ "TSSEND" , dccModuleParseDccSend   },
	{ "STSEND" , dccModuleParseDccSend   },
	{ "GET"    , dccModuleParseDccGet    },
	{ "SGET"   , dccModuleParseDccGet    },
	{ "TGET"   , dccModuleParseDccGet    },
	{ "STGET"  , dccModuleParseDccGet    },
	{ "TSGET"  , dccModuleParseDccGet    },
	{ "LIST"   , dccModuleParseDccList   },
	{ "ACCEPT" , dccModuleParseDccAccept },
	{ "RESUME" , dccModuleParseDccResume },
	{ "RECV"   , dccModuleParseDccRecv   },
	{ "SRECV"  , dccModuleParseDccRecv   },
	{ "TRECV"  , dccModuleParseDccRecv   },
	{ "TSRECV" , dccModuleParseDccRecv   },
	{ "STRECV" , dccModuleParseDccRecv   },
	{ "RSEND"  , dccModuleParseDccRSend  },
	{ "SRSEND" , dccModuleParseDccRSend  },
	{ "TRSEND" , dccModuleParseDccRSend  },
	{ "STRSEND", dccModuleParseDccRSend  },
	{ "TSRSEND", dccModuleParseDccRSend  },
	{ "CANVAS" , dccModuleParseDccCanvas },
	{ "VOICE"  , dccModuleParseDccVoice  },
	{ "VIDEO"  , dccModuleParseDccVideo  }
	// clang-format on
};

// We want C linkage on this one: we want to be able to dlsym() it with a simple name
// FIXME: Is this portable enough ? Or is better to have a table entry ?

KVIMODULEEXPORTFUNC void dccModuleCtcpDccParseRoutine(KviDccRequest * dcc)
{
	dcc->szType.toUpper();

	for(auto & i : dccParseProcTable)
	{
		if(kvi_strEqualCS(i.type, dcc->szType.ptr()))
		{
			(i.proc)(dcc);
			return;
		}
	}
	// ops...we don't know this dcc type
	if(!dcc->ctcpMsg->msg->haltOutput())
	{
		QString szError = QString(__tr2qs_ctx("Unknown DCC type '%1'", "dcc")).arg(dcc->szType.ptr());
		dcc_module_request_error(dcc, szError);
	}
}
