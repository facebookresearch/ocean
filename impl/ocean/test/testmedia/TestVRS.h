// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_TEST_TESTMEDIA_TEST_VRS_H
#define META_OCEAN_TEST_TESTMEDIA_TEST_VRS_H

#include "ocean/test/testmedia/TestMedia.h"

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

/**
 * This class implements a test for the VRS media library.
 * @ingroup testmedia
 */
class OCEAN_TEST_MEDIA_EXPORT TestVRS
{
	public:

		/**
		 * Invokes all tests that are defined.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

#ifndef OCEAN_DO_NOT_TEST_VRS

		/**
		 * Tests the read and write functions for BMP images.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testFlexibleRecorder(const double testDuration);

#endif // OCEAN_DO_NOT_TEST_VRS

};

}

}

}

#endif // META_OCEAN_TEST_TESTMEDIA_TEST_IMAGE_IO_H
