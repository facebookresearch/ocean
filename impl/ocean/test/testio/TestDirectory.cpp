/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/TestDirectory.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Directory.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

bool TestDirectory::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("Directory test");
	Log::info() << " ";

	if (selector.shouldRun("scopeddirectory"))
	{
		testResult = testScopedDirectory(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
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

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		std::string directoryPath;

		{
			const IO::ScopedDirectory scopedDirectory(IO::Directory::createTemporaryDirectory());

			if (!scopedDirectory.isValid() || !scopedDirectory.exists())
			{
				OCEAN_SET_FAILED(validation);
				break;
			}

			const unsigned int numberFiles = RandomI::random(randomGenerator, 0u, 2u);
			const unsigned int numberDirectories = RandomI::random(randomGenerator, 0u, 2u);

			OCEAN_EXPECT_TRUE(validation, createContentInDirectory(scopedDirectory, numberFiles, numberDirectories));

			OCEAN_EXPECT_TRUE(validation, scopedDirectory.exists());

			directoryPath = scopedDirectory();
		}

		OCEAN_EXPECT_TRUE(validation, !directoryPath.empty() && !IO::Directory(directoryPath).exists());
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
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
