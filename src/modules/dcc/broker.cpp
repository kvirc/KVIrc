//=============================================================================
//
//   File : broker.cpp
//   Creation date : Tue Sep 19 09 2000 10:21:54 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"

#include "broker.h"
#include "dialogs.h"
#include "chat.h"
#include "send.h"
#include "voice.h"

#ifndef COMPILE_DISABLE_DCC_VIDEO
	#include "video.h"
#endif

#ifdef COMPILE_DCC_CANVAS
	#include "canvas.h"
#endif

#include "kvi_app.h"
#include "kvi_frame.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_console.h"
#include "kvi_fileutils.h"
#include "kvi_out.h"
#include "kvi_mediatype.h"
#include "kvi_ircconnection.h"
#include "kvi_sharedfiles.h"

#include <QFileInfo>
#include <QString>

// kvi_app.cpp
extern KVIRC_API KviMediaManager * g_pMediaManager;
extern KVIRC_API KviSharedFilesManager * g_pSharedFilesManager;

//#warning "The broker might lookup the remote host name"

KviDccBroker::KviDccBroker()
: QObject(0)
{
	setObjectName("dcc_broker");
	KviDccFileTransfer::init();

	m_pBoxList = new KviPointerList<KviDccBox>;
	m_pBoxList->setAutoDelete(false);

	m_pDccWindowList = new KviPointerList<KviWindow>;
	m_pDccWindowList->setAutoDelete(false);

	m_pZeroPortTags = new KviPointerHashTable<QString,KviDccZeroPortTag>(17);
	m_pZeroPortTags->setAutoDelete(true);
}

KviDccBroker::~KviDccBroker()
{
	delete m_pZeroPortTags;
	while(m_pBoxList->first())delete m_pBoxList->first();
	delete m_pBoxList;
	m_pBoxList = 0;
	while(m_pDccWindowList->first())delete m_pDccWindowList->first();
	delete m_pDccWindowList;
	KviDccFileTransfer::done();
}


KviDccZeroPortTag * KviDccBroker::addZeroPortTag()
{
	static unsigned int g_uNextZeroPortTag = 0;
	g_uNextZeroPortTag++;
	KviDccZeroPortTag * t = new KviDccZeroPortTag;
	t->m_tTimestamp = QDateTime::currentDateTime();
	t->m_szTag.setNum(g_uNextZeroPortTag);
	//t->m_szTag.prepend("mIrc-zero-port-");
	t->m_uResumePosition = 0;
	// FIXME: we should clear this dict if it grows too high....
	m_pZeroPortTags->insert(t->m_szTag,t);
	return t;
}

KviDccZeroPortTag * KviDccBroker::findZeroPortTag(const QString &szTag)
{
	KviDccZeroPortTag * t = m_pZeroPortTags->find(szTag);
	if(!t)return 0;
	if(t->m_tTimestamp.secsTo(QDateTime::currentDateTime()) > 180)
	{
		// too late man...
		m_pZeroPortTags->remove(szTag);
		return 0;
	}
	return t;
}

void KviDccBroker::removeZeroPortTag(const QString &szTag)
{
	m_pZeroPortTags->remove(szTag);
}

unsigned int KviDccBroker::dccBoxCount()
{
	return m_pBoxList->count();
}

void KviDccBroker::unregisterDccWindow(KviWindow *wnd)
{
	m_pDccWindowList->removeRef(wnd);
}

void KviDccBroker::unregisterDccBox(KviDccBox * box)
{
	//debug("Forgetting box %d",box);
	m_pBoxList->removeRef(box);
}

void KviDccBroker::cancelDcc(KviDccDescriptor * dcc)
{
	delete dcc;
	dcc = 0;
}

void KviDccBroker::cancelDcc(KviDccBox *box,KviDccDescriptor * dcc)
{
	if(box)box->forgetDescriptor();
	delete dcc;
	dcc = 0;
}

///////////////////////////////////////////////////////////////////////////////
// RSEND
///////////////////////////////////////////////////////////////////////////////

void KviDccBroker::rsendManage(KviDccDescriptor * dcc)
{
	// We need the filename...
	QFileInfo fi(dcc->szLocalFileName);
	if(fi.exists())rsendExecute(0,dcc);
	else rsendAskForFileName(dcc);
}

void KviDccBroker::rsendAskForFileName(KviDccDescriptor * dcc)
{
	QStringList filenames;
	if(
		KviFileDialog::askForOpenFileNames(filenames,
		__tr2qs_ctx("Choose Files to Send - KVIrc","dcc"),"")
		) {
			if(filenames.count() > 0)
			{
				KviDccDescriptor * d;
				QStringList::Iterator it=filenames.begin();
				while(it != filenames.end())
				{
					d = new KviDccDescriptor(*dcc);
					d->szLocalFileName = *(it);
					d->szLocalFileName.trimmed();
					++it;
					if(d->szLocalFileName.isEmpty())
						cancelDcc(d);
					else
						rsendExecute(d);
				}
				delete dcc;
			}
	} else {
		cancelDcc(dcc);
	}
}

