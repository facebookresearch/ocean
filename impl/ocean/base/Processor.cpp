/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Processor.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/String.h"
#include "ocean/base/Utilities.h"

#if defined(_WINDOWS)
	#include <winsock2.h>
	#include <windows.h>
	#include <intrin.h>
#elif defined (__APPLE__)
	#include <sys/sysctl.h>
	#include <unistd.h>
#else
	#include <unistd.h>
#endif

namespace Ocean
{

Processor::Processor()
{
	processorInstructions_ = realInstructions();
}

bool Processor::forceCores(const unsigned int cores)
{
	const ScopedLock scopedLock(lock_);

	Log::info() << "Forcing " << cores << " CPU cores to be used.";

	forcedCores_ = cores;
	return true;
}

bool Processor::forceInstructions(const ProcessorInstructions instructions)
{
	const ScopedLock scopedLock(lock_);

	Log::info() << "Forcing the instruction set: " << (unsigned int)instructions;

	forcedProcessorInstructions_ = instructions;
	return true;
}

std::string Processor::brand()
{

#if defined(_WINDOWS)

	static_assert(sizeof(char) == 1, "Invalid char data type!");
	static_assert(sizeof(int) == 4, "Invalid integer data type!");

	char brandString[3 * 4 * 4 + 1];

	__cpuid((int*)brandString + 0 * 4, 0x80000002);
	__cpuid((int*)brandString + 1 * 4, 0x80000003);
	__cpuid((int*)brandString + 2 * 4, 0x80000004);
	brandString[3 * 4 * 4] = 0;

	return String::trimWhitespace(std::string(brandString));

#elif defined(__APPLE__)

	#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE==1

		return deviceModelAppleIOS();

	#else

		char buffer[1025] = {'\0'};
		size_t bufferSize = 1024;

		const int result = sysctlbyname("machdep.cpu.brand_string", buffer, &bufferSize, nullptr, 0);
		ocean_assert(result == 0);

		if (bufferSize >= 1024)
		{
			ocean_assert(false && "This should never happen!");
			return std::string("Unknown Processor");
		}

		return String::trimWhitespace(std::string(buffer));

	#endif // TARGET_OS_IPHONE==1

#else

	std::ifstream stream;
	stream.open(std::string("/proc/cpuinfo").c_str(), std::ios::binary);
	if (stream.good())
	{
		std::vector<char> buffer(1024 * 10 + 1);

		do
		{
			buffer[0] = '\0';
			stream.getline(buffer.data(), buffer.size() - 1);

			const std::string line(buffer.data());

			if (line.find("Hardware") == 0)
			{
				const std::string::size_type pos = line.find(':');
				return String::trimWhitespace(line.substr(pos + 1));
			}
		}
		while (stream.good());
	}

	return std::string("Unknown Processor");

#endif

}

unsigned int Processor::realCores()
{

#if defined(_WINDOWS)

	SYSTEM_INFO info;

	#if defined(_WIN64)

		GetSystemInfo(&info);

	#else

		BOOL isWow = FALSE;
		if (IsWow64Process(GetCurrentProcess(), &isWow) == TRUE && isWow == TRUE)
		{
			GetNativeSystemInfo(&info);
		}
		else
		{
			GetSystemInfo(&info);
		}

	#endif // _WIN64

	return info.dwNumberOfProcessors;

#elif defined(_ANDROID)

#ifdef OCEAN_SUPPORT_EXCEPTIONS

	try
	{

#endif // OCEAN_SUPPORT_EXCEPTIONS

		// kernel_max: the maximum cpu index allowed by the kernel configuration

		std::ifstream stream;
		stream.open("/sys/devices/system/cpu/kernel_max", std::ios::binary);
		if (stream.fail())
		{
			Log::error() << "Failed to receive cpu number.";
			return 1u;
		}

		int indexValue = 0;
		stream >> indexValue;
		ocean_assert(indexValue >= 0);

		// we clamp the number of cores to the range [1, 4] as e.g., on a Samsung Galaxy S6 we receive 8 as maximal number of cores (due to the 2x 4 cores)
		const unsigned int maximalCores = minmax<int>(1, indexValue + 1, 4);

		return maximalCores;

#ifdef OCEAN_SUPPORT_EXCEPTIONS

	}
	catch(...)
	{
		Log::error() << "Failed to receive cpu number.";
		return 1u;
	}

#endif // OCEAN_SUPPORT_EXCEPTIONS

#elif defined(__APPLE__)

	return realCoresApple();

#else

	const long cores = sysconf(_SC_NPROCESSORS_ONLN);

	static_assert(sizeof(long) >= sizeof(unsigned int), "Invalid data type!");
	ocean_assert(cores < (long)std::numeric_limits<unsigned int>::max());

	return (unsigned int)cores;

#endif

}

ProcessorInstructions Processor::realInstructions()
{
	ProcessorInstructions instructions(PI_NONE);

#if defined(_WINDOWS)

	int info[4] = {0, 0, 0, 0};

	__cpuid(info, 0);
	const int functionIds = info[0];

	if (functionIds >= 1)
	{
		__cpuid(info, 1);

		//const int eax = info[0];
		//const int ebx = info[1];
		const int ecx = info[2];
		const int edx = info[3];

		if (edx & (1 << 25))
		{
			instructions = ProcessorInstructions(instructions | PI_SSE);
		}

		if (edx & (1 << 26))
		{
			instructions = ProcessorInstructions(instructions | PI_SSE_2);
		}

		if (ecx & (1 << 0))
		{
			instructions = ProcessorInstructions(instructions | PI_SSE_3);
		}

		if (ecx & (1 << 9))
		{
			instructions = ProcessorInstructions(instructions | PI_SSSE_3);
		}

		if (ecx & (1 << 19))
		{
			instructions = ProcessorInstructions(instructions | PI_SSE_4_1);
		}

		if (ecx & (1 << 20))
		{
			instructions = ProcessorInstructions(instructions | PI_SSE_4_2);
		}

		if (ecx & (1 << 25))
		{
			instructions = ProcessorInstructions(instructions | PI_AES);
		}

		if (ecx & (1 << 28))
		{
			instructions = ProcessorInstructions(instructions | PI_AVX);
		}
	}

	if (functionIds >= 7)
	{
		__cpuid(info, 7);

		//const int eax = info[0];
		const int ebx = info[1];
		//const int ecx = info[2];
		//const int edx = info[3];

		if (ebx & (1 << 5))
		{
			instructions = ProcessorInstructions(instructions | PI_AVX_2);
		}

		if ((ebx & (1 << 16)) != 0 && (ebx & (1 << 26)) != 0 && (ebx & (1 << 27)) != 0 && (ebx & (1 << 28)) != 0)
		{
			instructions = ProcessorInstructions(instructions | PI_AVX_512);
		}
	}

#elif defined(__APPLE__)

	#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		return PI_NEON;

	#else

		const size_t maxBufferSize = 2048;
		char buffer[maxBufferSize + 1] = {'\0'};
		size_t bufferSize = maxBufferSize;

		const int result = sysctlbyname("machdep.cpu.features", buffer, &bufferSize, nullptr, 0);
		ocean_assert_and_suppress_unused(result == 0, result);

		if (bufferSize >= maxBufferSize)
		{
			ocean_assert(false && "This should never happen!");
			return PI_NONE;
		}

		std::stringstream stream(buffer);
		while (stream)
		{
			std::string feature;
			stream >> feature;

			if (feature == "SSE")
			{
				instructions = ProcessorInstructions(instructions | PI_SSE);
			}
			else if (feature == "SSE2")
			{
				instructions = ProcessorInstructions(instructions | PI_SSE_2);
			}
			else if (feature == "SSE3")
			{
				instructions = ProcessorInstructions(instructions | PI_SSE_3);
			}
			else if (feature == "SSSE3")
			{
				instructions = ProcessorInstructions(instructions | PI_SSSE_3);
			}
			else if (feature == "SSE4.1")
			{
				instructions = ProcessorInstructions(instructions | PI_SSE_4_1);
			}
			else if (feature == "SSE4.2")
			{
				instructions = ProcessorInstructions(instructions | PI_SSE_4_2);
			}
			else if (feature == "AVX1.0")
			{
				instructions = ProcessorInstructions(instructions | PI_AVX);
			}
			else if (feature == "AVX2")
			{
				instructions = ProcessorInstructions(instructions | PI_AVX_2);
			}
			else if (feature == "AES")
			{
				instructions = ProcessorInstructions(instructions | PI_AES);
			}
		}

		return instructions;

	#endif

#elif defined(__linux__) && !defined(_ANDROID)

	std::ifstream stream;
	stream.open(std::string("/proc/cpuinfo").c_str(), std::ios::binary);
	if (stream.good())
	{
		std::string line;
		const std::string flagsKey = "flags";

		while (std::getline(stream, line))
		{
			if (line.compare(0, flagsKey.length(), flagsKey) != 0)
			{
				continue;
			}

			std::stringstream buffer(line);
			std::string flag;

			while (buffer >> flag)
			{
				if (flag == "sse")
				{
					instructions = ProcessorInstructions(instructions | PI_SSE);
				}
				else if (flag == "sse2")
				{
					instructions = ProcessorInstructions(instructions | PI_SSE_2);
				}
				else if (flag == "sse3")
				{
					instructions = ProcessorInstructions(instructions | PI_SSE_3);
				}
				else if (flag == "ssse3")
				{
					instructions = ProcessorInstructions(instructions | PI_SSSE_3);
				}
				else if (flag == "sse4_1")
				{
					instructions = ProcessorInstructions(instructions | PI_SSE_4_1);
				}
				else if (flag == "sse4_2")
				{
					instructions = ProcessorInstructions(instructions | PI_SSE_4_2);
				}
				else if (flag == "avx")
				{
					instructions = ProcessorInstructions(instructions | PI_AVX);
				}
				else if (flag == "avx2")
				{
					instructions = ProcessorInstructions(instructions | PI_AVX_2);
				}
				else if (flag == "aes")
				{
					instructions = ProcessorInstructions(instructions | PI_AES);
				}
			}
		}
	}

#elif defined(_ANDROID)

	std::ifstream stream;
	stream.open(std::string("/proc/cpuinfo").c_str(), std::ios::binary);
	if (stream.good())
	{
		std::vector<char> buffer(1024 * 10 + 1);

		do
		{
			buffer[0] = '\0';
			stream.getline(buffer.data(), buffer.size() - 1);

			const std::string line(buffer.data());

#ifdef __aarch64__
			if (line.find("Features") == 0 && line.find("asimd") != std::string::npos)
#else
			if (line.find("Features") == 0 && line.find("neon") != std::string::npos)
#endif
			{
				instructions = ProcessorInstructions(instructions | PI_NEON);
				break;
			}

			if (line.find("Features") == 0 && line.find("aes") != std::string::npos)
			{
				instructions = ProcessorInstructions(instructions | PI_AES);
				break;
			}
		}
		while (stream.good());
	}

#endif

	return instructions;
}

std::string Processor::translateInstructions(const ProcessorInstructions instructions)
{
	std::string result;

	if ((instructions & PI_SSE_ANY) == PI_SSE_ANY)
	{
		result += "Full SSE Support (SSE1 - SSE4.2), ";
	}
	else
	{
		if (instructions & PI_SSE)
		{
			result += "SSE, ";
		}

		if (instructions & PI_SSE_2)
		{
			result += "SSE2, ";
		}

		if (instructions & PI_SSE_3)
		{
			result += "SSE3, ";
		}

		if (instructions & PI_SSSE_3)
		{
			result += "SSSE3, ";
		}

		if (instructions & PI_SSE_4_1)
		{
			result += "SSE4.1, ";
		}

		if (instructions & PI_SSE_4_2)
		{
			result += "SSE4.2, ";
		}
	}

	if ((instructions & PI_AVX_ANY) == PI_AVX_ANY)
	{
		result += "Full AVX Support (AVX - AVX512), ";
	}
	else
	{
		if (instructions & PI_AVX)
		{
			result += "AVX, ";
		}

		if (instructions & PI_AVX_2)
		{
			result += "AVX2, ";
		}

		if (instructions & PI_AVX_512)
		{
			result += "AVX512, ";
		}
	}

	if ((instructions & PI_NEON_ANY) == PI_NEON_ANY)
	{
		result += "Full NEON Support (NEON), ";
	}
	else
	{
		if (instructions & PI_NEON)
		{
			result += "NEON, ";
		}
	}

	if (instructions & PI_AES)
	{
		result += "AES, ";
	}

	if (result.empty())
	{
		ocean_assert(instructions == PI_NONE);
		result = "No SIMD Instructions";
	}
	else
	{
		ocean_assert(result.size() > 2 && result[result.size() - 2] == ',' && result[result.size() - 1] == ' ');

		if (result.size() > 2 && result[result.size() - 2] == ',' && result[result.size() - 1] == ' ')
		{
			result.pop_back();
			result.pop_back();
		}
	}

	return result;
}

}
