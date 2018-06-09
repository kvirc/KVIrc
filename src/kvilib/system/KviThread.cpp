//=============================================================================
//
//   File : KviThread.cpp
//   Creation date : Tue Jul 6 1999 16:04:45 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "KviThread.h"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <io.h> // for _pipe()
#else
#include <unistd.h> //for pipe() and other tricks
#include <csignal>  // on Windows it is useless
#include <fcntl.h>
#endif

#include <cerrno>

#include "kvi_settings.h"
#include "KviError.h"

#include <QSocketNotifier>
#include <QApplication>

static void kvi_threadIgnoreSigalarm()
{
// On Windows this stuff is useless anyway
#ifdef COMPILE_IGNORE_SIGALARM
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	// Funky hack for some Solaris machines (maybe others ?)
	// For an obscure (at least to me) reason
	// when using threads,some part of the system
	// starts kidding us by sending a SIGALRM in apparently
	// "random" circumstances. (Xlib ?) (XServer ?)
	// The default action for SIGALRM is to exit the application.
	// Could not guess more about this stuff...
	// Here goes a "blind" hack for that.

	// Update: now we have an explanation too
	//
	//    From: "Andre Stechert" (astechert at email dot com)
	//    To: pragma at kvirc dot net
	//    Subject: sigalarm on solaris ...
	//    Date:  26/7/2005 09:36

	//    Hi,
	//    I noticed in your readme that you were having problems with sigalarm
	//    in your solaris port and you weren't sure why.  I quickly scanned your
	//    source code and noticed that you use usleep and threads.  That's the problem,
	//    if you haven't already figured it out. On Solaris, usleep is implemented with
	//    SIGALARM. So is threading. So if you the active thread changes while
	//    a usleep is in progress, bang, the process is dead.
	//
	// There is no real feedback on this at the moment: if somebody
	// experiences the problems please drop me a mail at pragma at kvirc dot net
	// and we'll try to look for a better solution.
	// If the explanation is correct then KVIrc could even lock up on those machines
	// (never returning from an usleep() call ?)...

	struct sigaction ignr_act;
	ignr_act.sa_handler = SIG_IGN;
	sigemptyset(&ignr_act.sa_mask);

#ifdef SA_NOMASK
	ignr_act.sa_flags = SA_NOMASK;
#else
	ignr_act.sa_flags = 0;
#endif

#ifdef SA_RESTART
	ignr_act.sa_flags |= SA_RESTART;
#endif

	if(sigaction(SIGALRM, &ignr_act, 0) == -1)
		qDebug("Failed to set SIG_IGN for SIGALRM.");
#endif
#endif
}

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)

static void kvi_threadSigpipeHandler(int)
{
	qDebug("Thread ????: Caught SIGPIPE: ignoring.");
}

#endif

static void kvi_threadCatchSigpipe()
{
// On windows this stuff is useless
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	struct sigaction act;
	act.sa_handler = &kvi_threadSigpipeHandler;
	sigemptyset(&(act.sa_mask));
	sigaddset(&(act.sa_mask), SIGPIPE);
// CC: take care of SunOS which automatically restarts interrupted system
// calls (and thus does not have SA_RESTART)
#ifdef SA_NOMASK
	act.sa_flags = SA_NOMASK;
#else
	act.sa_flags = 0;
#endif

#ifdef SA_RESTART
	act.sa_flags |= SA_RESTART;
#endif

	if(sigaction(SIGPIPE, &act, nullptr) == -1)
		qDebug("Failed to set the handler for SIGPIPE.");
#endif
}

static void kvi_threadInitialize()
{
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	kvi_threadIgnoreSigalarm();
	kvi_threadCatchSigpipe();
#endif
}

#define KVI_THREAD_PIPE_SIDE_MASTER 0
#define KVI_THREAD_PIPE_SIDE_SLAVE 1

// the maximum length of the slave->master queue
// over this length, the slave is forced to usleep()
#define KVI_THREAD_MAX_EVENT_QUEUE_LENGTH 50

static KviThreadManager * g_pThreadManager = nullptr;

void KviThreadManager::globalInit()
{
	kvi_threadInitialize(); // we want this to apply to the main thread too
	g_pThreadManager = new KviThreadManager();
}

void KviThreadManager::globalDestroy()
{
	delete g_pThreadManager;
	g_pThreadManager = nullptr;
}

