//=============================================================================
//
//   File : DccFileTransfer.cpp
//   Creation date : Tue Sep 20 09 2000 15:14:14 by Szymon Stefanek
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

#define _KVI_DEBUG_CHECK_RANGE_

#include "DccFileTransfer.h"
#include "DccBroker.h"
#include "DccMarshal.h"
#include "DccWindow.h"

#include "kvi_debug.h"
#include "KviApplication.h"
#include "KviOptions.h"
#include "KviIrcView.h"
#include "KviIconManager.h"
#include "KviLocale.h"
#include "KviError.h"
#include "kvi_out.h"
#include "KviNetUtils.h"
#include "KviConsoleWindow.h"
#include "KviMainWindow.h"
#include "KviMemory.h"
#include "KviThread.h"
#include "KviIrcSocket.h"
#include "KviMediaManager.h"
#include "kvi_socket.h"
#include "KviKvsEventTriggers.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviIrcServerParser.h"
#include "KviKvsScript.h"

#ifdef COMPILE_ON_WINDOWS
// Ugly Windoze compiler...
#include "DccDialog.h"
#endif

#include <QFile>
#include <QPainter>
#include <QDateTime>
#include <qglobal.h>
#include <QSpinBox>
#include <QLayout>
#include <QPushButton>
#include <QEvent>
#include <QCloseEvent>
#include <QTimer>
#include <QtEndian>

#define INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS 3000
#define INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_SECS 3

// This limit, when multiplied by INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_SECS
// must fit in 31 bits (0x7fffffff)! (because of data size limits)
#define MAX_DCC_BANDWIDTH_LIMIT 0x1fffffff

// FIXME: The events OnDCCConnect etc are in wrong places here...!

extern DccBroker * g_pDccBroker;

extern KVIRC_API KviMediaManager * g_pMediaManager; // KviApplication.cpp

static KviPointerList<DccFileTransfer> * g_pDccFileTransfers = nullptr;
static QPixmap * g_pDccFileTransferIcon = nullptr;

//#warning "The events that have a KviCString data pointer should become real classes, that take care of deleting the data pointer!"
//#warning "Otherwise, when left undispatched we will be leaking memory (event class destroyed but not the data ptr)"

DccRecvThread::DccRecvThread(QObject * par, kvi_socket_t fd, KviDccRecvThreadOptions * opt)
    : DccThread(par, fd)
{
	m_pOpt = opt;
	m_uAverageSpeed = 0;
	m_uInstantSpeed = 0;
	m_uFilePosition = 0;

	m_uTotalReceivedBytes = 0;
	m_uInstantReceivedBytes = 0;
	m_pFile = nullptr;
	m_pTimeInterval = new KviMSecTimeInterval();
	m_uStartTime = 0;
	m_uInstantSpeedInterval = 0;
}

DccRecvThread::~DccRecvThread()
{
	if(m_pOpt)
		delete m_pOpt;
	if(m_pFile)
		delete m_pFile;
	delete m_pTimeInterval;
}

bool DccRecvThread::sendAck(qint64 filePos, bool bUse64BitAck)
{
	quint32 ack32 = htonl(filePos & 0xffffffff);
	quint64 ack64 = qToBigEndian(filePos);

	char * ack = (char *)&ack32;
	int ackSize = 4;

	if(bUse64BitAck)
	{
		ackSize = 8;
		ack = (char *)&ack64;
	}

	int iRet = 0;
#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
		iRet = m_pSSL->write(ack, ackSize);
	else
#endif //COMPILE_SSL_SUPPORT
		iRet = kvi_socket_send(m_fd, (void *)(ack), ackSize);

	if(iRet == ackSize)
		return true; // everything sent

	// When downloading from a fast server using send-ahead via an asymmetric link (such as the
	// common ADSL lines) it may happen that the network output queue gets saturated with ACKs.
	// In this case the network stack will refuse to send our packet and we get here.
	//
	// We should either retry to send the ACK in a while or avoid sending it at all (as with
	// send-ahead acks aren't usually checked per-packet).

	if(iRet == 0)
	{
		// We can live with this: no data has been sent at all
		// Not sending the ack and hoping that the server will not stall is better than
		// killing the connection from our side anyway.
		return true;
	}

	if(iRet < 0)
	{
// Reported error. If it's EAGAIN or EINTR then no data has been sent.
#ifdef COMPILE_SSL_SUPPORT
		if(m_pSSL)
		{
			// dropping ack when no serious ssl error occurred
			switch(m_pSSL->getProtocolError(iRet))
			{
				case KviSSL::ZeroReturn:
				//return false; check eagain
				case KviSSL::Success:
				case KviSSL::WantRead:
				case KviSSL::WantWrite:
					return true;
					break;
				default:
					// Raise unknown SSL ERROR
					postErrorEvent(KviError::SSLError);
					return false;
					break;
			}

			return false;
		}
#endif //COMPILE_SSL_SUPPORT

		int err = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else  //!(defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW))
		if((err != EAGAIN) && (err != EINTR))
#endif //!(defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW))
		{
			// some other kind of error
			postErrorEvent(KviError::AcknowledgeError);
			return false;
		}

		return true; // no data sent: same as iRet == 0 above.
	}

	// Sent something but not everything.
	// How likely is it to get in here ?!
	// Sleep for a short while and try to send the missing part.
	// This will probably throttle the bandwidth usage a bit too.
	msleep(10);

	int iMissingPart = ackSize - iRet;

#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
		iRet = m_pSSL->write(ack + iRet, iMissingPart);
	else
#endif //COMPILE_SSL_SUPPORT
		iRet = kvi_socket_send(m_fd, (void *)(ack + iRet), iMissingPart);

	if(iRet != iMissingPart)
	{
		// Crap.. couldn't send the missing part of the ack :/
		postErrorEvent(KviError::AcknowledgeError);
		return false;
	}
	return true;
}

void DccRecvThread::updateStats()
{
	m_uInstantSpeedInterval += m_pTimeInterval->mark();
	unsigned long uCurTime = m_pTimeInterval->secondsCounter();

	m_pMutex->lock();
	unsigned long uElapsedTime = uCurTime - m_uStartTime;
	if(uElapsedTime < 1)
		uElapsedTime = 1;

	m_uFilePosition = m_pFile->pos();
	m_uAverageSpeed = m_uTotalReceivedBytes / uElapsedTime;

	if(m_uInstantSpeedInterval > INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS)
	{
		unsigned int uMSecsOfTheNextInterval = 0;
		if(m_uInstantSpeedInterval < (INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS + (INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS / 2)))
			uMSecsOfTheNextInterval = m_uInstantSpeedInterval - INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS;
		m_uInstantSpeed = (m_uInstantReceivedBytes * 1000) / m_uInstantSpeedInterval;
		m_uInstantReceivedBytes = 0;
		m_uInstantSpeedInterval = uMSecsOfTheNextInterval;
	}
	else
	{
		if(uElapsedTime <= INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_SECS)
			m_uInstantSpeed = m_uAverageSpeed;
	}
	m_pMutex->unlock();
}

void DccRecvThread::postMessageEvent(const char * m)
{
	KviThreadDataEvent<KviCString> * e = new KviThreadDataEvent<KviCString>(KVI_DCC_THREAD_EVENT_MESSAGE);
	e->setData(new KviCString(m));
	postEvent(parent(), e);
}

// FIXME: This stuff should be somewhat related to the 1448 bytes TCP basic packet size
//#define KVI_DCC_RECV_BLOCK_SIZE 8192
//#define KVI_DCC_RECV_75PERCENTOF_BLOCK_SIZE 6150
#define KVI_DCC_RECV_BLOCK_SIZE 16384
#define KVI_DCC_RECV_75PERCENTOF_BLOCK_SIZE 12280

