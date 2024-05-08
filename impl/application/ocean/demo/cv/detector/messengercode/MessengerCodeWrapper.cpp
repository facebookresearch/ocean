/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "MessengerCodeWrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/detector/MessengerCodeDetector.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/ImageSequence.h"
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

MessengerCodeWrapper::MessengerCodeWrapper() :
	pixelFormat_(FrameType::FORMAT_RGB24)
{
	// Nothing to do here
}

MessengerCodeWrapper::MessengerCodeWrapper(MessengerCodeWrapper&& messengerCodeWrapper) noexcept
{
	*this = std::move(messengerCodeWrapper);
}

MessengerCodeWrapper::MessengerCodeWrapper(const std::vector<std::wstring>& commandArguments) :
	pixelFormat_(FrameType::FORMAT_RGB24)
{
#if 0
	// Disable multi-core computation by forcing one CPU core
	Processor::get().forceCores(1);
#endif

	// Self-documentation (until named arguments get added)
	// No arguments => live video input, so need to specify -h or --help
	if (commandArguments.size() == 1 &&
		(commandArguments[0] == L"-h" || commandArguments[0] == L"--help"))
	{
		Log::info() << "usage: oceandemomessengercode source_image_or_video dimensions pixelFormat [no]loop";
		Log::info() << "example: LiveVideoId:0 1280x720 RGB24 loop";
		exit(0);
	}

	RandomI::initialize();

	// First, we register or load the media plugin(s)
	// If we have a shared runtime we simply load all media plugins available in a specific directory
	// If we have a static runtime we explicitly need to register all plugins we want to use (at compile time)

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

	// Although we could use the tracking capabilities via the devices interface we invoke the trackers directly to simplify the application
	// thus, we use the media plugins only
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
#endif

	// First, we get access to the frame medium that is intended to be used for the tracking

	if (commandArguments.size() > 0 && !commandArguments[0].empty())
	{
		const std::string argument = String::toAString(commandArguments[0]);

		// First we try to get an image sequence

		frameMedium_ = Media::Manager::get().newMedium(argument, Media::Medium::IMAGE_SEQUENCE);

		const Media::ImageSequenceRef imageSequence(frameMedium_);

		if (imageSequence)
		{
			// In the case we have an image sequence as input we want to process the images as fast as possible
			// (not with any specific fps number) so we use the explicit mode

			imageSequence->setMode(Media::ImageSequence::SM_EXPLICIT);
		}
		else
		{
			// Provided command argument seems to be something else but an image sequence
			// so now we try to get any possible medium

			frameMedium_ = Media::Manager::get().newMedium(argument);
		}
	}

	if (frameMedium_.isNull())
	{
		// If the user did not specify a medium, first we try to get a live video with id 1 (often an external web cam - not the builtin camera of a laptop)
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");
	}

	if (frameMedium_.isNull())
	{
		// If we could not get the device with id 1 we try to get the device with id 0
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0");
	}

	if (frameMedium_.isNull())
	{
		Platform::Utilities::showMessageBox("Error", "No valid input medium could be found!");

		// The device does not have an accessible live camera (or a necessary media plugin hasn't loaded successfully)
		return;
	}

	// Second, we check whether a desired frame dimension is specified for the input frame medium

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
	else
	{
		frameMedium_->setPreferredFrameDimension(1280u, 720u);
	}

	// Next, we check whether a specific pixel format for tracking is specified

	if (commandArguments.size() > 2 && !commandArguments[2].empty())
	{
		const std::string argument(String::toAString(commandArguments[2]));

		pixelFormat_ = FrameType::translatePixelFormat(argument);

		if (pixelFormat_ == FrameType::FORMAT_UNDEFINED || FrameType::numberPlanes(pixelFormat_) != 1u)
		{
			Platform::Utilities::showMessageBox("Information", "Invalid pixel format, got: \"" + argument + "\"\nHowever we proceed with the default value (RGB24).");
		}
	}

	// Last, we check whether the medium is intended to be looped (as long if the medium is finite)

	bool loopMedium = true;

	if (commandArguments.size() > 3 && !commandArguments[3].empty())
	{
		const std::string argument(String::toAString(commandArguments[3]));

		if (argument == "loop")
		{
			loopMedium = true;
		}
		else if (argument == "noloop")
		{
			loopMedium = false;
		}
		else
		{
			Platform::Utilities::showMessageBox("Information", "Invalid medium property, got: \"" + argument + "\"\nHowever we proceed with the default value (loop).");
		}
	}

	const Media::FiniteMediumRef finiteMedium(frameMedium_);

	if (finiteMedium)
	{
		finiteMedium->setLoop(loopMedium);
	}

	// We start the medium so that medium will deliver frames and wait for the first frame to be able to receive the matching camera calibration

	frameMedium_->start();
}

MessengerCodeWrapper::~MessengerCodeWrapper()
{
	// We do not release this instance, this should be done by the user before the application ends
}

void MessengerCodeWrapper::release()
{
	frameMedium_.release();

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		Media::DirectShow::unregisterDirectShowLibrary();
		Media::MediaFoundation::unregisterMediaFoundationLibrary();
		Media::WIC::unregisterWICLibrary();
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

bool MessengerCodeWrapper::detectAndDecode(Frame& frame, double& time, std::vector<std::string>& messages, bool* lastFrameReached)
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

	// We request the most recent frame from our input medium

	const FrameRef currentFrameRef = frameMedium_->frame();

	if (currentFrameRef.isNull())
	{
		return false;
	}

	const Frame& currentFrame = *currentFrameRef;

	// We only handle a frame once

	if (currentFrame.timestamp() == timestamp_)
	{
		return false;
	}

	timestamp_ = currentFrame.timestamp();

	const Media::ImageSequenceRef imageSequence(frameMedium_);

	// As we will need worker objects in several function calls we simply request one for the remaining function

	Frame currentFrameRGB;
	if (!CV::FrameConverter::Comfort::convert(currentFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, currentFrameRGB, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(currentFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (messengerCodePerformance_.measurements() % 20u == 0u)
	{
		messengerCodePerformance_.reset();
	}

	bool codeDetected = false;

	HighPerformanceStatistic::ScopedStatistic scopedPerformance(messengerCodePerformance_);

	CV::Detector::MessengerCodeDetector::DebugInformation debugInformation;
	const CV::Detector::MessengerCodeDetector::Codes codes = CV::Detector::MessengerCodeDetector::detectMessengerCodesWithDebugInformation(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), debugInformation, 0u, WorkerPool::get().scopedWorker()());

	scopedPerformance.release();

	debugInformation.draw(currentFrameRGB);

	OCEAN_SUPPRESS_UNUSED_WARNING(messages);

	codeDetected = !codes.empty();

	frame = std::move(currentFrameRGB);
	frame.setTimestamp(timestamp_);

	time = messengerCodePerformance_.average();

	if (imageSequence)
	{
		// In the case we use an image sequence as input we simply now activate the next frame
		// (as we use the explicit mode of the sequence)

		imageSequence->forceNextFrame();
	}

	return codeDetected;
}

MessengerCodeWrapper& MessengerCodeWrapper::operator=(MessengerCodeWrapper&& messengerCodeWrapper) noexcept
{
	if (this != &messengerCodeWrapper)
	{
		// Only one instance of this class may exist at the same time
		ocean_assert(frameMedium_.isNull());

		frameMedium_ = std::move(messengerCodeWrapper.frameMedium_);
		pixelFormat_ = messengerCodeWrapper.pixelFormat_;
		timestamp_ = messengerCodeWrapper.timestamp_;
	}

	return *this;
}
