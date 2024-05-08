/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/TaskQueue.h"
#include "ocean/base/RandomI.h"

namespace Ocean
{

TaskQueue::TaskQueue() :
	Thread("TaskQueue thread"),
	queueHadTask(false)
{
	// nothing to do here
}

TaskQueue::~TaskQueue()
{
	clear();
	stopThreadExplicitly();
}

void TaskQueue::pushTask(const Task& task)
{
	const ScopedLock scopedLock(queueLock);

	queueTasks.push(task);

	if (!queueHadTask)
	{
		queueHadTask = true;
		startThread();
	}
}

size_t TaskQueue::pendingTasks()
{
	const ScopedLock scopedLock(queueLock);

	const size_t size = queueTasks.size();
	return size;
}

void TaskQueue::clear()
{
	const ScopedLock scopedLock(queueLock);
	queueTasks = Tasks();
}

void TaskQueue::threadRun()
{
	while (!shouldThreadStop())
	{
		TemporaryScopedLock scopedLock(queueLock);

		if (queueTasks.empty())
		{
			scopedLock.release();
			sleep(1u);
		}
		else
		{
			const Task task(queueTasks.front());
			queueTasks.pop();

			scopedLock.release();

			ocean_assert(task);
			task();
		}
	}
}

}
