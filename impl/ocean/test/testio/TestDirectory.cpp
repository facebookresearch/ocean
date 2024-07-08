/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestDirectory.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Directory.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

bool TestDirectory::test(const double testDuration)
{
	Log::info() << "Directory test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testScopedDirectory(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Entire Directory test succeeded.";
	}
	else
	{
		Log::info() << "Directory test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestDirectory, ScopedDirectory)
{
	EXPECT_TRUE(TestDirectory::testScopedDirectory(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestDirectory::testScopedDirectory(const double testDuration)
{
	Log::info() << "ScopedDirectory test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		std::string directoryPath;

		{
			const IO::ScopedDirectory scopedDirectory(IO::Directory::createTemporaryDirectory());

			if (!scopedDirectory.isValid() || !scopedDirectory.exists())
			{
				allSucceeded = false;
				break;
			}

			const unsigned int numberFiles = RandomI::random(0u, 2u);
			const unsigned int numberDirectories = RandomI::random(0u, 2u);

			if (!createContentInDirectory(scopedDirectory, numberFiles, numberDirectories))
			{
				allSucceeded = false;
			}

			if (!scopedDirectory.exists())
			{
				allSucceeded = false;
			}

			directoryPath = scopedDirectory();
		}

		if (directoryPath.empty() || IO::Directory(directoryPath).exists())
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

bool TestDirectory::createContentInDirectory(const IO::Directory& directory, const unsigned int numberFiles, const unsigned int numberDirectories)
{
	ocean_assert(directory.isValid() && directory.exists());

	for (unsigned int n = 0u; n < numberFiles; ++n)
	{
		const IO::File file(directory + IO::File("file_" + String::toAString(n)));

		if (file.exists())
		{
			return false;
		}
		else
		{
			std::ofstream stream(file().c_str(), std::ios::binary);

			stream << "CONTENT";

			if (!stream.good())
			{
				return false;
			}
		}

		if (!file.exists())
		{
			return false;
		}
	}

	for (unsigned int n = 0u; n < numberDirectories; ++n)
	{
		const IO::Directory subDirectory(directory + IO::Directory("directory_" + String::toAString(n)));

		if (subDirectory.exists())
		{
			return false;
		}
		else
		{
			if (!subDirectory.create())
			{
				return false;
			}
		}

		if (!subDirectory.exists())
		{
			return false;
		}

		ocean_assert(numberDirectories >= 1u);
		if (!createContentInDirectory(subDirectory, numberFiles, numberDirectories - 1u))
		{
			return false;
		}
	}

	return true;
}

}

}

}
