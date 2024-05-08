/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SHARED_LOCK_H
#define META_OCEAN_BASE_SHARED_LOCK_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"

namespace Ocean
{

/**
 * This class implements a lock shared over individual processes (a system-wide lock).
 * @see ScopedSharedLock, SharedMemory.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT SharedLock
{
	public:

		/**
		 * Creates an invalid shared lock object.
		 */
		SharedLock() = default;

		/**
		 * Creates a new shared lock object by a system unique name of this lock.
		 * @param name System wide unique lock name
		 */
		explicit SharedLock(const std::wstring& name);

		/**
		 * Disabled copy constructor.
		 * @param sharedLock Shared lock object to be copied
		 */
		SharedLock(const SharedLock& sharedLock) = delete;

		/**
		 * Destructs a shared lock object.
		 */
		~SharedLock();

		/**
		 * Returns the system wide unique name of this lock.
		 * @return Lock name
		 */
		inline const std::wstring& name() const;

		/**
		 * Looks the mutex.
		 * Beware: This object must have a valid name before locking!<br>
		 * @see unlock(), tryLock(), name(), isValid().
		 */
		void lock();

		/**
		 * Tries to lock the mutex.
		 * Beware: This object must have a valid name before locking!<br>
		 * @return True, if the mutex could be locked
		 * @see unlock(), lock(), name(), isValid().
		 */
		bool tryLock();

		/**
		 * Unlocks the mutex.
		 * Beware: This object must have a valid name before locking!<br>
		 * @see lock(), name(), isVaid().
		 */
		void unlock();

		/**
		 * Returns whether this object is valid and can be used correctly.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this object is valid and can be used correctly.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Disabled assign operator.
		 * @param sharedLock Shared lock object to be assigned
		 * @return Reference to this object
		 */
		SharedLock& operator=(const SharedLock& sharedLock) = delete;

	private:

#if defined(__APPLE__) || (defined(__linux__) && !defined(_ANDROID))

		/**
		 * Creates the semaphore object of this lock.
		 * @return True, if succeeded
		 */
		bool createSemaphore();

#endif

	private:

		/// System wide unique name.
		std::wstring name_;

		/// Local lock object.
		Lock localLock_;

		/// Local lock counter.
		unsigned int localCounter_ = 0u;

		/// Handle of the mutex.
		void* handle_ = nullptr;
};

/**
 * This class defines a scoped lock object for shared locks.
 * @see SharedLock.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT ScopedSharedLock
{
	public:

		/**
		 * Creates a new shared scoped lock object.
		 * @param sharedLock Shared lock object to be used for locking
		 */
		inline ScopedSharedLock(SharedLock& sharedLock);

		/**
		 * Releases a shared scoped lock object.
		 */
		inline ~ScopedSharedLock();

	protected:

		/**
		 * Disabled copy constructor.
		 * @param scopedSharedLock The object which would be copied
		 */
		ScopedSharedLock(const ScopedSharedLock& scopedSharedLock) = delete;

		/**
		 * Disabled copy operator.
		 * @param scopedSharedLock The object which would be copied
		 * @return Reference to this object
		 */
		ScopedSharedLock& operator=(const ScopedSharedLock& scopedSharedLock) = delete;

	private:

		/// Shared lock object to be used for locking.
		SharedLock& scopedLock_;
};

inline const std::wstring& SharedLock::name() const
{
	return name_;
}

inline bool SharedLock::isValid() const
{
	return !name_.empty();
}

inline SharedLock::operator bool() const
{
	return !name_.empty();
}

inline ScopedSharedLock::ScopedSharedLock(SharedLock& sharedLock) :
	scopedLock_(sharedLock)
{
	scopedLock_.lock();
}

inline ScopedSharedLock::~ScopedSharedLock()
{
	scopedLock_.unlock();
}

}

#endif // META_OCEAN_BASE_LOCK_H