void DccRecvThread::run()
{
	// take care of sleeping a bit if we can't read stuff
	// so we don't hog the CPU too much...
	int iFailedSelects = 0;
	// take care of sleeping a bit if we get a lot of short reads
	// so we don't hog the CPU too much...
	int iShortReadQuantifier = 0;
	// the algorithm is as follows:
	//    attempt to read KVI_DCC_RECV_BLOCK_SIZE bytes
	//    iShortReadQuantifier += ((KVI_DCC_RECV_75PERCENT_OF_BLOCK_SIZE - realReadedBytes) / 42);
	// thus we gain points if we read less than 75% of the requested size
	// and we loose points otherwise
	// there are nearly 24 points per KB
	//    if(iShortReadQuantifier > 10)
	//       msleep(iShortReadQuantifier);
	// also never sleep more than 500 msecs since it will
	// rise our exit latency too much

	m_pTimeInterval->mark();
	m_pMutex->lock();
	m_uStartTime = m_pTimeInterval->secondsCounter();
	m_pMutex->unlock();

	int iProbableTerminationTime = 0;

	m_pFile = new QFile(QString::fromUtf8(m_pOpt->szFileName.ptr()));

	bool bSend64BitAck = m_pOpt->bSend64BitAck && (m_pOpt->uTotalFileSize >> 32);

	if(m_pOpt->bResume)
	{
		if(!m_pFile->open(QIODevice::WriteOnly | QIODevice::Append))
		{
			postErrorEvent(KviError::CantOpenFileForAppending);
			goto exit_dcc;
		} // else pFile is already at end
	}
	else
	{
		if(!m_pFile->open(QIODevice::WriteOnly))
		{
			postErrorEvent(KviError::CantOpenFileForWriting);
			goto exit_dcc;
		}
	}

	if(m_pOpt->bSendZeroAck && (!m_pOpt->bNoAcks))
	{
		if(!sendAck(m_pFile->pos(), bSend64BitAck))
			goto exit_dcc;
	}

	for(;;)
	{
		// Dequeue events
		while(KviThreadEvent * e = dequeueEvent())
		{
			if(e->id() == KVI_THREAD_EVENT_TERMINATE)
			{
				delete e;
				goto exit_dcc;
			}
			else
			{
				// Other events are senseless to us
				delete e;
			}
		}

		bool bCanRead;
		bool bDummy;

		if(kvi_select(m_fd, &bCanRead, &bDummy, 15000))
		{
			// reset sleep time

			if(bCanRead)
			{
				iFailedSelects = 0;

				// Read a data block
				char buffer[KVI_DCC_RECV_BLOCK_SIZE];

				m_pMutex->lock(); // FIXME: how to remove this lock ?
				unsigned int uMaxPossible = (m_pOpt->uMaxBandwidth < MAX_DCC_BANDWIDTH_LIMIT) ? m_pOpt->uMaxBandwidth * INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_SECS : MAX_DCC_BANDWIDTH_LIMIT * INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_SECS;
				m_pMutex->unlock();
				unsigned int uToRead = uMaxPossible > m_uInstantReceivedBytes ? uMaxPossible - m_uInstantReceivedBytes : 0;
				if(uToRead > KVI_DCC_RECV_BLOCK_SIZE)
					uToRead = KVI_DCC_RECV_BLOCK_SIZE;

				if(uToRead > 0)
				{
					int readLen;
#ifdef COMPILE_SSL_SUPPORT
					if(m_pSSL)
					{
						readLen = m_pSSL->read(buffer, uToRead);
					}
					else
					{
#endif
						readLen = kvi_socket_recv(m_fd, buffer, uToRead);
#ifdef COMPILE_SSL_SUPPORT
					}
#endif

					if(readLen > 0)
					{
						// Readed something useful...write back
						if(((uint)(readLen + m_pFile->pos())) > m_pOpt->uTotalFileSize)
						{
							postMessageEvent(__tr_no_lookup_ctx("WARNING: the peer is sending garbage data past the end of the file", "dcc"));
							postMessageEvent(__tr_no_lookup_ctx("WARNING: ignoring data past the declared end of file and closing the connection", "dcc"));

							readLen = m_pOpt->uTotalFileSize - m_pFile->pos();
							if(readLen > 0)
							{
								if(m_pFile->write(buffer, readLen) != readLen)
									postErrorEvent(KviError::FileIOError);
							}
							break;
						}
						else
						{
							if(m_pFile->write(buffer, readLen) != readLen)
							{
								postErrorEvent(KviError::FileIOError);
								break;
							}
						}

						// Update stats
						m_uTotalReceivedBytes += readLen;
						m_uInstantReceivedBytes += readLen;

						updateStats();
						// Now send the ack
						if(m_pOpt->bNoAcks)
						{
							// No acks...
							// Interrupt if the whole file has been received
							if(m_pOpt->uTotalFileSize > 0)
							{
								if((quint64)m_pFile->pos() == m_pOpt->uTotalFileSize)
								{
									// Received the whole file...die
									KviThreadEvent * e = new KviThreadEvent(KVI_DCC_THREAD_EVENT_SUCCESS);
									postEvent(parent(), e);
									break;
								}
							}
						}
						else
						{
							// Must send the ack... the peer must close the connection

							if(!sendAck(m_pFile->pos(), bSend64BitAck))
								break;
						}

						// now take care of short reads
						iShortReadQuantifier += ((KVI_DCC_RECV_75PERCENTOF_BLOCK_SIZE - readLen) / 42);
						if(iShortReadQuantifier > 10)
						{
							// we're having short reads.. sleep a while
							// but don't allow it to go too high: 0.45 sec is really a lot
							if(iShortReadQuantifier > 500)
								iShortReadQuantifier = 500;
							msleep(iShortReadQuantifier);
						}
						else
						{
							// don't allow it to go too low
							if(iShortReadQuantifier < -500)
								iShortReadQuantifier = -500;
						}
					}
					else
					{
						updateStats();
// Read problem...

#ifdef COMPILE_SSL_SUPPORT
						if(m_pSSL)
						{
							// ssl error....?
							switch(m_pSSL->getProtocolError(readLen))
							{
								case KviSSL::ZeroReturn:
									//check again not necessary a connection closure!
									//if (!handleInvalidSocketRead(readLen)
									// break;
									readLen = 0;
									break;
								case KviSSL::Success:
								case KviSSL::WantRead:
								case KviSSL::WantWrite:
									// hmmm... DO NOT CALL handleInvalidSocketRead
									break;
								case KviSSL::SyscallError:
								{
									int iE = m_pSSL->getLastError(true);
									if(iE != 0)
									{
										raiseSSLError();
										postErrorEvent(KviError::SSLError);
										goto exit_dcc;
									}
								}
								break;
								case KviSSL::SSLError:
								{
									raiseSSLError();
									postErrorEvent(KviError::SSLError);
									goto exit_dcc;
								}
								break;
								default:
									// Raise unknown SSL ERROR
									postErrorEvent(KviError::SSLError);
									goto exit_dcc;
									break;
							}
						}
#endif

						if(readLen == 0)
						{
							// read EOF..
							if(((quint64)m_pFile->pos() == m_pOpt->uTotalFileSize) || (m_pOpt->uTotalFileSize == 0))
							{
								// success if we got the whole file or if we don't know the file size (we trust the peer)
								KviThreadEvent * e = new KviThreadEvent(KVI_DCC_THREAD_EVENT_SUCCESS);
								postEvent(parent(), e);
								break;
							}
						}
#ifdef COMPILE_SSL_SUPPORT
						if(!m_pSSL && !handleInvalidSocketRead(readLen))
							break;
#else
						if(!handleInvalidSocketRead(readLen))
							break;
#endif
					}
				}
				else
				{
					updateStats();

					// reached the bandwidth limit: slow down a bit
					if(m_uInstantSpeedInterval < (INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS - 100))
						msleep(100);
					else if(m_uInstantSpeedInterval < (INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS - 20))
						msleep(20);
				}
			}
			else
			{
				// Can't read stuff (can just write)
				updateStats();

				// sleep up to 300 msecs (if data arrives...we want low exit latency here)
				if(iFailedSelects < 100)
					iFailedSelects++;
				updateStats();
				if(iFailedSelects > 3)
					msleep(3 * iFailedSelects);

				if((quint64)m_pFile->pos() == m_pOpt->uTotalFileSize)
				{
					// Wait for the peer to close the connection
					if(iProbableTerminationTime == 0)
					{
						iProbableTerminationTime = (int)kvi_unixTime();
						m_pFile->flush();
						postMessageEvent(__tr_no_lookup_ctx("Data transfer terminated, waiting 30 seconds for the peer to close the connection...", "dcc"));
						// FIXME: Close the file ?
					}
					else
					{
						int iDiff = (((int)kvi_unixTime()) - iProbableTerminationTime);
						if(iDiff > 30)
						{
							// success if we got the whole file or if we don't know the file size (we trust the peer)
							postMessageEvent(__tr_no_lookup_ctx("Data transfer was terminated 30 seconds ago, closing the connection", "dcc"));
							KviThreadEvent * e = new KviThreadEvent(KVI_DCC_THREAD_EVENT_SUCCESS);
							postEvent(parent(), e);
							break;
						}
					}
				}
			}
			// include the artificial delay if needed
			if(m_pOpt->iIdleStepLengthInMSec > 0)
				msleep(m_pOpt->iIdleStepLengthInMSec);
		}
		else
		{
			// sleep up to 200 msecs (if data arrives...we want low exit latency here)
			if(iFailedSelects < 100)
				iFailedSelects++;
			updateStats();
			if(iFailedSelects > 3)
				msleep(2 * iFailedSelects);
		}
	}

exit_dcc:
	if(m_pFile)
	{
		m_pFile->close();
		delete m_pFile;
		m_pFile = nullptr;
	}

#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		KviSSLMaster::freeSSL(m_pSSL);
		m_pSSL = nullptr;
	}
#endif

	kvi_socket_close(m_fd);
	m_fd = KVI_INVALID_SOCKET;
}

void DccRecvThread::initGetInfo()
{
	m_pMutex->lock();
}

void DccRecvThread::doneGetInfo()
{
	m_pMutex->unlock();
}

DccSendThread::DccSendThread(QObject * par, kvi_socket_t fd, KviDccSendThreadOptions * opt)
    : DccThread(par, fd)
{
	m_pOpt = opt;
	// stats
	m_uAverageSpeed = 0;
	m_uInstantSpeed = 0;
	m_uFilePosition = 0;
	m_uTotalSentBytes = 0;
	m_pTimeInterval = new KviMSecTimeInterval();
	m_uStartTime = 0;
	m_uInstantSpeedInterval = 0;
}

DccSendThread::~DccSendThread()
{
	if(m_pOpt)
		delete m_pOpt;
	delete m_pTimeInterval;
}

void DccSendThread::updateStats()
{
	m_uInstantSpeedInterval += m_pTimeInterval->mark();

	m_pMutex->lock();
	unsigned long uElapsedTime = m_pTimeInterval->secondsCounter() - m_uStartTime;
	if(uElapsedTime < 1)
		uElapsedTime = 1;

	if(m_pOpt->bNoAcks)
	{
		// There are no acks : the avg bandwidth is based on the sent bytes
		m_uAverageSpeed = m_uTotalSentBytes / uElapsedTime;
	}
	else
	{
		// acknowledges : we compute the avg bandwidth based on the acks we receive
		m_uAverageSpeed = (m_uAckedBytes - m_pOpt->uStartPosition) / uElapsedTime;
	}

	if(m_uInstantSpeedInterval >= INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS)
	{
		// we often overcount the time interval of 10-20 msecs
		// and thus our bandwidth is used less than requested.
		// for this reason we try to account the time in excess
		// to the next period in order to balance the bandwidth usage.
		unsigned long uMSecsOfNextPeriodUsed = 0;
		if(m_uInstantSpeedInterval > INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS)
		{
			if(m_uInstantSpeedInterval < (INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS + (INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS / 2)))
			{
				uMSecsOfNextPeriodUsed = m_uInstantSpeedInterval - INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS;
				m_uInstantSpeedInterval = INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS;
			}
			// else we have been delayed for a time comparable to a period
			// and thus we can't recover the bandwidth... let it go as it does...
		}
		m_uInstantSpeed = (m_uInstantSentBytes * 1000) / m_uInstantSpeedInterval;
		m_uInstantSpeedInterval = uMSecsOfNextPeriodUsed;
		m_uInstantSentBytes = 0;
	}
	else
	{
		if(uElapsedTime <= INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_SECS)
			m_uInstantSpeed = m_uAverageSpeed;
	}
	m_pMutex->unlock();
}

union _ack_buffer {
	char cAckBuffer[4];
	quint32 i32AckBuffer;
};

