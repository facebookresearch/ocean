/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestFile.h"

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

bool TestFile::test(const double testDuration)
{
	Log::info() << "File test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testFileExists(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Entire File test succeeded.";
	}
	else
	{
		Log::info() << "File test FAILED!";
	}

	return allSucceeded;
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

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		IO::Files files;

		{
			const IO::ScopedDirectory scopedDirectory(IO::Directory::createTemporaryDirectory());

			if (!scopedDirectory.isValid() || !scopedDirectory.exists())
			{
				allSucceeded = false;
				break;
			}

			const unsigned int numberFiles = RandomI::random(1u, 20u);

			for (unsigned int n = 0u; n < numberFiles; ++n)
			{
				IO::File newFile(scopedDirectory + IO::File("file_" + String::toAString(n)));

				if (newFile.exists())
				{
					allSucceeded = false;
				}

				{
					std::ofstream stream(newFile().c_str(), std::ios::binary);

					stream << "CONTENT";

					if (!stream.good())
					{
						allSucceeded = false;
					}
				}

				if (!newFile.exists())
				{
					allSucceeded = false;
				}

				files.emplace_back(std::move(newFile));
			}
		}

		if (files.empty())
		{
			allSucceeded = false;
		}
		else
		{
			for (const IO::File& file : files)
			{
				if (file.exists())
				{
					allSucceeded = false;
				}
			}
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
