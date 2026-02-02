/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTNETWORK_TEST_RESOLVER_H
#define META_OCEAN_TEST_TESTNETWORK_TEST_RESOLVER_H

#include "ocean/test/testnetwork/TestNetwork.h"

#include "ocean/test/TestSelector.h"

namespace Ocean
{

namespace Test
{

namespace TestNetwork
{

/**
 * This class implements tests for Resolver.
 * @ingroup testnetwork
 */
class OCEAN_TEST_NETWORK_EXPORT TestResolver
{
	public:

		/**
		 * Tests all Resolver functions.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @param selector The selector defining which tests to run
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, const TestSelector& selector);

		/**
		 * Tests the resolveIp4 function.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testResolveIp4(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTNETWORK_TEST_RESOLVER_H
