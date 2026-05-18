/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testtracking/TestUtilities.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Bitstream.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Random.h"
#include "ocean/math/SquareMatrix3.h"

#include "ocean/tracking/Database.h"
#include "ocean/tracking/Utilities.h"

#include <sstream>

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

bool TestUtilities::test(const double testDuration, Worker& /*worker*/, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Utilities test");
	Log::info() << " ";

	if (selector.shouldRun("blendframessame"))
	{
		testResult = testBlendFramesSameDimensions(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("blendframesdifferent"))
	{
		testResult = testBlendFramesDifferentDimensions(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("alignframeshomography"))
	{
		testResult = testAlignFramesHomography(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("alignframeshomographyfullcoverage"))
	{
		testResult = testAlignFramesHomographyFullCoverage(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("cameraserialization"))
	{
		testResult = testCameraSerialization(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("readdatabaseerrorpaths"))
	{
		testResult = testReadDatabaseErrorPaths(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("paintcorrespondenceshomographysingular"))
	{
		testResult = testPaintCorrespondencesHomographySingular(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("paintpoints"))
	{
		testResult = testPaintPoints(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("visualizedatabase"))
	{
		testResult = testVisualizeDatabase(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

// Invariant: blendFrames returns a valid blended frame when both inputs have the same FrameType
TEST(TestUtilities, BlendFramesSameDimensions)
{
	EXPECT_TRUE(TestUtilities::testBlendFramesSameDimensions(GTEST_TEST_DURATION));
}

// Invariant: blendFrames computes centering offsets when frames have different dimensions
TEST(TestUtilities, BlendFramesDifferentDimensions)
{
	EXPECT_TRUE(TestUtilities::testBlendFramesDifferentDimensions(GTEST_TEST_DURATION));
}

// Invariant: alignFramesHomography fails for singular homography and succeeds for valid identity
TEST(TestUtilities, AlignFramesHomography)
{
	EXPECT_TRUE(TestUtilities::testAlignFramesHomography(GTEST_TEST_DURATION));
}

// Invariant: alignFramesHomographyFullCoverage rejects oversized results and singular matrices
TEST(TestUtilities, AlignFramesHomographyFullCoverage)
{
	EXPECT_TRUE(TestUtilities::testAlignFramesHomographyFullCoverage(GTEST_TEST_DURATION));
}

// Invariant: writeCamera/readCamera round-trip preserves parameters; zero-dimension yields invalid camera
TEST(TestUtilities, CameraSerialization)
{
	EXPECT_TRUE(TestUtilities::testCameraSerialization(GTEST_TEST_DURATION));
}

// Invariant: readDatabase rejects streams with invalid magic tag or unsupported version
TEST(TestUtilities, ReadDatabaseErrorPaths)
{
	EXPECT_TRUE(TestUtilities::testReadDatabaseErrorPaths(GTEST_TEST_DURATION));
}

// Invariant: paintCorrespondencesHomography returns false for singular (non-invertible) homography
TEST(TestUtilities, PaintCorrespondencesHomographySingular)
{
	EXPECT_TRUE(TestUtilities::testPaintCorrespondencesHomographySingular(GTEST_TEST_DURATION));
}

// Invariant: paintPoints handles radius==0 without crash and skips out-of-bounds points
TEST(TestUtilities, PaintPoints)
{
	EXPECT_TRUE(TestUtilities::testPaintPoints(GTEST_TEST_DURATION));
}

// Invariant: visualizeDatabase returns immediately without modifying frame for missing pose or short path
TEST(TestUtilities, VisualizeDatabase)
{
	EXPECT_TRUE(TestUtilities::testVisualizeDatabase(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestUtilities::testBlendFramesSameDimensions(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test blendFrames (same dimensions):";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 10u, 200u);
		const unsigned int height = RandomI::random(randomGenerator, 10u, 200u);

		Frame frame0(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		Frame frame1(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

		frame0.setValue(0x40u);
		frame1.setValue(0xC0u);

		const Frame result = Tracking::Utilities::blendFrames(frame0, frame1);

		if (!result.isValid())
		{
			OCEAN_SET_FAILED(validation);
		}
		else
		{
			OCEAN_EXPECT_EQUAL(validation, result.width(), width);
			OCEAN_EXPECT_EQUAL(validation, result.height(), height);
			OCEAN_EXPECT_EQUAL(validation, result.pixelFormat(), FrameType::FORMAT_RGB24);

			// blending 0x40 and 0xC0 at alpha=0x80 should produce values in range [0x40, 0xC0]
			const uint8_t* centerPixel = result.constpixel<uint8_t>(width / 2u, height / 2u);
			for (unsigned int c = 0u; c < 3u; ++c)
			{
				if (centerPixel[c] < 0x40u || centerPixel[c] > 0xC0u)
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testBlendFramesDifferentDimensions(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test blendFrames (different dimensions with offsets):";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width0 = RandomI::random(randomGenerator, 10u, 100u);
		const unsigned int height0 = RandomI::random(randomGenerator, 10u, 100u);
		const unsigned int width1 = RandomI::random(randomGenerator, 10u, 100u);
		const unsigned int height1 = RandomI::random(randomGenerator, 10u, 100u);

		Frame frame0(FrameType(width0, height0, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		Frame frame1(FrameType(width1, height1, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

		frame0.setValue(0x40u);
		frame1.setValue(0xC0u);

		Vector2 offset0;
		Vector2 offset1;

		const Frame result = Tracking::Utilities::blendFrames(frame0, frame1, offset0, offset1);

		const unsigned int expectedWidth = std::max(width0, width1);
		const unsigned int expectedHeight = std::max(height0, height1);

		if (!result.isValid())
		{
			OCEAN_SET_FAILED(validation);
		}
		else
		{
			OCEAN_EXPECT_EQUAL(validation, result.width(), expectedWidth);
			OCEAN_EXPECT_EQUAL(validation, result.height(), expectedHeight);

			// offset for the smaller frame should be non-zero if dimensions differ
			if (width0 < width1 || height0 < height1)
			{
				const Scalar expectedOffsetX = Scalar(int(expectedWidth - width0) / 2);
				const Scalar expectedOffsetY = Scalar(int(expectedHeight - height0) / 2);

				OCEAN_EXPECT_EQUAL(validation, offset0.x(), expectedOffsetX);
				OCEAN_EXPECT_EQUAL(validation, offset0.y(), expectedOffsetY);
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, offset0.x(), Scalar(0));
				OCEAN_EXPECT_EQUAL(validation, offset0.y(), Scalar(0));
			}

			if (width1 < width0 || height1 < height0)
			{
				const Scalar expectedOffsetX = Scalar(int(expectedWidth - width1) / 2);
				const Scalar expectedOffsetY = Scalar(int(expectedHeight - height1) / 2);

				OCEAN_EXPECT_EQUAL(validation, offset1.x(), expectedOffsetX);
				OCEAN_EXPECT_EQUAL(validation, offset1.y(), expectedOffsetY);
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, offset1.x(), Scalar(0));
				OCEAN_EXPECT_EQUAL(validation, offset1.y(), Scalar(0));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testAlignFramesHomography(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test alignFramesHomography:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 20u, 100u);
		const unsigned int height = RandomI::random(randomGenerator, 20u, 100u);

		Frame fixedFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		Frame dynamicFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

		fixedFrame.setValue(0x80u);
		dynamicFrame.setValue(0x40u);

		const SquareMatrix3 identityHomography(true);

		// blend=true path uses bilinear interpolation + FrameBlender
		Frame resultBlend;
		if (!Tracking::Utilities::alignFramesHomography(fixedFrame, dynamicFrame, identityHomography, resultBlend, /*blend=*/true))
		{
			OCEAN_SET_FAILED(validation);
		}
		else
		{
			OCEAN_EXPECT_EQUAL(validation, resultBlend.width(), width);
			OCEAN_EXPECT_EQUAL(validation, resultBlend.height(), height);
			OCEAN_EXPECT_TRUE(validation, resultBlend.isValid());
		}

		// blend=false path uses homographyMask + manual pixel copy
		Frame resultNoBlend;
		if (!Tracking::Utilities::alignFramesHomography(fixedFrame, dynamicFrame, identityHomography, resultNoBlend, /*blend=*/false))
		{
			OCEAN_SET_FAILED(validation);
		}
		else
		{
			OCEAN_EXPECT_EQUAL(validation, resultNoBlend.width(), width);
			OCEAN_EXPECT_EQUAL(validation, resultNoBlend.height(), height);
			OCEAN_EXPECT_TRUE(validation, resultNoBlend.isValid());

			// non-blend path copies dynamicFrame pixels over fixedFrame where mask is set
			// center pixel should be from dynamicFrame (0x40), not fixedFrame (0x80)
			const uint8_t* noBlendPixel = resultNoBlend.constpixel<uint8_t>(width / 2u, height / 2u);
			OCEAN_EXPECT_EQUAL(validation, noBlendPixel[0], (uint8_t)0x40u);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testAlignFramesHomographyFullCoverage(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test alignFramesHomographyFullCoverage:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 20u, 80u);
		const unsigned int height = RandomI::random(randomGenerator, 20u, 80u);

		Frame fixedFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		Frame dynamicFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

		fixedFrame.setValue(0x80u);
		dynamicFrame.setValue(0x40u);

		// singular homography must fail
		const SquareMatrix3 singularHomography(false);
		Frame resultSingular;
		if (Tracking::Utilities::alignFramesHomographyFullCoverage(fixedFrame, dynamicFrame, singularHomography, resultSingular, /*blend=*/true))
		{
			OCEAN_SET_FAILED(validation);
		}

		// identity homography with blend=true must produce a result at least as large as the input
		const SquareMatrix3 identityHomography(true);
		Frame resultBlend;
		if (!Tracking::Utilities::alignFramesHomographyFullCoverage(fixedFrame, dynamicFrame, identityHomography, resultBlend, /*blend=*/true))
		{
			OCEAN_SET_FAILED(validation);
		}
		else
		{
			OCEAN_EXPECT_TRUE(validation, resultBlend.isValid());
			OCEAN_EXPECT_TRUE(validation, resultBlend.width() >= width);
			OCEAN_EXPECT_TRUE(validation, resultBlend.height() >= height);
		}

		// maximalWidth/Height guard: setting very small limits should cause failure with a large homography
		const SquareMatrix3 translationHomography(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(Scalar(200), Scalar(200), 1));
		Frame resultTooLarge;
		if (Tracking::Utilities::alignFramesHomographyFullCoverage(fixedFrame, dynamicFrame, translationHomography, resultTooLarge, /*blend=*/true, /*worker=*/nullptr, /*maximalWidth=*/50u, /*maximalHeight=*/50u))
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testCameraSerialization(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test writeCamera / readCamera:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// round-trip with valid camera
		const unsigned int cameraWidth = RandomI::random(randomGenerator, 100u, 1920u);
		const unsigned int cameraHeight = RandomI::random(randomGenerator, 100u, 1080u);
		const Scalar focalLengthX = Random::scalar(randomGenerator, Scalar(200), Scalar(1500));
		const Scalar focalLengthY = Random::scalar(randomGenerator, Scalar(200), Scalar(1500));
		const Scalar principalX = Random::scalar(randomGenerator, Scalar(0), Scalar(cameraWidth));
		const Scalar principalY = Random::scalar(randomGenerator, Scalar(0), Scalar(cameraHeight));

		const PinholeCamera originalCamera(cameraWidth, cameraHeight, focalLengthX, focalLengthY, principalX, principalY);

		std::ostringstream outputStringStream(std::ios::binary);
		IO::OutputBitstream outputStream(outputStringStream);

		if (!Tracking::Utilities::writeCamera(originalCamera, outputStream))
		{
			OCEAN_SET_FAILED(validation);
			continue;
		}

		std::istringstream inputStringStream(outputStringStream.str(), std::ios::binary);
		IO::InputBitstream inputStream(inputStringStream);

		PinholeCamera readCamera;
		if (!Tracking::Utilities::readCamera(inputStream, readCamera))
		{
			OCEAN_SET_FAILED(validation);
			continue;
		}

		OCEAN_EXPECT_EQUAL(validation, readCamera.width(), cameraWidth);
		OCEAN_EXPECT_EQUAL(validation, readCamera.height(), cameraHeight);
		OCEAN_EXPECT_TRUE(validation, readCamera.isValid());

		// zero-dimension detection: writing a default (invalid) camera and reading it back
		const PinholeCamera invalidCamera;

		std::ostringstream invalidOutputStream(std::ios::binary);
		IO::OutputBitstream invalidOutputBitstream(invalidOutputStream);

		if (!Tracking::Utilities::writeCamera(invalidCamera, invalidOutputBitstream))
		{
			OCEAN_SET_FAILED(validation);
			continue;
		}

		std::istringstream invalidInputStream(invalidOutputStream.str(), std::ios::binary);
		IO::InputBitstream invalidInputBitstream(invalidInputStream);

		PinholeCamera readInvalidCamera(640u, 480u, Scalar(500), Scalar(500), Scalar(320), Scalar(240));
		if (!Tracking::Utilities::readCamera(invalidInputBitstream, readInvalidCamera))
		{
			OCEAN_SET_FAILED(validation);
			continue;
		}

		// after reading zero-dimension data, the camera should be invalid (default-constructed)
		OCEAN_EXPECT_FALSE(validation, readInvalidCamera.isValid());
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testReadDatabaseErrorPaths(const double /*testDuration*/)
{
	Log::info() << "Test readDatabase error paths:";

	Validation validation;

	// invalid magic tag
	{
		std::ostringstream outputStringStream(std::ios::binary);
		IO::OutputBitstream outputStream(outputStringStream);

		outputStream.write<std::string>("INVALID_TAG");
		outputStream.write<unsigned int>(1u);

		std::istringstream inputStringStream(outputStringStream.str(), std::ios::binary);
		IO::InputBitstream inputStream(inputStringStream);

		Tracking::Database database;
		if (Tracking::Utilities::readDatabase(inputStream, database))
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	// wrong version number
	{
		std::ostringstream outputStringStream(std::ios::binary);
		IO::OutputBitstream outputStream(outputStringStream);

		outputStream.write<std::string>("OCN_TRACKING_DATABASE");
		outputStream.write<unsigned int>(99u);

		std::istringstream inputStringStream(outputStringStream.str(), std::ios::binary);
		IO::InputBitstream inputStream(inputStringStream);

		Tracking::Database database;
		if (Tracking::Utilities::readDatabase(inputStream, database))
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	// empty stream
	{
		std::istringstream emptyStream("", std::ios::binary);
		IO::InputBitstream inputStream(emptyStream);

		Tracking::Database database;
		if (Tracking::Utilities::readDatabase(inputStream, database))
		{
			OCEAN_SET_FAILED(validation);
		}
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testPaintCorrespondencesHomographySingular(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test paintCorrespondencesHomography with singular homography:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 20u, 100u);
		const unsigned int height = RandomI::random(randomGenerator, 20u, 100u);

		Frame frame0(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		Frame frame1(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

		frame0.setValue(0x80u);
		frame1.setValue(0x40u);

		// singular (all-zeros) homography cannot be inverted => must return false
		const SquareMatrix3 singularHomography(false);
		Frame result;

		if (Tracking::Utilities::paintCorrespondencesHomography(frame0, frame1, singularHomography, /*points0=*/nullptr, /*points1=*/nullptr, /*numberPoints=*/0, /*fullCoverage=*/false, result))
		{
			OCEAN_SET_FAILED(validation);
		}

		if (Tracking::Utilities::paintCorrespondencesHomography(frame0, frame1, singularHomography, /*points0=*/nullptr, /*points1=*/nullptr, /*numberPoints=*/0, /*fullCoverage=*/true, result))
		{
			OCEAN_SET_FAILED(validation);
		}

		// valid identity homography must succeed
		const SquareMatrix3 identityHomography(true);
		Frame resultValid;

		if (!Tracking::Utilities::paintCorrespondencesHomography(frame0, frame1, identityHomography, /*points0=*/nullptr, /*points1=*/nullptr, /*numberPoints=*/0, /*fullCoverage=*/false, resultValid))
		{
			OCEAN_SET_FAILED(validation);
		}
		else
		{
			OCEAN_EXPECT_TRUE(validation, resultValid.isValid());
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testPaintPoints(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test paintPoints:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 50u, 200u);
		const unsigned int height = RandomI::random(randomGenerator, 50u, 200u);

		Frame frame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		frame.setValue(0x00u);

		// radius==0 special case: raw pixel drawing, frame should be modified
		{
			Frame testFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
			testFrame.setValue(0x00u);

			const unsigned int cx = width / 2u;
			const unsigned int cy = height / 2u;
			const Vector2 point = Vector2(Scalar(cx), Scalar(cy));
			const uint8_t color[3] = {0xFF, 0x00, 0x00};

			Tracking::Utilities::paintPoints(testFrame, &point, /*number=*/1, /*radius=*/0u, color);

			bool anyPixelModified = false;
			for (unsigned int y = 0u; y < height && !anyPixelModified; ++y)
			{
				const uint8_t* row = testFrame.constrow<uint8_t>(y);
				for (unsigned int x = 0u; x < width * 3u && !anyPixelModified; ++x)
				{
					if (row[x] != 0x00u)
					{
						anyPixelModified = true;
					}
				}
			}
			OCEAN_EXPECT_TRUE(validation, anyPixelModified);
		}

		// radius > 0 with in-bounds point: center pixel should be modified
		{
			Frame testFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
			testFrame.setValue(0x00u);

			const unsigned int cx = width / 2u;
			const unsigned int cy = height / 2u;
			const Vector2 point = Vector2(Scalar(cx), Scalar(cy));
			const uint8_t color[3] = {0x00, 0xFF, 0x00};

			Tracking::Utilities::paintPoints(testFrame, &point, /*number=*/1, /*radius=*/5u, color);

			const uint8_t* pixel = testFrame.constpixel<uint8_t>(cx, cy);
			bool pixelModified = (pixel[0] != 0x00u || pixel[1] != 0x00u || pixel[2] != 0x00u);
			OCEAN_EXPECT_TRUE(validation, pixelModified);
		}

		// out-of-bounds point should be skipped: frame must remain unchanged
		{
			Frame testFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
			testFrame.setValue(0x00u);

			const Vector2 outOfBoundsPoint(Scalar(width + 100u), Scalar(height + 100u));
			const uint8_t color[3] = {0x00, 0x00, 0xFF};

			Tracking::Utilities::paintPoints(testFrame, &outOfBoundsPoint, /*number=*/1, /*radius=*/5u, color);

			bool allZero = true;
			for (unsigned int y = 0u; y < height && allZero; ++y)
			{
				const uint8_t* row = testFrame.constrow<uint8_t>(y);
				for (unsigned int x = 0u; x < width * 3u && allZero; ++x)
				{
					if (row[x] != 0x00u)
					{
						allZero = false;
					}
				}
			}
			OCEAN_EXPECT_TRUE(validation, allZero);
		}

		// zero points should leave frame unchanged
		{
			Frame testFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
			testFrame.setValue(0xAAu);

			const Vector2 dummyPoint(0, 0);
			Tracking::Utilities::paintPoints(testFrame, &dummyPoint, /*number=*/0, /*radius=*/5u);

			const uint8_t* pixel = testFrame.constpixel<uint8_t>(0u, 0u);
			OCEAN_EXPECT_EQUAL(validation, pixel[0], (uint8_t)0xAAu);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestUtilities::testVisualizeDatabase(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test visualizeDatabase:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 50u, 200u);
		const unsigned int height = RandomI::random(randomGenerator, 50u, 200u);

		Frame frame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		frame.setValue(0x00u);

		Frame frameCopy(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

		Tracking::Database database;

		const uint8_t colorPoints[3] = {0xFF, 0x00, 0x00};
		const uint8_t colorInstable[3] = {0x00, 0xFF, 0x00};
		const uint8_t colorStable[3] = {0x00, 0x00, 0xFF};

		// pose not in database: should return immediately without modifying frame
		Tracking::Utilities::visualizeDatabase(database, /*poseId=*/0u, frame, colorPoints, colorInstable, colorStable, /*maximalPathLength=*/10u, /*stablePathLength=*/5u);

		// frame should be unchanged when pose is not in database
		bool framesIdentical = true;
		for (unsigned int y = 0u; y < height && framesIdentical; ++y)
		{
			if (memcmp(frame.constrow<uint8_t>(y), frameCopy.constrow<uint8_t>(y), width * 3u) != 0)
			{
				framesIdentical = false;
			}
		}
		OCEAN_EXPECT_TRUE(validation, framesIdentical);

		// add a pose with image points to test the maximalPathLength < 2u early return
		const Index32 poseId = 0u;
		database.addPose<false>(poseId, HomogenousMatrix4(true));
		const Index32 objectPointId = database.addObjectPoint<false>(Vector3(0, 0, 0));
		const Index32 imagePointId = database.addImagePoint<false>(Vector2(Scalar(width / 2u), Scalar(height / 2u)));
		database.attachImagePointToPose<false>(imagePointId, poseId);
		database.attachImagePointToObjectPoint<false>(imagePointId, objectPointId);

		Frame frame2(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		frame2.setValue(0x00u);

		// maximalPathLength < 2u causes early return after painting initial points (but points are still painted)
		Tracking::Utilities::visualizeDatabase(database, poseId, frame2, colorPoints, colorInstable, colorStable, /*maximalPathLength=*/1u, /*stablePathLength=*/5u);

		// the image point should still be painted even with short path length
		bool frame2Modified = false;
		for (unsigned int y = 0u; y < height && !frame2Modified; ++y)
		{
			const uint8_t* row = frame2.constrow<uint8_t>(y);
			for (unsigned int x = 0u; x < width * 3u && !frame2Modified; ++x)
			{
				if (row[x] != 0x00u)
				{
					frame2Modified = true;
				}
			}
		}
		OCEAN_EXPECT_TRUE(validation, frame2Modified);

		// with identity transformation: image point should be painted at its original position
		Frame frame3(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		frame3.setValue(0x00u);

		const SquareMatrix3 identity(true);
		Tracking::Utilities::visualizeDatabase(database, poseId, frame3, colorPoints, colorInstable, colorStable, /*maximalPathLength=*/10u, /*stablePathLength=*/5u, identity);

		bool frame3Modified = false;
		for (unsigned int y = 0u; y < height && !frame3Modified; ++y)
		{
			const uint8_t* row = frame3.constrow<uint8_t>(y);
			for (unsigned int x = 0u; x < width * 3u && !frame3Modified; ++x)
			{
				if (row[x] != 0x00u)
				{
					frame3Modified = true;
				}
			}
		}
		OCEAN_EXPECT_TRUE(validation, frame3Modified);

		// with non-identity transformation: frame should still be modified
		Frame frame4(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		frame4.setValue(0x00u);

		const SquareMatrix3 scaleTransform(Vector3(Scalar(0.5), 0, 0), Vector3(0, Scalar(0.5), 0), Vector3(0, 0, 1));
		Tracking::Utilities::visualizeDatabase(database, poseId, frame4, colorPoints, colorInstable, colorStable, /*maximalPathLength=*/10u, /*stablePathLength=*/5u, scaleTransform);

		bool frame4Modified = false;
		for (unsigned int y = 0u; y < height && !frame4Modified; ++y)
		{
			const uint8_t* row = frame4.constrow<uint8_t>(y);
			for (unsigned int x = 0u; x < width * 3u && !frame4Modified; ++x)
			{
				if (row[x] != 0x00u)
				{
					frame4Modified = true;
				}
			}
		}
		OCEAN_EXPECT_TRUE(validation, frame4Modified);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
