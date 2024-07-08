/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Thread.h"
#include "ocean/base/Exception.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/ScopedValue.h"
#include "ocean/base/Timestamp.h"

#ifndef _WINDOWS
	#include <unistd.h>
#endif

#ifdef __linux__
	#include <sys/syscall.h>
#endif

namespace Ocean
{

Thread::Thread(const std::string& name) :
	threadName_(name),
	threadRandomNumberSeedValue_(RandomI::random32())
{
	// nothing to do here
}

Thread::Thread(const unsigned int randomNumberSeedValue, const std::string& name) :
	threadName_(name),
	threadRandomNumberSeedValue_(randomNumberSeedValue)
{
	// nothing to do here
}

Thread::~Thread()
{
	stopThreadExplicitly();
	destroyThread();
}

bool Thread::startThread()
{
	ocean_assert(!threadIsInvokedToStart_);
	threadShouldStop_ = false;

	if (isThreadActive())
		return false;

	destroyThread();

	threadIsInvokedToStart_ = true;

	createThread();

	return true;
}

void Thread::stopThread()
{
	threadShouldStop_ = true;
}

bool Thread::terminateThread()
{
	threadIsInvokedToStart_ = false;

#ifdef _WINDOWS

	if (threadHandle_ != nullptr)
	{
		return TerminateThread(threadHandle_, 0) == TRUE;
	}

#else

	Log::error() << "This platform does not support explicit thread termination.";

#endif

	return false;
}

bool Thread::joinThread(const unsigned int timeout)
{

#if defined(_WINDOWS)

	if (threadHandle_)
	{
		const DWORD result = WaitForSingleObject(threadHandle_, timeout == (unsigned int)(-1) ? INFINITE : (DWORD)timeout);

		if (result == WAIT_OBJECT_0)
		{
			return true;
		}

		return false;
	}

#else

	if (threadObject_)
	{
		if (timeout == (unsigned int)(-1))
		{
			void* returnValue = nullptr;
			if (pthread_join(threadObject_, &returnValue) != 0)
			{
				return false;
			}

			// the thread has terminated, so we must not use the corresponding id anymore
			// e.g., calling pthread_detach(threadObject_) could result in a crash

			threadObject_ = 0;
			return true;
		}
		else
		{

#if defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))
			const Timestamp currentTimestamp(true);

			const double currentTimestampSeconds = double(int64_t(double(currentTimestamp)));
			const double currenTimestampNanoseconds = double((currentTimestamp - currentTimestampSeconds).nanoseconds());

			struct timespec absTime;
			absTime.tv_sec = long(currentTimestampSeconds);
			absTime.tv_nsec = long(currenTimestampNanoseconds);

			return pthread_timedjoin_np(threadObject_, nullptr, &absTime) == 0;
#else

			OCEAN_WARNING_MISSING_IMPLEMENTATION;

			void* returnValue = nullptr;
			if (pthread_join(threadObject_, &returnValue) != 0)
			{
				return false;
			}

			// the thread has terminated, so we must not use the corresponding id anymore
			// e.g., calling pthread_detach(threadObject_) could result in a crash

			threadObject_ = 0;
			return true;
#endif
		}
	}

#endif

	return true;
}

bool Thread::shouldThreadStop() const
{
	return threadShouldStop_;
}

bool Thread::isThreadInvokedToStart() const
{
	return threadIsInvokedToStart_;
}

bool Thread::isThreadActive() const
{
#ifdef _WINDOWS

	if (threadHandle_ != nullptr)
	{
		DWORD code = 0;
		if (GetExitCodeThread(threadHandle_, &code) == TRUE || code == STILL_ACTIVE)
		{
			return code == STILL_ACTIVE;
		}
		else
		{
			return false;
		}
	}

	return false;

#else

	return threadIsActive_;

#endif // _WINDOWS
}

void Thread::sleep(unsigned int ms)
{
#if defined(_WINDOWS)

	static_assert(sizeof(ms) == sizeof(DWORD), "Invalid data type!");
	Sleep(DWORD(ms));

#else

	usleep(ms * 1000);

#endif
}

void Thread::giveUp()
{
#if defined(_WINDOWS)

	Sleep(0);

#else

	sched_yield();

#endif
}

Thread::ThreadId Thread::currentThreadId()
{
#if defined(__APPLE__)

	__uint64_t value = 0;
	const int result = pthread_threadid_np(nullptr, &value);
	ocean_assert_and_suppress_unused(result == 0, result);

	return ThreadId(uint64_t(value));

#elif defined(_WINDOWS)

	static_assert(sizeof(DWORD) == sizeof(unsigned int), "Invalid size of the data type!");
	return ThreadId(uint64_t(GetCurrentThreadId()));

#elif defined(__linux__) && !defined(_ANDROID) && !defined(__EMSCRIPTEN__)

	// From "man 2 gettid": Glibc does not provide a wrapper for this system call (i.e. gettid); call it using syscall(2).
	static_assert(sizeof(pid_t) == sizeof(unsigned int), "Invalid size of the data type!");
	return ThreadId(uint64_t(syscall(SYS_gettid)));

#else

	static_assert(sizeof(pid_t) == sizeof(unsigned int), "Invalid size of the data type!");
	return ThreadId(uint64_t(gettid()));

#endif
}

#if defined(__APPLE__)

// Cf. impl/src/ocean/base/Thread.mm

#else // defined(__APPLE__)

Thread::ThreadPriority Thread::threadPriority()
{

#ifdef _WINDOWS

	const HANDLE threadHandle = GetCurrentThread();
	ocean_assert(threadHandle != nullptr);

	switch (GetThreadPriority(threadHandle))
	{
		case THREAD_PRIORITY_IDLE:
			return PRIORITY_IDLE;

		case THREAD_PRIORITY_BELOW_NORMAL:
			return PRIORTY_BELOW_NORMAL;

		case THREAD_PRIORITY_NORMAL:
			return PRIORTY_NORMAL;

		case THREAD_PRIORITY_ABOVE_NORMAL:
			return PRIORTY_ABOVE_NORMAL;

		case THREAD_PRIORITY_HIGHEST:
			return PRIORTY_HIGH;

		case THREAD_PRIORITY_TIME_CRITICAL:
			return PRIORTY_REALTIME;
	}

	ocean_assert(false && "Unknown priority value.");
	return PRIORTY_NORMAL;

#else

	return PRIORTY_NORMAL;

#endif

}

bool Thread::setThreadPriority(const ThreadPriority priority)
{

#ifdef _WINDOWS

	const HANDLE threadHandle = GetCurrentThread();

	if (threadHandle == nullptr)
	{
		return false;
	}

	switch (priority)
	{
		case PRIORITY_IDLE:
			return SetThreadPriority(threadHandle, THREAD_PRIORITY_IDLE) == TRUE;

		case PRIORTY_BELOW_NORMAL:
			return SetThreadPriority(threadHandle, THREAD_PRIORITY_BELOW_NORMAL) == TRUE;

		case PRIORTY_NORMAL:
			return SetThreadPriority(threadHandle, THREAD_PRIORITY_NORMAL) == TRUE;

		case PRIORTY_ABOVE_NORMAL:
			return SetThreadPriority(threadHandle, THREAD_PRIORITY_ABOVE_NORMAL) == TRUE;

		case PRIORTY_HIGH:
			return SetThreadPriority(threadHandle, THREAD_PRIORITY_HIGHEST) == TRUE;

		case PRIORTY_REALTIME:
			return SetThreadPriority(threadHandle, THREAD_PRIORITY_TIME_CRITICAL) == TRUE;
	};

	ocean_assert(false && "Unknown priority value.");
	return false;

#else

	OCEAN_SUPPRESS_UNUSED_WARNING(priority);
	return false;

#endif

}

#endif // defined(__APPLE__)

#ifdef __APPLE__

int Thread::pthread_timedjoin_np(pthread_t thread, void** retval, const struct timespec* abstime)
{
	ocean_assert(abstime);

	TimedJoinPair joinPair(thread, false);

	pthread_t helperThreadId;
	if (pthread_create(&helperThreadId, nullptr, pthread_timedjoin_np_helper, &joinPair) != 0)
	{
		return -1;
	}

	const Timestamp absTimestamp(double(abstime->tv_sec) + double(abstime->tv_nsec) * 0.000000001);

	while (Timestamp(true) < absTimestamp && joinPair.second == false)
	{
		sleep(1u);
	}

	int result = 0;

	// check whether the thread has ended gracefully
	if (joinPair.second)
	{
		pthread_cancel(helperThreadId);
		result = ETIMEDOUT;
	}

	pthread_join(helperThreadId, nullptr);

	return result;
}

void* Thread::pthread_timedjoin_np_helper(void* threadData)
{
	ocean_assert(threadData);

	TimedJoinPair* joinPair = (TimedJoinPair*)threadData;

	pthread_join(joinPair->first, nullptr);
	joinPair->second = true;

	return nullptr;
}

#endif // __APPLE__

void Thread::createThread()
{
#if defined(_WINDOWS)

	ocean_assert(threadHandle_ == nullptr);
	threadHandle_ = CreateThread(nullptr, 0, staticThreadRun, this, 0, nullptr);

#else

	ocean_assert(threadObject_ == 0);
	pthread_create(&threadObject_, nullptr, staticThreadRun, this);

#endif
}

void Thread::destroyThread()
{
#if defined(_WINDOWS)

	ocean_assert(isThreadActive() == false);

	if (threadHandle_)
	{
		CloseHandle(threadHandle_);
		threadHandle_ = nullptr;
	}

#else

	if (threadObject_ != 0)
	{
		pthread_detach(threadObject_);
		threadObject_ = 0;
	}

#endif
}

void Thread::stopThreadExplicitly(const unsigned int timeout)
{
	stopThread();

	if (isThreadActive())
	{
		// the thread has 5 seconds to stop by itself
		// otherwise it will be terminated roughly

#ifdef _WINDOWS
		if (WaitForSingleObject(threadHandle_, timeout) == WAIT_TIMEOUT)
#else
		Timestamp now(true);

		while (threadIsActive_ && Timestamp(true) < now + double(timeout) * double(0.001))
		{
			sleep(1);
		}

		if (threadIsActive_)
#endif
		{
			Log::info() << "The thread \"" << threadName_ << "\" will be terminated roughly.";

			if (terminateThread() == false)
			{
				Log::error() << "The thread \"" << threadName_ << "\" couldn't be terminated roughly.";
			}
		}
	}
}

void Thread::internalThreadRun()
{
	// here we first initialize the standard random generator with the seed value for this thread
	RandomI::initialize(threadRandomNumberSeedValue_);
#if defined(_WINDOWS)

	#ifdef OCEAN_COMPILER_MSC
		#ifdef OCEAN_DEBUG

	if (!threadName_.empty() && IsDebuggerPresent())
	{
		// code from MS article "How to: Set a Thread Name in Native Code"
		// http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx

		const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)

		typedef struct tagTHREADNAME_INFO
		{
			DWORD dwType; // Must be 0x1000.
			LPCSTR szName; // Pointer to name (in user addr space).
			DWORD dwThreadID; // Thread ID (-1=caller thread).
			DWORD dwFlags; // Reserved for future use, must be zero.
		} THREADNAME_INFO;

#pragma pack(pop)

		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName_.c_str();
		info.dwThreadID = DWORD(-1);
		info.dwFlags = 0;

#ifdef OCEAN_SUPPORT_EXCEPTIONS

		try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
		}
		catch (...)
		{
			// nothing to do here
		}

#endif // OCEAN_SUPPORT_EXCEPTIONS
	}

		#endif // OCEAN_DEBUG
	#endif // OCEAN_COMPILER_MSC

