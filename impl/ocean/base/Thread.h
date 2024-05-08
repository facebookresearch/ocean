/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_THREAD_H
#define META_OCEAN_BASE_THREAD_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Triple.h"

#if defined(_WINDOWS)
	#include <winsock2.h>
	#include <windows.h>
#else
	#include <pthread.h>
#endif

#include <atomic>

namespace Ocean
{

/**
 * This class implements a thread.
 * The implementation can be used in two ways:<br>
 * First: Derive an own class from this thread class and overwrite the internal Thread::threadRun() function.<br>
 * This function will then be called once the thread has been started. If this run function returns the thread will be closed.<br>
 * Second: Set the run callback function which will be called instead of the normal internal run function.<br>
 * Similar to the first solution the thread will be closed after the callback function returns.<br>
 * However, if the normal internal run function is overloaded by a derived call a possible defined callback function has no effect.<br>
 *
 * Each thread can be started using the Thread::startThread() function,<br>
 * and stopped using the Thread::stopThread() function.<br>
 * However, the stop function will not explicit terminate the thread, it sets the thread's should-stop-state only.<br>
 * Therefore, an implementation using this thread class must check the thread's should-stop-state recurrently.<br>
 * Use Thread::shouldThreadStop() to determine whether your implementation should stop the thread execution.<br>
 *
 * If the thread execution does not return after a Thread::stopThread() the thread can be kill in an explicit manner.<br>
 * Beware: Such kind of rough termination should be avoided and in most cases evidences a dirty usage of the internal run function.<br>
 *
 * See this tutorial:
 * @code
 * // Any class using the thread class as base class.
 * class Timer : protected Thread
 * {
 *     public:
 *
 *         // Creates a new timer object.
 *         Timer();
 *
 *         // Stars the timer.
 *         void start();
 *
 *         // Stops the timer.
 *         void stop()
 *
 *     private:
 *
 *         // Internal thread run function which overloaded the function from the thread class.
 *         void threadRun() override;
 * };
 *
 * Timer::Timer() :
 *     Thread("Optional thread name")
 * {
 *     // nothing to do here
 *     // optional the thread could be started here,
 *     // however we use the start function for this explicitly
 * }
 *
 * void Timer::start()
 * {
 *     startThread();
 * }
 *
 * void Timer::stop()
 * {
 *     stopThread();
 * }
 *
 * void Timer::threadRun()
 * {
 *     // check whether this thread should stop execution
 *     while (shouldThreadStop() == false)
 *     {
 *         // do anything a timer can do
 *
 *         // to avoid a very high CPU load it can be suitable to sleep this thread for a small time
 *         sleep(1);
 *     }
 *
 *     // now the run function will return and the thread will be closed
 * }
 *
 * // Usage of the timer in any environment, here it is used in a main function
 * void main()
 * {
 *     Timer timer;
 *     timer.start();
 *
 *     // do anything in a e.g. message loop
 *
 *     timer.stop();
 * }
 * @endcode
 * @see ThreadPool, Scheduler, Worker.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Thread
{
	public:

		/**
		 * This class implements a platform independent wrapper for thread ids.
		 */
		class ThreadId
		{
			friend class Thread;

			public:

				/**
				 * Creates a new thread id object with invalid id value.
				 */
				ThreadId() = default;

				/**
				 * Returns whether this object holds a valid id.
				 * @return True, if succeeded
				 */
				inline bool isValid() const;

				/**
				 * Returns the hash value of this thread id.
				 * On platforms directly providing a unique integer thread id as value the hash value is exactly that integer value.
				 * @return The unique hash value of this thread id
				 */
				inline uint64_t hash() const;

				/**
				 * Returns whether two thread id objects are identical.
				 * @param id The second thread id to compare
				 * @return True, if succeeded
				 */
				inline bool operator==(const ThreadId& id) const;

				/**
				 * Returns whether two thread id objects are not identical.
				 * @param id The second thread id to compare
				 * @return True, if succeeded
				 */
				inline bool operator!=(const ThreadId& id) const;

