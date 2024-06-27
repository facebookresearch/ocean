/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/featuretracker/FeatureTrackerWrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/devices/Manager.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#include "ocean/tracking/Utilities.h"

#include "ocean/tracking/pattern/PatternTracker6DOF.h"

#include "ocean/tracking/orb/FeatureTracker6DOF.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/directshow/DirectShow.h"
		#include "ocean/media/mediafoundation/MediaFoundation.h"
		#include "ocean/media/wic/WIC.h"
	#elif defined(__APPLE__)
		#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
			#include "ocean/devices/ios/IOS.h"
		#endif

		#include "ocean/media/avfoundation/AVFoundation.h"
		#include "ocean/media/imageio/ImageIO.h"
	#elif defined(_ANDROID)
		#include "ocean/devices/android/Android.h"
		#include "ocean/media/openimagelibraries/OpenImageLibraries.h"
	#endif
#endif

FeatureTrackerWrapper::FeatureTrackerWrapper()
{
	// nothing to do here
}

FeatureTrackerWrapper::FeatureTrackerWrapper(const std::vector<std::wstring>& separatedCommandArguments)
{
#if 0
	// disable multi-core computation by forcing one CPU core
	Processor::get().forceCores(1);
#endif

	CommandArguments commandArguments("Demo application for feature-based trackers");
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as input parameter");
	commandArguments.registerParameter("help", "h", "Showing this help output.");
	commandArguments.registerParameter("input", "i", "Input to be used for tracking, e.g. an image sequence");
	commandArguments.registerParameter("pattern", "p", "Optional: the filename of the tracking pattern that will be used for tracking.");
	commandArguments.registerParameter("resolution", "r", "Optional: the resolution of the input, e.g. \"1280x720\"");
	commandArguments.registerParameter("tracker", "t", "Optional: the name of the tracker that will be used, e.g. \"Pattern 6DOF Tracker\", or \"ORB Feature Based 6DOF Tracker\"");
	commandArguments.registerParameter("calibration", "c", "Optional: the filename of the camera calibration file containing the calibration for the input source (*.occ)");

	commandArguments.parse(separatedCommandArguments);

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		exit(0);
	}

	// first, we register or load the media plugin(s)
	// if we have a shared runtime we simply load all media plugins available in a specific directory
	// if we have a static runtime we explicitly need to register all plugins we want to use (at compile time)

	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		Media::DirectShow::registerDirectShowLibrary();
		Media::MediaFoundation::registerMediaFoundationLibrary();
		Media::WIC::registerWICLibrary();
	#elif defined(__APPLE__)
		Media::AVFoundation::registerAVFLibrary();
		Media::ImageIO::registerImageIOLibrary();

		#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
			Devices::IOS::registerIOSLibrary();
		#endif
	#elif defined(_ANDROID)
		Media::OpenImageLibraries::registerOpenImageLibrariesLibrary();
		Devices::Android::registerAndroidLibrary();
	#endif
#else

	// we collect all plugins located in the resource path of the application

	#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
		PluginManager::get().collectPlugins(StringOSX::toUTF8([[NSBundle mainBundle] resourcePath])); // ]] <- DEBUG VSCode thinks the closing brackets are missing; messes up syntax high-lighting
	#else
		PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	#endif

	// Although we could use the tracking capabilities via the devices interface we invoke the trackers directly to simplify the application
	// thus, we use the media plugins only
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
#endif

	// first, we get access to the frame medium that is intended to be used for the tracking

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER
	devicePlayer_ = FeatureTrackerWrapper_createExternalDevicePlayer(commandArguments);
