/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/slam/BackgroundTask.h"

#include "ocean/base/Thread.h"

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

BackgroundTask::BackgroundTask(Task&& task)
{
	setTask(std::move(task));
}

BackgroundTask::~BackgroundTask()
{
	release();
}

bool BackgroundTask::setTask(Task&& task)
{
	if (!task)
	{
		return false;
	}

	const std::lock_guard lock(mutex_);

	if (task_)
	{
		return false;
	}

	ocean_assert(!taskExecute_);
	ocean_assert(taskProcessed_);
	ocean_assert(released_);

	task_ = std::move(task);

	thread_ = std::jthread([this](std::stop_token stopToken) {
		threadRun(stopToken);
		});

	released_ = false;
	ocean_assert(thread_.joinable());

	return true;
}

bool BackgroundTask::execute()
{
	std::unique_lock lock(mutex_);

		if (released_)
		{
			return false;
		}

		// wait() must have been called before execute(), ensuring taskProcessed_ is false
		ocean_assert(!taskProcessed_);

		ocean_assert(!taskExecute_);
		if (taskExecute_)
		{
			// the task is already executing
			return false;
		}

		taskExecute_ = true;

	lock.unlock();

	taskExecuteCondition_.notify_one();

	return true;
}

BackgroundTask::WaitResult BackgroundTask::wait()
{
	std::unique_lock lock(mutex_);

	// wait until the task has been processed or the task has been released
	taskProcessedCondition_.wait(lock, [&]
		{
			return taskProcessed_ || released_;
		});

	WaitResult waitResult = WR_INVALID;

	if (taskProcessed_)
	{
		waitResult = WR_PROCESSED;
	}
	else if (released_)
	{
		waitResult = WR_RELEASED;
	}

	// reset the processed flag to prepare for the next execute() call
	taskProcessed_ = false;

	return waitResult;
}

void BackgroundTask::release()
{
	std::unique_lock lock(mutex_);

		released_ = true;
		thread_.request_stop();

	lock.unlock();

	// wake up the background thread if it's waiting
	taskExecuteCondition_.notify_one();
	taskProcessedCondition_.notify_one();

	// wait for the background thread to terminate
	if (thread_.joinable())
	{
		thread_.join();
	}
}

void BackgroundTask::threadRun(std::stop_token stopToken)
{
	Thread::setThreadPriority(Thread::PRIORTY_BELOW_NORMAL);

	while (!stopToken.stop_requested())
	{
		// wait until the task should be executed

		std::unique_lock lock(mutex_);

			taskExecuteCondition_.wait(lock, [&]
				{
					return taskExecute_ || stopToken.stop_requested();
				});

			if (stopToken.stop_requested())
			{
				break;
			}

			ocean_assert(taskExecute_);
			ocean_assert(!taskProcessed_);

		lock.unlock();

		// execute the task without holding the lock
		ocean_assert(task_);
		task_();

		// signal that the task has been processed

		lock.lock();

			taskExecute_ = false;
			taskProcessed_ = true;

		lock.unlock();

		taskProcessedCondition_.notify_one();
	}
}

}

}

}
