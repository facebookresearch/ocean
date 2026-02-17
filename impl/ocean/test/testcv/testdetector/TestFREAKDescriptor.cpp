/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestFREAKDescriptor.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/FramePyramid.h"

#include "ocean/geometry/Jacobian.h"

#include <bitset>
#include <cmath>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

using namespace CV::Detector;

template <size_t tSize>
bool TestFREAKDescriptorT<tSize>::test(const double testDuration, Worker& worker, const TestSelector& selector)
{
	static_assert(tSize == 32 || tSize == 64, "The FREAK descriptor test is only defined for 32 and 64 bytes descriptor lengths");

	ocean_assert(testDuration > 0.0);

	TestResult testResult("FREAK descriptor test (" + std::string(tSize == 32 ? "32" : "64") + " bytes)");
	Log::info() << " ";

	if (selector.shouldRun("createblurredframepyramid"))
	{
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";

		testResult = testCreateBlurredFramePyramid(testDuration, worker);
	}

#ifdef OCEAN_USE_EXTERNAL_TEST_FREAK_DESCRIPTOR

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	testResult = TestFREAKDescriptorT_externalTests<tSize>(testDuration, worker);

#endif // OCEAN_USE_EXTERNAL_TEST_FREAK_DESCRIPTOR

	Log::info() << " ";
	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

// 32-byte FREAK

#define GTEST_FREAK_MAX_HAMMING_DISTANCE 1u

TEST(TestFREAKDescriptor32, CreateBlurredFramePyramid)
{
	Worker worker;
	EXPECT_TRUE(TestFREAKDescriptor32::testCreateBlurredFramePyramid(GTEST_TEST_DURATION, worker));
}

// 64-byte FREAK

TEST(TestFREAKDescriptor64, CreateBlurredFramePyramid)
{
	Worker worker;
	EXPECT_TRUE(TestFREAKDescriptor64::testCreateBlurredFramePyramid(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

template <size_t tSize>
bool TestFREAKDescriptorT<tSize>::testCreateBlurredFramePyramid(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing creation of blurred frame pyramid:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTime(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 20u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 20u, 2000u);

		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const Frame yFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, pixelOrigin), &randomGenerator);

		const unsigned int kernelWidth = RandomI::random(randomGenerator, 1u, 15u) | 0x01u; // ensuring that the kernel is odd
		const unsigned int kernelHeight = RandomI::random(randomGenerator, 1u, 15u) | 0x01u;

		Worker* useWorker = RandomI::boolean(randomGenerator) ? &worker : nullptr;

		const unsigned int maximalLayers = CV::FramePyramid::idealLayers(width, height, 0u);

		const unsigned int layers = RandomI::random(randomGenerator, 1u, maximalLayers);

		const CV::FramePyramid blurredFramePyramid = FREAKDescriptorT<tSize>::createFramePyramidWithBlur8BitsPerChannel(yFrame, kernelWidth, kernelHeight, layers, useWorker);

		if (blurredFramePyramid.layers() == layers)
		{
			// the first layer must be identical

			for (unsigned int y = 0u; y < yFrame.height(); ++y)
			{
				OCEAN_EXPECT_EQUAL(validation, memcmp(yFrame.constrow<void>(y), blurredFramePyramid.finestLayer().constrow<void>(y), yFrame.planeWidthBytes(0u)), 0);
			}

			// the remaining layers are based on the blurred version of the finer pyramid layer and then downsampled

			Frame finerLayer(yFrame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

			for (unsigned int layerIndex = 1u; layerIndex < blurredFramePyramid.layers(); ++layerIndex)
			{
				if (kernelWidth <= finerLayer.width() && kernelHeight <= finerLayer.height()) // we skip the blur if the layer is already too small
				{
					OCEAN_EXPECT_TRUE(validation, CV::FrameFilterGaussian::filter<uint8_t, uint32_t>(finerLayer.data<uint8_t>(), finerLayer.width(), finerLayer.height(), finerLayer.channels(), finerLayer.paddingElements(), kernelWidth, kernelHeight, -1.0f, &worker));
				}

				const CV::FramePyramid twoLayerPyramid(CV::FramePyramid::DM_FILTER_11, std::move(finerLayer), 2u, &worker);

				const Frame& blurredFrameCoarserLayer = blurredFramePyramid[layerIndex];
				Frame testCoarserLayer(twoLayerPyramid.coarsestLayer(), Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

				ocean_assert(blurredFrameCoarserLayer.isFrameTypeCompatible(testCoarserLayer, false));

				for (unsigned int y = 0u; y < blurredFrameCoarserLayer.height(); ++y)
				{
					OCEAN_EXPECT_EQUAL(validation, memcmp(testCoarserLayer.constrow<void>(y), blurredFrameCoarserLayer.constrow<void>(y), testCoarserLayer.planeWidthBytes(0u)), 0);
				}

				finerLayer = std::move(testCoarserLayer);
			}
		}
		else
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (startTime + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

// Explicit instantiations of the test of the FREAK descriptor class
template class TestFREAKDescriptorT<32>;
template class TestFREAKDescriptorT<64>;

} // namespace TestDetector

} // namespace TestCV

} // namespace Test

} // namespace Ocean