#endif

	if (devicePlayer_)
	{
		if (devicePlayer_->start())
		{
			if (devicePlayer_->frameMediums().empty())
			{
				Log::error() << "The recording does not contain frame mediums";
			}
			else
			{
				// Only select the first medium and ignore all others
				inputMedium_ = devicePlayer_->frameMediums().front();
				ocean_assert(inputMedium_);
			}
		}
		else
		{
			Log::error() << "Failed to start the recording";
		}

		if (inputMedium_.isNull())
		{
			Log::error() << "Invalid recording input";
			return;
		}
	}

	// we check whether an explicit camera calibration file has been provided

	IO::File cameraCalibrationFile;

	Value calibrationValue;
	if (commandArguments.hasValue("calibration", &calibrationValue, false, 0u) && calibrationValue.isString())
	{
		const IO::File file(calibrationValue.stringValue());

		if (file.exists())
		{
			cameraCalibrationFile = file;
		}
	}

	if (cameraCalibrationFile.isNull())
	{
		const IO::File relativeFile("res/application/ocean/demo/tracking/featuretracker/cameracalibration.occ");

		cameraCalibrationFile = IO::Directory(frameworkPath) + relativeFile;
	}

	if (cameraCalibrationFile.exists())
	{
		IO::CameraCalibrationManager::get().registerCalibrationFile(cameraCalibrationFile());
	}

	if (inputMedium_.isNull())
	{
		std::string input;
		if (commandArguments.hasValue("input", input, false, 0u) && !input.empty())
		{
			inputMedium_ = Media::Manager::get().newMedium(input);

			// if we have a finite medium (e.g., a movie) we loop it

			const Media::FiniteMediumRef finiteMedium(inputMedium_);

			if (finiteMedium)
			{
				finiteMedium->setLoop(true);
			}
		}
	}

	if (inputMedium_.isNull())
	{
		// if the user did not specify a medium, first we try to get a live video with id 1 (often an external web cam - not the builtin camera of a laptop)
		inputMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");
	}

	if (inputMedium_.isNull())
	{
		// if we could not get the device with id 1 we try to get the device with id 0
		inputMedium_ = Media::Manager::get().newMedium("LiveVideoId:0");
	}

	if (inputMedium_.isNull())
	{
		Platform::Utilities::showMessageBox("Error", "No valid input medium could be found!");

		// the device does not have an accessible live camera (or a necessary media plugin hasn't loaded successfully)
		return;
	}


	// second, we load the pattern that is intended to be used for the feature map (to be used as tracking reference)

	IO::File patternAbsoluteFile;

	Value patternValue;
	if (commandArguments.hasValue("pattern", &patternValue, false) && patternValue.isString())
	{
		const std::string argument = patternValue.stringValue();

		const IO::File file(argument);

		if (file.exists())
		{
			patternAbsoluteFile = file;
		}
		else
		{
			Platform::Utilities::showMessageBox("Error", std::string("Could not find a valid tracking pattern!\n\nGot \"") + file() + std::string("\""));
		}
	}
	else
	{
		const IO::File relativeFile("res/application/ocean/demo/tracking/featuretracker/tropical-island-with-toucans_800x800.jpg");

		patternAbsoluteFile = IO::Directory(frameworkPath) + relativeFile;
	}

	if (!patternAbsoluteFile.exists())
	{
		Log::warning() << "The pattern file \"" << patternAbsoluteFile() << "\" does not exist.";

		// we do not have a valid tracking pattern, so we stop here
		return;
	}

	const Frame patternFrame = Media::Utilities::loadImage(patternAbsoluteFile());

	if (!patternFrame.isValid())
	{
		Log::warning() << "The pattern file \"" << patternAbsoluteFile() << "\" could not be loaded.";

		// we do not have a valid tracking pattern, so we stop here
		return;
	}


	// third, we check whether a desired frame dimension is specified for the input frame medium

	Value resolutionValue;
	if (commandArguments.hasValue("resolution", &resolutionValue, false, 0u) && resolutionValue.isString())
	{
		const std::string dimension = resolutionValue.stringValue();

		if (dimension == "320x240")
		{
			inputMedium_->setPreferredFrameDimension(320u, 240u);
		}
		else if (dimension == "640x480")
		{
			inputMedium_->setPreferredFrameDimension(640u, 480u);
		}
		else if (dimension == "1280x720")
		{
			inputMedium_->setPreferredFrameDimension(1280u, 720u);
		}
		else if (dimension == "1920x1080")
		{
			inputMedium_->setPreferredFrameDimension(1920u, 1080u);
		}
	}
	else
	{
		inputMedium_->setPreferredFrameDimension(1280u, 720u);
	}


	// fourth, we check whether a specific tracker type is specified, if not we simply take one of the existing ones

	std::string trackerName;

	Value trackerValue;
	if (commandArguments.hasValue("tracker", &trackerValue, false, 0u) && trackerValue.isString())
	{
		trackerName = trackerValue.stringValue();
	}

	const Scalar patternWidth = Scalar(1.0); // we simply define the width of the pattern only

	ocean_assert(patternFrame.width() != 0u);
	const Vector2 patternDimension = Vector2(patternWidth, patternWidth * Scalar(patternFrame.height()) / Scalar(patternFrame.width()));

	if (visualTracker_.isNull() && trackerName == std::string("ORB Feature Based 6DOF Tracker"))
	{
		visualTracker_ = Tracking::VisualTrackerRef(new Tracking::ORB::FeatureTracker6DOF());
		visualTracker_.force<Tracking::ORB::FeatureTracker6DOF>().setFeatureMap(Tracking::ORB::FeatureMap(patternFrame, patternDimension, Scalar(6), true, 0u, false, WorkerPool::get().scopedWorker()()));

		objectDimension_ = Box3(Vector3(0, 0, 0), Vector3(patternDimension.x(), patternDimension.length() * Scalar(0.2), patternDimension.y()));
	}

	if (visualTracker_.isNull() || trackerName == std::string("Pattern 6DOF Tracker"))
	{
		visualTracker_ = Tracking::VisualTrackerRef(new Tracking::Pattern::PatternTracker6DOF());
		visualTracker_.force<Tracking::Pattern::PatternTracker6DOF>().addPattern(patternFrame, patternDimension, WorkerPool::get().scopedWorker()());

		objectDimension_ = Box3(Vector3(0, 0, 0), Vector3(patternDimension.x(), patternDimension.length() * Scalar(0.2), patternDimension.y()));
	}

	if (visualTracker_.isNull())
	{
		Platform::Utilities::showMessageBox("Error", std::string("No valid tracker found!\n\nGot \"") + trackerName + std::string("\""));

		// we could not create a valid tracker
		return;
	}

	// we start the medium so that medium will deliver frames

	inputMedium_->start();

	orientationTracker3DOF_ = Devices::Manager::get().device(Devices::OrientationTracker3DOF::deviceTypeOrientationTracker3DOF());

	if (orientationTracker3DOF_)
	{
		orientationTracker3DOF_->start();
	}
}

