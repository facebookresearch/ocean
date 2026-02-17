/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestUtilities.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

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

bool TestUtilities::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("Utilities test");
	Log::info() << " ";

	if (selector.shouldRun("readfile"))
	{
		testResult = testReadFile(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const IO::ScopedDirectory scopedDirectory(IO::Directory::createTemporaryDirectory());

		OCEAN_EXPECT_TRUE(validation, scopedDirectory.exists());

		if (scopedDirectory.exists())
		{
			const IO::File file(scopedDirectory + IO::File(String::toAString(RandomI::random32(randomGenerator))));

			OCEAN_EXPECT_FALSE(validation, file.exists());

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

					OCEAN_EXPECT_TRUE(validation, stream.good());
				}

				IO::Utilities::Buffer readBuffer;

				OCEAN_EXPECT_TRUE(validation, IO::Utilities::readFile(file(), readBuffer));

				OCEAN_EXPECT_EQUAL(validation, readBuffer.size(), writeBuffer.size());

				if (readBuffer.size() == writeBuffer.size() && !readBuffer.empty())
				{
					OCEAN_EXPECT_TRUE(validation, memcmp(readBuffer.data(), writeBuffer.data(), readBuffer.size()) == 0);
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
