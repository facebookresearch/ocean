/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testio/testserialization/TestMediaSerializer.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/Validation.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/Bitstream.h"
#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"
#include "ocean/math/Vector3.h"

#include <sstream>

namespace Ocean
{

namespace Test
{

namespace TestIO
{

namespace TestSerialization
{

bool TestMediaSerializer::test(const double testDuration, const TestSelector& selector)
{
	TestResult testResult("MediaSerializer test");

	Log::info() << " ";

	if (selector.shouldRun("sampleframeconstructor"))
	{
		testResult = testSampleFrameConstructor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("sampleframereadwrite"))
	{
		testResult = testSampleFrameReadWrite(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("sampleframecustombuffer"))
	{
		testResult = testSampleFrameCustomBuffer(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("sampleframewithcamera"))
	{
		testResult = testSampleFrameWithCamera(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(MediaSerializer, SampleFrameConstructor)
{
	EXPECT_TRUE(TestMediaSerializer::testSampleFrameConstructor(GTEST_TEST_DURATION));
}

TEST(MediaSerializer, SampleFrameReadWrite)
{
	EXPECT_TRUE(TestMediaSerializer::testSampleFrameReadWrite(GTEST_TEST_DURATION));
}

TEST(MediaSerializer, SampleFrameCustomBuffer)
{
	EXPECT_TRUE(TestMediaSerializer::testSampleFrameCustomBuffer(GTEST_TEST_DURATION));
}

TEST(MediaSerializer, SampleFrameWithCamera)
{
	EXPECT_TRUE(TestMediaSerializer::testSampleFrameWithCamera(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestMediaSerializer::testSampleFrameConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SampleFrame constructor test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const Frame frame = randomFrame(randomGenerator);

		// first we serialize the random frame

		IO::Serialization::MediaSerializer::DataSampleFrame sampleFrame(frame, "ocn");
		sampleFrame.configurePlaybackTimestamp(startTimestamp);

		OCEAN_EXPECT_TRUE(validation, sampleFrame.isValid());

		OCEAN_EXPECT_EQUAL(validation, sampleFrame.imageType(), std::string("ocn"));
		OCEAN_EXPECT_FALSE(validation, sampleFrame.buffer().empty());

		// now we deserialize the frame

		const Frame decodedFrame = sampleFrame.frame();

		OCEAN_EXPECT_TRUE(validation, decodedFrame.isValid());

		if (decodedFrame.isValid())
		{
			OCEAN_EXPECT_EQUAL(validation, decodedFrame.frameType(), frame.frameType());

			OCEAN_EXPECT_EQUAL(validation, decodedFrame.timestamp(), frame.timestamp());

			OCEAN_EXPECT_TRUE(validation, isFrameContentEqual(frame, decodedFrame));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestMediaSerializer::testSampleFrameReadWrite(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SampleFrame read/write test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const Frame frame = randomFrame(randomGenerator);

		IO::Serialization::MediaSerializer::DataSampleFrame originalSampleFrame(frame, "ocn");
		originalSampleFrame.configurePlaybackTimestamp(startTimestamp);

		OCEAN_EXPECT_TRUE(validation, originalSampleFrame.isValid());

		std::ostringstream outputStream;
		IO::OutputBitstream outputBitstream(outputStream);

		const bool writeSuccess = originalSampleFrame.writeSample(outputBitstream);
		OCEAN_EXPECT_TRUE(validation, writeSuccess);

		if (writeSuccess)
		{
			const std::string data = outputStream.str();
			std::istringstream inputStream(data);
			IO::InputBitstream inputBitstream(inputStream);

			IO::Serialization::MediaSerializer::DataSampleFrame readSampleFrame;

			const bool readSuccess = readSampleFrame.readSample(inputBitstream);
			OCEAN_EXPECT_TRUE(validation, readSuccess);

			if (readSuccess)
			{
				OCEAN_EXPECT_TRUE(validation, readSampleFrame.isValid());

				const Frame decodedFrame = readSampleFrame.frame();

				OCEAN_EXPECT_TRUE(validation, decodedFrame.isValid());

				if (decodedFrame.isValid())
				{
					OCEAN_EXPECT_EQUAL(validation, decodedFrame.frameType(), frame.frameType());

					OCEAN_EXPECT_EQUAL(validation, decodedFrame.timestamp(), frame.timestamp());

					OCEAN_EXPECT_TRUE(validation, isFrameContentEqual(frame, decodedFrame));
				}
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestMediaSerializer::testSampleFrameCustomBuffer(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SampleFrame custom buffer test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const size_t bufferSize = RandomI::random(randomGenerator, 100u, 10000u);
		std::vector<uint8_t> buffer(bufferSize);

		for (size_t i = 0; i < bufferSize; ++i)
		{
			buffer[i] = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const std::string imageType = "custom";
		const IO::Serialization::DataTimestamp dataTimestamp(double(Timestamp(true)));

		const std::vector<uint8_t> originalBuffer = buffer;

		IO::Serialization::MediaSerializer::DataSampleFrame sampleFrame(std::move(buffer), imageType, dataTimestamp);
		sampleFrame.configurePlaybackTimestamp(startTimestamp);

		OCEAN_EXPECT_TRUE(validation, sampleFrame.isValid());
		OCEAN_EXPECT_EQUAL(validation, sampleFrame.imageType(), imageType);
		OCEAN_EXPECT_EQUAL(validation, sampleFrame.buffer().size(), originalBuffer.size());

		const bool bufferMatches = memcmp(sampleFrame.buffer().data(), originalBuffer.data(), originalBuffer.size()) == 0;
		OCEAN_EXPECT_TRUE(validation, bufferMatches);

		std::ostringstream outputStream;
		IO::OutputBitstream outputBitstream(outputStream);

		const bool writeSuccess = sampleFrame.writeSample(outputBitstream);
		OCEAN_EXPECT_TRUE(validation, writeSuccess);

		if (writeSuccess)
		{
			const std::string data = outputStream.str();
			std::istringstream inputStream(data);
			IO::InputBitstream inputBitstream(inputStream);

			IO::Serialization::MediaSerializer::DataSampleFrame readSampleFrame;

			const bool readSuccess = readSampleFrame.readSample(inputBitstream);
			OCEAN_EXPECT_TRUE(validation, readSuccess);

			if (readSuccess)
			{
				OCEAN_EXPECT_TRUE(validation, readSampleFrame.isValid());
				OCEAN_EXPECT_EQUAL(validation, readSampleFrame.imageType(), imageType);
				OCEAN_EXPECT_EQUAL(validation, readSampleFrame.buffer().size(), originalBuffer.size());

				const bool readBufferMatches = memcmp(readSampleFrame.buffer().data(), originalBuffer.data(), originalBuffer.size()) == 0;
				OCEAN_EXPECT_TRUE(validation, readBufferMatches);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestMediaSerializer::testSampleFrameWithCamera(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "SampleFrame with camera test:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const Frame frame = randomFrame(randomGenerator);

		const unsigned int width = frame.width();
		const unsigned int height = frame.height();

		const Scalar fovX = Random::scalar(randomGenerator, Numeric::deg2rad(40), Numeric::deg2rad(90));
		const PinholeCamera pinholeCamera(width, height, fovX);

		const SharedAnyCamera originalCamera = std::make_shared<AnyCameraPinhole>(pinholeCamera);

		HomogenousMatrixD4 originalDevice_T_camera(false);

		if (RandomI::boolean(randomGenerator))
		{
			const VectorD3 translation = RandomD::vector3(randomGenerator, -10.0, 10.0);
			const QuaternionD rotation = RandomD::quaternion(randomGenerator);

			originalDevice_T_camera = HomogenousMatrixD4(translation, rotation);
		}

		IO::Serialization::MediaSerializer::DataSampleFrame sampleFrame(frame, "ocn", originalCamera, originalDevice_T_camera);
		sampleFrame.configurePlaybackTimestamp(startTimestamp);

		OCEAN_EXPECT_TRUE(validation, sampleFrame.isValid());
		OCEAN_EXPECT_FALSE(validation, sampleFrame.cameraModel().empty());

		const HomogenousMatrixD4& retrievedDevice_T_camera = sampleFrame.device_T_camera();

		OCEAN_EXPECT_EQUAL(validation, retrievedDevice_T_camera, originalDevice_T_camera);

		// Verify camera can be retrieved
		const SharedAnyCamera retrievedCamera = sampleFrame.camera();
		OCEAN_EXPECT_TRUE(validation, retrievedCamera != nullptr);

		if (retrievedCamera != nullptr)
		{
			OCEAN_EXPECT_EQUAL(validation, retrievedCamera->width(), width);
			OCEAN_EXPECT_EQUAL(validation, retrievedCamera->height(), height);
			OCEAN_EXPECT_EQUAL(validation, retrievedCamera->name(), originalCamera->name());
		}

		// Test frame() with camera parameter
		SharedAnyCamera cameraFromFrame;
		const Frame decodedFrame = sampleFrame.frame(&cameraFromFrame);

		OCEAN_EXPECT_TRUE(validation, decodedFrame.isValid());
		OCEAN_EXPECT_TRUE(validation, cameraFromFrame != nullptr);

		if (cameraFromFrame != nullptr)
		{
			OCEAN_EXPECT_EQUAL(validation, cameraFromFrame->width(), width);
			OCEAN_EXPECT_EQUAL(validation, cameraFromFrame->height(), height);
		}

		// Test serialization/deserialization
		std::ostringstream outputStream;
		IO::OutputBitstream outputBitstream(outputStream);

		const bool writeSuccess = sampleFrame.writeSample(outputBitstream);
		OCEAN_EXPECT_TRUE(validation, writeSuccess);

		if (writeSuccess)
		{
			const std::string data = outputStream.str();
			std::istringstream inputStream(data);
			IO::InputBitstream inputBitstream(inputStream);

			IO::Serialization::MediaSerializer::DataSampleFrame readSampleFrame;

			const bool readSuccess = readSampleFrame.readSample(inputBitstream);
			OCEAN_EXPECT_TRUE(validation, readSuccess);

			if (readSuccess)
			{
				OCEAN_EXPECT_TRUE(validation, readSampleFrame.isValid());
				OCEAN_EXPECT_FALSE(validation, readSampleFrame.cameraModel().empty());

				const HomogenousMatrixD4& deserializedDevice_T_camera = readSampleFrame.device_T_camera();

				OCEAN_EXPECT_EQUAL(validation, deserializedDevice_T_camera, originalDevice_T_camera);

				// Verify camera survived the round-trip
				const SharedAnyCamera deserializedCamera = readSampleFrame.camera();
				OCEAN_EXPECT_TRUE(validation, deserializedCamera != nullptr);

				if (deserializedCamera != nullptr)
				{
					OCEAN_EXPECT_EQUAL(validation, deserializedCamera->width(), width);
					OCEAN_EXPECT_EQUAL(validation, deserializedCamera->height(), height);
					OCEAN_EXPECT_EQUAL(validation, deserializedCamera->name(), originalCamera->name());
				}

				const Frame deserializedFrame = readSampleFrame.frame();
				OCEAN_EXPECT_TRUE(validation, deserializedFrame.isValid());

				if (deserializedFrame.isValid())
				{
					OCEAN_EXPECT_EQUAL(validation, deserializedFrame.frameType(), frame.frameType());
					OCEAN_EXPECT_EQUAL(validation, deserializedFrame.timestamp(), frame.timestamp());
					OCEAN_EXPECT_TRUE(validation, isFrameContentEqual(frame, deserializedFrame));
				}
			}
		}

		// Test SampleFrame without camera (should have empty cameraModel_)
		IO::Serialization::MediaSerializer::DataSampleFrame sampleFrameWithoutCamera(frame, "ocn");
		sampleFrameWithoutCamera.configurePlaybackTimestamp(startTimestamp);

		OCEAN_EXPECT_TRUE(validation, sampleFrameWithoutCamera.isValid());
		OCEAN_EXPECT_TRUE(validation, sampleFrameWithoutCamera.cameraModel().empty());
		OCEAN_EXPECT_TRUE(validation, sampleFrameWithoutCamera.camera() == nullptr);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

Frame TestMediaSerializer::randomFrame(RandomGenerator& randomGenerator)
{
	const FrameType::PixelFormats& pixelFormats = FrameType::definedPixelFormats();

	const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);

	const unsigned int widthMultiple = FrameType::widthMultiple(pixelFormat);
	const unsigned int heightMultiple = FrameType::heightMultiple(pixelFormat);

	const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u) * widthMultiple;
	const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u) * heightMultiple;

	const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

	Frame frame(FrameType(width, height, pixelFormat, pixelOrigin));

	// let's create random image content

	for (unsigned int planeIndex = 0u; planeIndex < frame.numberPlanes(); ++planeIndex)
	{
		const unsigned int planeWidthBytes = frame.planeWidthBytes(planeIndex);

		for (unsigned int y = 0u; y < frame.planeHeight(planeIndex); ++y)
		{
			uint8_t* const planeRow = (uint8_t*)(frame.row<void>(y, planeIndex));

			for (unsigned int x = 0u; x < planeWidthBytes; ++x)
			{
				planeRow[x] = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
			}
		}
	}

	const Timestamp frameTimestamp(RandomD::scalar(randomGenerator, -1000.0, 1000.0));
	frame.setTimestamp(frameTimestamp);

	return frame;
}

bool TestMediaSerializer::isFrameContentEqual(const Frame& frameA, const Frame& frameB)
{
	ocean_assert(frameA.isValid());
	ocean_assert(frameB.isValid());

	if (!frameA.isValid() || !frameB.isValid())
	{
		return false;
	}

	if (frameA.frameType() != frameB.frameType())
	{
		return false;
	}

	if (frameA.timestamp() != frameB.timestamp())
	{
		return false;
	}

	for (unsigned int planeIndex = 0u; planeIndex < frameA.numberPlanes(); ++planeIndex)
	{
		const unsigned int planeWidthBytes = frameA.planeWidthBytes(planeIndex);

		for (unsigned int y = 0u; y < frameA.planeHeight(planeIndex); ++y)
		{
			const void* const planeRowA = frameA.constrow<void>(y, planeIndex);
			const void* const planeRowB = frameB.constrow<void>(y, planeIndex);

			if (memcmp(planeRowA, planeRowB, planeWidthBytes) != 0)
			{
				return false;
			}
		}
	}

	return true;
}

}

}

}

}
