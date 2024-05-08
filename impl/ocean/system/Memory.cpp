/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/Memory.h"

#if defined(_WINDOWS)
	#include <winsock2.h>
	#include <windows.h>
	#include <Psapi.h>
#elif defined(__APPLE__)
	#include <mach/mach.h>
#elif defined(__linux__)
	#include <fstream>
	#include <sstream>
#endif

namespace Ocean
{

namespace System
{

void Memory::MemoryMeasurement::start()
{
	ocean_assert(isThreadActive() == false);

	measurements_.clear();
	measurementFirstInThread_ = 0ull;

	// first measurement
	measurements_.emplace_back(processVirtualMemory());

	startThread();
}

void Memory::MemoryMeasurement::stop()
{
	if (!isThreadActive())
	{
		return;
	}

	stopThread();

	while (isThreadActive())
	{
		Thread::sleep(1u);
	}

	// last measurement
	measurements_.emplace_back(processVirtualMemory());
}

const std::vector<uint64_t>& Memory::MemoryMeasurement::measurements()
{
	return measurements_;
}

int64_t Memory::MemoryMeasurement::measurementImpact()
{
	if (measurements_.empty())
	{
		return 0ll;
	}

	ocean_assert(measurementFirstInThread_ != 0ull);

	if (measurementFirstInThread_ >= measurements_.front())
	{
		return int64_t(measurementFirstInThread_ - measurements_.front()) + int64_t(measurements_.capacity() * sizeof(uint64_t));
	}
	else
	{
		return -int64_t(measurements_.front() - measurementFirstInThread_) + int64_t(measurements_.capacity() * sizeof(uint64_t));
	}
}

uint64_t Memory::MemoryMeasurement::minimum()
{
	uint64_t value = uint64_t(-1);

	for (const uint64_t& measurement : measurements_)
	{
		if (measurement < value)
		{
			value = measurement;
		}
	}

	return value;
}

uint64_t Memory::MemoryMeasurement::maximum()
{
	uint64_t value = 0ull;

	for (const uint64_t& measurement : measurements_)
	{
		if (measurement > value)
		{
			value = measurement;
		}
	}

	return value;
}

int64_t Memory::MemoryMeasurement::minPeakToIdentity()
{
	if (measurements_.empty())
	{
		return 0ll;
	}

	const uint64_t minValue = minimum();

	if (minValue > measurements_.front())
	{
		return int64_t(minValue - measurements_.front());
	}
	else
	{
		return -int64_t(measurements_.front() - minValue);
	}
}

int64_t Memory::MemoryMeasurement::maxPeakToIdentity()
{
	if (measurements_.empty())
	{
		return 0ll;
	}

	const uint64_t maxValue = maximum();

	if (maxValue > measurements_.front())
	{
		return int64_t(maxValue - measurements_.front());
	}
	else
	{
		return -int64_t(measurements_.front() - maxValue);
	}
}

void Memory::MemoryMeasurement::threadRun()
{
	ocean_assert(measurementFirstInThread_ == 0ull);
	ocean_assert(measurements_.size() == 1);

	measurementFirstInThread_ = processVirtualMemory();

	while (!shouldThreadStop())
	{
		measurements_.emplace_back(processVirtualMemory());
		Thread::sleep(0u);
	}
}

int Memory::memoryLoad()
{

#ifdef _WINDOWS

	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);

	GlobalMemoryStatusEx(&status);
	ocean_assert(status.dwMemoryLoad >= 0 && status.dwMemoryLoad <= 100);
	return int(status.dwMemoryLoad);

#elif defined(__APPLE__)

	// not available on Apple platforms due to the need of "Describing use of required reason API"
	return -1;

#else

	/*struct sysinfo info;
	const int result = sysinfo(&info);
	ocean_assert(result == 0);

	return (unsigned int)(100ull - 100ull * (uint64_t(info.freeram) + uint64_t(info.freeswap)) / (uint64_t(info.totalram) + uint64_t(info.totalswap)));*/

	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	return 0ull;

#endif

}

uint64_t Memory::processVirtualMemory()
{

#if defined(_WINDOWS)

	PROCESS_MEMORY_COUNTERS_EX processMemoryCounters;
	const bool result = GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&processMemoryCounters, sizeof(processMemoryCounters)) == TRUE;
	ocean_assert(result);

