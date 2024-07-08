/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestDescriptor.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/detector/Descriptor.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

bool TestDescriptor::test(const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Descriptor test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testCalculateHammingDistance(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Descriptor test succeeded.";
	}
	else
	{
		Log::info() << "Descriptor test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestDescriptor, CalculateHammingDistance)
{
	EXPECT_TRUE(TestDescriptor::testCalculateHammingDistance(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestDescriptor::testCalculateHammingDistance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test calculateHammingDistance():";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp start(true);

	do
	{
		for (unsigned int nBits = 128u; nBits <= 128u * 5u; nBits += 128u)
		{
			std::vector<uint8_t> descriptorA(nBits / 8u);
			std::vector<uint8_t> descriptorB(nBits / 8u);

			for (uint8_t& valueA : descriptorA)
			{
				valueA = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			for (uint8_t& valueB : descriptorB)
			{
				valueB = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			unsigned int hammingDistance = (unsigned int)(-1);

			switch (nBits)
			{
				case 128u * 1u:
					hammingDistance = CV::Detector::Descriptor::calculateHammingDistance<128u * 1u>(descriptorA.data(), descriptorB.data());
					break;

				case 128u * 2u:
					hammingDistance = CV::Detector::Descriptor::calculateHammingDistance<128u * 2u>(descriptorA.data(), descriptorB.data());
					break;

				case 128u * 3u:
					hammingDistance = CV::Detector::Descriptor::calculateHammingDistance<128u * 3u>(descriptorA.data(), descriptorB.data());
					break;

				case 128u * 4u:
					hammingDistance = CV::Detector::Descriptor::calculateHammingDistance<128u * 4u>(descriptorA.data(), descriptorB.data());
					break;

				case 128u * 5u:
					hammingDistance = CV::Detector::Descriptor::calculateHammingDistance<128u * 5u>(descriptorA.data(), descriptorB.data());
					break;

				default:
					ocean_assert(false && "Invalid bits!");
					allSucceeded = false;
			}

			if (hammingDistance > nBits)
			{
				allSucceeded = false;
			}

			unsigned int counter = 0u;

			ocean_assert(descriptorA.size() == descriptorB.size());
			for (size_t n = 0; n < descriptorA.size(); ++n)
			{
				const uint8_t valueA = descriptorA[n];
				const uint8_t valueB = descriptorB[n];

				const uint8_t xorValue = valueA ^ valueB;

				for (unsigned int i = 0u; i < 8u; ++i)
				{
					if (xorValue & (1u << i))
					{
						++counter;
					}
				}
			}

			if (hammingDistance != counter)
			{
				allSucceeded = false;
			}
		}
	}
	while (Timestamp(true) < start + testDuration);

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Validation succeeded.";
	}
	else
	{
		Log::info() << "Validation FAILED!";
	}

	return allSucceeded;
}

}

}

}

}