void KviDccBroker::rsendExecute(KviDccDescriptor * dcc)
{
	if(!g_pApp->windowExists(dcc->console()))
	{
		// No way...we NEED the right IRC context...
		g_pApp->activeConsole()->output(KVI_OUT_DCCERROR,
			__tr2qs_ctx("Can't send DCC %Q request to %Q: IRC connection has been terminated","dcc"),
			&(dcc->szType),&(dcc->szNick));
		delete dcc;
		return;
	}

	// Ok...we need the file to exist
	QFileInfo fi(dcc->szLocalFileName);
	if(!(fi.exists() && fi.isReadable() && (fi.isFile()) && (fi.size() > 0)))
	{
		dcc->console()->output(KVI_OUT_DCCERROR,__tr2qs_ctx("Can't open file %Q for reading","dcc"),
			&(dcc->szLocalFileName));
		delete dcc;
		return;
	}

	dcc->szFileName = dcc->szLocalFileName;
	dcc->szFileName = QFileInfo(dcc->szFileName).fileName();

	QString fName = dcc->szFileName;
	fName.replace(' ',"\\040"); // be cool :)

	QString szTag;
	if(dcc->isZeroPortRequest())
	{
		// actually we tagged it as "nonempty" in /dcc.rsend --zero-port
		// retag it with something more reasonable
		KviDccZeroPortTag * t = addZeroPortTag();
		t->m_uFileSize = fi.size();
		dcc->setZeroPortRequestTag(t->m_szTag.toLatin1()); // latin1() should be ok here
		szTag = t->m_szTag;

		// DCC [ST]SEND <filename> <fakeipaddress> <zero-port> <filesize> <sessionid>
		dcc->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC %s %s 127.0.0.1 0 %u %s%c",
			dcc->console()->connection()->encodeText(dcc->szNick).data(),
			0x01,
			dcc->console()->connection()->encodeText(dcc->szType).data(),
			dcc->console()->connection()->encodeText(fName).data(),
			fi.size(),
			dcc->console()->connection()->encodeText(szTag).data(),
			0x01);
	} else {
		dcc->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC %s %s %u%c",
			dcc->console()->connection()->encodeText(dcc->szNick).data(),
			0x01,
			dcc->console()->connection()->encodeText(dcc->szType).data(),
			dcc->console()->connection()->encodeText(fName).data(),
			fi.size(),0x01);
		szTag = dcc->szFileName;
	}

	// now add a file offer , so he we will accept it automatically
	// 120 secs is a reasonable timeout
	QString szMask = dcc->szNick;
	szMask += "!*@*";

	g_pSharedFilesManager->addSharedFile(szTag,dcc->szLocalFileName,szMask,120);

	delete dcc;
}

void KviDccBroker::rsendExecute(KviDccBox * box,KviDccDescriptor * dcc)
{
	if(box)box->forgetDescriptor();
	rsendExecute(dcc);
}


///////////////////////////////////////////////////////////////////////////////
// DCC CHAT
///////////////////////////////////////////////////////////////////////////////

void KviDccBroker::handleChatRequest(KviDccDescriptor * dcc)
{
	if(!dcc->bAutoAccept)
	{
		// FIXME: better message ? Secure Direct Client Connection...eventually
		// need confirmation
		QString tmp = __tr2qs_ctx( \
				"<b>%1 [%2@%3]</b> requests a " \
				"<b>Direct Client Connection</b> in <b>%4</b> mode.<br>", \
				"dcc").arg(dcc->szNick, dcc->szUser, dcc->szHost, dcc->szType);

#ifdef COMPILE_SSL_SUPPORT
		if(dcc->bIsSSL)tmp += __tr2qs_ctx("The connection will be secured using SSL.<br>","dcc");
#endif

		if(dcc->isZeroPortRequest())
		{
			tmp += __tr2qs_ctx( \
					"You will be the passive side of the connection.<br>" \
					,"dcc");
		} else {
			tmp += __tr2qs_ctx( \
					"The connection target will be host <b>%1</b> on port <b>%2</b><br>" \
					,"dcc").arg(dcc->szIp, dcc->szPort);
		}


		QString caption = __tr2qs_ctx("DCC %1 Request - KVIrc","dcc").arg(dcc->szType);

		KviDccAcceptBox * box = new KviDccAcceptBox(this,dcc,tmp,caption);

		m_pBoxList->append(box);
		connect(box,SIGNAL(accepted(KviDccBox *,KviDccDescriptor *)),
				this,SLOT(executeChat(KviDccBox *,KviDccDescriptor *)));
		connect(box,SIGNAL(rejected(KviDccBox *,KviDccDescriptor *)),
				this,SLOT(cancelDcc(KviDccBox *,KviDccDescriptor *)));
		box->show();
	} else {
		// auto accept
		executeChat(0,dcc);
	}
}

