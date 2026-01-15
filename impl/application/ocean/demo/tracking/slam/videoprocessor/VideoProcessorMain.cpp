/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/FrameTransposer.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/File.h"

#include "ocean/media/ImageSequence.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Movie.h"
#include "ocean/media/MovieRecorder.h"

#include "ocean/tracking/Utilities.h"

#include "ocean/tracking/slam/TrackerMono.h"
#include "ocean/tracking/slam/Utilities.h"

#if defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS)
	#include "ocean/media/avfoundation/AVFoundation.h"
	#include "ocean/media/imageio/ImageIO.h"
	#include "ocean/platform/apple/System.h"
#endif

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	#include "ocean/media/wic/WIC.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
#endif

using namespace Ocean;
using namespace Ocean::Tracking::SLAM;

/**
 * Just a helper class to ensure that all media plugins are unregistered when this object is disposed.
 */
class ScopedPlugin
{
	public:

		/**
		 * Creates a new object and registers all plugins.
		 */
		inline ScopedPlugin();

		/**
		 * Destructs this object and unregisters all plugins.
		 */
		inline ~ScopedPlugin();
};

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int argc, wchar_t* argv[])
#elif defined(__APPLE__) || defined(__linux__)
	// main function on OSX platforms
	int main(int argc, char* argv[])
#else
	#error Missing implementation.
