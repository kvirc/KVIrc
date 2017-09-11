//=============================================================================
//
//   File : DccBroker.cpp
//   Creation date : Tue Sep 19 09 2000 10:21:54 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#include "DccBroker.h"
#include "DccDialog.h"
#include "DccChatWindow.h"
#include "DccFileTransfer.h"
#include "DccVoiceWindow.h"

#ifndef COMPILE_DISABLE_DCC_VIDEO
#include "DccVideoWindow.h"
#endif

#ifdef COMPILE_DCC_CANVAS
#include "DccCanvasWindow.h"
#endif

#include "KviApplication.h"
#include "KviMainWindow.h"
#include "KviLocale.h"
#include "KviFileDialog.h"
#include "KviOptions.h"
#include "KviConsoleWindow.h"
#include "KviFileUtils.h"
#include "kvi_out.h"
#include "KviMediaManager.h"
#include "KviIrcConnection.h"
#include "KviSharedFilesManager.h"
#include "KviIrcMessage.h"

#include <QFileInfo>
#include <QString>

extern KVIRC_API KviMediaManager * g_pMediaManager;
extern KVIRC_API KviSharedFilesManager * g_pSharedFilesManager;

//#warning "The broker might lookup the remote host name"

DccBroker::DccBroker()
    : QObject(nullptr)
{
	setObjectName("dcc_broker");
	DccFileTransfer::init();

	m_pBoxList = new KviPointerList<DccDialog>;
	m_pBoxList->setAutoDelete(false);

	m_pDccWindowList = new KviPointerList<KviWindow>;
	m_pDccWindowList->setAutoDelete(false);

	m_pZeroPortTags = new KviPointerHashTable<QString, KviDccZeroPortTag>(17);
	m_pZeroPortTags->setAutoDelete(true);
}

DccBroker::~DccBroker()
{
	delete m_pZeroPortTags;
	while(m_pBoxList->first())
		delete m_pBoxList->first();
	delete m_pBoxList;
	m_pBoxList = nullptr;
	while(m_pDccWindowList->first())
		delete m_pDccWindowList->first();
	delete m_pDccWindowList;
	DccFileTransfer::done();
}

KviDccZeroPortTag * DccBroker::addZeroPortTag()
{
	static unsigned int g_uNextZeroPortTag = 0;
	g_uNextZeroPortTag++;
	KviDccZeroPortTag * t = new KviDccZeroPortTag;
	t->m_tTimestamp = QDateTime::currentDateTime();
	t->m_szTag.setNum(g_uNextZeroPortTag);
	//t->m_szTag.prepend("mIrc-zero-port-");
	t->m_uResumePosition = 0;
	// FIXME: we should clear this dict if it grows too high....
	m_pZeroPortTags->insert(t->m_szTag, t);
	return t;
}

KviDccZeroPortTag * DccBroker::findZeroPortTag(const QString & szTag)
{
	KviDccZeroPortTag * t = m_pZeroPortTags->find(szTag);
	if(!t)
		return nullptr;
	if(t->m_tTimestamp.secsTo(QDateTime::currentDateTime()) > 180)
	{
		// too late man...
		m_pZeroPortTags->remove(szTag);
		return nullptr;
	}
	return t;
}

void DccBroker::removeZeroPortTag(const QString & szTag)
{
	m_pZeroPortTags->remove(szTag);
}

unsigned int DccBroker::dccBoxCount()
{
	return m_pBoxList->count();
}

void DccBroker::unregisterDccWindow(KviWindow * wnd)
{
	m_pDccWindowList->removeRef(wnd);
}

void DccBroker::unregisterDccBox(DccDialog * box)
{
	//qDebug("Forgetting box %d",box);
	m_pBoxList->removeRef(box);
}

void DccBroker::cancelDcc(DccDescriptor * dcc)
{
	delete dcc;
	dcc = nullptr;
}

void DccBroker::cancelDcc(DccDialog * box, DccDescriptor * dcc)
{
	if(box)
		box->forgetDescriptor();
	delete dcc;
	dcc = nullptr;
}

//
// RSEND
//

void DccBroker::rsendManage(DccDescriptor * dcc)
{
	// We need the filename...
	QFileInfo fi(dcc->szLocalFileName);
	if(fi.exists())
		rsendExecute(nullptr, dcc);
	else
		rsendAskForFileName(dcc);
}

