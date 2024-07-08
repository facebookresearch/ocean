/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/pointtracker/PointTrackerWrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/media/FiniteMedium.h"
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
		#include "ocean/media/avfoundation/AVFoundation.h"
		#include "ocean/media/imageio/ImageIO.h"
	#elif defined(_ANDROID)
		#include "ocean/media/openimagelibraries/OpenImageLibraries.h"
	#endif
#endif

using namespace Tracking::Point;

PointTrackerWrapper::PointTrackerWrapper()
{
	// nothing to do here
}

PointTrackerWrapper::PointTrackerWrapper(const std::vector<std::wstring>& commandArguments)
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
	#elif defined(_ANDROID)
		Media::OpenImageLibraries::registerOpenImageLibrariesLibrary();
	#endif
#else

	// we collect all plugins located in the resource path of the application

	#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
		PluginManager::get().collectPlugins(StringOSX::toUTF8([[NSBundle mainBundle] resourcePath]));
	#else
		PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	#endif

	// although we could use the tracking capabilities via the devices interface we invoke the trackers directly to simplify the application
	// thus, we use the media plugins only
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));

#endif


	// first, we get access to the frame medium that is intended to be used for the tracking

	if (commandArguments.size() >= 1)
	{
		const std::string argument(String::toAString(commandArguments[0]));

		if (!argument.empty())
		{
			frameMedium_ = Media::Manager::get().newMedium(argument);

			// if we have a finite medium (e.g., a movie) we loop it

			const Media::FiniteMediumRef finiteMedium(frameMedium_);
			if (finiteMedium)
				finiteMedium->setLoop(true);
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


	// third, we check whether a desired frame dimension is specified for the input frame medium

	if (commandArguments.size() >= 2)
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

	// we start the medium so that medium will deliver frames

	frameMedium_->start();
}

PointTrackerWrapper::~PointTrackerWrapper()
{
	// we do not release the point tracker here, this should be done by the user before the application ends
}

void PointTrackerWrapper::release()
{
	frameMedium_.release();
	pointTracker_.clear();

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		Media::WIC::unregisterWICLibrary();
		Media::MediaFoundation::unregisterMediaFoundationLibrary();
		Media::DirectShow::unregisterDirectShowLibrary();
	#elif defined(__APPLE__)
		Media::AVFoundation::unregisterAVFLibrary();
		Media::ImageIO::unregisterImageIOLibrary();
	#elif defined(_ANDROID)
		Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
	#endif
#else
	PluginManager::get().release();
#endif
}

bool PointTrackerWrapper::trackNewFrame(Frame& frame, double& time)
{
	if (frameMedium_.isNull())
	{
		return false;
	}

	// we request the most recent frame from our input/tracking medium

	const FrameRef liveFrameRef = frameMedium_->frame();

	if (liveFrameRef.isNull())
	{
		return false;
	}

	// we only handle a frame once

	if (liveFrameRef->timestamp() == frameTimestamp_)
	{
		return false;
	}

	const Frame& liveFrame = *liveFrameRef;

	frameTimestamp_ = liveFrame.timestamp();

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(liveFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, true, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(liveFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, false, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (performance_.measurements() % 20u == 0u)
	{
		performance_.reset();
	}

	performance_.start();

	// we invoke the actual point tracking
	const unsigned int imageIndex = pointTracker_.newFrame(yFrame, WorkerPool::get().scopedWorker()());

	if (imageIndex != PointTracker::invalidFrameIndex)
	{
		// we draw all Tracks (paths of connected image points) belonging to feature points visible in the current frame
		// very short point paths have a red color, while longer point paths have a green color, with interpolation between red and green for intermediate lengths

		unsigned int maximalLength = 30;
		const PointTracker::PointTracks pointTracks = pointTracker_.pointTracks(imageIndex, maximalLength);

		performance_.stop();

		Scalars factors;
		factors.reserve(pointTracks.size());

		for (Vectors2 pointTrack : pointTracks)
		{
			factors.push_back(Scalar(pointTrack.size()) / Scalar(maximalLength));
		}

		const uint8_t* const red = CV::Canvas::red(rgbFrame.pixelFormat());
		const uint8_t* const green = CV::Canvas::green(rgbFrame.pixelFormat());
		Tracking::Utilities::paintPaths<3u>(rgbFrame, pointTracks.data(), pointTracks.size(), red, green, factors.data(), WorkerPool::get().scopedWorker()());
	}
	else
	{
		performance_.stop();
	}

	if (imageIndex >= 40u)
	{
		// we prevent the database from growing infinitely
		pointTracker_.clearUpTo(imageIndex - 40u);
	}

	time = performance_.average();
	frame = std::move(rgbFrame);

	// we also set the timestamp of the resulting frame so that we know to which live video frame the result belongs
	frame.setTimestamp(frameTimestamp_);

	return true;
}

void PointTrackerWrapper::nextTrackingMode()
{
	pointTracker_.setTrackingMode(PointTracker::TrackingMode((pointTracker_.trackingMode() + 1u) % PointTracker::TM_END));
}

std::string PointTrackerWrapper::trackingMode() const
{
	switch (pointTracker_.trackingMode())
	{
		case PointTracker::TM_SSD_7:
			return std::string("SSD 7");

		case PointTracker::TM_SSD_15:
			return std::string("SSD 15");

		case PointTracker::TM_SSD_31:
			return std::string("SSD 31");

		case PointTracker::TM_ZM_SSD_7:
			return std::string("ZM SSD 7");

		case PointTracker::TM_ZM_SSD_15:
			return std::string("ZM SSD 15");

		case PointTracker::TM_ZM_SSD_31:
			return std::string("ZM SSD 31");

		case PointTracker::TM_END:
			break;
	}

	ocean_assert(false && "Invalid tracking mode!");
	return std::string("Unknown");
}
