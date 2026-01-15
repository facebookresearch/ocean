/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/slam/replay/ReplayMain.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/FrameTransposer.h"

#include "ocean/devices/Manager.h"

#include "ocean/devices/serialization/SerializerDevicePlayer.h"

#include "ocean/io/File.h"

#include "ocean/media/Manager.h"
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
	commandArguments.registerParameter("downsample", "d", "If specified, the input will be downsampled by a factor of 2");
	commandArguments.registerParameter("rotateOutput", "ro", "Optional allowing to rotate the output clockwise, in 90 degree steps");
	commandArguments.registerParameter("stopMotionInterval", "smi", "Optional interval in seconds between individual stop-motion frames, otherwise the replay will be in real-time");
	commandArguments.registerParameter("darkeningFactor", "df", "Optional darkening factor for the output, with range [0, infinity), 0 means no darkening");
	commandArguments.registerParameter("renderPointTracks", "rpt", "If specified, the point tracks will be rendered");
	commandArguments.registerParameter("renderGravityVectors", "rgv", "If specified, the gravity vectors will be rendered");
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

	if (!inputFile.exists() || inputFile.extension() != "osn")
	{
		Log::error() << "The provided input file '" << inputFile() << "' does not exist or does not have the correct extension";
		return 1;
	}

	Devices::Serialization::SerializerDevicePlayer devicePlayer;

	if (!devicePlayer.initialize(inputFile()))
	{
		Log::error() << "Failed to initialize device player";
		return 1;
	}

	if (devicePlayer.frameMediums().empty())
	{
		Log::error() << "The device player does not contain any frame mediums";
		return 1;
	}

	Media::MovieRecorderRef movieRecorder = Media::Manager::get().newRecorder(Media::Recorder::MOVIE_RECORDER);

	if (!movieRecorder)
	{
		Log::error() << "Failed to create movie recorder";
		return 1;
	}

	constexpr double outputFrameRate = 30.0;

	const IO::File outputFile(inputFile.base() + "_output.mp4");

	if (outputFile.exists())
	{
		outputFile.remove();
	}

	movieRecorder->setFilename(outputFile());
	movieRecorder->setFrameFrequency(outputFrameRate);
	movieRecorder->setPreferredBitrate(10u * 1000u * 1000u); // 10 MBit/s

	OCEAN_APPLY_IF_APPLE(Platform::Apple::System::runMainLoop(0.0)); // necessary for AVFoundation in CLI apps

	const int rotateClockwise = commandArguments.value<int>("rotateOutput", 0, false);

	const bool downsample = commandArguments.hasValue("downsample");

	const double stopMotionInterval = commandArguments.value<double>("stopMotionInterval", 0.0, false);

	const int darkeningFactor = commandArguments.value<int>("darkeningFactor", 0, false);
	const bool renderPointTracks = commandArguments.hasValue("renderPointTracks");
	const bool renderGravityVectors = commandArguments.hasValue("renderGravityVectors");
	const bool renderCoordinateSystems = commandArguments.hasValue("renderCoordinateSystems");

	Frame yFrame;

	SensorAccessor sensorAccessor;

	TrackerMono trackerMono;

	if (stopMotionInterval <= 0.0)
	{
		if (!devicePlayer.start(1.0f))
		{
			Log::error() << "Failed to start device player";
			return 1;
		}
	}
	else
	{
		if (!devicePlayer.start(0.0f))
		{
			Log::error() << "Failed to start device player";
			return 1;
		}

		constexpr double stopMotionTolerance = 0.005; // 5ms

		devicePlayer.setStopMotionTolerance(IO::Serialization::DataTimestamp(stopMotionTolerance));
	}

	Media::FrameMediumRef frameMedium = devicePlayer.frameMediums().front();

	AnyCameraClipper cameraClipper;

	Timestamp frameTimestamp(false);
	Timestamp iterationTimestamp(false);

	unsigned int frameIndex = 0u;

	Tracking::SLAM::TrackerMono::DebugData debugData;

	while (devicePlayer.isPlaying())
	{
		SharedAnyCamera camera;
		FrameRef frame;

		if (stopMotionInterval > 0.0)
		{
			frameTimestamp = devicePlayer.playNextFrame();

			if (frameTimestamp.isInvalid())
			{
				// we have reached the end
				break;
			}

			frame = frameMedium->frame(frameTimestamp, &camera);
			ocean_assert(frame && frame->timestamp() == frameTimestamp);

			iterationTimestamp.toNow();
		}
		else
		{
			frame = frameMedium->frame(&camera);

			if (!frame || frame->timestamp() == frameTimestamp)
			{
				Thread::sleep(1u);
				continue;
			}

			// let's sleep for 5ms to ensure that we have received all necessary device samples
			Thread::sleep(5u);
		}

		if (!camera)
		{
			Log::error() << "No camera for frame medium";
			return 1;
		}

		frameTimestamp = frame->timestamp();

		if (!CV::FrameConverter::Comfort::convert(*frame, FrameType::formatGrayscalePixelFormat(frame->pixelFormat()), FrameType::ORIGIN_UPPER_LEFT, yFrame))
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

		const Quaternion device_Q_camera(frameMedium->device_T_camera().rotation());

		HomogenousMatrix4 world_T_camera(false);

		const Vector3 cameraGravity = sensorAccessor.cameraGravity(device_Q_camera, frameTimestamp);
		const Quaternion anyWorld_Q_camera = sensorAccessor.anyWorld_Q_camera(device_Q_camera, frameTimestamp);

		Tracking::SLAM::TrackerMono::DebugData frameDebugData;

		const bool handleResult = trackerMono.handleFrame(*yFrameCamera, std::move(yFrame), world_T_camera, cameraGravity, anyWorld_Q_camera, &frameDebugData);
		ocean_assert_and_suppress_unused(handleResult, handleResult);

		cameraClipper.update(yFrameCamera);

		debugData.update(std::move(frameDebugData));

		{
			const unsigned int darkeningValue = std::max(0, darkeningFactor);

			const size_t maximalTrackLength = renderPointTracks ? 3 : 0;

			Tracking::SLAM::Utilities::paintDebugDataByPosePrecision(outputFrame, cameraClipper, world_T_camera, debugData, maximalTrackLength, darkeningValue);
		}

		if (renderGravityVectors)
		{
			if (!cameraGravity.isNull())
			{
				for (const Scalar value : {Scalar(-0.5), Scalar(0), Scalar(0.5)})
				{
					const Vector3 position = rotateClockwise == 0 ? Vector3(value, 0, -1) : Vector3(0, value, -1);

					Tracking::Utilities::paintGravity(*yFrameCamera, outputFrame, cameraGravity, 1u, CV::Canvas::blue(), 20u, position);
				}
			}
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

		if (iterationTimestamp.isValid())
		{
			ocean_assert(stopMotionInterval > 0.0);
			const double waitTime = stopMotionInterval - double(Timestamp(true) - iterationTimestamp);

			if (waitTime > 0.0)
			{
				Thread::sleep((unsigned int)(double(1000) * waitTime + 0.5));
			}
		}
	}

	movieRecorder->stop();

	sensorAccessor.release();

	frameMedium.release();

	devicePlayer.release();

	Log::debug() << "Processed " << trackerMono.frameIndex() + 1u << " frames";
	Log::info() << " ";
	Log::info() << trackerMono.performance();
	Log::info() << " ";
	Log::info() << "Output: " << outputFile();

	return 0;
}

