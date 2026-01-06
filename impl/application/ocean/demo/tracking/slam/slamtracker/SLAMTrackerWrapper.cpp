/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/slam/slamtracker/SLAMTrackerWrapper.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/DateTime.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/File.h"

#include "ocean/media/ImageSequence.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Movie.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/Utilities.h"

#include "ocean/tracking/Utilities.h"

#include "ocean/tracking/slam/Utilities.h"

#include "ocean/devices/Manager.h"

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	#include "ocean/media/directshow/DirectShow.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
	#include "ocean/media/wic/WIC.h"
#elif defined(OCEAN_PLATFORM_BUILD_APPLE)
	#include "ocean/media/avfoundation/AVFoundation.h"
	#include "ocean/media/imageio/ImageIO.h"
	#include "ocean/platform/apple/Utilities.h"
	#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS)
		#include "ocean/devices/ios/IOS.h"
	#endif
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
	#include "ocean/media/android/Android.h"
	#include "ocean/devices/android/Android.h"
	#include "ocean/platform/android/ResourceManager.h"
#endif

// #define DISABLE_TRACKING

Vector3 SLAMTrackerWrapper::SensorAccessor::cameraGravity(const Quaternion& device_Q_camera, const Timestamp& timestamp)
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

Quaternion SLAMTrackerWrapper::SensorAccessor::anyWorld_Q_camera(const Quaternion& device_Q_camera, const Timestamp& timestamp)
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

void SLAMTrackerWrapper::SensorAccessor::release()
{
	gravityTracker_.release();
	orientationTracker_.release();
}

SLAMTrackerWrapper::SLAMTrackerWrapper(SLAMTrackerWrapper&& slamTrackerWrapper) noexcept
{
	*this = std::move(slamTrackerWrapper);
}

SLAMTrackerWrapper::SLAMTrackerWrapper(const std::vector<std::wstring>& arguments)
{
#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	Media::DirectShow::registerDirectShowLibrary();
	Media::MediaFoundation::registerMediaFoundationLibrary();
	Media::WIC::registerWICLibrary();
#elif defined(OCEAN_PLATFORM_BUILD_APPLE)
	Media::AVFoundation::registerAVFLibrary();
	Media::ImageIO::registerImageIOLibrary();
	#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS)
		Devices::IOS::registerIOSLibrary();
	#endif
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
	Media::Android::registerAndroidLibrary();
	Devices::Android::registerAndroidLibrary();
#endif

	CommandArguments commandArguments;
	commandArguments.registerParameter("input", "i", "The input to be used");
	commandArguments.registerParameter("calibration", "c", "The calibration file to be used");
	commandArguments.registerParameter("resolution", "r", "The resolution of the input to be used, e.g., 1280x720");
	commandArguments.registerParameter("downsample", "d", "If specified, the input will be downsampled by a factor of 2");
	commandArguments.registerParameter("help", "h", "Displays this help");

	if (!commandArguments.parse(arguments))
	{
		Log::warning() << "Failure when parsing the command arguments";
	}

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		return;
	}

	std::string calibrationValue = commandArguments.value("calibration", std::string(), false);

	if (!calibrationValue.empty() && !IO::CameraCalibrationManager::get().registerCalibrations(calibrationValue))
	{
		Log::warning() << "Failed to register the calibration '" << calibrationValue << "'";
	}

	std::string inputValue;
	if (!commandArguments.hasValue("input", inputValue, false, 0u) || inputValue.empty())
	{
		Log::debug() << "No input defined, using LiveVideoId:0";

		inputValue = "LiveVideoId:0";
	}

	const IO::File inputFile(inputValue);

	if (inputFile.exists())
	{
		if (inputFile.extension() == "jpg" || inputFile.extension() == "png")
		{
			frameMedium_ = Media::Manager::get().newMedium(inputFile(), Media::Medium::IMAGE_SEQUENCE);

			if (Media::ImageSequenceRef imageSequence = frameMedium_)
			{
				imageSequence->setMode(Media::ImageSequence::SM_EXPLICIT);
			}
		}

		if (!frameMedium_)
		{
			frameMedium_ = Media::Manager::get().newMedium(inputFile(), Media::Medium::MOVIE);

			if (Media::MovieRef movie = frameMedium_)
			{
				movie->setSpeed(0.0f);
			}
		}
	}

	if (!frameMedium_)
	{
		frameMedium_ = Media::Manager::get().newMedium(inputValue);
	}

	if (!frameMedium_)
	{
		Platform::Utilities::showMessageBox("Error", "No valid input medium could be found!");

		// the device does not have an accessible live camera (or a necessary media plugin hasn't loaded successfully)
		return;
	}

	std::string resolution;
	if (commandArguments.hasValue("resolution", resolution))
	{
		unsigned int preferredWidth = 0u;
		unsigned int preferredHeight = 0u;
		if (Media::Utilities::parseResolution(resolution, preferredWidth, preferredHeight))
		{
			frameMedium_->setPreferredFrameDimension(preferredWidth, preferredHeight);
		}
		else
		{
			Log::error() << "Failed to parse resolution: " << resolution;
		}
	}

	frameMedium_->setPreferredFrameFrequency(30.0);

	downsample_ = commandArguments.hasValue("downsample");

	if (frameMedium_->start())
	{
		Log::info() << "Started input medium '" << frameMedium_->url() << "'";
	}
}

