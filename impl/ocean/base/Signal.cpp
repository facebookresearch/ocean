/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Signal.h"
#include "ocean/base/DateTime.h"

#include <time.h>

#ifdef _WINDOWS
	#include <winsock2.h>
	#include <windows.h>
#endif

namespace Ocean
{

Signal::Signal()
{

#if defined(_WINDOWS)

	signalHandle = CreateEvent(nullptr, false, false, nullptr);

#elif defined(__APPLE__)

	semaphoreObject = dispatch_semaphore_create(0);
	ocean_assert(semaphoreObject != nullptr);

	semaphoreObjectReleased = false;

#else

	semaphoreObjectState = sem_init(&semaphoreObject, 0, 0) == 0;
	semaphoreObjectReleased = false;

#endif

}

Signal::~Signal()
{

#if defined(_WINDOWS)

	if (signalHandle)
	{
		CloseHandle(signalHandle);
	}

#elif defined(__APPLE__)

	if (semaphoreObject != nullptr)
	{
		/*
		 * Although Apple's documentation regarding dispatch_release states the following,
		 * we have to release the semaphore object to avoid a memory leak.
		 *
		 * Important
		 * If your app is built with a deployment target of macOS 10.8 and later or iOS v6.0 and later,
		 * dispatch queues are typically managed by ARC,
		 * so you do not need to retain or release the dispatch queues.
		 */

		// so we do not release the semaphore object anymore
		dispatch_release(semaphoreObject);
	}

#else

	if (semaphoreObjectState)
	{
		sem_destroy(&semaphoreObject);
	}

#endif

}

void Signal::wait() const
{

#if defined(_WINDOWS)

	ocean_assert(signalHandle);
	WaitForSingleObject(signalHandle, INFINITE);

#elif defined(__APPLE__)

	if (!semaphoreObjectReleased)
	{
		ocean_assert(semaphoreObject != nullptr);
		dispatch_semaphore_wait(semaphoreObject, DISPATCH_TIME_FOREVER);
	}

#else

	if (!semaphoreObjectReleased)
	{
		ocean_assert(semaphoreObjectState);
		sem_wait(&semaphoreObject);
	}

#endif
}

bool Signal::wait(const unsigned int time) const
{

#if defined(_WINDOWS)

	ocean_assert(signalHandle);
	return WAIT_OBJECT_0 == WaitForSingleObject(signalHandle, DWORD(time));

#elif defined(__APPLE__)

	if (!semaphoreObjectReleased)
	{
		ocean_assert(semaphoreObject != nullptr);
		dispatch_time_t dispatchTime = dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_MSEC * time);

		return dispatch_semaphore_wait(semaphoreObject, dispatchTime) == 0;
	}

	return true;

#else

	if (!semaphoreObjectReleased)
	{
		ocean_assert(semaphoreObjectState);

		timespec stopTimestamp;
		clock_gettime(CLOCK_REALTIME, &stopTimestamp);

		// Normalize the timespec, range of nanoseconds: [0, 999999999]
		const unsigned int nsec = stopTimestamp.tv_nsec + (time % 1000u) * 1000000u;
		stopTimestamp.tv_nsec = nsec % 1000000000u;
		stopTimestamp.tv_sec += (time / 1000u) + (nsec / 1000000000u);

		return sem_timedwait(&semaphoreObject, &stopTimestamp) == 0;
	}

	return true;

#endif

}

bool Signal::pulse()
{

#if defined(_WINDOWS)

	ocean_assert(signalHandle);
	return SetEvent(signalHandle) == TRUE;

#elif defined(__APPLE__)

	ocean_assert(semaphoreObject != nullptr);
	ocean_assert(!semaphoreObjectReleased);

	return dispatch_semaphore_signal(semaphoreObject) == 0;

#else

	ocean_assert(semaphoreObjectState);
	ocean_assert(!semaphoreObjectReleased);

	return sem_post(&semaphoreObject) == 0;

#endif

}

bool Signal::reset()
{

#if defined(_WINDOWS)

	ocean_assert(signalHandle);
	return ResetEvent(signalHandle) == TRUE;

#elif defined (__APPLE__)

	ocean_assert(semaphoreObject != nullptr);
	ocean_assert(!semaphoreObjectReleased);

	while (dispatch_semaphore_wait(semaphoreObject, 0) == 0);

	return true;

#else

	ocean_assert(semaphoreObjectState);
	ocean_assert(!semaphoreObjectReleased);

	while (sem_trywait(&semaphoreObject) == 0);
#if !defined(__linux__)
	ocean_assert(semaphoreObject.count == 0);
#endif

	return true;

#endif

}

void Signal::release()
{

#if defined(_WINDOWS)

	if (signalHandle)
	{
		SetEvent(signalHandle);
		CloseHandle(signalHandle);
		signalHandle = nullptr;
	}

#elif defined(__APPLE__)

	if (semaphoreObject != nullptr)
	{
		dispatch_semaphore_signal(semaphoreObject);

		// as we cannot invoke dispatch_release() here (as another thread may wait/block for this semaphore)
		// therefore, we set the release state here and destory the object in the destructor
		semaphoreObjectReleased = true;
	}

#else

	if (semaphoreObjectState)
	{
		sem_post(&semaphoreObject);

		// as we cannot invoke sem_destroy() here (as another thread may wait/block for this semaphore)
		// therefore, we set the release state here and destory the object in the destructor
		semaphoreObjectReleased = true;
	}

#endif

}

