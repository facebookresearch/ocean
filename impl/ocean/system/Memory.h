/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_MEMORY_H
#define META_OCEAN_SYSTEM_MEMORY_H

#include "ocean/system/System.h"

#include "ocean/base/Thread.h"

namespace Ocean
{

namespace System
{

/**
 * This class implements system memory management functions.
 * @ingroup system
 */
class OCEAN_SYSTEM_EXPORT Memory
{
	public:

		/**
		 * This class implements a simple engine measuring memory (RAM) usage over time for the entire process.
		 * The object simply starts an own thread and measures the used virtual memory as often as possible.<br>
		 * Beware: The object can create several thousand measurements in a few seconds.<br>
		 * Thus, this function is mainly intended to measure the RAM impact of individual functions instead of measuring the RAM impact during a longer time period.<br>
		 * This function is not thread-safe.
		 *
		 * Here is a tutorial how to use this class:
		 * @code
		 * {
		 *     Memory::MemoryMeasurement measurement;
		 *
		 *     measurement.start();
		 *
		 *     // now we can call a function needing a huge amount of memory
		 *     // beware: other threads may also have a memory impact that will be part of the measurements
		 *     expensiveMemoryFunction();
		 *
		 *     measurement.stop();
		 * }
		 * @endcode
		 * @see processVirtualMemory().
		 */
		class MemoryMeasurement : protected Thread
		{
			public:

				/**
				 * Starts the measurements.
				 * Previous measurements will be cleared.<br>
				 * Beware: Stop measuring as quick as possible - as we will get a large amount of measurements.
				 * @see stop().
				 */
				void start();

				/**
				 * Stops the measurements.
				 * Do not call this function before calling start().
				 * @see start().
				 */
				void stop();

				/**
				 * Returns all memory measurements that have been done between calling start() and stop().
				 * The resulting measurements provide the virtual memory used by the calling process in bytes (over time).<br>
				 * Beware: Do not call this function before stopping measurements.
				 * @return The entire set of measurements
				 * @see stop(), processVirtualMemory().
				 */
				const std::vector<uint64_t>& measurements();

				/**
				 * Returns the number of bytes this memory profiler object will be responsible for.
				 * The impact is based on the assumption e.g., starting a thread or storing measurements will need memory.<br>
				 * We can use the result of this function to have a rough guess how big this impact is.
				 * @return The impact this profile tool has on the memory usage in bytes, with range (-infinity, infinity)
				 */
				int64_t measurementImpact();

				/**
				 * Returns the minimal measurement that has been done between calling start() and stop().
				 * @return The minimal virtual memory usage in bytes, with range [0, maximal()]
				 * Beware: Do not call this function before stopping measurements.
				 * @see stop(), maximal().
				 */
				uint64_t minimum();

				/**
				 * Returns the maximal measurement that has been done between calling start() and stop().
				 * @return The maximal virtual memory usage in bytes, with range [minimum(), infinity)
				 * Beware: Do not call this function before stopping measurements.
				 * @see stop(), minimum().
				 */
				uint64_t maximum();

				/**
				 * Returns the minimal memory peak in relation to the identity.
				 * The identity is the very first measurement immediately determined when starting the profiler via start().<br>
				 * This Function actually returns minimum() - measurements.front()<br>
				 * Beware: Do not call this function before stopping measurements.
				 * @return The difference between minimum memory usage and the very first memory measurement
				 * @see measurementImpact(), maxPeakToIdentity(), stop().
				 */
				int64_t minPeakToIdentity();

				/**
				 * Returns the maximal memory peak in relation to the identity.
				 * The identity is the very first measurement immediately determined when starting the profiler via start().<br>
				 * This Function actually returns maximum() - measurements.front()<br>
				 * Beware: Do not call this function before stopping measurements.
				 * @return The difference between maximal memory usage and the very first memory measurement
				 * @see measurementImpact(), minPeakToIdentity(), stop().
				 */
				int64_t maxPeakToIdentity();

			protected:

				/**
				 * This function has to be overloaded in derived class.
				 */
				void threadRun() override;

			protected:

				/// All measurements that have been gathered between calling start() and stop().
				std::vector<uint64_t> measurements_;

				/// The first measurement within the thread function.
				uint64_t measurementFirstInThread_;
		};

	public:

		/**
		 * Returns the current system memory load in percent, with range [0, 100], -1 if not available
		 * @return Current memory load
		 */
		static int memoryLoad();

		/**
		 * Returns the amount of virtual memory used by the calling process.
		 * @return The virtual memory use by the process, in bytes, 0 if not available
		 */
		static uint64_t processVirtualMemory();

		/**
		 * Returns the system-wide total amount of actual physical memory in bytes.
		 * @return Physical memory in bytes, 0 if not available
		 */
		static uint64_t totalPhysicalMemory();

		/**
		 * Returns the system-wide total amount of virtual memory in bytes.
		 * @return Virtual memory in bytes, 0 if not available
		 */
		static uint64_t totalVirtualMemory();

		/**
		 * Returns the system-wide available amount of physical memory in bytes.
		 * @return Available physical memory in bytes, 0 if not available
		 */
		static uint64_t availablePhysicalMemory();

		/**
		 * Returns the system-wide available amount of virtual memory in bytes.
		 * @return Available virtual memory in bytes, 0 if not available
		 */
		static uint64_t availableVirtualMemory();
};

}

}

#endif // META_OCEAN_SYSTEM_MEMORY_H