void KviDccBroker::executeChat(KviDccBox *box,KviDccDescriptor * dcc)
{
	if(box)box->forgetDescriptor();

	if(!g_pApp->windowExists(dcc->console()))
	{
		// rebind to the first available console....
		dcc->setConsole(g_pApp->activeConsole());
	}

	KviStr szSubProto = dcc->szType;
	szSubProto.toLower();

	QString tmp = QString("dcc: %1 %2@%3:%4").arg(szSubProto.ptr(), dcc->szNick, dcc->szIp, dcc->szPort);
	KviDccChat * chat = new KviDccChat(dcc->console()->frame(),dcc,tmp.toUtf8().data());

	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : \
			(KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChat) || \
				(dcc->bAutoAccept && KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChatWhenAutoAccepted)));

	dcc->console()->frame()->addWindow(chat,!bMinimized);
	if(bMinimized)chat->minimize();
	m_pDccWindowList->append(chat);
}

///////////////////////////////////////////////////////////////////////////////
// ACTIVE VOICE
///////////////////////////////////////////////////////////////////////////////

void KviDccBroker::activeVoiceManage(KviDccDescriptor * dcc)
{
	if(!dcc->bAutoAccept)
	{
		// need confirmation
		QString tmp = __tr2qs_ctx(
					"<b>%1 [%2@%3]</b> requests a<br>" \
					"<b>Direct Client Connection</b> in <b>VOICE</b> mode.<br>" \
					"The connection target will be host <b>%4</b> on port <b>%5</b><br>" \
				,"dcc" \
			).arg(dcc->szNick, dcc->szUser, dcc->szHost, dcc->szIp, dcc->szPort);

		KviDccAcceptBox * box = new KviDccAcceptBox(this,dcc,tmp,__tr2qs_ctx("DCC VOICE request","dcc"));
		m_pBoxList->append(box);
		connect(box,SIGNAL(accepted(KviDccBox *,KviDccDescriptor *)),
				this,SLOT(activeVoiceExecute(KviDccBox *,KviDccDescriptor *)));
		connect(box,SIGNAL(rejected(KviDccBox *,KviDccDescriptor *)),
				this,SLOT(cancelDcc(KviDccBox *,KviDccDescriptor *)));
		box->show();
	} else {
		// auto accept
		activeVoiceExecute(0,dcc);
	}
}

void KviDccBroker::activeVoiceExecute(KviDccBox *box,KviDccDescriptor * dcc)
{
	if(box)box->forgetDescriptor();

	if(!g_pApp->windowExists(dcc->console()))
	{
		// rebind to the first available console....
		dcc->setConsole(g_pApp->activeConsole());
	}

	KviStr tmp(KviStr::Format,"dcc: voice %s@%s:%s",dcc->szNick.toUtf8().data(),dcc->szIp.toUtf8().data(),dcc->szPort.toUtf8().data());
	KviDccVoice * v = new KviDccVoice(dcc->console()->frame(),dcc,tmp.ptr());

	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : \
			(KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccVoice) || \
				(dcc->bAutoAccept && KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccVoiceWhenAutoAccepted)));

	dcc->console()->frame()->addWindow(v,!bMinimized);
	if(bMinimized)v->minimize();

	m_pDccWindowList->append(v);
}


///////////////////////////////////////////////////////////////////////////////
// PASSIVE VOICE
///////////////////////////////////////////////////////////////////////////////

void KviDccBroker::passiveVoiceExecute(KviDccDescriptor * dcc)
{
	KviStr tmp(KviStr::Format,"dcc: voice %s@%s:%s",dcc->szNick.toUtf8().data(),dcc->szIp.toUtf8().data(),dcc->szPort.toUtf8().data());
	KviDccVoice * v = new KviDccVoice(dcc->console()->frame(),dcc,tmp.ptr());
	//#warning "Create minimized dcc voice ?... or maybe it's too much ? :)"
	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChat);
	dcc->console()->frame()->addWindow(v,!bMinimized);
	if(bMinimized)v->minimize();
	m_pDccWindowList->append(v);
}


///////////////////////////////////////////////////////////////////////////////
// ACTIVE VIDEO
///////////////////////////////////////////////////////////////////////////////