void DccBroker::rsendAskForFileName(DccDescriptor * dcc)
{
	QStringList filenames;
	if(
	    KviFileDialog::askForOpenFileNames(
	        filenames,
	        __tr2qs_ctx("Choose Files to Send - KVIrc", "dcc"),
	        QString(),
	        QString(),
	        false,
	        true,
	        g_pMainWindow))
	{
		if(filenames.count() > 0)
		{
			DccDescriptor * d;
			QStringList::Iterator it = filenames.begin();
			while(it != filenames.end())
			{
				d = new DccDescriptor(*dcc);
				d->szLocalFileName = *(it);
				d->szLocalFileName = d->szLocalFileName.trimmed();
				++it;
				if(d->szLocalFileName.isEmpty())
					cancelDcc(d);
				else
					rsendExecute(d);
			}
			delete dcc;
		}
	}
	else
	{
		cancelDcc(dcc);
	}
}

void DccBroker::rsendExecute(DccDescriptor * dcc)
{
	if(!g_pApp->windowExists(dcc->console()))
	{
		// No way...we NEED the right IRC context...
		g_pApp->activeConsole()->output(KVI_OUT_DCCERROR,
		    __tr2qs_ctx("Can't send DCC %Q request to %Q: IRC connection has been terminated", "dcc"),
		    &(dcc->szType), &(dcc->szNick));
		delete dcc;
		return;
	}

	// Ok...we need the file to exist
	QFileInfo fi(dcc->szLocalFileName);
	if(!(fi.exists() && fi.isReadable() && (fi.isFile()) && (fi.size() > 0)))
	{
		dcc->console()->output(KVI_OUT_DCCERROR, __tr2qs_ctx("Can't open file %Q for reading", "dcc"),
		    &(dcc->szLocalFileName));
		delete dcc;
		return;
	}

	dcc->szFileName = dcc->szLocalFileName;
	dcc->szFileName = QFileInfo(dcc->szFileName).fileName();

	QString fName = dcc->szFileName;
	fName.replace(' ', "\\040"); // be cool :)

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
		dcc->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC %s %s 2130706433 0 %s %s%c",
		    dcc->console()->connection()->encodeText(dcc->szNick).data(),
		    0x01,
		    dcc->console()->connection()->encodeText(dcc->szType).data(),
		    dcc->console()->connection()->encodeText(fName).data(),
		    dcc->console()->connection()->encodeText(QString::number(fi.size())).data(),
		    dcc->console()->connection()->encodeText(szTag).data(),
		    0x01);
	}
	else
	{
		dcc->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC %s %s %s%c",
		    dcc->console()->connection()->encodeText(dcc->szNick).data(),
		    0x01,
		    dcc->console()->connection()->encodeText(dcc->szType).data(),
		    dcc->console()->connection()->encodeText(fName).data(),
		    dcc->console()->connection()->encodeText(QString::number(fi.size())).data(),
		    0x01);
		szTag = dcc->szFileName;
	}

	// now add a file offer, so he we will accept it automatically
	// 120 secs is a reasonable timeout
	QString szMask = dcc->szNick;
	szMask += "!*@*";

	g_pSharedFilesManager->addSharedFile(szTag, dcc->szLocalFileName, szMask, 120);

	delete dcc;
}

void DccBroker::rsendExecute(DccDialog * box, DccDescriptor * dcc)
{
	if(box)
		box->forgetDescriptor();
	rsendExecute(dcc);
}

//
// DCC CHAT
//

