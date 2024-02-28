// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_BASE_WORKER_POOL_H
#define META_OCEAN_BASE_WORKER_POOL_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/StaticVector.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

/**
 * This class implements a pool holding worker objects for individual use.
 * @see Worker.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT WorkerPool : public Singleton<WorkerPool>
{
	friend class Singleton<WorkerPool>;
	friend class ScopedWorker;

	private:

		/**
		 * Definition of a static vector holding worker objects.
		 */
		typedef StaticVector<Worker*, 10> Workers;

	public:

		/**
		 * Definition of scoped worker object.
		 */
		class OCEAN_BASE_EXPORT ScopedWorker
		{
			public:

				/**
				 * Creates an empty scoped worker object.
				 */
				inline ScopedWorker();

				/**
				 * Move constructor.
				 * @param object The object to move
				 */
				inline ScopedWorker(ScopedWorker&& object);

				/**
				 * Creates a new scoped worker object.
				 * @param worker The worker object of this scoped object.
				 */
				inline explicit ScopedWorker(Worker* worker);

				/**
				 * Destructs a scoped worker object and unlocks the internal worker object automatically.
				 */
				inline ~ScopedWorker();

				/**
				 * Returns the internal worker object.
				 * Beware: Do not store this object outside this scoped object.
				 * The internal object will exist as long this scoped objects exist.
				 * @return Worker object of this scoped object
				 */
				inline Worker* worker() const;

				/**
				 * Explicitly releases the object and does not wait until the scope ends.
				 * The internal worker object will be returned to the worker pool and cannot be accessed anymore by this object.
				 */
				inline void release();

				/**
				 * Move a scoped worker object.
				 * @param object Object to move
				 */
				inline ScopedWorker& operator=(ScopedWorker&& object);

				/**
				 * Returns the internal worker object.
				 * Beware: Do not store this object outside this scoped object.
				 * The internal object will exist as long this scoped objects exist.
				 * @return Worker object of this scoped object
				 */
				inline Worker* operator()() const;

				/**
				 * Returns whether this scoped objects holds an internal worker object.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			private:

				/**
				 * Disabled copy constructor.
				 * @param scopedWorker Object which would be copied
				 */
				ScopedWorker(const ScopedWorker& scopedWorker) = delete;

				/**
				 * Disabled copy operator.
				 * @param scopedWorker Object which would be copied
				 * @return Reference to this object
				 */
				ScopedWorker& operator=(const ScopedWorker& scopedWorker) = delete;

			private:

				/// Internal worker object.
				Worker* scopedWorker;
		};

	public:

		/**
		 * Returns the maximal number of worker objects allowed inside this pool.
		 * @return Maximal worker capacity, with range [1, 10], 2 by default
		 */
		inline unsigned int capacity();

		/**
		 * Returns the number of currently existing worker objects in this pool.
		 * @return Worker count, with range [0, capacity()]
		 */
		inline unsigned int size();

		/**
		 * Defines the maximal number of worker objects existing concurrently.
		 * @param workers Maximal number of worker objects to be allowed inside this pool, with range [capacity(), 10]
		 * @return True, if succeeded
		 */
		bool setCapacity(const unsigned int workers);

		/**
		 * Returns a scoped object holding the real worker if available.
		 * The scoped object guarantees the existence of the real worker (if available in the moment this function is called) as long as the scoped object exists.
		 * @return Object holding the worker if available, otherwise the object will be empty
		 */
		ScopedWorker scopedWorker();

		/**
		 * Returns a scoped object holding the real worker if a given condition is 'True' and if a worker is available.
		 * The scoped object guarantees the existence of the real worker (if provided in the moment this function is called) as long as the scoped object exists.<br>
		 * This function allows to apply something like:
		 * @code
		 * // we invoke the following function in a single-core manner for small images (< 400 * 400 pixels),
		 * // and in a multi-core manner for large images
		 * invokeMulticoreFunction(frame, WorkerPool::get().conditionalScopedWorker(frame.pixels() >= 400u * 400u)());
		 * @endcode
		 * @param condition True, to get a scoped worker with real worker object (if available); False, to get an empty scoped worker
		 * @return Object holding the worker if available, otherwise the object will be empty
		 */
		inline ScopedWorker conditionalScopedWorker(const bool condition);

	private:

		/**
		 * Creates a new worker pool and initializes the maximal worker capacity to 2.
		 */
		WorkerPool();

		/**
		 * Destructs a worker pool.
		 */
		~WorkerPool();

		/**
		 * Tries to lock a worker to be used for individual worker.
		 * Beware: This worker object must be unlocked after usage.
		 * @return Worker object if available, otherwise NULL
		 * @see unlock().
		 */
		Worker* lock();

		/**
		 * Unlocks a previously locked worker object to make it available for other users.
		 * Beware: To not use an unlocked worker object anymore.
		 * @param worker The worker to be unlocked
		 */
		void unlock(Worker* worker);

	private:

		/// Vector holding the currently not-used worker objects.
		Workers poolFreeWorkers;

		/// Vector holding the currently used worker objects.
		Workers poolUsedWorkers;

		/// Maximal pool capacity, with range [1, infinity)
		unsigned int poolCapacity;

		/// Lock for the entire pool.
		Lock poolLock;
};

WorkerPool::ScopedWorker::ScopedWorker() :
	scopedWorker(NULL)
{
	// nothing to do here
}

inline WorkerPool::ScopedWorker::ScopedWorker(ScopedWorker&& object) :
	scopedWorker(object.scopedWorker)
{
	object.scopedWorker = NULL;
}

inline WorkerPool::ScopedWorker::ScopedWorker(Worker* worker) :
	scopedWorker(worker)
{
	// nothing to do here
}

inline WorkerPool::ScopedWorker::~ScopedWorker()
{
	release();
}

inline Worker* WorkerPool::ScopedWorker::worker() const
{
	return scopedWorker;
}

inline void WorkerPool::ScopedWorker::release()
{
	if (scopedWorker)
	{
		WorkerPool::get().unlock(scopedWorker);
		scopedWorker = NULL;
	}
}

inline WorkerPool::ScopedWorker& WorkerPool::ScopedWorker::operator=(ScopedWorker&& object)
{
	if (this != &object)
	{
		release();

		scopedWorker = object.scopedWorker;
		object.scopedWorker = NULL;
	}

	return *this;
}

inline Worker* WorkerPool::ScopedWorker::operator()() const
{
	return scopedWorker;
}

inline WorkerPool::ScopedWorker::operator bool() const
{
	return scopedWorker != NULL;
}

inline unsigned int WorkerPool::capacity()
{
	const ScopedLock scopedLock(poolLock);
	return poolCapacity;
}

inline unsigned int WorkerPool::size()
{
	const ScopedLock scopedLock(poolLock);
	return (unsigned int)(poolUsedWorkers.size() + poolFreeWorkers.size());
}

inline WorkerPool::ScopedWorker WorkerPool::conditionalScopedWorker(const bool condition)
{
	if (condition)
	{
		return scopedWorker();
	}
	else
	{
		return ScopedWorker();
	}
}

}

#endif // META_OCEAN_BASE_WORKER_POOL_H
