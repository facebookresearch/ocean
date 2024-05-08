/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_TASK_QUEUE_H
#define META_OCEAN_BASE_TASK_QUEUE_H

#include "ocean/base/Base.h"
#include "ocean/base/Caller.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

#include <queue>

namespace Ocean
{

/**
 * This class implements a queue for tasks.
 * The queue is based on a single thread so that all tasks will be processes sequential in a normal FIFO queue order.<br>
 * In contrast to the Scheduler the TaskQueue invokes a function only once.<br>
 * @see Scheduler, ThreadPool.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT TaskQueue :
	public Singleton<TaskQueue>,
	protected Thread
{
	friend class Singleton<TaskQueue>;

	public:

		/**
		 * Definition of a task as caller function.
		 */
		typedef Caller<void> Task;

	protected:

		/**
		 * Definition of a queue holding tasks.
		 */
		typedef std::queue<Task> Tasks;

	public:

		/**
		 * Adds a new tasks to the queue which will be invoked after all previous task have been invoked.
		 * @param task The new task to add
		 */
		void pushTask(const Task& task);

		/**
		 * Returns the number of tasks currently in the queue (not counting an currently invoked task).
		 * @return The number of pending tasks.
		 */
		size_t pendingTasks();

		/**
		 * Removes all pending tasks from this queue.
		 * A currently invoked task will not be stopped.
		 */
		void clear();

	protected:

		/**
		 * Creates a new task queue object.
		 */
		TaskQueue();

		/**
		 * Destructs a task queue object.
		 */
		~TaskQueue();

		/**
		 * The thread run function.
		 */
		virtual void threadRun();

	protected:

		/// The tasks if this queue.
		Tasks queueTasks;

		/// True, if at least one task has been added to this queue before.
		bool queueHadTask;

		/// The lock of this queue.
		Lock queueLock;
};

}

#endif // META_OCEAN_BASE_TASK_QUEUE_H
