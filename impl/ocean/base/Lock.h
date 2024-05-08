/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_LOCK_H
#define META_OCEAN_BASE_LOCK_H

#include "ocean/base/Base.h"

#if defined(_WINDOWS)
	#include <winsock2.h>
	#include <windows.h>
#else
	#include <pthread.h>
#endif

namespace Ocean
{

/**
 * This class implements a recursive lock object.
 * You can either explicitly lock and unlock an Lock object by using the appropriated functions.<br>
 * However, it's recommended to use the corresponding scope classes for this lock object.
 * @see TemplatedLock, ScopedLock, TemplatedScopedLock, TemporaryScopedLock, OptionalScopedLock.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Lock
{
	public:

		/**
		 * Creates a new lock object.
		 */
		inline Lock();

		/**
		 * Destructs a lock object.
		 */
		inline ~Lock();

		/**
		 * Locks the critical section.
		 */
		inline void lock();

		/**
		 * Unlocks the critical section.
		 */
		inline void unlock();

		/**
		 * Returns whether this critical section is locked by another thread.
		 * @return True, if so
		 */
		inline bool isLocked();

	protected:

		/**
		 * Disabled copy constructor.
		 * @param lock Lock object to be copied
		 */
		Lock(const Lock& lock) = delete;

		/**
		 * Disabled assign operator.
		 * @param lock Lock object to be assigned
		 * @return Reference to this object
		 */
		Lock& operator=(const Lock& lock) = delete;

	protected:

#if defined(_WINDOWS)

		/// Critical section object of windows.
		CRITICAL_SECTION criticalSection_;

#else

		/// Pthread mutex object.
		pthread_mutex_t mutex_;

#endif

};

/**
 * This class implements a template-based recursive lock object.
 * The class allows to e.g., implement classes which can be thread-safe or not based on a template parameter.
 * @tparam tActive True, to activate the lock; False, to avoid using the lock at all
 * @ingroup base
 */
template <bool tActive>
class TemplatedLock : public Lock
{
	// nothing to do here
};

/**
 * Specialization of the TemplatedLock class.
 * This class does nothing.
 * @ingroup base
 */
template <>
class TemplatedLock<false>
{
	protected:

		/**
		 * Disabled copy constructor.
		 * @param lock Lock object to be copied
		 */
		TemplatedLock(const TemplatedLock& lock) = delete;

		/**
		 * Disabled assign operator.
		 * @param lock Lock object to be assigned
		 * @return Reference to this object
		 */
		TemplatedLock& operator=(const TemplatedLock& lock) = delete;
};

/**
 * This class implements a scoped lock object for recursive lock objects.
 * The application of a scoped lock object ensures that a lock will be unlocked in any case if the scope (in which the object exists) ends.<br>
 * Thus, possible dead locks can be avoided (e.g., if a function returns while the before the unlock() function of the Lock object is invoke or if an exception is thrown but not intercepted).
 * @see Lock, TemplatedScopedLock, TemporaryScopedLock, OptionalScopedLock.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT ScopedLock
{
	public:

		/**
		 * Creates a new scoped lock object by a given lock object.
		 * @param lock Lock object used for locking
		 */
		explicit inline ScopedLock(Lock& lock);

		/**
		 * Destructs a scoped lock and unlocks the internal lock object.
		 */
		inline ~ScopedLock();

	protected:

		/**
		 * Disabled accessible copy operator.
		 * @param object Object to copy
		 */
		ScopedLock(const ScopedLock& object) = delete;

		/**
		 * Disabled accessible assigns operator.
		 * @param object The right object
		 * @return Reference to this object
		 */
		ScopedLock& operator=(const ScopedLock& object) = delete;

	protected:

		/// Lock object which is locked during the existence of this scoped lock object.
		Lock& lock_;
};

/**
 * This class implements a recursive scoped lock object that is activated by a boolean template parameter.
 * @tparam tActive True, to active the scoped lock, false to deactivate the scoped lock
 * @see Lock, ScopedLock, TemporaryScopedLock, OptionalScopedLock.
 * @ingroup base
 */
template <bool tActive>
class TemplatedScopedLock
{
	public:

		/**
		 * Creates a new scoped lock object by a given lock object.
		 * @param lock Lock object used for locking
		 */
		explicit inline TemplatedScopedLock(Lock& lock);