	if (result)
	{
		return uint64_t(processMemoryCounters.PrivateUsage);
	}
	else
	{
		return 0ull;
	}

#elif defined(__APPLE__)

	mach_task_basic_info_data_t taskInfo;
	mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
	const bool result = task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&taskInfo, &count) == KERN_SUCCESS;
	ocean_assert(result);

	if (result)
	{
		return uint64_t(taskInfo.resident_size);
	}
	else
	{
		return 0ull;
	}

#elif defined(__linux__) && !defined(_ANDROID)

	// The /proc/self/ directory is a link to the currently running process. This
	// allows a process to look at itself without having to know its process ID.
	std::ifstream statusFile("/proc/self/status");

	if (!statusFile.is_open())
	{
		ocean_assert(false && "Failed to read from /proc/self/...");
		return 0ull;
	}

	std::string line;

	while (std::getline(statusFile, line))
	{
		// Expected format: VmSize:    16400 kB
		std::istringstream stream(line);

		std::string fieldName;
		stream >> fieldName;

		if (fieldName == "VmSize:")
		{
			uint64_t value = 0ull;
			std::string unitName;
			stream >> value >> unitName;

			if (unitName == "kB")
			{
				return 1000ull * value; // kB = 1000, KB = KiB = 1024
			}
			else
			{
				ocean_assert(false && "Unknown memory unit!");
			}
		}
	}

	ocean_assert(false && "Failed to query virtual memory used by this process");
	return 0ull;

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	return 0ull;

#endif

}

uint64_t Memory::totalPhysicalMemory()
{

#ifdef _WINDOWS

	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);

	GlobalMemoryStatusEx(&status);
	return uint64_t(status.ullTotalPhys);

#elif defined(__APPLE__)

	// not available on Apple platforms due to the need of "Describing use of required reason API"
	return 0ull;

#else

	/*struct sysinfo info;
	const int result = sysinfo(&info);
	ocean_assert(result == 0);

	return uint64_t(info.totalram) * uint64_t(info.mem_unit);*/

	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	return 0ull;

#endif

}

uint64_t Memory::totalVirtualMemory()
{

#ifdef _WINDOWS

	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);

	GlobalMemoryStatusEx(&status);
	return uint64_t(status.ullTotalVirtual);

#elif defined(__APPLE__)

	// not available on Apple platforms due to the need of "Describing use of required reason API"
	return 0ull;

#else

	/*struct sysinfo info;
	const int result = sysinfo(&info);
	ocean_assert(result == 0);

	return (uint64_t(info.totalram) + uint64_t(info.totalswap)) * uint64_t(info.mem_unit);*/

	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	return 0ull;

#endif

}

uint64_t Memory::availablePhysicalMemory()
{

#ifdef _WINDOWS

	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);

	GlobalMemoryStatusEx(&status);
	return uint64_t(status.ullAvailPhys);

#elif defined(__APPLE__)

	// not available on Apple platforms due to the need of "Describing use of required reason API"
	return 0ull;

#else

	/*struct sysinfo info;
	const int result = sysinfo(&info);
	ocean_assert(result == 0);

	return uint64_t(info.freeram) * uint64_t(info.mem_unit);*/

	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	return 0ull;

#endif

}

uint64_t Memory::availableVirtualMemory()
{

#ifdef _WINDOWS

	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);

	GlobalMemoryStatusEx(&status);
	return uint64_t(status.ullAvailVirtual);

#elif defined(__APPLE__)

	// not available on Apple platforms due to the need of "Describing use of required reason API"
	return 0ull;

#else

	/*struct sysinfo info;
	const int result = sysinfo(&info);
	ocean_assert(result == 0);

	return (uint64_t(info.freeram) + uint64_t(info.freeswap)) * uint64_t(info.mem_unit);*/

	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	return 0ull;

#endif

}

}

}