void DccSendThread::run()
{
	m_pTimeInterval->mark();
	m_pMutex->lock();
	m_uStartTime = m_pTimeInterval->secondsCounter();
	m_pMutex->unlock();

	m_uTotalSentBytes = 0;
	m_uInstantSentBytes = 0;
	int iFailedSelects = 0;
	_ack_buffer ackbuffer;
	int iBytesInAckBuffer = 0;
	quint32 uLastAck = 0;
	quint64 uTotLastAck = 0;
	bool bAckHack = false;
	quint64 iAckHackRounds = 0;

	if(m_pOpt->iPacketSize < 32)
		m_pOpt->iPacketSize = 32;
	char * buffer = (char *)KviMemory::allocate(m_pOpt->iPacketSize * sizeof(char));

	QFile * pFile = new QFile(QString::fromUtf8(m_pOpt->szFileName.ptr()));

	if(!pFile->open(QIODevice::ReadOnly))
	{
		postErrorEvent(KviError::CantOpenFileForReading);
		goto exit_dcc;
	}

	if(pFile->size() < 1)
	{
		postErrorEvent(KviError::CantSendAZeroSizeFile);
		goto exit_dcc;
	}

	if(pFile->size() >= 0xffffffff)
	{
		//dcc acks support only files up to 4GiB
		bAckHack = true;
	}

	if(m_pOpt->uStartPosition > 0)
	{
		// seek
		if(!(pFile->seek(m_pOpt->uStartPosition)))
		{
			postErrorEvent(KviError::FileIOError);
			goto exit_dcc;
		}
	}

	uLastAck = m_pOpt->uStartPosition;

	for(;;)
	{
		// Dequeue events
		while(KviThreadEvent * e = dequeueEvent())
		{
			if(e->id() == KVI_THREAD_EVENT_TERMINATE)
			{
				delete e;
				goto exit_dcc;
			}
			else
			{
				// Other events are senseless to us
				delete e;
			}
		}

		bool bCanRead;
		bool bCanWrite;

		if(kvi_select(m_fd, &bCanRead, &bCanWrite, 15000))
		{
			// reset the sleep time
			iFailedSelects = 0;
			if(bCanRead)
			{
				if(!m_pOpt->bNoAcks)
				{
					int iAckBytesToRead = 4 - iBytesInAckBuffer;

					int readLen;
#ifdef COMPILE_SSL_SUPPORT
					if(m_pSSL)
					{
						readLen = m_pSSL->read((ackbuffer.cAckBuffer + iBytesInAckBuffer), iAckBytesToRead);
					}
					else
					{
#endif
						readLen = kvi_socket_recv(m_fd, (ackbuffer.cAckBuffer + iBytesInAckBuffer), iAckBytesToRead);
#ifdef COMPILE_SSL_SUPPORT
					}
#endif

					if(readLen > 0)
					{
						iBytesInAckBuffer += readLen;
						if(iBytesInAckBuffer == 4)
						{
							quint32 iNewAck = ntohl(ackbuffer.i32AckBuffer);
							if(iNewAck > pFile->pos())
							{
								// the peer is drunk or is trying to fool us
								postErrorEvent(KviError::AcknowledgeError);
								break;
							}
							if(iNewAck < uLastAck)
							{
								if(bAckHack)
								{
									//we reached the 4gb ack limit
									iAckHackRounds++;
								}
								else
								{
									// the peer is drunk or is trying to fool us
									postErrorEvent(KviError::AcknowledgeError);
									break;
								}
							}
							uLastAck = iNewAck;
							if(bAckHack)
							{
								uTotLastAck = (iAckHackRounds << 32) + iNewAck;
							}
							else
							{

								uTotLastAck = iNewAck;
							}
							iBytesInAckBuffer = 0;
						}
					}
					else
					{
#ifdef COMPILE_SSL_SUPPORT
						if(m_pSSL)
						{
							// ssl error....?
							switch(m_pSSL->getProtocolError(readLen))
							{

								case KviSSL::ZeroReturn:
									//if (!handleInvalidSocketRead(readLen)
									// break;
									readLen = 0;
									break;
								case KviSSL::Success:
								case KviSSL::WantRead:
								case KviSSL::WantWrite:
									// hmmm...
									break;
								case KviSSL::SyscallError:
								{
									int iE = m_pSSL->getLastError(true);
									if(iE != 0)
									{
										raiseSSLError();
										postErrorEvent(KviError::SSLError);
										goto exit_dcc;
									}
								}
								break;
								case KviSSL::SSLError:
								{
									raiseSSLError();
									postErrorEvent(KviError::SSLError);
									goto exit_dcc;
								}
								break;
								default:
									// Raise unknown SSL ERROR
									postErrorEvent(KviError::SSLError);
									goto exit_dcc;
									break;
							}
						}

						if(!m_pSSL && !handleInvalidSocketRead(readLen))
							break;
#else
						if(!handleInvalidSocketRead(readLen))
							break;
#endif
					}

					// update stats
					m_pMutex->lock(); // is this really necessary ?
					m_uAckedBytes = uTotLastAck;
					m_pMutex->unlock();

					if(uLastAck >= (quint64)pFile->size())
					{
						KviThreadEvent * e = new KviThreadEvent(KVI_DCC_THREAD_EVENT_SUCCESS);
						postEvent(parent(), e);
						break;
					}
				}
				else
				{
					// No acknowledges
					if(m_pOpt->bIsTdcc)
					{
						// We expect the remote end to close the connection when the whole file has been sent
						if(pFile->atEnd())
						{
							int iAck;
							int readLen;
#ifdef COMPILE_SSL_SUPPORT
							if(m_pSSL)
							{
								readLen = m_pSSL->read((char *)&iAck, 4);
							}
							else
							{
#endif
								readLen = kvi_socket_recv(m_fd, (char *)&iAck, 4);
#ifdef COMPILE_SSL_SUPPORT
							}
#endif
							if(readLen == 0)
							{
								// done...success
								updateStats();
								KviThreadEvent * e = new KviThreadEvent(KVI_DCC_THREAD_EVENT_SUCCESS);
								postEvent(parent(), e);
								break;
							}
							else
							{
								if(readLen < 0)
								{
#ifdef COMPILE_SSL_SUPPORT
									if(m_pSSL)
									{
										// ssl error....?
										switch(m_pSSL->getProtocolError(readLen))
										{

											case KviSSL::ZeroReturn:
												readLen = 0;
												break;
											case KviSSL::Success:
											case KviSSL::WantRead:
											case KviSSL::WantWrite:
												// hmmm...
												break;
											case KviSSL::SyscallError:
											{
												int iE = m_pSSL->getLastError(true);
												if(iE != 0)
												{
													raiseSSLError();
													postErrorEvent(KviError::SSLError);
													goto exit_dcc;
												}
											}
											break;
											case KviSSL::SSLError:
											{
												raiseSSLError();
												postErrorEvent(KviError::SSLError);
												goto exit_dcc;
											}
											break;
											default:
												// Raise unknown SSL ERROR
												postErrorEvent(KviError::SSLError);
												goto exit_dcc;
												break;
										}
									}

									if(!m_pSSL && !handleInvalidSocketRead(readLen))
										break;
#else
									if(!handleInvalidSocketRead(readLen))
										break;
#endif
								}
								else
								{
									KviThreadDataEvent<KviCString> * e = new KviThreadDataEvent<KviCString>(KVI_DCC_THREAD_EVENT_MESSAGE);
									e->setData(new KviCString(__tr2qs_ctx("WARNING: received data in a DCC TSEND, there should be no acknowledges", "dcc")));
									postEvent(parent(), e);
								}
							}
						}
					}
				}
			}
			if(bCanWrite)
			{
				if(!pFile->atEnd())
				{
					if(m_pOpt->bFastSend || m_pOpt->bNoAcks || (uLastAck == (quint64)pFile->pos()))
					{
						// maximum readable size
						qint64 toRead = pFile->size() - pFile->pos();
						// the max number of bytes we can send in this interval (bandwidth limit)
						m_pMutex->lock(); // FIXME: how to remove this lock ?
						uint uMaxPossible = m_pOpt->uMaxBandwidth < MAX_DCC_BANDWIDTH_LIMIT ? m_pOpt->uMaxBandwidth * INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_SECS : MAX_DCC_BANDWIDTH_LIMIT * INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_SECS;
						m_pMutex->unlock();
						if(uMaxPossible < m_uInstantSentBytes)
							toRead = 0; // already sent too much!
						else
						{
							uMaxPossible -= m_uInstantSentBytes;
							if(toRead > uMaxPossible)
								toRead = uMaxPossible;
						}
						// limit to packet size
						if(toRead > m_pOpt->iPacketSize)
							toRead = m_pOpt->iPacketSize;

						int written = 0;
						if(toRead > 0)
						{
							// read data
							int readed = pFile->read(buffer, toRead);
							if(readed < toRead)
							{
								postErrorEvent(KviError::FileIOError);
								break;
							}
// send it out

#ifdef COMPILE_SSL_SUPPORT
							if(m_pSSL)
							{
								written = m_pSSL->write(buffer, toRead);
							}
							else
							{
#endif
								written = kvi_socket_send(m_fd, buffer, toRead);
#ifdef COMPILE_SSL_SUPPORT
							}
#endif

							if(written < toRead)
							{
								if(written < 0)
								{
#ifdef COMPILE_SSL_SUPPORT
									if(m_pSSL)
									{
										// ops...might be an SSL error
										switch(m_pSSL->getProtocolError(written))
										{
											case KviSSL::Success:
											case KviSSL::WantWrite:
											case KviSSL::WantRead:
												// Async continue...
												goto handle_system_error;
												break;
											case KviSSL::SyscallError:
												{
													int iSSLErr = m_pSSL->getLastError(true);
													if(iSSLErr != 0)
													{
														raiseSSLError();
														postErrorEvent(KviError::SSLError);
														goto exit_dcc;
													}
													else
													{
														goto handle_system_error;
													}
												}
												break;
											case KviSSL::SSLError:
												raiseSSLError();
												postErrorEvent(KviError::SSLError);
												goto exit_dcc;
												break;
											default:
												postErrorEvent(KviError::SSLError);
												goto exit_dcc;
												break;
										}
									}

									if(!m_pSSL && !handleInvalidSocketRead(written))
										break;

#else
									if(!handleInvalidSocketRead(written))
										break;
#endif

								handle_system_error:
									int err = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
									if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
									if((err != EAGAIN) && (err != EINTR))
#endif
									{
										postErrorEvent(KviError::translateSystemError(err));
										goto exit_dcc;
									}
								}
								else
								{
									// seek back to the right position
									pFile->seek(pFile->pos() - (toRead - written));
								}
							}
						}
						else
						{
							// just nothing to send out in this interval
							// sleep a while
							if(m_uInstantSpeedInterval < (INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS - 100))
							{
								msleep(100);
							}
							else if(m_uInstantSpeedInterval < (INSTANT_BANDWIDTH_CHECK_INTERVAL_IN_MSECS - 20))
							{
								msleep(20);
							}
						}

						m_uTotalSentBytes += written;
						m_uInstantSentBytes += written;
						m_uFilePosition = pFile->pos();
						updateStats();
					}
				}
				else
				{
					if(m_pOpt->bNoAcks && !m_pOpt->bIsTdcc)
					{
						// at end of the file in a blind dcc send...
						// not in a tdcc: we can close the file...
						updateStats();
						KviThreadEvent * e = new KviThreadEvent(KVI_DCC_THREAD_EVENT_SUCCESS);
						postEvent(parent(), e);
						break;
					}
					else
					{
						// upload finished but we're waiting for the last ack
						// sleep a bit: don't lag the kernie too much while waiting
						msleep(100);
					}
				}
			}
		}
		else
		{
			// after 2 failed selects start to sleep
			if(iFailedSelects > 3)
			{
				// sleep up to 200 msecs
				if(iFailedSelects < 100)
					iFailedSelects++;
				msleep(3 * iFailedSelects);
			}
			else
			{
				iFailedSelects++;
			}
		}

		// include the artificial delay if needed
		if(m_pOpt->iIdleStepLengthInMSec > 0)
		{
			msleep(m_pOpt->iIdleStepLengthInMSec);
		}
	}

exit_dcc:
	KviMemory::free(buffer);
	pFile->close();
	delete pFile;
	pFile = nullptr;

#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		KviSSLMaster::freeSSL(m_pSSL);
		m_pSSL = nullptr;
	}