#endif
{
#ifdef OCEAN_COMPILER_MSC
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	const ScopedPlugin scopedPlugin;

	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	RandomI::initialize();

	CommandArguments commandArguments;
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as input parameter");
	commandArguments.registerParameter("input", "i", "The file to be used as input");
	commandArguments.registerParameter("calibration", "c", "The file containing the camera calibration for the input, otherwise a default calibration path will be used");
	commandArguments.registerParameter("downsample", "d", "If specified, the input will be downsampled by a factor of 2");
	commandArguments.registerParameter("rotateOutput", "ro", "Optional allowing to rotate the output clockwise, in 90 degree steps");
	commandArguments.registerParameter("stopMotionInterval", "smi", "Optional interval in seconds between individual stop-motion frames, otherwise the replay will be in real-time");
	commandArguments.registerParameter("darkeningFactor", "df", "Optional darkening factor for the output, with range [0, infinity), 0 means no darkening");
	commandArguments.registerParameter("renderPointTracks", "rpt", "If specified, the point tracks will be rendered");
	commandArguments.registerParameter("renderCoordinateSystems", "rcs", "If specified, the coordinate systems will be rendered");
	commandArguments.registerParameter("help", "h", "Showing this help");

	if (!commandArguments.parse(argv, argc))
	{
		Log::warning() << "Failure when parsing the command arguments";
	}

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		return 1;
	}

	std::string inputValue;
	if (!commandArguments.hasValue("input", inputValue, false, 0u) || inputValue.empty())
	{
		Log::error() << "No input defined";
		return 1;
	}

	const IO::File inputFile(inputValue);

	if (!inputFile.exists())
	{
		Log::error() << "The provided input file '" << inputFile() << "' does not exist";
		return 1;
	}

	Media::FrameMediumRef frameMedium;

	if (inputFile.extension() == "jpg" || inputFile.extension() == "png")
	{
		frameMedium = Media::Manager::get().newMedium(inputFile(), Media::Medium::IMAGE_SEQUENCE);

		if (Media::ImageSequenceRef imageSequence = frameMedium)
		{
			imageSequence->setMode(Media::ImageSequence::SM_EXPLICIT);
		}
	}

	if (!frameMedium)
	{
		frameMedium = Media::Manager::get().newMedium(inputFile(), Media::Medium::MOVIE);

		if (Media::MovieRef movie = frameMedium)
		{
			movie->setSpeed(0.0f);
		}
	}

	if (!frameMedium)
	{
		Log::error() << "Failed to load input file '" << inputFile() << "'";
		return 1;
	}

	IO::File calibrationFile;

	const std::string calibration = commandArguments.value("calibration", std::string(), false);

	if (!calibration.empty())
	{
		calibrationFile = IO::File(calibration);
	}
	else
	{
	   calibrationFile = IO::File(inputFile.base() + "_calibration.json");
	}

	if (!calibrationFile.exists())
	{
		Log::error() << "No calibration file found at " << calibrationFile();
		return 1;
	}

	const SharedAnyCamera camera = IO::CameraCalibrationManager::get().parseCamera(calibrationFile());

	if (!camera)
	{
		Log::error() << "Failed to parse camera calibration file '" << calibrationFile() << "'";
		return 1;
	}

	Media::MovieRecorderRef movieRecorder = Media::Manager::get().newRecorder(Media::Recorder::MOVIE_RECORDER);

	if (!movieRecorder)
	{
		Log::error() << "Failed to create movie recorder";
		return 1;
	}

	double outputFrameRate = 30.0;

	if (Media::MovieRef movie = frameMedium)
	{
		double frameRate = movie->frameFrequency();

		if (frameRate > 0.0)
		{
			outputFrameRate = frameRate;
		}
	}

	const IO::File outputFile(inputFile.base() + "_output.mp4");

	if (outputFile.exists())
	{
		outputFile.remove();
	}

	movieRecorder->setFilename(outputFile());
	movieRecorder->setFrameFrequency(outputFrameRate);

	Media::FrameMedium::FrameReceiver frameReceiver;
	Media::FrameMedium::FrameCallbackScopedSubscription subscription = frameMedium->addFrameCallback(std::bind(&Media::FrameMedium::FrameReceiver::onFrame, &frameReceiver, std::placeholders::_1, std::placeholders::_2));

	frameMedium->start();

	OCEAN_APPLY_IF_APPLE(Platform::Apple::System::runMainLoop(0.0)); // necessary for AVFoundation in CLI apps

	const bool downsample = commandArguments.hasValue("downsample");
	const int rotateClockwise = commandArguments.value<int>("rotateOutput", 0, false);
	const double stopMotionInterval = commandArguments.value<double>("stopMotionInterval", 0.0, false);

	const int darkeningFactor = commandArguments.value<int>("darkeningFactor", 0, false);
	const bool renderPointTracks = commandArguments.hasValue("renderPointTracks");
	const bool renderCoordinateSystems = commandArguments.hasValue("renderCoordinateSystems");

	Frame frame;
	Frame yFrame;
	AnyCameraClipper cameraClipper;

	TrackerMono trackerMono;

	unsigned int frameIndex = 0u;

	Tracking::SLAM::TrackerMono::DebugData debugData;

	while (frameReceiver.latestFrameAndReset(frame, nullptr, 1000u))
	{
		ocean_assert(frame.isValid());

		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::formatGrayscalePixelFormat(frame.pixelFormat()), FrameType::ORIGIN_UPPER_LEFT, yFrame))
		{
			std::cerr << "This should never happen!" << std::endl;
			return 1;
		}

		SharedAnyCamera yFrameCamera = camera;

		if (downsample)
		{
			CV::FrameShrinker::downsampleByTwo11(yFrame);
			yFrameCamera = camera->clone(yFrame.width(), yFrame.height());
		}

		Frame outputFrame;
		if (!CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, outputFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return 1;
		}

		const Timestamp iterationTimestamp(true);

		const Vector3 cameraGravity = Vector3(0, 0, 0);
		const Quaternion anyWorld_Q_camera = Quaternion(false);

		HomogenousMatrix4 world_T_camera(false);
		TrackerMono::DebugData frameDebugData;
		const bool handleResult = trackerMono.handleFrame(*yFrameCamera, std::move(yFrame), world_T_camera, cameraGravity, anyWorld_Q_camera, &frameDebugData);

		ocean_assert_and_suppress_unused(handleResult, handleResult);

		cameraClipper.update(yFrameCamera);

		debugData.update(std::move(frameDebugData));

		{
			const unsigned int darkeningValue = std::max(0, darkeningFactor);

			const size_t maximalTrackLength = renderPointTracks ? 3 : 0;

			Tracking::SLAM::Utilities::paintDebugDataByPosePrecision(outputFrame, cameraClipper, world_T_camera, debugData, maximalTrackLength, darkeningValue);
		}

		if (renderCoordinateSystems)
		{
			if (world_T_camera.isValid())
			{
				for (const Vector3& position : {Vector3(0, 0, 0), Vector3(0, 0, -1), Vector3(-1, 0, -1), Vector3(1, 0, -1)})
				{
					Tracking::Utilities::paintCoordinateSystemIF(outputFrame, cameraClipper, Camera::standard2InvertedFlipped(world_T_camera), HomogenousMatrix4(position), Scalar(0.25));
				}
			}
		}

		if (rotateClockwise != 0)
		{
			CV::FrameTransposer::Comfort::rotate(outputFrame, rotateClockwise);
		}

		CV::Canvas::drawText(outputFrame, String::toAString(frameIndex++), 5, 5, CV::Canvas::white(), CV::Canvas::black());

		CV::Canvas::drawText(outputFrame, "Good: " + String::toAString(debugData.posePreciseObjectPointIds_.size()), 5, outputFrame.height() - 45, CV::Canvas::white(), CV::Canvas::black());
		CV::Canvas::drawText(outputFrame, " Bad: " + String::toAString(debugData.poseNotPreciseObjectPointIds_.size()), 5, outputFrame.height() - 25, CV::Canvas::white(), CV::Canvas::black());

		if (!movieRecorder->isRecording())
		{
			movieRecorder->setPreferredFrameType(outputFrame.frameType());
			movieRecorder->start();
		}

		Frame recorderFrame;
		if (movieRecorder->lockBufferToFill(recorderFrame, false))
		{
			if (!CV::FrameConverter::Comfort::convertAndCopy(outputFrame, recorderFrame))
			{
				Log::error() << "Failed to create recorder frame";
			}

			movieRecorder->unlockBufferToFill();
		}
		else
		{
			Log::error() << "Failed to lock recorder frame";
		}

		OCEAN_APPLY_IF_APPLE(Platform::Apple::System::runMainLoop(0.0));

		if (stopMotionInterval > 0.0)
		{
			const double waitTime = stopMotionInterval - double(Timestamp(true) - iterationTimestamp);

			if (waitTime > 0.0)
			{
				const unsigned int sleepTimeMs = (unsigned int)(double(1000) * waitTime + 0.5);

				Thread::sleep(sleepTimeMs);
			}
		}
	}

	movieRecorder->stop();

	Log::debug() << "Processed " << trackerMono.frameIndex() + 1u << " frames";
	Log::info() << " ";
	Log::info() << trackerMono.performance();
	Log::info() << " ";
	Log::info() << "Output: " << outputFile();

	return 0;
}

inline ScopedPlugin::ScopedPlugin()
{
#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::registerMediaFoundationLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::registerAVFLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());
#endif // OCEAN_RUNTIME_STATIC
}

inline ScopedPlugin::~ScopedPlugin()
{
#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::unregisterAVFLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::unregisterMediaFoundationLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
#endif // OCEAN_RUNTIME_STATIC
}
