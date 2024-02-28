// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/base/SharedLock.h"

#if defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))
	#include <sys/sem.h>
#endif

namespace Ocean
{

SharedLock::SharedLock() :
	lockLocalCounter(0u),
	lockHandle(NULL)
{
	// nothing do do here
}

SharedLock::SharedLock(const std::wstring& name) :
	lockName(name),
	lockLocalCounter(0u),
	lockHandle(NULL)
{
	ocean_assert(!lockName.empty());
}

SharedLock::~SharedLock()
{
	ocean_assert(lockHandle == NULL);

#ifdef _WINDOWS

	if (lockHandle)
		ReleaseMutex(lockHandle);

#else

	// nothing to release here
	ocean_assert(lockLocalCounter == 0u);

#endif
}

void SharedLock::lock()
{
	const ScopedLock scopedLock(lockLocalLock);

	ocean_assert(!lockName.empty());

	if (lockLocalCounter == 0u && !lockName.empty())
	{

#if defined(_WINDOWS)

		ocean_assert(lockHandle == NULL);

		// try to create the mutex until this process is the owner
		while (true)
		{
			const HANDLE handle = CreateMutex(NULL, TRUE, lockName.c_str());

			if (handle != NULL)
			{
				if (GetLastError() != ERROR_ALREADY_EXISTS)
				{
					lockHandle = handle;
					break;
				}

				// we are not the owner of this mutex, however we have to close the handle
				CloseHandle(handle);
			}

			Sleep(0);
		}

#elif defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))

		if (!createSemaphore())
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		const int semaphoreId = int(size_t(lockHandle));

		// finally we have to lock the semaphore
		sembuf semaphoreLock = {0, -1, SEM_UNDO};
		const int result = semop(semaphoreId, &semaphoreLock, 1);
		ocean_assert_and_suppress_unused(result == 0, result);

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;

#endif

		lockLocalCounter = 1u;
	}
	else
		++lockLocalCounter;
}

bool SharedLock::tryLock()
{
	const ScopedLock scopedLock(lockLocalLock);

	ocean_assert(!lockName.empty());

	if (lockLocalCounter == 0u && !lockName.empty())
	{

#if defined(_WINDOWS)

		ocean_assert(lockHandle == NULL);

		// try to create the mutex and check whether this process is the owner
		const HANDLE handle = CreateMutex(NULL, TRUE, lockName.c_str());

		if (handle != NULL)
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				lockHandle = handle;
				lockLocalCounter = 1u;

				return true;
			}

			// we are not the owner of this mutex, however we have to close the handle
			CloseHandle(handle);
		}

#elif defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))

		if (!createSemaphore())
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		const int semaphoreId = int(size_t(lockHandle));

		// finally we have to lock the semaphore
		sembuf semaphoreLock = {0, -1, SEM_UNDO | IPC_NOWAIT};
		const int result = semop(semaphoreId, &semaphoreLock, 1);

		if (result != 0)
			return false;

		lockLocalCounter = 1u;
		return true;

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;

#endif

	}
	else
	{
		ocean_assert(lockHandle != NULL);

		++lockLocalCounter;
		return true;
	}

	return false;
}

void SharedLock::unlock()
{
	const ScopedLock scopedLock(lockLocalLock);

	ocean_assert(!lockName.empty());

	ocean_assert(lockLocalCounter > 0u);
	ocean_assert(lockHandle != NULL);

	if (lockLocalCounter == 1u)
	{

#if defined(_WINDOWS)

		CloseHandle(lockHandle);

#elif defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))

		const int semaphoreId = int(size_t(lockHandle));

		// we have to unlock the semaphore
		sembuf semaphoreUnlock = {0, 1, SEM_UNDO};
		const int result = semop(semaphoreId, &semaphoreUnlock, 1);
		ocean_assert_and_suppress_unused(result == 0, result);

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;

#endif

		lockHandle = NULL;
		lockLocalCounter = 0u;
	}
	else
		--lockLocalCounter;
}

#if defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))

bool SharedLock::createSemaphore()
{
	if (lockHandle != NULL)
		return true;

	// we create a (almost unique) hash value from the given name;
	const key_t uniqueKey = key_t(std::hash<std::wstring>()(lockName));

	// check whether the semaphore exists already
	int semaphoreId = semget(uniqueKey, 0, IPC_PRIVATE);

	while (semaphoreId == -1)
	{
		// check whether the semaphore has been created in the meantime
		semaphoreId = semget(uniqueKey, 0, IPC_PRIVATE);

		if (semaphoreId != -1)
			break;

		// we create the semaphore on our own
		semaphoreId = semget(uniqueKey, 1, IPC_CREAT | IPC_EXCL | 0666);
		ocean_assert(semaphoreId != -1);

		// we have to initialize the semaphore's value

		if (semaphoreId != -1)
		{
			const int result = semctl(semaphoreId, 0, SETVAL, 1);
			ocean_assert_and_suppress_unused(result != -1, result);
		}
	}

	lockHandle = (void*)size_t(semaphoreId);

	return lockHandle != NULL;
}

#endif // defined(__APPLE__) || defined(__linux__)

}
