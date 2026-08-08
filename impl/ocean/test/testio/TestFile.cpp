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

	if (selector.shouldRun("name"))
	{
		testResult = testName();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("base"))
	{
		testResult = testBase();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("basename"))
	{
		testResult = testBaseName();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("extension"))
	{
		testResult = testExtension();

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

TEST(TestFile, Name)
{
	EXPECT_TRUE(TestFile::testName());
}

TEST(TestFile, Base)
{
	EXPECT_TRUE(TestFile::testBase());
}

TEST(TestFile, BaseName)
{
	EXPECT_TRUE(TestFile::testBaseName());
}

TEST(TestFile, Extension)
{
	EXPECT_TRUE(TestFile::testExtension());
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

bool TestFile::testName()
{
	Log::info() << "Name test:";

	Validation validation;

	// the paths are relative, an absolute path is platform specific

	const FilenamePairs filenamePairs =
	{
		{"example.bmp", "example.bmp"},
		{"example", "example"},
		{"example.tar.gz", "example.tar.gz"},
		{"second/example.bmp", "example.bmp"},
		{"first/second/example.txt", "example.txt"},
		{"first/second/example", "example"},
		{"first.second/example", "example"},
		{"first.second/example.txt", "example.txt"}
	};

	for (const FilenamePair& filenamePair : filenamePairs)
	{
		const IO::File file(filenamePair.first);

		OCEAN_EXPECT_TRUE(validation, file.isValid());
		OCEAN_EXPECT_EQUAL(validation, file.name(), filenamePair.second);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFile::testBase()
{
	Log::info() << "Base test:";

	Validation validation;

	const FilenamePairs filenamePairs =
	{
		{"example.bmp", "example"},
		{"example", "example"},
		{"example.tar.gz", "example.tar"},
		{"second/example.bmp", "second/example"},
		{"first/second/example.txt", "first/second/example"},
		{"first/second/example", "first/second/example"},
		{"first.second/example", "first.second/example"},
		{"first.second/example.txt", "first.second/example"}
	};

	for (const FilenamePair& filenamePair : filenamePairs)
	{
		const IO::File file(filenamePair.first);

		OCEAN_EXPECT_TRUE(validation, file.isValid());
		OCEAN_EXPECT_EQUAL(validation, file.base(), filenamePair.second);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFile::testBaseName()
{
	Log::info() << "Base name test:";

	Validation validation;

	const FilenamePairs filenamePairs =
	{
		{"example.bmp", "example"},
		{"example", "example"},
		{"example.tar.gz", "example.tar"},
		{"second/example.bmp", "example"},
		{"first/second/example.txt", "example"},
		{"first/second/example", "example"},
		{"first.second/example", "example"},
		{"first.second/example.txt", "example"}
	};

	for (const FilenamePair& filenamePair : filenamePairs)
	{
		const IO::File file(filenamePair.first);

		OCEAN_EXPECT_TRUE(validation, file.isValid());
		OCEAN_EXPECT_EQUAL(validation, file.baseName(), filenamePair.second);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFile::testExtension()
{
	Log::info() << "Extension test:";

	Validation validation;

	const FilenamePairs filenamePairs =
	{
		{"example.bmp", "bmp"},
		{"example", ""},
		{"example.tar.gz", "gz"},
		{"second/example.bmp", "bmp"},
		{"first/second/example.txt", "txt"},
		{"first/second/example", ""},
		{"first.second/example", ""},
		{"first.second/example.txt", "txt"}
	};

	for (const FilenamePair& filenamePair : filenamePairs)
	{
		const IO::File file(filenamePair.first);

		OCEAN_EXPECT_TRUE(validation, file.isValid());
		OCEAN_EXPECT_EQUAL(validation, file.extension(), filenamePair.second);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
