/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Worker.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include <ctime>

namespace Ocean
{

Worker::WorkerThread::WorkerThread(const unsigned int workerSeedValue, const unsigned int workerThreadId) :
	Thread(workerSeedValue + workerThreadId, std::string("Worker thread ") + String::toAString(workerThreadId)),
	id_(workerThreadId)
{
	// nothing to do here
}

Worker::WorkerThread::~WorkerThread()
{
	stopThread();
	internalSignal_.release();
	joinThread();

	stopThreadExplicitly();
}

bool Worker::WorkerThread::start(Signal& signal)
{
	if (externalSignal_ != nullptr)
	{
		return false;
	}

	externalSignal_ = &signal;

	return startThread();
}

void Worker::WorkerThread::threadRun()
{
	ocean_assert(externalSignal_ != nullptr);

	while (shouldThreadStop() == false)
	{
		internalSignal_.wait();

		if (function_)
		{
			ocean_assert(!abortableFunction_);
			ocean_assert(abortIndex_ == (unsigned int)(-1));

#ifdef OCEAN_SUPPORT_EXCEPTIONS

			// we need a try-and-catch block as an uncaught exception would create a dead-lock as the signal would never be pulsed
			try
			{
				function_();
			}
			catch (const std::exception& exception)
			{
				ocean_assert(false && "Unhandled exception in worker!");
				Log::error() << "Unhandled exception in worker, reason: " << exception.what();
			}
			catch (...)
			{
				ocean_assert(false && "Unhandled exception in worker!");
				Log::error() << "Unhandled exception in worker!";
			}

#else

			function_();

#endif // OCEAN_SUPPORT_EXCEPTIONS

			workerState_ = WS_UNKNOWN_RESULT;
			function_ = Function();

			externalSignal_->pulse();
		}
		else if (abortableFunction_)
		{
			ocean_assert(!function_);
			ocean_assert(abortIndex_ != (unsigned int)(-1));

#ifdef OCEAN_SUPPORT_EXCEPTIONS

			// we need a try-and-catch block as an uncaught exception would create a dead-lock as the signal would never be pulsed
			try
			{

#endif // OCEAN_SUPPORT_EXCEPTIONS

				if (abortableFunction_())
				{
					workerState_ = WS_POSITIVE_RESULT;

					bool* abortState = abortableFunction_.parameter<bool*>(abortIndex_);
					*abortState = true;
				}
				else
				{
					workerState_ = WS_NEGATIVE_RESULT;
				}

#ifdef OCEAN_SUPPORT_EXCEPTIONS

			}
			catch (const std::exception& exception)
			{
				ocean_assert(false && "Unhandled exception in worker!");
				Log::error() << "Unhandled exception in worker, reason: " << exception.what();
			}
			catch (...)
			{
				ocean_assert(false && "Unhandled exception in worker!");
				Log::error() << "Unhandled exception in worker!";
			}

#endif // OCEAN_SUPPORT_EXCEPTIONS

			abortableFunction_ = AbortableFunction();
			abortIndex_ = (unsigned int)(-1);

			externalSignal_->pulse();
		}
	}
}

Worker::Worker(const LoadType loadType, const unsigned int maximalNumberCores)
{
	ocean_assert(loadType != TYPE_CUSTOM);
	ocean_assert(maximalNumberCores >= 1u);

	unsigned int processors = max(1u, Processor::get().cores());
	unsigned int cores = 1u;

	switch (loadType)
	{
		case TYPE_ONE_CORE:
			break;

		case TYPE_HALF_CORES:
			cores = max(1u, processors / 2u);
			break;

		case TYPE_ALL_BUT_ONE_CORE:
			cores = max(1u, processors - 1);
			break;

		case TYPE_ALL_CORES:
			cores = max(1u, processors);
			break;

		case TYPE_DOUBLE_CORES:
			cores = max(1u, processors * 2u);
			break;

		case TYPE_CUSTOM:
			ocean_assert(false && "Invalid type, using default behavior!");
			cores = max(1u, processors);
			break;
	}

	cores = min(cores, maximalNumberCores);

	workerThreads_.reserve(cores);

	if (cores > 1u)
	{
		signals_.setSize(cores);

		// we determine one global seed value for all threads in one worker
		// however, each worker thread will receive an own seed value based on the global seed value and the index of the thread

		const unsigned int workerSeedValue = RandomI::random32();

		for (unsigned int n = 0u; n < cores; ++n)
		{
			WorkerThread* newWorkerThread = new WorkerThread(workerSeedValue, n);

			ocean_assert(newWorkerThread != nullptr);

			newWorkerThread->start(signals_[n]);

			workerThreads_.emplace_back(newWorkerThread);
		}
	}
}

Worker::Worker(const unsigned int numberCores, const LoadType loadType)
{
	ocean_assert(numberCores >= 1u);
	ocean_assert_and_suppress_unused(loadType == TYPE_CUSTOM, loadType);

	workerThreads_.reserve(numberCores);

	if (numberCores > 1u)
	{
		signals_.setSize(numberCores);

		// we determine one global seed value for all threads in one worker
		// however, each worker thread will receive an own seed value based on the global seed value and the index of the thread

		const unsigned int workerSeedValue = RandomI::random32();

		for (unsigned int n = 0u; n < numberCores; ++n)
		{
			WorkerThread* newWorkerThread = new WorkerThread(workerSeedValue, n);

			ocean_assert(newWorkerThread != nullptr);

			newWorkerThread->start(signals_[n]);

			workerThreads_.emplace_back(newWorkerThread);
		}
	}
}

Worker::~Worker()
{
	for (size_t n = 0; n < workerThreads_.size(); ++n)
	{
		delete workerThreads_[n];
	}
}

unsigned int Worker::threads() const
{
	return max(1u, signals_.size());
}

bool Worker::executeFunction(const Function& function, const unsigned int first, const unsigned int size, const unsigned int firstIndex, const unsigned int sizeIndex, const unsigned int minimalIterations, const unsigned int threadIndex)
{
	ocean_assert(minimalIterations > 0);

	const ScopedLock scopedLock(lock_);

	if (size == 0u)
	{
		return false;
	}

	const unsigned int internalFirstIndex = (firstIndex == (unsigned int)(-1)) ? (function.parameters() - 2) : firstIndex;
	const unsigned int internalSizeIndex = (sizeIndex == (unsigned int)(-1)) ? (function.parameters() - 1) : sizeIndex;

	ocean_assert(internalFirstIndex != internalSizeIndex);

	if (size <= minimalIterations + (minimalIterations / 2u) || signals_.size() == 0u)
	{
		Function specializedFunction(function);
		ocean_assert(internalFirstIndex < specializedFunction.parameters());
		ocean_assert(internalSizeIndex < specializedFunction.parameters());

		specializedFunction.setParameter(internalFirstIndex, first);
		specializedFunction.setParameter(internalSizeIndex, size);

		if (threadIndex != (unsigned int)(-1))
		{
			specializedFunction.setParameter(threadIndex, 0u);
		}

		specializedFunction();
		return true;
	}

	unsigned int firstElement = first;
	unsigned int pendingElements = size;
	unsigned int usedWorkers = 0u;
	unsigned int availableWorkers = min(signals_.size(), pendingElements / minimalIterations);

	while (availableWorkers != 0u && pendingElements != 0u)
	{
		ocean_assert(pendingElements > 0u);
		ocean_assert(availableWorkers > 0u);

		unsigned int elements = max(1u, min(pendingElements, max(pendingElements / availableWorkers, minimalIterations)));

		if (availableWorkers == 1u)
		{
			elements = pendingElements;
		}

		ocean_assert(elements > 0u);

		Function specializedFunction(function);
		ocean_assert(internalFirstIndex < specializedFunction.parameters());
		ocean_assert(internalSizeIndex < specializedFunction.parameters());
		specializedFunction.setParameter(internalFirstIndex, firstElement);
		specializedFunction.setParameter(internalSizeIndex, elements);

		if (threadIndex != (unsigned int)(-1))
		{
			specializedFunction.setParameter(threadIndex, usedWorkers);
		}

		ocean_assert(workerThreads_[usedWorkers]);
		workerThreads_[usedWorkers]->setThreadFunction(std::move(specializedFunction));
		++usedWorkers;
		--availableWorkers;

		firstElement += elements;

		ocean_assert(pendingElements >= elements);
		pendingElements -= elements;
	}

	ocean_assert(pendingElements == 0u);
	ocean_assert(usedWorkers > 0u);

	signals_.waitSubset(usedWorkers);

	return true;
}

Worker::StartIndices Worker::separation(const unsigned int first, const unsigned int size, const unsigned int minimalIterations)
{
	ocean_assert(minimalIterations > 0u);

	if (size == 0u)
	{
		return StartIndices();
	}

	StartIndices startIndices;

	if (size <= minimalIterations + (minimalIterations / 2u) || signals_.size() == 0u)
	{
		return StartIndices(1, first);
	}

	unsigned int firstElement = first;
	unsigned int pendingElements = size;
	unsigned int usedWorkers = 0u;
	unsigned int availableWorkers = min(signals_.size(), pendingElements / minimalIterations);

	while (availableWorkers != 0u && pendingElements != 0u)
	{
		ocean_assert(pendingElements > 0u);
		ocean_assert(availableWorkers > 0u);

		unsigned int elements = max(1u, min(pendingElements, max(pendingElements / availableWorkers, minimalIterations)));

		if (availableWorkers == 1u)
			elements = pendingElements;

		ocean_assert(elements > 0u);

		startIndices.emplace_back(firstElement);

		++usedWorkers;
		--availableWorkers;

		firstElement += elements;

		ocean_assert(pendingElements >= elements);
		pendingElements -= elements;
	}

	ocean_assert(pendingElements == 0u);
	ocean_assert_and_suppress_unused(usedWorkers > 0u, usedWorkers);

	return startIndices;
}

bool Worker::executeAbortableFunction(const AbortableFunction& abortableFunction, const unsigned int abortIndex, const unsigned int maximalExecutions)
{
	const ScopedLock scopedLock(lock_);

	if (maximalExecutions == 1u || signals_.size() == 0u)
	{
		AbortableFunction functionCopy(abortableFunction);
		*functionCopy.parameter<bool*>(abortIndex) = false;

		return functionCopy();
	}

	unsigned int usedWorkers = signals_.size();
	if (maximalExecutions > 0u && maximalExecutions < signals_.size())
	{
		usedWorkers = maximalExecutions;
	}

	AbortableFunction functionCopy(abortableFunction);
	*functionCopy.parameter<bool*>(abortIndex) = false;

	for (unsigned int n = 0u; n < usedWorkers; ++n)
	{
		ocean_assert(workerThreads_[n]);
		workerThreads_[n]->setThreadFunction(functionCopy, abortIndex);
	}

	signals_.waitSubset(usedWorkers);
	bool result = false;

	for (unsigned int n = 0u; n < usedWorkers; ++n)
	{
		const WorkerThread::WorkerState state(workerThreads_[n]->state());
		ocean_assert(state == WorkerThread::WS_POSITIVE_RESULT || state == WorkerThread::WS_NEGATIVE_RESULT);

		if (state == WorkerThread::WS_POSITIVE_RESULT)
		{
			result = true;
			break;
		}
	}

	return result;
}

bool Worker::executeSeparableAndAbortableFunction(const AbortableFunction& abortableFunction, const unsigned int first, const unsigned int size, const unsigned int firstIndex, const unsigned int sizeIndex, const unsigned int abortIndex, const unsigned int minimalIterations)
{
	ocean_assert(minimalIterations > 0u);

	const ScopedLock scopedLock(lock_);

	if (size == 0u)
	{
		return false;
	}

	if (size <= minimalIterations || signals_.size() == 0u)
	{
		AbortableFunction specializedFunction(abortableFunction);
		ocean_assert(firstIndex < specializedFunction.parameters());
		ocean_assert(sizeIndex < specializedFunction.parameters());

		specializedFunction.setParameter(firstIndex, first);
		specializedFunction.setParameter(sizeIndex, size);

		return specializedFunction();
	}

	unsigned int availableWorkers = signals_.size();
	unsigned int firstElement = first;
	unsigned int pendingElements = size;
	unsigned int usedWorkers = 0u;

	while (availableWorkers != 0u && pendingElements != 0u)
	{
		ocean_assert(pendingElements > 0u);
		ocean_assert(availableWorkers > 0u);

		unsigned int elements = max(1u, min(pendingElements, max(pendingElements / availableWorkers, minimalIterations)));

		if (availableWorkers == 1u)
		{
			elements = pendingElements;
		}

		ocean_assert(elements > 0u);

		AbortableFunction specializedFunction(abortableFunction);
		*specializedFunction.parameter<bool*>(abortIndex) = false;
		ocean_assert(firstIndex < specializedFunction.parameters());
		ocean_assert(sizeIndex < specializedFunction.parameters());
		specializedFunction.setParameter(firstIndex, firstElement);
		specializedFunction.setParameter(sizeIndex, elements);

		ocean_assert(workerThreads_[usedWorkers]);
		workerThreads_[usedWorkers]->setThreadFunction(std::move(specializedFunction), abortIndex);
		++usedWorkers;
		--availableWorkers;

		firstElement += elements;

		ocean_assert(pendingElements >= elements);
		pendingElements -= elements;
	}

	ocean_assert(pendingElements == 0u);
	ocean_assert(usedWorkers > 0u);

	signals_.waitSubset(usedWorkers);

	bool result = false;

	for (unsigned int n = 0u; n < usedWorkers; ++n)
	{
		const WorkerThread::WorkerState state(workerThreads_[n]->state());
		ocean_assert(state == WorkerThread::WS_POSITIVE_RESULT || state == WorkerThread::WS_NEGATIVE_RESULT);

		if (state == WorkerThread::WS_POSITIVE_RESULT)
		{
			result = true;
			break;
		}
	}

	return result;
}

bool Worker::executeFunctions(const Functions& functions)
{
	const ScopedLock scopedLock(lock_);

	if (functions.empty())
	{
		return false;
	}

	if (signals_.size() == 0u)
	{
		for (Functions::const_iterator i = functions.begin(); i != functions.end(); ++i)
		{
			(*i)();
		}

		return true;
	}

	Functions::const_iterator i = functions.begin();

	while (i != functions.end())
	{
		unsigned int availableWorkers = signals_.size();
		unsigned int usedWorkers = 0u;

		while (availableWorkers != 0u && i != functions.end())
		{
			ocean_assert(workerThreads_[usedWorkers]);
			workerThreads_[usedWorkers]->setThreadFunction(*i);

			++i;
			++usedWorkers;
			--availableWorkers;
		}

		ocean_assert(usedWorkers > 0u);

		signals_.waitSubset(usedWorkers);
	}

	return true;
}

}