KviThreadManager::KviThreadManager()
    : QObject()
{
	if(g_pThreadManager)
		qDebug("Hey... what are ya doing?");

	m_pMutex = new KviMutex();
	m_pThreadList = new KviPointerList<KviThread>;
	m_pThreadList->setAutoDelete(false);

	m_iWaitingThreads = 0;

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)

	m_iTriggerCount = 0;

	m_pEventQueue = new KviPointerList<KviThreadPendingEvent>;
	m_pEventQueue->setAutoDelete(true);

	if(pipe(m_fd) != 0)
	{
		qDebug("Oops! Thread manager pipe creation failed (%s)", KviError::getDescription(KviError::translateSystemError(errno)).toUtf8().data());
	}

	if(fcntl(m_fd[KVI_THREAD_PIPE_SIDE_SLAVE], F_SETFL, O_NONBLOCK) == -1)
	{
		qDebug("Oops! Thread manager slave pipe initialisation failed (%s)", KviError::getDescription(KviError::translateSystemError(errno)).toUtf8().data());
	}

	if(fcntl(m_fd[KVI_THREAD_PIPE_SIDE_MASTER], F_SETFL, O_NONBLOCK) == -1)
	{
		qDebug("Oops! Thread manager master pipe initialisation failed (%s)", KviError::getDescription(KviError::translateSystemError(errno)).toUtf8().data());
	}

	m_pSn = new QSocketNotifier(m_fd[KVI_THREAD_PIPE_SIDE_MASTER], QSocketNotifier::Read);
	connect(m_pSn, SIGNAL(activated(int)), this, SLOT(eventsPending(int)));
	m_pSn->setEnabled(true);
#endif
}

KviThreadManager::~KviThreadManager()
{
	m_pMutex->lock();
	// Terminate all the slaves
	while(KviThread * t = m_pThreadList->first())
	{
		m_pMutex->unlock();
		delete t;
		m_pMutex->lock();
	}

// there are no more child threads
// thus no more slave events are sent.
// Disable the socket notifier, we no longer need it
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	m_pSn->setEnabled(false);
	delete m_pSn;
	m_pSn = nullptr;
#endif

	// we're no longer in this world
	g_pThreadManager = nullptr;

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	// close the pipes
	close(m_fd[KVI_THREAD_PIPE_SIDE_SLAVE]);
	close(m_fd[KVI_THREAD_PIPE_SIDE_MASTER]);
	// Kill the pending events
	while(KviThreadPendingEvent * ev = m_pEventQueue->first())
	{
		delete ev->e;
		m_pEventQueue->removeFirst();
	}
	delete m_pEventQueue;
	m_pEventQueue = nullptr;
#endif

	m_pMutex->unlock();

	// finish the cleanup
	delete m_pMutex;
	m_pMutex = nullptr;
	delete m_pThreadList;
	m_pThreadList = nullptr;

	// byez :)
}

void KviThreadManager::killPendingEvents(QObject * receiver)
{
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	if(!g_pThreadManager)
		return;
	g_pThreadManager->killPendingEventsByReceiver(receiver);
#endif
}

void KviThreadManager::killPendingEventsByReceiver(QObject * receiver)
{
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	KviPointerList<KviThreadPendingEvent> l;
	l.setAutoDelete(false);
	m_pMutex->lock();
	for(KviThreadPendingEvent * ev = m_pEventQueue->first(); ev; ev = m_pEventQueue->next())
	{
		if(ev->o == receiver)
			l.append(ev);
	}
	for(KviThreadPendingEvent * ev = l.first(); ev; ev = l.next())
	{
		delete ev->e;
		m_pEventQueue->removeRef(ev);
	}
	m_pMutex->unlock();
#endif
}

void KviThreadManager::registerSlaveThread(KviThread * t)
{
	m_pMutex->lock();
	m_pThreadList->append(t);
	m_pMutex->unlock();
}

void KviThreadManager::unregisterSlaveThread(KviThread * t)
{
	m_pMutex->lock();
	m_pThreadList->removeRef(t);
	m_pMutex->unlock();
}

