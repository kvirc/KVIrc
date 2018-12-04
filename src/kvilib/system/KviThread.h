#ifndef _KVI_THREAD_H_
#define _KVI_THREAD_H_
//=============================================================================
//
//   File : KviThread.h
//   Creation date : Mon May 17 1999 04:26:41 CEST by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviHeapObject.h"
#include "KviPointerList.h"

#include <QObject>
#include <QEvent>

class QSocketNotifier;

//
// Simple thread implementation
// This is enough for KVIrc needs
// HANDLE WITH CARE
//

// Portability stuff

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

#include <winsock2.h>
// Windoze thread abstraction layer
#define kvi_mutex_t HANDLE
inline void kvi_threadMutexInit(kvi_mutex_t * _pMutex_t)
{
	*_pMutex_t = CreateMutex(0, 0, NULL);
}
#define kvi_threadMutexLock(_pMutex_t) WaitForSingleObject(*_pMutex_t, INFINITE)
#define kvi_threadMutexUnlock(_pMutex_t) ReleaseMutex(*_pMutex_t)
#define kvi_threadMutexDestroy(_pMutex_t) CloseHandle(*_pMutex_t)
inline bool kvi_threadMutexTryLock(kvi_mutex_t * _pMutex_t)
{
	return (WaitForSingleObject(*_pMutex_t, 0) == WAIT_OBJECT_0);
}

#define kvi_thread_t HANDLE

inline bool kvi_threadCreate(kvi_thread_t * t, LPTHREAD_START_ROUTINE start_routine, void * arg)
{
	DWORD dwThreadId;
	*t = CreateThread(NULL, 0, start_routine, arg, 0, &dwThreadId);
	return (*t != NULL);
}

#define kvi_threadExit() ExitThread(0)

#else
#ifdef COMPILE_THREADS_USE_POSIX
// Glibc pthread implementation

#include <pthread.h>
#include <errno.h> // for EBUSY

// Mutex stuff
#define kvi_mutex_t pthread_mutex_t
#define kvi_threadMutexInit(_pMutex_t) pthread_mutex_init(_pMutex_t, 0)
#define kvi_threadMutexLock(_pMutex_t) pthread_mutex_lock(_pMutex_t)
#define kvi_threadMutexUnlock(_pMutex_t) pthread_mutex_unlock(_pMutex_t)
#define kvi_threadMutexDestroy(_pMutex_t) pthread_mutex_destroy(_pMutex_t)
inline bool kvi_threadMutexTryLock(kvi_mutex_t * _pMutex_t)
{
	return (pthread_mutex_trylock(_pMutex_t) != EBUSY);
}
// Actually unused
// #define kvi_threadMutexTryLock(_pMutex_t) pthread_mutex_trylock(_pMutex_t)

// Thread stuff
#define kvi_thread_t pthread_t

inline bool kvi_threadCreate(kvi_thread_t * t, void * (*start_routine)(void *), void * arg)
{
	pthread_attr_t a;
	pthread_attr_init(&a);
	pthread_attr_setinheritsched(&a, PTHREAD_INHERIT_SCHED);
	pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);

	int ret = pthread_create(t, &a, start_routine, arg);

	pthread_attr_destroy(&a);
	return (ret == 0);
}

// We don't care about exit codes at all
#define kvi_threadExit() pthread_exit(0)
#else
#ifdef COMPILE_THREADS_USE_SOLARIS_LIBTHREAD
// Native solaris implementation
#include <thread.h>
#include <synch.h>
#include <errno.h>

// Mutex stuff
#define kvi_mutex_t mutex_t
#define kvi_threadMutexInit(_pMutex_t) mutex_init(_pMutex_t, 0, 0)
#define kvi_threadMutexLock(_pMutex_t) mutex_lock(_pMutex_t)
#define kvi_threadMutexUnlock(_pMutex_t) mutex_unlock(_pMutex_t)
#define kvi_threadMutexDestroy(_pMutex_t) mutex_destroy(_pMutex_t)
inline bool kvi_threadMutexTryLock(kvi_mutex_t * _pMutex_t)
{
	return (mutex_trylock(_pMutex_t) != EBUSY);
};
// Actually unused
// #define kvi_threadMutexTryLock(_pMutex_t) mutex_trylock(_pMutex_t)

