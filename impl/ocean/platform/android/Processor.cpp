/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/Processor.h"

#include "ocean/base/String.h"

#include <cstdlib>
#include <iostream>

#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

namespace Ocean
{

namespace Platform
{

namespace Android
{

unsigned int Processor::installedCores()
{

#ifdef OCEAN_SUPPORT_EXCEPTIONS

	try
	{

#endif // OCEAN_SUPPORT_EXCEPTIONS

		std::ifstream stream;
		stream.open("/sys/devices/system/cpu/kernel_max", std::ios::binary);
		if (stream.fail())
		{
			return 0u;
		}

		int value = 0;
		stream >> value;
		ocean_assert(value > 0);

		return (unsigned int)(value + 1u);

#ifdef OCEAN_SUPPORT_EXCEPTIONS

	}
	catch(...)
	{
		return 0u;
	}

#endif // OCEAN_SUPPORT_EXCEPTIONS

}

Processor::Frequency Processor::currentFrequency(const unsigned int core)
{

#ifdef OCEAN_SUPPORT_EXCEPTIONS

	try
	{

#endif // OCEAN_SUPPORT_EXCEPTIONS

		std::ifstream stream;
		stream.open((std::string("/sys/devices/system/cpu/cpu") + String::toAString(core) + std::string("/cpufreq/scaling_cur_freq")).c_str(), std::ios::binary);
		if (stream.fail())
		{
			return invalidFrequency();
		}

		Frequency value;
		stream >> value;

		if (stream.fail())
		{
			return invalidFrequency();
		}

		return value / 1000u;

#ifdef OCEAN_SUPPORT_EXCEPTIONS

	}
	catch(...)
	{
		return invalidFrequency();
	}

#endif // OCEAN_SUPPORT_EXCEPTIONS
}

Processor::Frequency Processor::maxFrequency(const unsigned int core)
{
#ifdef OCEAN_SUPPORT_EXCEPTIONS

	try
	{

#endif // OCEAN_SUPPORT_EXCEPTIONS

		{
			std::ifstream stream;
			stream.open((std::string("/sys/devices/system/cpu/cpu") + String::toAString(core) + std::string("/cpufreq/scaling_max_freq")).c_str(), std::ios::binary);
			if (!stream.fail())
			{
				Frequency value;
				stream >> value;

				if (!stream.fail())
				{
					return value / 1000u;
				}
			}
		}

		return invalidFrequency();

#ifdef OCEAN_SUPPORT_EXCEPTIONS

	}
	catch(...)
	{
		return invalidFrequency();
	}

#endif // OCEAN_SUPPORT_EXCEPTIONS

}

Processor::Frequency Processor::minFrequency(const unsigned int core)
{
#ifdef OCEAN_SUPPORT_EXCEPTIONS

	try
	{

#endif // OCEAN_SUPPORT_EXCEPTIONS

		std::ifstream stream;
		stream.open((std::string("/sys/devices/system/cpu/cpu") + String::toAString(core) + std::string("/cpufreq/scaling_min_freq")).c_str(), std::ios::binary);
		if (stream.fail())
		{
			return invalidFrequency();
		}

		Frequency value;
		stream >> value;

		if (stream.fail())
		{
			return invalidFrequency();
		}

		return value / 1000u;

#ifdef OCEAN_SUPPORT_EXCEPTIONS

	}
	catch(...)
	{
		return invalidFrequency();
	}

#endif // OCEAN_SUPPORT_EXCEPTIONS

}

bool Processor::currentThreadAffinity(uint32_t& cpuMask)
{
	cpuMask = 0u;

	cpu_set_t mask;
	CPU_ZERO(&mask);

	if (sched_getaffinity(gettid(), sizeof(mask), &mask) != 0)
	{
		return false;
	}

	for (unsigned int n = 0u; n < 32u; ++n)
	{
		if (CPU_ISSET(n, &mask))
		{
			cpuMask |= 1u << n;
		}
	}

	return true;
}

bool Processor::setCurrentThreadAffinity(const uint32_t cpuMask)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);

	for (unsigned int n = 0u; n < 32u; ++n)
	{
		if (cpuMask & (1u << n))
		{
			CPU_SET(n, &mask);
		}
	}

	return sched_setaffinity(gettid(), sizeof(mask), &mask) == 0;
}

bool Processor::setCurrentThreadAffinity(const unsigned int firstCPU, const unsigned int lastCPU)
{
	ocean_assert(firstCPU <= lastCPU);
	ocean_assert(lastCPU <= 31u);

	uint32_t cpuMask = 0u;

	for (unsigned int n = firstCPU; n <= lastCPU; ++n)
	{
		cpuMask |= 1u << n;
	}

	return setCurrentThreadAffinity(cpuMask);
}

}

}

}
