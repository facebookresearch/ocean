/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/SharedMemory.h"

#if defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))
	#include <sys/shm.h>
#endif

namespace Ocean
{

SharedMemory::SharedMemory(const std::wstring& name, const size_t size) :
	name_(name),
	size_(size)
{
	ocean_assert(!name_.empty());

	if (!name_.empty() && size_ > 0)
	{
		requestSharedMemory(name_, size_, handle_, data_);
		ocean_assert(data_);
	}
	else
	{
		size_ = 0;
	}
}

SharedMemory::SharedMemory(SharedMemory&& sharedMemory) noexcept :
	SharedMemory()
{
	*this = std::move(sharedMemory);
}

SharedMemory::~SharedMemory()
{
	release();
}

bool SharedMemory::resize(const size_t newSize)
{
	if (newSize == size_)
	{
		return true;
	}

	release();
	ocean_assert(size_ == 0);
	ocean_assert(data_ == nullptr);
	ocean_assert(handle_ == nullptr);

	if (newSize > 0)
	{
		size_ = newSize;

		requestSharedMemory(name_, size_, handle_, data_);
		ocean_assert(data_);
	}

	return true;
}

void SharedMemory::flush()
{

#ifdef _WINDOWS

	if (data_)
	{
		FlushViewOfFile(data_, 0);
	}

#else

	// nothing to do here

#endif
}

void SharedMemory::release()
{

#if defined(_WINDOWS)

	if (data_)
	{
		const BOOL result = UnmapViewOfFile(data_);
		ocean_assert_and_suppress_unused(result == TRUE, result);
	}

	if (handle_)
	{
		const BOOL result = CloseHandle(handle_);
		ocean_assert_and_suppress_unused(result == TRUE, result);
	}

#elif defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))

	if (data_)
	{
		shmdt(data_);
		data_ = nullptr;
	}

	if (handle_)
	{
		shmctl(int(size_t(handle_)), IPC_RMID, nullptr);
		handle_ = nullptr;
	}

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;

#endif

	data_ = nullptr;
	handle_ = nullptr;
	size_ = 0;
}

SharedMemory& SharedMemory::operator=(SharedMemory&& sharedMemory) noexcept
{
	if (this != &sharedMemory)
	{
		release();

		size_ = sharedMemory.size_;
		name_ = std::move(sharedMemory.name_);
		handle_ = sharedMemory.handle_;
		data_ = sharedMemory.data_;

		sharedMemory.size_ = 0;
		sharedMemory.handle_ = nullptr;
		sharedMemory.data_ = nullptr;
	}

	return *this;
}

bool SharedMemory::requestSharedMemory(const std::wstring& name, size_t& size, void*& handle, void*& data, bool* existedAlready)
{
	ocean_assert(!name.empty());
	ocean_assert(size > 0);

	handle = nullptr;
	data = nullptr;

#if defined(_WINDOWS)

	unsigned int lowSize = 0u;
	unsigned int highSize = 0u;

	lowSize = (unsigned int)size;
	highSize = (unsigned int)(((unsigned long long)size) >> 32);

	const HANDLE handle_ = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, highSize, lowSize, (std::wstring(L"Local\\") + name).c_str());

	if (handle_ == nullptr)
	{
		size = 0;
		return false;
	}

	const bool newCreated = GetLastError() != ERROR_ALREADY_EXISTS;

	if (existedAlready)
	{
		*existedAlready = !newCreated;
	}

	void* data_ = MapViewOfFile(handle_, FILE_MAP_WRITE, 0, 0, 0);

	if (data_ == nullptr)
	{
		ocean_assert(false && "This should never happen!");
		CloseHandle(handle_);
		return false;
	}

	// we initialize the entire memory with zero if the memory has been created for the first time
	if (newCreated)
		memset(data_, 0, size);

	handle = handle_;
	data = data_;
	return true;

#elif defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))

	// we create a (almost unique) hash value from the given name;
	const key_t uniqueKey = key_t(std::hash<std::wstring>()(name));

	bool newCreated = false;

	int memoryId = shmget(uniqueKey, size, 0666);

	if (memoryId == -1)
	{
		memoryId = shmget(uniqueKey, size, IPC_CREAT | 0666);

#ifdef OCEAN_DEBUG
		ocean_assert(memoryId != -1);
#endif

		if (memoryId == -1)
		{
			return false;
		}

		newCreated = true;
	}

	if (existedAlready)
	{
		*existedAlready = !newCreated;
	}

	void* data_ = shmat(memoryId, nullptr, 0);

	if (data_ == nullptr)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	// we initialize the entire memory with zero if the memory has been created for the first time
	if (newCreated)
	{
		memset(data_, 0, size);
	}

	handle = (void*)size_t(memoryId);
	data = data_;
	return true;

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	return false;

#endif

}

}
