/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/Process.h"

#include "ocean/base/Processor.h"

#if defined(_WINDOWS)
	#include <winsock2.h>
	#include <windows.h>
#else
	#include <unistd.h>

	#include <sys/resource.h>

	#ifdef __APPLE__
		#include <sys/sysctl.h>
	#endif // __APPLE__

	#ifdef __linux__
		#include <sys/time.h>
	#endif
#endif // _WINDOWS

namespace Ocean
{

namespace System
{

Process::Process() :
	loadTimestamp_(0ll),
	loadTime_(0ll),
	numberCores_(Processor::get().realCores())
{
	// nothing to do here
}

Process::ProcessPriority Process::priority()
{

#if defined(_WINDOWS)

	const HANDLE processHandle = GetCurrentProcess();
	ocean_assert(processHandle != nullptr);

	switch (GetPriorityClass(processHandle))
	{
		case IDLE_PRIORITY_CLASS:
			return PRIORITY_IDLE;

		case BELOW_NORMAL_PRIORITY_CLASS:
			return PRIORITY_BELOW_NORMAL;

		case NORMAL_PRIORITY_CLASS:
			return PRIORITY_NORMAL;

		case ABOVE_NORMAL_PRIORITY_CLASS:
			return PRIORITY_ABOVE_NORMAL;

		case HIGH_PRIORITY_CLASS:
			return PRIORITY_HIGH;

		case REALTIME_PRIORITY_CLASS:
			return PRIORITY_REALTIME;
	}

	ocean_assert(false && "Unknown priority value.");
	return PRIORITY_NORMAL;

#else

	const int priority = getpriority(PRIO_PROCESS, 0);

	if (priority <= -20)
	{
		return PRIORITY_REALTIME;
	}

	if (priority <= -14)
	{
		return PRIORITY_HIGH;
	}

	if (priority <= -7)
	{
		return PRIORITY_ABOVE_NORMAL;
	}

	if (priority <= 0)
	{
		return PRIORITY_NORMAL;
	}

	if (priority <= 7)
	{
		return PRIORITY_BELOW_NORMAL;
	}

	return PRIORITY_IDLE;

#endif

}

bool Process::setPriority(const ProcessPriority priority)
{

#if defined(_WINDOWS)

	const HANDLE processHandle = GetCurrentProcess();

	if (processHandle == nullptr)
	{
		return false;
	}

	switch (priority)
	{
		case PRIORITY_IDLE:
			return SetPriorityClass(processHandle, IDLE_PRIORITY_CLASS) == TRUE;

		case PRIORITY_BELOW_NORMAL:
			return SetPriorityClass(processHandle, BELOW_NORMAL_PRIORITY_CLASS) == TRUE;

		case PRIORITY_NORMAL:
			return SetPriorityClass(processHandle, NORMAL_PRIORITY_CLASS) == TRUE;

		case PRIORITY_ABOVE_NORMAL:
			return SetPriorityClass(processHandle, ABOVE_NORMAL_PRIORITY_CLASS) == TRUE;

		case PRIORITY_HIGH:
			return SetPriorityClass(processHandle, HIGH_PRIORITY_CLASS) == TRUE;

		case PRIORITY_REALTIME:
			return SetPriorityClass(processHandle, REALTIME_PRIORITY_CLASS) == TRUE;
	};

	ocean_assert(false && "Unknown priority value.");
	return false;

#else

	switch (priority)
	{
		case PRIORITY_IDLE:
			return setpriority(PRIO_PROCESS, 0, 14) == 0;

		case PRIORITY_BELOW_NORMAL:
			return setpriority(PRIO_PROCESS, 0, 7) == 0;

		case PRIORITY_NORMAL:
			return setpriority(PRIO_PROCESS, 0, 0) == 0;

		case PRIORITY_ABOVE_NORMAL:
			return setpriority(PRIO_PROCESS, 0, -7) == 0;

		case PRIORITY_HIGH:
			return setpriority(PRIO_PROCESS, 0, -14) == 0;

		case PRIORITY_REALTIME:
			return setpriority(PRIO_PROCESS, 0, -20) == 0;
	};

	ocean_assert(false && "Unknown priority value.");
	return false;

#endif

}

float Process::currentProcessorLoad() const
{

#if defined(_WINDOWS)

	FILETIME creationTime, exitTime, kernelTime, userTime;
	const BOOL result = GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
	ocean_assert_and_suppress_unused(result == TRUE, result);

	ocean_assert(sizeof(creationTime) == sizeof(long long));
	const long long processorUserTime(*(long long*)(&userTime));
	const long long processorKernelTime(*(long long*)(&kernelTime));
	const long long processorTime(processorUserTime + processorKernelTime);

	SYSTEMTIME currentSystemTime;
	long long currentTime;
	GetSystemTime(&currentSystemTime);
	SystemTimeToFileTime(&currentSystemTime, (FILETIME*)&currentTime);

	const int64_t oldProcessorLoadTimestamp(loadTimestamp_);
	const int64_t oldProcessorLoadTime(loadTime_);

	loadTimestamp_ = int64_t(currentTime);
	loadTime_ = int64_t(processorTime);

	if (oldProcessorLoadTimestamp == 0ll)
	{
		return 0.0f;
	}

	const int64_t interval = loadTimestamp_ - oldProcessorLoadTimestamp;
	if (interval <= 0ll)
	{
		return 0.0f;
	}

	const int64_t processorInterval = loadTime_ - oldProcessorLoadTime;
	ocean_assert(processorInterval >= 0);

	return float(double(processorInterval) / double(interval * numberCores_));

#else

	struct rusage resourceUsage;
	if (getrusage(RUSAGE_SELF, &resourceUsage) == -1)
	{
		return 0.0;
	}

	const int64_t processorTime = int64_t(resourceUsage.ru_utime.tv_sec + resourceUsage.ru_stime.tv_sec) * 1000000ll + int64_t(resourceUsage.ru_utime.tv_usec + resourceUsage.ru_stime.tv_usec);

	struct timeval currentSystemTime;
	gettimeofday(&currentSystemTime, nullptr);

	const int64_t currentTime(int64_t(currentSystemTime.tv_sec) * 1000000ll + int64_t(currentSystemTime.tv_usec));

	const int64_t oldProcessorLoadTimestamp(loadTimestamp_);
	const int64_t oldProcessorLoadTime(loadTime_);

	loadTimestamp_ = currentTime;
	loadTime_ = processorTime;

	if (oldProcessorLoadTimestamp == 0ll)
	{
		return 0.0f;
	}

	const int64_t interval = loadTimestamp_ - oldProcessorLoadTimestamp;
	if (interval <= 0ll)
	{
		return 0.0f;
	}

	const int64_t processorInterval = loadTime_ - oldProcessorLoadTime;
	ocean_assert(processorInterval >= 0);

	return float(double(processorInterval) / double(interval * numberCores_));

#endif

}

float Process::averageProcessorLoad() const
{

#if defined(_WINDOWS)

	FILETIME creationTime, exitTime, kernelTime, userTime;
	const BOOL result = GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
	ocean_assert_and_suppress_unused(result == TRUE, result);

	ocean_assert(sizeof(creationTime) == sizeof(long long));
	const long long processCreationTime = *(long long*)(&creationTime);

	SYSTEMTIME currentSystemTime;
	long long currentTime;
	GetSystemTime(&currentSystemTime);
	SystemTimeToFileTime(&currentSystemTime, (FILETIME*)&currentTime);

	const double processLiveTime = double(currentTime - processCreationTime);

	const long long processorUserTime(*(long long*)(&userTime));
	const long long processorKernelTime(*(long long*)(&kernelTime));
	const long long processorTime(processorUserTime + processorKernelTime);

	ocean_assert(processLiveTime >= double(processorTime));

	if (processLiveTime <= 0.0)
	{
		return 0.0f;
	}

	return float(double(processorTime) / double(processLiveTime * numberCores_));

#else

	struct rusage resourceUsage;
	if (getrusage(RUSAGE_SELF, &resourceUsage) == -1)
	{
		return 0.0f;
	}

	const long long processorTime = (long long)(resourceUsage.ru_utime.tv_sec + resourceUsage.ru_stime.tv_sec) * 1000000ll + (long long)(resourceUsage.ru_utime.tv_usec + resourceUsage.ru_stime.tv_usec);

	struct timeval currentSystemTime;
	gettimeofday(&currentSystemTime, nullptr);

	const long long currentTime((long long)(currentSystemTime.tv_sec) * 1000000ll + (long long)currentSystemTime.tv_usec);
	const long long processCreationTime = processStartTime();

	if (processCreationTime == -1ll)
	{
		return 0.0f;
	}

	const double processLiveTime = double(currentTime - processCreationTime);

	ocean_assert(processLiveTime >= double(processorTime));

	if (processLiveTime <= 0.0)
	{
		return 0.0f;
	}

	return float(double(processorTime) / double(processLiveTime * numberCores_));

#endif

}

double Process::entireProcessorTime()
{

#if defined(_WINDOWS)

	FILETIME creationTime, exitTime, kernelTime, userTime;
	const BOOL result = GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
	ocean_assert_and_suppress_unused(result == TRUE, result);

	ocean_assert(sizeof(creationTime) == 8);

	const long long processorUserTime(*(long long*)(&userTime));
	const long long processorKernelTime(*(long long*)(&kernelTime));

	return double(processorUserTime + processorKernelTime) * 0.0000001;

#else

	struct rusage resourceUsage;
	if (getrusage(RUSAGE_SELF, &resourceUsage) == -1)
	{
		return -1.0;
	}

	return (double)resourceUsage.ru_stime.tv_sec + (double)resourceUsage.ru_utime.tv_sec + ((double)resourceUsage.ru_utime.tv_usec + (double)resourceUsage.ru_stime.tv_usec) * 0.000001;

#endif

}

double Process::entireProcessLiveTime()
{

#if defined(_WINDOWS)

	FILETIME creationTime, exitTime, kernelTime, userTime;
	const BOOL result = GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
	ocean_assert_and_suppress_unused(result == TRUE, result);

	ocean_assert(sizeof(creationTime) == sizeof(long long));
	const long long processCreationTime = *(long long*)(&creationTime);

	SYSTEMTIME currentSystemTime;
	long long currentTime;
	GetSystemTime(&currentSystemTime);
	SystemTimeToFileTime(&currentSystemTime, (FILETIME*)&currentTime);

	return double(currentTime - processCreationTime) * 0.0000001;

#else

	struct timeval currentSystemTime;
	const int result = gettimeofday(&currentSystemTime, nullptr);
	ocean_assert_and_suppress_unused(result == 0, result);

	const long long currentTime((long long)(currentSystemTime.tv_sec) * 1000000ll + (long long)currentSystemTime.tv_usec);
	const long long processCreationTime = processStartTime();

	if (processCreationTime == -1ll)
	{
		return -1.0;
	}

	return double(currentTime - processCreationTime) * 0.000001;

#endif

}

long long Process::processStartTime()
{
#if defined(_WINDOWS)


	FILETIME creationTime, exitTime, kernelTime, userTime;
	const BOOL result = GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
	ocean_assert_and_suppress_unused(result == TRUE, result);

	ocean_assert(sizeof(creationTime) == sizeof(long long));
	return (long long)(double(*(long long*)(&creationTime)) * 0.1 - 11644473600000000.0);

#elif defined(__APPLE__)

	struct kinfo_proc info;
	size_t size = sizeof(info);

	int managementInformationBase[] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid()};

	if (sysctl(managementInformationBase, 4u, &info, &size, nullptr, 0) == -1)
	{
		return -1ll;
	}

	const struct timeval startTime = info.kp_proc.p_starttime;
	return (long long)(startTime.tv_sec) * 1000000ll + (long long)startTime.tv_usec;

#else

	OCEAN_WARNING_MISSING_IMPLEMENTATION;
	return 0ull;

#endif

}

unsigned int Process::currentProcessId()
{
#if defined(_WINDOWS)

	static_assert(sizeof(DWORD) == sizeof(unsigned int), "Invalid data type!");

	return (unsigned int)GetCurrentProcessId();

#else

	static_assert(sizeof(pid_t) <= sizeof(unsigned int), "Invalid data type!");

	return (unsigned int)getpid();

#endif

}

}

}
