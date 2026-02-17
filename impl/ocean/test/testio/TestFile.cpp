/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestFile.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

bool TestFile::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("File test");
	Log::info() << " ";

	if (selector.shouldRun("fileexists"))
	{
		testResult = testFileExists(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFile, FileExists)
{
	EXPECT_TRUE(TestFile::testFileExists(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFile::testFileExists(const double testDuration)
{
	Log::info() << "File exists test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		IO::Files files;

		{
			const IO::ScopedDirectory scopedDirectory(IO::Directory::createTemporaryDirectory());

			if (!scopedDirectory.isValid() || !scopedDirectory.exists())
			{
				OCEAN_SET_FAILED(validation);
				break;
			}

			const unsigned int numberFiles = RandomI::random(randomGenerator, 1u, 20u);

			for (unsigned int n = 0u; n < numberFiles; ++n)
			{
				IO::File newFile(scopedDirectory + IO::File("file_" + String::toAString(n)));

				OCEAN_EXPECT_FALSE(validation, newFile.exists());

				{
					std::ofstream stream(newFile().c_str(), std::ios::binary);

					stream << "CONTENT";

					OCEAN_EXPECT_TRUE(validation, stream.good());
				}

				OCEAN_EXPECT_TRUE(validation, newFile.exists());

				files.emplace_back(std::move(newFile));
			}
		}

		OCEAN_EXPECT_FALSE(validation, files.empty());

		for (const IO::File& file : files)
		{
			OCEAN_EXPECT_FALSE(validation, file.exists());
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
