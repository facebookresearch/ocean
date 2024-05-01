// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testmedia/TestVRS.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/media/ImageSequence.h"
#include "ocean/media/Manager.h"

#ifndef OCEAN_DO_NOT_TEST_VRS
	#include "metaonly/ocean/math/AnyCameraPerception.h"

	#include "metaonly/ocean/media/vrs/VRS.h"

	#include "metaonly/ocean/io/vrs/FlexibleRecorder.h"

	#include <perception/camera/CameraModelFactory.h>
	#include <perception/camera/CameraModelInterface.h>
#endif

namespace Ocean
{

namespace Test
{

namespace TestMedia
{

#ifdef OCEAN_DO_NOT_TEST_VRS

bool TestVRS::test(const double /*testDuration*/)
{
	Log::info() << "Skipping VRS test.";

	return true;
}

#else // OCEAN_DO_NOT_TEST_VRS

bool TestVRS::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

#ifdef OCEAN_RUNTIME_STATIC
	Media::VRS::registerVRSLibrary();
#endif

	Log::info() << "VRS test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testFlexibleRecorder(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Entire VRS test succeeded.";
	}
	else
	{
		Log::info() << "VRS test FAILED!";
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::VRS::unregisterVRSLibrary();
#endif

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

/**
 * This class implements a simple instance for the GTest ensuring that
 * the VRS plugin is registered during startup and unregistered before shutdown.
 */
class TestVRSGTestInstance : public ::testing::Test
{
	protected:

		/**
		 * Default constructor.
		 */
  		TestVRSGTestInstance()
  		{
  			// nothing to do here
		}

		/**
		 * Sets up the test.
		 */
		void SetUp() override
		{
#ifdef OCEAN_RUNTIME_STATIC
			Media::VRS::registerVRSLibrary();
#endif
		}

		/**
		 * Tears down the test.
		 */
		void TearDown() override
		{
#ifdef OCEAN_RUNTIME_STATIC
			Media::VRS::unregisterVRSLibrary();
#endif
		}
};

TEST_F(TestVRSGTestInstance, FlexibleRecorder)
{
	EXPECT_TRUE(TestVRS::testFlexibleRecorder(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestVRS::testFlexibleRecorder(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Flexible recorder test:";

	bool allSucceeded = true;

	constexpr unsigned int images = 50u;

	const FrameType::PixelFormats pixelFormats = {FrameType::FORMAT_Y8, FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32};
	const std::vector<unsigned int> widths = {640u, 1280u, 1920u};
	const std::vector<unsigned int> heights = {480u, 720u, 1080u};

	const IO::Directory directory = IO::Directory::createTemporaryDirectory();

	if (!directory.isValid())
	{
		Log::info() << "Failed to create temporary directory.";
		return false;
	}

	RandomGenerator randomGenerator;

	const IO::ScopedFile scopedFile(directory + IO::File("file.vrs"));
	ocean_assert(!scopedFile.exists());

	IO::VRS::FlexibleRecorder flexibleRecorder;

	// we randomly change the order of starting the recording and adding a recordable

	unsigned int recordableIndex = IO::VRS::FlexibleRecorder::invalidIndex();

	if (RandomI::random(randomGenerator, 1u) == 0u)
	{
		// we use a custom index

		recordableIndex = RandomI::random(randomGenerator, 255u);
	}

	std::vector<std::string> orderOfOperations = {"add_recordable", "start_recorder"};

	if (RandomI::random(randomGenerator, 1u) == 0u)
	{
		std::swap(orderOfOperations[0], orderOfOperations[1]);
	}

	for (const std::string& operation : orderOfOperations)
	{
		if (operation == "add_recordable")
		{
			if (recordableIndex == IO::VRS::FlexibleRecorder::invalidIndex())
			{
				recordableIndex = flexibleRecorder.addRecordable<IO::VRS::RecordableCamera>();
			}
			else
			{
				if (!flexibleRecorder.addRecordable<IO::VRS::RecordableCamera>(recordableIndex))
				{
					allSucceeded = false;
				}
			}
		}
		else
		{
			ocean_assert(operation == "start_recorder");

			if (!flexibleRecorder.start(scopedFile()))
			{
				Log::info() << "Failed to start recorder.";
				return false;
			}
		}
	}

	// now we write several random images to the recording, afterwards we check whether we are able to extract the same information again

	Frames frames;
	SharedAnyCamerasD anyCameras;

	FrameType lastFrameType;
	unsigned int lastPaddingElements = 0u;

	SharedAnyCameraD lastAnyCamera;

	Timestamp timestamp(RandomD::scalar(randomGenerator, 1.0, 1000.0));

	for (unsigned int n = 0u; n < images; ++n)
	{
		if (!lastFrameType.isValid() || RandomI::random(randomGenerator, 10u) == 0u)
		{
			lastFrameType = FrameType(RandomI::random(randomGenerator, widths), RandomI::random(randomGenerator, heights), RandomI::random(randomGenerator, pixelFormats), FrameType::ORIGIN_UPPER_LEFT);

			lastPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		}

		Frame frame(lastFrameType, lastPaddingElements, timestamp);
		CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);

		if (!lastAnyCamera || lastAnyCamera->width() != frame.width() || lastAnyCamera->height() != frame.height() || RandomI::random(randomGenerator, 10u) == 0u)
		{
			const unsigned int randomCameraIndex = RandomI::random(randomGenerator, 1u);

			if (randomCameraIndex == 0u)
			{
				// we create a pinhole camera

				const double focalX = RandomD::scalar(randomGenerator, 100.0, double(frame.width() * 2u));
				const double focalY = RandomD::scalar(randomGenerator, 100.0, double(frame.height() * 2u));

				const double principalPointX = RandomD::scalar(randomGenerator, 0.0, double(frame.width()));
				const double principalPointY = RandomD::scalar(randomGenerator, 0.0, double(frame.height()));

				const PinholeCameraD::DistortionPair radialDistortion(Random::scalar(randomGenerator, -0.1, 0.1), Random::scalar(randomGenerator, -0.1, 0.1));
				const PinholeCameraD::DistortionPair tangentialDistortion(Random::scalar(randomGenerator, -0.1, 0.1), Random::scalar(randomGenerator, -0.1, 0.1));

				lastAnyCamera = std::make_shared<AnyCameraPinholeD>(PinholeCameraD(frame.width(), frame.height(), focalX, focalY, principalPointX, principalPointY, radialDistortion, tangentialDistortion));
			}
			else if (randomCameraIndex == 1u)
			{
				// we create a perception fisheye camera

				std::vector<double> parameters =
				{
					RandomD::scalar(randomGenerator, 100.0, double(frame.width() * 2u)), // focal length
					RandomD::scalar(randomGenerator, 0.0, double(frame.width())), // principal point
					RandomD::scalar(randomGenerator, 0.0, double(frame.height()))
				};

				for (size_t i = 0; i < 8; ++i) // radial & tangential distortation
				{
					parameters.emplace_back(RandomD::scalar(randomGenerator, -0.1, 1.0));
				}

				ocean_assert(parameters.size() == 11);

				lastAnyCamera = std::make_shared<AnyCameraFisheyeD>(FisheyeCameraD(frame.width(), frame.height(), FisheyeCameraD::PC_11_PARAMETERS_ONE_FOCAL_LENGTH, parameters.data()));
			}
#if 0 // enable once we can read Perception camera models
			else
			{
				ocean_assert(randomCameraIndex == 2u);

				// we create a perception pinhole camera

				const double focalX = RandomD::scalar(randomGenerator, 100.0, double(frame.width() * 2u));
				const double focalY = RandomD::scalar(randomGenerator, 100.0, double(frame.height() * 2u));

				const double principalPointX = RandomD::scalar(randomGenerator, 0.0, double(frame.width()));
				const double principalPointY = RandomD::scalar(randomGenerator, 0.0, double(frame.height()));

				const double radialDistortion0 = RandomD::scalar(randomGenerator, -0.1, 0.1);
				const double radialDistortion1 = RandomD::scalar(randomGenerator, -0.1, 0.1);

				const double tangentialDistortion0 = RandomD::scalar(randomGenerator, -0.1, 0.1);
				const double tangentialDistortion1 = RandomD::scalar(randomGenerator, -0.1, 0.1);

				const std::vector<double> intrinsics = {focalX, focalY, principalPointX, principalPointY, radialDistortion0, radialDistortion1, tangentialDistortion0, tangentialDistortion1};
				std::unique_ptr<perception::CameraModelInterface<double>> cameraModelInterface = perception::createModel(perception::CameraModelType::RADTAN, perception::ImageSize(frame.width(), frame.height()), intrinsics, false);

				lastAnyCamera = std::make_shared<AnyCameraPerceptionD>(std::move(cameraModelInterface));
			}
#endif
		}

		if (!flexibleRecorder.recordable<IO::VRS::RecordableCamera>(recordableIndex).addData(uint32_t(recordableIndex), lastAnyCamera, frame, HomogenousMatrixD4(false), timestamp, timestamp))
		{
			allSucceeded = false;
		}

		frames.emplace_back(std::move(frame));
		anyCameras.push_back(lastAnyCamera);

		timestamp = timestamp + RandomD::scalar(randomGenerator, 1.0, 1000.0);
	}

	if (!flexibleRecorder.stop())
	{
		Log::info() << "Failed to stop recording";
		allSucceeded = false;
	}

	ocean_assert(frames.size() == anyCameras.size());

	Media::ImageSequenceRef imageSequence = Media::Manager::get().newMedium(scopedFile(), "VRS", Media::Medium::IMAGE_SEQUENCE);

	if (imageSequence.isNull())
	{
		Log::info() << "Failed to load VRS file";
		return false;
	}

	imageSequence->setMode(Media::ImageSequence::SM_EXPLICIT);
	imageSequence->start();

	const unsigned int imageSequenceFrames = imageSequence->images();

	if (imageSequenceFrames != (unsigned int)(frames.size()))
	{
		Log::info() << "The recording contains " << frames.size() << " frames, but we extract " << imageSequenceFrames;
		return false;
	}

	for (unsigned int frameIndex = 0u; frameIndex < imageSequenceFrames; ++frameIndex)
	{
		SharedAnyCamera anyCamera;
		const FrameRef frameRef = imageSequence->frame(&anyCamera);

		if (frameRef && *frameRef && anyCamera)
		{
			ocean_assert(anyCamera->isValid());

			const Frame& frame = *frameRef;

			if (frame.frameType() != frames[frameIndex].frameType())
			{
				Log::info() << "Frame type of frame " << frameIndex << " is wrong";
				allSucceeded = false;

				break;
			}

			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				if (memcmp(frame.constrow<void>(y), frames[frameIndex].constrow<void>(y), frame.planeWidthBytes(0u)) != 0)
				{
					Log::info() << "Frame memory of frame " << frameIndex << " is wrong";
					allSucceeded = false;

					break;
				}
			}

			if (frame.timestamp() != frames[frameIndex].timestamp())
			{
				Log::info() << "Frame timestamp of frame " << frameIndex << " is wrong";
				allSucceeded = false;

				break;
			}

			if (!anyCameras[frameIndex]->isEqual(*(anyCamera->cloneToDouble()), double(NumericF::weakEps())))
			{
				Log::info() << "Camera profile of frame " << frameIndex << " is wrong";
				allSucceeded = false;
			}

			if (!allSucceeded)
			{
				break;
			}
		}
		else
		{
			Log::info() << "Failed to extract frame " << frameIndex;
			allSucceeded = false;

			break;
		}

		imageSequence->forceNextFrame();
	}

	if (allSucceeded)
	{
		Log::info() << "Flexible recorder test succeeded.";
	}
	else
	{
		Log::info() << "Flexible recorder test FAILED!";
	}

	return allSucceeded;
}

#endif // OCEAN_DO_NOT_TEST_VRS

}

}

}
