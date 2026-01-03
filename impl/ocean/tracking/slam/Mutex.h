/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_MUTEX_H
#define META_OCEAN_TRACKING_SLAM_MUTEX_H

#include "ocean/tracking/slam/SLAM.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Utilities.h"

#include <mutex>
#include <shared_mutex>

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * Definition of a mutex supporting read and write locks.
 * The mutex is not recursive.
 *
 * This mutex allows multiple concurrent read locks (shared access) but only one write lock (exclusive access).
 * Use ReadLock for read-only operations and WriteLock when modifying shared data.
 *
 * Example usage:
 * @code
 * class SharedResource
 * {
 *     public:
 *
 *         // Read operation: multiple threads can read simultaneously
 *         int getValue() const
 *         {
 *             const ReadLock readLock(mutex_);
 *
 *             return value_;
 *         }
 *
 *         // Write operation: exclusive access, blocks all readers and writers
 *         void setValue(int newValue)
 *         {
 *             const WriteLock writeLock(mutex_);
 *
 *             value_ = newValue;
 *         }
 *
 *         // Named locks for timing diagnostics (when LockManager::isEnabled_ is true)
 *         void updateWithTiming(int newValue)
 *         {
 *             const WriteLock writeLock(mutex_, "SharedResource::updateWithTiming");
 *
 *             value_ = newValue;
 *         }
 *
 *     protected:
 *
 *         /// The value of the shared resource.
 *         int value_ = 0;
 *
 *         /// The mutex of the shared resource.
 *         mutable Mutex mutex_;
 * };
 * @endcode
 * @ingroup trackingslam
 */
using Mutex = std::shared_mutex;

/**
 * This class implements a singleton manager for collecting and reporting lock timing measurements.
 * The isEnabled_ constant allows to activate or deactivate measurements in general.<br>
 * The manager can operate in two modes based on the threshold values:<br>
 * - If threshold > 0: Logs to console when acquisition delay or hold duration exceeds the threshold.<br>
 * - If threshold == 0: Collects all measurements for aggregate reporting via report().
 * In production code, measurements should be disabled as they add minor performance overhead.
 * @ingroup trackingslam
 */
class LockManager : public Singleton<LockManager>
{
	friend class Singleton<LockManager>;

	public:

		/// True, to enable lock timing measurements; False, to disable.
		static constexpr bool isEnabled_ = false;

		/// The threshold in milliseconds for acquisition delay logging, with range [0, infinity), 0 to collect measurements instead of logging.
		static constexpr double acquisitionDelayThresholdMs_ = 2.0;

		/// The threshold in milliseconds for hold duration logging, with range [0, infinity), 0 to collect measurements instead of logging.
		static constexpr double holdDurationThresholdMs_ = 2.0;

	protected:

		/**
		 * Definition of a key combining a lock type (read/write) and a name.
		 */
		using Key = std::pair<bool, std::string>;

		/**
		 * Definition of a vector holding duration measurements in seconds.
		 */
		using Measurements = std::vector<double>;

		/**
		 * Definition of a map mapping keys to measurements.
		 */
		using Map = std::unordered_map<Key, Measurements, PairHash>;

	public:

		/**
		 * Generates a report of all collected lock timing measurements.
		 * The report includes P99, P99.5, P99.9, and P100 (worst) statistics for each lock.
		 * @param skipZero True, to skip locks with measurements below 0.01ms; False, to include all locks
		 * @return The formatted report string, "Report is disabled" if isEnabled_ is false
		 */
		std::string report(const bool skipZero = true);

		/**
		 * Adds an acquisition delay measurement for a specific lock.
		 * @param name The name of the lock
		 * @param isWriteLock True, if this is a write lock; False, if this is a read lock
		 * @param duration The acquisition delay in seconds, with range [0, infinity)
		 */
		void addAcquisitionDelayMeasurement(const std::string& name, const bool isWriteLock, const double duration);

		/**
		 * Adds a hold duration measurement for a specific lock.
		 * @param name The name of the lock
		 * @param isWriteLock True, if this is a write lock; False, if this is a read lock
		 * @param duration The hold duration in seconds, with range [0, infinity)
		 */
		void addHoldDurationMeasurement(const std::string& name, const bool isWriteLock, const double duration);

	protected:

		/**
		 * Creates a new lock manager.
		 */
		LockManager();

	protected:

		/// The map storing acquisition delay measurements for each lock.
		Map acquisitionDelayMap_;

		/// The map storing hold duration measurements for each lock.
		Map holdDurationMap_;

		/// The lock protecting the maps.
		mutable Lock lock_;
};

/**
 * This class implements a helper for timing lock acquisition and hold durations.
 * The class is used as a base class for ReadLock and WriteLock to optionally measure lock performance.
 * @tparam tEnable True, to enable timing measurements; False, to disable (no-op implementation)
 * @ingroup trackingslam
 */