ScopedPlugin::ScopedPlugin()
{
#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::registerMediaFoundationLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::registerAVFLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());

	Devices::Serialization::registerSerializationLibrary();
#endif // OCEAN_RUNTIME_STATIC
}

ScopedPlugin::~ScopedPlugin()
{
#ifdef OCEAN_RUNTIME_STATIC
	Devices::Serialization::unregisterSerializationLibrary();

	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::unregisterAVFLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::unregisterMediaFoundationLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
#endif // OCEAN_RUNTIME_STATIC
}

Vector3 SensorAccessor::cameraGravity(const Quaternion& device_Q_camera, const Timestamp& timestamp)
{
	if (!device_Q_camera.isValid())
	{
		return Vector3(0, 0, 0);
	}

	if (gravityTracker_.isNull())
	{
		gravityTracker_ = Devices::Manager::get().device(Devices::GravityTracker3DOF::deviceTypeGravityTracker3DOF());

		if (gravityTracker_)
		{
			gravityTracker_->start();
		}
	}

	if (gravityTracker_)
	{
		const Devices::GravityTracker3DOF::GravityTracker3DOFSampleRef sample = gravityTracker_->sample(timestamp, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

		if (sample)
		{
			ocean_assert(sample->gravities().size() == 1);
			ocean_assert(sample->referenceSystem() == Devices::Tracker::RS_OBJECT_IN_DEVICE);

			constexpr double epsilon = 0.0001; // 0.1 ms

			const double timestampDifference = double(sample->timestamp()) - double(timestamp);

			if (NumericD::isNotEqual(timestampDifference, 0.0, epsilon))
			{
				if (timestampDifference >= 0.0)
				{
					Log::warning() << "Sample timestamp for camera gravity does not fit to request, the sample's timestamp is in the future by " << String::toAString(timestampDifference * 1000.0, 5u) << "ms";
				}
				else
				{
					Log::warning() << "Sample timestamp for camera gravity does not fit to request, the sample's timestamp is in the past by " << String::toAString(-timestampDifference * 1000.0, 5u) << "ms";
				}
			}

			const Vector3& deviceGravity = sample->gravities().front();

			const Quaternion camera_Q_device = device_Q_camera.inverted();

			return camera_Q_device * deviceGravity;
		}
	}

	return Vector3(0, 0, 0);
}

Quaternion SensorAccessor::anyWorld_Q_camera(const Quaternion& device_Q_camera, const Timestamp& timestamp)
{
	if (!device_Q_camera.isValid())
	{
		return Quaternion(false);
	}

	if (orientationTracker_.isNull())
	{
		orientationTracker_ = Devices::Manager::get().device(Devices::OrientationTracker3DOF::deviceTypeOrientationTracker3DOF());

		if (orientationTracker_)
		{
			orientationTracker_->start();
		}
	}

	if (orientationTracker_)
	{
		const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef sample = orientationTracker_->sample(timestamp, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

		if (sample)
		{
			ocean_assert(sample->orientations().size() == 1);
			ocean_assert(sample->referenceSystem() == Devices::Tracker::RS_DEVICE_IN_OBJECT);

			constexpr double epsilon = 0.0001; // 0.1 ms

			const double timestampDifference = double(sample->timestamp()) - double(timestamp);

			if (NumericD::isNotEqual(timestampDifference, 0.0, epsilon))
			{
				if (timestampDifference >= 0.0)
				{
					Log::warning() << "Sample timestamp for anyWorld_Q_camera does not fit to request, the sample's timestamp is in the future by " << String::toAString(timestampDifference * 1000.0, 5u) << "ms";
				}
				else
				{
					Log::warning() << "Sample timestamp for anyWorld_Q_camera does not fit to request, the sample's timestamp is in the past by " << String::toAString(-timestampDifference * 1000.0, 5u) << "ms";
				}
			}

			const Quaternion anyWorld_Q_device = sample->orientations().front();

			return anyWorld_Q_device * device_Q_camera;
		}
	}

	return Quaternion(false);
}

void SensorAccessor::release()
{
	gravityTracker_.release();
	orientationTracker_.release();
}