// Thread stuff
#define kvi_thread_t thread_t

inline bool kvi_threadCreate(kvi_thread_t * t, void * (*start_routine)(void *), void * arg)
{
	return (thr_create(0, 0, start_routine, arg, THR_DETACHED, t) == 0);
}

// We don't care about exit codes at all
#define kvi_threadExit() thr_exit(0)
#else
// FIXME: 			#warning "Missing a decent thread implementation: we're going to fail, sorry!"
#endif
#endif
#endif

class KVILIB_API KviMutex : public KviHeapObject
{
private:
	kvi_mutex_t m_mutex;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	bool m_bLocked;
#endif
public:
	KviMutex() { kvi_threadMutexInit(&m_mutex); };
	virtual ~KviMutex() { kvi_threadMutexDestroy(&m_mutex); };
public:
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	void lock()
	{
		kvi_threadMutexLock(&m_mutex);
		m_bLocked = true;
	};
	void unlock()
	{
		m_bLocked = false;
		kvi_threadMutexUnlock(&m_mutex);
	};
	bool locked() { return m_bLocked; };
#else
	void lock()
	{
		kvi_threadMutexLock(&m_mutex);
	};
	void unlock() { kvi_threadMutexUnlock(&m_mutex); };
	bool locked();
#endif
};

// simple thread class implementation
// this is also called "Blind" thread class

class KVILIB_API KviThread : public KviHeapObject
{
public:
	KviThread();
	virtual ~KviThread();

private:
	kvi_thread_t m_thread;
	bool m_bRunning;
	bool m_bStartingUp;
	KviMutex * m_pRunningMutex;
	KviPointerList<QEvent> * m_pLocalEventQueue;

public:
	// public KviThread interface
	// HANDLE WITH CARE

	// Runs the thread...call only from external threads!!! :)
	// This function returns true if the child thread has been successfully created
	// this des not mean that run() is being already executed...
	// isStartingUp() will return true from this moment until
	// the child thread jumps into run() where it will be set to running state (isRunning() == true)
	// and removed from startingUp state.
	bool start();
	// Returns the state of the thread...safe to call from anywhere
	bool isRunning();
	// Returns the state of the thread...safe to call from anywhere
	bool isStartingUp(); // start() called, but not in run() yet...
	// Waits for the termination of this thread: call only from external threads!!! :)
	void wait();
	// DO NOT TOUCH THIS ONE!
	void internalThreadRun_doNotTouchThis();

	static void sleep(unsigned long sec);
	static void msleep(unsigned long msec);
	static void usleep(unsigned long usec);

protected:
	// protected KviThread interface
	// HANDLE WITH CARE TOO!

	// Reimplement this with your job
	virtual void run(){};
	// Terminates the execution of the calling thread
	void exit();
	// The tricky part: threadsafe event dispatching
	// Slave thread -> main thread objects
	virtual void postEvent(QObject * o, QEvent * e);

private:
	void setRunning(bool bRunning);
	void setStartingUp(bool bStartingUp);
};

// QEvent::Type for Thread events
#define KVI_THREAD_EVENT (((int)QEvent::User) + 2000)

// CONSTANTS FOR KviThreadEvent::eventId();

///////////////////////////////////////////////////////////////
// extern -> slave thread

// Your reimplementation of KviSensitiveThread MUST handle this
// and exit when this event is received

// Terminate is a plain KviThreadEvent
#define KVI_THREAD_EVENT_TERMINATE 0

///////////////////////////////////////////////////////////////
// slave thread -> master object

// The following standard events are sent from the thread to the master object

// The following are plain KviThreadEvent objects
#define KVI_THREAD_EVENT_SUCCESS 100

// The following are KviThreadDataEvent<int>
#define KVI_THREAD_EVENT_STATECHANGE 150

// The following are KviThreadDataEvent<KviCString>
#define KVI_THREAD_EVENT_MESSAGE 200
#define KVI_THREAD_EVENT_WARNING 201
#define KVI_THREAD_EVENT_ERROR 202
#define KVI_THREAD_EVENT_DATA 203

