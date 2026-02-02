/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testnetwork/TestResolver.h"

#include "ocean/test/TestResult.h"

#include "ocean/network/Resolver.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestNetwork
{

bool TestResolver::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Resolver test");
	Log::info() << " ";

	if (selector.shouldRun("resolveip4"))
	{
		testResult = testResolveIp4(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

#ifndef OCEAN_NETWORK_DISABLE_RESOLVER_TEST

TEST(TestResolver, ResolveIp4)
{
	EXPECT_TRUE(TestResolver::testResolveIp4(GTEST_TEST_DURATION));
}

#endif // OCEAN_NETWORK_DISABLE_RESOLVER_TEST

#endif // OCEAN_USE_GTEST

bool TestResolver::testResolveIp4(const double /*testDuration*/)
{
	Log::info() << "Resolver::resolveIp4 test:";

	Validation validation;

	const Network::Resolver::Addresses4 localhostAddresses = Network::Resolver::resolveIp4("localhost");

	if (localhostAddresses.empty())
	{
		OCEAN_SET_FAILED(validation);
	}
	else
	{
		bool foundLocalhost = false;

		for (const Network::Address4& address : localhostAddresses)
		{
			if (address == Network::Address4::localHost())
			{
				foundLocalhost = true;
				break;
			}
		}

		OCEAN_EXPECT_TRUE(validation, foundLocalhost);
	}

	const Network::Resolver::Addresses4 invalidAddresses = Network::Resolver::resolveIp4("this.host.should.not.exist.invalid");

	OCEAN_EXPECT_TRUE(validation, invalidAddresses.empty());

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
