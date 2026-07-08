/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifdef OCEAN_USE_GTEST

#include <gtest/gtest.h>

#include "ocean/cv/advanced/AdvancedFrameFilterGaussian.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include <cstdint>
#include <cstring>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

namespace
{

/**
 * Fills a single-plane frame with a uniform pixel value.
 * Iterates over rows (respecting padding) and writes @p value into every
 * element of every channel for every pixel.
 */
template <typename T>
void fillUniform(Frame& frame, const T value)
{
	const unsigned int width = frame.width();
	const unsigned int height = frame.height();
	const unsigned int channels = frame.channels();

	for (unsigned int y = 0u; y < height; ++y)
	{
		T* row = frame.row<T>(y);
		for (unsigned int i = 0u; i < width * channels; ++i)
		{
			row[i] = value;
		}
	}
}

/**
 * Fills a single-channel uint8 mask frame with a uniform value.
 */
void fillMaskUniform(Frame& mask, const uint8_t value)
{
	for (unsigned int y = 0u; y < mask.height(); ++y)
	{
		uint8_t* row = mask.row<uint8_t>(y);
		std::memset(row, value, mask.width());
	}
}

/**
 * Returns true if every pixel channel in @p frame equals @p expected.
 */
template <typename T>
bool allPixelsEqual(const Frame& frame, const T expected)
{
	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		const T* row = frame.constrow<T>(y);
		for (unsigned int i = 0u; i < frame.width() * frame.channels(); ++i)
		{
			if (row[i] != expected)
			{
				return false;
			}
		}
	}
	return true;
}

} // namespace

// Integration tests for AdvancedFrameFilterGaussian::Comfort — the wrapper
// class that dispatches at runtime on Frame::dataType(). Each test exercises
// the interaction between the Comfort wrapper (component 1) and the underlying
// templated AdvancedFrameFilterGaussian implementation (component 2), routed
// through the Frame/FrameType/PixelFormat data-model components.

class AdvancedFrameFilterGaussianComfortIntegrationTest : public ::testing::Test
{
	protected:
		static constexpr unsigned int kWidth = 32u;
		static constexpr unsigned int kHeight = 24u;
		static constexpr uint8_t kValidMaskValue = 0xFFu;
		static constexpr uint8_t kInvalidMaskValue = 0x00u;

		Worker worker_;
};