void DccBroker::handleChatRequest(DccDescriptor * dcc)
{
	if(!dcc->bAutoAccept)
	{
		// FIXME: better message ? Secure Direct Client Connection...eventually
		// need confirmation
		QString tmp = __tr2qs_ctx(
		                  "<b>%1 [%2@%3]</b> requests a "
		                  "<b>Direct Client Connection</b> in <b>%4</b> mode.<br>",
		                  "dcc")
		                  .arg(dcc->szNick, dcc->szUser, dcc->szHost, dcc->szType);

#ifdef COMPILE_SSL_SUPPORT
		if(dcc->bIsSSL)
			tmp += __tr2qs_ctx("The connection will be secured using SSL.<br>", "dcc");
#endif

		if(dcc->isZeroPortRequest())
		{
			tmp += __tr2qs_ctx(
			    "You will be the passive side of the connection.<br>", "dcc");
		}
		else
		{
			tmp += __tr2qs_ctx(
			           "The connection target will be host <b>%1</b> on port <b>%2</b><br>", "dcc")
			           .arg(dcc->szIp, dcc->szPort);
		}

		QString caption = __tr2qs_ctx("DCC %1 Request - KVIrc", "dcc").arg(dcc->szType);

		DccAcceptDialog * box = new DccAcceptDialog(this, dcc, tmp, caption);

		m_pBoxList->append(box);
		connect(box, SIGNAL(accepted(DccDialog *, DccDescriptor *)),
		    this, SLOT(executeChat(DccDialog *, DccDescriptor *)));
		connect(box, SIGNAL(rejected(DccDialog *, DccDescriptor *)),
		    this, SLOT(cancelDcc(DccDialog *, DccDescriptor *)));
		box->show();
	}
	else
	{
		// auto accept
		executeChat(nullptr, dcc);
	}
}

void DccBroker::executeChat(DccDialog * box, DccDescriptor * dcc)
{
	if(box)
		box->forgetDescriptor();

	if(!g_pApp->windowExists(dcc->console()))
	{
		// rebind to the first available console....
		dcc->setConsole(g_pApp->activeConsole());
	}

	KviCString szSubProto = dcc->szType;
	szSubProto.toLower();

	QString tmp = QString("DCC: %1 %2@%3:%4").arg(szSubProto.ptr(), dcc->szNick, dcc->szIp, dcc->szPort);
	DccChatWindow * chat = new DccChatWindow(dcc, tmp.toUtf8().data());

	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : (KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChat) || (dcc->bAutoAccept && KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChatWhenAutoAccepted)));

	g_pMainWindow->addWindow(chat, !bMinimized);
	m_pDccWindowList->append(chat);
}

//
// ACTIVE VOICE
//

void DccBroker::activeVoiceManage(DccDescriptor * dcc)
{
	if(!dcc->bAutoAccept)
	{
		// need confirmation
		QString tmp = __tr2qs_ctx(
		                  "<b>%1 [%2@%3]</b> requests a<br>"
		                  "<b>Direct Client Connection</b> in <b>VOICE</b> mode.<br>"
		                  "The connection target will be host <b>%4</b> on port <b>%5</b><br>",
		                  "dcc")
		                  .arg(dcc->szNick, dcc->szUser, dcc->szHost, dcc->szIp, dcc->szPort);

		DccAcceptDialog * box = new DccAcceptDialog(this, dcc, tmp, __tr2qs_ctx("DCC VOICE request", "dcc"));
		m_pBoxList->append(box);
		connect(box, SIGNAL(accepted(DccDialog *, DccDescriptor *)),
		    this, SLOT(activeVoiceExecute(DccDialog *, DccDescriptor *)));
		connect(box, SIGNAL(rejected(DccDialog *, DccDescriptor *)),
		    this, SLOT(cancelDcc(DccDialog *, DccDescriptor *)));
		box->show();
	}
	else
	{
		// auto accept
		activeVoiceExecute(nullptr, dcc);
	}
}

void DccBroker::activeVoiceExecute(DccDialog * box, DccDescriptor * dcc)
{
	if(box)
		box->forgetDescriptor();

	if(!g_pApp->windowExists(dcc->console()))
	{
		// rebind to the first available console....
		dcc->setConsole(g_pApp->activeConsole());
	}

	KviCString tmp(KviCString::Format, "DCC: voice %s@%s:%s", dcc->szNick.toUtf8().data(), dcc->szIp.toUtf8().data(), dcc->szPort.toUtf8().data());
	DccVoiceWindow * v = new DccVoiceWindow(dcc, tmp.ptr());

	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : (KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccVoice) || (dcc->bAutoAccept && KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccVoiceWhenAutoAccepted)));

	g_pMainWindow->addWindow(v, !bMinimized);

	m_pDccWindowList->append(v);
}

//
// PASSIVE VOICE
//

void DccBroker::passiveVoiceExecute(DccDescriptor * dcc)
{
	KviCString tmp(KviCString::Format, "DCC: voice %s@%s:%s", dcc->szNick.toUtf8().data(), dcc->szIp.toUtf8().data(), dcc->szPort.toUtf8().data());
	DccVoiceWindow * v = new DccVoiceWindow(dcc, tmp.ptr());
	//#warning "Create minimized dcc voice ?... or maybe it's too much ? :)"
	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChat);
	g_pMainWindow->addWindow(v, !bMinimized);
	m_pDccWindowList->append(v);
}

