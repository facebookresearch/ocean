/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/ThreadPool.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"

namespace Ocean
{

ThreadPool::PoolThread::PoolThread(const std::string& name) :
	Thread(name)
{
	startThread();
}

ThreadPool::PoolThread::~PoolThread()
{
	// we inform this thread that is should stop
	stopThread();

	// we release the signal so that the waiting signal will not wait anymore
	signal_.release();

	stopThreadExplicitly();
}

void ThreadPool::PoolThread::invoke(Function&& function)
{
	ocean_assert(function);

	TemporaryScopedLock scopedLock(lock_);

		ocean_assert(!function_);
		function_ = std::move(function);

	scopedLock.release();

	signal_.pulse();
}

void ThreadPool::PoolThread::threadRun()
{
	while (shouldThreadStop() == false)
	{
		signal_.wait();

		if (shouldThreadStop() == false)
		{
			ocean_assert(function_);

			if (function_)
			{
				function_();
			}

			const ScopedLock scopedLock(lock_);

			function_ = nullptr;
		}
	}
}

ThreadPool::~ThreadPool()
{
	const ScopedLock scopedLock(lock_);

	pendingFunctions_ = FunctionQueue();

	stopThread();

	stopThreadExplicitly();
}

bool ThreadPool::setCapacity(const size_t capacity)
{
	if (capacity == 0)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	capacity_ = capacity;

	return true;
}

bool ThreadPool::invoke(Function&& function)
{
	ocean_assert(function);
	if (!function)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (!pendingFunctions_.empty())
	{
		// at least one function is already waiting in the queue

		pendingFunctions_.push(std::move(function));

		return true;
	}

	if (!idlePoolThreads_.empty())
	{
		// we have an idle pool thread

		UniquePoolThread poolThread = std::move(idlePoolThreads_.back());
		ocean_assert(poolThread != nullptr);

		idlePoolThreads_.pop_back();


		poolThread->invoke(std::move(function));

		busyPoolThreads_.emplace_back(std::move(poolThread));

		return true;
	}

	if (busyPoolThreads_.size() + idlePoolThreads_.size() < capacity_)
	{
		// we don't have an idle pool thread, but we also did not yet reach the maximal pool capacity

		if (poolThreadIdCounter_ == 0)
		{
			// we need to start the pool's maintenance thread
			startThread();
		}

		UniquePoolThread poolThread = std::make_unique<PoolThread>("Pool Thread" + String::toAString(poolThreadIdCounter_++));

		poolThread->invoke(std::move(function));

		busyPoolThreads_.emplace_back(std::move(poolThread));

		return true;
	}

	// the pool is at capacity, so we need to defer the execution

	pendingFunctions_.push(std::move(function));

	return true;
}

void ThreadPool::threadRun()
{
	while (!shouldThreadStop())
	{
		TemporaryScopedLock scopedLock(lock_);

			for (size_t n = 0; n < busyPoolThreads_.size(); /*noop*/)
			{
				// let's check whether one of the busy threads is not busy anymore

				if (!busyPoolThreads_[n]->isBusy())
				{
					idlePoolThreads_.emplace_back(std::move(busyPoolThreads_[n]));

					busyPoolThreads_[n] = std::move(busyPoolThreads_.back());
					busyPoolThreads_.pop_back();
				}
				else
				{
					++n;
				}
			}

			while (capacity_ < busyPoolThreads_.size() + idlePoolThreads_.size() && idlePoolThreads_.empty())
			{
				// we have an idle pool thread and we have more threads than the specified capacity (e.g., because the capacity has been reduced lately)

				idlePoolThreads_.pop_back();
			}

			while (!pendingFunctions_.empty())
			{
				// let's check whether we have an idle thread for the next pending function

				if (idlePoolThreads_.empty())
				{
					break;
				}

				UniquePoolThread poolThread = std::move(idlePoolThreads_.back());
				idlePoolThreads_.pop_back();

				poolThread->invoke(std::move(pendingFunctions_.front()));
				pendingFunctions_.pop();

				busyPoolThreads_.emplace_back(std::move(poolThread));
			}

		scopedLock.release();

		sleep(5u);
	}
}

ThreadPoolSingleton::ThreadPoolSingleton()
{
	// nothing to do here, however cannot be declared as default
}

}
