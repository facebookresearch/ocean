/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_THREAD_POOL_H
#define META_OCEAN_BASE_THREAD_POOL_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Signal.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

#include <functional>
#include <queue>

namespace Ocean
{

/**
 * This class implements a pool holding re-usable thread objects for individual use.
 * This class is not a singleton, use ThreadPoolSingleton in case a singleton is sufficient.
 * @see ThreadPoolSingleton, Scheduler, TaskQueue.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT ThreadPool : protected Thread
{
	public:

		/**
		 * Definition of a caller function without return parameter.
		 */
		using Function = std::function<void()>;

	protected:

		/**
		 * This class implements one thread of this thread pool.
		 */
		class OCEAN_BASE_EXPORT PoolThread : protected Thread
		{
			public:

				/**
				 * Creates a new thread object.
				 * @param name The thread name
				 */
				explicit PoolThread(const std::string& name);

				/**
				 * Destructs a thread object.
				 */
				~PoolThread() override;

				/**
				 * Returns whether this thread is currently busy.
				 * @return True, if so
				 */
				inline bool isBusy() const;

				/**
				 * Sets the thread function.
				 * @param function The thread function that will be invoked in this thread
				 */
				void invoke(Function&& function);

			protected:

				/**
				 * Thread run function.
				 */
				void threadRun() override;

			protected:

				/// Internal signal handling the internal thread execution.
				Signal signal_;

				/// The function that is invoked in this thread.
				Function function_;

				/// Thread lock.
				mutable Lock lock_;
		};

		/**
		 * Definition of a unique pointer holding a pool thread.
		 */
		using UniquePoolThread = std::unique_ptr<PoolThread>;

		/**
		 * Definition of a vector holding pool threads.
		 */
		using PoolThreads = std::vector<UniquePoolThread>;

		/**
		 * Definition of a queue holding functions.
		 */
		using FunctionQueue = std::queue<Function>;

	public:

		/**
		 * Creates a new thread pool object.
		 */
		ThreadPool() = default;

		/**
		 * Destructs a thread pool.
		 */
		~ThreadPool() override;

		/**
		 * Returns the maximal number of thread objects allowed inside this pool.
		 * @return Maximal thread capacity, with range [1, infinity); 2 by default
		 */
		inline size_t capacity() const;

		/**
		 * Returns the number of currently busy/active threads in this pool.
		 * @return Thread number, with range [0, infinity)
		 */
		inline size_t size() const;

		/**
		 * Defines the maximal number of thread objects existing concurrently.
		 * @param capacity Maximal number of thread objects to be allowed inside this pool, with range (1, infinity)
		 * @return True, if succeeded
		 */
		bool setCapacity(const size_t capacity);

		/**
		 * Invokes a function on one of the free threads of this pool.
		 * @param function The function that will be invoked by a free thread
		 * @return True, if the function will be invoked by a free thread, otherwise no free thread was available
		 */
		bool invoke(Function&& function);

		/**
		 * Returns the number of pending functions.
		 * @return The number of pending functions which have not been executed yet as the pool is at capacity, with range [0, infinity)
		 */
		inline size_t pending() const;

	protected:

		/**
		 * Thread run function.
		 */
		void threadRun() override;

	protected:

		/// The busy pool threads.
		PoolThreads busyPoolThreads_;

		/// The idle pool threads.
		PoolThreads idlePoolThreads_;

		/// The pending functions.
		FunctionQueue pendingFunctions_;

		/// Maximal pool capacity, with range [1, infinity)
		size_t capacity_ = 2;

		/// The counter for pool thread ids.
		size_t poolThreadIdCounter_ = 0;

		/// Pool lock.
		mutable Lock lock_;
};

/**
 * This class wraps the ThreadPool into a singleton for global usage.
 * Use the standard ThreadPool in case a singleton is not sufficient.
 * @see ThreadPool.
 * @ingroup base
 */
class ThreadPoolSingleton :
	public Singleton<ThreadPoolSingleton>,
	public ThreadPool
{
	friend class Singleton<ThreadPoolSingleton>;

	protected:

		/**
		 * Protected default constructor.
		 */
		ThreadPoolSingleton();
};

inline bool ThreadPool::PoolThread::isBusy() const
{
	const ScopedLock scopedLock(lock_);

	return function_ != nullptr;
}

inline size_t ThreadPool::capacity() const
{
	const ScopedLock scopedLock(lock_);
	return capacity_;
}

inline size_t ThreadPool::size() const
{
	const ScopedLock scopedLock(lock_);

	return busyPoolThreads_.size();
}

inline size_t ThreadPool::pending() const
{
	const ScopedLock scopedLock(lock_);

	return pendingFunctions_.size();
}

}

#endif // META_OCEAN_BASE_THREAD_POOL_H