//
// ACTIVE VIDEO
//

#ifndef COMPILE_DISABLE_DCC_VIDEO
void DccBroker::activeVideoManage(DccDescriptor * dcc)
{
	if(!dcc->bAutoAccept)
	{
		// need confirmation
		QString tmp = __tr2qs_ctx(
		                  "<b>%1 [%2@%3]</b> requests a<br>"
		                  "<b>Direct Client Connection</b> in <b>VIDEO</b> mode.<br>"
		                  "The connection target will be host <b>%4</b> on port <b>%5</b><br>",
		                  "dcc")
		                  .arg(dcc->szNick, dcc->szUser, dcc->szHost, dcc->szIp, dcc->szPort);

		DccAcceptDialog * box = new DccAcceptDialog(this, dcc, tmp, __tr2qs_ctx("DCC VIDEO request", "dcc"));
		m_pBoxList->append(box);
		connect(box, SIGNAL(accepted(DccDialog *, DccDescriptor *)),
		    this, SLOT(activeVideoExecute(DccDialog *, DccDescriptor *)));
		connect(box, SIGNAL(rejected(DccDialog *, DccDescriptor *)),
		    this, SLOT(cancelDcc(DccDialog *, DccDescriptor *)));
		box->show();
	}
	else
	{
		// auto accept
		activeVideoExecute(0, dcc);
	}
}

void DccBroker::activeVideoExecute(DccDialog * box, DccDescriptor * dcc)
{
	if(box)
		box->forgetDescriptor();

	if(!g_pApp->windowExists(dcc->console()))
	{
		// rebind to the first available console....
		dcc->setConsole(g_pApp->activeConsole());
	}

	KviCString tmp(KviCString::Format, "DCC: video %s@%s:%s", dcc->szNick.toUtf8().data(), dcc->szIp.toUtf8().data(), dcc->szPort.toUtf8().data());
	DccVideoWindow * v = new DccVideoWindow(dcc, tmp.ptr());

	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : (KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccVideo) || (dcc->bAutoAccept && KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccVideoWhenAutoAccepted)));

	g_pMainWindow->addWindow(v, !bMinimized);

	m_pDccWindowList->append(v);
#else
void DccBroker::activeVideoExecute(DccDialog *, DccDescriptor *)
{
#endif
}

//
// PASSIVE VIDEO
//

#ifndef COMPILE_DISABLE_DCC_VIDEO
void DccBroker::passiveVideoExecute(DccDescriptor * dcc)
{
	KviCString tmp(KviCString::Format, "DCC: video %s@%s:%s", dcc->szNick.toUtf8().data(), dcc->szIp.toUtf8().data(), dcc->szPort.toUtf8().data());
	DccVideoWindow * v = new DccVideoWindow(dcc, tmp.ptr());
	//#warning "Create minimized dcc video ?... or maybe it's too much ? :)"
	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChat);
	g_pMainWindow->addWindow(v, !bMinimized);
	m_pDccWindowList->append(v);
#else
void DccBroker::passiveVideoExecute(DccDescriptor *)
{
#endif
}

//
// ACTIVE CANVAS
//

#ifdef COMPILE_DCC_CANVAS
void DccBroker::activeCanvasManage(DccDescriptor * dcc)
{
	if(!dcc->bAutoAccept)
	{
		// need confirmation
		QString tmp = __tr2qs_ctx(
		                  "<b>%1 [%2@%3]</b> requests a<br>"
		                  "<b>Direct Client Connection</b> in <b>CANVAS</b> mode.<br>"
		                  "The connection target will be host <b>%4</b> on port <b>%5</b><br>",
		                  "dcc")
		                  .arg(dcc->szNick, dcc->szUser, dcc->szHost, dcc->szIp, dcc->szPort);

		DccAcceptDialog * box = new DccAcceptDialog(this, dcc, tmp, __tr2qs_ctx("DCC CANVAS request", "dcc"));
		m_pBoxList->append(box);
		connect(box, SIGNAL(accepted(DccDialog *, DccDescriptor *)),
		    this, SLOT(activeCanvasExecute(DccDialog *, DccDescriptor *)));
		connect(box, SIGNAL(rejected(DccDialog *, DccDescriptor *)),
		    this, SLOT(cancelDcc(DccDialog *, DccDescriptor *)));
		box->show();
	}
	else
	{
		// auto accept
		activeCanvasExecute(0, dcc);
	}
}
#endif

