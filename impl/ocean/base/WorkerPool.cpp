/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/WorkerPool.h"
#include "ocean/base/Processor.h"

namespace Ocean
{

WorkerPool::~WorkerPool()
{
	freeWorkers_.clear();
	usedWorkers_.clear();
}

bool WorkerPool::setCapacity(const size_t workers)
{
	const ScopedLock scopedLock(lock_);

	if (workers >= capacity_)
	{
		capacity_ = workers;

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
	const ScopedLock scopedLock(lock_);

	if (freeWorkers_.empty() && usedWorkers_.empty())
	{
		const unsigned int cores = Processor::get().cores();

		if (cores == 1u)
		{
			return nullptr;
		}
	}

	// try to find an unused worker
	if (!freeWorkers_.empty())
	{
		UniqueWorker worker = std::move(freeWorkers_.back());
		freeWorkers_.popBack();

		ocean_assert(usedWorkers_.size() < usedWorkers_.capacity());
		usedWorkers_.pushBack(std::move(worker));

		return usedWorkers_.back().get();
	}

	// if all workers are in use, but there is still capacity for a new worker
	if (freeWorkers_.size() + usedWorkers_.size() < capacity_)
	{
		UniqueWorker worker = std::make_unique<Worker>();

		ocean_assert(usedWorkers_.size() < usedWorkers_.capacity());
		usedWorkers_.pushBack(std::move(worker));

		ocean_assert(freeWorkers_.empty());

		return usedWorkers_.back().get();
	}

	return nullptr;
}

void WorkerPool::unlock(Worker* worker)
{
	if (worker != nullptr)
	{
		const ScopedLock scopedLock(lock_);

		for (size_t n = 0; n < min(usedWorkers_.size(), size_t(20)); ++n)
		{
			if (usedWorkers_[n].get() == worker)
			{
				UniqueWorker usedWorker = std::move(usedWorkers_[n]);
				usedWorkers_[n] = std::move(usedWorkers_.back());
				usedWorkers_.popBack();

				ocean_assert(freeWorkers_.size() < freeWorkers_.capacity());
				freeWorkers_.pushBack(std::move(usedWorker));

				return;
			}
		}

		ocean_assert(false && "Invalid worker to unlock");
	}
}

}