		/**
		 * Creates a new scoped lock object by a given lock object.
		 * @param lock Lock object used for locking
		 */
		explicit inline TemplatedScopedLock(TemplatedLock<tActive>& lock);

		/**
		 * Destructs a scoped lock and unlocks the internal lock object.
		 */
		inline ~TemplatedScopedLock();

	protected:

		/**
		 * Disabled accessible copy operator.
		 * @param object Object to copy
		 */
		TemplatedScopedLock(const TemplatedScopedLock<tActive>& object) = delete;

		/**
		 * Disabled accessible assigns operator.
		 * @param object The right object
		 * @return Reference to this object
		 */
		TemplatedScopedLock& operator=(const TemplatedScopedLock<tActive>& object) = delete;

	protected:

		/// Lock object which is locked during the existence of this scoped lock object.
		Lock& lock_;
};

/**
 * Specialization of the TemplatedScopedLock class.
 * This class does nothing.
 * @ingroup base
 */
template <>
class TemplatedScopedLock<false>
{
	public:

		/**
		 * Creates a new scoped lock object by a given lock object.
		 * @param lock Lock object used for locking
		 */
		explicit inline TemplatedScopedLock(Lock& lock)
		{
			OCEAN_SUPPRESS_UNUSED_WARNING(lock);
		}

		/**
		 * Creates a new scoped lock object by a given lock object.
		 * @param lock Lock object used for locking
		 */
		explicit inline TemplatedScopedLock(TemplatedLock<false>& lock)
		{
			OCEAN_SUPPRESS_UNUSED_WARNING(lock);
		}
};

/**
 * This class implements a recursive scoped lock object allowing to release the lock before the scoped object itself is released.
 * @see Lock, ScopedLock, TemplatedScopedLock, OptionalScopedLock.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT TemporaryScopedLock
{
	public:

		/**
		 * Creates a new scoped lock object which is not locked yet.
		 */
		inline TemporaryScopedLock();

		/**
		 * Creates a new scoped lock object by a given lock object.
		 * @param lock Lock object used for locking
		 */
		explicit inline TemporaryScopedLock(Lock& lock);

		/**
		 * Destructs a scoped lock and unlocks the internal lock object (if still locked).
		 */
		inline ~TemporaryScopedLock();

		/**
		 * Explicitly releases the lock before the scoped lock object is released.
		 */
		inline void release();

		/**
		 * Re-locks this scoped lock with a given lock.
		 * This scoped lock must be released before re-locking it again.
		 * @param lock The lock to be used for locking
		 * @see isReleased().
		 */
		inline void relock(Lock& lock);

		/**
		 * Returns whether this scoped lock is released already.
		 * @return True, if so
		 */
		inline bool isReleased() const;

	protected:

		/**
		 * Disabled accessible copy operator.
		 * @param object Object to copy
		 */
		inline TemporaryScopedLock(const TemporaryScopedLock& object) = delete;

		/**
		 * Disabled accessible assigns operator.
		 * @param object The right object
		 * @return Reference to this object
		 */
		inline TemporaryScopedLock& operator=(const TemporaryScopedLock& object) = delete;

	protected:

		/// Lock object which is locked during the existence of this scoped lock object.
		Lock* lock_;
};

/**
 * This class implements an optional recursive scoped lock object locking the lock object only if it's defined.
 * @see Lock, ScopedLock, TemplatedScopedLock, TemporaryScopedLock.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT OptionalScopedLock
{
	public:

		/**
		 * Creates a new optional scoped lock object by a given lock object.
		 * @param lock Optional lock object used for locking, otherwise nullptr
		 */
		explicit inline OptionalScopedLock(Lock* lock);

		/**
		 * Creates a new optional scoped lock object by a given lock object and a boolean statement whether the lock is invoked or not.
		 * @param lock Lock object used for locking
		 * @param apply True, to invoke the lock; False, to avoid the locking
		 */
		inline OptionalScopedLock(Lock& lock, const bool apply);

		/**
		 * Destructs an optional scoped lock and unlocks the internal lock object if defined.
		 */
		inline ~OptionalScopedLock();

	protected:

		/**
		 * Disabled accessible copy operator.
		 * @param object Object to copy
		 */
		inline OptionalScopedLock(const OptionalScopedLock& object) = delete;

		/**
		 * Disabled accessible assigns operator.
		 * @param object The right object
		 * @return Reference to this object
		 */
		inline OptionalScopedLock& operator=(const OptionalScopedLock& object) = delete;

	protected:

		/// Lock object which is locked during the existence of this scoped lock object.
		Lock* lock_;
};