#ifndef COMPILE_DISABLE_DCC_VIDEO
void KviDccBroker::activeVideoManage(KviDccDescriptor * dcc)
{
	if(!dcc->bAutoAccept)
	{
		// need confirmation
		QString tmp = __tr2qs_ctx(
					"<b>%1 [%2@%3]</b> requests a<br>" \
					"<b>Direct Client Connection</b> in <b>VIDEO</b> mode.<br>" \
					"The connection target will be host <b>%4</b> on port <b>%5</b><br>" \
				,"dcc" \
			).arg(dcc->szNick, dcc->szUser, dcc->szHost, dcc->szIp, dcc->szPort);

		KviDccAcceptBox * box = new KviDccAcceptBox(this,dcc,tmp,__tr2qs_ctx("DCC VIDEO request","dcc"));
		m_pBoxList->append(box);
		connect(box,SIGNAL(accepted(KviDccBox *,KviDccDescriptor *)),
				this,SLOT(activeVideoExecute(KviDccBox *,KviDccDescriptor *)));
		connect(box,SIGNAL(rejected(KviDccBox *,KviDccDescriptor *)),
				this,SLOT(cancelDcc(KviDccBox *,KviDccDescriptor *)));
		box->show();
	} else {
		// auto accept
		activeVideoExecute(0,dcc);
	}
}

void KviDccBroker::activeVideoExecute(KviDccBox *box,KviDccDescriptor * dcc)
{
	if(box)box->forgetDescriptor();

	if(!g_pApp->windowExists(dcc->console()))
	{
		// rebind to the first available console....
		dcc->setConsole(g_pApp->activeConsole());
	}

	KviStr tmp(KviStr::Format,"dcc: video %s@%s:%s",dcc->szNick.toUtf8().data(),dcc->szIp.toUtf8().data(),dcc->szPort.toUtf8().data());
	KviDccVideo * v = new KviDccVideo(dcc->console()->frame(),dcc,tmp.ptr());

	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : \
			(KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccVideo) || \
				(dcc->bAutoAccept && KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccVideoWhenAutoAccepted)));

	dcc->console()->frame()->addWindow(v,!bMinimized);
	if(bMinimized)v->minimize();

	m_pDccWindowList->append(v);
#else
void KviDccBroker::activeVideoExecute(KviDccBox *,KviDccDescriptor *)
{
#endif
}


///////////////////////////////////////////////////////////////////////////////
// PASSIVE VIDEO
///////////////////////////////////////////////////////////////////////////////

#ifndef COMPILE_DISABLE_DCC_VIDEO
void KviDccBroker::passiveVideoExecute(KviDccDescriptor * dcc)
{
	KviStr tmp(KviStr::Format,"dcc: video %s@%s:%s",dcc->szNick.toUtf8().data(),dcc->szIp.toUtf8().data(),dcc->szPort.toUtf8().data());
	KviDccVideo * v = new KviDccVideo(dcc->console()->frame(),dcc,tmp.ptr());
	//#warning "Create minimized dcc video ?... or maybe it's too much ? :)"
	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChat);
	dcc->console()->frame()->addWindow(v,!bMinimized);
	if(bMinimized)v->minimize();
	m_pDccWindowList->append(v);
}
#endif


///////////////////////////////////////////////////////////////////////////////
// ACTIVE CANVAS
///////////////////////////////////////////////////////////////////////////////

#ifdef COMPILE_DCC_CANVAS
void KviDccBroker::activeCanvasManage(KviDccDescriptor * dcc)
{
	if(!dcc->bAutoAccept)
	{
		// need confirmation
		QString tmp = __tr2qs_ctx( \
					"<b>%1 [%2@%3]</b> requests a<br>" \
					"<b>Direct Client Connection</b> in <b>CANVAS</b> mode.<br>" \
					"The connection target will be host <b>%4</b> on port <b>%5</b><br>" \
				,"dcc" \
			).arg(dcc->szNick, dcc->szUser, dcc->szHost, dcc->szIp, dcc->szPort);

		KviDccAcceptBox * box = new KviDccAcceptBox(this,dcc,tmp,__tr2qs_ctx("DCC CANVAS request","dcc"));
		m_pBoxList->append(box);
		connect(box,SIGNAL(accepted(KviDccBox *,KviDccDescriptor *)),
				this,SLOT(activeCanvasExecute(KviDccBox *,KviDccDescriptor *)));
		connect(box,SIGNAL(rejected(KviDccBox *,KviDccDescriptor *)),
				this,SLOT(cancelDcc(KviDccBox *,KviDccDescriptor *)));
		box->show();
	} else {
		// auto accept
		activeCanvasExecute(0,dcc);
	}
}
#endif

