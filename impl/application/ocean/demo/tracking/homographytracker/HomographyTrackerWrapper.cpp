/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/homographytracker/HomographyTrackerWrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/devices/Manager.h"

#include "ocean/geometry/Homography.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/ImageSequence.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#include "ocean/tracking/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/directshow/DirectShow.h"
		#include "ocean/media/mediafoundation/MediaFoundation.h"
		#include "ocean/media/wic/WIC.h"
	#elif defined(__APPLE__)
		#if TARGET_OS_IPHONE == 1
			#include "ocean/devices/ios/IOS.h"
		#endif

		#include "ocean/media/avfoundation/AVFoundation.h"
		#include "ocean/media/imageio/ImageIO.h"
	#elif defined(_ANDROID)
		#include "ocean/devices/android/Android.h"
	#endif
#endif

HomographyTrackerWrapper::HomographyTrackerWrapper() :
	trackingPixelFormat_(FrameType::FORMAT_Y8)
{
	// nothing to do here
}

HomographyTrackerWrapper::HomographyTrackerWrapper(HomographyTrackerWrapper&& homographyTrackerWrapper)
{
	*this = std::move(homographyTrackerWrapper);
}

HomographyTrackerWrapper::HomographyTrackerWrapper(const std::vector<std::wstring>& commandArguments) :
	trackingPixelFormat_(FrameType::FORMAT_Y8)
{
#if 0
	// disable multi-core computation by forcing one CPU core
	Processor::get().forceCores(1);
#endif

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

		#if TARGET_OS_IPHONE == 1
			Devices::IOS::registerIOSLibrary();
		#endif
	#elif defined(_ANDROID)
		Devices::Android::registerAndroidLibrary();
	#endif
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());

	// although we could use the tracking capabilities via the devices interface we invoke the trackers directly to simplify the application
	// thus, we use the media plugins only
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
#endif


	// first, we get access to the frame medium that is intended to be used for the tracking

	if (commandArguments.size() > 0 && !commandArguments[0].empty())
	{
		const std::string argument = String::toAString(commandArguments[0]);

		// first we try to get an image sequence

		frameMedium_ = Media::Manager::get().newMedium(argument, Media::Medium::IMAGE_SEQUENCE);

		const Media::ImageSequenceRef imageSequence(frameMedium_);

		if (imageSequence)
		{
			// in the case we have an image sequence as input we want to process the images as fast as possible
			// (not with any specific fps number) so we use the explicit mode

			imageSequence->setMode(Media::ImageSequence::SM_EXPLICIT);
		}
		else
		{
			// provided command argument seems to be something else but an image sequence
			// so now we try to get any possible medium

			frameMedium_ = Media::Manager::get().newMedium(argument);
		}
	}

	if (frameMedium_.isNull())
	{
		// if the user did not specify a medium, first we try to get a live video with id 1 (often an external web cam - not the built-in camera of a laptop)
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");
	}

	if (frameMedium_.isNull())
	{
		// if we could not get the device with id 1 we try to get the device with id 0
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0");
	}

	if (frameMedium_.isNull())
	{
		Platform::Utilities::showMessageBox("Error", "No valid input medium could be found!");

		// the device does not have an accessible live camera (or a necessary media plugin hasn't loaded successfully)
		return;
	}


	// second, we check whether a desired frame dimension is specified for the input frame medium

	if (commandArguments.size() > 1 && !commandArguments[1].empty())
	{
		const std::string dimension = String::toAString(commandArguments[1]);

		if (dimension == "320x240")
		{
			frameMedium_->setPreferredFrameDimension(320u, 240u);
		}
		else if (dimension == "640x480")
		{
			frameMedium_->setPreferredFrameDimension(640u, 480u);
		}
		else if (dimension == "1280x720")
		{
			frameMedium_->setPreferredFrameDimension(1280u, 720u);
		}
		else if (dimension == "1920x1080")
		{
			frameMedium_->setPreferredFrameDimension(1920u, 1080u);
		}
	}

	if (Media::FiniteMediumRef finiteMedium = frameMedium_)
	{
		finiteMedium->setLoop(false);
	}

	// we start the medium so that medium will deliver frames and wait for the first frame to be able to receive the matching camera calibration

	frameMedium_->start();

	orientationTracker3DOF_ = Devices::Manager::get().device(Devices::OrientationTracker3DOF::deviceTypeOrientationTracker3DOF());

	if (orientationTracker3DOF_)
	{
		orientationTracker3DOF_->start();
	}
}

HomographyTrackerWrapper::~HomographyTrackerWrapper()
{
	// we do not release the aligner, this should be done by the user before the application ends
}

void HomographyTrackerWrapper::release()
{
	orientationTracker3DOF_.release();
	frameMedium_.release();

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		Media::DirectShow::unregisterDirectShowLibrary();
		Media::MediaFoundation::unregisterMediaFoundationLibrary();
		Media::WIC::unregisterWICLibrary();
	#elif defined(__APPLE__)
		#if TARGET_OS_IPHONE == 1
			Devices::IOS::unregisterIOSLibrary();
		#endif
		Media::AVFoundation::unregisterAVFLibrary();
		Media::ImageIO::unregisterImageIOLibrary();
	#elif defined(_ANDROID)
		Devices::Android::unregisterAndroidLibrary();
	#endif
#else
	PluginManager::get().release();
#endif
}