SLAMTrackerWrapper::~SLAMTrackerWrapper()
{
	// we do not release the aligner, this should be done by the user before the application ends
}

void SLAMTrackerWrapper::release()
{
	frameMedium_.release();

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	Media::DirectShow::unregisterDirectShowLibrary();
	Media::MediaFoundation::unregisterMediaFoundationLibrary();
	Media::WIC::unregisterWICLibrary();
#elif defined(OCEAN_PLATFORM_BUILD_APPLE)
	Media::AVFoundation::unregisterAVFLibrary();
	Media::ImageIO::unregisterImageIOLibrary();
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
	Media::Android::unregisterAndroidLibrary();
#endif
}

bool SLAMTrackerWrapper::trackNewFrame(Frame& outputFrame, bool* lastFrameReached)
{
	if (frameMedium_.isNull())
	{
		return false;
	}

	if (lastFrameReached && frameMedium_->stopTimestamp().isValid())
	{
		*lastFrameReached = true;
	}

	SharedAnyCamera camera;
	const FrameRef frame = frameMedium_->frame(&camera);

	if (frame.isNull() || frame->timestamp() == frameTimestamp_)
	{
		return false;
	}

	frameTimestamp_ = frame->timestamp();

	if (warmupTimestamp_.isValid())
	{
		if (warmupTimestamp_ <= frameTimestamp_)
		{
			warmupTimestamp_.toInvalid();
		}
	}

	if (!camera)
	{
		Log::error() << "The camera of the frame medium is invalid";
		ocean_assert(false && "Invalid camera model");

		return false;
	}

	{
		static bool once = true;
		if (once)
		{
			once = false;

			Log::info() << "Camera: " << camera->name() << ", " << camera->width() << "x" << camera->height() << ", " << Numeric::rad2deg(camera->fovX()) << ", " << camera->focalLengthX() << ", " << camera->focalLengthY() << ", " << camera->principalPointX() << ", " << camera->principalPointY();
		}
	}

	if (!CV::FrameConverter::Comfort::convert(*frame, FrameType::formatGrayscalePixelFormat(frame->pixelFormat()), FrameType::ORIGIN_UPPER_LEFT, yFrame_, false))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (downsample_)
	{
		CV::FrameShrinker::downsampleByTwo11(yFrame_);
		camera = camera->clone(yFrame_.width(), yFrame_.height());
	}

	static bool first = true;
	if (first)
	{
		first = false;

		Tracking::SLAM::TrackerMono::Configuration configuration;

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
		configuration.numberBins_ = 100u;
		configuration.patchSize_ = 7u;
#endif

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS
		configuration.numberBins_ = 160u;
		configuration.patchSize_ = 7u;
#endif

		slamTracker_.configure(configuration);
	}

	if (!CV::FrameConverter::Comfort::convert(yFrame_, FrameType::FORMAT_RGB24, outputFrame, true))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const Quaternion device_Q_camera(frameMedium_->device_T_camera().rotation());

	HomogenousMatrix4 world_T_camera(false);

	Tracking::SLAM::TrackerMono::DebugData debugData;

	const Vector3 cameraGravity = sensorAccessor_.cameraGravity(device_Q_camera, frameTimestamp_);

#ifndef DISABLE_TRACKING

	const Quaternion anyWorld_Q_camera = sensorAccessor_.anyWorld_Q_camera(device_Q_camera, frameTimestamp_);

	if (warmupTimestamp_.isInvalid())
	{
		performance_.start();
			slamTracker_.handleFrame(*camera, std::move(yFrame_), world_T_camera, cameraGravity, anyWorld_Q_camera, &debugData);
		performance_.stop();
	}

#ifndef OCEAN_DEBUG
	if (performance_.lastMseconds() > 15.0)
	{
		Log::info() << "SLAMTracker::handleFrame() slow: " << performance_.lastMseconds() << "ms"; // TODO remove
	}
#endif

#endif // DISABLE_TRACKING

	cameraClipper_.update(camera);

	if (!debugData.tracksMap_.empty())
	{
		constexpr size_t maximalTrackLength = 5;

		Tracking::SLAM::Utilities::paintDebugDataByLocalizationPrecision(outputFrame, cameraClipper_, world_T_camera, debugData, maximalTrackLength);
	}

	if (!cameraGravity.isNull())
	{
		Tracking::Utilities::paintGravity(*camera, outputFrame, cameraGravity, 1u, CV::Canvas::blue());
	}

	if (world_T_camera.isValid() && cameraClipper_.isValid())
	{
		Tracking::Utilities::paintCoordinateSystemIF(outputFrame, cameraClipper_, Camera::standard2InvertedFlipped(world_T_camera), HomogenousMatrix4(Vector3(0, 0, Scalar(-0.5))), Scalar(0.25));
	}

	if (performance_.measurements() % 150 == 1u)
	{
		Log::info() << "Performance handleFrame(): P95: " << performance_.percentileMseconds(0.95) << "ms";
	}

	CV::Canvas::drawText(outputFrame, String::toAString(performance_.averageMseconds(), 1u) + "ms, " + String::toAString(performance_.lastMseconds(), 1u) + "ms", 5, 5, CV::Canvas::white());

	return true;
}