#endif
	kvi_socket_close(m_fd);
	m_fd = KVI_INVALID_SOCKET;
}

void DccSendThread::initGetInfo()
{
	m_pMutex->lock();
}

void DccSendThread::doneGetInfo()
{
	m_pMutex->unlock();
}

DccFileTransfer::DccFileTransfer(DccDescriptor * dcc)
    : KviFileTransfer()
{
	init(); // ensure we're initialized
	g_pDccFileTransfers->append(this);

	m_pResumeTimer = nullptr;
	m_pBandwidthDialog = nullptr;

	m_szTransferIdString = QString(__tr2qs_ctx("TRANSFER %1", "dcc")).arg(id());

	m_pDescriptor = dcc;
	m_pDescriptor->setTransfer(this);

	m_pMarshal = new DccMarshal(this);

	connect(m_pMarshal, SIGNAL(error(KviError::Code)), this, SLOT(handleMarshalError(KviError::Code)));
	connect(m_pMarshal, SIGNAL(connected()), this, SLOT(connected()));
	connect(m_pMarshal, SIGNAL(inProgress()), this, SLOT(connectionInProgress()));
#ifdef COMPILE_SSL_SUPPORT
	connect(m_pMarshal, SIGNAL(startingSSLHandshake()), this, SLOT(startingSSLHandshake()));
	connect(m_pMarshal, SIGNAL(sslError(const char *)), this, SLOT(sslError(const char *)));
#endif

	m_szDccType = dcc->bRecvFile ? "RECV" : "SEND";
	if(dcc->bIsTdcc)
		m_szDccType.prepend("T");
#ifdef COMPILE_SSL_SUPPORT
	if(dcc->bIsSSL)
		m_szDccType.prepend("S");
#endif
	m_pSlaveRecvThread = nullptr;
	m_pSlaveSendThread = nullptr;

	m_tTransferStartTime = 0;
	m_tTransferEndTime = 0;

	m_szStatusString = __tr2qs_ctx("Setting up the connection", "dcc");
	m_eGeneralStatus = Connecting;

	bool bOk;
	m_uTotalFileSize = dcc->bRecvFile ? dcc->szFileSize.toULongLong(&bOk) : dcc->szLocalFileSize.toULongLong(&bOk);
	if(!bOk)
		m_uTotalFileSize = 0;

	if(m_pDescriptor->bRecvFile)
		m_uMaxBandwidth = KVI_OPTION_BOOL(KviOption_boolLimitDccRecvSpeed) ? KVI_OPTION_UINT(KviOption_uintMaxDccRecvSpeed) : MAX_DCC_BANDWIDTH_LIMIT;
	else
		m_uMaxBandwidth = KVI_OPTION_BOOL(KviOption_boolLimitDccSendSpeed) ? KVI_OPTION_UINT(KviOption_uintMaxDccSendSpeed) : MAX_DCC_BANDWIDTH_LIMIT;

	startConnection();
}

DccFileTransfer::~DccFileTransfer()
{
	g_pDccFileTransfers->removeRef(this);

	if(m_pResumeTimer)
		delete m_pResumeTimer;
	if(m_pBandwidthDialog)
		delete m_pBandwidthDialog;

	if(m_pSlaveRecvThread)
	{
		m_pSlaveRecvThread->terminate();
		delete m_pSlaveRecvThread;
		m_pSlaveRecvThread = nullptr;
	}

	if(m_pSlaveSendThread)
	{
		m_pSlaveSendThread->terminate();
		delete m_pSlaveSendThread;
		m_pSlaveSendThread = nullptr;
	}

	KviThreadManager::killPendingEvents(this);

	delete m_pDescriptor;
	delete m_pMarshal;
}

void DccFileTransfer::bandwidthDialogDestroyed()
{
	m_pBandwidthDialog = nullptr;
}

KviWindow * DccFileTransfer::eventWindow()
{
	KviWindow * w = transferWindow();
	if(w)
		return w;
	return m_pDescriptor->console();
}

void DccFileTransfer::startConnection()
{
	if(!(m_pDescriptor->bActive))
	{
		// PASSIVE CONNECTION
		m_szStatusString = __tr2qs_ctx("Attempting a passive DCC %1 connection", "dcc").arg(m_szDccType.ptr());
		outputAndLog(m_szStatusString);
	}
	else
	{
		// ACTIVE CONNECTION
		m_szStatusString = __tr2qs_ctx("Attempting an active DCC %1 connection", "dcc").arg(m_szDccType.ptr());
		outputAndLog(m_szStatusString);
	}

	if(m_pDescriptor->bResume && m_pDescriptor->bRecvFile)
	{
		QString fName;
		KviIrcServerParser::encodeCtcpParameter(m_pDescriptor->szFileName.toUtf8().data(), fName);
		if(m_pDescriptor->isZeroPortRequest())
		{
			m_pDescriptor->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC RESUME %s %s %s %s%c",
			    m_pDescriptor->console()->connection()->encodeText(m_pDescriptor->szNick).data(),
			    0x01,
			    m_pDescriptor->console()->connection()->encodeText(fName).data(),
			    m_pDescriptor->szPort.toUtf8().data(),
			    m_pDescriptor->szLocalFileSize.toUtf8().data(),
			    m_pDescriptor->zeroPortRequestTag(), 0x01);
		}
		else
		{
			m_pDescriptor->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC RESUME %s %s %s%c",
			    m_pDescriptor->console()->connection()->encodeText(m_pDescriptor->szNick).data(),
			    0x01,
			    m_pDescriptor->console()->connection()->encodeText(fName).data(),
			    m_pDescriptor->szPort.toUtf8().data(),
			    m_pDescriptor->szLocalFileSize.toUtf8().data(), 0x01);
		}
		m_szStatusString = __tr2qs_ctx("Sent DCC RESUME request to %1, waiting for ACCEPT", "dcc").arg(m_pDescriptor->szNick);
		outputAndLog(m_szStatusString);

		// setup the resume timer: we don't want to wait forever

		if(KVI_OPTION_UINT(KviOption_uintDccSocketTimeout) < 5)
			KVI_OPTION_UINT(KviOption_uintDccSocketTimeout) = 5;

		if(m_pResumeTimer)
			delete m_pResumeTimer;
		m_pResumeTimer = new QTimer(this);
		connect(m_pResumeTimer, SIGNAL(timeout()), this, SLOT(resumeTimedOut()));
		m_pResumeTimer->setInterval(KVI_OPTION_UINT(KviOption_uintDccSocketTimeout) * 1000);
		m_pResumeTimer->setSingleShot(true);
		m_pResumeTimer->start();
	}
	else
	{
		listenOrConnect();
	}

	displayUpdate();
}

void DccFileTransfer::listenOrConnect()
{
	if(!(m_pDescriptor->bActive))
	{
#ifdef COMPILE_SSL_SUPPORT
		KviError::Code eError = m_pMarshal->dccListen(m_pDescriptor->szListenIp, m_pDescriptor->szListenPort, m_pDescriptor->bDoTimeout, m_pDescriptor->bIsSSL);
#else
		KviError::Code eError = m_pMarshal->dccListen(m_pDescriptor->szListenIp, m_pDescriptor->szListenPort, m_pDescriptor->bDoTimeout);
#endif
		if(eError != KviError::Success)
			handleMarshalError(eError);
	}
	else
	{
#ifdef COMPILE_SSL_SUPPORT
		KviError::Code eError = m_pMarshal->dccConnect(m_pDescriptor->szIp.toUtf8().data(), m_pDescriptor->szPort.toUtf8().data(), m_pDescriptor->bDoTimeout, m_pDescriptor->bIsSSL);
#else
		KviError::Code eError = m_pMarshal->dccConnect(m_pDescriptor->szIp.toUtf8().data(), m_pDescriptor->szPort.toUtf8().data(), m_pDescriptor->bDoTimeout);
#endif
		if(eError != KviError::Success)
			handleMarshalError(eError);
	}

	displayUpdate();
}

void DccFileTransfer::resumeTimedOut()
{
	if(m_pResumeTimer)
	{
		delete m_pResumeTimer;
		m_pResumeTimer = nullptr;
	}
	handleMarshalError(KviError::ConnectionTimedOut);
}

KviWindow * DccFileTransfer::dccMarshalOutputWindow()
{
	return transferWindow();
}

const char * DccFileTransfer::dccMarshalOutputContextString()
{
	return m_szTransferIdString.toUtf8().data();
}

QString DccFileTransfer::localFileName()
{
	return m_pDescriptor->szLocalFileName;
}

void DccFileTransfer::abort()
{
	if(m_pSlaveRecvThread)
		m_pSlaveRecvThread->terminate();
	if(m_pSlaveSendThread)
		m_pSlaveSendThread->terminate();
	if(m_pMarshal)
		m_pMarshal->abort();

	if(m_pDescriptor->bRecvFile)
		g_pApp->fileDownloadTerminated(false, m_pDescriptor->szFileName.toUtf8().data(), m_pDescriptor->szLocalFileName.toUtf8().data(), m_pDescriptor->szNick.toUtf8().data(), __tr_ctx("Aborted", "dcc"));

	QString tmp;

	if(m_pSlaveRecvThread)
		tmp.setNum(m_pSlaveRecvThread->receivedBytes());
	else if(m_pSlaveSendThread)
		tmp.setNum(m_pSlaveSendThread->sentBytes());
	else
		tmp = '0';

	m_eGeneralStatus = Failure;
	m_tTransferEndTime = kvi_unixTime();
	m_szStatusString = __tr2qs_ctx("Transfer failed: ", "dcc");
	m_szStatusString += __tr2qs_ctx("Aborted", "dcc");

	KVS_TRIGGER_EVENT_3(KviEvent_OnDCCFileTransferFailed, eventWindow(), QString("Aborted by user"), tmp, m_pDescriptor->idString());

	outputAndLog(KVI_OUT_DCCERROR, m_szStatusString);
	displayUpdate();
}

