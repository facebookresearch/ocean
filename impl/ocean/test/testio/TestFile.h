/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TEST_FILE_H
#define META_OCEAN_TEST_TESTIO_TEST_FILE_H

#include "ocean/test/testio/TestIO.h"

#include "ocean/test/TestSelector.h"

#include <string>
#include <utility>
#include <vector>

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
class OCEAN_TEST_IO_EXPORT TestFile
{
	protected:

		/**
		 * Definition of a pair combining a filename with the expected result.
		 */
		using FilenamePair = std::pair<std::string, std::string>;

		/**
		 * Definition of a vector holding pairs of filenames and expected results.
		 */
		using FilenamePairs = std::vector<FilenamePair>;

	public:

		/**
		 * Invokes all tests.
		 * @param testDuration The number of seconds for each test
		 * @param selector The test selector
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the exists function.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testFileExists(const double testDuration);

		/**
		 * Tests the name function.
		 * @return True, if succeeded
		 */
		static bool testName();

		/**
		 * Tests the base function.
		 * @return True, if succeeded
		 */
		static bool testBase();

		/**
		 * Tests the base name function.
		 * @return True, if succeeded
		 */
		static bool testBaseName();

		/**
		 * Tests the extension function.
		 * @return True, if succeeded
		 */
		static bool testExtension();
};

}

}

}

#endif // META_OCEAN_TEST_TESTIO_TEST_FILE_H
