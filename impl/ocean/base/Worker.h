/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_WORKER_H
#define META_OCEAN_BASE_WORKER_H

#include "ocean/base/Base.h"
#include "ocean/base/Caller.h"
#include "ocean/base/Signal.h"
#include "ocean/base/Thread.h"

#include <vector>

namespace Ocean
{

/**
 * This class implements a worker able to distribute function calls over different threads.
 * Thus, this worker object can be used to distribute complex operations to several CPU cores to speed up the entire computation significantly.<br>
 * The worker provides several modes to distribute the computational load of a complex operation.<br>
 * Function call my be made faster by using subsets of the entire data by individual CPU cores only.<br>
 * Further, this worker supports abortable functions executing the same function several times and stops all other threads if the first function receives a valid result.<br>
 * For more details several code examples are provided:
 * @see executeFunction(), executeFunctions().
 * @see WorkerPool.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Worker
{
	public:

		/**
		 * Definition of a worker function caller object for standard functions to be distributed.<br>
		 * Standard functions allow the computation using a subset of the entire data.
		 */
		typedef Caller<void> Function;

		/**
		 * Definition of a worker function caller object for abort functions.
		 */
		typedef Caller<bool> AbortableFunction;

		/**
		 * Definition of a vector holding worker functions.
		 */
		typedef std::vector<Function> Functions;

		/**
		 * Definition of CPU load types.
		 */
		enum LoadType : uint32_t
		{
			/// One CPU core is used.
			TYPE_ONE_CORE,
			/// Half of the CPU cores are used, minimum is one CPU core.
			TYPE_HALF_CORES,
			/// All CPU cores but one, minimum is one CPU core.
			TYPE_ALL_BUT_ONE_CORE,
			/// All CPU cores are used.
			TYPE_ALL_CORES,
			/// For each CPU core two thread are used.
			TYPE_DOUBLE_CORES,
			/// A custom amount of CPU cores is used.
			TYPE_CUSTOM
		};

		/**
		 * Definition of a vector holding indices.
		 */
		typedef std::vector<unsigned int> StartIndices;

	protected:

		/**
		 * This class implements a thread with an explicit external thread function.<br>
		 */
		class WorkerThread : protected Thread
		{
			public:

				/**
				 * Definition of different worker thread states.
				 */
				enum WorkerState
				{
					/// The worker thread is in a state with unknown result.
					WS_UNKNOWN_RESULT,
					/// The worker thread is in a state with positive function result.
					WS_POSITIVE_RESULT,
					/// The worker thread is in a state with negative function result.
					WS_NEGATIVE_RESULT
				};

			public:

				/**
				 * Creates a new worker thread object.
				 * @param workerSeedValue Worker specific seed value e.g., for random number generators, each thread will work with an own seed value: threadSeedValue = workerSeedValue + workerThreadId
				 * @param workerThreadId Id of the worker thread to distinguish between all threads owned by one worker
				 */
				explicit WorkerThread(const unsigned int workerSeedValue, const unsigned int workerThreadId);

				/**
				 * Destructs a worker thread object.
				 */
				virtual ~WorkerThread();

				/**
				 * Returns the id of this worker thread.
				 * @return Worker thread id
				 */
				inline unsigned int id();

				/**
				 * Starts the worker thread.
				 * @param signal Extern worker signal pushed if this thread has finished
				 * @return True, if succeeded
				 */
				bool start(Signal& signal);

				/**
				 * Sets a new thread function which will be executed immediately using the internal thread.
				 * @param function Thread function to be set
				 */
				inline void setThreadFunction(const Function& function);

				/**
				 * Sets/moves a new thread function which will be executed immediately using the internal thread.
				 * @param function Thread function to be moved
				 */
				inline void setThreadFunction(Function&& function);

				/**
				 * Sets a new thread function which will be executed immediately using the internal thread.
				 * @param abortableFunction Thread function able to be aborted during execution
				 * @param abortIndex Index of the boolean abort parameter, which must be a pointer to a boolean parameter
				 */
				inline void setThreadFunction(const AbortableFunction& abortableFunction, const unsigned int abortIndex);

				/**
				 * Sets/moves a new thread function which will be executed immediately using the internal thread.
				 * @param abortableFunction Thread function able to be aborted during execution, will be moved
				 * @param abortIndex Index of the boolean abort parameter, which must be a pointer to a boolean parameter
				 */
				inline void setThreadFunction(AbortableFunction&& abortableFunction, const unsigned int abortIndex);

				/**
				 * Returns the current worker thread state.
				 * @return Worker state
				 */
				inline WorkerState state() const;

			private:

				/**
				 * This thread function has to be overloaded by derived classes.
				 * @see Thread::threadRun().
				 */
				virtual void threadRun();

			private:

				/// Function using the internal thread to execute.
				Function function_;

				/// Abortable function using the internal thread to execute.
				AbortableFunction abortableFunction_;

				/// Internal signal handling the internal thread execution.
				Signal internalSignal_;

				/// External signal determining the termination of the thread function.
				Signal* externalSignal_ = nullptr;

				/// Id of the worker thread.
				unsigned int id_ = (unsigned int)(-1);

