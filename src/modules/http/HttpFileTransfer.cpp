//=============================================================================
//
//   File : HttpFileTransfer.cpp
//   Creation date : Tue Apr 22 2003 02:00:12 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "HttpFileTransfer.h"

#include "KviApplication.h"
#include "kvi_out.h"
#include "KviLocale.h"
#include "KviWindow.h"
#include "KviIconManager.h"
#include "KviNetUtils.h"
#include "KviKvsEventTriggers.h"
#include "KviConsoleWindow.h"
#include "KviKvsScript.h"

#include <QPainter>
#include <QMenu>

static KviPointerList<HttpFileTransfer> * g_pHttpFileTransfers = nullptr;
static QPixmap * g_pHttpIcon = nullptr;

HttpFileTransfer::HttpFileTransfer()
    : KviFileTransfer()
{
	init(); // ensure we're initialized
	g_pHttpFileTransfers->append(this);

	m_tStartTime = kvi_unixTime();
	m_tTransferStartTime = 0;
	m_tTransferEndTime = 0;

	m_bNotifyCompletion = true;
	m_bAutoClean = false;
	m_pAutoCleanTimer = nullptr;
	m_bNoOutput = false;

	m_pHttpRequest = new KviHttpRequest();

	connect(m_pHttpRequest, SIGNAL(status(const QString &)), this, SLOT(statusMessage(const QString &)));
	connect(m_pHttpRequest, SIGNAL(terminated(bool)), this, SLOT(transferTerminated(bool)));
	connect(m_pHttpRequest, SIGNAL(header(KviPointerHashTable<const char *, KviCString> *)), this, SLOT(headersReceived(KviPointerHashTable<const char *, KviCString> *)));
	connect(m_pHttpRequest, SIGNAL(resolvingHost(const QString &)), this, SLOT(resolvingHost(const QString &)));
	connect(m_pHttpRequest, SIGNAL(requestSent(const QStringList &)), this, SLOT(requestSent(const QStringList &)));
	connect(m_pHttpRequest, SIGNAL(contactingHost(const QString &)), this, SLOT(contactingHost(const QString &)));
	connect(m_pHttpRequest, SIGNAL(receivedResponse(const QString &)), this, SLOT(receivedResponse(const QString &)));
	connect(m_pHttpRequest, SIGNAL(connectionEstablished()), this, SLOT(connectionEstablished()));

	m_eGeneralStatus = Initializing;
	m_szStatusString = __tr2qs_ctx("Initializing", "http");
}

HttpFileTransfer::~HttpFileTransfer()
{
	g_pHttpFileTransfers->removeRef(this);
	delete m_pHttpRequest;
	if(m_pAutoCleanTimer)
	{
		m_pAutoCleanTimer->stop();
		delete m_pAutoCleanTimer;
	}
}

void HttpFileTransfer::autoClean()
{
	killTimer(m_TimerId);
	die();
}

QString HttpFileTransfer::localFileName()
{
	return m_pHttpRequest->fileName();
}

void HttpFileTransfer::abort()
{
	m_pHttpRequest->abort();
}

void HttpFileTransfer::fillContextPopup(QMenu * m)
{
	QAction * pAction = m->addAction(__tr2qs_ctx("Abort", "http"), this, SLOT(abort()));
	if(!active())
		pAction->setEnabled(false);
}

bool HttpFileTransfer::active()
{
	return ((m_eGeneralStatus == Connecting) || (m_eGeneralStatus == Downloading));
}