// The following is KviThreadDataEvent<KviDataBuffer>
#define KVI_THREAD_EVENT_BINARYDATA 300

// The user events
#define KVI_THREAD_USER_EVENT_BASE 1000

// #warning "Get rid of the m_szMessage member of KviThreadEvent : eventual data should be passed with a KviThreadDataEvent"

// Base class for all thread events
class KVILIB_API KviThreadEvent : public QEvent, public KviHeapObject
{
protected:
	int m_eventId;
	KviThread * m_pSender;

public:
	KviThreadEvent(int evId, KviThread * sender = nullptr)
	    : QEvent((QEvent::Type)KVI_THREAD_EVENT), m_eventId(evId), m_pSender(sender){};
	~KviThreadEvent(){};

public:
	// This is the sender of the event
	// WARNING : this MAY be null, threads CAN send anonymous events
	KviThread * sender() { return m_pSender; };
	int id() { return m_eventId; };
};

template <class TData>
class KviThreadDataEvent : public KviThreadEvent
{
protected:
	TData * m_pData;

public:
	KviThreadDataEvent(int evId, TData * pData = nullptr, KviThread * sender = nullptr)
	    : KviThreadEvent(evId, sender) { m_pData = pData; };
	~KviThreadDataEvent()
	{
		if(m_pData)
			delete m_pData;
	};

public:
	void setData(TData * d)
	{
		if(m_pData)
			delete m_pData;
		m_pData = d;
	};
	TData * getData()
	{
		TData * aux = m_pData;
		m_pData = nullptr;
		return aux;
	};
	TData * data() { return m_pData; };
};

// A thread that has also an internal event queue
// so events can be posted from the master side to the slave one
// Reimplementations of this class should periodically check
// dequeueEvent() and eventually process the incoming events (and then DELETE it)

// KVI_THREAD_EVENT_TERMINATE should be always handled by the reimplementation
// and it should always exit (cleanly) when this event is received

class KVILIB_API KviSensitiveThread : public KviThread
{
public:
	KviSensitiveThread();
	virtual ~KviSensitiveThread();

protected:
	KviMutex * m_pLocalEventQueueMutex;
	KviPointerList<KviThreadEvent> * m_pLocalEventQueue;

public:
	// enqueues an event directed to THIS thread
	// the event must be allocated with NEW and
	// will be destroyed on the slave side
	void enqueueEvent(KviThreadEvent * e);
	// enqueues a terminate event and waits() for the slave thread
	// the slave thread MUST handle KVI_THREAD_EVENT_TERMINATE
	void terminate();

protected:
	// slave side:
	// returns the first event in the local queue
	// the event MUST BE DELETED after processing
	KviThreadEvent * dequeueEvent();
};

// =============================================================================================//
// This is private stuff...only KviThread and KviApplication may use it
// and may call only specific functions...don't touch.

struct KviThreadPendingEvent
{
	QObject * o;
	QEvent * e;
};

class KVILIB_API KviThreadManager : public QObject
{
	friend class KviApplication;
	friend class KviThread;
	Q_OBJECT
protected:
	// These should be private...but we don't want anyone to complain
	// Treat as private plz.
	KviThreadManager();
	~KviThreadManager();

public:
	static void killPendingEvents(QObject * receiver);

private:
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	QSocketNotifier * m_pSn;
#endif
	KviMutex * m_pMutex; // This class performs only atomic operations
	KviPointerList<KviThread> * m_pThreadList;
	int m_iWaitingThreads;
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	KviPointerList<KviThreadPendingEvent> * m_pEventQueue;
	int m_fd[2];
	int m_iTriggerCount;
#endif
protected:
	// Public to KviThread only
	void registerSlaveThread(KviThread * t);
	void unregisterSlaveThread(KviThread * t);

	void threadEnteredWaitState();
	void threadLeftWaitState();

	void postSlaveEvent(QObject * o, QEvent * e);
	void killPendingEventsByReceiver(QObject * receiver);
	// Public to KviApplication only
	static void globalInit();
	static void globalDestroy();
private slots:
	void eventsPending(int fd);
};

#endif //!_KVI_THREAD_H_