#ifdef COMPILE_DCC_CANVAS
void KviDccBroker::activeCanvasExecute(KviDccBox *box,KviDccDescriptor * dcc)
{
	if(box)box->forgetDescriptor();

	if(!g_pApp->windowExists(dcc->console()))
	{
		// rebind to the first available console....
		dcc->setConsole(g_pApp->activeConsole());
	}

	KviStr tmp(KviStr::Format,"dcc: canvas %s@%s:%s",dcc->szNick.toUtf8().data(),dcc->szIp.toUtf8().data(),dcc->szPort.toUtf8().data());
	KviDccCanvas * cnv = new KviDccCanvas(dcc->console()->frame(),dcc,tmp.ptr());

	//#warning "This option should be dedicated to Dcc Canvas!....for now we are using the DccChat options"
	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : \
			(KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChat) || \
				(dcc->bAutoAccept && KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChatWhenAutoAccepted)));

	dcc->console()->frame()->addWindow(cnv,!bMinimized);
	if(bMinimized)cnv->minimize();

	m_pDccWindowList->append(cnv);
#else
void KviDccBroker::activeCanvasExecute(KviDccBox *,KviDccDescriptor *)
{
#endif
}

///////////////////////////////////////////////////////////////////////////////
// PASSIVE CANVAS
///////////////////////////////////////////////////////////////////////////////
#ifdef COMPILE_DCC_CANVAS
void KviDccBroker::passiveCanvasExecute(KviDccDescriptor * dcc)
{
	KviStr tmp(KviStr::Format,"dcc: canvas %s@%s:%s",dcc->szNick.toUtf8().data(),dcc->szIp.toUtf8().data(),dcc->szPort.toUtf8().data());
	KviDccCanvas * cnv = new KviDccCanvas(dcc->console()->frame(),dcc,tmp.ptr());
	//#warning "This option should be dedicated to Dcc Canvas!....for now we are using the DccChat options"
	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChat);
	dcc->console()->frame()->addWindow(cnv,!bMinimized);
	if(bMinimized)cnv->minimize();
	m_pDccWindowList->append(cnv);
}
#endif

///////////////////////////////////////////////////////////////////////////////
// SEND
///////////////////////////////////////////////////////////////////////////////

void KviDccBroker::recvFileManage(KviDccDescriptor * dcc)
{
	if(dcc->bIsIncomingAvatar)
	{
		bool bOk;
		quint64 size = dcc->szFileSize.toULongLong(&bOk);
		if(bOk) {
			if(size>=KVI_OPTION_UINT(KviOption_uintMaximumRequestedAvatarSize)) {
				cancelDcc(0,dcc);
				return;
			}
		}
	}

	if(!dcc->bAutoAccept)
	{
		// need confirmation
		QString tmp;

		if(dcc->bActive)
		{
			// Normal active send: we will be connecting
			tmp = __tr2qs_ctx( \
						"<b>%1 [%2@%3]</b> " \
						"wants to send you the file " \
						"'<b>%4</b>', " \
						"<b>%5</b> large.<br>" \
						"The connection target will be host <b>%6</b> on port <b>%7</b><br>" \
					,"dcc" \
				).arg(dcc->szNick, dcc->szUser, dcc->szHost, dcc->szFileName,
					KviQString::makeSizeReadable(dcc->szFileSize.toULongLong()),
					dcc->szIp, dcc->szPort);

		} else {
			// passive: we will be listening!
			tmp = __tr2qs_ctx( \
						"<b>%1 [%2@%3]</b> "
						"wants to send you the file " \
						"'<b>%4</b>', " \
						"<b>%5</b> large.<br>" \
						"You will be the passive side of the connection.<br>" \
					,"dcc" \
				).arg(dcc->szNick, dcc->szUser, dcc->szHost, dcc->szFileName,
				      KviQString::makeSizeReadable(dcc->szFileSize.toULongLong()));
		}

		if(dcc->bIsIncomingAvatar)
		{
			tmp += __tr2qs_ctx( \
					"<center><b>Note:</b></center>" \
					"The file appears to be an avatar that you have requested. " \
					"You should not change its filename. " \
					"Save it in a location where KVIrc can find it, such as " \
					"the 'avatars', 'incoming', or 'pics' directories, " \
					"your home directory, or the save directory for the incoming file type. " \
					"The default save path will probably work. " \
					"You can instruct KVIrc to accept incoming avatars automatically " \
					"by setting the option <tt>boolAutoAcceptIncomingAvatars</tt> to true.<br>" \
				,"dcc" \
				);
		}

		//#warning "Maybe remove the pending avatar if rejected ?"

		QString title = __tr2qs_ctx("DCC %1 Request - KVIrc","dcc").arg(dcc->szType);

		KviDccAcceptBox * box = new KviDccAcceptBox(this,dcc,tmp,title);
		m_pBoxList->append(box);
		connect(box,SIGNAL(accepted(KviDccBox *,KviDccDescriptor *)),
				this,SLOT(chooseSaveFileName(KviDccBox *,KviDccDescriptor *)));
		connect(box,SIGNAL(rejected(KviDccBox *,KviDccDescriptor *)),
				this,SLOT(cancelDcc(KviDccBox *,KviDccDescriptor *)));
		box->show();
	} else {
		// auto accept
		if(_OUTPUT_VERBOSE)
		{
			dcc->console()->output(KVI_OUT_DCCMSG,__tr2qs_ctx("Auto-accepting DCC %Q request from %Q!%Q@%Q for file %Q","dcc"),
				&(dcc->szType),&(dcc->szNick),&(dcc->szUser),
				&(dcc->szHost),&(dcc->szFileName));
		}
		chooseSaveFileName(0,dcc);
	}
}