void DccFileTransfer::fillContextPopup(QMenu * m)
{
	m->addAction(__tr2qs_ctx("Configure Bandwidth...", "dcc"), this, SLOT(configureBandwidth()));
	m->addSeparator();
	m->addAction(__tr2qs_ctx("Resend DCC", "dcc"), this, SLOT(retryDCC()));
	m->addAction(__tr2qs_ctx("Resend TDCC", "dcc"), this, SLOT(retryTDCC()));
	m->addAction(__tr2qs_ctx("Resend RevDCC", "dcc"), this, SLOT(retryRevDCC()));
	QAction * pAction = m->addAction(__tr2qs_ctx("Abort", "dcc"), this, SLOT(abort()));
	if(!active())
		pAction->setEnabled(false);
}

void DccFileTransfer::configureBandwidth()
{
	if(m_pBandwidthDialog)
		return;
	m_pBandwidthDialog = new DccFileTransferBandwidthDialog(g_pMainWindow, this);
	connect(m_pBandwidthDialog, SIGNAL(destroyed()), this, SLOT(bandwidthDialogDestroyed()));
	m_pBandwidthDialog->setModal(true);
	m_pBandwidthDialog->show();
}

void DccFileTransfer::retryDCC()
{
	abort();
	QString szRemoteNick = m_pDescriptor->remoteNick();
	QString szFileName = m_pDescriptor->localFileName();
	QString szId = m_pDescriptor->idString();
	KviQString::escapeKvs(&szRemoteNick, KviQString::EscapeSpace);
	KviQString::escapeKvs(&szFileName, KviQString::EscapeSpace);

	QString szCommand = "dcc.send -r=$console($dcc.irccontext(" + szId + ")) " + szRemoteNick + " " + szFileName;
	KviKvsScript::run(szCommand, g_pActiveWindow);
}

void DccFileTransfer::retryTDCC()
{
	abort();
	QString szRemoteNick = m_pDescriptor->remoteNick();
	QString szFileName = m_pDescriptor->localFileName();
	QString szId = m_pDescriptor->idString();
	KviQString::escapeKvs(&szRemoteNick, KviQString::EscapeSpace);
	KviQString::escapeKvs(&szFileName, KviQString::EscapeSpace);

	QString szCommand = "dcc.send -r=$console($dcc.irccontext(" + szId + ")) -t " + szRemoteNick + " " + szFileName;
	KviKvsScript::run(szCommand, g_pActiveWindow);
}

void DccFileTransfer::retryRevDCC()
{
	abort();
	QString szRemoteNick = m_pDescriptor->remoteNick();
	QString szFileName = m_pDescriptor->localFileName();
	QString szId = m_pDescriptor->idString();
	KviQString::escapeKvs(&szRemoteNick, KviQString::EscapeSpace);
	KviQString::escapeKvs(&szFileName, KviQString::EscapeSpace);

	QString szCommand = "dcc.rsend -z -r=$console($dcc.irccontext(" + szId + ")) " + szRemoteNick + " " + szFileName;
	KviKvsScript::run(szCommand, g_pActiveWindow);
}

void DccFileTransfer::fillStatusString(QString & szBuffer)
{
	switch(m_eGeneralStatus)
	{
		case Connecting:
			szBuffer = "connecting";
			break;
		case Transferring:
			szBuffer = "transferring";
			break;
		case Failure:
			szBuffer = "failure";
			break;
		case Success:
			szBuffer = "success";
			break;
		default:
			szBuffer = "unknown";
			break;
	}
}

bool DccFileTransfer::active()
{
	return ((m_eGeneralStatus == Connecting) || (m_eGeneralStatus == Transferring));
}

int DccFileTransfer::bandwidthLimit()
{
	int iLimit = m_uMaxBandwidth; // we have the cached value anyway...
	if(m_pDescriptor->bRecvFile)
	{
		if(m_pSlaveRecvThread)
		{
			m_pSlaveRecvThread->initGetInfo();
			iLimit = (int)m_pSlaveRecvThread->bandwidthLimit();
			m_pSlaveRecvThread->doneGetInfo();
			if(iLimit < 0)
				iLimit = MAX_DCC_BANDWIDTH_LIMIT;
		}
	}
	else
	{
		if(m_pSlaveSendThread)
		{
			m_pSlaveSendThread->initGetInfo();
			iLimit = (int)m_pSlaveSendThread->bandwidthLimit();
			m_pSlaveSendThread->doneGetInfo();
			if(iLimit < 0)
				iLimit = MAX_DCC_BANDWIDTH_LIMIT;
		}
	}
	return iLimit;
}

void DccFileTransfer::setBandwidthLimit(int iVal)
{
	if(iVal < 0)
		iVal = MAX_DCC_BANDWIDTH_LIMIT;
	if(iVal > MAX_DCC_BANDWIDTH_LIMIT)
		iVal = MAX_DCC_BANDWIDTH_LIMIT;
	m_uMaxBandwidth = iVal;
	if(m_pDescriptor->bRecvFile)
	{
		if(m_pSlaveRecvThread)
		{
			m_pSlaveRecvThread->initGetInfo();
			m_pSlaveRecvThread->setBandwidthLimit(iVal);
			m_pSlaveRecvThread->doneGetInfo();
		}
	}
	else
	{
		if(m_pSlaveSendThread)
		{
			m_pSlaveSendThread->initGetInfo();
			m_pSlaveSendThread->setBandwidthLimit(iVal);
			m_pSlaveSendThread->doneGetInfo();
		}
	}
}

unsigned int DccFileTransfer::averageSpeed()
{
	unsigned int uAvgBandwidth = 0;
	if(m_pDescriptor->bRecvFile)
	{
		if(m_pSlaveRecvThread)
		{
			m_pSlaveRecvThread->initGetInfo();
			uAvgBandwidth = m_pSlaveRecvThread->averageSpeed();
			m_pSlaveRecvThread->doneGetInfo();
		}
	}
	else
	{
		if(m_pSlaveSendThread)
		{
			m_pSlaveSendThread->initGetInfo();
			uAvgBandwidth = m_pSlaveSendThread->averageSpeed();
			m_pSlaveSendThread->doneGetInfo();
		}
	}
	return uAvgBandwidth;
}

unsigned int DccFileTransfer::instantSpeed()
{
	unsigned int uInstBandwidth = 0;
	if(m_pDescriptor->bRecvFile)
	{
		if(m_pSlaveRecvThread)
		{
			m_pSlaveRecvThread->initGetInfo();
			uInstBandwidth = m_pSlaveRecvThread->instantSpeed();
			m_pSlaveRecvThread->doneGetInfo();
		}
	}
	else
	{
		if(m_pSlaveSendThread)
		{
			m_pSlaveSendThread->initGetInfo();
			uInstBandwidth = m_pSlaveSendThread->instantSpeed();
			m_pSlaveSendThread->doneGetInfo();
		}
	}
	return uInstBandwidth;
}

unsigned int DccFileTransfer::transferredBytes()
{
	unsigned int uTransferred = 0;
	if(m_pDescriptor->bRecvFile)
	{
		if(m_pSlaveRecvThread)
		{
			m_pSlaveRecvThread->initGetInfo();
			uTransferred = m_pSlaveRecvThread->filePosition();
			m_pSlaveRecvThread->doneGetInfo();
		}
	}
	else
	{
		if(m_pSlaveSendThread)
		{
			m_pSlaveSendThread->initGetInfo();
			uTransferred = m_pSlaveSendThread->filePosition();
			m_pSlaveSendThread->doneGetInfo();
		}
	}
	return uTransferred;
}