FeatureTrackerWrapper::~FeatureTrackerWrapper()
{
	// we do not release the tracker, this should be done by the user before the application ends
}

void FeatureTrackerWrapper::release()
{
	orientationTracker3DOF_.release();

	inputMedium_.release();

	if (devicePlayer_ && devicePlayer_->isStarted())
	{
		devicePlayer_->stop();
	}

	devicePlayer_ = nullptr;

	visualTracker_.release();

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		Media::DirectShow::unregisterDirectShowLibrary();
		Media::MediaFoundation::unregisterMediaFoundationLibrary();
		Media::WIC::unregisterWICLibrary();
	#elif defined(__APPLE__)
		#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
			Devices::IOS::unregisterIOSLibrary();
		#endif

		Media::AVFoundation::unregisterAVFLibrary();
		Media::ImageIO::unregisterImageIOLibrary();
	#elif defined(_ANDROID)
		Devices::Android::unregisterAndroidLibrary();
		Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
	#endif
#else
	PluginManager::get().release();
#endif
}

bool FeatureTrackerWrapper::trackNewFrame(Frame& resultFrame, double& time)
{
	if (visualTracker_.isNull() || inputMedium_.isNull())
	{
		return false;
	}

	FrameRef frameRef = inputMedium_->frame(&anyCamera_);

	if (anyCamera_ == nullptr || !anyCamera_->isValid())
	{
		// we still need to request the correct camera profile for our input medium
		// therefore, we need to know the dimensions of the input medium (the delivered frames respectively)

		if (frameRef.isNull())
		{
			frameRef = inputMedium_->frame();
		}

		if (frameRef.isNull())
		{
			// if we cannot extract the first frame within 5 seconds since we started the medium, something must be wrong

			if (inputMedium_->startTimestamp() + 5.0 < Timestamp(true))
			{
				Platform::Utilities::showMessageBox("Error", "Could not extract a valid frame from the input source!\nDefine a different source as input.");

				// we release the medium to ensure that we stop immediately the next time this function is called
				inputMedium_.release();
			}

			return false;
		}

		// the camera calibration manager will either provided the calibrated profile (if existing) or will provide a default profile

		anyCamera_ = std::make_shared<AnyCameraPinhole>(IO::CameraCalibrationManager::get().camera(inputMedium_->url(), frameRef->width(), frameRef->height(), nullptr, Numeric::deg2rad(60)));
	}

	ocean_assert(anyCamera_ && anyCamera_->isValid());

	// currently, only pinhole cameras are supported by the trackers, so abort if a different type of camera is used.

	if (frameRef.isNull())
	{
		return false;
	}

	// we only handle a frame once

	if (frameRef->timestamp() == lastHandledFrameTimestamp_)
	{
		return false;
	}

	const Frame& frame = *frameRef;

	lastHandledFrameTimestamp_ = frame.timestamp();

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (performance_.measurements() % 20u == 0u)
	{
		performance_.reset();
	}

	Quaternion world_Q_camera(false);
	if (orientationTracker3DOF_)
	{
		const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef sample = orientationTracker3DOF_->sample(lastHandledFrameTimestamp_, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

		if (sample && sample->orientations().size() == 1)
		{
			const Quaternion& world_Q_device = sample->orientations().front();

			world_Q_camera = world_Q_device * Quaternion(inputMedium_->device_T_camera().rotation());
		}
	}

	performance_.start();

	const Frames frames = { frame };
	const SharedAnyCameras anyCameras = { anyCamera_ };

	Tracking::VisualTracker::TransformationSamples resultingTransformationSamples;
	if (visualTracker_->determinePoses(frames, anyCameras, resultingTransformationSamples, world_Q_camera, WorkerPool::get().scopedWorker()()) && !resultingTransformationSamples.empty())
	{
		performance_.stop();

		// the resulting pose transforms points defined in the coordinate system of the camera to points defined in the coordinate system of the world (the pattern)

		ocean_assert(!resultingTransformationSamples.empty());
		const HomogenousMatrix4& resultingPose = resultingTransformationSamples.front().transformation();

		const HomogenousMatrix4 resultingPoseIF(PinholeCamera::standard2InvertedFlipped(resultingPose));

		const uint8_t* const black = CV::Canvas::black(rgbFrame.pixelFormat());
		const uint8_t* const white = CV::Canvas::white(rgbFrame.pixelFormat());

		Tracking::Utilities::paintBoundingBoxIF(rgbFrame, resultingPoseIF, *anyCamera_, objectDimension_, white, black);
		Tracking::Utilities::paintCoordinateSystemIF(rgbFrame, resultingPoseIF, *anyCamera_, HomogenousMatrix4(true), objectDimension_.diagonal() * Scalar(0.1));
	}
	else
	{
		performance_.stop();
	}

	time = performance_.average();
	resultFrame = std::move(rgbFrame);

	// we also set the timestamp of the resulting frame so that we know to which live video frame the result belongs
	resultFrame.setTimestamp(lastHandledFrameTimestamp_);

	return true;
}