template <bool tEnable>
class LockTimer
{
	protected:

		/**
		 * Creates a new lock timer without a name.
		 */
		LockTimer() = default;

		/**
		 * Creates a new lock timer with a name.
		 * @param name The name of the lock for identification in reports, must be valid
		 */
		LockTimer(const char* name);

		/**
		 * Reports the acquisition delay to the LockManager.
		 * This function is called after the lock has been acquired.
		 * @param isWriteLock True, if this is a write lock; False, if this is a read lock
		 */
		void reportAcquisitionDelay(const bool isWriteLock);

		/**
		 * Reports the hold duration to the LockManager.
		 * This function is called before the lock is released.
		 * @param isWriteLock True, if this is a write lock; False, if this is a read lock
		 */
		void reportHoldDuration(const bool isWriteLock);

	protected:

		/// The name of the lock, nullptr if not specified.
		const char* name_ = nullptr;

		/// The timer for measuring acquisition delay and hold duration.
		HighPerformanceTimer timer_;
};

/**
 * Specialization of LockTimer for disabled timing.
 * This specialization provides no-op implementations for all methods to eliminate overhead when timing is disabled.
 * @ingroup trackingslam
 */
template <>
class LockTimer<false>
{
	protected:

		/**
		 * Creates a new lock timer without a name.
		 */
		LockTimer() = default;

		/**
		 * Creates a new lock timer with a name.
		 * @param name The name of the lock, unused in this specialization
		 */
		LockTimer(const char* /*name*/)
		{
			// nothing to do here
		}

		/**
		 * No-op implementation of acquisition delay reporting.
		 * @param isWriteLock Unused in this specialization
		 */
		void reportAcquisitionDelay(const bool /*isWriteLock*/)
		{
			// nothing to do here
		}

		/**
		 * No-op implementation of hold duration reporting.
		 * @param isWriteLock Unused in this specialization
		 */
		void reportHoldDuration(const bool /*isWriteLock*/)
		{
			// nothing to do here
		}
};

/**
 * This class implements a scoped read lock for a shared mutex.
 * The lock is acquired upon construction and released upon destruction.
 * Multiple read locks can be held simultaneously, but a read lock cannot be acquired while a write lock is held.
 * @ingroup trackingslam
 */
class ReadLock : public LockTimer<LockManager::isEnabled_>
{
	public:

		/**
		 * Creates a new read lock and acquires the mutex.
		 * @param mutex The mutex to lock
		 */
		explicit ReadLock(Mutex& mutex);

		/**
		 * Creates a new read lock with a name and acquires the mutex.
		 * @param mutex The mutex to lock
		 * @param name The name of the lock for identification in timing reports, must be valid
		 */
		ReadLock(Mutex& mutex, const char* name);

		/**
		 * Move constructor.
		 * @param readLock The read lock to move
		 */
		ReadLock(ReadLock&&) = default;

		/**
		 * Move assignment operator.
		 * @param readLock The read lock to move
		 * @return Reference to this object
		 */
		ReadLock& operator=(ReadLock&&) = default;

		/**
		 * Destructs the read lock and releases the mutex.
		 */
		~ReadLock();

		/**
		 * Explicitly releases the lock before the scoped object is destroyed.
		 */
		void unlock();

#ifdef OCEAN_DEBUG
		/**
		 * Returns whether a mutex is currently locked (debug only).
		 * This function attempts to acquire an exclusive lock to determine if the mutex is held.
		 * @param mutex The mutex to check
		 * @return True, if the mutex is locked; False, otherwise
		 */
		static bool debugIsLocked(Mutex& mutex);
#endif // OCEAN_DEBUG

	protected:

		/// The underlying shared lock.
		std::shared_lock<Mutex> lock_;
};

/**
 * This class implements a scoped write lock for a shared mutex.
 * The lock is acquired upon construction and released upon destruction.
 * A write lock provides exclusive access; no other read or write locks can be held simultaneously.
 * @ingroup trackingslam
 */
class WriteLock : public LockTimer<LockManager::isEnabled_>
{
	public:

		/**
		 * Creates a new write lock and acquires the mutex.
		 * @param mutex The mutex to lock
		 */
		explicit WriteLock(Mutex& mutex);

		/**
		 * Creates a new write lock with a name and acquires the mutex.
		 * @param mutex The mutex to lock
		 * @param name The name of the lock for identification in timing reports, must be valid
		 */
		explicit WriteLock(Mutex& mutex, const char* name);

		/**
		 * Move constructor.
		 * @param writeLock The write lock to move
		 */
		WriteLock(WriteLock&&) = default;

		/**
		 * Move assignment operator.
		 * @param writeLock The write lock to move
		 * @return Reference to this object
		 */
		WriteLock& operator=(WriteLock&&) = default;