void HttpFileTransfer::displayPaint(QPainter * p, int column, QRect rect)
{
	int width = rect.width(), height = rect.height();
	QString txt;
	bool bIsTerminated = ((m_eGeneralStatus == Success) || (m_eGeneralStatus == Failure));

	switch(column)
	{
		case COLUMN_TRANSFERTYPE:
		{
			int offset = 0;
			switch(m_eGeneralStatus)
			{
				case Initializing:
				case Connecting:
					offset = 0;
					break;
				case Downloading:
					offset = 48;
					break;
				case Success:
					offset = 96;
					break;
				case Failure:
					offset = 144;
					break;
			}
			if(g_pHttpIcon)
				p->drawPixmap(width / 2 - 48 / 2, rect.top() + height / 2 - 64 / 2, *g_pHttpIcon, offset, 0, 48, 64);
		}
		break;
		case COLUMN_FILEINFO:
		{
			QFontMetrics fm(p->font());

			QString szFrom = __tr2qs_ctx("From: ", "http");
			QString szTo = __tr2qs_ctx("To: ", "http");

			int daW1 = fm.width(szFrom);
			int daW2 = fm.width(szTo);
			if(daW1 < daW2)
				daW1 = daW2;
			int iLineSpacing = fm.lineSpacing();

			p->setPen(Qt::black);

			int iY = rect.top() + 4;

			p->drawText(rect.left() + 4 + daW1, iY, width - (8 + daW1), height - 8, Qt::AlignTop | Qt::AlignLeft, m_pHttpRequest->url().url());
			iY += iLineSpacing;
			if(!(m_pHttpRequest->fileName().isEmpty()))
			{
				p->drawText(rect.left() + 4 + daW1, iY, width - (8 + daW1), height - 8, Qt::AlignTop | Qt::AlignLeft, m_pHttpRequest->fileName());
			}
			iY += iLineSpacing;

			p->setPen(Qt::darkGray);

			p->drawText(rect.left() + 4, rect.top() + 4, width - 8, height - 8, Qt::AlignTop | Qt::AlignLeft, szFrom);
			p->drawText(rect.left() + 4, rect.top() + 4 + iLineSpacing, width - 8, height - 8, Qt::AlignTop | Qt::AlignLeft, szTo);

			p->setPen(QColor(180, 180, 200));

			iLineSpacing += 2;

			p->drawRect(rect.left() + 4, rect.top() + height - (iLineSpacing + 4), width - 8, iLineSpacing);
			p->fillRect(rect.left() + 5, rect.top() + height - (iLineSpacing + 3), width - 10, iLineSpacing - 2, bIsTerminated ? QColor(210, 210, 210) : QColor(190, 190, 240));
			p->setPen(Qt::black);
			p->drawText(rect.left() + 7, rect.top() + height - (iLineSpacing + 4), width - 14, iLineSpacing, Qt::AlignVCenter | Qt::AlignLeft, m_szStatusString);
		}
		break;
		case COLUMN_PROGRESS:
		{
			QFontMetrics fm(p->font());

			unsigned int uTotal = m_pHttpRequest->totalSize();
			unsigned int uRecvd = m_pHttpRequest->receivedSize();
			int iW = width - 8;

			p->setPen(bIsTerminated ? Qt::lightGray : QColor(210, 210, 240));
			p->drawRect(rect.left() + 4, rect.top() + 4, iW, 12);

			int iAvgSpeed = -1;
			int iEta = -1;

			if(m_tTransferStartTime > 0)
			{
				int tSpan = kvi_timeSpan(m_tTransferEndTime > 0 ? m_tTransferEndTime : kvi_unixTime(), m_tTransferStartTime);
				if(tSpan > 0)
				{
					//qDebug("SPAN: %d (%d - %d)",tSpan,m_tTransferEndTime > 0 ? m_tTransferEndTime : kvi_unixTime(),m_tTransferStartTime);
					iAvgSpeed = uRecvd / tSpan;
					if(!bIsTerminated && (uTotal >= uRecvd))
					{
						unsigned int uRemaining = uTotal - uRecvd;
						iEta = uRemaining / iAvgSpeed;
					}
				}
			}

			if(uTotal > 0)
			{
				double dPerc = (double)(((double)uRecvd) * 100.0) / (double)uTotal;
				iW -= 2;
				int iL = (int)((((double)iW) * dPerc) / 100.0);
				//iR = iW - iL;
				p->fillRect(rect.left() + 5, rect.top() + 5, iL, 10, bIsTerminated ? QColor(140, 110, 110) : QColor(200, 100, 100));

				txt = QString(__tr2qs_ctx("%1 of %2 (%3 %)", "http")).arg(KviQString::makeSizeReadable(uRecvd), KviQString::makeSizeReadable(uTotal)).arg(dPerc, 0, 'f', 2);
			}
			else
			{
				txt = KviQString::makeSizeReadable(m_pHttpRequest->receivedSize());
			}

			p->setPen(Qt::black);

			p->drawText(rect.left() + 4, rect.top() + 19, width - 8, height - 8, Qt::AlignTop | Qt::AlignLeft, txt);

			int iLeftHalf = (iW - 2) / 2;
			int iRightHalf = iW - (iLeftHalf + 1);
			int iLineSpacing = fm.lineSpacing() + 2;

			txt = __tr2qs_ctx("Average:", "dcc");
			txt += " ";
			if(iAvgSpeed >= 0)
			{
				QString tmpspd;
				KviNetUtils::formatNetworkBandwidthString(tmpspd, iAvgSpeed);
				txt += tmpspd;
			}
			else
			{
				txt += "? B/s";
			}

			int iDaH = height - (iLineSpacing + 4);

			p->setPen(QColor(180, 180, 200));
			p->drawRect(rect.left() + 4, rect.top() + iDaH, iLeftHalf, iLineSpacing);
			p->fillRect(rect.left() + 5, rect.top() + iDaH + 1, iLeftHalf - 2, iLineSpacing - 2, bIsTerminated ? QColor(210, 210, 210) : QColor(190, 190, 240));
			p->setPen(bIsTerminated ? Qt::darkGray : Qt::black);
			p->drawText(rect.left() + 6, rect.top() + iDaH, iLeftHalf - 4, iLineSpacing, Qt::AlignLeft | Qt::AlignVCenter, txt);

			unsigned int uD, uH, uM, uS;

			if(bIsTerminated)
			{
				KviTimeUtils::secondsToDaysHoursMinsSecs(kvi_timeSpan(m_tTransferEndTime, m_tTransferStartTime), &uD, &uH, &uM, &uS);
				txt = "Total: ";
				if(uD > 0)
					txt += __tr2qs_ctx("%1d %2h %3m %4s", "http").arg(uD).arg(uH).arg(uM).arg(uS);
				else if(uH > 0)
					txt += __tr2qs_ctx("%1h %2m %3s", "http").arg(uH).arg(uM).arg(uS);
				else
					txt += __tr2qs_ctx("%1m %2s", "http").arg(uM).arg(uS);
			}
			else
			{
				if(iEta >= 0)
				{
					KviTimeUtils::secondsToDaysHoursMinsSecs(iEta, &uD, &uH, &uM, &uS);
					txt = "ETA: ";
					if(uD > 0)
						txt += __tr2qs_ctx("%1d %2h %3m %4s", "http").arg(uD).arg(uH).arg(uM).arg(uS);
					else if(uH > 0)
						txt += __tr2qs_ctx("%1h %2m %3s", "http").arg(uH).arg(uM).arg(uS);
					else
						txt += __tr2qs_ctx("%1m %2s", "http").arg(uM).arg(uS);
				}
				else
				{
					txt = "ETA: Unknown";
				}
			}

			p->setPen(QColor(180, 180, 200));
			p->drawRect(rect.left() + width - (4 + iRightHalf), rect.top() + iDaH, iRightHalf, iLineSpacing);
			p->fillRect(rect.left() + width - (3 + iRightHalf), rect.top() + iDaH + 1, iRightHalf - 2, iLineSpacing - 2, bIsTerminated ? QColor(210, 210, 210) : QColor(190, 190, 240));
			p->setPen(bIsTerminated ? Qt::darkGray : Qt::black);
			p->drawText(rect.left() + width - (2 + iRightHalf), rect.top() + iDaH, iRightHalf - 4, iLineSpacing, Qt::AlignLeft | Qt::AlignVCenter, txt);
		}
		break;
	}
}