#ifdef COMPILE_DCC_CANVAS
void DccBroker::activeCanvasExecute(DccDialog * box, DccDescriptor * dcc)
{
	if(box)
		box->forgetDescriptor();

	if(!g_pApp->windowExists(dcc->console()))
	{
		// rebind to the first available console....
		dcc->setConsole(g_pApp->activeConsole());
	}

	KviCString tmp(KviCString::Format, "DCC: canvas %s@%s:%s", dcc->szNick.toUtf8().data(), dcc->szIp.toUtf8().data(), dcc->szPort.toUtf8().data());
	DccCanvasWindow * cnv = new DccCanvasWindow(dcc, tmp.ptr());

	//#warning "This option should be dedicated to Dcc Canvas!....for now we are using the DccChat options"
	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : (KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChat) || (dcc->bAutoAccept && KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChatWhenAutoAccepted)));

	g_pMainWindow->addWindow(cnv, !bMinimized);

	m_pDccWindowList->append(cnv);
#else
void DccBroker::activeCanvasExecute(DccDialog *, DccDescriptor *)
{
#endif
}

//
// PASSIVE CANVAS
//

#ifdef COMPILE_DCC_CANVAS
void DccBroker::passiveCanvasExecute(DccDescriptor * dcc)
{
	KviCString tmp(KviCString::Format, "DCC: canvas %s@%s:%s", dcc->szNick.toUtf8().data(), dcc->szIp.toUtf8().data(), dcc->szPort.toUtf8().data());
	DccCanvasWindow * cnv = new DccCanvasWindow(dcc, tmp.ptr());
	//#warning "This option should be dedicated to Dcc Canvas!....for now we are using the DccChat options"
	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccChat);
	g_pMainWindow->addWindow(cnv, !bMinimized);
	m_pDccWindowList->append(cnv);
}
#endif

//
// SEND
//