				/**
				 * Compares two thread id objects by their hash values.
				 * @param id The second thread id to compare
				 * @return True, if the hash value of this object is smaller than the hash value of the second object
				 */
				inline bool operator<(const ThreadId& id) const;

			protected:

				/**
				 * Creates a new thread id object.
				 * @param value The value to be wrapped by the new object
				 */
				explicit inline ThreadId(const uint64_t value);

				/**
				 * Returns an invalid thread id value.
				 * @return The invalid thread id value
				 */
				static constexpr uint64_t invalidThreadId();

			protected:

				/// The value of the thread id.
				uint64_t value_ = invalidThreadId();
		};

		/**
		 * Definition of different thread priority values.
		 */
		enum ThreadPriority
		{
			/// The thread runs if the system is idle.
			PRIORITY_IDLE,
			/// The thread has a priority below normal.
			PRIORTY_BELOW_NORMAL,
			/// The thread has a normal priority.
			PRIORTY_NORMAL,
			/// The thread has a priority above normal.
			PRIORTY_ABOVE_NORMAL,
			/// The thread has a high priority.
			PRIORTY_HIGH,
			/// The thread has a real time priority.
			PRIORTY_REALTIME
		};

	protected:

#if defined(_WINDOWS)
#else

		/**
		 * Definition of a pair holding a thread id and a boolean state.
		 */
		typedef std::pair<pthread_t, bool> TimedJoinPair;

#endif

	public:

		/**
		 * Creates a new thread object.
		 * The thread will be initialized with a seed value automatically generated by using RandomI::random32().
		 * @param name Optional thread name which can be helpful for debugging tasks
		 */
		explicit Thread(const std::string& name = std::string());

		/**
		 * Creates a new thread object.
		 * @param randomNumberSeedValue An explicit seed value for the random number initialization, with range [0, infinity)
		 * @param name Optional thread name which can be helpful for debugging tasks
		 */
		explicit Thread(const unsigned int randomNumberSeedValue, const std::string& name = std::string());

		/**
		 * Destructs a thread object.
		 */
		virtual ~Thread();

		/**
		 * Starts the thread.
		 * @return True, if the thread is not active
		 */
		bool startThread();

		/**
		 * Informs the thread to stop.
		 * see shouldThreadStop().
		 */
		void stopThread();

		/**
		 * Terminates the thread.
		 * Beware: The thread will be terminated in a very rough way.
		 * @return True, if the thread could be terminated
		 */
		bool terminateThread();

		/**
		 * Waits until this thread has been stopped.
		 * @param timeout The number of milliseconds the caller thread will wait for this thread, -1 to wait infinite
		 * @return True, if the thread has finished; False, if the timeout was exceeded
		 */
		bool joinThread(const unsigned int timeout = (unsigned int)(-1));

		/**
		 * Returns whether this thread should stop.
		 * @return True, if so
		 * @see stopThread().
		 */
		bool shouldThreadStop() const;

		/**
		 * Returns whether this thread has been invoked to start immediately.
		 * Beware: No information is provided whether the thread is active already.
		 * However, to not start a thread invoked to start again, instead wait for the termination.
		 * @return True, if so
		 */
		bool isThreadInvokedToStart() const;

		/**
		 * Returns whether this thread is active.
		 * An active thread currently executes the internal thread function.
		 * @return True, if so
		 */
		bool isThreadActive() const;

		/**
		 * Sleeps the calling thread for a given time.
		 * @param ms Sleeping time in ms
		 */
		static void sleep(unsigned int ms);

		/**
		 * Gives up the remaining thread time.
		 */
		static void giveUp();

		/**
		 * Returns the thread id of the current (calling) thread.
		 * @return The thread id of the current thread
		 */
		static ThreadId currentThreadId();

		/**
		 * Returns the priority of the current thread.
		 * @return Thread priority
		 */
		static ThreadPriority threadPriority();

