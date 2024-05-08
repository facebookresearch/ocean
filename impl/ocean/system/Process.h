/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SYSTEM_PROCESS_H
#define META_OCEAN_SYSTEM_PROCESS_H

#include "ocean/system/System.h"

#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace System
{

/**
 * This class implements basic functions related with the application process.
 * @ingroup system
 */
class OCEAN_SYSTEM_EXPORT Process : public Singleton<Process>
{
	friend class Singleton<Process>;

	public:

		/**
		 * Definition of different process priority values.
		 */
		enum ProcessPriority
		{
			/// The process runs if the system is idle.
			PRIORITY_IDLE,
			/// The process has a priority below normal.
			PRIORITY_BELOW_NORMAL,
			/// The process has a normal priority.
			PRIORITY_NORMAL,
			/// The process has a priority above normal.
			PRIORITY_ABOVE_NORMAL,
			/// The process has a high priority.
			PRIORITY_HIGH,
			/// The process has a realtime priority.
			PRIORITY_REALTIME
		};

	public:

		/**
		 * Returns the processor load since the last call to this function.
		 * @return Current processor load in percent, with range [0.0, 1.0]
		 */
		float currentProcessorLoad() const;

		/**
		 * Returns the average processor load of this process since the creation.
		 * @return Average processor load in percent, with range [0.0, 1.0]
		 */
		float averageProcessorLoad() const;

		/**
		 * Returns the entire processor time of this process since the creation.
		 * @return Entire processor time in seconds
		 */
		static double entireProcessorTime();

		/**
		 * Returns the live time of this process since the creation.
		 * @return Live time in seconds
		 */
		static double entireProcessLiveTime();

		/**
		 * Returns the start time of this process since 01.01.1970 00:00:00 in UTC time.
		 * @return Start time in microseconds
		 */
		static long long processStartTime();

		/**
		 * Returns the priority of the process.
		 * @return Process priority
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		static ProcessPriority priority();

		/**
		 * Sets the priority of the process.
		 * @param priority Process priority to set
		 * @return True, if succeeded
		 */
		static bool setPriority(const ProcessPriority priority);

		/**
		 * Returns the process id of the current (calling) process.
		 * @return The process id of the current process
		 */
		static unsigned int currentProcessId();

	private:

		/**
		 * Creates a new process object.
		 * As this class is a singleton get access to this object by the singleton.
		 */
		Process();

	private:

		/// Time of most recent processor load request.
		mutable int64_t loadTimestamp_ = 0ll;

		/// Processor user time of most recent processor load request.
		mutable int64_t loadTime_ = 0ll;

		/// Number of real CPU cores available in this system.
		const unsigned int numberCores_ = 0u;
};

}

}

#endif // META_OCEAN_SYSTEM_PROCESS_H
