/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/base/sharedmemory/client/SharedMemoryClient.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/SharedLock.h"
#include "ocean/base/SharedMemory.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

using namespace Ocean;

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int /*argc*/, wchar_t** /*argv*/)
#elif defined(__APPLE__) || defined(__linux__)
	// main function on OSX platforms
	int main(int /*argc*/, char** /*argv*/)
#else
	#error Missing implementation.
#endif
{
	static_assert(sizeof(unsigned long long) == 8, "Invalid sizeof unsigned long long");

	static const std::wstring uniqueMemoryName(L"_OCEAN_BASE_SHAREDMEMORY_memory");
	static const std::wstring uniqueLockName(L"_OCEAN_BASE_SHAREDMEMORY_lock");

	std::cout << "Client has been started." << std::endl;

	SharedMemory sharedMemory(uniqueMemoryName, 8 * 2);
	SharedLock sharedLock(uniqueLockName);

	if (sharedMemory)
	{
		std::cout << "Shared memory opened." << std::endl;

		const Timestamp startTimestamp(true);

		unsigned long long data = 0ull;

		while (startTimestamp + 600.0 > Timestamp(true))
		{
			{
				const ScopedSharedLock scopedLock(sharedLock);

				const unsigned long long newData = ((unsigned long long*)sharedMemory.data())[0];
				const unsigned long long ticks = ((unsigned long long*)sharedMemory.data())[1];

				if (data != newData)
				{
					const unsigned long long currentTicks = HighPerformanceTimer::ticks();

					data = newData;
					std::cout << "Value has been changed to " << data << ", with time delay: " << HighPerformanceTimer::ticks2seconds(currentTicks - ticks) * 1000.0 << "ms" << std::endl;
				}
			}

			Thread::sleep(1u);
		}
	}
	else
		std::cout << "FAILED to open the shared memory!" << std::endl;

	std::cout << "Press a key to exit" << std::endl;
	getchar();

	return 0;
}