		/**
		 * Sets the priority of the current thread.
		 * @param priority Thread priority to set
		 * @return True, if succeeded
		 */
		static bool setThreadPriority(const ThreadPriority priority);

#ifdef __APPLE__

		/**
		 * Implements a thread join function with timeout value.
		 * Depending on the platform, this function may not exist in the default libraries (e.g., on Apple platforms).
		 * @param thread The thread for which the ending function will wait
		 * @param retval The optional return value of the thread
		 * @param abstime The absolute timestamp after which the thread will have been ended
		 * @return Zero (0), if succeeded
		 */
		static int pthread_timedjoin_np(pthread_t thread, void** retval, const struct timespec* abstime);

#endif

	protected:

		/**
		 * Disabled copy constructor.
		 * @param thread The object that would be copied
		 */
		Thread(const Thread& thread) = delete;

		/**
		 * Creates the thread itself.
		 */
		void createThread();

		/**
		 * Destroys the thread itself.
		 * However the thread must be terminated before!
		 */
		void destroyThread();

		/**
		 * Tries to stop the thread gracefully.
		 * However, if the thread can not be stopped it is terminated in a rough manner.<br>
		 * Call this function in the destructor of a derived class.
		 * @param timeout Time to wait for a graceful thread termination, in ms
		 */
		void stopThreadExplicitly(const unsigned int timeout = 5000u);

		/**
		 * This function has to be overloaded in derived class.
		 */
		virtual void threadRun() = 0;

		/**
		 * The disabled assign operator.
		 * @param thread The object that would be assigned
		 */
		Thread& operator=(const Thread& thread) = delete;

	private:

#ifdef __APPLE__

		/**
		 * The helper function for the pthread_timedjoin_np() implementation.
		 * @param threadData The thread's data
		 * @return The return value of the thread
		 */
		static void* pthread_timedjoin_np_helper(void* threadData);

#endif

		/**
		 * Platform independent internal thread function calling the external thread function.
		 */
		void internalThreadRun();

#if defined(_WINDOWS)

		/**
		 * Internal thread function calling the external thread function.
		 * @param data The data object which will contain the thread owner object
		 * @return Thread return value
		 */
		static DWORD __stdcall staticThreadRun(void* data);

#else

		/**
		 * Internal thread function calling the external thread function.
		 * @param data The data object which will contain the thread owner object
		 */
		static void* staticThreadRun(void* data);

#endif

	private:

#if defined(_WINDOWS)

		/// Internal windows thread handle.
		HANDLE threadHandle_ = nullptr;

#else

		/// Internal pthread object.
		pthread_t threadObject_ = 0;

#endif

		/// Determines whether this thread should stop.
		std::atomic<bool> threadShouldStop_{false};

		/// Determines whether this thread is actually running.
		bool threadIsActive_ = false;

		/// Determines whether this thread has been invoked to start immediately.
		bool threadIsInvokedToStart_ = false;

		/// Name of the thread.
		std::string threadName_;

		/// The seed value for random number generators.
		unsigned int threadRandomNumberSeedValue_ = 0u;
};

inline Thread::ThreadId::ThreadId(const uint64_t value) :
	value_(value)
{
	// nothing to do here
}

inline bool Thread::ThreadId::isValid() const
{
	return value_ != invalidThreadId();
}

inline uint64_t Thread::ThreadId::hash() const
{
	return value_;
}

inline bool Thread::ThreadId::operator==(const ThreadId& id) const
{
	return value_ == id.value_;
}

inline bool Thread::ThreadId::operator!=(const ThreadId& id) const
{
	return !(*this == id);
}

inline bool Thread::ThreadId::operator<(const ThreadId& id) const
{
	return hash() < id.hash();
}

constexpr uint64_t Thread::ThreadId::invalidThreadId()
{
	return uint64_t(-1);
}

}

#endif // META_OCEAN_BASE_THREAD_H
