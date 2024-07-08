/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTGEOMETRY_TEST_JLINKAGE_H
#define META_OCEAN_TEST_TESTGEOMETRY_TEST_JLINKAGE_H

#include "ocean/test/testgeometry/TestGeometry.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

/**
 * This class implements homography tests using J-Linkage.
 * @ingroup testgeometry
 */
class OCEAN_TEST_GEOMETRY_EXPORT TestJLinkage
{
	public:

		/**
		 * Definition of the linkage method.
		 */
		enum LinkageMethod
		{
			/// JLinkage method.
			LM_JLINKAGE,
			/// TLinkage method.
			LM_TLINKAGE
		};

		/**
		 * Tests all J-Linkage functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testJLinkage(const double testDuration);

		/**
		 * Tests all T-Linkage functions.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testTLinkage(const double testDuration);

	protected:

		/**
		 * Tests a single faultless homography.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tLinkageType Defines the linkage method
		 */
		template <LinkageMethod tLinkageType>
		static bool testFaultlessSingleHomography(const double testDuration);

		/**
		 * Tests the 3D planar 2D-2D homography.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)		 
		 * @return True, if succeeded
		 * @tparam tLinkageType Defines the linkage method
		 */
		template <LinkageMethod tLinkageType>
		static bool testFaultlessNoisedSingleHomography(const double testDuration);

		/**
		 * Tests multiple faultless homographies.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)		 
		 * @return True, if succeeded
		 * @tparam tLinkageType Defines the linkage method
		 */
		template <LinkageMethod tLinkageType>
		static bool testFaultlessMultipleHomography(const double testDuration);

		/**
		 * Tests multiple faultless lines.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)		 
		 * @return True, if succeeded
		 * @tparam tLinkageType Defines the linkage method
		 */
		template <LinkageMethod tLinkageType>
		static bool testFaultlessLines(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTGEOMETRY_TEST_JLINKAGE_H
