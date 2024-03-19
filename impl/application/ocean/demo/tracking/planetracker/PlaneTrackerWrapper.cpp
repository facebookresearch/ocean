// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/tracking/planetracker/PlaneTrackerWrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/devices/Manager.h"

#include "ocean/io/CameraCalibrationManager.h"

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
		#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
			#include "ocean/devices/ios/IOS.h"
		#endif

		#include "ocean/media/avfoundation/AVFoundation.h"
		#include "ocean/media/imageio/ImageIO.h"
	#elif defined(_ANDROID)
		#include "ocean/devices/android/Android.h"
	#endif
#endif

using namespace Tracking::Plane;

PlaneTrackerWrapper::PlaneTrackerWrapper()
{
	// nothing to do here
}

PlaneTrackerWrapper::PlaneTrackerWrapper(PlaneTrackerWrapper&& planeTrackerWrapper) noexcept :
	PlaneTrackerWrapper()
{
	*this = std::move(planeTrackerWrapper);
}

PlaneTrackerWrapper::PlaneTrackerWrapper(const std::vector<std::wstring>& commandArguments)
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

		#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
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
		// if the user did not specify a medium, first we try to get a live video with id 1 (often an external web cam - not the builtin camera of a laptop)
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
			frameMedium_->setPreferredFrameDimension(320u, 240u);
		else if (dimension == "640x480")
			frameMedium_->setPreferredFrameDimension(640u, 480u);
		else if (dimension == "1280x720")
			frameMedium_->setPreferredFrameDimension(1280u, 720u);
		else if (dimension == "1920x1080")
			frameMedium_->setPreferredFrameDimension(1920u, 1080u);
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

PlaneTrackerWrapper::~PlaneTrackerWrapper()
{
	// nothing to do here
}

void PlaneTrackerWrapper::release()
{
	orientationTracker3DOF_.release();
	frameMedium_.release();

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
	#endif
#else
	PluginManager::get().release();
#endif
}