Signals::Signals() :
	signalObjects(nullptr),
	signalsSize(0u)
{
	// nothing to do here
}

Signals::Signals(const unsigned int size) :
	signalObjects(nullptr),
	signalsSize(size)
{
	ocean_assert(signalsSize > 0u);

	if (signalsSize > 0u)
	{
		signalObjects = new Signal[signalsSize];
	}
}

Signals::~Signals()
{
	delete [] signalObjects;
}

void Signals::wait() const
{
#ifdef _WINDOWS

	if (signalsSize == 1u)
	{
		ocean_assert(signalObjects);

		const DWORD result = WaitForSingleObject(signalObjects[0].handle(), INFINITE);
		ocean_assert_and_suppress_unused(result == WAIT_OBJECT_0, result);
	}
	else
	{
		const DWORD result = WaitForMultipleObjects(DWORD(signalsSize), (HANDLE*)signalObjects, TRUE, INFINITE);
		ocean_assert_and_suppress_unused(result == WAIT_OBJECT_0, result);
	}

#else

	for (unsigned int n = 0u; n < signalsSize; ++n)
	{
		signalObjects[n].wait();
	}

#endif
}

void Signals::waitSubset(const unsigned int signalsCount) const
{
	ocean_assert(signalsCount > 0u);

#ifdef _WINDOWS

	if (signalsSize == 1u)
	{
		ocean_assert(signalObjects);

		const DWORD result = WaitForSingleObject(signalObjects[0].handle(), INFINITE);
		ocean_assert_and_suppress_unused(result, result);
	}
	else
	{
		const DWORD result = WaitForMultipleObjects(DWORD(min(signalsCount, signalsSize)), (HANDLE*)signalObjects, TRUE, INFINITE);
		ocean_assert_and_suppress_unused(result != WAIT_FAILED, result);
	}

#else

	for (unsigned int n = 0u; n < signalsSize && n < signalsCount; ++n)
	{
		signalObjects[n].wait();
	}

#endif
}

bool Signals::wait(const unsigned int time) const
{
#ifdef _WINDOWS

	if (signalsSize == 1)
	{
		ocean_assert(signalObjects);
		return WaitForSingleObject(signalObjects[0].handle(), DWORD(time)) != WAIT_TIMEOUT;
	}
	else
	{
		return WaitForMultipleObjects(DWORD(signalsSize), (HANDLE*)signalObjects, TRUE, DWORD(time)) != WAIT_TIMEOUT;
	}

#else

	unsigned int remainingTime = time;

	for (unsigned int n = 0u; n < signalsSize; ++n)
	{
		const long long startTimestamp = DateTime::microseconds();
		signalObjects[n].wait(remainingTime);
		const long long stopTimestamp = DateTime::microseconds();

		const unsigned int passedTime = (unsigned int)((stopTimestamp - startTimestamp) / 1000ll);

		if (passedTime >= remainingTime)
		{
			return false;
		}

		remainingTime -= passedTime;
		ocean_assert(remainingTime <= time);
	}

	return true;

#endif
}

bool Signals::waitSubset(const unsigned int signalsCount, const unsigned int time)
{
	ocean_assert(signalsCount > 0u);

#ifdef _WINDOWS

	if (signalsSize == 1u)
	{
		ocean_assert(signalsCount == 1u);
		ocean_assert(signalObjects);

		return WaitForSingleObject(signalObjects[0].handle(), DWORD(time)) != WAIT_TIMEOUT;
	}
	else
	{
		return WaitForMultipleObjects(DWORD(min(signalsCount, signalsSize)), (HANDLE*)signalObjects, TRUE, DWORD(time)) != WAIT_TIMEOUT;
	}

#else

	unsigned int remainingTime = time;

	for (unsigned int n = 0; n < signalsSize && n < signalsCount; ++n)
	{
		const long long startTimestamp = DateTime::microseconds();
		signalObjects[n].wait(remainingTime);
		const long long stopTimestamp = DateTime::microseconds();

		const unsigned int passedTime = (unsigned int)((stopTimestamp - startTimestamp) / 1000ll);

		if (passedTime >= remainingTime)
		{
			return false;
		}

		remainingTime -= passedTime;
	}

	return true;

#endif
}

bool Signals::pulse()
{
	bool result = true;

	for (unsigned int n = 0u; n < signalsSize; ++n)
	{
		result = signalObjects[n].pulse() && result;
	}

	return result;
}

bool Signals::reset()
{
	bool result = true;

	for (unsigned int n = 0u; n < signalsSize; ++n)
	{
		result = signalObjects[n].reset() && result;
	}

	return result;
}

void Signals::setSize(const unsigned int size)
{
	delete [] signalObjects;
	signalObjects = nullptr;
	signalsSize = size;

	if (signalsSize > 0u)
	{
		signalObjects = new Signal[signalsSize];
	}
}

Signal& Signals::operator[](const unsigned int index) const
{
	ocean_assert(index < signalsSize);
	return signalObjects[index];
}

} // namespace Ocean