				/// Index the abort parameter for abortable thread functions.
				unsigned int abortIndex_ = (unsigned int)(-1);

				/// Worker thread result.
				WorkerState workerState_;
		};

		/**
		 * Definition of a vector holding worker threads.
		 */
		typedef std::vector<WorkerThread*> WorkerThreads;

	public:

		/**
		 * Creates a new worker object.
		 * The load type defines the number of cores to be used, however the worker will not address more than 'maximalNumberCores'.
		 * @param loadType Load type used for this worker, must not be TYPE_CUSTOM
		 * @param maximalNumberCores The maximal number of cores to be used, with range [1, infinity)
		 */
		explicit Worker(const LoadType loadType = TYPE_ALL_CORES, const unsigned int maximalNumberCores = 16u);

		/**
		 * Creates a new worker object with a custom amount of worker threads.
		 * @param numberCores The number of threads to use, with range [1, infinity)
		 * @param loadType Must be TYPE_CUSTOM
		 */
		Worker(const unsigned int numberCores, const LoadType loadType);

		/**
		 * Destructs a worker object.
		 */
		~Worker();

		/**
		 * Returns the number of threads this worker uses.
		 * @return Number of used threads
		 */
		unsigned int threads() const;

		/**
		 * Executes a callback function separable by two function parameters.
		 * The first separable function parameter defines the start point.<br>
		 * The second separable function parameter defines the number of iterations for the specified start point.<br>
		 *
		 * This example shows how to use the worker object in combination with a distributable function:<br>
		 * @code
		 * void distributableFunction(unsigned char* data, const unsigned int firstByte, const unsigned int numberBytes)
		 * {
		 *     std::cout << "Function call handling bytes: " << firstByte << " up to " << firstByte + numberBytes - 1 << std::endl;
		 *
		 *     // do something with the byte buffer
		 *     // Beware: Change bytes inside the range [firstByte, firstByte + numberBytes - 1] only!
		 *     // ...
		 * }
		 *
		 * void main()
		 * {
		 *     // worker object to execute the distributable function
		 *     Worker worker;
		 *
		 *     // create byte buffer
		 *     unsigned char* data = new unsigned char[1024];
		 *
		 *     // initialize buffer with something
		 *     // ...
		 *
		 *     // call the distributable function to do something with the buffer
		 *     // start with element: 0
		 *     // number of entire elements: 1024
		 *     // the index of the function parameter 'firstByte' is 1 in the 'distributableFunction'
		 *     // the index of the function parameter 'numberBytes' is 2 in the 'distributableFunction'
		 *     worker.executeFunction(Worker::Function::createStatic(&distributeFunction, data, 0u, 0u), 0u, 1024u, 1u, 2u);
		 *
		 *     // we can also skip the indices of the two function parameters 'firstByte' and 'numberBytes' as these order and position is like the default case (last two parameters while the number is the last parameter):
		 *     worker.executeFunction(Worker::Function::createStatic(&distributeFunction, data, 0u, 0u), 0u, 1024u);
		 *
		 *     // use the changed buffer
		 *     // ...
		 *
		 *     delete [] data;
		 * }
		 * @endcode
		 * Output for a CPU with 4 cores would be e.g. (the order of the output may vary):<br>
		 * Function call handling bytes: 0, 255<br>
		 * Function call handling bytes: 256, 511<br>
		 * Function call handling bytes: 512, 767<br>
		 * Function call handling bytes: 768, 1023<bR>
		 *
		 * @param function Separable function to be execute
		 * @param first First function parameter
		 * @param size Size function parameter
		 * @param firstIndex Index of the worker function parameter receiving the start value, if -1 than the index will be set to the second last parameter, otherwise with range [0, function.parameters())
		 * @param sizeIndex Index of the worker function parameter receiving the number value, if -1 than the index will be set to the last parameter, otherwise with range [0, function.parameters())
		 * @param minimalIterations Minimal number of iterations assigned to one internal thread
		 * @param threadIndex Optional index of the worker function parameter receiving the index of the individual thread
		 * @return True, if succeeded
		 */
		bool executeFunction(const Function& function, const unsigned int first, const unsigned int size, const unsigned int firstIndex = (unsigned int)(-1), const unsigned int sizeIndex = (unsigned int)(-1), const unsigned int minimalIterations = 1u, const unsigned int threadIndex = (unsigned int)(-1));