bool HomographyTrackerWrapper::trackNewFrame(Frame& frame, double& time, const Vector2& newObjectPosition, bool* lastFrameReached)
{
	if (lastFrameReached)
	{
		*lastFrameReached = false;
	}

	if (frameMedium_.isNull())
	{
		return false;
	}

	if (lastFrameReached && frameMedium_->stopTimestamp().isValid())
	{
		*lastFrameReached = true;
	}

	// we request the most recent frame from our input medium

	SharedAnyCamera camera;
	const FrameRef currentFrameRef = frameMedium_->frame(&camera);

	if (currentFrameRef.isNull())
	{
		return false;
	}

	// we only handle a frame once

	if (currentFrameRef->timestamp() == frameTimestamp_)
	{
		return false;
	}

	const Frame& currentFrame = *currentFrameRef;

	if (!camera)
	{
		Log::warning() << "Unknown camera profile, using default profile instead";

		camera = std::make_shared<AnyCameraPinhole>(PinholeCamera(currentFrame.width(), currentFrame.height(), Numeric::deg2rad(60)));
	}

	frameTimestamp_ = currentFrame.timestamp();

	Quaternion world_Q_camera(false);
#if 0 // currently disabled
	if (orientationTracker3DOF_)
	{
		const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef sample = orientationTracker3DOF_->sample(frameTimestamp_, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

		if (sample && sample->orientations().size() == 1)
		{
			// the negative z-axis of the world coordinate system of the IMU (of the device) is pointing towards the ground (is identical with the gravity vector)
			// although the HomographyTracker does not explicit a specific coordinate system, we provide a coordinate system with negative y-axis pointing towards the ground

			const Quaternion& flippedWorld_Q_device = sample->orientations().front();

			const Quaternion world_Q_flippedWorld(Vector3(1, 0, 0), -Numeric::pi_2());

			world_Q_camera = world_Q_flippedWorld * flippedWorld_Q_device * Quaternion(frameMedium_->device_T_camera().rotation());
		}
	}
#endif

	// as we will need worker objects in several function calls we simply request one for the remaining function

	const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

	Frame currentFrameRGB;
	if (!CV::FrameConverter::Comfort::convert(currentFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, currentFrameRGB, CV::FrameConverter::CP_ALWAYS_COPY, scopedWorker()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (performance_.measurements() % 20u == 0u)
	{
		performance_.reset();
	}

	Frame trackingFrame;
	if (!CV::FrameConverter::Comfort::convert(currentFrame, trackingPixelFormat_, FrameType::ORIGIN_UPPER_LEFT, trackingFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, scopedWorker()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	ocean_assert(!frame.isValid());

	if (newObjectPosition.x() >= Scalar(0))
	{
		// we have a new tracking location provided by the user

		trackerRegion_ = Box2(newObjectPosition, Scalar(200), Scalar(200));

		if (world_Q_camera.isValid())
		{
			const Vector3 groundPlaneNormal(0, 1, 0);

			homographyTracker_.resetRegion(*camera, trackerRegion_, world_Q_camera, groundPlaneNormal);
		}
		else
		{
			homographyTracker_.resetRegion(*camera, trackerRegion_);
		}
	}

	if (trackerRegion_.isValid())
	{
		HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance_);

		SquareMatrix3 homography;

		if (homographyTracker_.determineHomography(*camera, trackingFrame, homography, nullptr, world_Q_camera, scopedWorker()))
		{
			scopedPerformance.release();

			Vectors2 regionCorners;

			for (unsigned int n = 0u; n < 4u; ++n)
			{
				regionCorners.push_back(homography * trackerRegion_.corner(n));
			}

			const Vectors2& startPositions = regionCorners;

			Vectors2 stopPositions;
			stopPositions.reserve(regionCorners.size());

			stopPositions.push_back(startPositions.back());

			for (size_t n = 0; n < startPositions.size() - 1; ++n)
			{
				stopPositions.push_back(startPositions[n]);
			}

			Tracking::Utilities::paintLines<3u, 5u>(currentFrameRGB, startPositions.data(), stopPositions.data(), startPositions.size(), CV::Canvas::white(), CV::Canvas::black());
		}
		else
		{
			//trackerRegion_ = Box2();
		}
	}

	// we swap both pyramids for the next iteration

	frame = std::move(currentFrameRGB);

	time = performance_.average();

	if (Media::ImageSequenceRef imageSequence = frameMedium_)
	{
		// in the case we use an image sequence as input we simply now activate the next frame
		// (as we use the explicit mode of the sequence)

		imageSequence->forceNextFrame();
	}

	return true;
}

HomographyTrackerWrapper& HomographyTrackerWrapper::operator=(HomographyTrackerWrapper&& homographyTrackerWrapper)
{
	if (this != &homographyTrackerWrapper)
	{
		// the aligner is designed to exist only once
		ocean_assert(frameMedium_.isNull());

		frameMedium_ = std::move(homographyTrackerWrapper.frameMedium_);

		trackingPixelFormat_ = homographyTrackerWrapper.trackingPixelFormat_;

		frameTimestamp_ = homographyTrackerWrapper.frameTimestamp_;

		trackerRegion_ = std::move(homographyTrackerWrapper.trackerRegion_);

		orientationTracker3DOF_ = std::move(homographyTrackerWrapper.orientationTracker3DOF_);
	}

	return *this;
}
