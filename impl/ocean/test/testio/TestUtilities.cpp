/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestUtilities.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"
#include "ocean/io/Utilities.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

bool TestUtilities::test(const double testDuration)
{
	Log::info() << "Utilities test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testReadFile(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Entire Utilities test succeeded.";
	}
	else
	{
		Log::info() << "Utilities test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestUtilities, ReadFile)
{
	EXPECT_TRUE(TestUtilities::testReadFile(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestUtilities::testReadFile(const double testDuration)
{
	Log::info() << "Read file test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const IO::ScopedDirectory scopedDirectory(IO::Directory::createTemporaryDirectory());

		if (scopedDirectory.exists())
		{
			const IO::File file(scopedDirectory + IO::File(String::toAString(RandomI::random32(randomGenerator))));

			if (!file.exists())
			{
				const unsigned int size = RandomI::random(randomGenerator, 10000u); // size may also be 0

				std::vector<uint8_t> writeBuffer(size);

				for (uint8_t& value : writeBuffer)
				{
					value = uint8_t(RandomI::random(randomGenerator, 255u));
				}

				{
					std::ofstream stream(file(), std::ios::binary);

					stream.write((const char*)(writeBuffer.data()), writeBuffer.size());

					if (!stream.good())
					{
						allSucceeded = false;
					}
				}

				IO::Utilities::Buffer readBuffer;

				if (IO::Utilities::readFile(file(), readBuffer))
				{
					if (readBuffer.size() == writeBuffer.size())
					{
						if (!readBuffer.empty() && memcmp(readBuffer.data(), writeBuffer.data(), readBuffer.size()) != 0)
						{
							allSucceeded = false;
						}
					}
					else
					{
						allSucceeded = false;
					}
				}
				else
				{
					allSucceeded = false;
				}
			}
			else
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

}

}

}
