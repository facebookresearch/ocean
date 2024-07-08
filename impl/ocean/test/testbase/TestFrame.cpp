/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestFrame.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Memory.h"
#include "ocean/base/Frame.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestFrame::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Test Frame:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testDefinedDataTypes() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testDefinedPixelFormats() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPlaneContructors(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPlaneCopyContructors(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFrameSpecificationGenericPixelFormats(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFrameSpecificationNonGenericPixelFormats(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCopyConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMoveConstructor(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCopyOperator(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPlaneInitializer(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPlaneLayout(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSubFrame(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testRelease(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTimestamp(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAccessorsDataTypes(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testAccessorsPixelFormats(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSetFrameType(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testLegacyCopy(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCopy(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMakeContinuous(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPlaneBytesPerPixel() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSetValue(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testContainsValue(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHasTransparentPixel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testStrideBytes2paddingElements(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testHaveIntersectingMemory(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testUpdateMemory(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFormatIsPacked() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTranslateDataType() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testTranslatePixelFormat() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame test succeeded.";
	}
	else
	{
		Log::info() << "Frame test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrame, DefinedDataTypes)
{
	EXPECT_TRUE(TestFrame::testDefinedDataTypes());
}

TEST(TestFrame, DefinedPixelFormats)
{
	EXPECT_TRUE(TestFrame::testDefinedPixelFormats());
}

TEST(TestFrame, PlaneContructors)
{
	EXPECT_TRUE(TestFrame::testPlaneContructors(GTEST_TEST_DURATION));
}

TEST(TestFrame, PlaneCopyContructors)
{
	EXPECT_TRUE(TestFrame::testPlaneCopyContructors(GTEST_TEST_DURATION));
}

TEST(TestFrame, FrameSpecificationGenericPixelFormats)
{
	EXPECT_TRUE(TestFrame::testFrameSpecificationGenericPixelFormats(GTEST_TEST_DURATION));
}

TEST(TestFrame, FrameSpecificationNonGenericPixelFormats)
{
	EXPECT_TRUE(TestFrame::testFrameSpecificationNonGenericPixelFormats(GTEST_TEST_DURATION));
}

TEST(TestFrame, Constructor)
{
	EXPECT_TRUE(TestFrame::testConstructor(GTEST_TEST_DURATION));
}

TEST(TestFrame, CopyConstructor)
{
	EXPECT_TRUE(TestFrame::testCopyConstructor(GTEST_TEST_DURATION));
}

TEST(TestFrame, MoveConstructor)
{
	EXPECT_TRUE(TestFrame::testMoveConstructor(GTEST_TEST_DURATION));
}

TEST(TestFrame, CopyOperator)
{
	EXPECT_TRUE(TestFrame::testCopyOperator(GTEST_TEST_DURATION));
}

TEST(TestFrame, PlaneInitializer)
{
	EXPECT_TRUE(TestFrame::testPlaneInitializer(GTEST_TEST_DURATION));
}

TEST(TestFrame, PlaneLayout)
{
	EXPECT_TRUE(TestFrame::testPlaneLayout(GTEST_TEST_DURATION));
}

TEST(TestFrame, Release)
{
	EXPECT_TRUE(TestFrame::testRelease(GTEST_TEST_DURATION));
}

TEST(TestFrame, SubFrame)
{
	EXPECT_TRUE(TestFrame::testSubFrame(GTEST_TEST_DURATION));
}

TEST(TestFrame, Timestmap)
{
	EXPECT_TRUE(TestFrame::testTimestamp(GTEST_TEST_DURATION));
}

TEST(TestFrame, AccessorsDataTypes)
{
	EXPECT_TRUE(TestFrame::testAccessorsDataTypes(GTEST_TEST_DURATION));
}

TEST(TestFrame, AccessorsPixelFormats)
{
	EXPECT_TRUE(TestFrame::testAccessorsPixelFormats(GTEST_TEST_DURATION));
}

TEST(TestFrame, SetFrameType)
{
	EXPECT_TRUE(TestFrame::testSetFrameType(GTEST_TEST_DURATION));
}

TEST(TestFrame, LegacyCopy)
{
	EXPECT_TRUE(TestFrame::testLegacyCopy(GTEST_TEST_DURATION));
}

TEST(TestFrame, Copy)
{
	EXPECT_TRUE(TestFrame::testCopy(GTEST_TEST_DURATION));
}

TEST(TestFrame, MakeContinuous)
{
	EXPECT_TRUE(TestFrame::testMakeContinuous(GTEST_TEST_DURATION));
}

TEST(TestFrame, PlaneBytesPerPixel)
{
	EXPECT_TRUE(TestFrame::testPlaneBytesPerPixel());
}

TEST(TestFrame, SetValue)
{
	EXPECT_TRUE(TestFrame::testSetValue(GTEST_TEST_DURATION));
}

TEST(TestFrame, ContainsValue)
{
	EXPECT_TRUE(TestFrame::testContainsValue(GTEST_TEST_DURATION));
}

TEST(TestFrame, HasTransparentPixel)
{
	EXPECT_TRUE(TestFrame::testHasTransparentPixel(GTEST_TEST_DURATION));
}

TEST(TestFrame, StrideBytes2paddingElements)
{
	EXPECT_TRUE(TestFrame::testStrideBytes2paddingElements(GTEST_TEST_DURATION));
}

TEST(TestFrame, HaveIntersectingMemory)
{
	EXPECT_TRUE(TestFrame::testHaveIntersectingMemory(GTEST_TEST_DURATION));
}

TEST(TestFrame, FormatIsPacked)
{
	EXPECT_TRUE(TestFrame::testFormatIsPacked());
}

TEST(TestFrame, TranslatePixelFormat)
{
	EXPECT_TRUE(TestFrame::testTranslatePixelFormat());
}

TEST(TestFrame, TranslateDataType)
{
	EXPECT_TRUE(TestFrame::testTranslateDataType());
}

TEST(TestFrame, UpdateMemory)
{
	EXPECT_TRUE(TestFrame::testUpdateMemory(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFrame::testDefinedDataTypes()
{
	Log::info() << "Testing defined data types:";

	const FrameType::DataTypes& dataTypes = FrameType::definedDataTypes();

	bool allSucceeded = true;

	if (dataTypes.size() + 1 != size_t(FrameType::DT_END))
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testDefinedPixelFormats()
{
	Log::info() << "Testing defined pixel formats:";

	const FrameType::PixelFormats pixelFormats = definedPixelFormats();

	bool allSucceeded = true;

	if (pixelFormats.size() + 1 != size_t(FrameType::FORMAT_END))
	{
		allSucceeded = false;
	}

	UnorderedIndexSet32 uniquenessSet;

	for (const FrameType::PixelFormat pixelFormat : pixelFormats)
	{
		static_assert(sizeof(FrameType::PixelFormat) == sizeof(uint64_t), "Invalid data type!");

		const uint64_t value64 = uint64_t(pixelFormat);
		const uint32_t value32 = uint32_t(value64 & 0xFFFFFFFFull);

		uniquenessSet.emplace(value32);
	}

	if (uniquenessSet.size() != pixelFormats.size())
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testPlaneContructors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing Plane constructors:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u);
		const unsigned int height = RandomI::random(1u, 1080u);
		const unsigned int channels = RandomI::random(1u, 5u);
		const unsigned int paddingElements = RandomI::random(1u, 100u) * RandomI::random(0u, 1u);

		allSucceeded = testPlaneContructors<int8_t>(width, height, channels, paddingElements) && allSucceeded;
		allSucceeded = testPlaneContructors<uint8_t>(width, height, channels, paddingElements) && allSucceeded;

		allSucceeded = testPlaneContructors<int16_t>(width, height, channels, paddingElements) && allSucceeded;
		allSucceeded = testPlaneContructors<uint16_t>(width, height, channels, paddingElements) && allSucceeded;

		allSucceeded = testPlaneContructors<int32_t>(width, height, channels, paddingElements) && allSucceeded;
		allSucceeded = testPlaneContructors<uint32_t>(width, height, channels, paddingElements) && allSucceeded;

		allSucceeded = testPlaneContructors<float>(width, height, channels, paddingElements) && allSucceeded;
		allSucceeded = testPlaneContructors<double>(width, height, channels, paddingElements) && allSucceeded;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testPlaneCopyContructors(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing Plane copy constructors:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	Frame::Plane defaultPlane;

	if (defaultPlane.isValid())
	{
		allSucceeded = false;
	}

	Frame::Plane movedDefaultPlane(std::move(defaultPlane));

	if (movedDefaultPlane.isValid())
	{
		allSucceeded = false;
	}

	if (defaultPlane.isValid()) // NOLINT(bugprone-use-after-move)
	{
		allSucceeded = false;
	}

	{
		const Frame::Plane copyInvalidPlane(movedDefaultPlane);

		if (copyInvalidPlane.isValid())
		{
			allSucceeded = false;
		}
	}

	{
		const Frame::Plane copyInvalidPlane(movedDefaultPlane, Frame::ACM_USE_KEEP_LAYOUT);

		if (copyInvalidPlane.isValid())
		{
			allSucceeded = false;
		}
	}

	{
		const Frame::Plane copyInvalidPlane(movedDefaultPlane, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		if (copyInvalidPlane.isValid())
		{
			allSucceeded = false;
		}
	}

	{
		const Frame::Plane copyInvalidPlane(movedDefaultPlane, Frame::ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA);

		if (copyInvalidPlane.isValid())
		{
			allSucceeded = false;
		}
	}

	{
		const Frame::Plane copyInvalidPlane(movedDefaultPlane, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		if (copyInvalidPlane.isValid())
		{
			allSucceeded = false;
		}
	}

	{
		const Frame::Plane copyInvalidPlane(movedDefaultPlane, Frame::ACM_USE_OR_COPY);

		if (copyInvalidPlane.isValid())
		{
			allSucceeded = false;
		}
	}

	{
		const Frame::Plane copyInvalidPlane(movedDefaultPlane, Frame::ACM_USE_OR_COPY_KEEP_LAYOUT);

		if (copyInvalidPlane.isValid())
		{
			allSucceeded = false;
		}
	}

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u);
		const unsigned int height = RandomI::random(1u, 1080u);
		const unsigned int channels = RandomI::random(1u, 5u);
		const unsigned int paddingElements = RandomI::random(1u, 100u) * RandomI::random(0u, 1u);

		allSucceeded = testPlaneCopyContructors<int8_t>(width, height, channels, paddingElements) && allSucceeded;
		allSucceeded = testPlaneCopyContructors<uint8_t>(width, height, channels, paddingElements) && allSucceeded;

		allSucceeded = testPlaneCopyContructors<int16_t>(width, height, channels, paddingElements) && allSucceeded;
		allSucceeded = testPlaneCopyContructors<uint16_t>(width, height, channels, paddingElements) && allSucceeded;

		allSucceeded = testPlaneCopyContructors<int32_t>(width, height, channels, paddingElements) && allSucceeded;
		allSucceeded = testPlaneCopyContructors<uint32_t>(width, height, channels, paddingElements) && allSucceeded;

		allSucceeded = testPlaneCopyContructors<float>(width, height, channels, paddingElements) && allSucceeded;
		allSucceeded = testPlaneCopyContructors<double>(width, height, channels, paddingElements) && allSucceeded;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;

}

bool TestFrame::testFrameSpecificationGenericPixelFormats(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing frame specification for generic pixel formats:";

	bool allSucceeded = true;

	const FrameType::DataTypes& dataTypes = FrameType::definedDataTypes();

	const std::vector<unsigned int> bytesPerElements =
	{
		1u, 1u, 2u, 2u, 4u,	4u,	8u,	8u,	2u,	4u,	8u
	};

	ocean_assert(dataTypes.size() == bytesPerElements.size());
	for (unsigned int n = 0u; n < dataTypes.size(); ++n)
	{
		ocean_assert(FrameType::bytesPerDataType(dataTypes[n]) == bytesPerElements[n]);
	}

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u);
		const unsigned int height = RandomI::random(1u, 1080u);

		const unsigned int dataTypeIndex = RandomI::random(0u, (unsigned int)(dataTypes.size()) - 1u);

		const FrameType::DataType dataType = dataTypes[dataTypeIndex];
		const unsigned int channels = RandomI::random(1u, 5u);
		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(dataType, channels);

		const FrameType::PixelOrigin pixelOrigin = RandomI::random({FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const unsigned int framePaddingElements = RandomI::random(0u, 100u);

		const FrameType frameType(width, height, pixelFormat, pixelOrigin);
		Frame frame(frameType, framePaddingElements);

		// just checking whether 'frame' has the correct specification
		if (!validateFrameSpecification(frame, frameType, framePaddingElements, bytesPerElements[dataTypeIndex], true, false))
		{
			allSucceeded = false;
		}

		const Frame frameCopyWithOwnership(frame);

		// 'frameCopyWithOwnership' is just an entire copy of 'frame' with own memory (and continuous memory layout)
		if (!validateFrameSpecification(frameCopyWithOwnership, frameType, 0u, bytesPerElements[dataTypeIndex], true, false))
		{
			allSucceeded = false;
		}

		const Frame frameCopyWithoutOwnership(frame, Frame::ACM_USE_KEEP_LAYOUT);

		// 'frameCopyWithoutOwnership' a copy without own memory
		if (!validateFrameSpecification(frameCopyWithoutOwnership, frameType, framePaddingElements, bytesPerElements[dataTypeIndex], false, false))
		{
			allSucceeded = false;
		}

		const Frame frameCopyWithOwnership2(frameCopyWithoutOwnership, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		// 'frameCopyWithOwnership2' is a copy of a frame (which does not own the memory) but will own the memory now (and continuous memory layout)
		if (!validateFrameSpecification(frameCopyWithOwnership2, frameType, 0u, bytesPerElements[dataTypeIndex], true, false))
		{
			allSucceeded = false;
		}

		Frame movedFrame = std::move(frame);

		// 'movedFrame' must be equivalent to the previous 'frame'
		if (!validateFrameSpecification(movedFrame, frameType, framePaddingElements, bytesPerElements[dataTypeIndex], true, false))
		{
			allSucceeded = false;
		}

		if (frame.isValid()) // NOLINT(bugprone-use-after-move)
		{
			allSucceeded = false;
		}

		// we release the memory sometimes explicitly
		if (RandomI::random(1u) == 0u)
		{
			frame.release();

			if (frame.isValid())
			{
				allSucceeded = false;
			}

			if (frame)
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testFrameSpecificationNonGenericPixelFormats(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing frame specification for non-generic pixel formats:";

	bool allSucceeded = true;

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_BGR4444,
		FrameType::FORMAT_BGRA4444,
		FrameType::FORMAT_RGB4444,
		FrameType::FORMAT_RGBA4444,
		FrameType::FORMAT_BGR5551,
		FrameType::FORMAT_BGR565,
		FrameType::FORMAT_RGB5551,
		FrameType::FORMAT_RGB565,
		FrameType::FORMAT_UYVY16,
		FrameType::FORMAT_Y_U_V12,
		FrameType::FORMAT_Y_V_U12,
		FrameType::FORMAT_Y_U_V12_FULL_RANGE,
		FrameType::FORMAT_Y_V_U12_FULL_RANGE,
		FrameType::FORMAT_Y_UV12,
		FrameType::FORMAT_Y_VU12,
		FrameType::FORMAT_Y_UV12_FULL_RANGE,
		FrameType::FORMAT_Y_VU12_FULL_RANGE,
		FrameType::FORMAT_YUYV16,
		FrameType::FORMAT_Y_U_V24,
		FrameType::FORMAT_Y_U_V24_FULL_RANGE
	};

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(2u, 1920u) & 0xFFFFFFFEu; // multiple of two
		const unsigned int height = RandomI::random(2u, 1080u) & 0xFFFFFFFEu;

		const FrameType::PixelFormat pixelFormat = RandomI::random(pixelFormats);
		const FrameType::PixelOrigin pixelOrigin = RandomI::random({FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const FrameType frameType(width, height, pixelFormat, pixelOrigin);

		Indices32 paddingElementsPerPlane;

		if (RandomI::random(1u) == 0u)
		{
			for (unsigned int n = 0u; n < frameType.numberPlanes(); ++n)
			{
				paddingElementsPerPlane.emplace_back(RandomI::random(1u, 100u) * RandomI::random(1u));
			}
		}

		Frame frame(frameType, paddingElementsPerPlane);

		if (frame.frameType() != frameType)
		{
			allSucceeded = false;
		}

		// we release the memory sometimes explicitly
		if (RandomI::random(1u) == 0u)
		{
			frame.release();

			if (frame.isValid())
			{
				allSucceeded = false;
			}

			if (frame)
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing constructors:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_ABGR32,
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_Y10,
		FrameType::FORMAT_Y32,
		FrameType::FORMAT_RGB5551,
		FrameType::FORMAT_Y_UV12,
		FrameType::FORMAT_F64,
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<float, 3u>(),
	};

	const std::vector<Frame::CopyMode> copyModes =
	{
		Frame::CM_USE_KEEP_LAYOUT,
		Frame::CM_COPY_REMOVE_PADDING_LAYOUT,
		Frame::CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA,
		Frame::CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA,
	};

	do
	{
		const FrameType frameType(randomizedFrameType(pixelFormats, &randomGenerator));

		Indices32 paddingElementsPerPlane;

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			for (unsigned int n = 0u; n < frameType.numberPlanes(); ++n)
			{
				paddingElementsPerPlane.emplace_back(RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u));
			}
		}

		Frame sourceFrame(frameType, paddingElementsPerPlane);

		{
			// writable source memory

			for (const Frame::CopyMode copyMode : copyModes)
			{
				Frame::PlaneInitializers<void> planeInitializers;

				for (unsigned int planeIndex = 0u; planeIndex < sourceFrame.numberPlanes(); ++planeIndex)
				{
					planeInitializers.emplace_back(sourceFrame.data<void>(planeIndex), copyMode, sourceFrame.paddingElements(planeIndex));
				}

				const Frame usedOrCopiedFrame(sourceFrame.frameType(), planeInitializers);

				if (!usedOrCopiedFrame.isValid())
				{
					allSucceeded = false;
				}

				if (usedOrCopiedFrame.frameType() != sourceFrame.frameType())
				{
					allSucceeded = false;
				}

				for (unsigned int planeIndex = 0u; planeIndex < sourceFrame.numberPlanes(); ++planeIndex)
				{
					unsigned int expectedPaddingElements = (unsigned int)(-1);
					bool expectedIsOwner = false;
					bool expectedIsReadOnly = false;

					switch (copyMode)
					{
						case Frame::CM_USE_KEEP_LAYOUT:
							expectedPaddingElements = sourceFrame.paddingElements(planeIndex);
							expectedIsOwner = false;
							expectedIsReadOnly = false;
							break;

						case Frame::CM_COPY_REMOVE_PADDING_LAYOUT:
							expectedPaddingElements = 0u;
							expectedIsOwner = true;
							expectedIsReadOnly = false;
							break;

						case Frame::CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA:
							expectedPaddingElements = sourceFrame.paddingElements(planeIndex);
							expectedIsOwner = true;
							expectedIsReadOnly = false;
							break;

						case Frame::CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA:
							expectedPaddingElements = sourceFrame.paddingElements(planeIndex);
							expectedIsOwner = true;
							expectedIsReadOnly = false;
							break;
					}

					ocean_assert(expectedPaddingElements != (unsigned int)(-1));

					if (usedOrCopiedFrame.paddingElements(planeIndex) != expectedPaddingElements)
					{
						allSucceeded = false;
					}

					if (usedOrCopiedFrame.isOwner() != expectedIsOwner)
					{
						allSucceeded = false;
					}

					if (usedOrCopiedFrame.isReadOnly() != expectedIsReadOnly)
					{
						allSucceeded = false;
					}

					if (expectedIsOwner)
					{
						if (usedOrCopiedFrame.constdata<void>(planeIndex) == sourceFrame.constdata<void>(planeIndex))
						{
							allSucceeded = false;
						}
					}
					else
					{
						if (usedOrCopiedFrame.constdata<void>(planeIndex) != sourceFrame.constdata<void>(planeIndex))
						{
							allSucceeded = false;
						}
					}
				}
			}
		}

		{
			// read-only source memory

			for (const Frame::CopyMode copyMode : copyModes)
			{
				Frame::PlaneInitializers<void> planeInitializers;

				for (unsigned int planeIndex = 0u; planeIndex < sourceFrame.numberPlanes(); ++planeIndex)
				{
					planeInitializers.emplace_back(sourceFrame.constdata<void>(planeIndex), copyMode, sourceFrame.paddingElements(planeIndex));
				}

				const Frame usedOrCopiedFrame(sourceFrame.frameType(), planeInitializers);

				if (!usedOrCopiedFrame.isValid())
				{
					allSucceeded = false;
				}

				if (usedOrCopiedFrame.frameType() != sourceFrame.frameType())
				{
					allSucceeded = false;
				}

				for (unsigned int planeIndex = 0u; planeIndex < sourceFrame.numberPlanes(); ++planeIndex)
				{
					unsigned int expectedPaddingElements = (unsigned int)(-1);
					bool expectedIsOwner = false;
					bool expectedIsReadOnly = false;

					switch (copyMode)
					{
						case Frame::CM_USE_KEEP_LAYOUT:
							expectedPaddingElements = sourceFrame.paddingElements(planeIndex);
							expectedIsOwner = false;
							expectedIsReadOnly = true;
							break;

						case Frame::CM_COPY_REMOVE_PADDING_LAYOUT:
							expectedPaddingElements = 0u;
							expectedIsOwner = true;
							expectedIsReadOnly = false;
							break;

						case Frame::CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA:
							expectedPaddingElements = sourceFrame.paddingElements(planeIndex);
							expectedIsOwner = true;
							expectedIsReadOnly = false;
							break;

						case Frame::CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA:
							expectedPaddingElements = sourceFrame.paddingElements(planeIndex);
							expectedIsOwner = true;
							expectedIsReadOnly = false;
							break;
					}

					ocean_assert(expectedPaddingElements != (unsigned int)(-1));

					if (usedOrCopiedFrame.paddingElements(planeIndex) != expectedPaddingElements)
					{
						allSucceeded = false;
					}

					if (usedOrCopiedFrame.isOwner() != expectedIsOwner)
					{
						allSucceeded = false;
					}

					if (usedOrCopiedFrame.isReadOnly() != expectedIsReadOnly)
					{
						allSucceeded = false;
					}

					if (expectedIsOwner)
					{
						if (usedOrCopiedFrame.constdata<void>(planeIndex) == sourceFrame.constdata<void>(planeIndex))
						{
							allSucceeded = false;
						}
					}
					else
					{
						if (usedOrCopiedFrame.constdata<void>(planeIndex) != sourceFrame.constdata<void>(planeIndex))
						{
							allSucceeded = false;
						}
					}
				}
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testCopyConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing copy constructor:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_ABGR32,
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_Y10,
		FrameType::FORMAT_Y32,
		FrameType::FORMAT_RGB5551,
		FrameType::FORMAT_Y_UV12,
		FrameType::FORMAT_F64,
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<float, 3u>(),
	};

	const std::vector<Frame::AdvancedCopyMode> advancedCopyModes =
	{
		Frame::ACM_USE_KEEP_LAYOUT,
		Frame::ACM_COPY_REMOVE_PADDING_LAYOUT,
		Frame::ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA,
		Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA,
		Frame::ACM_USE_OR_COPY,
		Frame::ACM_USE_OR_COPY_KEEP_LAYOUT
	};

	// ensuring that an invalid frame can be copied but creates an invalid frame

	for (const Frame::AdvancedCopyMode advancedCopyMode : advancedCopyModes)
	{
		const Frame invalidFrame;
		ocean_assert(!invalidFrame.isValid());

		const Frame invalidFrameCopy(invalidFrame, advancedCopyMode);

		if (invalidFrameCopy.isValid())
		{
			allSucceeded = false;
		}
	}

	do
	{
		const FrameType frameType(randomizedFrameType(pixelFormats, &randomGenerator));

		Indices32 paddingElementsPerPlane;

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				paddingElementsPerPlane.emplace_back(RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u));
			}
		}

		Frame sourceFrameOwner(frameType, paddingElementsPerPlane);

		{
			// source is owner, copy will be owner

			const Frame frameCopy(sourceFrameOwner);

			if (!frameCopy.isValid())
			{
				allSucceeded = false;
			}

			if (frameCopy.frameType() != sourceFrameOwner.frameType())
			{
				allSucceeded = false;
			}

			if (!frameCopy.isOwner())
			{
				allSucceeded = false;
			}

			if (frameCopy.isReadOnly())
			{
				allSucceeded = false;
			}

			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				if (frameCopy.paddingElements(planeIndex) != 0u)
				{
					allSucceeded = false;
				}

				if (frameCopy.constdata<void>(planeIndex) == sourceFrameOwner.constdata<void>(planeIndex))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// source is owner, copy will be owner

			const Frame& constSourceFrameOwner = sourceFrameOwner;

			const Frame frameCopy(constSourceFrameOwner);

			if (!frameCopy.isValid())
			{
				allSucceeded = false;
			}

			if (frameCopy.frameType() != sourceFrameOwner.frameType())
			{
				allSucceeded = false;
			}

			if (!frameCopy.isOwner())
			{
				allSucceeded = false;
			}

			if (frameCopy.isReadOnly())
			{
				allSucceeded = false;
			}

			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				if (frameCopy.paddingElements(planeIndex) != 0u)
				{
					allSucceeded = false;
				}

				if (frameCopy.constdata<void>(planeIndex) == sourceFrameOwner.constdata<void>(planeIndex))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// source is owner

			for (const Frame::AdvancedCopyMode advancedCopyMode : advancedCopyModes)
			{
				const Frame frameCopy(sourceFrameOwner, advancedCopyMode);

				if (!frameCopy.isValid())
				{
					allSucceeded = false;
				}

				if (frameCopy.frameType() != sourceFrameOwner.frameType())
				{
					allSucceeded = false;
				}

				for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
				{
					unsigned int expectedPaddingElements = (unsigned int)(-1);
					bool expectedIsOwner = false;

					switch (advancedCopyMode)
					{
						case Frame::ACM_USE_KEEP_LAYOUT:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = false;
							break;

						case Frame::ACM_COPY_REMOVE_PADDING_LAYOUT:
							expectedPaddingElements = 0u;
							expectedIsOwner = true;
							break;

						case Frame::ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = true;
							break;

						case Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = true;
							break;

						case Frame::ACM_USE_OR_COPY:
							expectedPaddingElements = 0u;
							expectedIsOwner = true;
							break;

						case Frame::ACM_USE_OR_COPY_KEEP_LAYOUT:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = true;
							break;
					}

					ocean_assert(expectedPaddingElements != (unsigned int)(-1));

					if (frameCopy.paddingElements(planeIndex) != expectedPaddingElements)
					{
						allSucceeded = false;
					}

					if (frameCopy.isOwner() != expectedIsOwner)
					{
						allSucceeded = false;
					}

					if (frameCopy.isReadOnly())
					{
						allSucceeded = false;
					}

					if ((expectedIsOwner && frameCopy.constdata<void>(planeIndex) == sourceFrameOwner.constdata<void>(planeIndex)) || (!expectedIsOwner && frameCopy.constdata<void>(planeIndex) != sourceFrameOwner.constdata<void>(planeIndex)))
					{
						allSucceeded = false;
					}
				}
			}
		}

		{
			// source is not owner, writable

			Frame::PlaneInitializers<void> planeInitializers;

			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				planeInitializers.emplace_back(sourceFrameOwner.data<void>(planeIndex), Frame::CM_USE_KEEP_LAYOUT, sourceFrameOwner.paddingElements(planeIndex));
			}

			const Frame constNotOwnerFrame(sourceFrameOwner.frameType(), planeInitializers);

			for (const Frame::AdvancedCopyMode advancedCopyMode : advancedCopyModes)
			{
				const Frame frameCopy(constNotOwnerFrame, advancedCopyMode);

				if (!frameCopy.isValid())
				{
					allSucceeded = false;
				}

				if (frameCopy.frameType() != sourceFrameOwner.frameType())
				{
					allSucceeded = false;
				}

				for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
				{
					unsigned int expectedPaddingElements = (unsigned int)(-1);
					bool expectedIsOwner = false;

					switch (advancedCopyMode)
					{
						case Frame::ACM_USE_KEEP_LAYOUT:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = false;
							break;

						case Frame::ACM_COPY_REMOVE_PADDING_LAYOUT:
							expectedPaddingElements = 0u;
							expectedIsOwner = true;
							break;

						case Frame::ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = true;
							break;

						case Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = true;
							break;

						case Frame::ACM_USE_OR_COPY:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = false;
							break;

						case Frame::ACM_USE_OR_COPY_KEEP_LAYOUT:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = false;
							break;
					}

					ocean_assert(expectedPaddingElements != (unsigned int)(-1));

					if (frameCopy.paddingElements(planeIndex) != expectedPaddingElements)
					{
						allSucceeded = false;
					}

					if (frameCopy.isOwner() != expectedIsOwner)
					{
						allSucceeded = false;
					}

					if (frameCopy.isReadOnly())
					{
						allSucceeded = false;
					}

					if ((expectedIsOwner && frameCopy.constdata<void>(planeIndex) == sourceFrameOwner.constdata<void>(planeIndex)) || (!expectedIsOwner && frameCopy.constdata<void>(planeIndex) != sourceFrameOwner.constdata<void>(planeIndex)))
					{
						allSucceeded = false;
					}
				}
			}
		}

		{
			// source is not owner, read-only

			Frame::PlaneInitializers<void> planeInitializers;

			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				planeInitializers.emplace_back(sourceFrameOwner.constdata<void>(planeIndex), Frame::CM_USE_KEEP_LAYOUT, sourceFrameOwner.paddingElements(planeIndex));
			}

			const Frame constNotOwnerFrame(sourceFrameOwner.frameType(), planeInitializers);

			for (const Frame::AdvancedCopyMode advancedCopyMode : advancedCopyModes)
			{
				const Frame frameCopy(constNotOwnerFrame, advancedCopyMode);

				if (!frameCopy.isValid())
				{
					allSucceeded = false;
				}

				if (frameCopy.frameType() != sourceFrameOwner.frameType())
				{
					allSucceeded = false;
				}

				for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
				{
					unsigned int expectedPaddingElements = (unsigned int)(-1);
					bool expectedIsOwner = false;
					bool expectedIsReadOnly = false;

					switch (advancedCopyMode)
					{
						case Frame::ACM_USE_KEEP_LAYOUT:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = false;
							expectedIsReadOnly = true;
							break;

						case Frame::ACM_COPY_REMOVE_PADDING_LAYOUT:
							expectedPaddingElements = 0u;
							expectedIsOwner = true;
							expectedIsReadOnly = false;
							break;

						case Frame::ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = true;
							expectedIsReadOnly = false;
							break;

						case Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = true;
							expectedIsReadOnly = false;
							break;

						case Frame::ACM_USE_OR_COPY:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = false;
							expectedIsReadOnly = true;
							break;

						case Frame::ACM_USE_OR_COPY_KEEP_LAYOUT:
							expectedPaddingElements = sourceFrameOwner.paddingElements(planeIndex);
							expectedIsOwner = false;
							expectedIsReadOnly = true;
							break;
					}

					ocean_assert(expectedPaddingElements != (unsigned int)(-1));

					if (frameCopy.paddingElements(planeIndex) != expectedPaddingElements)
					{
						allSucceeded = false;
					}

					if (frameCopy.isOwner() != expectedIsOwner)
					{
						allSucceeded = false;
					}

					if (frameCopy.isReadOnly() != expectedIsReadOnly)
					{
						allSucceeded = false;
					}

					if ((expectedIsOwner && frameCopy.constdata<void>(planeIndex) == sourceFrameOwner.constdata<void>(planeIndex)) || (!expectedIsOwner && frameCopy.constdata<void>(planeIndex) != sourceFrameOwner.constdata<void>(planeIndex)))
					{
						allSucceeded = false;
					}
				}
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testMoveConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing move constructor:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_ABGR32,
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_Y10,
		FrameType::FORMAT_Y32,
		FrameType::FORMAT_RGB5551,
		FrameType::FORMAT_Y_UV12,
		FrameType::FORMAT_Y_V_U12,
		FrameType::FORMAT_Y_U_V24,
		FrameType::FORMAT_F64,
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<float, 3u>(),
	};

	do
	{
		const FrameType frameType(randomizedFrameType(pixelFormats, &randomGenerator));

		const unsigned int bytesPerDataType = frameType.bytesPerDataType();

		{
			// Move constructor

			const Timestamp timestamp = Timestamp(double(RandomI::random(randomGenerator, -100, 100)));

			Frame originalFrame(frameType, Indices32(), timestamp);

			for (const bool sourceIsOwner : {true, false})
			{
				const Frame::AdvancedCopyMode copyMode = sourceIsOwner ? Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA : Frame::ACM_USE_KEEP_LAYOUT;

				Frame sourceFrame(originalFrame, copyMode);

				std::vector<const void*> sourceFrameDatas;
				for (unsigned int planeIndex = 0u; planeIndex < sourceFrame.numberPlanes(); ++planeIndex)
				{
					sourceFrameDatas.push_back(sourceFrame.constdata<void>(planeIndex));
				}

				Frame targetFrame(std::move(sourceFrame));

				if (sourceFrame.isValid()) // NOLINT(bugprone-use-after-move)
				{
					allSucceeded = false;
				}

				if (targetFrame.frameType() != frameType)
				{
					allSucceeded = false;
				}

				if (targetFrame.timestamp() != timestamp)
				{
					allSucceeded = false;
				}

				for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
				{
					unsigned int planeWidth;
					unsigned int planeHeight;
					unsigned int planeChannels;

					if (FrameType::planeLayout(frameType.pixelFormat(), frameType.width(), frameType.height(), planeIndex, planeWidth, planeHeight, planeChannels))
					{
						const Frame::Plane& plane = targetFrame.planes()[planeIndex];

						if (plane.height() != planeHeight)
						{
							allSucceeded = false;
						}

						const unsigned int planeWidthBytes = planeWidth * planeChannels * bytesPerDataType;

						if (plane.widthBytes() != planeWidthBytes)
						{
							allSucceeded = false;
						}

						if (plane.strideBytes() != planeWidthBytes)
						{
							allSucceeded = false;
						}

						if (plane.paddingBytes() != 0u || plane.paddingElements() != 0u)
						{
							allSucceeded = false;
						}

						if (plane.elementTypeSize() != bytesPerDataType)
						{
							allSucceeded = false;
						}

						if (plane.constdata<void>() != sourceFrameDatas[planeIndex])
						{
							allSucceeded = false;
						}

						if (plane.isOwner() != sourceIsOwner)
						{
							allSucceeded = false;
						}
					}
					else
					{
						ocean_assert(false && "Invalid plane layout!");
						allSucceeded = false;
					}
				}
			}
		}

		{
			// Move operator

			const Timestamp timestamp = Timestamp(double(RandomI::random(randomGenerator, -100, 100)));

			Frame originalFrame(frameType, Indices32(), timestamp);

			for (const bool sourceIsOwner : {true, false})
			{
				const Frame::AdvancedCopyMode copyMode = sourceIsOwner ? Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA : Frame::ACM_USE_KEEP_LAYOUT;

				Frame sourceFrame(originalFrame, copyMode);

				std::vector<const void*> sourceFrameDatas;
				for (unsigned int planeIndex = 0u; planeIndex < sourceFrame.numberPlanes(); ++planeIndex)
				{
					sourceFrameDatas.push_back(sourceFrame.constdata<void>(planeIndex));
				}

				Indices32 paddingElementsPerPlane;

				if (RandomI::random(1u) == 0u)
				{
					for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
					{
						paddingElementsPerPlane.push_back(RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(1u));
					}
				}

				Frame targetFrame(frameType, paddingElementsPerPlane, Timestamp(double(RandomI::random(randomGenerator, -100, 100))));

				targetFrame = std::move(sourceFrame);

				if (sourceFrame.isValid()) // NOLINT(bugprone-use-after-move)
				{
					allSucceeded = false;
				}

				if (targetFrame.frameType() != frameType)
				{
					allSucceeded = false;
				}

				if (targetFrame.timestamp() != timestamp)
				{
					allSucceeded = false;
				}

				for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
				{
					unsigned int planeWidth;
					unsigned int planeHeight;
					unsigned int planeChannels;

					if (FrameType::planeLayout(frameType.pixelFormat(), frameType.width(), frameType.height(), planeIndex, planeWidth, planeHeight, planeChannels))
					{
						const Frame::Plane& plane = targetFrame.planes()[planeIndex];

						if (plane.height() != planeHeight)
						{
							allSucceeded = false;
						}

						const unsigned int planeWidthBytes = planeWidth * planeChannels * bytesPerDataType;

						if (plane.widthBytes() != planeWidthBytes)
						{
							allSucceeded = false;
						}

						if (plane.strideBytes() != planeWidthBytes)
						{
							allSucceeded = false;
						}

						if (plane.paddingBytes() != 0u || plane.paddingElements() != 0u)
						{
							allSucceeded = false;
						}

						if (plane.elementTypeSize() != bytesPerDataType)
						{
							allSucceeded = false;
						}

						if (plane.constdata<void>() != sourceFrameDatas[planeIndex])
						{
							allSucceeded = false;
						}

						if (plane.isOwner() != sourceIsOwner)
						{
							allSucceeded = false;
						}
					}
					else
					{
						ocean_assert(false && "Invalid plane layout!");
						allSucceeded = false;
					}
				}
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testCopyOperator(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing copy operators:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_ABGR32,
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_Y10,
		FrameType::FORMAT_Y32,
		FrameType::FORMAT_RGB5551,
		FrameType::FORMAT_Y_UV12,
		FrameType::FORMAT_F64,
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<float, 3u>(),
	};

	do
	{
		const FrameType frameType(randomizedFrameType(pixelFormats, &randomGenerator));

		Indices32 paddingElementsPerPlane;

		if (RandomI::random(1u) == 0u)
		{
			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				paddingElementsPerPlane.push_back(RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(1u));
			}
		}

		Frame sourceFrameOwner(frameType, paddingElementsPerPlane);

		FrameType intermediateFrameType;

		if (RandomI::random(randomGenerator, 1u) == 1u)
		{
			intermediateFrameType = randomizedFrameType(pixelFormats, &randomGenerator);
		}

		{
			// source is owner, copy will be owner

			Frame frameCopy = intermediateFrameType.isValid() ? Frame(intermediateFrameType) : Frame();

			frameCopy = sourceFrameOwner;

			if (!frameCopy.isValid())
			{
				allSucceeded = false;
			}

			if (frameCopy.frameType() != sourceFrameOwner.frameType())
			{
				allSucceeded = false;
			}

			if (!frameCopy.isOwner())
			{
				allSucceeded = false;
			}

			if (frameCopy.isReadOnly())
			{
				allSucceeded = false;
			}

			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				if (frameCopy.paddingElements(planeIndex) != 0u)
				{
					allSucceeded = false;
				}

				if (frameCopy.constdata<void>(planeIndex) == sourceFrameOwner.constdata<void>(planeIndex))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// source is owner, copy will be owner

			const Frame& constSourceFrameOwner = sourceFrameOwner;

			Frame frameCopy = intermediateFrameType.isValid() ? Frame(intermediateFrameType) : Frame();

			frameCopy = constSourceFrameOwner;

			if (!frameCopy.isValid())
			{
				allSucceeded = false;
			}

			if (frameCopy.frameType() != sourceFrameOwner.frameType())
			{
				allSucceeded = false;
			}

			if (!frameCopy.isOwner())
			{
				allSucceeded = false;
			}

			if (frameCopy.isReadOnly())
			{
				allSucceeded = false;
			}

			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				if (frameCopy.paddingElements(planeIndex) != 0u)
				{
					allSucceeded = false;
				}

				if (frameCopy.constdata<void>(planeIndex) == sourceFrameOwner.constdata<void>(planeIndex))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// source is not owner, copy will not be owner

			Frame::PlaneInitializers<void> planeInitializers;

			for (unsigned int planeIndex = 0u; planeIndex < sourceFrameOwner.numberPlanes(); ++planeIndex)
			{
				planeInitializers.emplace_back(sourceFrameOwner.constdata<void>(planeIndex), Frame::CM_USE_KEEP_LAYOUT, sourceFrameOwner.paddingElements(planeIndex));
			}

			const Frame constNotOwnerFrame(sourceFrameOwner.frameType(), planeInitializers);

			Frame usedFrame = intermediateFrameType.isValid() ? Frame(intermediateFrameType) : Frame();

			usedFrame = constNotOwnerFrame;

			if (!usedFrame.isValid())
			{
				allSucceeded = false;
			}

			if (usedFrame.frameType() != sourceFrameOwner.frameType())
			{
				allSucceeded = false;
			}

			if (usedFrame.isOwner())
			{
				allSucceeded = false;
			}

			if (!usedFrame.isReadOnly())
			{
				allSucceeded = false;
			}

			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				const unsigned int paddingElements = paddingElementsPerPlane.empty() ? 0u : paddingElementsPerPlane[planeIndex];

				if (usedFrame.paddingElements(planeIndex) != paddingElements)
				{
					allSucceeded = false;
				}

				if (usedFrame.constdata<void>(planeIndex) != sourceFrameOwner.constdata<void>(planeIndex))
				{
					allSucceeded = false;
				}
			}
		}

		{
			// source is not owner, copy will not be owner

			Frame::PlaneInitializers<void> planeInitializers;

			for (unsigned int planeIndex = 0u; planeIndex < sourceFrameOwner.numberPlanes(); ++planeIndex)
			{
				planeInitializers.emplace_back(sourceFrameOwner.data<void>(planeIndex), Frame::CM_USE_KEEP_LAYOUT, sourceFrameOwner.paddingElements(planeIndex));
			}

			const Frame nonconstNotOwnerFrame(sourceFrameOwner.frameType(), planeInitializers);

			Frame usedFrame = intermediateFrameType.isValid() ? Frame(intermediateFrameType) : Frame();

			usedFrame = nonconstNotOwnerFrame;

			if (!usedFrame.isValid())
			{
				allSucceeded = false;
			}

			if (usedFrame.frameType() != sourceFrameOwner.frameType())
			{
				allSucceeded = false;
			}

			if (usedFrame.isOwner())
			{
				allSucceeded = false;
			}

			if (usedFrame.isReadOnly())
			{
				allSucceeded = false;
			}

			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				const unsigned int paddingElements = paddingElementsPerPlane.empty() ? 0u : paddingElementsPerPlane[planeIndex];

				if (usedFrame.paddingElements(planeIndex) != paddingElements)
				{
					allSucceeded = false;
				}

				if (usedFrame.constdata<void>(planeIndex) != sourceFrameOwner.constdata<void>(planeIndex))
				{
					allSucceeded = false;
				}
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testPlaneInitializer(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing plane initializer:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		{
			const FrameType::PixelFormats pixelFormats =
			{
				FrameType::FORMAT_RGB24,
				FrameType::FORMAT_BGRA32,
				FrameType::FORMAT_Y8,
				FrameType::FORMAT_Y10,
				FrameType::FORMAT_Y32,
				FrameType::FORMAT_RGB565,
				FrameType::FORMAT_UYVY16,
				FrameType::FORMAT_YUYV16,
				FrameType::genericPixelFormat<uint8_t, 5u>(),
				FrameType::genericPixelFormat<int16_t, 5u>(),
				FrameType::genericPixelFormat<float, 1u>(),
				FrameType::FORMAT_Y_UV12,
				FrameType::FORMAT_Y_U_V12,
				FrameType::FORMAT_Y_U_V24,
				FrameType::FORMAT_F64
			};

			const FrameType::PixelFormat pixelFormat = pixelFormats[RandomI::random(randomGenerator, (unsigned int)(pixelFormats.size() - 1))];
			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const unsigned int widthMultiple = FrameType::widthMultiple(pixelFormat);
			const unsigned int heightMultiple = FrameType::heightMultiple(pixelFormat);

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u) * widthMultiple;
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u) * heightMultiple;

			const FrameType frameType(width, height, pixelFormat, pixelOrigin);

			switch (frameType.dataType())
			{
				case FrameType::DT_UNSIGNED_INTEGER_8:
				{
					if (!validatePlaneInitializer<uint8_t>(frameType, randomGenerator))
					{
						allSucceeded = false;
					}

					break;
				}

				case FrameType::DT_UNSIGNED_INTEGER_16:
				{
					if (!validatePlaneInitializer<uint16_t>(frameType, randomGenerator))
					{
						allSucceeded = false;
					}

					break;
				}

				case FrameType::DT_SIGNED_INTEGER_16:
				{
					if (!validatePlaneInitializer<int16_t>(frameType, randomGenerator))
					{
						allSucceeded = false;
					}

					break;
				}

				case FrameType::DT_UNSIGNED_INTEGER_32:
				{
					if (!validatePlaneInitializer<uint32_t>(frameType, randomGenerator))
					{
						allSucceeded = false;
					}

					break;
				}

				case FrameType::DT_SIGNED_FLOAT_32:
				{
					if (!validatePlaneInitializer<float>(frameType, randomGenerator))
					{
						allSucceeded = false;
					}

					break;
				}

				case FrameType::DT_SIGNED_FLOAT_64:
				{
					if (!validatePlaneInitializer<double>(frameType, randomGenerator))
					{
						allSucceeded = false;
					}

					break;
				}

				default:
					ocean_assert(false && "Invalid data type!");
					allSucceeded = false;
					break;
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testPlaneLayout(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing plane layout:";

	bool allSucceeded = true;

	const FrameType::PixelFormats genericPixelFormats =
	{
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<uint8_t, 3u>(),
		FrameType::genericPixelFormat<int16_t, 2u>(),
		FrameType::genericPixelFormat<float, 4u>(),
		FrameType::genericPixelFormat<uint32_t, 3u, 2u>(),
	};

	const FrameType::PixelFormats pixelFormats = definedPixelFormats(genericPixelFormats);

	const Timestamp startTimestamp(true);

	do
	{
		for (const FrameType::PixelFormat pixelFormat : pixelFormats)
		{
			const unsigned int width = RandomI::random(1u, 1920u) * FrameType::widthMultiple(pixelFormat);
			const unsigned int height = RandomI::random(1u, 1080u) * FrameType::heightMultiple(pixelFormat);

			Indices32 paddingElementsPerPlane;

			if (RandomI::random(1u) == 0u)
			{
				for (unsigned int n = 0u; n < FrameType::numberPlanes(pixelFormat); ++n)
				{
					paddingElementsPerPlane.emplace_back(RandomI::random(1u, 100u) * RandomI::random(1u));
				}
			}

			const Frame frame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), paddingElementsPerPlane);

			for (unsigned int planeIndex = 0u; planeIndex < frame.numberPlanes(); ++planeIndex)
			{
				unsigned int planeWidth = 0u;
				unsigned int planeHeight = 0u;
				unsigned int planeChannels = 0u;

				unsigned int planeWidthMultiple = 0u;
				unsigned int planeHeightMultiple = 0u;

				if (FrameType::planeLayout(frame.frameType(), planeIndex, planeWidth, planeHeight, planeChannels, &planeWidthMultiple, &planeHeightMultiple))
				{
					unsigned int expectedPlaneWidth = width;
					unsigned int expectedPlaneHeight = height;
					unsigned int expectedPlaneChannels = frame.channels();

					unsigned int expectedPlaneWidthMultiple = 1u;
					unsigned int expectedPlaneHeightMultiple = 1u;

					switch (pixelFormat)
					{
						case FrameType::FORMAT_BGR4444:
						case FrameType::FORMAT_BGR5551:
						case FrameType::FORMAT_BGR565:
						case FrameType::FORMAT_BGRA4444:
						case FrameType::FORMAT_RGB4444:
						case FrameType::FORMAT_RGB5551:
						case FrameType::FORMAT_RGB565:
						case FrameType::FORMAT_RGBA4444:
						{
							expectedPlaneChannels = 1u;
							break;
						}

						case FrameType::FORMAT_UYVY16:
						case FrameType::FORMAT_YUYV16:
						{
							expectedPlaneChannels = 2u;
							break;
						}

						case FrameType::FORMAT_Y_U_V24_LIMITED_RANGE:
						case FrameType::FORMAT_Y_U_V24_FULL_RANGE:
						{
							expectedPlaneChannels = 1u;
							break;
						}

						case FrameType::FORMAT_BGGR10_PACKED:
						case FrameType::FORMAT_RGGB10_PACKED:
						{
							ocean_assert(width % 4u == 0u);
							ocean_assert(height % 2u == 0u);

							expectedPlaneChannels = 1u;

							expectedPlaneWidth = width * 5u / 4u;

							expectedPlaneWidthMultiple = 5u;
							expectedPlaneHeightMultiple = 2u;

							break;
						}

						case FrameType::FORMAT_Y10_PACKED:
						{
							ocean_assert(width % 4u == 0u);
							expectedPlaneWidth = width * 5u / 4u;

							expectedPlaneWidthMultiple = 5u;

							break;
						}

						case FrameType::FORMAT_Y_UV12_LIMITED_RANGE:
						case FrameType::FORMAT_Y_UV12_FULL_RANGE:
						case FrameType::FORMAT_Y_VU12_LIMITED_RANGE:
						case FrameType::FORMAT_Y_VU12_FULL_RANGE:
						{
							if (planeIndex == 0u)
							{
								expectedPlaneChannels = 1u;
							}
							else
							{
								expectedPlaneChannels = 2u;

								ocean_assert(width % 2u == 0u && height % 2u == 0u);
								expectedPlaneWidth = width / 2u;
								expectedPlaneHeight = height / 2u;
							}
							break;
						}


						case FrameType::FORMAT_Y_U_V12_LIMITED_RANGE:
						case FrameType::FORMAT_Y_U_V12_FULL_RANGE:
						case FrameType::FORMAT_Y_V_U12_LIMITED_RANGE:
						case FrameType::FORMAT_Y_V_U12_FULL_RANGE:
						{
							expectedPlaneChannels = 1u;

							if (planeIndex >= 1u)
							{
								ocean_assert(width % 2u == 0u && height % 2u == 0u);
								expectedPlaneWidth = width / 2u;
								expectedPlaneHeight = height / 2u;
							}
							break;
						}

						default:
							break;
					}

					if (planeWidth != expectedPlaneWidth)
					{
						allSucceeded = false;
					}

					if (planeHeight != expectedPlaneHeight)
					{
						allSucceeded = false;
					}

					if (planeChannels != expectedPlaneChannels)
					{
						allSucceeded = false;
					}

					if (planeWidthMultiple != expectedPlaneWidthMultiple)
					{
						allSucceeded = false;
					}

					if (planeHeightMultiple != expectedPlaneHeightMultiple)
					{
						allSucceeded = false;
					}
				}
				else
				{
					allSucceeded = false;
				}
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testRelease(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing release:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const FrameType::PixelFormats genericPixelFormats =
	{
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<uint8_t, 3u>(),
		FrameType::genericPixelFormat<int16_t, 2u>(),
		FrameType::genericPixelFormat<float, 4u>(),
		FrameType::genericPixelFormat<uint32_t, 3u, 2u>(),
	};

	const FrameType::PixelFormats pixelFormats = definedPixelFormats(genericPixelFormats);

	const Timestamp startTimestamp(true);

	do
	{
		Frame frame(randomizedFrameType(pixelFormats, &randomGenerator));

		if (!frame.isValid())
		{
			OCEAN_SET_FAILED(validation);
		}

		frame.release();

		if (frame.isValid() || frame.planes().size() != 1u)
		{
			OCEAN_SET_FAILED(validation);
		}

		frame.release(); // just a second call

		if (frame.isValid() || frame.planes().size() != 1u)
		{
			OCEAN_SET_FAILED(validation);
		}

		const bool forceOwner = RandomI::random(randomGenerator, 1u) == 0u;
		const bool forceWritable = RandomI::random(randomGenerator, 1u) == 0u;

		const bool setResult = frame.set(randomizedFrameType(pixelFormats, &randomGenerator), forceOwner, forceWritable);

		OCEAN_EXPECT_TRUE(validation, setResult);

		if (!frame.isValid())
		{
			OCEAN_SET_FAILED(validation);
		}


		frame.release();

		if (frame.isValid() || frame.planes().size() != 1u)
		{
			OCEAN_SET_FAILED(validation);
		}

		frame = Frame(randomizedFrameType(pixelFormats, &randomGenerator));

		if (!frame.isValid())
		{
			OCEAN_SET_FAILED(validation);
		}

		frame.release();

		if (frame.isValid() || frame.planes().size() != 1u)
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFrame::testSubFrame(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing sub-frame extraction:";

	bool allSucceeded = true;

	const FrameType::PixelFormats genericPixelFormats =
	{
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<uint8_t, 3u>(),
		FrameType::genericPixelFormat<int16_t, 2u>(),
		FrameType::genericPixelFormat<float, 4u>(),
		FrameType::genericPixelFormat<uint32_t, 3u, 2u>(),
	};

	const FrameType::PixelFormats pixelFormats = definedPixelFormats(genericPixelFormats);

	const std::vector<Frame::CopyMode> copyModes =
	{
		Frame::CM_USE_KEEP_LAYOUT,
		Frame::CM_COPY_REMOVE_PADDING_LAYOUT,
		Frame::CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA
		// Frame::CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA not supported as copy mode in subFrame()
	};

	const Timestamp startTimestamp(true);

	do
	{
		const FrameType::PixelFormat pixelFormat = RandomI::random(pixelFormats);
		const unsigned int bytesPerElement = FrameType::bytesPerDataType(FrameType::dataType(pixelFormat));

		const unsigned int widthMultiple = FrameType::widthMultiple(pixelFormat);
		const unsigned int heightMultiple = FrameType::heightMultiple(pixelFormat);

		const unsigned int width = RandomI::random(1u, 1920u) * widthMultiple;
		const unsigned int height = RandomI::random(1u, 1080u) * heightMultiple;

		const FrameType::PixelOrigin pixelOrigin = RandomI::random({FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const FrameType frameType(width, height, pixelFormat, pixelOrigin);

		Frame frame;

		const Timestamp frameTimestamp(double(RandomI::random(1000u)));

		Indices32 paddingElementsPerPlane;

		if (RandomI::random(1u) == 0u)
		{
			Frame::PlaneInitializers<void> planeInitializers;

			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				const unsigned int paddingElements = RandomI::random(0u, 100u) * RandomI::random(1u);

				paddingElementsPerPlane.emplace_back(paddingElements);
				planeInitializers.emplace_back(paddingElements);
			}

			frame = Frame(frameType, planeInitializers, frameTimestamp);
		}
		else
		{
			frame = Frame(frameType);
			frame.setTimestamp(frameTimestamp);

			paddingElementsPerPlane = Indices32(frameType.numberPlanes(), 0u);
		}

		ocean_assert(frame.isOwner());
		ocean_assert(!frame.isReadOnly());

		ocean_assert(frame.width() % widthMultiple == 0u);
		ocean_assert(frame.height() % heightMultiple == 0u);

		const unsigned int subFrameWidth = RandomI::random(1u, width / widthMultiple) * widthMultiple;
		const unsigned int subFrameHeight = RandomI::random(1u, height / heightMultiple) * heightMultiple;

		const unsigned int subFrameLeft = FrameType::formatIsPacked(pixelFormat) ? 0u : RandomI::random(0u, (width - subFrameWidth) / widthMultiple) * widthMultiple;
		const unsigned int subFrameTop = RandomI::random(0u, (height - subFrameHeight) / heightMultiple) * heightMultiple;

		ocean_assert(subFrameLeft + subFrameWidth <= width);
		ocean_assert(subFrameTop + subFrameHeight <= height);

		const FrameType subFrameType(frameType, subFrameWidth, subFrameHeight);

		for (const Frame::CopyMode copyMode : copyModes)
		{
			Frame subFrame = frame.subFrame(subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, copyMode);

			// just checking whether 'frame' still has the correct specification
			if (!validateFrameSpecification(frame, frameType, paddingElementsPerPlane, bytesPerElement, true, false))
			{
				allSucceeded = false;
			}

			Indices32 expectedSubFramePaddingElementsPerPlane;

			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				const unsigned int planePaddingElements = paddingElementsPerPlane[planeIndex];

				unsigned int planeWidthMultipleOffset = 0u;
				unsigned int planeHeightMultipleOffset = 0u;
				unsigned int planeChannelsDummy = 0u;

				if (!FrameType::planeLayout(pixelFormat, widthMultiple, heightMultiple, planeIndex, planeWidthMultipleOffset, planeHeightMultipleOffset, planeChannelsDummy))
				{
					ocean_assert(false && "Invalid parameter!");
					allSucceeded = false;

					break;
				}

				unsigned int planeSubFrameLeft = 0u;
				unsigned int planeSubFrameTop = 0u;

				if (!FrameType::planeLayout(pixelFormat, widthMultiple + subFrameLeft, heightMultiple + subFrameTop, planeIndex, planeSubFrameLeft, planeSubFrameTop, planeChannelsDummy)) // planeLayout() cannot be called with subFrameLeft == 0 or subFrameTop == 0
				{
					ocean_assert(false && "Invalid parameter!");
					allSucceeded = false;

					break;
				}

				ocean_assert(planeSubFrameLeft >= planeWidthMultipleOffset);
				ocean_assert(planeSubFrameTop >= planeHeightMultipleOffset);
				planeSubFrameLeft -= planeWidthMultipleOffset;
				planeSubFrameTop -= planeHeightMultipleOffset;

				unsigned int planeSubFrameWidth = 0u;
				unsigned int planeSubFrameHeight = 0u;

				if (!FrameType::planeLayout(pixelFormat, subFrameWidth, subFrameHeight, planeIndex, planeSubFrameWidth, planeSubFrameHeight, planeChannelsDummy))
				{
					ocean_assert(false && "Invalid parameter!");
					allSucceeded = false;

					break;
				}

				const Frame::Plane& currentPlane = frame.planes()[planeIndex];

				unsigned int expectedSubFramePaddingElements = 0u;
				unsigned int expectedSubFrameStrideElements = 0u;

				switch (copyMode)
				{
					case Frame::CM_USE_KEEP_LAYOUT:
						expectedSubFramePaddingElements = (currentPlane.width() - planeSubFrameWidth) * currentPlane.channels() + planePaddingElements;
						expectedSubFrameStrideElements = currentPlane.width() * currentPlane.channels() + planePaddingElements;
						break;

					case Frame::CM_COPY_REMOVE_PADDING_LAYOUT:
						expectedSubFramePaddingElements = 0u;
						expectedSubFrameStrideElements = planeSubFrameWidth * currentPlane.channels();
						break;

					case Frame::CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA:
						expectedSubFramePaddingElements = (currentPlane.width() - planeSubFrameWidth) * currentPlane.channels() + planePaddingElements;
						expectedSubFrameStrideElements = currentPlane.width() * currentPlane.channels() + planePaddingElements;
						break;

					case Frame::CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA:
						ocean_assert(false && "Invalid copy mode!");
						allSucceeded = false;
						break;
				}

				ocean_assert(expectedSubFrameStrideElements != 0u);

				expectedSubFramePaddingElementsPerPlane.emplace_back(expectedSubFramePaddingElements);

				// checking whether the sub frame has the correct starting pointer

				const uint8_t* expectedPlaneSubFrameStartPointer = (const uint8_t*)(frame.constdata<void>(planeIndex)) + planeSubFrameTop * (currentPlane.width() * currentPlane.channels() + planePaddingElements) * bytesPerElement + (planeSubFrameLeft * currentPlane.channels()) * bytesPerElement;
				const uint8_t* planeSubFrameStartPointer = (const uint8_t*)(subFrame.constdata<void>(planeIndex));

				const bool expectedIsOwner = copyMode != Frame::CM_USE_KEEP_LAYOUT;

				if (expectedIsOwner == false)
				{
					if (expectedPlaneSubFrameStartPointer != planeSubFrameStartPointer)
					{
						allSucceeded = false;
					}
				}
				else
				{
					for (unsigned int n = 0u; n < frame.numberPlanes(); ++n)
					{
						if (planeSubFrameStartPointer >= (const uint8_t*)(frame.constdata<void>(planeIndex)) && planeSubFrameStartPointer < (const uint8_t*)(frame.constdata<void>(planeIndex)) + frame.planes()[planeIndex].size())
						{
							// overlapping memory
							allSucceeded = false;
						}
					}
				}

				// we check the memory

				for (unsigned int y = 0u; y < planeSubFrameHeight; ++y)
				{
					const void* rowPointerLargeFrame = expectedPlaneSubFrameStartPointer + y * (currentPlane.width() * currentPlane.channels() + planePaddingElements) * bytesPerElement;
					const void* rowPointerSubFrame = (const uint8_t*)(subFrame.constdata<void>(planeIndex)) + y * expectedSubFrameStrideElements * bytesPerElement;

					if (rowPointerSubFrame != subFrame.constrow<void>(y, planeIndex))
					{
						allSucceeded = false;
					}

					if (memcmp(rowPointerLargeFrame, rowPointerSubFrame, (planeSubFrameWidth * currentPlane.channels()) * bytesPerElement) != 0)
					{
						allSucceeded = false;
					}
				}
			}

			const bool expectedIsOwner = copyMode != Frame::CM_USE_KEEP_LAYOUT;
			constexpr bool expectedIsReadOnly = false;

			// just checking whether 'subFrame' has the correct specification
			if (!validateFrameSpecification(subFrame, subFrameType, expectedSubFramePaddingElementsPerPlane, bytesPerElement, expectedIsOwner, expectedIsReadOnly))
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testTimestamp(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing timestamp:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	const Frame invalidFrame;

	if (invalidFrame.timestamp().isValid())
	{
		allSucceeded = false;
	}

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u);
		const unsigned int height = RandomI::random(1u, 1080u);

		const FrameType frameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT);

		const unsigned int paddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);

		const Timestamp timestamp(double(RandomI::random(-100, 100)));

		Frame frame(frameType, paddingElements, timestamp);

		if (frame.timestamp() != timestamp)
		{
			allSucceeded = false;
		}

		const Frame copiedFrame(frame);

		if (copiedFrame.timestamp() != timestamp)
		{
			allSucceeded = false;
		}

		Frame movedFrame(std::move(frame));

		if (movedFrame.timestamp() != timestamp)
		{
			allSucceeded = false;
		}

		if (frame.isValid() || frame.timestamp().isValid()) // NOLINT(bugprone-use-after-move)
		{
			allSucceeded = false;
		}

		movedFrame.release();

		if (movedFrame.timestamp().isValid())
		{
			allSucceeded = false;
		}

		Frame assignedFrame(frameType, paddingElements);

		if (assignedFrame.timestamp().isValid())
		{
			allSucceeded = false;
		}

		assignedFrame = copiedFrame;

		if (assignedFrame.timestamp() != timestamp)
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testAccessorsDataTypes(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing row and pixel accessors with data types:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u);
		const unsigned int height = RandomI::random(1u, 1080u);

		const FrameType::DataType dataType = RandomI::random(FrameType::definedDataTypes());
		const unsigned int channels = RandomI::random(1u, 5u);
		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(dataType, channels);
		const unsigned int bytesPerElement = FrameType::bytesPerDataType(dataType);

		const FrameType::PixelOrigin pixelOrigin = RandomI::random({FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const unsigned int framePaddingElements = RandomI::random(0u, 100u);

		const FrameType frameType(width, height, pixelFormat, pixelOrigin);
		Frame frame(frameType, framePaddingElements);

		const unsigned int frameStrideBytes = (width * channels + framePaddingElements) * bytesPerElement;

		// testing row accessors

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int y = RandomI::random(0u, height - 1u);

			const uint8_t* const constRowPointer = frame.constrow<uint8_t>(y);
			uint8_t* const rowPointer = frame.row<uint8_t>(y);

			const int8_t* const constRowPointerInt8 = frame.constrow<int8_t>(y);
			const uint8_t* const constRowPointerUint8 = frame.constrow<uint8_t>(y);
			const int16_t* const constRowPointerInt16 = frame.constrow<int16_t>(y);
			const uint16_t* const constRowPointerUint16 = frame.constrow<uint16_t>(y);
			const int32_t* const constRowPointerInt32 = frame.constrow<int32_t>(y);
			const uint32_t* const constRowPointerUint32 = frame.constrow<uint32_t>(y);
			const float* const constRowPointerFloat = frame.constrow<float>(y);
			const double* const constRowPointerDouble = frame.constrow<double>(y);
			const int64_t* const constRowPointerInt64 = frame.constrow<int64_t>(y);
			const uint64_t* const constRowPointerUint64 = frame.constrow<uint64_t>(y);

			const uint8_t* const testPointer = frame.constdata<uint8_t>() + frameStrideBytes * y;

			if (testPointer != constRowPointer)
			{
				allSucceeded = false;
			}

			if ((uint8_t*)(testPointer) != rowPointer)
			{
				allSucceeded = false;
			}

			if ((const int8_t*)(testPointer) != constRowPointerInt8)
			{
				allSucceeded = false;
			}

			if (testPointer != constRowPointerUint8)
			{
				allSucceeded = false;
			}

			if ((const int16_t*)(testPointer) != constRowPointerInt16)
			{
				allSucceeded = false;
			}

			if ((const uint16_t*)(testPointer) != constRowPointerUint16)
			{
				allSucceeded = false;
			}

			if ((const int32_t*)(testPointer) != constRowPointerInt32)
			{
				allSucceeded = false;
			}

			if ((const uint32_t*)(testPointer) != constRowPointerUint32)
			{
				allSucceeded = false;
			}

			if ((const float*)(testPointer) != constRowPointerFloat)
			{
				allSucceeded = false;
			}

			if ((const double*)(testPointer) != constRowPointerDouble)
			{
				allSucceeded = false;
			}

			if ((const int64_t*)(testPointer) != constRowPointerInt64)
			{
				allSucceeded = false;
			}

			if ((const uint64_t*)(testPointer) != constRowPointerUint64)
			{
				allSucceeded = false;
			}
		}


		// testing pixel accessors

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			const unsigned int x = RandomI::random(0u, width - 1u) / bytesPerElement;
			const unsigned int y = RandomI::random(0u, height - 1u);

			const uint8_t* const testRowPointer = frame.constdata<uint8_t>() + frameStrideBytes * y;

			if (dataType == FrameType::DT_UNSIGNED_INTEGER_8)
			{
				const uint8_t* const constPixelPointer = frame.constpixel<uint8_t>(x, y);

				if ((testRowPointer + x * channels) != constPixelPointer)
				{
					allSucceeded = false;
				}

				uint8_t* const pixelPointer = frame.pixel<uint8_t>(x, y);

				if (((uint8_t*)(testRowPointer) + x * channels) != pixelPointer)
				{
					allSucceeded = false;
				}

				const uint8_t* const constPixelPointerUint8 = frame.constpixel<uint8_t>(x, y);

				if ((testRowPointer + x * channels) != constPixelPointerUint8)
				{
					allSucceeded = false;
				}
			}

			if (dataType == FrameType::DT_SIGNED_INTEGER_8)
			{
				const int8_t* const constPixelPointerInt8 = frame.constpixel<int8_t>(x, y);

				if (((const int8_t*)(testRowPointer) + x * channels) != constPixelPointerInt8)
				{
					allSucceeded = false;
				}
			}

			if (dataType == FrameType::DT_SIGNED_INTEGER_16)
			{
				const int16_t* const constPixelPointerInt16 = frame.constpixel<int16_t>(x, y);

				if (((const int16_t*)(testRowPointer) + x * channels) != constPixelPointerInt16)
				{
					allSucceeded = false;
				}
			}

			if (dataType == FrameType::DT_UNSIGNED_INTEGER_16)
			{
				const uint16_t* const constPixelPointerUint16 = frame.constpixel<uint16_t>(x, y);

				if (((const uint16_t*)(testRowPointer) + x * channels) != constPixelPointerUint16)
				{
					allSucceeded = false;
				}
			}

			if (dataType == FrameType::DT_SIGNED_INTEGER_32)
			{
				const int32_t* const constPixelPointerInt32 = frame.constpixel<int32_t>(x, y);

				if (((const int32_t*)(testRowPointer) + x * channels) != constPixelPointerInt32)
				{
					allSucceeded = false;
				}
			}

			if (dataType == FrameType::DT_UNSIGNED_INTEGER_32)
			{
				const uint32_t* const constPixelPointerUint32 = frame.constpixel<uint32_t>(x, y);

				if (((const uint32_t*)(testRowPointer) + x * channels) != constPixelPointerUint32)
				{
					allSucceeded = false;
				}
			}

			if (dataType == FrameType::DT_SIGNED_FLOAT_32)
			{
				const float* const constFloatPixelPointer = frame.constpixel<float>(x, y);

				if (((const float*)(testRowPointer) + x * channels) != constFloatPixelPointer)
				{
					allSucceeded = false;
				}
			}

			if (dataType == FrameType::DT_SIGNED_FLOAT_64)
			{
				const double* const constDoublePixelPointer = frame.constpixel<double>(x, y);

				if (((const double*)(testRowPointer) + x * channels) != constDoublePixelPointer)
				{
					allSucceeded = false;
				}
			}

			if (dataType == FrameType::DT_SIGNED_INTEGER_64)
			{
				const int64_t* const constPixelPointerInt64 = frame.constpixel<int64_t>(x, y);

				if (((const int64_t*)(testRowPointer) + x * channels) != constPixelPointerInt64)
				{
					allSucceeded = false;
				}
			}

			if (dataType == FrameType::DT_UNSIGNED_INTEGER_64)
			{
				const uint64_t* const constPixelPointerUint64 = frame.constpixel<uint64_t>(x, y);

				if (((const uint64_t*)(testRowPointer) + x * channels) != constPixelPointerUint64)
				{
					allSucceeded = false;
				}
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testAccessorsPixelFormats(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing row and pixel accessors with pixel formats:";

	bool allSucceeded = true;

	constexpr unsigned int testsPerPlane = 100u;

	const FrameType::PixelFormats pixelFormats = FrameType::definedPixelFormats();

	const Timestamp startTimestamp(true);

	do
	{
		const FrameType::PixelFormat pixelFormat = RandomI::random(FrameType::definedPixelFormats());

		const unsigned int widthMultiple = FrameType::widthMultiple(pixelFormat);
		const unsigned int heightMultiple = FrameType::heightMultiple(pixelFormat);

		ocean_assert(widthMultiple != 0u && heightMultiple != 0u);

		const unsigned int width = RandomI::random(1u, 1920u / widthMultiple) * widthMultiple;
		const unsigned int height = RandomI::random(1u, 1080u / heightMultiple) * heightMultiple;

		const FrameType::PixelOrigin pixelOrigin = RandomI::random({FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const FrameType frameType(width, height, pixelFormat, pixelOrigin);
		ocean_assert(frameType.isValid());

		const unsigned int numberPlanes = frameType.numberPlanes();
		ocean_assert(numberPlanes != 0u);

		Indices32 paddingElementsPerPlane;

		if (RandomI::random(1u) == 0u)
		{
			for (unsigned int n = 0u; n < numberPlanes; ++n)
			{
				paddingElementsPerPlane.emplace_back(RandomI::random(0u, 100u) * RandomI::random(1u));
			}
		}

		Frame frame(frameType, paddingElementsPerPlane);

		if (FrameType::formatIsGeneric(pixelFormat) || numberPlanes > 1u)
		{
			// we have either a generic pixel format like FORMAT_Y8, FORMAT_RGB24, FORMAT_ABGR32, etc.,
			// or a standard multi-plane frame like Y_UV12, _Y_U_V12, Y_U_V24 etc.
			// both types are simple to test

			const unsigned int bytesPerDataType = frameType.bytesPerDataType();

			for (unsigned int planeIndex = 0u; planeIndex < numberPlanes; ++planeIndex)
			{
				const unsigned int planeWidth = frame.planeWidth(planeIndex);
				const unsigned int planeHeight = frame.planeHeight(planeIndex);

				const unsigned int planeChannels = frame.planeChannels(planeIndex);
				const unsigned int planePaddingElements = frame.paddingElements(planeIndex);

				const unsigned int planeStrideElements = planeWidth * planeChannels + planePaddingElements;

				const unsigned int planeStrideBytes = planeStrideElements * bytesPerDataType;

				const void* const data = frame.constdata<void>(planeIndex);

				for (unsigned int n = 0u; n < testsPerPlane; ++n)
				{
					const unsigned int x = RandomI::random(planeWidth - 1u);
					const unsigned int y = RandomI::random(planeHeight - 1u);

					const void* const testRow = (const void*)((const uint8_t*)(data) + y * planeStrideBytes);
					const void* const row = frame.constrow<void>(y, planeIndex);

					if (row != testRow)
					{
						allSucceeded = false;
					}

					if (frame.constrow<void>(y, planeIndex) != (const void*)(frame.row<void>(y, planeIndex)))
					{
						allSucceeded = false;
					}

					const void* const testPixel = (const void*)((const uint8_t*)(testRow) + (x * planeChannels) * bytesPerDataType);
					const void* const pixel = frame.constpixel<void>(x, y, planeIndex);

					if (pixel != testPixel)
					{
						allSucceeded = false;
					}

					if (frame.constpixel<void>(x, y, planeIndex) != (const void*)(frame.pixel<void>(x, y, planeIndex)))
					{
						allSucceeded = false;
					}
				}
			}
		}
		else
		{
			// we have a special pixel format

			ocean_assert(numberPlanes == 1u);

			if (numberPlanes == 1u)
			{
				switch (pixelFormat)
				{
					case FrameType::FORMAT_BGR4444:
					case FrameType::FORMAT_BGRA4444:
					case FrameType::FORMAT_RGB4444:
					case FrameType::FORMAT_RGBA4444:
					case FrameType::FORMAT_BGR5551:
					case FrameType::FORMAT_RGB5551:
					case FrameType::FORMAT_BGR565:
					case FrameType::FORMAT_RGB565:
					{
						const unsigned int elementsPerPixel = 1u;

						const unsigned int strideElements = width * elementsPerPixel + frame.paddingElements();

						const unsigned int bytesPerElement = 2u;

						const unsigned int strideBytes = strideElements * bytesPerElement;

						ocean_assert(frame.planeChannels(0u) == elementsPerPixel);
						ocean_assert(frame.bytesPerDataType() == bytesPerElement);

						const void* const data = frame.constdata<void>();

						for (unsigned int n = 0u; n < testsPerPlane; ++n)
						{
							const unsigned int x = RandomI::random(width - 1u);
							const unsigned int y = RandomI::random(height - 1u);

							const void* const testRow = (const void*)((const uint8_t*)(data) + y * strideBytes);
							const void* const row = frame.constrow<void>(y);

							if (row != testRow)
							{
								allSucceeded = false;
							}

							if (frame.constrow<void>(y) != (const void*)(frame.row<void>(y)))
							{
								allSucceeded = false;
							}

							const void* const testPixel = (const void*)((const uint8_t*)(testRow) + (x * elementsPerPixel) * bytesPerElement);
							const void* const pixel = frame.constpixel<void>(x, y);

							if (pixel != testPixel)
							{
								allSucceeded = false;
							}

							if (frame.constpixel<void>(x, y) != (const void*)(frame.pixel<void>(x, y)))
							{
								allSucceeded = false;
							}
						}

						break;
					}

					case FrameType::FORMAT_UYVY16:
					case FrameType::FORMAT_YUYV16:
					{
						const unsigned int elementsPerPixel = 2u;

						const unsigned int strideElements = width * elementsPerPixel + frame.paddingElements();

						const unsigned int bytesPerElement = 1u;

						const unsigned int strideBytes = strideElements * bytesPerElement;

						ocean_assert(frame.planeChannels(0u) == elementsPerPixel);
						ocean_assert(frame.bytesPerDataType() == bytesPerElement);

						const void* const data = frame.constdata<void>();

						for (unsigned int n = 0u; n < testsPerPlane; ++n)
						{
							const unsigned int x = RandomI::random(width - 1u);
							const unsigned int y = RandomI::random(height - 1u);

							const void* const testRow = (const void*)((const uint8_t*)(data) + y * strideBytes);
							const void* const row = frame.constrow<void>(y);

							if (row != testRow)
							{
								allSucceeded = false;
							}

							if (frame.constrow<void>(y) != (const void*)(frame.row<void>(y)))
							{
								allSucceeded = false;
							}

							const void* const testPixel = (const void*)((const uint8_t*)(testRow) + (x * elementsPerPixel) * bytesPerElement);
							const void* const pixel = frame.constpixel<void>(x, y);

							if (pixel != testPixel)
							{
								allSucceeded = false;
							}

							if (frame.constpixel<void>(x, y) != (const void*)(frame.pixel<void>(x, y)))
							{
								allSucceeded = false;
							}
						}

						break;
					}

					case FrameType::FORMAT_BGGR10_PACKED:
					case FrameType::FORMAT_RGGB10_PACKED:
					case FrameType::FORMAT_Y10_PACKED:
					{
						const unsigned int strideElements = width * 5u / 4u + frame.paddingElements();

						const unsigned int bytesPerElement = 1u;

						const unsigned int strideBytes = strideElements * bytesPerElement;

						ocean_assert(frame.bytesPerDataType() == bytesPerElement);

						const void* const data = frame.constdata<void>();

						for (unsigned int n = 0u; n < testsPerPlane; ++n)
						{
							//const unsigned int x = RandomI::random(width - 1u);
							const unsigned int y = RandomI::random(height - 1u);

							const void* const testRow = (const void*)((const uint8_t*)(data) + y * strideBytes);
							const void* const row = frame.constrow<void>(y);

							if (row != testRow)
							{
								allSucceeded = false;
							}

							if (frame.constrow<void>(y) != (const void*)(frame.row<void>(y)))
							{
								allSucceeded = false;
							}

#if 0 // Frame::pixel() is currently not supported for packed pixel formats

							const unsigned int pixelBlock = (x / 4u);
							const unsigned int pixelInBlock = x % 4u;

							const unsigned int element = pixelBlock * 5u + pixelInBlock;

							const void* const testPixel = (const void*)((const uint8_t*)(testRow) + element * bytesPerElement);
							const void* const pixel = frame.constpixel<void>(x, y);

							if (pixel != testPixel)
							{
								allSucceeded = false;
							}

							if (frame.constpixel<void>(x, y) != (const void*)(frame.pixel<void>(x, y)))
							{
								allSucceeded = false;
							}
#endif
						}

						break;
					}

					default:
						ocean_assert(false && "The pixel format is not covered in this test!");
						allSucceeded = false;
				}
			}
			else
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testSetFrameType(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing set frame type:";

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int sourceWidth = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int sourceHeight = RandomI::random(randomGenerator, 1u, 1080u);

		const FrameType::DataType sourceDataType = RandomI::random(randomGenerator, FrameType::definedDataTypes());
		const unsigned int sourceChannels = RandomI::random(randomGenerator, 1u, 5u);
		const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat(sourceDataType, sourceChannels);
		const FrameType::PixelOrigin sourcePixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const unsigned int sourceFramePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		const FrameType sourceFrameType(sourceWidth, sourceHeight, sourcePixelFormat, sourcePixelOrigin);
		Frame sourceFrame(sourceFrameType, sourceFramePaddingElements);
		sourceFrame.setValue(0x80u);


		const unsigned int targetWidth = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int targetHeight = RandomI::random(randomGenerator, 1u, 1080u);

		const FrameType::DataType targetDataType = RandomI::random(randomGenerator, FrameType::definedDataTypes());
		const unsigned int targetChannels = RandomI::random(randomGenerator, 1u, 5u);
		const FrameType::PixelFormat targetPixelFormat = FrameType::genericPixelFormat(targetDataType, targetChannels);
		const FrameType::PixelOrigin targetPixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});;

		const FrameType targetFrameType(targetWidth, targetHeight, targetPixelFormat, targetPixelOrigin);

		const bool forceOwner = RandomI::random(randomGenerator, 1u) == 0u;
		const bool forceWritable = RandomI::random(randomGenerator, 1u) == 0u;

		// testing setting/changing/updating an existing valid frame

		const bool setResult = sourceFrame.set(targetFrameType, forceOwner, forceWritable);

		OCEAN_EXPECT_TRUE(validation, setResult);

		if (sourceFrame.frameType() != targetFrameType)
		{
			OCEAN_SET_FAILED(validation);
		}

		if (sourceFrameType != targetFrameType)
		{
			// in case the new frame type is different, the padding frame will be continuous

			if (sourceFrame.isContinuous() == false)
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		if (forceOwner && !sourceFrame.isOwner())
		{
			OCEAN_SET_FAILED(validation);
		}

		if (sourceFrame.isReadOnly())
		{
			OCEAN_SET_FAILED(validation);
		}

		// testing setting and invalid frame 1/2

		Frame newSourceFrame;
		newSourceFrame.set(sourceFrameType, forceOwner, forceWritable);
		if (!newSourceFrame.isValid() || newSourceFrame.frameType() != sourceFrameType || newSourceFrame.isContinuous() == false)
		{
			OCEAN_SET_FAILED(validation);
		}

		if (forceOwner && !newSourceFrame.isOwner())
		{
			OCEAN_SET_FAILED(validation);
		}

		if (newSourceFrame.isReadOnly())
		{
			OCEAN_SET_FAILED(validation);
		}

		// testing setting and invalid frame 2/2

		Frame newTargetFrame;
		newTargetFrame.set(targetFrameType, forceOwner, forceWritable);
		if (!newTargetFrame.isValid() || newTargetFrame.frameType() != targetFrameType || newTargetFrame.isContinuous() == false)
		{
			OCEAN_SET_FAILED(validation);
		}

		if (forceOwner && !newTargetFrame.isOwner())
		{
			OCEAN_SET_FAILED(validation);
		}

		if (newTargetFrame.isReadOnly())
		{
			OCEAN_SET_FAILED(validation);
		}

		const Frame& constNewTargetFrame = newTargetFrame;

		Frame readOnlyFrame(constNewTargetFrame, Frame::ACM_USE_KEEP_LAYOUT);
		readOnlyFrame.set(targetFrameType, forceOwner, forceWritable);
		if (!readOnlyFrame.isValid() || readOnlyFrame.frameType() != targetFrameType || readOnlyFrame.isContinuous() == false)
		{
			OCEAN_SET_FAILED(validation);
		}

		if (forceOwner && !readOnlyFrame.isOwner())
		{
			OCEAN_SET_FAILED(validation);
		}

		if (forceWritable && readOnlyFrame.isReadOnly())
		{
			OCEAN_SET_FAILED(validation);
		}

		// finally testing whether the updated frames have consistent/valid data

		if (!sourceFrame.isReadOnly())
		{
			sourceFrame.setValue(0x80u);
		}

		if (!newSourceFrame.isReadOnly())
		{
			newSourceFrame.setValue(0x80u);
		}

		if (!newTargetFrame.isReadOnly())
		{
			newTargetFrame.setValue(0x80u);
		}

		if (!readOnlyFrame.isReadOnly())
		{
			readOnlyFrame.setValue(0x80u);
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFrame::testLegacyCopy(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing legacy copy:";

	bool allSucceeded = true;

	const FrameType::PixelFormats genericPixelFormats =
	{
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<uint8_t, 3u>(),
		FrameType::genericPixelFormat<int16_t, 2u>(),
		FrameType::genericPixelFormat<float, 4u>(),
		FrameType::genericPixelFormat<uint32_t, 3u, 2u>(),
	};

	const FrameType::PixelFormats pixelFormats = definedPixelFormats(genericPixelFormats);

	const Timestamp startTimestamp(true);

	do
	{
		for (const FrameType::PixelFormat pixelFormat : pixelFormats)
		{
			const unsigned int width = RandomI::random(1u, 1920u) * FrameType::widthMultiple(pixelFormat);
			const unsigned int height = RandomI::random(1u, 1080u) * FrameType::heightMultiple(pixelFormat);

			Indices32 paddingElementsPerPlane;

			if (RandomI::random(1u) == 0u)
			{
				for (unsigned int n = 0u; n < FrameType::numberPlanes(pixelFormat); ++n)
				{
					paddingElementsPerPlane.emplace_back(RandomI::random(1u, 100u) * RandomI::random(1u));
				}
			}

			const Timestamp sourceTimestamp = Timestamp(double(RandomI::random32()));

			const Frame owningSourceFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), paddingElementsPerPlane, sourceTimestamp);
			const Frame notOwningSourceFrame(owningSourceFrame, Frame::ACM_USE_KEEP_LAYOUT);

			const bool copyTimestamp = RandomI::random(1u) == 0u;

			{
				// testing to copy the source frame (which is owning the memory) to an invalid target frame

				Frame invalidTargetFrame;
				invalidTargetFrame.copy(owningSourceFrame, copyTimestamp);

				const Timestamp expectedTimestamp = copyTimestamp ? sourceTimestamp : Timestamp(false);

				if (!invalidTargetFrame.isValid() || invalidTargetFrame.frameType() != owningSourceFrame.frameType() || !invalidTargetFrame.isOwner() || invalidTargetFrame.timestamp() != expectedTimestamp)
				{
					allSucceeded = false;
				}
			}

			{
				// testing to copy the source frame (which is not owning the memory) to an invalid target frame

				Frame invalidTargetFrame;
				invalidTargetFrame.copy(notOwningSourceFrame, copyTimestamp);

				const Timestamp expectedTimestamp = copyTimestamp ? sourceTimestamp : Timestamp(false);

				if (!invalidTargetFrame.isValid() || invalidTargetFrame.frameType() != notOwningSourceFrame.frameType() || !invalidTargetFrame.isOwner() || invalidTargetFrame.timestamp() != expectedTimestamp)
				{
					allSucceeded = false;
				}
			}

			{
				// testing to copy the source frame (which is owning the memory) to a valid target frame (with identical frame type as the source frame)

				const Timestamp previousTimestamp = Timestamp(double(RandomI::random32()));

				Frame validTargetFrame(owningSourceFrame.frameType());
				validTargetFrame.setTimestamp(previousTimestamp);

				validTargetFrame.copy(owningSourceFrame, copyTimestamp);

				const Timestamp expectedTimestamp = copyTimestamp ? sourceTimestamp : previousTimestamp;

				if (!validTargetFrame.isValid() || validTargetFrame.frameType() != owningSourceFrame.frameType() || !validTargetFrame.isOwner() || validTargetFrame.timestamp() != expectedTimestamp)
				{
					allSucceeded = false;
				}
			}

			{
				// testing to copy the source frame (which is not owning the memory) to a valid target frame (with identical frame type as the source frame)

				const Timestamp previousTimestamp = Timestamp(double(RandomI::random32()));

				Frame validTargetFrame(notOwningSourceFrame.frameType());
				validTargetFrame.setTimestamp(previousTimestamp);

				validTargetFrame.copy(notOwningSourceFrame, copyTimestamp);

				const Timestamp expectedTimestamp = copyTimestamp ? sourceTimestamp : previousTimestamp;

				if (!validTargetFrame.isValid() || validTargetFrame.frameType() != notOwningSourceFrame.frameType() || !validTargetFrame.isOwner() || validTargetFrame.timestamp() != expectedTimestamp)
				{
					allSucceeded = false;
				}
			}

			const FrameType::PixelFormat previousTargetPixelFormat = RandomI::random(pixelFormats);

			const unsigned int previousTargetWidth = RandomI::random(1u, 1920u) * FrameType::widthMultiple(previousTargetPixelFormat);
			const unsigned int previousTargetHeight = RandomI::random(1u, 1080u) * FrameType::heightMultiple(previousTargetPixelFormat);

			const FrameType::PixelOrigin previousTargetPixelOrigin = RandomI::random({FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			{
				// testing to copy the source frame (which is owning the memory) to a valid target frame (with arbitrary frame type)

				const Timestamp previousTimestamp = Timestamp(double(RandomI::random32()));

				Frame validTargetFrame(FrameType(previousTargetWidth, previousTargetHeight, previousTargetPixelFormat, previousTargetPixelOrigin));
				validTargetFrame.setTimestamp(previousTimestamp);

				validTargetFrame.copy(owningSourceFrame, copyTimestamp);

				const Timestamp expectedTimestamp = copyTimestamp ? sourceTimestamp : previousTimestamp;

				if (!validTargetFrame.isValid() || validTargetFrame.frameType() != owningSourceFrame.frameType() || !validTargetFrame.isOwner() || validTargetFrame.timestamp() != expectedTimestamp)
				{
					allSucceeded = false;
				}
			}

			{
				// testing to copy the source frame (which is not owning the memory) to a valid target frame (with arbitrary frame type)

				const Timestamp previousTimestamp = Timestamp(double(RandomI::random32()));

				Frame validTargetFrame(FrameType(previousTargetWidth, previousTargetHeight, previousTargetPixelFormat, previousTargetPixelOrigin));
				validTargetFrame.setTimestamp(previousTimestamp);

				validTargetFrame.copy(notOwningSourceFrame, copyTimestamp);

				const Timestamp expectedTimestamp = copyTimestamp ? sourceTimestamp : previousTimestamp;

				if (!validTargetFrame.isValid() || validTargetFrame.frameType() != notOwningSourceFrame.frameType() || !validTargetFrame.isOwner() || validTargetFrame.timestamp() != expectedTimestamp)
				{
					allSucceeded = false;
				}
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testCopy(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing copy:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	// testing all non-packed pixel formats

	FrameType::PixelFormats pixelFormats(FrameType::definedPixelFormats());

	for (size_t n = 0; n < pixelFormats.size(); /*noop*/)
	{
		if (FrameType::formatIsPacked(pixelFormats[n]))
		{
			pixelFormats[n] = pixelFormats.back();
			pixelFormats.pop_back();
		}
		else
		{
			++n;
		}
	}

	const Timestamp startTimestamp(true);

	do
	{
		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);

		switch (FrameType::dataType(pixelFormat))
		{
			case FrameType::DT_SIGNED_INTEGER_8:
			case FrameType::DT_UNSIGNED_INTEGER_8:
			{
				if (!testCopy<uint8_t>(pixelFormat, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_SIGNED_INTEGER_16:
			case FrameType::DT_UNSIGNED_INTEGER_16:
			case FrameType::DT_SIGNED_FLOAT_16:
			{
				if (!testCopy<uint16_t>(pixelFormat, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_SIGNED_INTEGER_32:
			case FrameType::DT_UNSIGNED_INTEGER_32:
			case FrameType::DT_SIGNED_FLOAT_32:
			{
				if (!testCopy<uint32_t>(pixelFormat, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_SIGNED_INTEGER_64:
			case FrameType::DT_UNSIGNED_INTEGER_64:
			case FrameType::DT_SIGNED_FLOAT_64:
			{
				if (!testCopy<uint64_t>(pixelFormat, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			default:
				ocean_assert(false && "Invalid data type!");
				allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestFrame::testCopy(const FrameType::PixelFormat pixelFormat, RandomGenerator& randomGenerator)
{
	const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

	const unsigned int widthMultiple = FrameType::widthMultiple(pixelFormat);
	const unsigned int heightMultiple = FrameType::heightMultiple(pixelFormat);

	const unsigned int sourceWidth = RandomI::random(randomGenerator, 1u, 1920u) * widthMultiple;
	const unsigned int sourceHeight = RandomI::random(randomGenerator, 1u, 1080u) * heightMultiple;

	const unsigned int targetWidth = RandomI::random(randomGenerator, 1u, 1920u) * widthMultiple;
	const unsigned int targetHeight = RandomI::random(randomGenerator, 1u, 1080u) * heightMultiple;

	int targetLeft = 0;
	int targetTop = 0;

	do
	{
		targetLeft = RandomI::random(randomGenerator, -int(std::max(100u, targetWidth / 2u)), int(targetWidth + 100u));
		targetTop = RandomI::random(randomGenerator, -int(std::max(100u, targetHeight / 2u)), int(targetHeight + 100u));
	}
	while (((unsigned int)(abs(targetLeft)) % FrameType::widthMultiple(pixelFormat)) != 0u || ((unsigned int)(abs(targetTop)) % FrameType::heightMultiple(pixelFormat)) != 0u);

	const FrameType sourceFrameType(sourceWidth, sourceHeight, pixelFormat, pixelOrigin);

	Indices32 sourcePaddingElementsPerPlane;

	if (RandomI::random(randomGenerator, 1u) == 0u)
	{
		for (unsigned int planeIndex = 0u; planeIndex < sourceFrameType.numberPlanes(); ++planeIndex)
		{
			sourcePaddingElementsPerPlane.emplace_back(RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u));
		}
	}

	const FrameType targetFrameType(targetWidth, targetHeight, pixelFormat, pixelOrigin);

	Indices32 targetPaddingElementsPerPlane;

	if (RandomI::random(randomGenerator, 1u) == 0u)
	{
		for (unsigned int planeIndex = 0u; planeIndex < targetFrameType.numberPlanes(); ++planeIndex)
		{
			targetPaddingElementsPerPlane.emplace_back(RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u));
		}
	}

	Frame source(sourceFrameType, sourcePaddingElementsPerPlane);
	Frame target(targetFrameType, targetPaddingElementsPerPlane);

	// randomizing the frame's image content

	for (unsigned int planeIndex = 0u; planeIndex < source.numberPlanes(); ++planeIndex)
	{
		T* data = source.data<T>(planeIndex);

		const unsigned int elements = source.strideElements(planeIndex) * source.planeHeight(planeIndex);

		for (unsigned int n = 0u; n < elements; ++n)
		{
			data[n] = T(RandomI::random(randomGenerator, 255u));
		}
	}

	for (unsigned int planeIndex = 0u; planeIndex < target.numberPlanes(); ++planeIndex)
	{
		T* data = target.data<T>(planeIndex);

		const unsigned int elements = target.strideElements(planeIndex) * target.planeHeight(planeIndex);

		for (unsigned int n = 0u; n < elements; ++n)
		{
			data[n] = T(RandomI::random(randomGenerator, 255u));
		}
	}

	const Frame copyTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

	// now, verifying whether the image content is correct

	if (target.copy(targetLeft, targetTop, source))
	{
		ocean_assert(source.numberPlanes() == target.numberPlanes());

		for (unsigned int planeIndex = 0u; planeIndex < target.numberPlanes(); ++planeIndex)
		{
			const unsigned int numberPlaneChannels = target.planeChannels(planeIndex);

			const unsigned int targetWidthPlane = target.planeWidth(planeIndex);
			const unsigned int targetHeightPlane = target.planeHeight(planeIndex);

			for (unsigned int yTargetPlane = 0u; yTargetPlane < targetHeightPlane; ++yTargetPlane)
			{
				const unsigned int yTarget = yTargetPlane * target.height() / targetHeightPlane;
				ocean_assert(yTarget * targetHeightPlane == yTargetPlane * target.height());

				const int ySource = int(yTarget) - targetTop;

				for (unsigned int xTargetPlane = 0u; xTargetPlane < targetWidthPlane; ++xTargetPlane)
				{
					const unsigned int xTarget = xTargetPlane * target.width() / targetWidthPlane;
					ocean_assert(xTarget * targetWidthPlane == xTargetPlane * target.width());

					const int xSource = int(xTarget) - targetLeft;

					const T* const targetPixel = target.constpixel<T>(xTargetPlane, yTargetPlane, planeIndex);

					if (xSource < 0 || xSource >= int(sourceWidth) || ySource < 0 || ySource >= int(sourceHeight))
					{
						// outside intersection, nothing has been copied

						const T* const copyTargetPixel = copyTarget.constpixel<T>(xTargetPlane, yTargetPlane, planeIndex);

						for (unsigned int n = 0u; n < numberPlaneChannels; ++n)
						{
							if (targetPixel[n] != copyTargetPixel[n])
							{
								return false;
							}
						}
					}
					else
					{
						const unsigned int xSourcePlane = xSource * source.planeWidth(planeIndex) / source.width();
						const unsigned int ySourcePlane = ySource * source.planeHeight(planeIndex) / source.height();

						ocean_assert(xSourcePlane * source.width() == xSource * source.planeWidth(planeIndex));
						ocean_assert(ySourcePlane * source.height() == ySource * source.planeHeight(planeIndex));

						const T* const sourcePixel = source.constpixel<T>(xSourcePlane, ySourcePlane, planeIndex);

						for (unsigned int n = 0u; n < numberPlaneChannels; ++n)
						{
							if (targetPixel[n] != sourcePixel[n])
							{
								return false;
							}
						}
					}
				}
			}
		}
	}

	return true;
}

bool TestFrame::testMakeContinuous(const double testDuration)
{
	Log::info() << "Make continuous test:";

	bool allSucceeded = true;

	const FrameType::PixelFormats genericPixelFormats =
	{
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<uint8_t, 3u>(),
		FrameType::genericPixelFormat<int16_t, 2u>(),
		FrameType::genericPixelFormat<float, 4u>(),
		FrameType::genericPixelFormat<uint32_t, 3u, 2u>(),
	};

	const FrameType::PixelFormats pixelFormats = definedPixelFormats(genericPixelFormats);

	const Timestamp startTimestamp(true);

	do
	{
		const FrameType::PixelFormat pixelFormat = RandomI::random(pixelFormats);

		const unsigned int width = RandomI::random(1u, 1920u) * FrameType::widthMultiple(pixelFormat);
		const unsigned int height = RandomI::random(1u, 1080u) * FrameType::heightMultiple(pixelFormat);

		Indices32 paddingElementsPerPlane;

		if (RandomI::random(1u) == 0u)
		{
			for (unsigned int n = 0u; n < FrameType::numberPlanes(pixelFormat); ++n)
			{
				paddingElementsPerPlane.emplace_back(RandomI::random(1u, 100u) * RandomI::random(1u));
			}
		}

		const Frame frame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), paddingElementsPerPlane);

		Frame notOwningFrame(frame, Frame::ACM_USE_KEEP_LAYOUT);

		notOwningFrame.makeContinuous();

		if (!notOwningFrame.isContinuous())
		{
			allSucceeded = false;
		}

		if (!notOwningFrame.isOwner() && !frame.isContinuous())
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testPlaneBytesPerPixel()
{
	Log::info() << "Plane bytes per pixel test:";

	bool allSucceeded = true;

	const FrameType::PixelFormats pixelFormats = definedPixelFormats();

	for (const FrameType::PixelFormat& pixelFormat : pixelFormats)
	{
		const unsigned int width = RandomI::random(1u, 1920u) * FrameType::widthMultiple(pixelFormat);
		const unsigned int height = RandomI::random(1u, 1080u) * FrameType::heightMultiple(pixelFormat);

		const Frame frame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), Indices32());

		switch (frame.pixelFormat())
		{
			case FrameType::FORMAT_Y8_LIMITED_RANGE:
			case FrameType::FORMAT_Y8_FULL_RANGE:
			{
				if (frame.numberPlanes() != 1u)
				{
					allSucceeded = false;
				}

				if (frame.planeBytesPerPixel(0u) != 1u)
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::FORMAT_BGR4444:
			case FrameType::FORMAT_BGR5551:
			case FrameType::FORMAT_BGR565:
			case FrameType::FORMAT_BGRA4444:
			case FrameType::FORMAT_RGB4444:
			case FrameType::FORMAT_RGB5551:
			case FrameType::FORMAT_RGB565:
			case FrameType::FORMAT_RGBA4444:
			case FrameType::FORMAT_UYVY16:
			case FrameType::FORMAT_YUYV16:
			case FrameType::FORMAT_Y16:
			case FrameType::FORMAT_YA16:
			case FrameType::FORMAT_Y10:
			{
				if (frame.numberPlanes() != 1u)
				{
					allSucceeded = false;
				}

				if (frame.planeBytesPerPixel(0u) != 2u)
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::FORMAT_BGGR10_PACKED:
			case FrameType::FORMAT_RGGB10_PACKED:
			case FrameType::FORMAT_Y10_PACKED:
			{
				if (frame.numberPlanes() != 1u)
				{
					allSucceeded = false;
				}

				if (frame.planeBytesPerPixel(0u) != 0u)
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::FORMAT_BGR24:
			case FrameType::FORMAT_RGB24:
			case FrameType::FORMAT_YUV24:
			case FrameType::FORMAT_YVU24:
			{
				if (frame.numberPlanes() != 1u)
				{
					allSucceeded = false;
				}

				if (frame.planeBytesPerPixel(0u) != 3u)
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::FORMAT_ABGR32:
			case FrameType::FORMAT_ARGB32:
			case FrameType::FORMAT_BGR32:
			case FrameType::FORMAT_BGRA32:
			case FrameType::FORMAT_RGB32:
			case FrameType::FORMAT_RGBA32:
			case FrameType::FORMAT_RGBT32:
			case FrameType::FORMAT_YUVA32:
			case FrameType::FORMAT_YUVT32:
			case FrameType::FORMAT_Y32:
			case FrameType::FORMAT_F32:
			{
				if (frame.numberPlanes() != 1u)
				{
					allSucceeded = false;
				}

				if (frame.planeBytesPerPixel(0u) != 4u)
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::FORMAT_RGB48:
			{
				if (frame.numberPlanes() != 1u)
				{
					allSucceeded = false;
				}

				if (frame.planeBytesPerPixel(0u) != 6u)
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::FORMAT_Y64:
			case FrameType::FORMAT_RGBA64:
			case FrameType::FORMAT_F64:
			{
				if (frame.numberPlanes() != 1u)
				{
					allSucceeded = false;
				}

				if (frame.planeBytesPerPixel(0u) != 8u)
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::FORMAT_Y_U_V24_LIMITED_RANGE:
			case FrameType::FORMAT_Y_U_V24_FULL_RANGE:
			case FrameType::FORMAT_Y_U_V12_LIMITED_RANGE:
			case FrameType::FORMAT_Y_U_V12_FULL_RANGE:
			case FrameType::FORMAT_Y_V_U12_LIMITED_RANGE:
			case FrameType::FORMAT_Y_V_U12_FULL_RANGE:
			{
				if (frame.numberPlanes() != 3u)
				{
					allSucceeded = false;
				}

				for (unsigned int planeIndex = 0u; planeIndex < 3u; ++planeIndex)
				{
					if (frame.planeBytesPerPixel(planeIndex) != 1u)
					{
						allSucceeded = false;
					}
				}

				break;
			}

			case FrameType::FORMAT_Y_UV12_LIMITED_RANGE:
			case FrameType::FORMAT_Y_UV12_FULL_RANGE:
			case FrameType::FORMAT_Y_VU12_LIMITED_RANGE:
			case FrameType::FORMAT_Y_VU12_FULL_RANGE:
			{
				if (frame.numberPlanes() != 2u)
				{
					allSucceeded = false;
				}

				if (frame.planeBytesPerPixel(0u) != 1u)
				{
					allSucceeded = false;
				}

				if (frame.planeBytesPerPixel(1u) != 2u)
				{
					allSucceeded = false;
				}

				break;
			}

			default:
				ocean_assert(false && "Invalid pixel format!");
				allSucceeded = false;
				break;
		}
	}

	// now testing pure generic pixel formats

	for (const FrameType::DataType& dataType : FrameType::definedDataTypes())
	{
		const unsigned int width = RandomI::random(1u, 1920u);
		const unsigned int height = RandomI::random(1u, 1080u);
		const unsigned int channels = RandomI::random(1u, 5u);

		const unsigned int planes = RandomI::random(1u, 4u);

		const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(dataType, channels, planes);

		const Frame frame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), Indices32());

		const unsigned int bytesPerDataType = FrameType::bytesPerDataType(dataType);
		const unsigned int bytesPerPixel = bytesPerDataType * channels;

		if (frame.numberPlanes() != planes)
		{
			allSucceeded = false;
		}

		for (unsigned int planeIndex = 0u; planeIndex < planes; ++planeIndex)
		{
			if (frame.planeBytesPerPixel(planeIndex) != bytesPerPixel)
			{
				allSucceeded = false;
			}
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testSetValue(const double testDuration)
{
	Log::info() << "Set value test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const FrameType::PixelFormats genericPixelFormats =
	{
		FrameType::genericPixelFormat<uint8_t, 3u>(),
		FrameType::genericPixelFormat<int16_t, 2u>(),
		FrameType::genericPixelFormat<float, 4u>(),
		FrameType::genericPixelFormat<double, 1u>(),
	};

	const FrameType::PixelFormats pixelFormats = definedPixelFormats(genericPixelFormats);

	const Timestamp startTimestamp(true);

	do
	{
		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);
		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u) * FrameType::widthMultiple(pixelFormat);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u) * FrameType::heightMultiple(pixelFormat);

		Indices32 paddingElementsPerPlane;

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			for (unsigned int planeIndex = 0u; planeIndex < FrameType::numberPlanes(pixelFormat); ++planeIndex)
			{
				paddingElementsPerPlane.push_back(RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u));
			}
		}

		Frame frame(FrameType(width, height, pixelFormat, pixelOrigin), paddingElementsPerPlane);

		for (unsigned int planeIndex = 0u; planeIndex < frame.numberPlanes(); ++planeIndex)
		{
			uint8_t* plane = frame.data<uint8_t>(planeIndex);

			for (unsigned int n = 0u; n < frame.planeHeight(planeIndex) * frame.strideBytes(planeIndex); ++n)
			{
				plane[n] = uint8_t(RandomI::random(randomGenerator, 255u));
			}
		}

		switch (frame.dataType())
		{
			case FrameType::DT_UNSIGNED_INTEGER_8:
			{
				if (!testSetValue<uint8_t>(frame, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_SIGNED_INTEGER_8:
			{
				if (!testSetValue<int8_t>(frame, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_UNSIGNED_INTEGER_16:
			{
				if (!testSetValue<uint16_t>(frame, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_SIGNED_INTEGER_16:
			{
				if (!testSetValue<int16_t>(frame, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_UNSIGNED_INTEGER_32:
			{
				if (!testSetValue<uint32_t>(frame, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_SIGNED_INTEGER_32:
			{
				if (!testSetValue<int32_t>(frame, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_UNSIGNED_INTEGER_64:
			{
				if (!testSetValue<uint64_t>(frame, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_SIGNED_INTEGER_64:
			{
				if (!testSetValue<int64_t>(frame, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_SIGNED_FLOAT_32:
			{
				if (!testSetValue<float>(frame, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_SIGNED_FLOAT_64:
			{
				if (!testSetValue<double>(frame, randomGenerator))
				{
					allSucceeded = false;
				}

				break;
			}

			default:
				ocean_assert(false && "Invalid data type!");
				allSucceeded = false;
				break;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testContainsValue(const double testDuration)
{
	Log::info() << "Contains value test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

		const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		{
			// Y8
			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			Frame frame(FrameType(width, height, FrameType::FORMAT_Y8, pixelOrigin), paddingElements);

			const uint8_t backgroundColor = uint8_t(RandomI::random(randomGenerator, 255u));

			frame.setValue(backgroundColor);

			const Frame::PixelType<uint8_t, 1u> testColor =
			{{
				uint8_t(RandomI::random(randomGenerator, 255u))
			}};

			bool containsValue = backgroundColor == testColor.values_[0];

			if (!containsValue && RandomI::random(randomGenerator, 1u) == 0u)
			{
				const unsigned int subFrameLeft = RandomI::random(randomGenerator, frame.width() - 1u);
				const unsigned int subFrameTop = RandomI::random(randomGenerator, frame.height() - 1u);
				const unsigned int subFrameWidth = RandomI::random(randomGenerator, 1u, frame.width() - subFrameLeft);
				const unsigned int subFrameHeight = RandomI::random(randomGenerator, 1u, frame.height() - subFrameTop);

				frame.subFrame(subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight).setValue<uint8_t, 1u>(testColor);

				containsValue = true;
			}

			if (frame.containsValue<uint8_t, 1u>(testColor) != containsValue)
			{
				allSucceeded = false;
			}
		}

		{
			// RGB24
			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			Frame frame(FrameType(width, height, FrameType::FORMAT_RGB24, pixelOrigin), paddingElements);

			const Frame::PixelType<uint8_t, 3u> backgroundColor =
			{{
				uint8_t(RandomI::random(randomGenerator, 255u)),
				uint8_t(RandomI::random(randomGenerator, 255u)),
				uint8_t(RandomI::random(randomGenerator, 255u))
			}};

			frame.setValue<uint8_t, 3u>(backgroundColor);

			const Frame::PixelType<uint8_t, 3u> testColor =
			{{
				uint8_t(RandomI::random(randomGenerator, 255u)),
				uint8_t(RandomI::random(randomGenerator, 255u)),
				uint8_t(RandomI::random(randomGenerator, 255u))
			}};

			bool containsValue = backgroundColor == testColor;

			if (!containsValue && RandomI::random(randomGenerator, 1u) == 0u)
			{
				const unsigned int subFrameLeft = RandomI::random(randomGenerator, frame.width() - 1u);
				const unsigned int subFrameTop = RandomI::random(randomGenerator, frame.height() - 1u);
				const unsigned int subFrameWidth = RandomI::random(randomGenerator, 1u, frame.width() - subFrameLeft);
				const unsigned int subFrameHeight = RandomI::random(randomGenerator, 1u, frame.height() - subFrameTop);

				frame.subFrame(subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight).setValue<uint8_t, 3u>(testColor);

				containsValue = true;
			}

			if (frame.containsValue<uint8_t, 3u>(testColor) != containsValue)
			{
				allSucceeded = false;
			}
		}

		{
			// float, 2 channels
			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			Frame frame(FrameType(width, height, FrameType::genericPixelFormat<float, 2u>(), pixelOrigin), paddingElements);

			const Frame::PixelType<float, 2u> backgroundColor =
			{{
				float(RandomI::random(randomGenerator, 255u)),
				float(RandomI::random(randomGenerator, 255u))
			}};

			frame.setValue<float, 2u>(backgroundColor);

			const Frame::PixelType<float, 2u> testColor =
			{{
				float(RandomI::random(randomGenerator, 255u)),
				float(RandomI::random(randomGenerator, 255u))
			}};

			bool containsValue = backgroundColor == testColor;

			if (!containsValue && RandomI::random(randomGenerator, 1u) == 0u)
			{
				const unsigned int subFrameLeft = RandomI::random(randomGenerator, frame.width() - 1u);
				const unsigned int subFrameTop = RandomI::random(randomGenerator, frame.height() - 1u);
				const unsigned int subFrameWidth = RandomI::random(randomGenerator, 1u, frame.width() - subFrameLeft);
				const unsigned int subFrameHeight = RandomI::random(randomGenerator, 1u, frame.height() - subFrameTop);

				frame.subFrame(subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight).setValue<float, 2u>(testColor);

				containsValue = true;
			}

			if (frame.containsValue<float, 2u>(testColor) != containsValue)
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testHasTransparentPixel(const double testDuration)
{
	Log::info() << "Has transparent pixel test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const FrameType::PixelFormats genericPixelFormats =
	{
		FrameType::genericPixelFormat<uint8_t, 3u>(),
		FrameType::genericPixelFormat<int16_t, 2u>(),
		FrameType::genericPixelFormat<float, 4u>(),
		FrameType::genericPixelFormat<double, 1u>(),
	};

	const FrameType::PixelFormats pixelFormats = definedPixelFormats(genericPixelFormats);

	const Timestamp startTimestamp(true);

	for (const FrameType::PixelFormat& pixelFormat : pixelFormats)
	{
		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u) * FrameType::widthMultiple(pixelFormat);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u) * FrameType::heightMultiple(pixelFormat);

		Frame frame(FrameType(width, height, pixelFormat, pixelOrigin));

		// the frame will not contain a transparent pixel

		for (unsigned int planeIndex = 0u; planeIndex < frame.numberPlanes(); ++planeIndex)
		{
			frame.setValue(0xFFu, planeIndex);
		}

		if (frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
		{
			if (frame.hasTransparentPixel<uint8_t>(0xFFu))
			{
				allSucceeded = false;
			}
		}
		else if (frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_16)
		{
			if (frame.pixelFormat() == FrameType::FORMAT_BGRA4444 || frame.pixelFormat() == FrameType::FORMAT_RGBA4444)
			{
				if (frame.hasTransparentPixel<uint16_t>(0x000Fu))
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (frame.hasTransparentPixel<uint16_t>(0xFFFFu))
				{
					allSucceeded = false;
				}
			}
		}
		else if (frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_32)
		{
			if (frame.hasTransparentPixel<uint32_t>(0xFFFFFFFFu))
			{
				allSucceeded = false;
			}
		}
		else if (frame.dataType() == FrameType::DT_SIGNED_FLOAT_32)
		{
			if (frame.hasTransparentPixel<float>(0.0f))
			{
				allSucceeded = false;
			}
		}
		else if (frame.dataType() == FrameType::DT_SIGNED_FLOAT_64)
		{
			if (frame.hasTransparentPixel<double>(0.0f))
			{
				allSucceeded = false;
			}
		}
	}

	do
	{
		{
			// RGBA32, BGRA32, YUVA32

			const FrameType::PixelFormats subsetPixelFormats = {FrameType::FORMAT_RGBA32, FrameType::FORMAT_BGRA32, FrameType::FORMAT_YUVA32};

			const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, subsetPixelFormats);
			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

			const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame frame(FrameType(width, height, pixelFormat, pixelOrigin), paddingElements);

			const uint8_t opaqueValue = uint8_t(RandomI::random(randomGenerator, 255u));

			const uint8_t opaquePixelValue[4] =
			{
				uint8_t(RandomI::random(randomGenerator, 255u)),
				uint8_t(RandomI::random(randomGenerator, 255u)),
				uint8_t(RandomI::random(randomGenerator, 255u)),
				opaqueValue
			};

			frame.setValue<uint8_t>(opaquePixelValue, 4u);

			const bool willContainTransparentPixel = RandomI::random(randomGenerator, 1u) == 0u;

			if (willContainTransparentPixel)
			{
				const uint8_t transparentValue = uint8_t(((unsigned int)(opaqueValue) + RandomI::random(randomGenerator, 1u, 254u)) % 256u);
				ocean_assert(transparentValue != opaqueValue);

				const uint8_t transparentPixelValue[4] =
				{
					uint8_t(RandomI::random(randomGenerator, 255u)),
					uint8_t(RandomI::random(randomGenerator, 255u)),
					uint8_t(RandomI::random(randomGenerator, 255u)),
					transparentValue
				};

				const unsigned int transparentBlockWidth = RandomI::random(randomGenerator, 1u, width);
				const unsigned int transparentBlockHeight = RandomI::random(randomGenerator, 1u, height);

				const unsigned int transparentBlockLeft = RandomI::random(randomGenerator, 0u, width - transparentBlockWidth);
				const unsigned int transparentBlockTop = RandomI::random(randomGenerator, 0u, height - transparentBlockHeight);

				frame.subFrame(transparentBlockLeft, transparentBlockTop, transparentBlockWidth, transparentBlockHeight).setValue<uint8_t>(transparentPixelValue, 4u);
			}

			if (frame.hasTransparentPixel<uint8_t>(opaqueValue) != willContainTransparentPixel)
			{
				allSucceeded = false;
			}
		}

		{
			// ARGB32, ABGR32

			const FrameType::PixelFormats subsetPixelFormats = {FrameType::FORMAT_ARGB32, FrameType::FORMAT_ABGR32};

			const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, subsetPixelFormats);
			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

			const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame frame(FrameType(width, height, pixelFormat, pixelOrigin), paddingElements);

			const uint8_t opaqueValue = uint8_t(RandomI::random(randomGenerator, 0xFFu));

			const uint8_t opaquePixelValue[4] =
			{
				opaqueValue,
				uint8_t(RandomI::random(randomGenerator, 255u)),
				uint8_t(RandomI::random(randomGenerator, 255u)),
				uint8_t(RandomI::random(randomGenerator, 255u))
			};

			frame.setValue<uint8_t>(opaquePixelValue, 4u);

			const bool willContainTransparentPixel = RandomI::random(randomGenerator, 1u) == 0u;

			if (willContainTransparentPixel)
			{
				const uint8_t transparentValue = uint8_t(((unsigned int)(opaqueValue) + RandomI::random(randomGenerator, 1u, 254u)) % 256u);
				ocean_assert(transparentValue != opaqueValue);

				const uint8_t transparentPixelValue[4] =
				{
					transparentValue,
					uint8_t(RandomI::random(randomGenerator, 255u)),
					uint8_t(RandomI::random(randomGenerator, 255u)),
					uint8_t(RandomI::random(randomGenerator, 255u))
				};

				const unsigned int transparentBlockWidth = RandomI::random(randomGenerator, 1u, width);
				const unsigned int transparentBlockHeight = RandomI::random(randomGenerator, 1u, height);

				const unsigned int transparentBlockLeft = RandomI::random(randomGenerator, 0u, width - transparentBlockWidth);
				const unsigned int transparentBlockTop = RandomI::random(randomGenerator, 0u, height - transparentBlockHeight);

				frame.subFrame(transparentBlockLeft, transparentBlockTop, transparentBlockWidth, transparentBlockHeight).setValue<uint8_t>(transparentPixelValue, 4u);
			}

			if (frame.hasTransparentPixel<uint8_t>(opaqueValue) != willContainTransparentPixel)
			{
				allSucceeded = false;
			}
		}

		{
			// RGBA64

			const FrameType::PixelFormats subsetPixelFormats = {FrameType::FORMAT_RGBA64};

			const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, subsetPixelFormats);
			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

			const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame frame(FrameType(width, height, pixelFormat, pixelOrigin), paddingElements);

			const uint16_t opaqueValue = uint8_t(RandomI::random(randomGenerator, 0xFFFFu));

			const uint16_t opaquePixelValue[4] =
			{
				uint16_t(RandomI::random(randomGenerator, 0xFFFFu)),
				uint16_t(RandomI::random(randomGenerator, 0xFFFFu)),
				uint16_t(RandomI::random(randomGenerator, 0xFFFFu)),
				opaqueValue
			};

			frame.setValue<uint16_t>(opaquePixelValue, 4u);

			const bool willContainTransparentPixel = RandomI::random(randomGenerator, 1u) == 0u;

			if (willContainTransparentPixel)
			{
				const uint16_t transparentValue = uint16_t(((unsigned int)(opaqueValue) + RandomI::random(randomGenerator, 65533u) + 1u) % 65536u);
				ocean_assert(transparentValue != opaqueValue);

				const uint16_t transparentPixelValue[4] =
				{
					uint16_t(RandomI::random(randomGenerator, 0xFFFFu)),
					uint16_t(RandomI::random(randomGenerator, 0xFFFFu)),
					uint16_t(RandomI::random(randomGenerator, 0xFFFFu)),
					transparentValue
				};

				const unsigned int transparentBlockWidth = RandomI::random(randomGenerator, 1u, width);
				const unsigned int transparentBlockHeight = RandomI::random(randomGenerator, 1u, height);

				const unsigned int transparentBlockLeft = RandomI::random(randomGenerator, 0u, width - transparentBlockWidth);
				const unsigned int transparentBlockTop = RandomI::random(randomGenerator, 0u, height - transparentBlockHeight);

				frame.subFrame(transparentBlockLeft, transparentBlockTop, transparentBlockWidth, transparentBlockHeight).setValue<uint16_t>(transparentPixelValue, 4u);
			}

			if (frame.hasTransparentPixel<uint16_t>(opaqueValue) != willContainTransparentPixel)
			{
				allSucceeded = false;
			}
		}

		{
			// YA16

			const FrameType::PixelFormats subsetPixelFormats = {FrameType::FORMAT_YA16};

			const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, subsetPixelFormats);
			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

			const unsigned int paddingElements = 0u;//RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame frame(FrameType(width, height, pixelFormat, pixelOrigin), paddingElements);

			const uint8_t opaqueValue = uint8_t(RandomI::random(randomGenerator, 255u));

			const uint8_t opaquePixelValue[2] =
			{
				uint8_t(RandomI::random(randomGenerator, 255u)),
				opaqueValue
			};

			frame.setValue<uint8_t>(opaquePixelValue, 2u);

			const bool willContainTransparentPixel = RandomI::random(randomGenerator, 1u) == 0u;

			if (willContainTransparentPixel)
			{
				const uint8_t transparentValue = uint8_t(((unsigned int)(opaqueValue) + RandomI::random(randomGenerator, 1u, 254u)) % 256u);
				ocean_assert(transparentValue != opaqueValue);

				const uint8_t transparentPixelValue[2] =
				{
					uint8_t(RandomI::random(randomGenerator, 255u)),
					transparentValue
				};

				const unsigned int transparentBlockWidth = RandomI::random(randomGenerator, 1u, width);
				const unsigned int transparentBlockHeight = RandomI::random(randomGenerator, 1u, height);

				const unsigned int transparentBlockLeft = RandomI::random(randomGenerator, 0u, width - transparentBlockWidth);
				const unsigned int transparentBlockTop = RandomI::random(randomGenerator, 0u, height - transparentBlockHeight);

				frame.subFrame(transparentBlockLeft, transparentBlockTop, transparentBlockWidth, transparentBlockHeight).setValue<uint8_t>(transparentPixelValue, 2u);
			}

			if (frame.hasTransparentPixel<uint8_t>(opaqueValue) != willContainTransparentPixel)
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testStrideBytes2paddingElements(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing calculate padding elements";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const FrameType::PixelFormats genericPixelFormats =
	{
		FrameType::genericPixelFormat<uint8_t, 3u>(),
		FrameType::genericPixelFormat<int16_t, 2u>(),
		FrameType::genericPixelFormat<float, 4u>(),
		FrameType::genericPixelFormat<double, 1u>(),
	};

	const FrameType::PixelFormats pixelFormats = definedPixelFormats(genericPixelFormats);

	const Timestamp startTimestamp(true);

	for (const FrameType::PixelFormat& pixelFormat : pixelFormats)
	{
		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u) * FrameType::widthMultiple(pixelFormat);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u) * FrameType::heightMultiple(pixelFormat);

		Indices32 paddingElementsPerPlane(FrameType::numberPlanes(pixelFormat));

		for (unsigned int& paddingElements : paddingElementsPerPlane)
		{
			paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		}

		const Frame frame(FrameType(width, height, pixelFormat, pixelOrigin), paddingElementsPerPlane);

		ocean_assert(frame.numberPlanes() >= 1u);
		if (frame.numberPlanes() == 0u)
		{
			allSucceeded = false;
		}

		for (unsigned int planeIndex = 0u; planeIndex < frame.numberPlanes(); ++planeIndex)
		{
			unsigned int planePaddingElements = (unsigned int)(-1);

			if (!Frame::strideBytes2paddingElements(frame.pixelFormat(), frame.width(), frame.strideBytes(planeIndex), planePaddingElements, planeIndex) || planePaddingElements != paddingElementsPerPlane[planeIndex])
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testHaveIntersectingMemory(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Intersecting memory test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		{
			// test for frames each owning the memory

			const unsigned int widthA = RandomI::random(randomGenerator, 1u, 1920u);
			const unsigned int widthB = RandomI::random(randomGenerator, 1u, 1920u);

			const unsigned int heightA = RandomI::random(randomGenerator, 1u, 1080u);
			const unsigned int heightB = RandomI::random(randomGenerator, 1u, 1080u);

			const FrameType::DataType dataTypeA = FrameType::DataType(RandomI::random(randomGenerator, (unsigned int)FrameType::DT_UNSIGNED_INTEGER_8, (unsigned int)FrameType::DT_SIGNED_FLOAT_64));
			const FrameType::DataType dataTypeB = FrameType::DataType(RandomI::random(randomGenerator, (unsigned int)FrameType::DT_UNSIGNED_INTEGER_8, (unsigned int)FrameType::DT_SIGNED_FLOAT_64));

			const FrameType::PixelFormat pixelFormatA = FrameType::genericPixelFormat(dataTypeA, RandomI::random(randomGenerator, 1u, 5u));
			const FrameType::PixelFormat pixelFormatB = FrameType::genericPixelFormat(dataTypeB, RandomI::random(randomGenerator, 1u, 5u));

			const FrameType::PixelOrigin pixelOriginA = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});
			const FrameType::PixelOrigin pixelOriginB = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const bool usePaddingA = RandomI::random(randomGenerator, 1u) < 1u;
			const bool usePaddingB = RandomI::random(randomGenerator, 1u) < 1u;

			const unsigned int paddingElementsA = usePaddingA ? RandomI::random(randomGenerator, 1u, 128u) : 0u;
			const unsigned int paddingElementsB = usePaddingB ? RandomI::random(randomGenerator, 1u, 128u) : 0u;

			const Frame frameA(FrameType(widthA, heightA, pixelFormatA, pixelOriginA), paddingElementsA);
			const Frame frameB(FrameType(widthB, heightB, pixelFormatB, pixelOriginB), paddingElementsB);

			// two individual frames never have intersecting memory

			if (frameA.haveIntersectingMemory(frameB))
			{
				allSucceeded = false;
			}
			if (frameB.haveIntersectingMemory(frameA))
			{
				allSucceeded = false;
			}

			// two identical frames always have intersecting memory

			if (frameA.haveIntersectingMemory(frameA) == false)
			{
				allSucceeded = false;
			}
			if (frameB.haveIntersectingMemory(frameB) == false)
			{
				allSucceeded = false;
			}
		}

		{
			// test for frames not owning the memory

			constexpr unsigned int maximalWidth = 1920u;
			constexpr unsigned int maximalHeight = 1080u;
			constexpr unsigned int maximalChannels = 5u;
			constexpr unsigned int maximalPaddingElements = 128u;

			const unsigned int maximalFrameMemory = sizeof(uint8_t) * (maximalWidth * maximalHeight * maximalChannels + maximalHeight * maximalPaddingElements);

			Memory memory(size_t(maximalFrameMemory * 2u)); // we allocate twice as much memory

			const unsigned int widthA = RandomI::random(randomGenerator, 1u, maximalWidth);
			const unsigned int widthB = RandomI::random(randomGenerator, 1u, maximalWidth);

			const unsigned int heightA = RandomI::random(randomGenerator, 1u, maximalHeight);
			const unsigned int heightB = RandomI::random(randomGenerator, 1u, maximalHeight);

			const FrameType::PixelFormat pixelFormatA = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, RandomI::random(randomGenerator, 1u, maximalChannels));
			const FrameType::PixelFormat pixelFormatB = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, RandomI::random(randomGenerator, 1u, maximalChannels));

			const FrameType::PixelOrigin pixelOriginA = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});
			const FrameType::PixelOrigin pixelOriginB = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const FrameType frameTypeA(widthA, heightA, pixelFormatA, pixelOriginA);
			const FrameType frameTypeB(widthB, heightB, pixelFormatB, pixelOriginB);

			const bool usePaddingA = RandomI::random(randomGenerator, 1u) < 1u;
			const bool usePaddingB = RandomI::random(randomGenerator, 1u) < 1u;

			const unsigned int paddingElementsA = usePaddingA ? RandomI::random(randomGenerator, 1u, maximalPaddingElements) : 0u;
			const unsigned int paddingElementsB = usePaddingB ? RandomI::random(randomGenerator, 1u, maximalPaddingElements) : 0u;

			const size_t startFrameA = size_t(RandomI::random(randomGenerator, maximalFrameMemory)); // we have twice as much memory
			const size_t startFrameB = size_t(RandomI::random(randomGenerator, maximalFrameMemory)); // we have twice as much memory

			const Frame frameA(frameTypeA, memory.data<uint8_t>() + startFrameA, Frame::CM_USE_KEEP_LAYOUT, paddingElementsA);
			const Frame frameB(frameTypeB, memory.data<uint8_t>() + startFrameB, Frame::CM_USE_KEEP_LAYOUT, paddingElementsB);

			const Frame constFrameA(frameTypeA, memory.data<const uint8_t>() + startFrameA, Frame::CM_USE_KEEP_LAYOUT, paddingElementsA);
			const Frame constFrameB(frameTypeB, memory.data<const uint8_t>() + startFrameB, Frame::CM_USE_KEEP_LAYOUT, paddingElementsB);

			const size_t sizeFrameA = size_t(frameA.size());
			const size_t sizeFrameB = size_t(frameB.size());

			// memory: |                                                                                         |
			// frameA:        [offset          ]offset + size
			// frameB:                                        [offset          ]offset + size

			const size_t endFrameA = startFrameA + sizeFrameA;
			const size_t endFrameB = startFrameB + sizeFrameB;

			const size_t startIntersection = std::max(startFrameA, startFrameB);
			const size_t endIntersection = std::min(endFrameA, endFrameB);

			const bool memoryIsIntersecting = startIntersection < endIntersection;

			if (memoryIsIntersecting != frameA.haveIntersectingMemory(frameB))
			{
				allSucceeded = false;
			}
			if (memoryIsIntersecting != frameB.haveIntersectingMemory(frameA))
			{
				allSucceeded = false;
			}

			if (memoryIsIntersecting != frameA.haveIntersectingMemory(constFrameB))
			{
				allSucceeded = false;
			}
			if (memoryIsIntersecting != constFrameB.haveIntersectingMemory(frameA))
			{
				allSucceeded = false;
			}

			if (memoryIsIntersecting != constFrameA.haveIntersectingMemory(frameB))
			{
				allSucceeded = false;
			}
			if (memoryIsIntersecting != frameB.haveIntersectingMemory(constFrameA))
			{
				allSucceeded = false;
			}

			if (memoryIsIntersecting != constFrameA.haveIntersectingMemory(constFrameB))
			{
				allSucceeded = false;
			}
			if (memoryIsIntersecting != constFrameB.haveIntersectingMemory(constFrameA))
			{
				allSucceeded = false;
			}

			// identical frames always share the same memory

			if (frameA.haveIntersectingMemory(frameA) == false)
			{
				allSucceeded = false;
			}
			if (frameB.haveIntersectingMemory(frameB) == false)
			{
				allSucceeded = false;
			}

			if (constFrameA.haveIntersectingMemory(constFrameA) == false)
			{
				allSucceeded = false;
			}
			if (constFrameB.haveIntersectingMemory(constFrameB) == false)
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
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testUpdateMemory(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Update memory:";

	const FrameType::PixelFormats genericPixelFormats =
	{
		FrameType::genericPixelFormat<uint8_t, 1u>(),
		FrameType::genericPixelFormat<uint8_t, 3u>(),
		FrameType::genericPixelFormat<int16_t, 2u>(),
		FrameType::genericPixelFormat<float, 4u>(),
		FrameType::genericPixelFormat<uint32_t, 3u, 2u>(),
	};

	const FrameType::PixelFormats pixelFormats = definedPixelFormats(genericPixelFormats);

	RandomGenerator randomGenerator;

	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// writable frame

			Frame sourceFrame(randomizedFrameType(pixelFormats, &randomGenerator));

			const unsigned int numberPlanes = sourceFrame.numberPlanes();

			Frame notOwnerFrame(sourceFrame, Frame::ACM_USE_KEEP_LAYOUT);

			for (unsigned int planeIndex = 0u; planeIndex < numberPlanes; ++planeIndex)
			{
				if (notOwnerFrame.data<void>(planeIndex) != sourceFrame.data<void>(planeIndex))
				{
					OCEAN_SET_FAILED(validation);
				}

				if (notOwnerFrame.constdata<void>(planeIndex) != sourceFrame.constdata<void>(planeIndex))
				{
					OCEAN_SET_FAILED(validation);
				}

				if (notOwnerFrame.isPlaneOwner(planeIndex))
				{
					OCEAN_SET_FAILED(validation);
				}
			}

			const unsigned int updatedPlaneIndex = RandomI::random(randomGenerator, numberPlanes - 1u);

			Memory newExternalMemory(1024);

			if (sourceFrame.bytesPerDataType() == 2u)
			{
				// we add a check for an explicit data type (just one out of all candidates)

				if (notOwnerFrame.updateMemory(newExternalMemory.data<uint16_t>(), updatedPlaneIndex))
				{
					for (unsigned int planeIndex = 0u; planeIndex < numberPlanes; ++planeIndex)
					{
						if (planeIndex == updatedPlaneIndex)
						{
							if (notOwnerFrame.data<uint16_t>(planeIndex) != newExternalMemory.data())
							{
								OCEAN_SET_FAILED(validation);
							}

							if (notOwnerFrame.constdata<uint16_t>(planeIndex) != newExternalMemory.data())
							{
								OCEAN_SET_FAILED(validation);
							}
						}
						else
						{
							if (notOwnerFrame.data<uint16_t>(planeIndex) != sourceFrame.data<uint16_t>(planeIndex))
							{
								OCEAN_SET_FAILED(validation);
							}

							if (notOwnerFrame.constdata<uint16_t>(planeIndex) != sourceFrame.constdata<uint16_t>(planeIndex))
							{
								OCEAN_SET_FAILED(validation);
							}
						}

						if (notOwnerFrame.isPlaneOwner(planeIndex))
						{
							OCEAN_SET_FAILED(validation);
						}
					}
				}
				else
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				if (notOwnerFrame.updateMemory(newExternalMemory.data(), updatedPlaneIndex))
				{
					for (unsigned int planeIndex = 0u; planeIndex < numberPlanes; ++planeIndex)
					{
						if (planeIndex == updatedPlaneIndex)
						{
							if (notOwnerFrame.data<void>(planeIndex) != newExternalMemory.data())
							{
								OCEAN_SET_FAILED(validation);
							}

							if (notOwnerFrame.constdata<void>(planeIndex) != newExternalMemory.data())
							{
								OCEAN_SET_FAILED(validation);
							}
						}
						else
						{
							if (notOwnerFrame.data<void>(planeIndex) != sourceFrame.data<void>(planeIndex))
							{
								OCEAN_SET_FAILED(validation);
							}

							if (notOwnerFrame.constdata<void>(planeIndex) != sourceFrame.constdata<void>(planeIndex))
							{
								OCEAN_SET_FAILED(validation);
							}
						}

						if (notOwnerFrame.isPlaneOwner(planeIndex))
						{
							OCEAN_SET_FAILED(validation);
						}
					}
				}
				else
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}

		{
			// read-only frame

			const Frame sourceFrame(randomizedFrameType(pixelFormats, &randomGenerator));

			const unsigned int numberPlanes = sourceFrame.numberPlanes();

			Frame notOwnerFrame(sourceFrame, Frame::ACM_USE_KEEP_LAYOUT);

			for (unsigned int planeIndex = 0u; planeIndex < numberPlanes; ++planeIndex)
			{
				if (notOwnerFrame.constdata<void>(planeIndex) != sourceFrame.constdata<void>(planeIndex))
				{
					OCEAN_SET_FAILED(validation);
				}

				if (notOwnerFrame.isPlaneOwner(planeIndex))
				{
					OCEAN_SET_FAILED(validation);
				}
			}

			const unsigned int updatedPlaneIndex = RandomI::random(randomGenerator, numberPlanes - 1u);

			Memory newExternalMemory(1024);

			if (sourceFrame.bytesPerDataType() == 2u)
			{
				// we add a check for an explicit data type (just one out of all candidates)

				if (notOwnerFrame.updateMemory(newExternalMemory.constdata<uint16_t>(), updatedPlaneIndex))
				{
					for (unsigned int planeIndex = 0u; planeIndex < numberPlanes; ++planeIndex)
					{
						if (planeIndex == updatedPlaneIndex)
						{
							if (notOwnerFrame.data<uint16_t>(planeIndex) != nullptr)
							{
								OCEAN_SET_FAILED(validation);
							}

							if (notOwnerFrame.constdata<uint16_t>(planeIndex) != newExternalMemory.data<const uint16_t>())
							{
								OCEAN_SET_FAILED(validation);
							}
						}
						else
						{
							if (notOwnerFrame.constdata<uint16_t>(planeIndex) != sourceFrame.constdata<uint16_t>(planeIndex))
							{
								OCEAN_SET_FAILED(validation);
							}
						}

						if (notOwnerFrame.isPlaneOwner(planeIndex))
						{
							OCEAN_SET_FAILED(validation);
						}
					}
				}
				else
				{
					OCEAN_SET_FAILED(validation);
				}
			}
			else
			{
				if (notOwnerFrame.updateMemory(newExternalMemory.constdata(), updatedPlaneIndex))
				{
					for (unsigned int planeIndex = 0u; planeIndex < numberPlanes; ++planeIndex)
					{
						if (planeIndex == updatedPlaneIndex)
						{
							if (notOwnerFrame.data<void>(planeIndex) != nullptr)
							{
								OCEAN_SET_FAILED(validation);
							}

							if (notOwnerFrame.constdata<void>(planeIndex) != newExternalMemory.data())
							{
								OCEAN_SET_FAILED(validation);
							}
						}
						else
						{
							if (notOwnerFrame.constdata<void>(planeIndex) != sourceFrame.constdata<void>(planeIndex))
							{
								OCEAN_SET_FAILED(validation);
							}
						}

						if (notOwnerFrame.isPlaneOwner(planeIndex))
						{
							OCEAN_SET_FAILED(validation);
						}
					}
				}
				else
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}

		{
			// updating several planes at the same time, writable

			Frame sourceFrame(randomizedFrameType(pixelFormats, &randomGenerator));

			const unsigned int numberPlanes = sourceFrame.numberPlanes();

			Frame notOwnerFrame(sourceFrame, Frame::ACM_USE_KEEP_LAYOUT);

			std::vector<Memory> memories;
			memories.reserve(numberPlanes);

			for (unsigned int n = 0u; n < numberPlanes; ++n)
			{
				memories.emplace_back(1024);
			}

			switch (numberPlanes)
			{
				case 1u:
				{
					if (!notOwnerFrame.updateMemory({memories[0].data()}))
					{
						OCEAN_SET_FAILED(validation);
					}

					break;
				}

				case 2u:
				{
					if (!notOwnerFrame.updateMemory({memories[0].data(), memories[1].data()}))
					{
						OCEAN_SET_FAILED(validation);
					}

					break;
				}

				case 3u:
				{
					if (!notOwnerFrame.updateMemory({memories[0].data(), memories[1].data(), memories[2].data()}))
					{
						OCEAN_SET_FAILED(validation);
					}

					break;
				}

				case 4u:
				{
					if (!notOwnerFrame.updateMemory({memories[0].data(), memories[1].data(), memories[2].data(), memories[3].data()}))
					{
						OCEAN_SET_FAILED(validation);
					}

					break;
				}
			}

			for (unsigned int planeIndex = 0u; planeIndex < numberPlanes; ++planeIndex)
			{
				if (notOwnerFrame.data<void>(planeIndex) != memories[planeIndex].data())
				{
					OCEAN_SET_FAILED(validation);
				}

				if (notOwnerFrame.constdata<void>(planeIndex) != memories[planeIndex].constdata())
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}

		{
			// updating several planes at the same time, read-only

			const Frame sourceFrame(randomizedFrameType(pixelFormats, &randomGenerator));

			const unsigned int numberPlanes = sourceFrame.numberPlanes();

			Frame notOwnerFrame(sourceFrame, Frame::ACM_USE_KEEP_LAYOUT);

			std::vector<Memory> memories;
			memories.reserve(numberPlanes);

			for (unsigned int n = 0u; n < numberPlanes; ++n)
			{
				memories.emplace_back(1024);
			}

			switch (numberPlanes)
			{
				case 1u:
				{
					if (!notOwnerFrame.updateMemory({memories[0].constdata()}))
					{
						OCEAN_SET_FAILED(validation);
					}

					break;
				}

				case 2u:
				{
					if (!notOwnerFrame.updateMemory({memories[0].constdata(), memories[1].constdata()}))
					{
						OCEAN_SET_FAILED(validation);
					}

					break;
				}

				case 3u:
				{
					if (!notOwnerFrame.updateMemory({memories[0].constdata(), memories[1].constdata(), memories[2].constdata()}))
					{
						OCEAN_SET_FAILED(validation);
					}

					break;
				}

				case 4u:
				{
					if (!notOwnerFrame.updateMemory({memories[0].constdata(), memories[1].constdata(), memories[2].constdata(), memories[3].constdata()}))
					{
						OCEAN_SET_FAILED(validation);
					}

					break;
				}

				default:
				{
					ocean_assert(false && "This should never happen!");
					OCEAN_SET_FAILED(validation);

					break;
				}
			}

			for (unsigned int planeIndex = 0u; planeIndex < numberPlanes; ++planeIndex)
			{
				if (notOwnerFrame.data<void>(planeIndex) != nullptr)
				{
					OCEAN_SET_FAILED(validation);
				}

				if (notOwnerFrame.constdata<void>(planeIndex) != memories[planeIndex].constdata())
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}

#ifndef OCEAN_DEBUG

		// a couple of test in release builds, otherwise asserts would fire inside Frame

		{
			Frame ownerFrame(randomizedFrameType(pixelFormats, &randomGenerator));

			const unsigned int numberPlanes = ownerFrame.numberPlanes();

			const unsigned int planeIndex = RandomI::random(randomGenerator, numberPlanes - 1u);

			Memory newExternalMemory(1024);

			void* const originalData = ownerFrame.data<void>(planeIndex);

			if (ownerFrame.updateMemory<void>(newExternalMemory.data(), planeIndex))
			{
				// a frame owning the memory cannot be updated

				OCEAN_SET_FAILED(validation);
			}

			if (ownerFrame.data<void>(planeIndex) != originalData)
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		{
			Frame ownerFrame(randomizedFrameType(pixelFormats, &randomGenerator));

			const unsigned int numberPlanes = ownerFrame.numberPlanes();

			const unsigned int planeIndex = RandomI::random(randomGenerator, numberPlanes, 1000u);

			Memory newExternalMemory(1024);

			if (ownerFrame.updateMemory<void>(newExternalMemory.data(), planeIndex))
			{
				// the plane does not exist

				OCEAN_SET_FAILED(validation);
			}
		}

		{
			Frame ownerFrame(randomizedFrameType(pixelFormats, &randomGenerator));

			const unsigned int numberPlanes = ownerFrame.numberPlanes();

			const unsigned int planeIndex = RandomI::random(randomGenerator, numberPlanes - 1u);

			if (ownerFrame.updateMemory<void>(nullptr, planeIndex))
			{
				// the memory must always be valid

				OCEAN_SET_FAILED(validation);
			}
		}
#endif
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestFrame::testFormatIsPacked()
{
	Log::info() << "Format is packed test:";

	bool allSucceeded = true;

	const std::unordered_set<FrameType::PixelFormat> packedPixelFormats =
	{
		FrameType::FORMAT_BGGR10_PACKED,
		FrameType::FORMAT_RGGB10_PACKED,
		FrameType::FORMAT_Y10_PACKED
	};

	for (const FrameType::PixelFormat pixelFormat : packedPixelFormats)
	{
		if (!FrameType::formatIsPacked(pixelFormat))
		{
			allSucceeded = false;
		}
	}

	for (const FrameType::PixelFormat pixelFormat : FrameType::definedPixelFormats())
	{
		if (packedPixelFormats.find(pixelFormat) == packedPixelFormats.cend())
		{
			if (FrameType::formatIsPacked(pixelFormat))
			{
				allSucceeded = false;
			}
		}
		else
		{
			if (!FrameType::formatIsPacked(pixelFormat))
			{
				allSucceeded = false;
			}
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testTranslateDataType()
{
	Log::info() << "Translate data type test:";

	bool allSucceeded = true;

	for (const FrameType::DataType& dataType : FrameType::definedDataTypes())
	{
		const std::string dataTypeString = FrameType::translateDataType(dataType);

		if (dataTypeString.empty())
		{
			allSucceeded = false;
		}
		else
		{
			const FrameType::DataType translatedDataType = FrameType::translateDataType(dataTypeString);

			if (translatedDataType != dataType)
			{
				allSucceeded = false;
			}
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrame::testTranslatePixelFormat()
{
	Log::info() << "Translate pixel format test:";

	bool allSucceeded = true;

	for (const FrameType::PixelFormat& pixelFormat : definedPixelFormats())
	{
		const std::string pixelFormatString = FrameType::translatePixelFormat(pixelFormat);

		if (pixelFormatString.empty())
		{
			allSucceeded = false;
		}
		else
		{
			const FrameType::PixelFormat translatedPixelFormat = FrameType::translatePixelFormat(pixelFormatString);

			if (translatedPixelFormat != pixelFormat)
			{
				allSucceeded = false;
			}
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestFrame::testPlaneContructors(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int paddingElements)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);

	bool allSucceeded = true;

	{
		// create plane owning the memory

		Frame::Plane plane(width, height, channels, sizeof(T), paddingElements);

		if (plane.isValid() == false)
		{
			allSucceeded = false;
		}

		if (plane.isOwner() == false)
		{
			allSucceeded = false;
		}

		if (plane.isReadOnly())
		{
			allSucceeded = false;
		}

		if (plane.isContinuous() != (paddingElements == 0u))
		{
			allSucceeded = false;
		}

		if (plane.isCompatibleWithDataType<typename SignedTyper<T>::Type>() == false)
		{
			allSucceeded = false;
		}

		if (plane.isCompatibleWithDataType<typename UnsignedTyper<T>::Type>() == false)
		{
			allSucceeded = false;
		}

		if (sizeof(int8_t) != sizeof(T) && plane.isCompatibleWithDataType<int8_t>())
		{
			allSucceeded = false;
		}

		if (sizeof(int16_t) != sizeof(T) && plane.isCompatibleWithDataType<int16_t>())
		{
			allSucceeded = false;
		}

		if (sizeof(int32_t) != sizeof(T) && plane.isCompatibleWithDataType<int32_t>())
		{
			allSucceeded = false;
		}

		if (sizeof(double) != sizeof(T) && plane.isCompatibleWithDataType<double>())
		{
			allSucceeded = false;
		}

		if (plane.strideBytes() != (width * channels + paddingElements) * sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.strideElements() != width * channels + paddingElements)
		{
			allSucceeded = false;
		}

		if (plane.paddingElements() != paddingElements)
		{
			allSucceeded = false;
		}

		if (plane.paddingBytes() != paddingElements * sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.height() != height)
		{
			allSucceeded = false;
		}

		if (plane.size() != (width * channels + paddingElements) * height * sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.elementTypeSize() != sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.constdata<void>() == nullptr || plane.data<void>() == nullptr)
		{
			allSucceeded = false;
		}
	}

	{
		// create plane not owning the memory, using read-only memory

		const Memory memory = Memory::create<T>(height * (width * channels + paddingElements));

		Frame::Plane plane(width, height, channels, memory.constdata<T>(), paddingElements);

		if (plane.isValid() == false)
		{
			allSucceeded = false;
		}

		if (plane.isOwner())
		{
			allSucceeded = false;
		}

		if (plane.isReadOnly() == false)
		{
			allSucceeded = false;
		}

		if (plane.isContinuous() != (paddingElements == 0u))
		{
			allSucceeded = false;
		}

		if (plane.isCompatibleWithDataType<typename SignedTyper<T>::Type>() == false)
		{
			allSucceeded = false;
		}

		if (plane.isCompatibleWithDataType<typename UnsignedTyper<T>::Type>() == false)
		{
			allSucceeded = false;
		}

		if (sizeof(int8_t) != sizeof(T) && plane.isCompatibleWithDataType<int8_t>())
		{
			allSucceeded = false;
		}

		if (sizeof(int16_t) != sizeof(T) && plane.isCompatibleWithDataType<int16_t>())
		{
			allSucceeded = false;
		}

		if (sizeof(int32_t) != sizeof(T) && plane.isCompatibleWithDataType<int32_t>())
		{
			allSucceeded = false;
		}

		if (sizeof(double) != sizeof(T) && plane.isCompatibleWithDataType<double>())
		{
			allSucceeded = false;
		}

		if (plane.strideBytes() != (width * channels + paddingElements) * sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.strideElements() != width * channels + paddingElements)
		{
			allSucceeded = false;
		}

		if (plane.paddingElements() != paddingElements)
		{
			allSucceeded = false;
		}

		if (plane.paddingBytes() != paddingElements * sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.height() != height)
		{
			allSucceeded = false;
		}

		if (plane.size() != (width * channels + paddingElements) * height * sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.elementTypeSize() != sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.constdata<void>() == nullptr || plane.data<void>() != nullptr)
		{
			allSucceeded = false;
		}
	}

	{
		// create plane not owning the memory, using writable memory

		Memory memory = Memory::create<T>(height * (width * channels + paddingElements));

		Frame::Plane plane(width, height, channels, memory.data<T>(), paddingElements);

		if (plane.isValid() == false)
		{
			allSucceeded = false;
		}

		if (plane.isOwner())
		{
			allSucceeded = false;
		}

		if (plane.isReadOnly())
		{
			allSucceeded = false;
		}

		if (plane.isContinuous() != (paddingElements == 0u))
		{
			allSucceeded = false;
		}

		if (plane.isCompatibleWithDataType<typename SignedTyper<T>::Type>() == false)
		{
			allSucceeded = false;
		}

		if (plane.isCompatibleWithDataType<typename UnsignedTyper<T>::Type>() == false)
		{
			allSucceeded = false;
		}

		if (sizeof(int8_t) != sizeof(T) && plane.isCompatibleWithDataType<int8_t>())
		{
			allSucceeded = false;
		}

		if (sizeof(int16_t) != sizeof(T) && plane.isCompatibleWithDataType<int16_t>())
		{
			allSucceeded = false;
		}

		if (sizeof(int32_t) != sizeof(T) && plane.isCompatibleWithDataType<int32_t>())
		{
			allSucceeded = false;
		}

		if (sizeof(double) != sizeof(T) && plane.isCompatibleWithDataType<double>())
		{
			allSucceeded = false;
		}

		if (plane.strideBytes() != (width * channels + paddingElements) * sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.strideElements() != width * channels + paddingElements)
		{
			allSucceeded = false;
		}

		if (plane.paddingElements() != paddingElements)
		{
			allSucceeded = false;
		}

		if (plane.paddingBytes() != paddingElements * sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.height() != height)
		{
			allSucceeded = false;
		}

		if (plane.size() != (width * channels + paddingElements) * height * sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.elementTypeSize() != sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.constdata<void>() == nullptr || plane.data<void>() == nullptr)
		{
			allSucceeded = false;
		}
	}

	{
		// create plane copying the memory

		const unsigned int sourcePaddingElements = RandomI::random(1u, 100u) * RandomI::random(0u, 1u);

		const Memory memory = Memory::create<T>(height * (width * channels + sourcePaddingElements));

		const bool makeCopyOfPaddingData = paddingElements == sourcePaddingElements && RandomI::random(0u, 1u) == 1u;

		Frame::Plane plane(memory.constdata<T>(), width, height, channels, paddingElements, sourcePaddingElements, makeCopyOfPaddingData);

		if (plane.isValid() == false)
		{
			allSucceeded = false;
		}

		if (plane.isOwner() == false)
		{
			allSucceeded = false;
		}

		if (plane.isReadOnly())
		{
			allSucceeded = false;
		}

		if (plane.isContinuous() != (paddingElements == 0u))
		{
			allSucceeded = false;
		}

		if (plane.isCompatibleWithDataType<typename SignedTyper<T>::Type>() == false)
		{
			allSucceeded = false;
		}

		if (plane.isCompatibleWithDataType<typename UnsignedTyper<T>::Type>() == false)
		{
			allSucceeded = false;
		}

		if (sizeof(int8_t) != sizeof(T) && plane.isCompatibleWithDataType<int8_t>())
		{
			allSucceeded = false;
		}

		if (sizeof(int16_t) != sizeof(T) && plane.isCompatibleWithDataType<int16_t>())
		{
			allSucceeded = false;
		}

		if (sizeof(int32_t) != sizeof(T) && plane.isCompatibleWithDataType<int32_t>())
		{
			allSucceeded = false;
		}

		if (sizeof(double) != sizeof(T) && plane.isCompatibleWithDataType<double>())
		{
			allSucceeded = false;
		}

		if (plane.strideBytes() != (width * channels + paddingElements) * sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.strideElements() != width * channels + paddingElements)
		{
			allSucceeded = false;
		}

		if (plane.paddingElements() != paddingElements)
		{
			allSucceeded = false;
		}

		if (plane.paddingBytes() != paddingElements * sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.height() != height)
		{
			allSucceeded = false;
		}

		if (plane.size() != (width * channels + paddingElements) * height * sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.elementTypeSize() != sizeof(T))
		{
			allSucceeded = false;
		}

		if (plane.constdata<void>() == nullptr || plane.data<void>() == nullptr)
		{
			allSucceeded = false;
		}
	}

	{
		// create plane with copy mode

		const std::vector<Frame::CopyMode> copyModes =
		{
			Frame::CM_USE_KEEP_LAYOUT,
			Frame::CM_COPY_REMOVE_PADDING_LAYOUT,
			Frame::CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA,
			Frame::CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA
		};

		const Memory memory = Memory::create<T>(height * (width * channels + paddingElements));

		for (const Frame::CopyMode copyMode : copyModes)
		{
			Frame::Plane plane(memory.constdata<T>(), width, height, channels, paddingElements, copyMode);

			if (plane.isValid() == false)
			{
				allSucceeded = false;
			}

			bool expectedIsOwner = false;
			bool expectedIsReadOnly = false;
			unsigned int expectedStrideBytes = 0u;
			unsigned int expectedPaddingElements = 0u;

			switch (copyMode)
			{
				case Frame::CM_USE_KEEP_LAYOUT:
					expectedIsOwner = false;
					expectedIsReadOnly = true;
					expectedStrideBytes = (width * channels + paddingElements) * sizeof(T);
					expectedPaddingElements = paddingElements;
					break;

				case Frame::CM_COPY_REMOVE_PADDING_LAYOUT:
					expectedIsOwner = true;
					expectedIsReadOnly = false;
					expectedStrideBytes = (width * channels) * sizeof(T);
					expectedPaddingElements = 0u;
					break;

				case Frame::CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA:
					expectedIsOwner = true;
					expectedIsReadOnly = false;
					expectedStrideBytes = (width * channels + paddingElements) * sizeof(T);
					expectedPaddingElements = paddingElements;
					break;

				case Frame::CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA:
					expectedIsOwner = true;
					expectedIsReadOnly = false;
					expectedStrideBytes = (width * channels + paddingElements) * sizeof(T);
					expectedPaddingElements = paddingElements;
					break;
			}

			ocean_assert(expectedStrideBytes != 0u);

			if (plane.isOwner() != expectedIsOwner)
			{
				allSucceeded = false;
			}

			if (plane.isReadOnly() != expectedIsReadOnly)
			{
				allSucceeded = false;
			}

			if (plane.isContinuous() != (expectedPaddingElements == 0u))
			{
				allSucceeded = false;
			}

			if (plane.isCompatibleWithDataType<typename SignedTyper<T>::Type>() == false)
			{
				allSucceeded = false;
			}

			if (plane.isCompatibleWithDataType<typename UnsignedTyper<T>::Type>() == false)
			{
				allSucceeded = false;
			}

			if (sizeof(int8_t) != sizeof(T) && plane.isCompatibleWithDataType<int8_t>())
			{
				allSucceeded = false;
			}

			if (sizeof(int16_t) != sizeof(T) && plane.isCompatibleWithDataType<int16_t>())
			{
				allSucceeded = false;
			}

			if (sizeof(int32_t) != sizeof(T) && plane.isCompatibleWithDataType<int32_t>())
			{
				allSucceeded = false;
			}

			if (sizeof(double) != sizeof(T) && plane.isCompatibleWithDataType<double>())
			{
				allSucceeded = false;
			}

			if (plane.strideBytes() != expectedStrideBytes)
			{
				allSucceeded = false;
			}

			if (plane.strideElements() != expectedStrideBytes / sizeof(T))
			{
				allSucceeded = false;
			}

			if (plane.paddingElements() != expectedPaddingElements)
			{
				allSucceeded = false;
			}

			if (plane.paddingBytes() != expectedPaddingElements * sizeof(T))
			{
				allSucceeded = false;
			}

			if (plane.height() != height)
			{
				allSucceeded = false;
			}

			if (plane.size() != expectedStrideBytes * height)
			{
				allSucceeded = false;
			}

			if (plane.elementTypeSize() != sizeof(T))
			{
				allSucceeded = false;
			}

			if (expectedIsReadOnly)
			{
				if (plane.constdata<void>() == nullptr || plane.data<void>() != nullptr)
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (plane.constdata<void>() == nullptr || plane.data<void>() == nullptr)
				{
					allSucceeded = false;
				}
			}
		}
	}

	return allSucceeded;
}

template <typename T>
bool TestFrame::testPlaneCopyContructors(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int paddingElements)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(channels >= 1u);

	bool allSucceeded = true;

	{
		Frame::Plane sourcePlane(width, height, channels, sizeof(T), paddingElements);

		Frame::Plane newPlane(sourcePlane, Frame::ACM_USE_KEEP_LAYOUT);

		if (newPlane.widthBytes() != sourcePlane.widthBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.strideBytes() != sourcePlane.strideBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.paddingBytes() != sourcePlane.paddingBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.height() != sourcePlane.height())
		{
			allSucceeded = false;
		}

		if (newPlane.isOwner())
		{
			allSucceeded = false;
		}

		if (newPlane.data<void>() != sourcePlane.data<void>() || newPlane.constdata<void>() != sourcePlane.constdata<void>())
		{
			allSucceeded = false;
		}

		if (newPlane.isReadOnly())
		{
			allSucceeded = false;
		}

		if (!newPlane.isCompatibleWithDataType<T>())
		{
			allSucceeded = false;
		}
	}

	{
		Frame::Plane sourcePlane(width, height, channels, sizeof(T), paddingElements);

		Frame::Plane newPlane(sourcePlane, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		if (newPlane.widthBytes() != sourcePlane.widthBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.strideBytes() != sourcePlane.strideBytes() - sourcePlane.paddingBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.paddingBytes() != 0u)
		{
			allSucceeded = false;
		}

		if (newPlane.height() != sourcePlane.height())
		{
			allSucceeded = false;
		}

		if (newPlane.isOwner() == false)
		{
			allSucceeded = false;
		}

		if (newPlane.data<void>() == sourcePlane.data<void>() || newPlane.constdata<void>() == sourcePlane.constdata<void>())
		{
			allSucceeded = false;
		}

		if (newPlane.isReadOnly())
		{
			allSucceeded = false;
		}

		if (!newPlane.isCompatibleWithDataType<T>())
		{
			allSucceeded = false;
		}

		for (unsigned int y = 0u; y < sourcePlane.height(); ++y)
		{
			if (memcmp(newPlane.constdata<uint8_t>() + y * newPlane.strideBytes() , sourcePlane.constdata<uint8_t>() + y * sourcePlane.strideBytes(), newPlane.widthBytes()) != 0)
			{
				allSucceeded = false;
			}
		}
	}

	{
		Frame::Plane sourcePlane(width, height, channels, sizeof(T), paddingElements);

		Frame::Plane newPlane(sourcePlane, Frame::ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA);

		if (newPlane.widthBytes() != sourcePlane.widthBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.strideBytes() != sourcePlane.strideBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.paddingBytes() != sourcePlane.paddingBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.height() != sourcePlane.height())
		{
			allSucceeded = false;
		}

		if (newPlane.isOwner() == false)
		{
			allSucceeded = false;
		}

		if (newPlane.data<void>() == sourcePlane.data<void>() || newPlane.constdata<void>() == sourcePlane.constdata<void>())
		{
			allSucceeded = false;
		}

		if (newPlane.isReadOnly())
		{
			allSucceeded = false;
		}

		if (!newPlane.isCompatibleWithDataType<T>())
		{
			allSucceeded = false;
		}

		for (unsigned int y = 0u; y < sourcePlane.height(); ++y)
		{
			if (memcmp(newPlane.constdata<uint8_t>() + y * newPlane.strideBytes() , sourcePlane.constdata<uint8_t>() + y * sourcePlane.strideBytes(), newPlane.widthBytes()) != 0)
			{
				allSucceeded = false;
			}
		}
	}

	{
		Frame::Plane sourcePlane(width, height, channels, sizeof(T), paddingElements);

		Frame::Plane newPlane(sourcePlane, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		if (newPlane.widthBytes() != sourcePlane.widthBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.strideBytes() != sourcePlane.strideBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.paddingBytes() != sourcePlane.paddingBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.height() != sourcePlane.height())
		{
			allSucceeded = false;
		}

		if (newPlane.isOwner() == false)
		{
			allSucceeded = false;
		}

		if (newPlane.data<void>() == sourcePlane.data<void>() || newPlane.constdata<void>() == sourcePlane.constdata<void>())
		{
			allSucceeded = false;
		}

		if (newPlane.isReadOnly())
		{
			allSucceeded = false;
		}

		if (!newPlane.isCompatibleWithDataType<T>())
		{
			allSucceeded = false;
		}

		if (newPlane.strideBytes() * newPlane.height() != newPlane.size())
		{
			allSucceeded = false;
		}

		if (memcmp(newPlane.constdata<void>(), sourcePlane.constdata<void>(), newPlane.size()) != 0)
		{
			allSucceeded = false;
		}
	}

	{
		// CM_USE_OR_COPY with source plane owning the data

		Frame::Plane sourcePlane(width, height, channels, sizeof(T), paddingElements);

		Frame::Plane newPlane(sourcePlane, Frame::ACM_USE_OR_COPY);

		if (newPlane.widthBytes() != sourcePlane.widthBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.strideBytes() != sourcePlane.strideBytes() - sourcePlane.paddingBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.paddingBytes() != 0u)
		{
			allSucceeded = false;
		}

		if (newPlane.height() != sourcePlane.height())
		{
			allSucceeded = false;
		}

		if (newPlane.isOwner() == false)
		{
			allSucceeded = false;
		}

		if (newPlane.data<void>() == sourcePlane.data<void>() || newPlane.constdata<void>() == sourcePlane.constdata<void>())
		{
			allSucceeded = false;
		}

		if (newPlane.isReadOnly())
		{
			allSucceeded = false;
		}

		if (!newPlane.isCompatibleWithDataType<T>())
		{
			allSucceeded = false;
		}

		for (unsigned int y = 0u; y < sourcePlane.height(); ++y)
		{
			if (memcmp(newPlane.constdata<uint8_t>() + y * newPlane.strideBytes() , sourcePlane.constdata<uint8_t>() + y * sourcePlane.strideBytes(), newPlane.widthBytes()) != 0)
			{
				allSucceeded = false;
			}
		}
	}

	{
		// CM_USE_OR_COPY with source plane not owning the data

		Frame::Plane owningPlane(width, height, channels, sizeof(T), paddingElements);

		for (unsigned int nConst = 0u; nConst < 2u; ++nConst)
		{
			const bool makeReadOnly = nConst == 0u;

			const Frame::Plane sourcePlane = makeReadOnly ? Frame::Plane(width, height, channels, owningPlane.constdata<T>(), paddingElements) : Frame::Plane(width, height, channels, owningPlane.data<T>(), paddingElements);

			Frame::Plane newPlane(sourcePlane, Frame::ACM_USE_OR_COPY);

			if (newPlane.widthBytes() != sourcePlane.widthBytes())
			{
				allSucceeded = false;
			}

			if (newPlane.strideBytes() != sourcePlane.strideBytes())
			{
				allSucceeded = false;
			}

			if (newPlane.paddingBytes() != sourcePlane.paddingBytes())
			{
				allSucceeded = false;
			}

			if (newPlane.height() != sourcePlane.height())
			{
				allSucceeded = false;
			}

			if (newPlane.isOwner())
			{
				allSucceeded = false;
			}

			if (newPlane.constdata<void>() != sourcePlane.constdata<void>())
			{
				allSucceeded = false;
			}

			if (newPlane.isReadOnly() != makeReadOnly)
			{
				allSucceeded = false;
			}

			if (!newPlane.isCompatibleWithDataType<T>())
			{
				allSucceeded = false;
			}
		}
	}

	{
		// CM_USE_OR_COPY_KEEP_LAYOUT with source plane owning the data

		Frame::Plane sourcePlane(width, height, channels, sizeof(T), paddingElements);

		Frame::Plane newPlane(sourcePlane, Frame::ACM_USE_OR_COPY_KEEP_LAYOUT);

		if (newPlane.widthBytes() != sourcePlane.widthBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.strideBytes() != sourcePlane.strideBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.paddingBytes() != sourcePlane.paddingBytes())
		{
			allSucceeded = false;
		}

		if (newPlane.height() != sourcePlane.height())
		{
			allSucceeded = false;
		}

		if (newPlane.isOwner() == false)
		{
			allSucceeded = false;
		}

		if (newPlane.data<void>() == sourcePlane.data<void>() || newPlane.constdata<void>() == sourcePlane.constdata<void>())
		{
			allSucceeded = false;
		}

		if (newPlane.isReadOnly())
		{
			allSucceeded = false;
		}

		if (!newPlane.isCompatibleWithDataType<T>())
		{
			allSucceeded = false;
		}

		for (unsigned int y = 0u; y < sourcePlane.height(); ++y)
		{
			if (memcmp(newPlane.constdata<uint8_t>() + y * newPlane.strideBytes() , sourcePlane.constdata<uint8_t>() + y * sourcePlane.strideBytes(), newPlane.widthBytes()) != 0)
			{
				allSucceeded = false;
			}
		}
	}

	{
		// CM_USE_OR_COPY_KEEP_LAYOUT with source plane not owning the data

		Frame::Plane owningPlane(width, height, channels, sizeof(T), paddingElements);

		for (unsigned int nConst = 0u; nConst < 2u; ++nConst)
		{
			const bool makeReadOnly = nConst == 0u;

			const Frame::Plane sourcePlane = makeReadOnly ? Frame::Plane(width, height, channels, owningPlane.constdata<T>(), paddingElements) : Frame::Plane(width, height, channels, owningPlane.data<T>(), paddingElements);

			Frame::Plane newPlane(sourcePlane, Frame::ACM_USE_OR_COPY_KEEP_LAYOUT);

			if (newPlane.widthBytes() != sourcePlane.widthBytes())
			{
				allSucceeded = false;
			}

			if (newPlane.strideBytes() != sourcePlane.strideBytes())
			{
				allSucceeded = false;
			}

			if (newPlane.paddingBytes() != sourcePlane.paddingBytes())
			{
				allSucceeded = false;
			}

			if (newPlane.height() != sourcePlane.height())
			{
				allSucceeded = false;
			}

			if (newPlane.isOwner())
			{
				allSucceeded = false;
			}

			if (newPlane.constdata<void>() != sourcePlane.constdata<void>())
			{
				allSucceeded = false;
			}

			if (newPlane.isReadOnly() != makeReadOnly)
			{
				allSucceeded = false;
			}

			if (!newPlane.isCompatibleWithDataType<T>())
			{
				allSucceeded = false;
			}
		}
	}

	return allSucceeded;
}

template <typename T>
bool TestFrame::validatePlaneInitializer(const FrameType& frameType, RandomGenerator& randomGenerator)
{
	ocean_assert(frameType.isValid());

	const std::vector<Frame::CopyMode> copyModes =
	{
		Frame::CM_USE_KEEP_LAYOUT,
		Frame::CM_COPY_REMOVE_PADDING_LAYOUT,
		Frame::CM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA,
		Frame::CM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA
	};

	if (frameType.numberPlanes() == 1u)
	{
		const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		{
			// plane without memory pointer, but specified padding elements

			const Frame::PlaneInitializers<T> planeInitializers =
			{
				Frame::PlaneInitializer<T>(paddingElements)
			};

			const Frame frame(frameType, planeInitializers);

			if (frame.planes().size() != 1u)
			{
				return false;
			}

			if (frame.frameType() != frameType)
			{
				return false;
			}

			if (frame.paddingElements() != paddingElements)
			{
				return false;
			}

			if (frame.isOwner() == false)
			{
				return false;
			}

			if (frame.isReadOnly())
			{
				return false;
			}
		}

		{
			// plane without memory pointer, but specified padding elements, in place

			const Frame frame(frameType, Frame::PlaneInitializers<T>{Frame::PlaneInitializer<T>(paddingElements)});

			if (frame.planes().size() != 1u)
			{
				return false;
			}

			if (frame.frameType() != frameType)
			{
				return false;
			}

			if (frame.paddingElements() != paddingElements)
			{
				return false;
			}

			if (frame.isOwner() == false)
			{
				return false;
			}

			if (frame.isReadOnly())
			{
				return false;
			}
		}

		{
			// plane with const memory pointer

			const Frame sourceFrame(frameType, paddingElements);

			const Frame::CopyMode copyMode = copyModes[RandomI::random(randomGenerator, (unsigned int)(copyModes.size() - 1))];

			const Frame frame(frameType, Frame::PlaneInitializers<T>{Frame::PlaneInitializer<T>(sourceFrame.constdata<T>(), copyMode, paddingElements)});

			if (frame.planes().size() != 1u)
			{
				return false;
			}

			if (frame.frameType() != frameType)
			{
				return false;
			}

			const unsigned int expectedPaddingElements = copyMode == Frame::CM_COPY_REMOVE_PADDING_LAYOUT ? 0u : paddingElements;

			if (frame.paddingElements() != expectedPaddingElements)
			{
				return false;
			}

			const bool expectedIsOwner = copyMode != Frame::CM_USE_KEEP_LAYOUT;

			if (frame.isOwner() != expectedIsOwner)
			{
				return false;
			}

			const bool expectedIsReadOnly = copyMode == Frame::CM_USE_KEEP_LAYOUT;

			if (frame.isReadOnly() != expectedIsReadOnly)
			{
				return false;
			}
		}

		{
			// plane with non-const memory pointer

			Frame sourceFrame(frameType, paddingElements);

			const Frame::CopyMode copyMode = copyModes[RandomI::random(randomGenerator, (unsigned int)(copyModes.size() - 1))];

			const Frame frame(frameType, Frame::PlaneInitializers<T>{Frame::PlaneInitializer<T>(sourceFrame.data<T>(), copyMode, paddingElements)});

			if (frame.planes().size() != 1u)
			{
				return false;
			}

			if (frame.frameType() != frameType)
			{
				return false;
			}

			const unsigned int expectedPaddingElements = copyMode == Frame::CM_COPY_REMOVE_PADDING_LAYOUT ? 0u : paddingElements;

			if (frame.paddingElements() != expectedPaddingElements)
			{
				return false;
			}

			const bool expectedIsOwner = copyMode != Frame::CM_USE_KEEP_LAYOUT;

			if (frame.isOwner() != expectedIsOwner)
			{
				return false;
			}

			const bool expectedIsReadOnly = false;

			if (frame.isReadOnly() != expectedIsReadOnly)
			{
				return false;
			}
		}
	}
	else if (frameType.numberPlanes() == 2u)
	{
		const unsigned int paddingElements0 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int paddingElements1 = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		{
			// plane without memory pointer, but specified padding elements

			const Frame::PlaneInitializers<T> planeInitializers =
			{
				Frame::PlaneInitializer<T>(paddingElements0),
				Frame::PlaneInitializer<T>(paddingElements1)
			};

			const Frame frame(frameType, planeInitializers);

			if (frame.planes().size() != 2u)
			{
				return false;
			}

			if (frame.frameType() != frameType)
			{
				return false;
			}

			if (frame.paddingElements(0u) != paddingElements0 || frame.paddingElements(1u) != paddingElements1)
			{
				return false;
			}

			if (frame.isOwner() == false)
			{
				return false;
			}

			if (frame.isReadOnly())
			{
				return false;
			}
		}

		{
			// plane without memory pointer, but specified padding elements, in place

			const Frame frame(frameType, Frame::PlaneInitializers<T>{Frame::PlaneInitializer<T>(paddingElements0), Frame::PlaneInitializer<T>(paddingElements1)});

			if (frame.planes().size() != 2u)
			{
				return false;
			}

			if (frame.frameType() != frameType)
			{
				return false;
			}

			if (frame.paddingElements(0u) != paddingElements0 || frame.paddingElements(1u) != paddingElements1)
			{
				return false;
			}

			if (frame.isOwner() == false)
			{
				return false;
			}

			if (frame.isReadOnly())
			{
				return false;
			}
		}

		unsigned int planeWidth0 = 0u;
		unsigned int planeHeight0 = 0u;
		unsigned int planeChannels0 = 0u;

		unsigned int planeWidth1 = 0u;
		unsigned int planeHeight1 = 0u;
		unsigned int planeChannels1 = 0u;

		if (!FrameType::planeLayout(frameType, 0u, planeWidth0, planeHeight0, planeChannels0))
		{
			return false;
		}

		if (!FrameType::planeLayout(frameType, 1u, planeWidth1, planeHeight1, planeChannels1))
		{
			return false;
		}

		const size_t planeSize0 = size_t((planeWidth0 * planeChannels0 + paddingElements0) * planeHeight0);
		const size_t planeSize1 = size_t((planeWidth1 * planeChannels1 + paddingElements1) * planeHeight1);

		{
			// plane with const memory pointer

			const Memory sourcePlane0(planeSize0);
			const Memory sourcePlane1(planeSize1);

			if (sourcePlane0.isNull() || sourcePlane1.isNull())
			{
				return false;
			}

			const Frame::CopyMode copyMode = copyModes[RandomI::random(randomGenerator, (unsigned int)(copyModes.size() - 1))];

			const Frame frame(frameType, Frame::PlaneInitializers<T>{Frame::PlaneInitializer<T>(sourcePlane0.constdata<T>(), copyMode, paddingElements0), Frame::PlaneInitializer<T>(sourcePlane1.constdata<T>(), copyMode, paddingElements1)});

			if (frame.planes().size() != 2u)
			{
				return false;
			}

			if (frame.frameType() != frameType)
			{
				return false;
			}

			const unsigned int expectedPaddingElements0 = copyMode == Frame::CM_COPY_REMOVE_PADDING_LAYOUT ? 0u : paddingElements0;
			const unsigned int expectedPaddingElements1 = copyMode == Frame::CM_COPY_REMOVE_PADDING_LAYOUT ? 0u : paddingElements1;

			if (frame.paddingElements(0u) != expectedPaddingElements0 || frame.paddingElements(1u) != expectedPaddingElements1)
			{
				return false;
			}

			const bool expectedIsOwner = copyMode != Frame::CM_USE_KEEP_LAYOUT;

			if (frame.isOwner() != expectedIsOwner)
			{
				return false;
			}

			const bool expectedIsReadOnly = copyMode == Frame::CM_USE_KEEP_LAYOUT;

			if (frame.isReadOnly() != expectedIsReadOnly)
			{
				return false;
			}
		}

		{
			// plane with non-const memory pointer

			Memory sourcePlane0(planeSize0);
			Memory sourcePlane1(planeSize1);

			const Frame::CopyMode copyMode = copyModes[RandomI::random(randomGenerator, (unsigned int)(copyModes.size() - 1))];

			const Frame frame(frameType, Frame::PlaneInitializers<T>{Frame::PlaneInitializer<T>(sourcePlane0.data<T>(), copyMode, paddingElements0), Frame::PlaneInitializer<T>(sourcePlane1.data<T>(), copyMode, paddingElements1)});

			if (frame.planes().size() != 2u)
			{
				return false;
			}

			if (frame.frameType() != frameType)
			{
				return false;
			}

			const unsigned int expectedPaddingElements0 = copyMode == Frame::CM_COPY_REMOVE_PADDING_LAYOUT ? 0u : paddingElements0;
			const unsigned int expectedPaddingElements1 = copyMode == Frame::CM_COPY_REMOVE_PADDING_LAYOUT ? 0u : paddingElements1;

			if (frame.paddingElements(0u) != expectedPaddingElements0 || frame.paddingElements(1u) != expectedPaddingElements1)
			{
				return false;
			}

			const bool expectedIsOwner = copyMode != Frame::CM_USE_KEEP_LAYOUT;

			if (frame.isOwner() != expectedIsOwner)
			{
				return false;
			}

			const bool expectedIsReadOnly = false;

			if (frame.isReadOnly() != expectedIsReadOnly)
			{
				return false;
			}
		}
	}
	else
	{
		ocean_assert(frameType.numberPlanes() >= 2u);

		const unsigned int numberPlanes = frameType.numberPlanes();

		unsigned int maxPaddingElements = 0u;

		Indices32 paddingElementPerPlane;

		for (unsigned int nPlane = 0u; nPlane < numberPlanes; ++nPlane)
		{
			const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			paddingElementPerPlane.push_back(paddingElements);

			maxPaddingElements = max(maxPaddingElements, paddingElements);
		}

		{
			// plane without memory pointer, but specified padding elements

			Frame::PlaneInitializers<T> planeInitializers;

			for (unsigned int nPlane = 0u; nPlane < numberPlanes; ++nPlane)
			{
				planeInitializers.push_back(Frame::PlaneInitializer<T>(paddingElementPerPlane[nPlane]));
			}

			const Frame frame(frameType, planeInitializers);

			if (frame.planes().size() != numberPlanes)
			{
				return false;
			}

			if (frame.frameType() != frameType)
			{
				return false;
			}

			for (unsigned int nPlane = 0u; nPlane < numberPlanes; ++nPlane)
			{
				if (frame.paddingElements(nPlane) != paddingElementPerPlane[nPlane])
				{
					return false;
				}
			}

			if (frame.isOwner() == false)
			{
				return false;
			}

			if (frame.isReadOnly())
			{
				return false;
			}
		}

		{
			// plane with const memory pointer

			const Memory allSourcePlanes((frameType.width() * frameType.channels() + maxPaddingElements) * frameType.bytesPerDataType() * frameType.height()); // just enough memory

			const Frame::CopyMode copyMode = copyModes[RandomI::random(randomGenerator, (unsigned int)(copyModes.size() - 1))];

			Frame::PlaneInitializers<T> planeInitializers;

			for (unsigned int nPlane = 0u; nPlane < numberPlanes; ++nPlane)
			{
				planeInitializers.emplace_back(allSourcePlanes.constdata<T>(), copyMode, paddingElementPerPlane[nPlane]);
			}

			const Frame frame(frameType, planeInitializers);

			if (frame.planes().size() != numberPlanes)
			{
				return false;
			}

			if (frame.frameType() != frameType)
			{
				return false;
			}

			for (unsigned int nPlane = 0u; nPlane < numberPlanes; ++nPlane)
			{
				const unsigned int expectedPaddingElements = copyMode == Frame::CM_COPY_REMOVE_PADDING_LAYOUT ? 0u : paddingElementPerPlane[nPlane];

				if (frame.paddingElements(nPlane) != expectedPaddingElements)
				{
					return false;
				}
			}

			const bool expectedIsOwner = copyMode != Frame::CM_USE_KEEP_LAYOUT;

			if (frame.isOwner() != expectedIsOwner)
			{
				return false;
			}

			const bool expectedIsReadOnly = copyMode == Frame::CM_USE_KEEP_LAYOUT;

			if (frame.isReadOnly() != expectedIsReadOnly)
			{
				return false;
			}
		}

		{
			// plane with non-const memory pointer

			Memory allSourcePlanes((frameType.width() * frameType.channels() + maxPaddingElements) * frameType.bytesPerDataType() * frameType.height()); // just enough memory

			const Frame::CopyMode copyMode = copyModes[RandomI::random(randomGenerator, (unsigned int)(copyModes.size() - 1))];

			Frame::PlaneInitializers<T> planeInitializers;

			for (unsigned int nPlane = 0u; nPlane < numberPlanes; ++nPlane)
			{
				planeInitializers.emplace_back(allSourcePlanes.data<T>(), copyMode, paddingElementPerPlane[nPlane]);
			}

			const Frame frame(frameType, planeInitializers);

			if (frame.planes().size() != numberPlanes)
			{
				return false;
			}

			if (frame.frameType() != frameType)
			{
				return false;
			}

			for (unsigned int nPlane = 0u; nPlane < numberPlanes; ++nPlane)
			{
				const unsigned int expectedPaddingElements = copyMode == Frame::CM_COPY_REMOVE_PADDING_LAYOUT ? 0u : paddingElementPerPlane[nPlane];

				if (frame.paddingElements(nPlane) != expectedPaddingElements)
				{
					return false;
				}
			}

			const bool expectedIsOwner = copyMode != Frame::CM_USE_KEEP_LAYOUT;

			if (frame.isOwner() != expectedIsOwner)
			{
				return false;
			}

			const bool expectedIsReadOnly = false;

			if (frame.isReadOnly() != expectedIsReadOnly)
			{
				return false;
			}
		}
	}

	return true;
}

bool TestFrame::validateFrameSpecification(const Frame& frame, const FrameType& frameType, const Indices32& paddingElementsPerPlane, const unsigned int bytesPerElement, const bool isOwner, const bool isReadOnly)
{
	ocean_assert(frame);

	if (frame.frameType() != frameType)
	{
		return false;
	}

	if (frame.isOwner() != isOwner)
	{
		return false;
	}

	if (frame.isReadOnly() != isReadOnly)
	{
		return false;
	}

	const unsigned int framePixels = frameType.width() * frameType.height();

	if (frame.pixels() != framePixels)
	{
		return false;
	}

	if (frame.numberPlanes() != (unsigned int)(paddingElementsPerPlane.size()))
	{
		ocean_assert(false && "Invalid test parameters!");
		return false;
	}

	for (unsigned int planeIndex = 0u; planeIndex < frame.numberPlanes(); ++planeIndex)
	{
		const unsigned int planePaddingElements = paddingElementsPerPlane[planeIndex];

		if (frame.paddingElements(planeIndex) != planePaddingElements)
		{
			return false;
		}

		const unsigned int planePaddingBytes = planePaddingElements * bytesPerElement;

		if (frame.paddingBytes(planeIndex) != planePaddingBytes)
		{
			return false;
		}

		unsigned int planeWidth = 0u;
		unsigned int planeHeight = 0u;
		unsigned int planeChannels = 0u;
		if (!FrameType::planeLayout(frameType, planeIndex, planeWidth, planeHeight, planeChannels))
		{
			ocean_assert(false && "Invalid parameters!");
			return false;
		}

		const unsigned int planeStrideElements = planeWidth * planeChannels + planePaddingElements;

		if (frame.strideElements(planeIndex) != planeStrideElements)
		{
			return false;
		}

		const unsigned int planeStrideBytes = planeStrideElements * bytesPerElement;

		if (frame.strideBytes(planeIndex) != planeStrideBytes)
		{
			return false;
		}

		const unsigned int planeSize = planeStrideBytes * planeHeight;

		if (frame.size(planeIndex) != planeSize)
		{
			return false;
		}
	}

	if (!frame.isValid())
	{
		return false;
	}

	if (!frame)
	{
		return false;
	}

	return true;
}

template <typename T>
bool TestFrame::testSetValue(const Frame& frame, RandomGenerator& randomGenerator)
{
	ocean_assert(frame.isValid());
	ocean_assert(frame.dataType() == FrameType::dataType<T>());

	{
		// testing the byte-value set function

		Frame frameCopy(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const uint8_t byteValue = uint8_t(RandomI::random(randomGenerator, 255u));

		const unsigned int planeIndex = RandomI::random(randomGenerator, frame.numberPlanes() - 1u);
		const bool skipPaddingData = RandomI::random(randomGenerator, 1u) == 0u;

		if (frameCopy.setValue(byteValue, planeIndex, skipPaddingData))
		{
			for (unsigned int nPlane = 0u; nPlane < frame.numberPlanes(); ++nPlane)
			{
				const Frame::Plane& copyPlane = frameCopy.planes()[nPlane];
				const Frame::Plane& originalPlane = frame.planes()[nPlane];

				if (nPlane == planeIndex)
				{
					for (unsigned int y = 0u; y < copyPlane.height(); ++y)
					{
						const unsigned int changedBytes = skipPaddingData ? copyPlane.widthBytes() : copyPlane.strideBytes();
						const unsigned int notChangedBytes = skipPaddingData ? copyPlane.paddingBytes() : 0u;

						const uint8_t* const copyRow = copyPlane.constdata<uint8_t>() + y * copyPlane.strideBytes();
						const uint8_t* const originalRow = originalPlane.constdata<uint8_t>() + y * originalPlane.strideBytes();

						for (unsigned int xByte = 0u; xByte < changedBytes; ++xByte)
						{
							if (copyRow[xByte] != byteValue)
							{
								return false;
							}
						}

						if(memcmp(copyRow + copyPlane.widthBytes(), originalRow + copyPlane.widthBytes(), notChangedBytes) != 0)
						{
							return false;
						}
					}
				}
				else
				{
					// nothing must have changed

					if (memcmp(copyPlane.constdata<void>(), originalPlane.constdata<void>(), copyPlane.size()) != 0)
					{
						return false;
					}
				}
			}
		}
		else
		{
			return false;
		}
	}

	{
		// testing the pixel-value set function

		Frame frameCopy(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const unsigned int planeIndex = RandomI::random(randomGenerator, frame.numberPlanes() - 1u);

		std::vector<T> values(frame.planeChannels(planeIndex));

		for (T& value : values)
		{
			value = T(RandomI::random(randomGenerator, 255u));
		}

		if (frameCopy.setValue(values.data(), values.size(), planeIndex))
		{
			for (unsigned int nPlane = 0u; nPlane < frame.numberPlanes(); ++nPlane)
			{
				const Frame::Plane& copyPlane = frameCopy.planes()[nPlane];
				const Frame::Plane& originalPlane = frame.planes()[nPlane];

				if (nPlane == planeIndex)
				{
					for (unsigned int y = 0u; y < copyPlane.height(); ++y)
					{
						const T* copyRow = (const T*)(copyPlane.constdata<uint8_t>() + y * copyPlane.strideBytes());
						const T* originalRow = (const T*)(originalPlane.constdata<uint8_t>() + y * originalPlane.strideBytes());

						ocean_assert(values.size() == copyPlane.channels());
						ocean_assert(originalPlane.channels() == copyPlane.channels());

						for (unsigned int x = 0u; x < copyPlane.width(); ++x)
						{
							for (unsigned int c = 0u; c < copyPlane.channels(); ++c)
							{
								if (copyRow[c] != values[c])
								{
									return false;
								}
							}

							copyRow += copyPlane.channels();
							originalRow += copyPlane.channels();
						}

						if(memcmp(copyRow, originalRow, copyPlane.paddingBytes()) != 0)
						{
							return false;
						}
					}
				}
				else
				{
					// nothing must have changed

					if (memcmp(copyPlane.constdata<void>(), originalPlane.constdata<void>(), copyPlane.size()) != 0)
					{
						return false;
					}
				}
			}
		}
		else
		{
			return false;
		}
	}

	{
		// testing the initialize-list set function

		Frame frameCopy(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const unsigned int planeIndex = RandomI::random(randomGenerator, frame.numberPlanes() - 1u);

		const unsigned int planeChannels = frame.planeChannels(planeIndex);

		std::vector<T> values(planeChannels);

		for (T& value : values)
		{
			value = T(RandomI::random(randomGenerator, 255u));
		}

		switch (planeChannels)
		{
			case 1u:
			{
				if (!frameCopy.setValue({values[0]}, planeIndex))
				{
					return false;
				}

				break;
			}

			case 2u:
			{
				if (!frameCopy.setValue({values[0], values[1]}, planeIndex))
				{
					return false;
				}

				break;
			}

			case 3u:
			{
				if (!frameCopy.setValue({values[0], values[1], values[2]}, planeIndex))
				{
					return false;
				}

				break;
			}

			case 4u:
			{
				if (!frameCopy.setValue({values[0], values[1], values[2], values[3]}, planeIndex))
				{
					return false;
				}

				break;
			}

			default:
				ocean_assert(false && "Invalid channel number!");
				return false;
		}

		for (unsigned int nPlane = 0u; nPlane < frame.numberPlanes(); ++nPlane)
		{
			const Frame::Plane& copyPlane = frameCopy.planes()[nPlane];
			const Frame::Plane& originalPlane = frame.planes()[nPlane];

			if (nPlane == planeIndex)
			{
				for (unsigned int y = 0u; y < copyPlane.height(); ++y)
				{
					const T* copyRow = (const T*)(copyPlane.constdata<uint8_t>() + y * copyPlane.strideBytes());
					const T* originalRow = (const T*)(originalPlane.constdata<uint8_t>() + y * originalPlane.strideBytes());

					ocean_assert(values.size() == copyPlane.channels());
					ocean_assert(originalPlane.channels() == copyPlane.channels());

					for (unsigned int x = 0u; x < copyPlane.width(); ++x)
					{
						for (unsigned int c = 0u; c < copyPlane.channels(); ++c)
						{
							if (copyRow[c] != values[c])
							{
								return false;
							}
						}

						copyRow += copyPlane.channels();
						originalRow += copyPlane.channels();
					}

					if(memcmp(copyRow, originalRow, copyPlane.paddingBytes()) != 0)
					{
						return false;
					}
				}
			}
			else
			{
				// nothing must have changed

				if (memcmp(copyPlane.constdata<void>(), originalPlane.constdata<void>(), copyPlane.size()) != 0)
				{
					return false;
				}
			}
		}
	}

	return true;
}

FrameType::PixelFormats TestFrame::definedPixelFormats(const FrameType::PixelFormats& genericPixelFormats)
{
	FrameType::PixelFormats pixelFormats = FrameType::definedPixelFormats();

	for (const FrameType::PixelFormat& genericPixelFormat : genericPixelFormats)
	{
		ocean_assert(FrameType::formatIsPureGeneric(genericPixelFormat));

		pixelFormats.emplace_back(genericPixelFormat);
	}

	return pixelFormats;
}

FrameType TestFrame::randomizedFrameType(const FrameType::PixelFormats& pixelFormats, RandomGenerator* randomGenerator)
{
	RandomGenerator localRandomGenerator(randomGenerator);

	const FrameType::PixelFormat pixelFormat = RandomI::random(localRandomGenerator, pixelFormats);

	const unsigned int widthMultiple = FrameType::widthMultiple(pixelFormat);
	const unsigned int heightMultiple = FrameType::heightMultiple(pixelFormat);

	const unsigned int width = RandomI::random(localRandomGenerator, 1u, 1920u) * widthMultiple;
	const unsigned int height = RandomI::random(localRandomGenerator, 1u, 1080u) * heightMultiple;

	const FrameType::PixelOrigin pixelOrigin = RandomI::random(localRandomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

	return FrameType(width, height, pixelFormat, pixelOrigin);
}

}

}

}
