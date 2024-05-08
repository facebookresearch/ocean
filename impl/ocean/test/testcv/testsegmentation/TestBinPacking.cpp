/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testsegmentation/TestBinPacking.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/PixelBoundingBox.h"

#include "ocean/cv/segmentation/BinPacking.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestSegmentation
{

bool TestBinPacking::test(const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Bin packing test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testBinPacking(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Mask analyzer test succeeded.";
	}
	else
	{
		Log::info() << "Mask analyzer test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestBinPacking, BinPacking)
{
	EXPECT_TRUE(TestBinPacking::testBinPacking(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestBinPacking::testBinPacking(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Bin packing test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int numberBoxes = RandomI::random(1u, 50u);

		CV::PixelBoundingBoxes boxes;

		for (unsigned int n = 0u; n < numberBoxes; ++n)
		{
			const unsigned int width = RandomI::random(1u, 200u);
			const unsigned int height = RandomI::random(1u, 200u);

			boxes.emplace_back(0u, 0u, width, height);
		}

		for (const bool allowTransposed : {false, true})
		{
			unsigned int width = 0u;
			unsigned int height = 0u;
			const CV::Segmentation::BinPacking::Packings packings = CV::Segmentation::BinPacking::binPacking(boxes, allowTransposed, &width, &height);

			if (packings.size() != boxes.size())
			{
				allSucceeded = false;
			}

			if (width == 0u || height == 0u)
			{
				allSucceeded = false;
			}

			Frame frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
			frame.setValue(0xFFu);

			UnorderedIndexSet32 boxIndicesSet;

			unsigned int usedWidth = 0u;
			unsigned int usedHeight = 0u;

			for (const CV::Segmentation::BinPacking::Packing& packing : packings)
			{
				const Index32& boxIndex = packing.boxIndex();

				boxIndicesSet.emplace(boxIndex);

				if (boxIndex >= boxes.size())
				{
					allSucceeded = false;
					break;
				}

				const unsigned int boxWidth = packing.transposed() ? boxes[boxIndex].height() :  boxes[boxIndex].width();
				const unsigned int boxHeight = packing.transposed() ? boxes[boxIndex].width() :  boxes[boxIndex].height();

				const CV::PixelBoundingBox box(packing.topLeft(), boxWidth, boxHeight);

				for (unsigned int y = box.top(); y < box.bottomEnd(); ++y)
				{
					for (unsigned int x = box.left(); x < box.rightEnd(); ++x)
					{
						if (frame.constpixel<uint8_t>(x, y)[0] == 0x00)
						{
							allSucceeded = false;
						}
					}
				}

				for (unsigned int y = box.top(); y < box.bottomEnd(); ++y)
				{
					memset(frame.pixel<uint8_t>(box.left(), y), 0x00, box.width());
				}

				usedWidth = std::max(usedWidth, box.rightEnd());
				usedHeight = std::max(usedHeight, box.bottomEnd());
			}

			if (width != usedWidth || height != usedHeight)
			{
				allSucceeded = false;
			}

			if (boxIndicesSet.size() != boxes.size())
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED";
	}

	return allSucceeded;
}

} // namespace TestSegmentation

} // namespace TestCV

} // namespace Test

} // namespace Ocean