bool SLAMTrackerWrapper::startRecording()
{
	if (!frameMedium_)
	{
		return false;
	}

	const ScopedLock scopedLock(deviceRecorderLock_);

	if (deviceRecorder_)
	{
		return false;
	}

	deviceRecorder_ = std::make_unique<Devices::Serialization::SerializerDeviceRecorder>();

	if (!deviceRecorder_->addFrameMedium(frameMedium_))
	{
		return false;
	}

	std::string recordingDirectory;

#if defined(OCEAN_PLATFORM_BUILD_ANDROID)
	Platform::Android::ResourceManager::externalFilesDirectory(recordingDirectory);
#elif defined(OCEAN_PLATFORM_BUILD_APPLE)
	recordingDirectory = Platform::Apple::Utilities::documentDirectory();
#endif

	const std::string recordingFilename = recordingDirectory + "slam_tracker_" + DateTime::localStringForFile() + ".osn";

	if (deviceRecorder_->start(recordingFilename))
	{
		Log::info() << "Started recording: " << recordingFilename;

		return true;
	}
	else
	{
		Log::error() << "Failed to start recording to file " << recordingFilename;
	}

	return false;
}

bool SLAMTrackerWrapper::stopRecording()
{
	const ScopedLock scopedLock(deviceRecorderLock_);

	if (!deviceRecorder_)
	{
		return false;
	}

	deviceRecorder_->stop(); // TODO, let recorder continue recording in background
	deviceRecorder_ = nullptr;

	return true;
}

SLAMTrackerWrapper& SLAMTrackerWrapper::operator=(SLAMTrackerWrapper&& slamTrackerWrapper) noexcept
{
	if (this != &slamTrackerWrapper)
	{
		// the aligner is designed to exist only once
		ocean_assert(frameMedium_.isNull());

		frameMedium_ = std::move(slamTrackerWrapper.frameMedium_);
		downsample_ = slamTrackerWrapper.downsample_;

		frameTimestamp_ = slamTrackerWrapper.frameTimestamp_;

		sensorAccessor_ = std::move(slamTrackerWrapper.sensorAccessor_);
	}

	return *this;
}