		/**
		 * Executes several callback functions concurrently.
		 *
		 * This example shows how to use the worker object in combination with several individual callback functions:<br>
		 * @code
		 * void function0(unsigned int value)
		 * {
		 *     std::cout << "Function0 call with value: " << value << std::endl;
		 *
		 *     // do something here
		 *     // ...
		 * }
		 *
		 * void function1(std::string value)
		 * {
		 *     std::cout << "Function1 call with value: " << value << std::endl;
		 *
		 *     // do something here
		 *     // ...
		 * }
		 *
		 * void main()
		 * {
		 *     // worker object to execute the distributable function
		 *     Worker worker;
		 *
		 *     // vector with callback functions
		 *     Worker::Functions functions;
		 *
		 *     // creating simple callback functions and adding them to the vector
		 *     functions.push_back(Worker::Function::createStatic(&function0, 9));
		 *     functions.push_back(Worker::Function::createStatic(&function0, 101));
		 *     functions.push_back(Worker::Function::createStatic(&function1, std::string("hallo"));
		 *     functions.push_back(Worker::Function::createStatic(&function1, std::string("world"));
		 *
		 *     // call the callback functions concurrently
		 *     worker.executeFunctions(functions);
		 *
		 *     // do something
		 * }
		 * @endcode
		 * Output (the order of the output may vary):<br>
		 * Function0 call with value: 9<br>
		 * Function1 call with value: world<br>
		 * Function0 call with value: 101<br>
		 * Function1 call with value: hallo<br>
		 *
		 * @param functions Callback function to execute
		 * @return True, if succeeded
		 */
		bool executeFunctions(const Functions& functions);

		/**
		 * Executes an abortable function on several CPU cores.
		 * The function must provide an abort parameter. The parameter is a pointer to a boolean state initialized with False.<br>
		 * @param abortableFunction Function supporting an abort state to terminate the function explicitly
		 * @param abortIndex Index of the abort parameter
		 * @param maximalExecutions Number of maximal CPU core executions
		 * @return True, if one of the functions succeeded
		 */
		bool executeAbortableFunction(const AbortableFunction& abortableFunction, const unsigned int abortIndex, const unsigned int maximalExecutions = 0u);

		/**
		 * Executes an abortable and separable function on several CPU cores.
		 * The function must be separable by two function parameters and must provide an abort parameter allowing to stop the function execution immediately.
		 * @param separableAbortableFunction Function supporting an separation and aborting
		 * @param first The first function parameter
		 * @param size The size function parameter
		 * @param firstIndex Index of the worker function parameter receiving the start value
		 * @param sizeIndex Index of the worker function parameter receiving the number value
		 * @param abortIndex Index of the abort parameter
		 * @param minimalIterations Minimal number of iterations assigned to one internal thread
		 * @return True, if one of the abortable functions succeeded
		 */
		bool executeSeparableAndAbortableFunction(const AbortableFunction& separableAbortableFunction, const unsigned int first, const unsigned int size, const unsigned int firstIndex, const unsigned int sizeIndex, const unsigned int abortIndex, const unsigned int minimalIterations = 1);

		/**
		 * Returns a separation this worker would apply to execute a distributable function.
		 * @param first The first function parameter
		 * @param size The size function parameter
		 * @param minimalIterations Minimal number of iterations assigned to one internal thread
		 * @return Resulting separation (indices of the start points)
		 */
		StartIndices separation(const unsigned int first, const unsigned int size, const unsigned int minimalIterations = 1u);

		/**
		 * Returns whether this worker uses more than one thread to distribute a function.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Explicit worker pointer operator.
		 * @return Worker operator
		 */
		inline operator Worker*() const;

	protected:

		/**
		 * Disabled copy constructor.
		 * @param worker Object which would be copied
		 */
		Worker(const Worker& worker) = delete;

		/**
		 * Disabled copy operator.
		 * @param worker Object which would be copied
		 * @return Reference to this object
		 */
		Worker& operator=(const Worker& worker) = delete;

	protected:

		/// Worker threads.
		WorkerThreads workerThreads_;

		/// Worker signals determining whether all thread finished their work.
		Signals signals_;

		/// Worker lock.
		Lock lock_;
};

inline unsigned int Worker::WorkerThread::id()
{
	return id_;
}

inline void Worker::WorkerThread::setThreadFunction(const Function& function)
{
	ocean_assert(externalSignal_);

	ocean_assert(!function_);
	ocean_assert(!abortableFunction_);

	function_ = function;

	internalSignal_.pulse();
}

inline void Worker::WorkerThread::setThreadFunction(Function&& function)
{
	ocean_assert(externalSignal_);

	ocean_assert(!function_);
	ocean_assert(!abortableFunction_);

	function_ = std::move(function);

	internalSignal_.pulse();
}

inline void Worker::WorkerThread::setThreadFunction(const AbortableFunction& abortableFunction, const unsigned int abortIndex)
{
	ocean_assert(externalSignal_);

	ocean_assert(!function_);
	ocean_assert(!abortableFunction_);

	abortableFunction_ = abortableFunction;
	abortIndex_ = abortIndex;

	internalSignal_.pulse();
}

inline void Worker::WorkerThread::setThreadFunction(AbortableFunction&& abortableFunction, const unsigned int abortIndex)
{
	ocean_assert(externalSignal_);

	ocean_assert(!function_);
	ocean_assert(!abortableFunction_);

	abortableFunction_ = std::move(abortableFunction);
	abortIndex_ = abortIndex;

	internalSignal_.pulse();
}

inline Worker::WorkerThread::WorkerState Worker::WorkerThread::state() const
{
	return workerState_;
}

inline Worker::operator bool() const
{
	return threads() > 1;
}

inline Worker::operator Worker*() const
{
	ocean_assert(false && "Use the explicit address operator instead!");
	return nullptr;
}

}

#endif // META_OCEAN_BASE_WORKER_H