void KviThreadManager::postSlaveEvent(QObject * o, QEvent * e)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	QApplication::postEvent(o, e); // we believe this to be thread-safe
#else
	KviThreadPendingEvent * ev = new KviThreadPendingEvent;
	ev->o = o;
	ev->e = e;

	m_pMutex->lock();

	// if the queue gets too long, make this (slave) thread sleep

	// there is a special case where we can't stop the slaves posting events
	// it's when a thread-master-side is waiting for it's thread-slave-side
	// it the thread-master-side runs in the application main thread then
	// the main thread is sleeping and can't process events.
	// Since we can't be really sure that the thread-master-side will be running
	// on the main application thread we also can't artificially process the events.
	// So the solution is to skip this algorithm when at least one
	// thread is in waiting state.
	while((m_pEventQueue->count() > KVI_THREAD_MAX_EVENT_QUEUE_LENGTH) && (m_iWaitingThreads < 1))
	{
		// wait for the master to process the queue

		m_pMutex->unlock();

// WARNING : This will fail if for some reason
// the master thread gets here! It will wait indefinitely for itself
// if(pthread_self() != m_hMasterThread) ... ????

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		::Sleep(1); // 1ms
#else
		// FIXME : use nanosleep() ?
		::usleep(1000); // 1 ms
#endif
		m_pMutex->lock();
	}

	m_pEventQueue->append(ev);
	// Write bulk to the pipe... but only if there is no other wakeup pending
	if(m_iTriggerCount < 1)
	{
		// I don't know if writing to a pipe is reentrant
		// thus, in doubt, the write is interlocked (it's non blocking anyway)
		int written = write(m_fd[KVI_THREAD_PIPE_SIDE_SLAVE], "?", 1);
		if(written < 1)
		{
			// ops.. failed to write down the event..
			// this is quite irritating now...
			qDebug("Oops! Failed to write down the trigger");
			// FIXME: maybe a single shot timer ?
		}
		else
		{
			m_iTriggerCount++;
		}
	} // else no need to trigger : there is a wakeup pending in there

	m_pMutex->unlock();

#endif
}

void KviThreadManager::eventsPending(int fd)
{
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	char buf[10];
	// do we need to check for errors here ?
	int readed = read(fd, buf, 10);

	m_pMutex->lock();
	// welcome to the critical section :)

	// grab the first event in the queue
	while(KviThreadPendingEvent * ev = m_pEventQueue->first())
	{
		// allow the other threads to post events:
		// unlock the event queue
		m_pMutex->unlock();
		// let the app process the event
		// DANGER !
		QApplication::postEvent(ev->o, ev->e);

		// jump out of the loop if we have been destroyed
		if(!g_pThreadManager)
			return;
		// ufff... we're still alive :)))

		// regrab the event queue
		m_pMutex->lock();
		// remove the event we have just processed
		m_pEventQueue->removeRef(ev);
		// here we're looping locked and havn't decremended the trigger count
	}
	// decrement the trigger count on the line: still atomic
	if(readed >= 0)
	{
		if(readed < m_iTriggerCount)
		{
			m_iTriggerCount -= readed;
		}
		else
		{
			m_iTriggerCount = 0;
		}
	}

	// ok, job done.. can relax now
	m_pMutex->unlock();

#endif
}

void KviThreadManager::threadEnteredWaitState()
{
	m_pMutex->lock();
	m_iWaitingThreads++;
	m_pMutex->unlock();
}

void KviThreadManager::threadLeftWaitState()
{
	m_pMutex->lock();
	m_iWaitingThreads--;
	if(m_iWaitingThreads < 0)
	{
		qDebug("Oops! Do I have a negative number of waiting threads?");
		m_iWaitingThreads = 0;
	}
	m_pMutex->unlock();
}

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
bool KviMutex::locked()
{
	if(!kvi_threadMutexTryLock(&m_mutex))
		return true;
	kvi_threadMutexUnlock(&m_mutex);
	return false;
}
#endif

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
DWORD WINAPI internal_start_thread(LPVOID arg)
{
	// Slave thread...
	((KviThread *)arg)->internalThreadRun_doNotTouchThis();
	return 0;
}
#else
static void * internal_start_thread(void * arg)
{
	// Slave thread...
	((KviThread *)arg)->internalThreadRun_doNotTouchThis();
	return nullptr;
}
#endif

KviThread::KviThread()
{
	g_pThreadManager->registerSlaveThread(this);
	m_pRunningMutex = new KviMutex();
	setRunning(false);
	setStartingUp(false);
}

KviThread::~KviThread()
{
	//qDebug(">> KviThread::~KviThread() : (this = %d)",this);
	wait();
	delete m_pRunningMutex;
	g_pThreadManager->unregisterSlaveThread(this);
	//qDebug("<< KviThread::~KviThread() : (this = %d)",this);
}

void KviThread::setRunning(bool bRunning)
{
	m_pRunningMutex->lock();
	m_bRunning = bRunning;
	m_pRunningMutex->unlock();
}

void KviThread::setStartingUp(bool bStartingUp)
{
	m_pRunningMutex->lock();
	m_bStartingUp = bStartingUp;
	m_pRunningMutex->unlock();
}

bool KviThread::isRunning()
{
	bool bRunning = true;
	m_pRunningMutex->lock();
	bRunning = m_bRunning;
	m_pRunningMutex->unlock();
	return bRunning;
}

bool KviThread::isStartingUp()
{
	bool bIsStartingUp = true;
	m_pRunningMutex->lock();
	bIsStartingUp = m_bStartingUp;
	m_pRunningMutex->unlock();
	return bIsStartingUp;
}