void DccFileTransfer::displayPaint(QPainter * p, int column, QRect rect)
{
	int width = rect.width(), height = rect.height();
	QString txt;
	bool bIsTerminated = ((m_eGeneralStatus == Success) || (m_eGeneralStatus == Failure));

	switch(column)
	{
		case COLUMN_TRANSFERTYPE:
		{
			if(!g_pDccFileTransferIcon)
				break;
			int xoffset = 0;
			int yoffset = 0;
			if(m_pDescriptor->bRecvFile)
				yoffset = 64;
			switch(m_eGeneralStatus)
			{
				case Connecting:
					xoffset = 0;
					break;
				case Transferring:
					xoffset = 48;
					break;
				case Success:
					xoffset = 96;
					break;
				case Failure:
					xoffset = 144;
					break;
			}
			// 48 is width, 64 is height
			p->drawPixmap(width / 2 - 48 / 2, rect.top() + height / 2 - 64 / 2, *g_pDccFileTransferIcon, xoffset, yoffset, 48, 64);
		}
		break;
		case COLUMN_FILEINFO:
		{

			QFontMetrics fm(p->font());

			QString szFrom = __tr2qs_ctx("From: ", "dcc");
			QString szTo = __tr2qs_ctx("To: ", "dcc");

			int daW1 = fm.width(szFrom);
			int daW2 = fm.width(szTo);
			if(daW1 < daW2)
				daW1 = daW2;
			int iLineSpacing = fm.lineSpacing();

			int iY = rect.top() + 4;

			p->setPen(Qt::black);

			KviCString szRemote(KviCString::Format, "dcc://%s@%s:%s/%s", m_pDescriptor->szNick.toUtf8().data(), m_pDescriptor->szIp.toUtf8().data(), m_pDescriptor->szPort.toUtf8().data(),
			    m_pDescriptor->szFileName.toUtf8().data());

			p->drawText(rect.left() + 4 + daW1, iY, width - (8 + daW1), height - 8, Qt::AlignTop | Qt::AlignLeft,
			    m_pDescriptor->bRecvFile ? szRemote.ptr() : m_pDescriptor->szLocalFileName.toUtf8().data());
			iY += iLineSpacing;

			p->drawText(rect.left() + 4 + daW1, iY, width - (8 + daW1), height - 8, Qt::AlignTop | Qt::AlignLeft,
			    m_pDescriptor->bRecvFile ? m_pDescriptor->szLocalFileName.toUtf8().data() : szRemote.ptr());
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

			int iW = width - 8;
			uint uAvgBandwidth = 0;
			uint uInstantSpeed = 0;
			quint64 uAckedBytes = 0;
			quint64 uTransferred = 0;
			int iEta = -1;

			if(m_pDescriptor->bRecvFile)
			{
				if(m_pSlaveRecvThread)
				{
					m_pSlaveRecvThread->initGetInfo();
					uAvgBandwidth = m_pSlaveRecvThread->averageSpeed();
					uInstantSpeed = m_pSlaveRecvThread->instantSpeed();
					uTransferred = m_pSlaveRecvThread->filePosition();
					m_pSlaveRecvThread->doneGetInfo();
				}
			}
			else
			{
				if(m_pSlaveSendThread)
				{
					m_pSlaveSendThread->initGetInfo();
					uAvgBandwidth = m_pSlaveSendThread->averageSpeed();
					uInstantSpeed = m_pSlaveSendThread->instantSpeed();
					uTransferred = m_pSlaveSendThread->filePosition();
					uAckedBytes = m_pSlaveSendThread->ackedBytes();
					m_pSlaveSendThread->doneGetInfo();
				}
			}

			p->setPen(bIsTerminated ? Qt::lightGray : QColor(210, 210, 240));
			p->drawRect(rect.left() + 4, rect.top() + 4, iW, 12);

			iW -= 2;

			if(m_uTotalFileSize > 0)
			{
				if(uAvgBandwidth > 0)
				{
					quint64 uRemaining = m_uTotalFileSize - uTransferred;
					iEta = uRemaining / uAvgBandwidth;
				}

				if(!m_pDescriptor->bNoAcks && (uAckedBytes > 0) && (uAckedBytes < uTransferred))
				{
					// we are sending a file and are getting acks

					double dPerc1 = (double)(((double)uTransferred) * 100.0) / (double)m_uTotalFileSize;
					int iL1 = (int)((((double)iW) * dPerc1) / 100.0);
					double dPerc2 = (double)(((double)uAckedBytes) * 100.0) / (double)m_uTotalFileSize;
					int iL2 = (int)((((double)iW) * dPerc2) / 100.0);
					int iW2 = iL1 - iL2;
					if(iW2 > 0)
						p->fillRect(rect.left() + 5 + iL2, rect.top() + 5, iW2, 10, bIsTerminated ? QColor(150, 130, 110) : QColor(220, 170, 100));
					p->fillRect(rect.left() + 5, rect.top() + 5, iL2, 10, bIsTerminated ? QColor(140, 110, 110) : QColor(200, 100, 100));

					txt = QString(__tr2qs_ctx("%1 of %2 (%3%)", "dcc")).arg(KviQString::makeSizeReadable(uAckedBytes)).arg(KviQString::makeSizeReadable(m_uTotalFileSize)).arg(dPerc2, 0, 'f', 2);
				}
				else
				{
					// we are receiving a file or not sending acks
					double dPerc = (double)(((double)uTransferred) * 100.0) / (double)m_uTotalFileSize;
					int iL = (int)((((double)iW) * dPerc) / 100.0);
					p->fillRect(rect.left() + 5, rect.top() + 5, iL, 10, bIsTerminated ? QColor(140, 110, 110) : QColor(200, 100, 100));

					txt = QString(__tr2qs_ctx("%1 of %2 (%3%)", "dcc")).arg(KviQString::makeSizeReadable(uTransferred)).arg(KviQString::makeSizeReadable(m_uTotalFileSize)).arg(dPerc, 0, 'f', 2);
				}
			}
			else
			{
				txt = QString(__tr2qs_ctx("%1", "dcc")).arg(KviQString::makeSizeReadable(uTransferred));
			}

			p->setPen(Qt::black);

			p->drawText(rect.left() + 4, rect.top() + 19, width - 8, height - 8, Qt::AlignTop | Qt::AlignLeft, txt);

			int iLeftHalf = (iW - 2) / 2;
			int iRightHalf = iW - (iLeftHalf + 1);
			int iLineSpacing = fm.lineSpacing() + 2;

			if(!bIsTerminated)
			{
				txt = __tr2qs_ctx("Speed:", "dcc");
				txt += " ";

				QString tmpisp;
				KviNetUtils::formatNetworkBandwidthString(tmpisp, uInstantSpeed);
				txt += tmpisp;
				txt += " [";
			}
			else
			{
				txt = "";
			}

			txt += __tr2qs_ctx("Average:", "dcc");
			txt += " ";
			QString tmpspd;
			KviNetUtils::formatNetworkBandwidthString(tmpspd, uAvgBandwidth);
			txt += tmpspd;

			if(!bIsTerminated)
			{
				txt += "]";
			}

			int iDaH = height - (iLineSpacing + 4);

			p->setPen(QColor(180, 180, 200));
			p->drawRect(rect.left() + 4, rect.top() + iDaH, iLeftHalf, iLineSpacing);
			p->fillRect(rect.left() + 5, rect.top() + iDaH + 1, iLeftHalf - 2, iLineSpacing - 2, bIsTerminated ? QColor(210, 210, 210) : QColor(190, 190, 240));
			p->setPen(bIsTerminated ? Qt::darkGray : Qt::black);
			p->drawText(rect.left() + 6, rect.top() + iDaH, iLeftHalf - 4, iLineSpacing, Qt::AlignLeft | Qt::AlignVCenter, txt);

			if(bIsTerminated)
			{
				if((m_tTransferStartTime != 0) && (m_tTransferEndTime != 0))
				{
					QString tot = KviTimeUtils::formatTimeInterval(kvi_timeSpan(m_tTransferEndTime, m_tTransferStartTime), KviTimeUtils::NoLeadingEmptyIntervals | KviTimeUtils::NoLeadingZeroes);
					txt = "Total: ";
					txt += tot;
				}
				else
				{
					txt = "";
				}
			}
			else
			{
				if(iEta >= 0)
				{
					QString eta = KviTimeUtils::formatTimeInterval(iEta, KviTimeUtils::NoLeadingEmptyIntervals | KviTimeUtils::NoLeadingZeroes);
					txt = "ETA: ";
					txt += eta;
				}
				else
				{
					txt = "ETA: ?";
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

int DccFileTransfer::displayHeight(int iLineSpacing)
{
	int iH = (iLineSpacing * 3) + 10;
	return iH >= 70 ? iH : 70;
}

QString DccFileTransfer::tipText()
{
	QString s;

	s = QString(R"(<table><tr><td bgcolor="#000000"><font color="#FFFFFF"><b>DCC %1 (ID %2)</b></font></td></tr>)").arg(m_szDccType.ptr()).arg(id());

	s += R"(<tr><td bgcolor="#404040"><font color="#FFFFFF">)";
	s += __tr2qs_ctx("Transfer Log", "dcc");
	s += "</font></td></tr>";
	s += "<tr><td bgcolor=\"#C0C0C0\">";
	s += m_szTransferLog;
	s += "</td></tr>";
	s += "<table>";

	return s;
}

void DccFileTransfer::init()
{
	if(g_pDccFileTransfers)
		return;
	g_pDccFileTransfers = new KviPointerList<DccFileTransfer>;
	g_pDccFileTransfers->setAutoDelete(false);

	QPixmap * pix = g_pIconManager->getImage("kvi_dccfiletransfericons.png", false);
	if(pix)
		g_pDccFileTransferIcon = new QPixmap(*pix);
	else
		g_pDccFileTransferIcon = nullptr;
}

void DccFileTransfer::done()
{
	if(!g_pDccFileTransfers)
		return;
	while(DccFileTransfer * t = g_pDccFileTransfers->first())
		delete t;
	delete g_pDccFileTransfers;
	g_pDccFileTransfers = nullptr;
	if(g_pDccFileTransferIcon)
		delete g_pDccFileTransferIcon;
	g_pDccFileTransferIcon = nullptr;
}

unsigned int DccFileTransfer::transferCount()
{
	if(!g_pDccFileTransfers)
		return 0;
	return g_pDccFileTransfers->count();
}

DccFileTransfer * DccFileTransfer::nonFailedTransferWithLocalFileName(const QString & szLocalFileName)
{
	if(!g_pDccFileTransfers)
		return nullptr;
	for(DccFileTransfer * t = g_pDccFileTransfers->first(); t; t = g_pDccFileTransfers->next())
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		// on windows the file names are case insensitive
		if(t->localFileName().toLower() == szLocalFileName.toLower())
#else
		if(t->localFileName() == szLocalFileName)
#endif
		{
			if(t->m_eGeneralStatus != Failure)
				return t;
		}
	}
	return nullptr;
}

unsigned int DccFileTransfer::runningTransfersCount()
{
	if(!g_pDccFileTransfers)
		return 0;
	unsigned int cnt = 0;
	for(DccFileTransfer * t = g_pDccFileTransfers->first(); t; t = g_pDccFileTransfers->next())
	{
		if(t->active())
			cnt++;
	}
	return cnt;
}

bool DccFileTransfer::handleResumeAccepted(const char * filename, const char * port, const char * szZeroPortTag)
{
	if(!g_pDccFileTransfers)
		return false;

	for(DccFileTransfer * t = g_pDccFileTransfers->first(); t; t = g_pDccFileTransfers->next())
	{
		if(t->resumeAccepted(filename, port, szZeroPortTag))
			return true;
	}

	return false;
}

bool DccFileTransfer::handleResumeRequest(const char * filename, const char * port, quint64 filePos)
{
	if(!g_pDccFileTransfers)
		return false;

	for(DccFileTransfer * t = g_pDccFileTransfers->first(); t; t = g_pDccFileTransfers->next())
	{
		if(t->doResume(filename, port, filePos))
			return true;
	}

	return false;
}

void DccFileTransfer::outputAndLog(const QString & s)
{
	KviWindow * out = transferWindow();
	addToTransferLog(s);
	if(out)
		out->output(KVI_OUT_DCCMSG, "[%Q]: %Q", &m_szTransferIdString, &s);
}

void DccFileTransfer::outputAndLog(int msgtype, const QString & s)
{
	KviWindow * out = transferWindow();
	addToTransferLog(s);
	if(out)
		out->output(msgtype, "[%Q]: %Q", &m_szTransferIdString, &s);
}

void DccFileTransfer::addToTransferLog(const QString & s)
{
	QDateTime dt = QDateTime::currentDateTime();
	QString ts;
	ts.sprintf("[%4d.%2d.%2d %2d:%2d:%2d] ", dt.date().year(), dt.date().month(), dt.date().day(), dt.time().hour(), dt.time().minute(), dt.time().second());
	m_szTransferLog += ts + s;
	m_szTransferLog += "<br>";
}

void DccFileTransfer::connectionInProgress()
{
	if(m_pDescriptor->bActive)
	{
		// ACTIVE CONNECTION
		//		if((kvi_strEqualCS(m_szDccType.ptr(), "RECV")) || (kvi_strEqualCS(m_szDccType.ptr(),"TRECV")))
		//		{
		//			// FIXME: that's not true!... we're NOT connected here
		//			if(TRIGGER_EVENT_5PARAM_RETVALUE(KviEvent_OnDCCGetConnected,this,m_pDescriptor->szPort.ptr(),m_pDescriptor->szFileName.ptr(),m_pDescriptor->szNick.ptr(),m_pDescriptor->szUser.ptr(),m_pDescriptor->szHost.ptr()));
		//		} else {
		//			if(TRIGGER_EVENT_5PARAM_RETVALUE(KviEvent_OnDCCSendConnected,this,m_pDescriptor->szPort.ptr(),m_pDescriptor->szFileName.ptr(),m_pDescriptor->szNick.ptr(),m_pDescriptor->szUser.ptr(),m_pDescriptor->szHost.ptr()));
		//		}
		//
		m_szStatusString = __tr2qs_ctx("Contacting host %1 on port %2", "dcc").arg(m_pDescriptor->szIp, m_pDescriptor->szPort);
		outputAndLog(m_szStatusString);
		displayUpdate();
		return;
	}

	// PASSIVE CONNECTION
	m_szStatusString = __tr2qs_ctx("Listening on interface %1 port %2", "dcc").arg(m_pMarshal->localIp(), m_pMarshal->localPort());
	outputAndLog(m_szStatusString);

	if(m_pDescriptor->bSendRequest)
	{
		QString ip;
		if(!m_pDescriptor->szFakeIp.isEmpty())
		{
			ip = m_pDescriptor->szFakeIp;
		}
		else
		{
			ip = m_pDescriptor->szListenIp;

			if(KVI_OPTION_BOOL(KviOption_boolDccGuessIpFromServerWhenLocalIsUnroutable))
			{
				if(!KviNetUtils::isRoutableIpString(ip))
				{
					// try to get the IP that the IRC server can see
					if(m_pDescriptor->console())
					{
						QString tmp = m_pDescriptor->console()->connection() ? m_pDescriptor->console()->connection()->userInfo()->hostIp() : "";
						if(!tmp.isEmpty())
						{
							ip = tmp;
							outputAndLog(__tr2qs_ctx("The local IP address is private, determining from IRC server: %1", "dcc").arg(ip));
						}
						else
						{
							outputAndLog(__tr2qs_ctx("The local IP address is private, but unable to determine it from the IRC server", "dcc"));
						}
					}
					else
					{
						outputAndLog(__tr2qs_ctx("The local IP address is private, but have no IRC server to determine it from", "dcc"));
					}
				}
			}
		}

		KviCString port = !m_pDescriptor->szFakePort.isEmpty() ? m_pDescriptor->szFakePort : m_pMarshal->localPort();
		//#warning "OPTION FOR SENDING 127.0.0.1 and so on (not an unsigned nuumber)"
		struct in_addr a;
		if(KviNetUtils::stringIpToBinaryIp(ip, &a))
			ip.setNum(htonl(a.s_addr));

		QString tmp = m_pDescriptor->szFileName;
		// just to be sure
		KviQString::cutToLast(tmp, '/');
		KviQString::cutToLast(tmp, '\\');

		QString fName;

		// BUG-TO-BUG mIrc compatibility
		if(KVI_OPTION_BOOL(KviOption_boolDCCFileTransferReplaceOutgoingSpacesWithUnderscores))
			tmp.replace(" ", "_");

		KviIrcServerParser::encodeCtcpParameter(tmp.toUtf8().data(), fName);
		// Zero port requests want DCC SEND as back-request
		KviCString szReq;

		if(m_pDescriptor->isZeroPortRequest())
		{
			szReq = "SEND";
			if(m_pDescriptor->bIsTdcc)
				szReq.prepend("T");
#ifdef COMPILE_SSL_SUPPORT
			if(m_pDescriptor->bIsSSL)
				szReq.prepend("S");
#endif
			m_pDescriptor->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC %s %s %s %s %s %s%c",
			    m_pDescriptor->console()->connection()->encodeText(m_pDescriptor->szNick).data(),
			    0x01,
			    m_pDescriptor->console()->connection()->encodeText(szReq.ptr()).data(),
			    m_pDescriptor->console()->connection()->encodeText(fName).data(),
			    ip.toUtf8().data(), port.ptr(),
			    m_pDescriptor->szFileSize.toUtf8().data(), m_pDescriptor->zeroPortRequestTag(), 0x01);
		}
		else
		{
			szReq = m_szDccType;
			m_pDescriptor->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC %s %s %s %s %Q%c",
			    m_pDescriptor->console()->connection()->encodeText(m_pDescriptor->szNick).data(),
			    0x01,
			    m_pDescriptor->console()->connection()->encodeText(szReq.ptr()).data(),
			    m_pDescriptor->console()->connection()->encodeText(fName).data(),
			    ip.toUtf8().data(), port.ptr(),
			    &(m_pDescriptor->szLocalFileSize), 0x01);
		}
		outputAndLog(__tr2qs_ctx("Sent DCC %1 request to %2, waiting for remote client to connect...", "dcc").arg(szReq.ptr(), m_pDescriptor->szNick));
	}
	else
	{
		outputAndLog(__tr2qs_ctx("DCC %1 request not sent, awaiting manual connection", "dcc").arg(m_szDccType.ptr()));
	}

	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCFileTransferConnectionInProgress, eventWindow(), m_pDescriptor->idString());

	displayUpdate();
}

void DccFileTransfer::startingSSLHandshake()
{
#ifdef COMPILE_SSL_SUPPORT
	outputAndLog(KVI_OUT_SSL, __tr2qs_ctx("Low-level transport connection established", "dcc"));
	outputAndLog(KVI_OUT_SSL, __tr2qs_ctx("Starting Secure Socket Layer handshake", "dcc"));
#endif
}

void DccFileTransfer::sslError(const char * msg)
{
#ifdef COMPILE_SSL_SUPPORT
	outputAndLog(KVI_OUT_DCCERROR, __tr2qs_ctx("[SSL ERROR]: %1", "dcc").arg(msg));
#endif
}

bool DccFileTransfer::event(QEvent * e)
{
	if(e->type() == KVI_THREAD_EVENT)
	{
		switch(((KviThreadEvent *)e)->id())
		{
			case KVI_DCC_THREAD_EVENT_ERROR:
			{
				KviError::Code * pError = ((KviThreadDataEvent<KviError::Code> *)e)->getData();
				QString szErrorString = KviError::getDescription(*pError);
				delete pError;
				if(m_pDescriptor->bRecvFile)
					g_pApp->fileDownloadTerminated(false, m_pDescriptor->szFileName.toUtf8().data(), m_pDescriptor->szLocalFileName.toUtf8().data(), m_pDescriptor->szNick.toUtf8().data(), szErrorString.toUtf8().data());

				m_szStatusString = __tr2qs_ctx("Transfer failed: ", "dcc");
				m_szStatusString += szErrorString;
				m_eGeneralStatus = Failure;
				m_tTransferEndTime = kvi_unixTime();

				KVS_TRIGGER_EVENT_3(KviEvent_OnDCCFileTransferFailed,
				    eventWindow(),
				    szErrorString,
				    (kvs_int_t)(m_pSlaveRecvThread ? m_pSlaveRecvThread->receivedBytes() : m_pSlaveSendThread->sentBytes()),
				    m_pDescriptor->idString());

				outputAndLog(KVI_OUT_DCCERROR, m_szStatusString);
				displayUpdate();
				return true;
			}
			break;
			case KVI_DCC_THREAD_EVENT_SUCCESS:
			{
				// FIXME: for >= 3.2.0 change this text to
				// File Upload/Download terminated, or something like this
				if(KVI_OPTION_BOOL(KviOption_boolNotifyDccSendSuccessInConsole))
				{
					KviConsoleWindow * c;
					if(!g_pApp->windowExists(m_pDescriptor->console()))
						c = g_pApp->activeConsole();
					else
						c = m_pDescriptor->console();
					c->output(KVI_OUT_DCCMSG, __tr2qs_ctx("DCC %s transfer with %Q@%Q:%Q completed: \r![!dbl]%Q\r%Q\r", "dcc"),
					    m_pDescriptor->bIsTdcc ? (m_pDescriptor->bRecvFile ? "TRECV" : "TSEND") : (m_pDescriptor->bRecvFile ? "RECV" : "SEND"),
					    &(m_pDescriptor->szNick), &(m_pDescriptor->szIp), &(m_pDescriptor->szPort),
					    &(KVI_OPTION_STRING(KviOption_stringUrlFileCommand)),
					    &(m_pDescriptor->szLocalFileName));
				}

				if(m_pDescriptor->bRecvFile)
					g_pApp->fileDownloadTerminated(true, m_pDescriptor->szFileName.toUtf8().data(), m_pDescriptor->szLocalFileName.toUtf8().data(), m_pDescriptor->szNick.toUtf8().data());
				m_szStatusString = __tr2qs_ctx("Transfer completed", "dcc");
				outputAndLog(m_szStatusString);
				m_eGeneralStatus = Success;
				m_tTransferEndTime = kvi_unixTime();
				if(m_pResumeTimer)
					delete m_pResumeTimer;

				KVS_TRIGGER_EVENT_2(KviEvent_OnDCCFileTransferSuccess,
				    eventWindow(),
				    (kvs_int_t)(m_pSlaveRecvThread ? m_pSlaveRecvThread->receivedBytes() : m_pSlaveSendThread->sentBytes()),
				    m_pDescriptor->idString());

				displayUpdate();

				if(KVI_OPTION_BOOL(KviOption_boolAutoCloseDccSendOnSuccess))
					die();
				return true;
			}
			break;
			case KVI_DCC_THREAD_EVENT_MESSAGE:
			{
				KviCString * str = ((KviThreadDataEvent<KviCString> *)e)->getData();
				outputAndLog(QString(__tr_no_xgettext_ctx(str->ptr(), "dcc")));
				delete str;
				return true;
			}
			break;
			default:
				qDebug("Invalid event type %d received", ((KviThreadEvent *)e)->id());
				break;
		}
	}
	return KviFileTransfer::event(e);
}

void DccFileTransfer::handleMarshalError(KviError::Code eError)
{
	QString szErr = KviError::getDescription(eError);
	m_eGeneralStatus = Failure;
	m_szStatusString = __tr2qs_ctx("Transfer failed: ", "dcc");
	m_szStatusString += szErr;
	outputAndLog(m_szStatusString);
	KVS_TRIGGER_EVENT_3(KviEvent_OnDCCFileTransferFailed, eventWindow(), szErr, (kvs_int_t)0, m_pDescriptor->idString());
	displayUpdate();
}

void DccFileTransfer::connected()
{
	outputAndLog(__tr2qs_ctx("Connected to %1:%2", "dcc").arg(m_pMarshal->remoteIp(), m_pMarshal->remotePort()));
	outputAndLog(__tr2qs_ctx("Local end is %1:%2", "dcc").arg(m_pMarshal->localIp(), m_pMarshal->localPort()));

	m_tTransferStartTime = kvi_unixTime();

	if(!(m_pDescriptor->bActive))
	{
		m_pDescriptor->szIp = m_pMarshal->remoteIp();
		m_pDescriptor->szPort = m_pMarshal->remotePort();
		m_pDescriptor->szHost = m_pMarshal->remoteIp();
	}

	if(m_pDescriptor->bRecvFile)
	{
		KviDccRecvThreadOptions * o = new KviDccRecvThreadOptions;
		o->szFileName = m_pDescriptor->szLocalFileName.toUtf8().data();
		bool bOk;
		o->uTotalFileSize = m_pDescriptor->szFileSize.toULongLong(&bOk);
		if(!bOk)
			o->uTotalFileSize = 0;
		o->bResume = m_pDescriptor->bResume;
		o->iIdleStepLengthInMSec = KVI_OPTION_BOOL(KviOption_boolDccSendForceIdleStep) ? KVI_OPTION_UINT(KviOption_uintDccSendIdleStepInMSec) : 0;
		o->bIsTdcc = m_pDescriptor->bIsTdcc;
		o->bSendZeroAck = KVI_OPTION_BOOL(KviOption_boolSendZeroAckInDccRecv);
		o->bSend64BitAck = KVI_OPTION_BOOL(KviOption_boolSend64BitAckInDccRecv);
		o->bNoAcks = m_pDescriptor->bNoAcks;
		o->uMaxBandwidth = m_uMaxBandwidth;
		m_pSlaveRecvThread = new DccRecvThread(this, m_pMarshal->releaseSocket(), o);

#ifdef COMPILE_SSL_SUPPORT
		KviSSL * s = m_pMarshal->releaseSSL();
		if(s)
		{
			m_pSlaveRecvThread->setSSL(s);
		}
#endif
		m_pSlaveRecvThread->start();
	}
	else
	{
		KviDccSendThreadOptions * o = new KviDccSendThreadOptions;
		o->szFileName = m_pDescriptor->szLocalFileName.toUtf8().data();
		o->bFastSend = KVI_OPTION_BOOL(KviOption_boolUseFastDccSend);
		o->iIdleStepLengthInMSec = KVI_OPTION_BOOL(KviOption_boolDccSendForceIdleStep) ? KVI_OPTION_UINT(KviOption_uintDccSendIdleStepInMSec) : 0;
		bool bOk;
		o->bIsTdcc = m_pDescriptor->bIsTdcc;
		o->uStartPosition = m_pDescriptor->szFileSize.toULongLong(&bOk);
		if(!bOk)
			o->uStartPosition = 0;
		o->iPacketSize = KVI_OPTION_UINT(KviOption_uintDccSendPacketSize);
		if(o->iPacketSize < 32)
			o->iPacketSize = 32;
		o->uMaxBandwidth = m_uMaxBandwidth;
		o->bNoAcks = m_pDescriptor->bNoAcks;
		m_pSlaveSendThread = new DccSendThread(this, m_pMarshal->releaseSocket(), o);
#ifdef COMPILE_SSL_SUPPORT
		KviSSL * s = m_pMarshal->releaseSSL();
		if(s)
		{
			m_pSlaveSendThread->setSSL(s);
		}
#endif
		m_pSlaveSendThread->start();
	}

	m_eGeneralStatus = Transferring;
	m_szStatusString = __tr2qs_ctx("Transferring data", "dcc");

	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCFileTransferBegin, eventWindow(), m_pDescriptor->idString());

	outputAndLog(m_szStatusString);
	displayUpdate();
}

bool DccFileTransfer::resumeAccepted(const char * filename, const char * port, const char * szZeroPortTag)
{
	if(!(kvi_strEqualCI(filename, m_pDescriptor->szFileName.toUtf8().data()) || KVI_OPTION_BOOL(KviOption_boolAcceptBrokenFileNameDccResumeRequests)))
		return false;

	if(!(kvi_strEqualCI(port, m_pDescriptor->szPort.toUtf8().data()) && (!m_pSlaveRecvThread) && m_pDescriptor->bResume && m_pDescriptor->bRecvFile && m_pResumeTimer))
		return false;

	if(kvi_strEqualCI(port, "0"))
	{
		if(!kvi_strEqualCI(szZeroPortTag, m_pDescriptor->zeroPortRequestTag()))
			return false;
	}

	delete m_pResumeTimer;
	m_pResumeTimer = nullptr;

	outputAndLog(__tr2qs_ctx("RESUME accepted, transfer will begin at position %1", "dcc").arg(m_pDescriptor->szLocalFileSize));

	listenOrConnect();

	return true;
}

bool DccFileTransfer::doResume(const char * filename, const char * port, quint64 filePos)
{
	if(m_pSlaveRecvThread)
		return false; // we're already receiving stuff...
	if(m_pSlaveSendThread)
		return false; // we're already sending stuff...

	if(m_pDescriptor->bRecvFile)
		return false; // we're receiving... can't resume anything

	bool bFileNameMatches = KviQString::equalCI(filename, m_pDescriptor->szFileName);
	bool bPortMatches = KviQString::equalCI(port, m_pMarshal->dccPort());

	if(!bPortMatches)
	{
		// port doesn't match
		if(!bFileNameMatches)
			return false; // neither filename nor port match

		if(!KVI_OPTION_BOOL(KviOption_boolAcceptMismatchedPortDccResumeRequests))
			return false;

		// hmm.. try to accept a mismatched port request

		if(_OUTPUT_VERBOSE)
			outputAndLog(KVI_OUT_DCCMSG, __tr2qs_ctx("Processing RESUME request with mismatched port (%1)", "dcc").arg(port));
	}

	// port matches

	if(!bFileNameMatches)
	{
		if(!KVI_OPTION_BOOL(KviOption_boolAcceptBrokenFileNameDccResumeRequests))
		{
			if(_OUTPUT_VERBOSE)
				outputAndLog(
				    KVI_OUT_DCCMSG,
				    __tr2qs_ctx("Invalid RESUME request: invalid file name (got '%1' but should be '%2')", "dcc")
				        .arg(filename)
				        .arg(m_pDescriptor->szFileName));
			return false; // bad file name
		}

		if(_OUTPUT_VERBOSE)
			outputAndLog(KVI_OUT_DCCMSG, __tr2qs_ctx("Processing RESUME request with broken filename (%1)", "dcc").arg(filename));
	}

	bool bOk;
	quint64 iLocalFileSize = m_pDescriptor->szLocalFileSize.toULongLong(&bOk);

	if(!bOk)
	{
		// ops...internal error
		outputAndLog(KVI_OUT_DCCERROR, __tr2qs_ctx("Internal error in RESUME request", "dcc"));
		return false;
	}

	if(iLocalFileSize <= filePos)
	{
		outputAndLog(KVI_OUT_DCCERROR, __tr2qs_ctx("Invalid RESUME request: position %1 is larger than file size", "dcc").arg(filePos));
		return false;
	}

	outputAndLog(KVI_OUT_DCCERROR, __tr2qs_ctx("Accepting RESUME request, transfer will begin at position %1", "dcc").arg(filePos));

	m_pDescriptor->szFileSize.setNum(filePos);

	KviCString szBuffy;
	KviIrcServerParser::encodeCtcpParameter(filename, szBuffy);

	m_pDescriptor->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC ACCEPT %s %s %s%c",
	    m_pDescriptor->console()->connection()->encodeText(m_pDescriptor->szNick).data(),
	    0x01,
	    m_pDescriptor->console()->connection()->encodeText(szBuffy.ptr()).data(),
	    port,
	    m_pDescriptor->console()->connection()->encodeText(QString::number(filePos)).data(),
	    0x01);

	return true;
}