inline Lock::Lock()
{
#if defined(_WINDOWS)

	InitializeCriticalSection(&criticalSection_);

#elif defined(__APPLE__) || defined(__linux__) || defined(__EMSCRIPTEN__)

	pthread_mutexattr_t mutexAttribute;
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_settype(&mutexAttribute, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex_, &mutexAttribute);

#else

	pthread_mutexattr_t mutexAttribute = PTHREAD_MUTEX_RECURSIVE;
	pthread_mutex_init(&mutex_, &mutexAttribute);

#endif

}

inline Lock::~Lock()
{

#if defined(_WINDOWS)

	ocean_assert(criticalSection_.RecursionCount == 0);
	DeleteCriticalSection(&criticalSection_);

#else

	pthread_mutex_destroy(&mutex_);

#endif

}

inline void Lock::lock()
{
#if defined(_WINDOWS)

	EnterCriticalSection(&criticalSection_);

#else

	pthread_mutex_lock(&mutex_);

#endif

}

inline void Lock::unlock()
{

#if defined(_WINDOWS)

	LeaveCriticalSection(&criticalSection_);

#else

	pthread_mutex_unlock(&mutex_);

#endif

}

inline bool Lock::isLocked()
{
#if defined(_WINDOWS)

	if (TryEnterCriticalSection(&criticalSection_) == TRUE)
	{
		LeaveCriticalSection(&criticalSection_);
		return false;
	}

#else

	if (pthread_mutex_trylock(&mutex_) == 0)
	{
		pthread_mutex_unlock(&mutex_);
		return false;
	}

#endif

	return true;
}

inline ScopedLock::ScopedLock(Lock& lock) :
	lock_(lock)
{
	lock_.lock();
}

inline ScopedLock::~ScopedLock()
{
	lock_.unlock();
}

template <bool tActive>
inline TemplatedScopedLock<tActive>::TemplatedScopedLock(Lock& lock) :
	lock_(lock)
{
	lock_.lock();
}

template <bool tActive>
inline TemplatedScopedLock<tActive>::TemplatedScopedLock(TemplatedLock<tActive>& lock) :
	lock_(lock)
{
	lock.lock();
}

template <bool tActive>
inline TemplatedScopedLock<tActive>::~TemplatedScopedLock()
{
	lock_.unlock();
}

inline TemporaryScopedLock::TemporaryScopedLock() :
	lock_(nullptr)
{
	// nothing to do here
}

inline TemporaryScopedLock::TemporaryScopedLock(Lock& lock) :
	lock_(&lock)
{
	ocean_assert(lock_ != nullptr);
	lock_->lock();
}

inline TemporaryScopedLock::~TemporaryScopedLock()
{
	if (lock_ != nullptr)
	{
		lock_->unlock();
	}
}

inline void TemporaryScopedLock::release()
{
	ocean_assert(!isReleased() && "This TemporaryScopedLock object has been released before");

	if (lock_)
	{
		lock_->unlock();
		lock_ = nullptr;
	}
}

inline void TemporaryScopedLock::relock(Lock& lock)
{
	ocean_assert(isReleased() && "This TemporaryScopedLock object must be released before");

	if (lock_ == nullptr)
	{
		lock_ = &lock;
		lock_->lock();
	}
}

inline bool TemporaryScopedLock::isReleased() const
{
	return lock_ == nullptr;
}

inline OptionalScopedLock::OptionalScopedLock(Lock* lock) :
	lock_(lock)
{
	if (lock_)
	{
		lock_->lock();
	}
}

inline OptionalScopedLock::OptionalScopedLock(Lock& lock, const bool apply) :
	lock_(nullptr)
{
	if (apply)
	{
		lock_ = &lock;
		lock_->lock();
	}
}

inline OptionalScopedLock::~OptionalScopedLock()
{
	if (lock_)
	{
		lock_->unlock();
	}
}

}

#endif // META_OCEAN_BASE_LOCK_H
