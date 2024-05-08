/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TEST_DIRECTORY_H
#define META_OCEAN_TEST_TESTIO_TEST_DIRECTORY_H

#include "ocean/test/testio/TestIO.h"

#include "ocean/io/Directory.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

/**
 * This class implements tests for the Directory class.
 * @ingroup testio
 */
class OCEAN_TEST_IO_EXPORT TestDirectory
{
	public:

		/**
		 * Invokes all tests.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the ScopedDirectory object.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testScopedDirectory(const double testDuration);

	protected:

		/**
		 * Creates random files and random sub-directories in a given directory.
		 * @param directory The directory in which the content will be created
		 * @param numberFiles The number of files to create
		 * @param numberDirectories The number of sub-directories to create
		 * @return True, if succeeded
		 */
		static bool createContentInDirectory(const IO::Directory& directory, const unsigned int numberFiles, const unsigned int numberDirectories);
};

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TEST_DIRECTORY_H