#elif defined(__APPLE__)

	#ifdef OCEAN_DEBUG
		if (!threadName_.empty())
			ocean_assert(pthread_setname_np(threadName_.c_str()) == 0);
	#endif

	#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
		// on iOS platforms the main thread seems to have a higher priority than the remaining threads of a process
		// therefore, we set the priority of any additional thread to the highest priority possible so that e.g., a tracker does not lose performance

		const bool priorityResult = Thread::setThreadPriority(Thread::PRIORTY_REALTIME);
		ocean_assert_and_suppress_unused(priorityResult == true, priorityResult);
	#endif

#endif // defined(_WINDOWS)

	const ScopedValueT<bool> scopedIsActiveValue(threadIsActive_, false, true);

	ocean_assert(threadIsInvokedToStart_);
	threadIsInvokedToStart_ = false;

	threadRun();
}

#ifdef _WINDOWS

DWORD __stdcall Thread::staticThreadRun(void* data)
{
	ocean_assert(data);

#ifdef OCEAN_SUPPORT_EXCEPTIONS

	try
	{
		((Thread*)data)->internalThreadRun();
	}
	catch (const OceanException& exception)
	{
		ocean_assert(false && "Should be caught earlier!");
		Log::error() << "An exception hasn't been caught in thread \"" << ((Thread*)data)->threadName_ << "\". The reason was: " << exception.what() << ".";
	}
	catch (const Exception& exception)
	{
		ocean_assert(false && "Should be caught earlier!");
		Log::error() << "An exception hasn't been caught in thread \"" << ((Thread*)data)->threadName_ << "\". The reason was: " << exception.what() << ".";
	}
	catch (...)
	{
		ocean_assert(false && "Should be caught earlier!");
		Log::error() << "An exception hasn't been caught in thread \"" << ((Thread*)data)->threadName_ << "\".";
	}

#else

	((Thread*)data)->internalThreadRun();

#endif // OCEAN_SUPPORT_EXCEPTIONS

	return 1;
}

#else

void* Thread::staticThreadRun(void* data)
{
	ocean_assert(data);

#ifdef OCEAN_SUPPORT_EXCEPTIONS

	try
	{
		((Thread*)data)->internalThreadRun();
	}
	catch (const Exception& exception)
	{
		ocean_assert(false && "Should be caught earlier!");
		Log::error() << "An exception hasn't been caught in thread \"" << ((Thread*)data)->threadName_ << "\". The reason was: " << exception.what() << ".";
	}
	catch (...)
	{
		ocean_assert(false && "Should be caught earlier!");
		Log::error() << "An exception hasn't been caught in thread \"" << ((Thread*)data)->threadName_ << "\".";
	}

#else

	((Thread*)data)->internalThreadRun();

#endif // OCEAN_SUPPORT_EXCEPTIONS

	return nullptr;
}

#endif

} // namespace Ocean
