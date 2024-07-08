/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SHARED_MEMORY_H
#define META_OCEAN_BASE_SHARED_MEMORY_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"

namespace Ocean
{

/**
 * This class implements a shared memory object allowing access to a shared buffer.
 * @see SharedLock.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT SharedMemory
{
	public:

		/**
		 * Creates a new shared memory object.
		 */
		SharedMemory() = default;

		/**
		 * Creates a new shared memory object with specified buffer size.
		 * @param name System wide unique name of the shared memory
		 * @param size Size of the buffer in bytes
		 */
		SharedMemory(const std::wstring& name, const size_t size);

		/**
		 * Disabled copy constructor for a shared memory object.
		 * @param sharedMemory Shared memory object to be copied
		 */
		SharedMemory(const SharedMemory& sharedMemory) = delete;

		/**
		 * Move constructor for a shared memory object.
		 * @param sharedMemory Shared memory object to be copied
		 */
		SharedMemory(SharedMemory&& sharedMemory) noexcept;

		/**
		 * Destructs a shared memory object.
		 */
		~SharedMemory();

		/**
		 * Returns the system wide unique name of this shared memory
		 * @return Shared memory name
		 */
		inline const std::wstring& name() const;

		/**
		 * Returns the size of the shared memory buffer in bytes.
		 * @return Buffer size
		 */
		inline size_t size() const;

		/**
		 * Resizes the shared memory buffer.
		 * @param newSize New size of the shared buffer in bytes
		 * @return True, if succeeded
		 */
		bool resize(const size_t newSize);

		/**
		 * Flushes the shared memory.
		 */
		void flush();

		/**
		 * Releases the shared memory buffer.
		 */
		void release();

		/**
		 * Returns a pointer to the shared memory buffer.
		 * @return Memory buffer
		 */
		inline const void* constdata() const;

		/**
		 * Returns a pointer to the shared memory buffer.
		 * @return Memory buffer
		 */
		inline void* data();

		/**
		 * Returns whether this shared memory object holds a valid shared buffer.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Disabled assign operator for shared memory objects.
		 * @param sharedMemory Shared memory object to be assigned
		 * @return Reference to this object
		 */
		SharedMemory& operator=(const SharedMemory& sharedMemory) = delete;

		/**
		 * Move operator for shared memory objects.
		 * @param sharedMemory Shared memory object to be moved
		 * @return Reference to this object
		 */
		SharedMemory& operator=(SharedMemory&& sharedMemory) noexcept;

	private:

		/**
		 * Requests a shared memory buffer.
		 * @param name Unique system wide memory name
		 * @param size Size of the requested memory and finally the resulting size of the created memory (or already existing memory)
		 * @param handle Resulting memory handle
		 * @param data Resulting buffer pointer
		 * @param existedAlready Optional resulting whether the buffer existed already
		 * @return True, if succeeded
		 */
		static bool requestSharedMemory(const std::wstring& name, size_t& size, void*& handle, void*& data, bool* existedAlready = nullptr);

	private:

		/// System wide unique memory name.
		std::wstring name_;

		/// Sized of the shared memory buffer in bytes.
		size_t size_ = 0;

		/// Pointer to the shared memory.
		void* data_ = nullptr;

		/// Shared memory handle.
		void* handle_ = nullptr;
};

inline const std::wstring& SharedMemory::name() const
{
	return name_;
}

inline size_t SharedMemory::size() const
{
	return size_;
}

inline const void* SharedMemory::constdata() const
{
	return data_;
}

inline void* SharedMemory::data()
{
	return data_;
}

inline SharedMemory::operator bool() const
{
	return data_ != nullptr;
}

}

#endif // META_OCEAN_BASE_SHARED_MEMORY_H
