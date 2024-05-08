/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTPLATFORM_TEST_APPLE_H
#define META_OCEAN_TEST_TESTPLATFORM_TEST_APPLE_H

#include "ocean/test/testplatform/TestPlatform.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Test
{

namespace TestPlatform
{

/**
 * This class implements a test for the Apple library.
 * @ingroup testplatform
 */
class OCEAN_TEST_PLATFORM_EXPORT TestApple
{
	public:

		/**
		 * Invokes all tests that are defined.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the toCGImage() function.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testToCGImage(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTPLATFORM_TEST_APPLE_H