// Component interaction: Comfort::filter (in-place, uint8) → templated
// AdvancedFrameFilterGaussian::filter → Frame element access.
// A uniform input with fully-valid mask must remain uniform after Gaussian
// blurring (any linear filter with normalized weights preserves DC).
TEST_F(AdvancedFrameFilterGaussianComfortIntegrationTest, InPlaceUint8UniformInputPreservesValue)
{
	Frame frame(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	Frame mask(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	ASSERT_TRUE(frame.isValid());
	ASSERT_TRUE(mask.isValid());

	fillUniform<uint8_t>(frame, uint8_t(128));
	fillMaskUniform(mask, kValidMaskValue);

	EXPECT_TRUE(CV::Advanced::AdvancedFrameFilterGaussian::Comfort::filter(frame, mask, 3u, kInvalidMaskValue, &worker_));

	EXPECT_TRUE(allPixelsEqual<uint8_t>(frame, uint8_t(128)));
}

// Component interaction: Comfort validates against FrameType::dataType() before
// dispatching. A float frame with a Y8 mask must round-trip through the
// float-branch dispatch.
TEST_F(AdvancedFrameFilterGaussianComfortIntegrationTest, InPlaceFloatUniformInputPreservesValue)
{
	Frame frame(FrameType(kWidth, kHeight, FrameType::genericPixelFormat<float, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
	Frame mask(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	ASSERT_TRUE(frame.isValid());
	ASSERT_TRUE(mask.isValid());

	fillUniform<float>(frame, 0.5f);
	fillMaskUniform(mask, kValidMaskValue);

	EXPECT_TRUE(CV::Advanced::AdvancedFrameFilterGaussian::Comfort::filter(frame, mask, 5u, kInvalidMaskValue, &worker_));

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		const float* row = frame.constrow<float>(y);
		for (unsigned int x = 0u; x < frame.width(); ++x)
		{
			EXPECT_NEAR(row[x], 0.5f, 1e-5f);
		}
	}
}

// Component interaction: Comfort's early-return path when parameter validation
// fails must NOT invoke the templated filter and must NOT mutate the frame.
TEST_F(AdvancedFrameFilterGaussianComfortIntegrationTest, InPlaceEvenFilterSizeIsRejected)
{
	Frame frame(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	Frame mask(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	ASSERT_TRUE(frame.isValid());
	fillUniform<uint8_t>(frame, uint8_t(200));
	fillMaskUniform(mask, kValidMaskValue);

	// Even filter sizes are invalid — Comfort must return false.
	EXPECT_FALSE(CV::Advanced::AdvancedFrameFilterGaussian::Comfort::filter(frame, mask, 4u, kInvalidMaskValue, &worker_));

	// Frame should be untouched.
	EXPECT_TRUE(allPixelsEqual<uint8_t>(frame, uint8_t(200)));
}

// Component interaction: Comfort's frame-size validation against filter
// half-size. A frame with dimensions smaller than filter/2 must be rejected.
TEST_F(AdvancedFrameFilterGaussianComfortIntegrationTest, InPlaceTinyFrameIsRejected)
{
	// filterSize = 11 → filterSize_2 = 5. Frame width/height must be > 5.
	Frame frame(FrameType(3u, 3u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	Frame mask(FrameType(3u, 3u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	ASSERT_TRUE(frame.isValid());
	fillUniform<uint8_t>(frame, uint8_t(77));
	fillMaskUniform(mask, kValidMaskValue);

	EXPECT_FALSE(CV::Advanced::AdvancedFrameFilterGaussian::Comfort::filter(frame, mask, 11u, kInvalidMaskValue, &worker_));
	EXPECT_TRUE(allPixelsEqual<uint8_t>(frame, uint8_t(77)));
}

// Component interaction: Comfort validates frame/mask FrameType compatibility.
// A mask whose dimensions differ from the frame must be rejected before
// dispatch.
TEST_F(AdvancedFrameFilterGaussianComfortIntegrationTest, InPlaceMismatchedMaskIsRejected)
{
	Frame frame(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	Frame mask(FrameType(kWidth + 8u, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	ASSERT_TRUE(frame.isValid());
	ASSERT_TRUE(mask.isValid());
	fillUniform<uint8_t>(frame, uint8_t(50));
	fillMaskUniform(mask, kValidMaskValue);

	EXPECT_FALSE(CV::Advanced::AdvancedFrameFilterGaussian::Comfort::filter(frame, mask, 3u, kInvalidMaskValue, &worker_));
	EXPECT_TRUE(allPixelsEqual<uint8_t>(frame, uint8_t(50)));
}

// Component interaction: out-of-place Comfort::filter must allocate/reset the
// target Frame (via Frame::set) and route data through the templated filter.
TEST_F(AdvancedFrameFilterGaussianComfortIntegrationTest, OutOfPlaceUint8ProducesFilteredTarget)
{
	Frame source(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	Frame sourceMask(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	ASSERT_TRUE(source.isValid());
	ASSERT_TRUE(sourceMask.isValid());
	fillUniform<uint8_t>(source, uint8_t(100));
	fillMaskUniform(sourceMask, kValidMaskValue);

	// Target frames intentionally start invalid — Comfort must initialize them.
	Frame target;
	Frame targetMask;

	EXPECT_TRUE(CV::Advanced::AdvancedFrameFilterGaussian::Comfort::filter(source, sourceMask, target, targetMask, 3u, kInvalidMaskValue, &worker_));

	ASSERT_TRUE(target.isValid());
	ASSERT_TRUE(targetMask.isValid());
	EXPECT_EQ(target.width(), source.width());
	EXPECT_EQ(target.height(), source.height());
	EXPECT_EQ(target.channels(), source.channels());
	EXPECT_EQ(target.frameType(), source.frameType());
	EXPECT_EQ(targetMask.frameType(), sourceMask.frameType());

	// Uniform source + fully-valid mask ⇒ uniform target.
	EXPECT_TRUE(allPixelsEqual<uint8_t>(target, uint8_t(100)));

	// The source frame must be untouched by the out-of-place variant.
	EXPECT_TRUE(allPixelsEqual<uint8_t>(source, uint8_t(100)));
}

// Component interaction: out-of-place float variant — validates dispatch on
// DT_SIGNED_FLOAT_32 and Frame::set initialization for a float target.
TEST_F(AdvancedFrameFilterGaussianComfortIntegrationTest, OutOfPlaceFloatProducesFilteredTarget)
{
	const FrameType frameType(kWidth, kHeight, FrameType::genericPixelFormat<float, 1u>(), FrameType::ORIGIN_UPPER_LEFT);
	const FrameType maskType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT);

	Frame source(frameType);
	Frame sourceMask(maskType);

	ASSERT_TRUE(source.isValid());
	ASSERT_TRUE(sourceMask.isValid());
	fillUniform<float>(source, 1.25f);
	fillMaskUniform(sourceMask, kValidMaskValue);

	Frame target;
	Frame targetMask;

	EXPECT_TRUE(CV::Advanced::AdvancedFrameFilterGaussian::Comfort::filter(source, sourceMask, target, targetMask, 3u, kInvalidMaskValue, &worker_));

	ASSERT_TRUE(target.isValid());
	ASSERT_TRUE(targetMask.isValid());
	EXPECT_EQ(target.frameType(), frameType);
	EXPECT_EQ(targetMask.frameType(), maskType);

	for (unsigned int y = 0u; y < target.height(); ++y)
	{
		const float* row = target.constrow<float>(y);
		for (unsigned int x = 0u; x < target.width(); ++x)
		{
			EXPECT_NEAR(row[x], 1.25f, 1e-5f);
		}
	}
}

// Component interaction: out-of-place validation must reject even filter
// sizes before touching the target Frame.
TEST_F(AdvancedFrameFilterGaussianComfortIntegrationTest, OutOfPlaceEvenFilterSizeIsRejected)
{
	Frame source(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	Frame sourceMask(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	ASSERT_TRUE(source.isValid());
	fillUniform<uint8_t>(source, uint8_t(10));
	fillMaskUniform(sourceMask, kValidMaskValue);

	Frame target;
	Frame targetMask;

	EXPECT_FALSE(CV::Advanced::AdvancedFrameFilterGaussian::Comfort::filter(source, sourceMask, target, targetMask, 2u, kInvalidMaskValue, &worker_));
}

// Component interaction: filter behavior with an all-invalid mask. All mask
// pixels equal maskValue ⇒ every source pixel is masked out. The Gaussian
// filter has no valid neighbors to draw from at any location; result-mask
// pixels should reflect this. Regardless, the Comfort call must succeed and
// return true because the input is valid.
TEST_F(AdvancedFrameFilterGaussianComfortIntegrationTest, OutOfPlaceAllInvalidMaskStillDispatches)
{
	Frame source(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	Frame sourceMask(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	ASSERT_TRUE(source.isValid());
	fillUniform<uint8_t>(source, uint8_t(200));
	// Every mask pixel equals the invalid-mask sentinel.
	fillMaskUniform(sourceMask, kInvalidMaskValue);

	Frame target;
	Frame targetMask;

	EXPECT_TRUE(CV::Advanced::AdvancedFrameFilterGaussian::Comfort::filter(source, sourceMask, target, targetMask, 3u, kInvalidMaskValue, &worker_));
	ASSERT_TRUE(target.isValid());
	ASSERT_TRUE(targetMask.isValid());
	EXPECT_EQ(target.frameType(), source.frameType());
	EXPECT_EQ(targetMask.frameType(), sourceMask.frameType());
}

// Component interaction: filter with a multi-channel (RGB24) frame validates
// that Comfort correctly forwards per-channel counts to the templated impl.
TEST_F(AdvancedFrameFilterGaussianComfortIntegrationTest, InPlaceRgb24UniformInputPreservesValue)
{
	Frame frame(FrameType(kWidth, kHeight, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
	Frame mask(FrameType(kWidth, kHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	ASSERT_TRUE(frame.isValid());
	ASSERT_EQ(frame.channels(), 3u);
	fillUniform<uint8_t>(frame, uint8_t(90));
	fillMaskUniform(mask, kValidMaskValue);

	EXPECT_TRUE(CV::Advanced::AdvancedFrameFilterGaussian::Comfort::filter(frame, mask, 3u, kInvalidMaskValue, &worker_));

	EXPECT_TRUE(allPixelsEqual<uint8_t>(frame, uint8_t(90)));
}

} // namespace TestAdvanced

} // namespace TestCV

} // namespace Test

} // namespace Ocean

#endif // OCEAN_USE_GTEST
