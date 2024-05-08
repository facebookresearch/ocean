/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_PROCESSOR_H
#define META_OCEAN_PLATFORM_ANDROID_PROCESSOR_H

#include "ocean/platform/android/Android.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

/**
 * This class implements processor functions.
 * @ingroup platformandroid
 */
class OCEAN_PLATFORM_ANDROID_EXPORT Processor
{
	public:

		/**
		 * Definition of a processor operation frequency.
		 */
		typedef unsigned int Frequency;

	public:

		/**
		 * Returns an invalid frequency.
		 */
		static constexpr Frequency invalidFrequency();

		/**
		 * Returns the number of installed CPU cores.
		 * @return Number of installed CPU cores, 0 if the number could not be determined
		 */
		static unsigned int installedCores();

		/**
		 * Returns the current operation frequency of a specified core.
		 * @param core The core to return the current frequency for
		 * @return Frequency in kHz otherwise invalidFrequency
		 */
		static Frequency currentFrequency(const unsigned int core = 0u);

		/**
		 * Returns the maximal operation frequency of a specified core.
		 * @param core The core to return the maximal frequency for
		 * @return Frequency in kHz otherwise invalidFrequency
		 */
		static Frequency maxFrequency(const unsigned int core = 0u);

		/**
		 * Returns the minimal operation frequency of a specified core.
		 * @param core The core to return the minimal frequency for
		 * @return Frequency in kHz otherwise invalidFrequency
		 */
		static Frequency minFrequency(const unsigned int core = 0u);

		/**
		 * Returns the CPU affinity of the current thread.
		 * The affinity defines on which CPU cores the thread is allowed to run.
		 * @param cpuMask The affinity mask, each bit is associated with the corresponding CPU
		 * @return True, if succeeded
		 */
		static bool currentThreadAffinity(uint32_t& cpuMask);

		/**
		 * Sets the CPU affinity of the current thread.
		 * The affinity defines on which CPU cores the thread is allowed to run.<br>
		 * Beware: Depending on the Android device, setting CPU affinity may not be allowed, or may be restricted to a subset of all cores.
		 * @param cpuMask The affinity mask, each bit is associated with the corresponding CPU
		 * @return True, if succeeded
		 */
		static bool setCurrentThreadAffinity(const uint32_t cpuMask);

		/**
		 * Sets the CPU affinity of the current thread.
		 * The affinity defines on which CPU cores the thread is allowed to run.<br>
		 * This function allows to specify a range of CPUs (not a mask).
		 * Beware: Depending on the Android device, setting CPU affinity may not be allowed, or may be restricted to a subset of all cores.
		 * @param firstCPU The index of the first CPU to set, with range [0, 31]
		 * @param lastCPU The index of the last (including) CPU to set, with range [firstCPU, 31]
		 * @return True, if succeeded
		 */
		static bool setCurrentThreadAffinity(const unsigned int firstCPU, const unsigned int lastCPU);
};

constexpr Processor::Frequency Processor::invalidFrequency()
{
	return Frequency(-1);
}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_PROCESSOR_H
