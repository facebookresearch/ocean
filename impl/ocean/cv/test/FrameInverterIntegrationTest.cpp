/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>

#include "ocean/cv/FrameInverter.h"

#include <cstdint>
#include <vector>

using namespace Ocean;
using namespace Ocean::CV;

namespace {

// Build a single-plane Y8 Frame with deterministic content equal to
// (x + y * width) modulo 256. This exercises both the Frame data layout and
// the FrameInverter::invert pixel transform.
Frame makeY8Frame(unsigned int width, unsigned int height)
{
	Frame frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	for (unsigned int y = 0u; y < height; ++y)
	{
		uint8_t* row = frame.row<uint8_t>(y);
		for (unsigned int x = 0u; x < width; ++x)
		{
			row[x] = static_cast<uint8_t>((x + y * width) & 0xFFu);
		}
	}
	return frame;
}

} // namespace

// FrameInverter + Frame integration: invert a Y8 frame into a separate target
// Frame and verify every output pixel equals 0xFF - source pixel.
TEST(FrameInverterFrameIntegrationTest, InvertY8FrameProducesComplementedPixels)
{
	constexpr unsigned int kWidth = 17u;
	constexpr unsigned int kHeight = 11u;

	Frame source = makeY8Frame(kWidth, kHeight);
	Frame target;

	ASSERT_TRUE(FrameInverter::invert(source, target, /*worker=*/nullptr));
	ASSERT_TRUE(target.isValid());
	EXPECT_EQ(target.width(), kWidth);
	EXPECT_EQ(target.height(), kHeight);
	EXPECT_EQ(target.channels(), 1u);
	EXPECT_EQ(target.pixelFormat(), FrameType::FORMAT_Y8);

	for (unsigned int y = 0u; y < kHeight; ++y)
	{
		const uint8_t* srcRow = source.constrow<uint8_t>(y);
		const uint8_t* dstRow = target.constrow<uint8_t>(y);
		for (unsigned int x = 0u; x < kWidth; ++x)
		{
			EXPECT_EQ(dstRow[x], static_cast<uint8_t>(0xFF - srcRow[x]))
				<< "Mismatch at (" << x << ", " << y << ")";
		}
	}
}

// FrameInverter in-place + Frame integration: applying invert twice on the
// same Frame must round-trip back to the original pixel values, and timestamp
// metadata must be preserved across the in-place operation.
TEST(FrameInverterFrameIntegrationTest, InvertInPlaceTwiceRoundTripsAndPreservesTimestamp)
{
	constexpr unsigned int kWidth = 8u;
	constexpr unsigned int kHeight = 6u;

	Frame original = makeY8Frame(kWidth, kHeight);
	Frame frame = makeY8Frame(kWidth, kHeight);

	const Timestamp ts(123.5);
	const Timestamp relativeTs(0.25);
	frame.setTimestamp(ts);
	frame.setRelativeTimestamp(relativeTs);

	ASSERT_TRUE(FrameInverter::invert(frame, /*worker=*/nullptr));
	EXPECT_EQ(frame.timestamp(), ts);
	EXPECT_EQ(frame.relativeTimestamp(), relativeTs);

	// After a single inversion the bytes must differ from the original for at
	// least one non-zero source pixel.
	bool sawDifference = false;
	for (unsigned int y = 0u; y < kHeight && !sawDifference; ++y)
	{
		const uint8_t* origRow = original.constrow<uint8_t>(y);
		const uint8_t* curRow = frame.constrow<uint8_t>(y);
		for (unsigned int x = 0u; x < kWidth; ++x)
		{
			if (origRow[x] != curRow[x])
			{
				sawDifference = true;
				break;
			}
		}
	}
	EXPECT_TRUE(sawDifference);

	ASSERT_TRUE(FrameInverter::invert(frame, /*worker=*/nullptr));
	EXPECT_EQ(frame.timestamp(), ts);
	EXPECT_EQ(frame.relativeTimestamp(), relativeTs);

	for (unsigned int y = 0u; y < kHeight; ++y)
	{
		const uint8_t* origRow = original.constrow<uint8_t>(y);
		const uint8_t* curRow = frame.constrow<uint8_t>(y);
		for (unsigned int x = 0u; x < kWidth; ++x)
		{
			EXPECT_EQ(curRow[x], origRow[x])
				<< "Round-trip mismatch at (" << x << ", " << y << ")";
		}
	}
}

// FrameInverter raw-buffer overload integration: drive the public
// invert8BitPerChannel entry point (which internally dispatches to
// invert8BitPerChannelSubset) and confirm it handles padding strides
// correctly when source and target have different padding.
TEST(FrameInverterFrameIntegrationTest, Invert8BitPerChannelSubsetRespectsPadding)
{
	constexpr unsigned int kHorizontal = 10u;
	constexpr unsigned int kRows = 4u;
	constexpr unsigned int kSrcPadding = 3u;
	constexpr unsigned int kDstPadding = 5u;

	const unsigned int srcStride = kHorizontal + kSrcPadding;
	const unsigned int dstStride = kHorizontal + kDstPadding;

	std::vector<uint8_t> srcBuf(srcStride * kRows, 0u);
	std::vector<uint8_t> dstBuf(dstStride * kRows, 0xAAu); // sentinel pattern

	for (unsigned int y = 0u; y < kRows; ++y)
	{
		for (unsigned int x = 0u; x < kHorizontal; ++x)
		{
			srcBuf[y * srcStride + x] = static_cast<uint8_t>((x * 7u + y * 13u) & 0xFFu);
		}
	}

	FrameInverter::invert8BitPerChannel(
		srcBuf.data(), dstBuf.data(), /*width=*/kHorizontal, /*height=*/kRows,
		/*channels=*/1u, kSrcPadding, kDstPadding, /*worker=*/nullptr);

	for (unsigned int y = 0u; y < kRows; ++y)
	{
		for (unsigned int x = 0u; x < kHorizontal; ++x)
		{
			EXPECT_EQ(dstBuf[y * dstStride + x],
				static_cast<uint8_t>(0xFF - srcBuf[y * srcStride + x]))
				<< "Active pixel mismatch at (" << x << ", " << y << ")";
		}
		for (unsigned int x = kHorizontal; x < dstStride; ++x)
		{
			EXPECT_EQ(dstBuf[y * dstStride + x], 0xAAu)
				<< "Padding byte must not be written at (" << x << ", " << y << ")";
		}
	}
}