void DccBroker::recvFileManage(DccDescriptor * dcc)
{
	if(dcc->bIsIncomingAvatar)
	{
		bool bOk;
		quint64 size = dcc->szFileSize.toULongLong(&bOk);
		if(bOk)
		{
			if(size >= KVI_OPTION_UINT(KviOption_uintMaximumRequestedAvatarSize))
			{
				cancelDcc(nullptr, dcc);
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
			tmp = __tr2qs_ctx(
			          "<b>%1 [%2@%3]</b> "
			          "wants to send you the file "
			          "'<b>%4</b>', "
			          "<b>%5</b> large.<br>"
			          "The connection target will be host <b>%6</b> on port <b>%7</b><br>",
			          "dcc")
			          .arg(dcc->szNick, dcc->szUser, dcc->szHost, dcc->szFileName)
			          .arg(KviQString::makeSizeReadable(dcc->szFileSize.toULongLong()))
			          .arg(dcc->szIp, dcc->szPort);
		}
		else
		{
			// passive: we will be listening!
			tmp = __tr2qs_ctx(
			          "<b>%1 [%2@%3]</b> "
			          "wants to send you the file "
			          "'<b>%4</b>', "
			          "<b>%5</b> large.<br>"
			          "You will be the passive side of the connection.<br>",
			          "dcc")
			          .arg(dcc->szNick, dcc->szUser, dcc->szHost, dcc->szFileName)
			          .arg(KviQString::makeSizeReadable(dcc->szFileSize.toULongLong()));
		}

		if(dcc->bIsIncomingAvatar)
		{
			tmp += __tr2qs_ctx(
			    "<b>Note:</b>"
			    "The file appears to be an avatar that you have requested. "
			    "You should not change its filename. "
			    "Save it in a location where KVIrc can find it, such as "
			    "the 'avatars', 'incoming', or 'pics' directories, "
			    "your home directory, or the save directory for the incoming file type. "
			    "The default save path will probably work. "
			    "You can instruct KVIrc to accept incoming avatars automatically "
			    "by setting the option <tt>boolAutoAcceptIncomingAvatars</tt> to true.<br>",
			    "dcc");
		}

		//#warning "Maybe remove the pending avatar if rejected ?"

		QString title = __tr2qs_ctx("DCC %1 Request - KVIrc", "dcc").arg(dcc->szType);

		DccAcceptDialog * box = new DccAcceptDialog(this, dcc, tmp, title);
		m_pBoxList->append(box);
		connect(box, SIGNAL(accepted(DccDialog *, DccDescriptor *)),
		    this, SLOT(chooseSaveFileName(DccDialog *, DccDescriptor *)));
		connect(box, SIGNAL(rejected(DccDialog *, DccDescriptor *)),
		    this, SLOT(cancelDcc(DccDialog *, DccDescriptor *)));
		box->show();
	}
	else
	{
		// auto accept
		if(_OUTPUT_VERBOSE)
		{
			dcc->console()->output(KVI_OUT_DCCMSG, __tr2qs_ctx("Auto-accepting DCC %Q request from %Q!%Q@%Q for file %Q", "dcc"),
			    &(dcc->szType), &(dcc->szNick), &(dcc->szUser),
			    &(dcc->szHost), &(dcc->szFileName));
		}
		chooseSaveFileName(nullptr, dcc);
	}
}

void DccBroker::chooseSaveFileName(DccDialog * box, DccDescriptor * dcc)
{
	if(box)
		box->forgetDescriptor();

	// Lookup the suggested save directory

	dcc->szLocalFileName = "";

	if(dcc->bIsIncomingAvatar)
		g_pApp->getLocalKvircDirectory(dcc->szLocalFileName, KviApplication::Avatars);
	else
	{

		if(KVI_OPTION_BOOL(KviOption_boolUseIncomingDccMediaTypeSavePath))
		{
			g_pMediaManager->lock();
			if(KviMediaType * mt = g_pMediaManager->findMediaType(dcc->szFileName.toUtf8().data(), false))
			{
				if(mt->szSavePath.hasData())
				{
					if(KviFileUtils::directoryExists(mt->szSavePath.ptr()))
						dcc->szLocalFileName = mt->szSavePath;
					else
					{
						if(KviFileUtils::makeDir(mt->szSavePath.ptr()))
							dcc->szLocalFileName = mt->szSavePath;
					}
					if(KVI_OPTION_BOOL(KviOption_boolSortReceivedByDccFilesByNicks))
					{
						KviQString::ensureLastCharIs(dcc->szLocalFileName, KVI_PATH_SEPARATOR_CHAR);
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
			g_pApp->getLocalKvircDirectory(dcc->szLocalFileName, KviApplication::Incoming);
			if(KVI_OPTION_BOOL(KviOption_boolSortReceivedByDccFilesByNicks))
			{
				KviQString::ensureLastCharIs(dcc->szLocalFileName, KVI_PATH_SEPARATOR_CHAR);
				QString cleanNick = dcc->szNick;
				KviFileUtils::cleanFileName(cleanNick);
				dcc->szLocalFileName.append(cleanNick);
				KviFileUtils::adjustFilePath(dcc->szLocalFileName);
				KviFileUtils::makeDir(dcc->szLocalFileName);
			}
		}
	}
	KviFileUtils::adjustFilePath(dcc->szLocalFileName);
	KviQString::ensureLastCharIs(dcc->szLocalFileName, KVI_PATH_SEPARATOR_CHAR);

	if(!(dcc->bAutoAccept))
	{
		QString cleanFilename = dcc->szFileName;
		KviFileUtils::cleanFileName(cleanFilename);
		dcc->szLocalFileName.append(cleanFilename);
		if(KviFileDialog::askForSaveFileName(
		       dcc->szLocalFileName,
		       __tr2qs_ctx("Choose Files to Save - KVIrc", "dcc"),
		       dcc->szLocalFileName,
		       QString(),
		       false,
		       false,
		       true,
		       g_pMainWindow))
		{
			renameOverwriteResume(nullptr, dcc);
		}
		else
		{
			cancelDcc(dcc);
		}
	}
	else
	{
		// auto accept
		// WE choose the filename
		QString cleanFilename = dcc->szFileName;
		KviFileUtils::cleanFileName(cleanFilename);
		dcc->szLocalFileName.append(cleanFilename);

		if(_OUTPUT_VERBOSE)
		{
			dcc->console()->output(KVI_OUT_DCCMSG, __tr2qs_ctx("Auto-saving DCC %Q file %Q as %Q", "dcc"),
			    &(dcc->szType), &(dcc->szFileName), &(dcc->szLocalFileName));
		}

		renameOverwriteResume(nullptr, dcc);
	}
}

void DccBroker::renameOverwriteResume(DccDialog * box, DccDescriptor * dcc)
{
	if(box)
		box->forgetDescriptor();

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
				tmp = __tr2qs_ctx(
				          "The file '<b>%1</b>' already exists "
				          "and is <b>%2</b> large.<br>"
				          "Do you wish to<br>"
				          "<b>auto-rename</b> the new file,<br>"
				          "<b>overwrite</b> the existing file, or<br> "
				          "<b>resume</b> an incomplete download?",
				          "dcc")
				          .arg(dcc->szLocalFileName)
				          .arg(KviQString::makeSizeReadable(fi.size()));
			}
			else
			{
				bDisableResume = true;
				// the file on disk is larger or equal to the remote one
				tmp = __tr2qs_ctx(
				          "The file '<b>%1</b>' already exists "
				          "and is larger than the offered one.<br>"
				          "Do you wish to<br>"
				          "<b>auto-rename</b> the new file, or<br>"
				          "<b>overwrite</b> the existing file?",
				          "dcc")
				          .arg(dcc->szLocalFileName);
			}

			DccRenameDialog * box = new DccRenameDialog(this, dcc, tmp, bDisableResume);
			m_pBoxList->append(box);
			connect(box, SIGNAL(renameSelected(DccDialog *, DccDescriptor *)),
			    this, SLOT(renameDccSendFile(DccDialog *, DccDescriptor *)));
			connect(box, SIGNAL(overwriteSelected(DccDialog *, DccDescriptor *)),
			    this, SLOT(recvFileExecute(DccDialog *, DccDescriptor *)));
			connect(box, SIGNAL(cancelSelected(DccDialog *, DccDescriptor *)),
			    this, SLOT(cancelDcc(DccDialog *, DccDescriptor *)));
			box->show();
			return;
		}

		// auto resume ?
		if(
		    KVI_OPTION_BOOL(KviOption_boolAutoResumeDccSendWhenAutoAccepted) && (bOk) && // only if the remote size is really known
		    (iRemoteSize > (quint64)fi.size()) &&                                        // only if the remote size is larger than the local size
		    (!DccFileTransfer::nonFailedTransferWithLocalFileName(dcc->szLocalFileName)) // only if there is no transfer with this local file name yet
		    )
		{
			// yep, auto resume...
			dcc->bResume = true;
			recvFileExecute(nullptr, dcc);
		}
		else if(iRemoteSize == (quint64)fi.size())
		{
			dcc->console()->output(KVI_OUT_DCCMSG, "Transfer aborted: file %Q already completed", &(dcc->szLocalFileName));
			cancelDcc(nullptr, dcc);
		}
		else
		{
			// otherwise auto rename
			renameDccSendFile(nullptr, dcc);
		}
		return;
	}

	dcc->szLocalFileSize = "0";

	// everything OK
	recvFileExecute(nullptr, dcc);
}

void DccBroker::renameDccSendFile(DccDialog * box, DccDescriptor * dcc)
{
	if(box)
		box->forgetDescriptor();

	if(QFileInfo(dcc->szLocalFileName).exists())
	{
		KviCString szOrig = dcc->szLocalFileName;
		int i = 1;
		do
		{
			KviCString szNum;
			szNum.setNum(i);
			int idx = szOrig.findLastIdx('.');
			if(idx != -1)
			{
				dcc->szLocalFileName = szOrig.left(idx);
				dcc->szLocalFileName += ".";
				dcc->szLocalFileName += szNum;
				dcc->szLocalFileName += szOrig.right(szOrig.len() - idx);
			}
			else
			{
				dcc->szLocalFileName = szOrig;
				dcc->szLocalFileName += ".";
				dcc->szLocalFileName += szNum;
			}
			i++;
		} while(QFileInfo(dcc->szLocalFileName).exists());

		if(_OUTPUT_VERBOSE)
		{
			dcc->console()->output(KVI_OUT_DCCMSG, __tr2qs_ctx("File %s exists, auto-renaming to %Q", "dcc"),
			    szOrig.ptr(), &(dcc->szLocalFileName));
		}
	}

	dcc->szLocalFileSize = "0"; // 0 for sure

	recvFileExecute(nullptr, dcc);
}

void DccBroker::recvFileExecute(DccDialog * box, DccDescriptor * dcc)
{
	if(box)
		box->forgetDescriptor();

	if(!g_pApp->windowExists(dcc->console()))
	{
		// rebind to the first available console....
		dcc->setConsole(g_pApp->activeConsole());
	}

	DccFileTransfer * send = new DccFileTransfer(dcc);

	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : (KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccSend) || (dcc->bAutoAccept && KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccSendWhenAutoAccepted)));

	send->invokeTransferWindow(bMinimized, bMinimized);
}