void KviDccBroker::chooseSaveFileName(KviDccBox *box,KviDccDescriptor *dcc)
{
	if(box)box->forgetDescriptor();

	// Lookup the suggested save directory

	dcc->szLocalFileName = "";

	if(dcc->bIsIncomingAvatar)g_pApp->getLocalKvircDirectory(dcc->szLocalFileName,KviApp::Avatars);
	else {

		if(KVI_OPTION_BOOL(KviOption_boolUseIncomingDccMediaTypeSavePath))
		{
			g_pMediaManager->lock();
			if(KviMediaType * mt = g_pMediaManager->findMediaType(dcc->szFileName.toUtf8().data(),false))
			{
				if(mt->szSavePath.hasData())
				{
					if(KviFileUtils::directoryExists(mt->szSavePath.ptr()))dcc->szLocalFileName = mt->szSavePath;
					else {
						if(KviFileUtils::makeDir(mt->szSavePath.ptr()))dcc->szLocalFileName = mt->szSavePath;
					}
					if(KVI_OPTION_BOOL(KviOption_boolSortReceivedByDccFilesByNicks))
					{
						KviQString::ensureLastCharIs(dcc->szLocalFileName,KVI_PATH_SEPARATOR_CHAR);
						QString cleanNick = dcc->szNick;
						KviFileUtils::cleanFileName(cleanNick);
						dcc->szLocalFileName.append(cleanNick);
						KviFileUtils::adjustFilePath(dcc->szLocalFileName);
					}
					KviFileUtils::makeDir(dcc->szLocalFileName);
				}
			}
			g_pMediaManager->unlock();
		}

		if(dcc->szLocalFileName.isEmpty())
		{
			g_pApp->getLocalKvircDirectory(dcc->szLocalFileName,KviApp::Incoming);
			if(KVI_OPTION_BOOL(KviOption_boolSortReceivedByDccFilesByNicks))
			{
				KviQString::ensureLastCharIs(dcc->szLocalFileName,KVI_PATH_SEPARATOR_CHAR);
				QString cleanNick = dcc->szNick;
				KviFileUtils::cleanFileName(cleanNick);
				dcc->szLocalFileName.append(cleanNick);
				KviFileUtils::adjustFilePath(dcc->szLocalFileName);
				KviFileUtils::makeDir(dcc->szLocalFileName);
			}
		}
	}
	KviFileUtils::adjustFilePath(dcc->szLocalFileName);
	KviQString::ensureLastCharIs(dcc->szLocalFileName,KVI_PATH_SEPARATOR_CHAR);

	if(!(dcc->bAutoAccept))
	{
		QString cleanFilename = dcc->szFileName;
		KviFileUtils::cleanFileName(cleanFilename);
		dcc->szLocalFileName.append(cleanFilename);
		if(KviFileDialog::askForSaveFileName(dcc->szLocalFileName,
			__tr2qs_ctx("Choose Files to Save - KVIrc","dcc"),dcc->szLocalFileName))
		{
			renameOverwriteResume(0,dcc);
		} else {
			cancelDcc(dcc);
		}
	} else {
		// auto accept
		// WE choose the filename
		QString cleanFilename = dcc->szFileName;
		KviFileUtils::cleanFileName(cleanFilename);
		dcc->szLocalFileName.append(cleanFilename);

		if(_OUTPUT_VERBOSE)
		{
			dcc->console()->output(KVI_OUT_DCCMSG,__tr2qs_ctx("Auto-saving DCC %Q file %Q as \r![!dbl]play $0\r%Q\r","dcc"),
				&(dcc->szType),&(dcc->szFileName),&(dcc->szLocalFileName));
		}

		renameOverwriteResume(0,dcc);
	}
}