bool PlaneTrackerWrapper::trackNewFrame(Frame& frame, double& time, const Vector2& newPlanePosition, bool* lastFrameReached)
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

	const FrameRef currentFrame = frameMedium_->frame();

	if (currentFrame.isNull())
	{
		return false;
	}

	// we only handle a frame once

	if (currentFrame->timestamp() == frameTimestamp_)
	{
		return false;
	}

	frameTimestamp_ = currentFrame->timestamp();

	Quaternion world_Q_camera(false);
	if (orientationTracker3DOF_)
	{
		const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef sample = orientationTracker3DOF_->sample(frameTimestamp_, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

		if (sample && sample->orientations().size() == 1)
		{
			// the negative z-axis of the world coordinate system of the IMU (of the device) is pointing towards the ground (is identical with the gravity vector)
			// although the HomographyTracker does not expcit a specific coordinate system, we provide a coordinate system with negative y-axis pointing towards the ground

			const Quaternion& flippedWorld_Q_device = sample->orientations().front();

			const Quaternion world_Q_flippedWorld(Vector3(1, 0, 0), -Numeric::pi_2());

			world_Q_camera = world_Q_flippedWorld * flippedWorld_Q_device * Quaternion(frameMedium_->device_T_camera().rotation());
		}
	}

	const Media::ImageSequenceRef imageSequence(frameMedium_);

	// as we will need worker objects in several function calls we simply request one for the remaining function

	const WorkerPool::ScopedWorker scopedWorker(WorkerPool::get().scopedWorker());

	Frame currentFrameRGB;
	if (!CV::FrameConverter::Comfort::convert(*currentFrame, FrameType(*currentFrame, FrameType::FORMAT_RGB24), currentFrameRGB, CV::FrameConverter::CP_ALWAYS_COPY, scopedWorker()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(*currentFrame, FrameType(*currentFrame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, scopedWorker()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	ocean_assert(yFrame);

	if (!camera_)
	{
		camera_ = IO::CameraCalibrationManager::get().camera(frameMedium_->url(), yFrame.width(), yFrame.height());
	}

	ocean_assert(camera_.isValid());

	if (!world_Q_camera.isValid())
	{
		// **TODO** workaround to ensure we always have a valid camera orientation
		world_Q_camera = Quaternion(true);
	}

	if (performance_.measurements() % 20u == 0u)
	{
		performance_.reset();
	}

	HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance_);

	if (newPlanePosition.x() >= Scalar(0))
	{
		// we have a new tracking location provided by the user

		if (planeTracker_.planes().size() >= 1)
		{
			planeTracker_.removePlane(planeTracker_.planes().begin()->first);
		}

		ocean_assert(camera_.isInside(newPlanePosition));
		const Vectors2 newPlanePositions(1, newPlanePosition);
		const PlaneTracker::PlaneProperties planeProperties(1, PlaneTracker::PP_HORIZONTAL_OR_VERTICAL);

		const PlaneTracker::PlaneIds planeIds = planeTracker_.addPlanes(camera_, yFrame, newPlanePositions, world_Q_camera, planeProperties, WorkerPool::get().scopedWorker()());
		ocean_assert(planeIds.size() == newPlanePositions.size());

		for (size_t n = 0; n < planeIds.size(); ++n)
		{
			if (planeIds[n] != (unsigned int)(-1))
			{
				Log::info() << "Valid plane";
			}
			else
			{
				Log::info() << "INVALID PLANE!";
			}
		}
	}

	const bool trackingResult = planeTracker_.trackPlanes(camera_, yFrame, world_Q_camera, WorkerPool::get().scopedWorker()());

	scopedPerformance.release();

	const PlaneTracker::PlaneMap planes(planeTracker_.planes());

	for (PlaneTracker::PlaneMap::const_iterator i = planes.cbegin(); i != planes.cend(); ++i)
	{
		const PlaneTracker::Plane& plane = i->second;

		// a plane can have an invalid pose, in case the plane is currently out of view

		if (plane.pose().isValid())
		{
			const unsigned char white[3] = {0xFF, 0xFF, 0xFF};
			const unsigned char black[3] = {0x00, 0x00, 0x00};

			const HomogenousMatrix4& pose = plane.pose();
			const HomogenousMatrix4& object = plane.object();

			const HomogenousMatrix4 poseIF = PinholeCamera::standard2InvertedFlipped(pose);

			Tracking::Utilities::paintPlaneIF(currentFrameRGB, poseIF, AnyCameraPinhole(camera_), object, 0.4, 9u, plane.accuratePoseTimestamp() == yFrame.timestamp() ? white : black, black);
			Tracking::Utilities::paintCoordinateSystemIF(currentFrameRGB, poseIF, AnyCameraPinhole(camera_), object, .1);
		}
	}

#if 0
	{
		// draw IMU-based gravity vector

		const HomogenousMatrix4 pose(wTc);

		const Vector2 originInCamera = camera_.principalPoint();
		const Vector3 originInWorld = camera_.ray(originInCamera, pose).point(1);

		const Vector3 gravityVectorInWorld = originInWorld + Vector3(0, -1, 0); // negative y-axis
		const Vector2 gravityVectorInCamera = camera_.projectToImage<true>(pose, gravityVectorInWorld, false);

		CV::Canvas::line<5u>(currentFrameRGB, originInCamera.x(), originInCamera.y(), gravityVectorInCamera.x(), gravityVectorInCamera.y());
	}
#endif

#if 0
	{
		// draw IMU-based coordinate system

		const HomogenousMatrix4 pose(wTc);

		const Vector2 originInCamera = camera_.principalPoint();
		const Vector3 originInWorld = camera_.ray(originInCamera, pose).point(1);

		const unsigned char red[] = {0xFF, 0x00, 0x00};
		const unsigned char green[] = {0x00, 0xFF, 0x00};
		const unsigned char blue[] = {0x00, 0x00, 0xFF};

		// draw x-axis
		const Vector3 xAxisInWorld = originInWorld + Vector3(0.5, 0, 0);
		const Vector2 xAxisInCamera = camera_.projectToImage<true>(pose, xAxisInWorld, false);
		CV::Canvas::line<5u>(currentPaddingFrameRGB, originInCamera.x(), originInCamera.y(), xAxisInCamera.x(), xAxisInCamera.y(), red);

		// draw y-axis
		const Vector3 yAxisInWorld = originInWorld + Vector3(0, 0.5, 0);
		const Vector2 yAxisInCamera = camera_.projectToImage<true>(pose, yAxisInWorld, false);
		CV::Canvas::line<5u>(currentPaddingFrameRGB, originInCamera.x(), originInCamera.y(), yAxisInCamera.x(), yAxisInCamera.y(), green);

		// draw z-axis
		const Vector3 zAxisInWorld = originInWorld + Vector3(0, 0, 0.5);
		const Vector2 zAxisInCamera = camera_.projectToImage<true>(pose, zAxisInWorld, false);

		CV::Canvas::line<5u>(currentFrameRGB, originInCamera.x(), originInCamera.y(), zAxisInCamera.x(), zAxisInCamera.y(), blue);
	}
#endif

	// we swap both pyramids for the next iteration

	frame = std::move(currentFrameRGB);
	frame.setTimestamp(frameTimestamp_);

	time = performance_.average();

	if (imageSequence)
	{
		// in the case we use an image sequence as input we simply now activate the next frame
		// (as we use the explicit mode of the sequence)

		imageSequence->forceNextFrame();
	}

	return trackingResult;
}

PlaneTrackerWrapper& PlaneTrackerWrapper::operator=(PlaneTrackerWrapper&& planeTrackerWrapper) noexcept
{
	if (this != &planeTrackerWrapper)
	{
		// the tracker is designed to exist only once
		ocean_assert(frameMedium_.isNull());

		frameMedium_ = std::move(planeTrackerWrapper.frameMedium_);

		camera_ = planeTrackerWrapper.camera_;

		frameTimestamp_ = planeTrackerWrapper.frameTimestamp_;

		orientationTracker3DOF_ = std::move(planeTrackerWrapper.orientationTracker3DOF_);
	}

	return *this;
}
