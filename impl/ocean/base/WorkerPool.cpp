// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/base/WorkerPool.h"
#include "ocean/base/Processor.h"

namespace Ocean
{

WorkerPool::WorkerPool() :
	poolCapacity(2u)
{
	// nothing to do here
}

WorkerPool::~WorkerPool()
{
	const ScopedLock scopedLock(poolLock);

	for (size_t n = 0; n < poolFreeWorkers.size(); ++n)
	{
		ocean_assert(poolFreeWorkers[n] != NULL);
		delete poolFreeWorkers[n];
	}

	ocean_assert(poolUsedWorkers.empty());
	for (size_t n = 0; n < poolUsedWorkers.size(); ++n)
	{
		ocean_assert(poolUsedWorkers[n] != NULL);
		delete poolUsedWorkers[n];
	}

	poolFreeWorkers.clear();
	poolUsedWorkers.clear();
}

bool WorkerPool::setCapacity(const unsigned int workers)
{
	const ScopedLock scopedLock(poolLock);

	if (workers >= poolCapacity)
	{
		poolCapacity = workers;
		return true;
	}

	return false;
}

WorkerPool::ScopedWorker WorkerPool::scopedWorker()
{
	return ScopedWorker(lock());
}

Worker* WorkerPool::lock()
{
	const ScopedLock scopedLock(poolLock);

	if (poolFreeWorkers.empty() && poolUsedWorkers.empty())
	{
		const unsigned int cores = Processor::get().cores();

		if (cores == 1u)
			return NULL;
	}

	// try to find an unused worker
	if (!poolFreeWorkers.empty())
	{
		Worker* worker = poolFreeWorkers.back();
		poolFreeWorkers.popBack();

		ocean_assert(poolUsedWorkers.size() < poolUsedWorkers.capacity());
		poolUsedWorkers.pushBack(worker);

		return worker;
	}

	// if all workers are in use, but there is still capacity for a new worker
	if (poolFreeWorkers.size() + poolUsedWorkers.size() < poolCapacity)
	{
		Worker* worker = new Worker();

		ocean_assert(poolUsedWorkers.size() < poolUsedWorkers.capacity());
		poolUsedWorkers.pushBack(worker);

		ocean_assert(poolFreeWorkers.empty());

		return worker;
	}

	return NULL;
}

void WorkerPool::unlock(Worker* worker)
{
	if (worker)
	{
		const ScopedLock scopedLock(poolLock);

		for (size_t n = 0; n < min(size_t(20), poolUsedWorkers.size()); ++n)
		{
			if (poolUsedWorkers[n] == worker)
			{
				poolUsedWorkers.unstableErase(n);

				ocean_assert(poolFreeWorkers.size() < poolFreeWorkers.capacity());
				poolFreeWorkers.pushBack(worker);

				return;
			}
		}

		ocean_assert(false && "Invalid worker to unlock");
	}
}

}