void KviDccBroker::renameOverwriteResume(KviDccBox *box,KviDccDescriptor * dcc)
{
	if(box)box->forgetDescriptor();

	// Check if file exists
	QFileInfo fi(dcc->szLocalFileName);
	if(fi.exists() && (fi.size() > 0)) // 0 byte files are senseless for us
	{
		dcc->szLocalFileSize.setNum(fi.size());

		bool bOk;
		quint64 iRemoteSize = dcc->szFileSize.toULongLong(&bOk);
		if(!bOk)
			iRemoteSize = 0;

		if(!dcc->bAutoAccept)
		{
			QString tmp;
			bool bDisableResume = false;

			if(
				(!bOk) ||                          // remote size is unknown
				(iRemoteSize > (quint64)fi.size()) // or it is larger than the actual size on disk
			) 
			{
				tmp = __tr2qs_ctx( \
							"The file '<b>%1</b>' already exists " \
							"and is <b>%2</b> large.<br>" \
							"Do you wish to<br>" \
							"<b>auto-rename</b> the new file,<br>" \
							"<b>overwrite</b> the existing file, or<br> " \
							"<b>resume</b> an incomplete download?" \
						,"dcc" \
					).arg(dcc->szLocalFileName, KviQString::makeSizeReadable(fi.size()));
			} else {
				bDisableResume = true;
				// the file on disk is larger or equal to the remote one
				tmp = __tr2qs_ctx( \
							"The file '<b>%1</b>' already exists " \
							"and is larger than the offered one.<br>" \
							"Do you wish to<br>" \
							"<b>auto-rename</b> the new file, or<br>" \
							"<b>overwrite</b> the existing file ?" \
						,"dcc" \
					).arg(dcc->szLocalFileName);
			}

			KviDccRenameBox * box = new KviDccRenameBox(this,dcc,tmp,bDisableResume);
			m_pBoxList->append(box);
			connect(box,SIGNAL(renameSelected(KviDccBox *,KviDccDescriptor *)),
					this,SLOT(renameDccSendFile(KviDccBox *,KviDccDescriptor *)));
			connect(box,SIGNAL(overwriteSelected(KviDccBox *,KviDccDescriptor *)),
					this,SLOT(recvFileExecute(KviDccBox *,KviDccDescriptor *)));
			connect(box,SIGNAL(cancelSelected(KviDccBox *,KviDccDescriptor *)),
					this,SLOT(cancelDcc(KviDccBox *,KviDccDescriptor *)));
			box->show();
			return;
		}

		// auto resume ?
		if(
				KVI_OPTION_BOOL(KviOption_boolAutoResumeDccSendWhenAutoAccepted) &&
				(bOk) &&                              // only if the remote size is really known
				(iRemoteSize > (quint64)fi.size()) && // only if the remote size is larger than the local size
				(!KviDccFileTransfer::nonFailedTransferWithLocalFileName(dcc->szLocalFileName)) // only if there is no transfer with this local file name yet
			) 
		{
			// yep, auto resume...
			dcc->bResume = true;
			recvFileExecute(0,dcc);
		} else if(iRemoteSize == (quint64)fi.size())
		{ 
			dcc->console()->output(KVI_OUT_DCCMSG,"Transfer aborted: file %Q already completed",&(dcc->szLocalFileName));
			cancelDcc(0,dcc);
		} else {
			// otherwise auto rename
			renameDccSendFile(0,dcc);
		}
		return;
	}
	
	dcc->szLocalFileSize = "0";

	// everything OK
	recvFileExecute(0,dcc);
}

void KviDccBroker::renameDccSendFile(KviDccBox *box,KviDccDescriptor * dcc)
{
	if(box)box->forgetDescriptor();


	if(QFileInfo(dcc->szLocalFileName).exists())
	{
		KviStr szOrig = dcc->szLocalFileName;
		int i = 1;
		do {
			KviStr szNum;
			szNum.setNum(i);
			int idx = szOrig.findLastIdx('.');
			if(idx != -1)
			{
				dcc->szLocalFileName = szOrig.left(idx);
				dcc->szLocalFileName += ".";
				dcc->szLocalFileName += szNum;
				dcc->szLocalFileName += szOrig.right(szOrig.len() - idx);
			} else {
				dcc->szLocalFileName = szOrig;
				dcc->szLocalFileName += ".";
				dcc->szLocalFileName += szNum;
			}
			i++;
		} while(QFileInfo(dcc->szLocalFileName).exists());

		if(_OUTPUT_VERBOSE)
		{
			dcc->console()->output(KVI_OUT_DCCMSG,__tr2qs_ctx("File %s exists, auto-renaming to %Q","dcc"),
				szOrig.ptr(),&(dcc->szLocalFileName));
		}
	}

	dcc->szLocalFileSize = "0"; // 0 for sure

	recvFileExecute(0,dcc);
}

