/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_SLAM_BACKGROUND_TASK_H
#define META_OCEAN_TRACKING_SLAM_BACKGROUND_TASK_H

#include "ocean/tracking/slam/SLAM.h"

#include <condition_variable>
#include <functional>
#include <thread>

namespace Ocean
{

namespace Tracking
{

namespace SLAM
{

/**
 * This class implements a task which runs in the background while the caller is able to wait for the task to be finished.
 * The class provides a handshake pattern where wait() and execute() must be called in alternating sequence.<br>
 * This ensures proper synchronization between the main thread and the background thread.<br>
 * Usage pattern:
 * <pre>
 * task.setTask(...);
 *
 * while (processing)
 * {
 *     task.wait();     // wait for previous execution to finish (first call returns immediately)
 *
 *     // ... do main thread work ...
 *
 *     task.execute();  // start background execution
 * }
 *
 * task.release();
 * </pre>
 * @ingroup trackingslam
 */
class OCEAN_TRACKING_SLAM_EXPORT BackgroundTask
{
	public:

		/**
		 * Definition of individual wait result states.
		 */
		enum WaitResult : uint32_t
		{
			/// Invalid wait result.
			WR_INVALID = 0u,
			/// The task was released before completion.
			WR_RELEASED,
			/// The task was processed successfully.
			WR_PROCESSED
		};

		/**
		 * Definition of a task function to be executed in the background.
		 */
		using Task = std::function<void()>;

	public:

		/**
		 * Creates a new background task object without a task.
		 * @see setTask()
		 */
		BackgroundTask() = default;

		/**
		 * Creates a new background task object with a given task.
		 * @param task The task to be executed in the background, must be valid
		 * @see setTask()
		 */
		explicit BackgroundTask(Task&& task);

		/**
		 * Destructs the background task object.
		 * The destructor automatically calls release() to ensure the background thread is stopped.
		 * @see release()
		 */
		~BackgroundTask();

	public:

		/**
		 * Sets the task to be executed in the background.
		 * This function creates and starts the background thread which will wait for execute() calls.
		 * @param task The task to be executed in the background, must be valid
		 * @return True, if succeeded; False, if a task is already set
		 */
		bool setTask(Task&& task);

		/**
		 * Executes the task in the background.
		 * This function signals the background thread to execute the task.
		 * The function returns immediately without waiting for the task to complete.
		 * wait() must be called before this function (except for the first call after setTask()).
		 * @return True, if the task execution was initiated successfully; False, if the task is already executing or the object was released
		 * @see wait()
		 */
		bool execute();

		/**
		 * Waits until the previous task execution has been processed.
		 * This function must be called before execute() to ensure the previous execution has completed.
		 * On the first call after setTask(), this function returns immediately with WR_PROCESSED.
		 * @return WR_PROCESSED if the task completed successfully, WR_RELEASED if the task was released, or WR_INVALID in case of an error
		 * @see execute()
		 */
		WaitResult wait();

		/**
		 * Explicitly releases the background task and stops the background thread.
		 * This function blocks until the background thread has fully terminated.
		 * After calling this function, the object cannot be used anymore unless setTask() is called again.
		 * The destructor automatically calls this function.
		 */
		void release();

	protected:

		/**
		 * Disabled copy constructor.
		 * @param backgroundTask The background task object to copy
		 */
		BackgroundTask(const BackgroundTask& backgroundTask) = delete;

		/**
		 * Disabled move constructor.
		 * @param backgroundTask The background task object to move
		 */
		BackgroundTask(BackgroundTask&& backgroundTask) = delete;

		/**
		 * The internal thread run function.
		 * This function runs in the background thread and waits for execute() signals to run the task.
		 * @param stopToken The stop token allowing to check whether the thread should be stopped
		 */
		void threadRun(std::stop_token stopToken);

		/**
		 * Disabled copy assignment operator.
		 * @param backgroundTask The background task object to copy
		 * @return Reference to this object
		 */
		BackgroundTask& operator=(const BackgroundTask& backgroundTask) = delete;

		/**
		 * Disabled move assignment operator.
		 * @param backgroundTask The background task object to move
		 * @return Reference to this object
		 */
		BackgroundTask& operator=(BackgroundTask&& backgroundTask) = delete;

	protected:

		/// The background task to execute.
		Task task_;

		/// The background thread.
		std::jthread thread_;

		/// True, if the task should be executed; False, otherwise.
		bool taskExecute_ = false;

		/// The condition variable to signal task execution.
		std::condition_variable taskExecuteCondition_;

		/// True, if the task has been processed; False, otherwise.
		bool taskProcessed_ = true;

		/// The condition variable to signal task completion.
		std::condition_variable taskProcessedCondition_;

		/// The mutex protecting the internal state.
		std::mutex mutex_;

		/// True, if the background task has been released or not yet initialized; False, if active.
		bool released_ = true;
};

}

}

}

#endif // META_OCEAN_TRACKING_SLAM_BACKGROUND_TASK_H
