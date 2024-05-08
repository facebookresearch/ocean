/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/SharedLock.h"

#if defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))
	#include <sys/sem.h>
#endif

namespace Ocean
{

SharedLock::SharedLock(const std::wstring& name) :
	name_(name)
{
	ocean_assert(!name_.empty());
}

SharedLock::~SharedLock()
{
	ocean_assert(handle_ == nullptr);

#ifdef _WINDOWS

	if (handle_ != nullptr)
	{
		ReleaseMutex(handle_);
	}

#else

	// nothing to release here
	ocean_assert(localCounter_ == 0u);

#endif
}

void SharedLock::lock()
{
	const ScopedLock scopedLock(localLock_);

	ocean_assert(!name_.empty());

	if (localCounter_ == 0u && !name_.empty())
	{

#if defined(_WINDOWS)

		ocean_assert(handle_ == nullptr);

		// try to create the mutex until this process is the owner
		while (true)
		{
			const HANDLE handle = CreateMutex(nullptr, TRUE, name_.c_str());

			if (handle != nullptr)
			{
				if (GetLastError() != ERROR_ALREADY_EXISTS)
				{
					handle_ = handle;
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

		const int semaphoreId = int(size_t(handle_));

		// finally we have to lock the semaphore
		sembuf semaphoreLock = {0, -1, SEM_UNDO};
		const int result = semop(semaphoreId, &semaphoreLock, 1);
		ocean_assert_and_suppress_unused(result == 0, result);

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;

#endif

		localCounter_ = 1u;
	}
	else
		++localCounter_;
}

bool SharedLock::tryLock()
{
	const ScopedLock scopedLock(localLock_);

	ocean_assert(!name_.empty());

	if (localCounter_ == 0u && !name_.empty())
	{

#if defined(_WINDOWS)

		ocean_assert(handle_ == nullptr);

		// try to create the mutex and check whether this process is the owner
		const HANDLE handle = CreateMutex(nullptr, TRUE, name_.c_str());

		if (handle != nullptr)
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				handle_ = handle;
				localCounter_ = 1u;

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

		const int semaphoreId = int(size_t(handle_));

		// finally we have to lock the semaphore
		sembuf semaphoreLock = {0, -1, SEM_UNDO | IPC_NOWAIT};
		const int result = semop(semaphoreId, &semaphoreLock, 1);

		if (result != 0)
		{
			return false;
		}

		localCounter_ = 1u;
		return true;

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;

#endif

	}
	else
	{
		ocean_assert(handle_ != nullptr);

		++localCounter_;
		return true;
	}

	return false;
}

void SharedLock::unlock()
{
	const ScopedLock scopedLock(localLock_);

	ocean_assert(!name_.empty());

	ocean_assert(localCounter_ > 0u);
	ocean_assert(handle_ != nullptr);

	if (localCounter_ == 1u)
	{

#if defined(_WINDOWS)

		CloseHandle(handle_);

#elif defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))

		const int semaphoreId = int(size_t(handle_));

		// we have to unlock the semaphore
		sembuf semaphoreUnlock = {0, 1, SEM_UNDO};
		const int result = semop(semaphoreId, &semaphoreUnlock, 1);
		ocean_assert_and_suppress_unused(result == 0, result);

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;

#endif

		handle_ = nullptr;
		localCounter_ = 0u;
	}
	else
	{
		--localCounter_;
	}
}

#if defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))

bool SharedLock::createSemaphore()
{
	if (handle_ != nullptr)
	{
		return true;
	}

	// we create a (almost unique) hash value from the given name;
	const key_t uniqueKey = key_t(std::hash<std::wstring>()(name_));

	// check whether the semaphore exists already
	int semaphoreId = semget(uniqueKey, 0, IPC_PRIVATE);

	while (semaphoreId == -1)
	{
		// check whether the semaphore has been created in the meantime
		semaphoreId = semget(uniqueKey, 0, IPC_PRIVATE);

		if (semaphoreId != -1)
		{
			break;
		}

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

	handle_ = (void*)size_t(semaphoreId);

	return handle_ != nullptr;
}

#endif // defined(__APPLE__) || defined(__linux__)

}