		/**
		 * Destructs the write lock and releases the mutex.
		 */
		~WriteLock();

		/**
		 * Explicitly releases the lock before the scoped object is destroyed.
		 */
		void unlock();

#ifdef OCEAN_DEBUG
		/**
		 * Returns whether a mutex is currently locked (debug only).
		 * This function attempts to acquire an exclusive lock to determine if the mutex is held.
		 * @param mutex The mutex to check
		 * @return True, if the mutex is locked; False, otherwise
		 */
		static bool debugIsLocked(Mutex& mutex);
#endif // OCEAN_DEBUG

	protected:

		/// The underlying unique lock.
		std::unique_lock<Mutex> lock_;
};

template <bool tEnable>
LockTimer<tEnable>::LockTimer(const char* name) :
	name_(name)
{
	ocean_assert(name_ != nullptr);
}

template <bool tEnable>
void LockTimer<tEnable>::reportAcquisitionDelay(const bool isWriteLock)
{
	const double seconds = timer_.seconds();

	const char* name = name_ != nullptr ? name_ : "Unknown";

	if constexpr (LockManager::acquisitionDelayThresholdMs_ > 0.0)
	{
		const double milliseconds = seconds * 1000.0;

		if (milliseconds >= LockManager::acquisitionDelayThresholdMs_)
		{
			if (isWriteLock)
			{
				Log::warning() << "Write lock acquisition time: " << name << " took " << milliseconds << "ms";
			}
			else
			{
				Log::warning() << "Read lock acquisition time: " << name << " took " << milliseconds << "ms";
			}
		}
	}
	else
	{
		LockManager::get().addAcquisitionDelayMeasurement(name, isWriteLock, seconds);
	}

	timer_.start(); // re-start timer for hold duration measurement
}

template <bool tEnable>
void LockTimer<tEnable>::reportHoldDuration(const bool isWriteLock)
{
	const double seconds = timer_.seconds();

	const char* name = name_ != nullptr ? name_ : "Unknown";

	if constexpr (LockManager::holdDurationThresholdMs_ > 0.0)
	{
		const double milliseconds = seconds * 1000.0;

		if (milliseconds >= LockManager::holdDurationThresholdMs_)
		{
			if (isWriteLock)
			{
				Log::warning() << "Write lock hold duration time: " << name << " took " << milliseconds << "ms";
			}
			else
			{
				Log::warning() << "Read lock hold duration time: " << name << " took " << milliseconds << "ms";
			}
		}
	}
	else
	{
		LockManager::get().addHoldDurationMeasurement(name, isWriteLock, seconds);
	}
}

inline ReadLock::ReadLock(Mutex& mutex) :
	LockTimer(),
	lock_(mutex)
{
	reportAcquisitionDelay(false);
}

inline ReadLock::ReadLock(Mutex& mutex, const char* name) :
	LockTimer(name),
	lock_(mutex)
{
	reportAcquisitionDelay(false);
}

inline ReadLock::~ReadLock()
{
	if constexpr (LockManager::isEnabled_)
	{
		if (lock_.owns_lock())
		{
			reportHoldDuration(false);
		}
	}
}

inline void ReadLock::unlock()
{
	ocean_assert(lock_.owns_lock());

	if constexpr (LockManager::isEnabled_)
	{
		reportHoldDuration(false);
	}

	lock_.unlock();
}

#ifdef OCEAN_DEBUG
inline bool ReadLock::debugIsLocked(Mutex& mutex)
{
	if (!mutex.try_lock()) // not try_lock_shared() as this returns would always return true for a read lock
	{
		// we cannot acquire the lock, so it must be locked
		return true;
	}

	mutex.unlock();
	return false;
}
#endif // OCEAN_DEBUG

inline WriteLock::WriteLock(Mutex& mutex) :
	LockTimer(),
	lock_(mutex)
{
	reportAcquisitionDelay(true);
}

inline WriteLock::WriteLock(Mutex& mutex, const char* name) :
	LockTimer(name),
	lock_(mutex)
{
	reportAcquisitionDelay(true);
}

inline WriteLock::~WriteLock()
{
	if constexpr (LockManager::isEnabled_)
	{
		if (lock_.owns_lock())
		{
			reportHoldDuration(true);
		}
	}
}

inline void WriteLock::unlock()
{
	ocean_assert(lock_.owns_lock());

	if constexpr (LockManager::isEnabled_)
	{
		if (lock_.owns_lock())
		{
			reportHoldDuration(true);
		}
	}

	lock_.unlock();
}

#ifdef OCEAN_DEBUG
inline bool WriteLock::debugIsLocked(Mutex& mutex)
{
	if (!mutex.try_lock())
	{
		// we cannot acquire the lock, so it must be locked
		return true;
	}

	mutex.unlock();
	return false;
}
#endif // OCEAN_DEBUG

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_MUTEX_H