void DccBroker::sendFileManage(DccDescriptor * dcc)
{
	QStringList filenames;
	if(KviFileDialog::askForOpenFileNames(
	       filenames,
	       __tr2qs_ctx("Choose Files to Send - KVIrc", "dcc"),
	       QString(),
	       QString(),
	       false,
	       true,
	       g_pMainWindow))
	{
		if(filenames.count() > 0)
		{
			DccDescriptor * d;
			QStringList::Iterator it = filenames.begin();
			while(it != filenames.end())
			{
				d = new DccDescriptor(*dcc);
				d->szLocalFileName = *(it);
				d->szLocalFileName = d->szLocalFileName.trimmed();
				++it;
				if(d->szLocalFileName.isEmpty())
					cancelDcc(d);
				else
					sendFileExecute(nullptr, d);
			}
			delete dcc;
		}
	}
	else
	{
		cancelDcc(dcc);
	}
}

void DccBroker::sendFileExecute(DccDialog * box, DccDescriptor * dcc)
{
	if(box)
		box->forgetDescriptor();

	if(!g_pApp->windowExists(dcc->console()))
	{
		// rebind to the first available console....
		dcc->setConsole(g_pApp->activeConsole());
	}

	QFileInfo fi(dcc->szLocalFileName);
	if(!(fi.exists() && fi.isReadable() && (fi.isFile()) && (fi.size() > 0)))
	{
		dcc->console()->output(KVI_OUT_DCCERROR, __tr2qs_ctx("Can't open file %Q for reading", "dcc"),
		    &(dcc->szLocalFileName));
		delete dcc;
		return;
	}

	dcc->szFileName = dcc->szLocalFileName;
	dcc->szFileName = QFileInfo(dcc->szFileName).fileName();

	dcc->szLocalFileSize.setNum(fi.size());

	DccFileTransfer * send = new DccFileTransfer(dcc);

	bool bMinimized = dcc->bOverrideMinimize ? dcc->bShowMinimized : KVI_OPTION_BOOL(KviOption_boolCreateMinimizedDccSend);

	send->invokeTransferWindow(bMinimized, bMinimized);
}