bool KviThread::start()
{
	// We're on the master side thread here!
	if(isStartingUp() || isRunning())
		return false;
	setStartingUp(true);
	return kvi_threadCreate(&m_thread, internal_start_thread, this);
}

void KviThread::wait()
{
	// We're on the master side here...and we're waiting the slave to exit
	//qDebug(">> KviThread::wait() (this=%d)",this);
	while(isStartingUp())
		usleep(500); // sleep 500 microseconds
	                 //	qDebug("!! KviThread::wait() (this=%d)",this);
	g_pThreadManager->threadEnteredWaitState();
	while(isRunning())
	{
		usleep(500); // sleep 500 microseconds
	}
	g_pThreadManager->threadLeftWaitState();
	//qDebug("<< KviThread::wait() (this=%d)",this);
}

void KviThread::exit()
{
	// We're on the slave side thread here! (m_bRunning is true, m_bStartingUp is false)
	setRunning(false);
	kvi_threadExit();
}

void KviThread::internalThreadRun_doNotTouchThis()
{
	// we're on the slave thread here!
	//qDebug(">> KviThread::internalRun (this=%d)",this);
	setRunning(true);
	setStartingUp(false);
	kvi_threadInitialize();
	run();
	setRunning(false);
	//qDebug("<< KviThread::internalRun (this=%d",this);
}

void KviThread::usleep(unsigned long usec)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	int s = usec / 1000;
	if(s < 1)
		s = 1;
	::Sleep(s); // Sleep one millisecond...this is the best that we can do
#else
	// FIXME : use nanosleep() ?
	::usleep(usec);
#endif
}

void KviThread::msleep(unsigned long msec)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	::Sleep(msec);
#else
	// FIXME : use nanosleep() ?
	::usleep(msec * 1000);
#endif
}

void KviThread::sleep(unsigned long sec)
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	::Sleep(sec * 1000);
#else
	::sleep(sec);
#endif
}

void KviThread::postEvent(QObject * o, QEvent * e)
{
	// slave side
	g_pThreadManager->postSlaveEvent(o, e);
}

KviSensitiveThread::KviSensitiveThread()
    : KviThread()
{
	m_pLocalEventQueueMutex = new KviMutex();
	m_pLocalEventQueue = new KviPointerList<KviThreadEvent>;
	m_pLocalEventQueue->setAutoDelete(false);
}

KviSensitiveThread::~KviSensitiveThread()
{
	//qDebug("Entering KviSensitiveThread::~KviSensitiveThread (this=%d)",this);
	terminate();
	//qDebug("KviSensitiveThread::~KviSensitiveThread : terminate called (This=%d)",this);
	m_pLocalEventQueueMutex->lock();
	m_pLocalEventQueue->setAutoDelete(true);
	delete m_pLocalEventQueue;
	m_pLocalEventQueue = nullptr;
	m_pLocalEventQueueMutex->unlock();
	delete m_pLocalEventQueueMutex;
	m_pLocalEventQueueMutex = nullptr;
	//qDebug("Exiting KviSensitiveThread::~KviSensitiveThread (this=%d)",this);
}

void KviSensitiveThread::enqueueEvent(KviThreadEvent * e)
{
	//qDebug(">>> KviSensitiveThread::enqueueEvent() (this=%d)",this);
	m_pLocalEventQueueMutex->lock();
	if(!m_pLocalEventQueue)
	{
		// ops...already terminated (???)...eat the event and return
		delete e;
		m_pLocalEventQueueMutex->unlock();
		return;
	}
	m_pLocalEventQueue->append(e);
	m_pLocalEventQueueMutex->unlock();
	//qDebug("<<< KviSensitiveThread::enqueueEvent() (this=%d)",this);
}

KviThreadEvent * KviSensitiveThread::dequeueEvent()
{
	//qDebug(">>> KviSensitiveThread::dequeueEvent() (this=%d)",this);
	KviThreadEvent * ret;
	m_pLocalEventQueueMutex->lock();
	ret = m_pLocalEventQueue->first();
	if(ret)
		m_pLocalEventQueue->removeFirst();
	m_pLocalEventQueueMutex->unlock();
	//qDebug("<<< KviSensitiveThread::dequeueEvent() (this=%d)",this);
	return ret;
}

void KviSensitiveThread::terminate()
{
	//qDebug("Entering KviSensitiveThread::terminate (this=%d)",this);
	enqueueEvent(new KviThreadEvent(KVI_THREAD_EVENT_TERMINATE));
	//qDebug("KviSensitiveThread::terminate() : event enqueued waiting (this=%d)",this);
	wait();
	//qDebug("Exiting KviSensitiveThread::terminate (this=%d)",this);
}