DccThread * DccFileTransfer::getSlaveThread()
{
	if(m_pDescriptor->bRecvFile)
	{
		return m_pSlaveRecvThread;
	}
	else
	{
		return m_pSlaveSendThread;
	}
}

DccFileTransferBandwidthDialog::DccFileTransferBandwidthDialog(QWidget * pParent, DccFileTransfer * t)
    : QDialog(pParent)
{
	QGridLayout * g = new QGridLayout(this);

	m_pTransfer = t;
	int iVal = m_pTransfer->bandwidthLimit();

	QString szText = __tr2qs_ctx("Configure Bandwidth for DCC Transfer %1", "dcc").arg(t->id());
	setWindowTitle(szText);

	szText = t->isFileUpload() ? __tr2qs_ctx("Limit upload bandwidth to:", "dcc") : __tr2qs_ctx("Limit download bandwidth to:", "dcc");

	m_pEnableLimitCheck = new QCheckBox(szText, this);
	g->addWidget(m_pEnableLimitCheck, 0, 0);

	m_pEnableLimitCheck->setChecked((iVal >= 0) && (iVal < MAX_DCC_BANDWIDTH_LIMIT));

	m_pLimitBox = new QSpinBox(this);
	m_pLimitBox->setMinimum(0);
	m_pLimitBox->setMaximum(MAX_DCC_BANDWIDTH_LIMIT - 1);
	m_pLimitBox->setSingleStep(1);

	m_pLimitBox->setEnabled((iVal >= 0) && (iVal < MAX_DCC_BANDWIDTH_LIMIT));
	connect(m_pEnableLimitCheck, SIGNAL(toggled(bool)), m_pLimitBox, SLOT(setEnabled(bool)));
	g->addWidget(m_pLimitBox, 0, 1, 1, 2);

	szText = " ";
	szText += __tr2qs_ctx("bytes/sec", "dcc");
	m_pLimitBox->setSuffix(szText);
	m_pLimitBox->setValue(iVal < MAX_DCC_BANDWIDTH_LIMIT ? iVal : 0);

	QPushButton * pb = new QPushButton(__tr2qs_ctx("OK", "dcc"), this);
	connect(pb, SIGNAL(clicked()), this, SLOT(okClicked()));
	pb->setMinimumWidth(80);
	g->addWidget(pb, 2, 2);

	pb = new QPushButton(__tr2qs_ctx("Cancel", "dcc"), this);
	connect(pb, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	pb->setMinimumWidth(80);
	g->addWidget(pb, 2, 1);

	g->setColumnStretch(0, 1);
	g->setRowStretch(1, 1);
}

DccFileTransferBandwidthDialog::~DccFileTransferBandwidthDialog()
    = default;

void DccFileTransferBandwidthDialog::okClicked()
{
	int iVal = MAX_DCC_BANDWIDTH_LIMIT;
	if(m_pEnableLimitCheck->isChecked())
	{
		iVal = m_pLimitBox->value();
		if(iVal < 0)
			iVal = MAX_DCC_BANDWIDTH_LIMIT;
		if(iVal > MAX_DCC_BANDWIDTH_LIMIT)
			iVal = MAX_DCC_BANDWIDTH_LIMIT;
	}
	m_pTransfer->setBandwidthLimit(iVal);
	delete this;
}

void DccFileTransferBandwidthDialog::cancelClicked()
{
	delete this;
}

void DccFileTransferBandwidthDialog::closeEvent(QCloseEvent * e)
{
	e->ignore();
	delete this;
}