bool DccBroker::canUnload()
{
	if(m_pBoxList)
	{
		if((m_pBoxList->count() != 0) || (m_pDccWindowList->count() != 0) || (DccFileTransfer::transferCount() != 0))
			return false;
	} // else in the destructor anyway (going to die)
	return true;
}

bool DccBroker::handleResumeAccepted(const char * filename, const char * port, const char * szZeroPortTag)
{
	return DccFileTransfer::handleResumeAccepted(filename, port, szZeroPortTag);
}

bool DccBroker::handleResumeRequest(KviDccRequest * dcc, const char * filename, const char * port, unsigned long filePos, const char * szZeroPortTag)
{
	//qDebug("HANDLE %s %s %u %s",filename,port,filePos,szZeroPortTag);
	// the zeroPOrtTag is nonempty here only if port == 0
	if(kvi_strEqualCI("0", port) && szZeroPortTag)
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

			KviCString szBuffy;
			KviIrcServerParser::encodeCtcpParameter(filename, szBuffy);

			dcc->ctcpMsg->msg->console()->connection()->sendFmtData(
			    "PRIVMSG %s :%cDCC ACCEPT %s %s %s %s%c",
			    dcc->ctcpMsg->msg->console()->connection()->encodeText(dcc->ctcpMsg->pSource->nick()).data(),
			    0x01,
			    szBuffy.ptr(),
			    port,
			    dcc->ctcpMsg->msg->console()->connection()->encodeText(QString::number(filePos)).data(),
			    szZeroPortTag,
			    0x01);

			return true;
		}
	}
	//qDebug("NOT A ZeRO PORT");

	return DccFileTransfer::handleResumeRequest(filename, port, filePos);
}