int HttpFileTransfer::displayHeight(int iLineSpacing)
{
	int iH = (iLineSpacing * 3) + 10;
	return iH >= 70 ? iH : 70;
}

QString HttpFileTransfer::tipText()
{
	QString s;
	s = QString(R"(<table><tr><td bgcolor="#000000"><font color="#FFFFFF"><b>HTTP Transfer (ID %1)</b></font></td></tr>)").arg(id());

	if(m_lRequest.count() > 0)
	{
		s += R"(<tr><td bgcolor="#404040"><font color="#FFFFFF">Request Headers</font></td></tr>)";
		s += "<tr><td bgcolor=\"#C0C0C0\">";
		for(QStringList::ConstIterator it = m_lRequest.begin(); it != m_lRequest.end(); ++it)
		{
			s += "&nbsp; &nbsp;";
			s += *it;
			s += "<br>";
		}
		s += "</td></tr>";
	}

	if(m_lHeaders.count() > 0)
	{
		s += R"(<tr><td bgcolor="#404040"><font color="#FFFFFF">Response Headers</font></td></tr>)";
		s += "<tr><td bgcolor=\"#C0C0C0\">";
		for(QStringList::ConstIterator it = m_lHeaders.begin(); it != m_lHeaders.end(); ++it)
		{
			s += "&nbsp; &nbsp;";
			s += *it;
			s += "<br>";
		}
		s += "</td></tr>";
	}

	s += "<table>";

	return s;
}

void HttpFileTransfer::init()
{
	if(g_pHttpFileTransfers)
		return;
	g_pHttpFileTransfers = new KviPointerList<HttpFileTransfer>;
	g_pHttpFileTransfers->setAutoDelete(false);

	QPixmap * pix = g_pIconManager->getImage("kvi_httpicons.png", false);
	if(pix)
		g_pHttpIcon = new QPixmap(*pix);
	else
		g_pHttpIcon = nullptr;
}

void HttpFileTransfer::done()
{
	if(!g_pHttpFileTransfers)
		return;
	while(HttpFileTransfer * t = g_pHttpFileTransfers->first())
		delete t;
	delete g_pHttpFileTransfers;
	g_pHttpFileTransfers = nullptr;
	if(g_pHttpIcon)
	{
		delete g_pHttpIcon;
		g_pHttpIcon = nullptr;
	}
}

unsigned int HttpFileTransfer::runningTransfers()
{
	if(!g_pHttpFileTransfers)
		return 0;
	return g_pHttpFileTransfers->count();
}

void HttpFileTransfer::requestSent(const QStringList & requestHeaders)
{
	m_szStatusString = __tr2qs_ctx("Request sent: waiting for reply...", "http");
	displayUpdate();

	KviWindow * out = transferWindow();
	if(!out)
		return;

	if(!m_bNoOutput)
		out->output(KVI_OUT_GENERICSTATUS, __tr2qs_ctx("[HTTP %d]: Request data sent:", "http"), id());

	for(const auto & requestHeader : requestHeaders)
	{
		if(!m_bNoOutput)
			out->output(KVI_OUT_GENERICSTATUS, "[HTTP %d]:   %s", id(), requestHeader.toUtf8().data());
	}

	m_lRequest = requestHeaders;
}

void HttpFileTransfer::connectionEstablished()
{
	m_szStatusString = __tr2qs_ctx("Connection established: sending request", "http");
	displayUpdate();
}

void HttpFileTransfer::resolvingHost(const QString & hostname)
{
	m_szStatusString = __tr2qs_ctx("Resolving host %1", "http").arg(hostname);
	displayUpdate();
}

void HttpFileTransfer::contactingHost(const QString & ipandport)
{
	m_szStatusString = __tr2qs_ctx("Contacting host %1", "http").arg(ipandport);
	displayUpdate();
}

void HttpFileTransfer::receivedResponse(const QString & response)
{
	m_lHeaders.clear();
	m_lHeaders.append(response);
	m_szStatusString = __tr2qs_ctx("Transferring data (%1)", "http").arg(response);
	m_tTransferStartTime = kvi_unixTime();
	m_eGeneralStatus = Downloading;
	displayUpdate();
}

void HttpFileTransfer::statusMessage(const QString & txt)
{
	KviWindow * out = transferWindow();
	if(out && (!m_bNoOutput))
		out->output(KVI_OUT_GENERICSTATUS, "[HTTP %d]: %Q", id(), &txt);
}

void HttpFileTransfer::transferTerminated(bool bSuccess)
{
	KviWindow * out = transferWindow();

	m_tTransferEndTime = kvi_unixTime();

	KviKvsVariantList vParams;
	vParams.append(bSuccess);
	vParams.append(new KviKvsVariant(m_pHttpRequest->url().url(), true));
	vParams.append(new KviKvsVariant(m_pHttpRequest->fileName(), true));
	vParams.append(new KviKvsVariant(m_vMagicIdentifier));

	if(m_szCompletionCallback.isNull())
	{
		KVS_TRIGGER_EVENT(KviEvent_OnHTTPGetTerminated, out ? out : (KviWindow *)(g_pApp->activeConsole()), &vParams)
	}
	else
	{
		KviKvsScript::run(m_szCompletionCallback, out ? out : (KviWindow *)(g_pApp->activeConsole()), &vParams);
	}

	if(bSuccess)
	{
		m_szStatusString = __tr2qs_ctx("Transfer completed", "http");
		m_eGeneralStatus = Success;
		displayUpdate();
		if(out && (!m_bNoOutput))
			out->output(KVI_OUT_GENERICSUCCESS, __tr2qs_ctx("[HTTP %d]: Transfer completed", "http"), id());
		g_pApp->fileDownloadTerminated(
		    true,
		    m_pHttpRequest->url().url(),
		    m_pHttpRequest->fileName(),
		    QString(),
		    QString(),
		    !m_bNotifyCompletion);
	}
	else
	{
		m_szStatusString = __tr2qs_ctx("Transfer failed", "http");
		m_szStatusString += ": ";
		m_szStatusString += m_pHttpRequest->lastError();
		m_eGeneralStatus = Failure;
		displayUpdate();
		if(out && (!m_bNoOutput))
			out->output(KVI_OUT_GENERICERROR, __tr2qs_ctx("[HTTP %d]: Transfer failed: %Q", "http"), id(), &(m_pHttpRequest->lastError()));
		g_pApp->fileDownloadTerminated(
		    false,
		    m_pHttpRequest->url().url(),
		    m_pHttpRequest->fileName(),
		    QString(),
		    m_pHttpRequest->lastError(),
		    !m_bNotifyCompletion);
	}

	if(m_bAutoClean)
	{
		if(m_pAutoCleanTimer)
			delete m_pAutoCleanTimer;
		m_pAutoCleanTimer = new QTimer();
		connect(m_pAutoCleanTimer, SIGNAL(timeout()), this, SLOT(autoClean()));
		m_pAutoCleanTimer->start(100);
		m_TimerId = m_pAutoCleanTimer->timerId();
	}
}

void HttpFileTransfer::headersReceived(KviPointerHashTable<const char *, KviCString> * h)
{
	if(!h)
		return;
	KviWindow * out = transferWindow();

	if(out && (!m_bNoOutput))
		out->output(KVI_OUT_GENERICSTATUS, __tr2qs_ctx("[HTTP %d]: response headers:", "http"), id());
	KviPointerHashTableIterator<const char *, KviCString> it(*h);
	while(KviCString * s = it.current())
	{
		QString szHeader = it.currentKey();
		szHeader += ": ";
		szHeader += s->ptr();
		m_lHeaders.append(szHeader);
		if(out && (!m_bNoOutput))
			out->output(KVI_OUT_GENERICSTATUS, "[HTTP %d]:   %s: %s", id(), it.currentKey(), s->ptr());
		++it;
	}
}

bool HttpFileTransfer::startDownload()
{
	m_eGeneralStatus = Connecting;
	return m_pHttpRequest->start();
}