void KviDccBroker::recvFileExecute(KviDccBox *box,KviDccDescriptor * dcc)
{
	if(box)box->forgetDescriptor();

	if(!g_pApp->windowExists(dcc->console()))
	{
		// rebind to the first available console....
		dcc->setConsole(g_pApp->activeConsole());
	}

	//KviDccSend * send = new KviDccSend(dcc->console()->frame(),dcc,tmp.ptr());
	KviDccFileTransfer * send = new KviDccFileTransfer(dcc);

	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : \
			(KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccSend) || \
				(dcc->bAutoAccept && KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccSendWhenAutoAccepted)));

	send->invokeTransferWindow(dcc->console(),bMinimized,bMinimized);
}


void KviDccBroker::sendFileManage(KviDccDescriptor * dcc)
{
	QStringList filenames;
	if(KviFileDialog::askForOpenFileNames(filenames,__tr2qs_ctx("Choose Files to Send - KVIrc","dcc"),""))
	{
		if(filenames.count() > 0)
		{
			KviDccDescriptor * d;
			QStringList::Iterator it=filenames.begin();
			while(it != filenames.end())
			{
				d = new KviDccDescriptor(*dcc);
				d->szLocalFileName = *(it);
				d->szLocalFileName.trimmed();
				++it;
				if(d->szLocalFileName.isEmpty())
					cancelDcc(d);
				else
					sendFileExecute(0,d);
			}
			delete dcc;
		}
	} else {
		cancelDcc(dcc);
	}
}

void KviDccBroker::sendFileExecute(KviDccBox * box,KviDccDescriptor *dcc)
{
	if(box)box->forgetDescriptor();

	if(!g_pApp->windowExists(dcc->console()))
	{
		// rebind to the first available console....
		dcc->setConsole(g_pApp->activeConsole());
	}

	QFileInfo fi(dcc->szLocalFileName);
	if(!(fi.exists() && fi.isReadable() && (fi.isFile()) && (fi.size() > 0)))
	{
		dcc->console()->output(KVI_OUT_DCCERROR,__tr2qs_ctx("Can't open file %Q for reading","dcc"),
			&(dcc->szLocalFileName));
		delete dcc;
		return;
	}
	
	dcc->szFileName = dcc->szLocalFileName;
	dcc->szFileName = QFileInfo(dcc->szFileName).fileName();

	dcc->szLocalFileSize.setNum(fi.size());

	KviDccFileTransfer * send = new KviDccFileTransfer(dcc);

	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccSend);

	send->invokeTransferWindow(dcc->console(),bMinimized,bMinimized);
}

bool KviDccBroker::canUnload()
{
	if(m_pBoxList)
	{
		if((m_pBoxList->count() != 0) ||
			(m_pDccWindowList->count() != 0) ||
			(KviDccFileTransfer::transferCount() != 0))return false;
	} // else in the destructor anyway (going to die)
	return true;
}

bool KviDccBroker::handleResumeAccepted(const char * filename,const char * port,const char * szZeroPortTag)
{
	return KviDccFileTransfer::handleResumeAccepted(filename,port,szZeroPortTag);
}

bool KviDccBroker::handleResumeRequest(KviDccRequest * dcc,const char * filename,const char * port,unsigned long filePos,const char * szZeroPortTag)
{
	//debug("HANDLE %s %s %u %s",filename,port,filePos,szZeroPortTag);
	// the zeroPOrtTag is nonempty here only if port == 0
	if(kvi_strEqualCI("0",port) && szZeroPortTag)
	{
		// zero port resume request (we have sent out a DCC SEND <filename> <fakeip> 0 <tag>
		KviDccZeroPortTag * t = findZeroPortTag(QString(szZeroPortTag));
		if(t)
		{
			// valid zero port resume request
			if(filePos >= t->m_uFileSize)
				return false; // invalid resume size
				
			// ok!
			t->m_uResumePosition = filePos;

			KviStr szBuffy;
			KviServerParser::encodeCtcpParameter(filename,szBuffy);

			dcc->ctcpMsg->msg->console()->connection()->sendFmtData(
					"PRIVMSG %s :%cDCC ACCEPT %s %s %u %s%c",
					dcc->ctcpMsg->msg->console()->connection()->encodeText(dcc->ctcpMsg->pSource->nick()).data(),
					0x01,
					szBuffy.ptr(),
					port,
					filePos,
					szZeroPortTag,
					0x01
				);

			return true;
		}
	}
	//debug("NOT A ZeRO PORT");

	return KviDccFileTransfer::handleResumeRequest(filename,port,filePos);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_broker.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
