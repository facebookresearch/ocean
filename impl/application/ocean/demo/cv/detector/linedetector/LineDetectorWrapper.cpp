/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "LineDetectorWrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/detector/LineDetectorHough.h"
#include "ocean/cv/detector/LineDetectorULF.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

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

#define USE_ULF_DETECTOR

LineDetectorWrapper::LineDetectorWrapper()
{
	// nothing to do here
}

LineDetectorWrapper::LineDetectorWrapper(const std::vector<std::wstring>& commandArguments)
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
		Media::MediaFoundation::registerMediaFoundationLibrary();
		Media::DirectShow::registerDirectShowLibrary();
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

	// we use the media plugins only
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
			{
				finiteMedium->setLoop(true);
			}
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

	// we start the medium so that medium will deliver frames

	frameMedium_->start();
}

LineDetectorWrapper::~LineDetectorWrapper()
{
	// We do not release this instance, this should be done by the user before the application ends
}

void LineDetectorWrapper::release()
{
	frameMedium_.release();

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		Media::DirectShow::unregisterDirectShowLibrary();
		Media::WIC::unregisterWICLibrary();
		Media::MediaFoundation::unregisterMediaFoundationLibrary();
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

bool LineDetectorWrapper::detectNewFrame(Frame& frame, double& time)
{
	if (frameMedium_.isNull())
	{
		return false;
	}

	// we request the most recent frame from our input/tracking medium

	const FrameRef liveFrame = frameMedium_->frame();

	if (liveFrame.isNull())
	{
		return false;
	}

	// we only handle a frame once

	if (liveFrame->timestamp() == frameTimestamp_)
	{
		return false;
	}

	frameTimestamp_ = liveFrame->timestamp();

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(*liveFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(*liveFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (performance_.measurements() % 20u == 0u)
	{
		performance_.reset();
	}

#ifdef USE_ULF_DETECTOR

	performance_.start();

	const unsigned int threshold = 40u;
	const unsigned int minimalLength = 10u;
	const float maximalStraightLineDistance = 1.6f;

	CV::Detector::LineDetectorULF::EdgeTypes edgeTypes;
	const FiniteLines2 finiteLines = CV::Detector::LineDetectorULF::detectLines(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), CV::Detector::LineDetectorULF::defaultEdgeDetectors(), threshold, minimalLength, maximalStraightLineDistance, &edgeTypes);

	performance_.stop();

	for (size_t n = 0; n < finiteLines.size(); ++n)
	{
		const FiniteLine2& line = finiteLines[n];

		const unsigned char* color = CV::Canvas::red();

		switch (uint8_t(edgeTypes[n]))
		{
			case CV::Detector::LineDetectorULF::ET_BAR | CV::Detector::LineDetectorULF::ET_SIGN_POSITIVE:
				color = CV::Canvas::white();
				break;

			case CV::Detector::LineDetectorULF::ET_BAR | CV::Detector::LineDetectorULF::ET_SIGN_NEGATIVE:
				color = CV::Canvas::gray();
				break;

			case CV::Detector::LineDetectorULF::ET_STEP | CV::Detector::LineDetectorULF::ET_SIGN_POSITIVE:
				color = CV::Canvas::red();
				break;

			case CV::Detector::LineDetectorULF::ET_STEP | CV::Detector::LineDetectorULF::ET_SIGN_NEGATIVE:
				color = CV::Canvas::green();
				break;

			default:
				ocean_assert(false && "This should never happen!");
				break;
		}

		CV::Canvas::line<3u>(rgbFrame, line, CV::Canvas::black());
		CV::Canvas::line<1u>(rgbFrame, line, color);
	}

#else // USE_ULF_DETECTOR

	performance_.start();

	constexpr CV::Detector::LineDetectorHough::FilterType filterType = CV::Detector::LineDetectorHough::FT_SOBEL;
	constexpr CV::Detector::LineDetectorHough::FilterResponse filterResponse = CV::Detector::LineDetectorHough::FR_HORIZONTAL_VERTICAL_DIAGONAL;
	constexpr bool optimizeLines = true;
	constexpr unsigned int accumulatorThreshold = 8u;
	constexpr bool determineExactPeak = true;

	CV::Detector::LineDetectorHough::InfiniteLines infiniteLines;
	FiniteLines2 finiteLines;

	CV::Detector::LineDetectorHough::detectLines(yFrame, filterType, filterResponse, infiniteLines, &finiteLines, optimizeLines, accumulatorThreshold, 4u, 5u, determineExactPeak, WorkerPool::get().scopedWorker()());

	performance_.stop();

	for (const FiniteLine2& line : finiteLines)
	{
		CV::Canvas::line8BitPerChannel<3u, 1u>(rgbFrame.data<uint8_t>(), rgbFrame.width(), rgbFrame.height(), line.point0().x(), line.point0().y(), line.point1().x(), line.point1().y(), CV::Canvas::green());
	}

#endif // USE_ULF_DETECTOR

	time = performance_.average();
	frame = std::move(rgbFrame);

	// we also set the timestamp of the resulting frame so that we know to which live video frame the result belongs
	frame.setTimestamp(frameTimestamp_);

	return true;
}
