/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/qrcodes/detector2d/Detector2DWrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/LegacyQRCodeDetector2D.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/io/File.h"

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

Detector2DWrapper::Detector2DWrapper(Detector2DWrapper&& detector2dWrapper)
{
	*this = std::move(detector2dWrapper);
}

Detector2DWrapper::Detector2DWrapper(const std::vector<std::wstring>& separatedCommandArguments)
{
#if defined(_WINDOWS)
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);
#endif

#if 0
	// Disable multi-core computation by forcing one CPU core
	Processor::get().forceCores(1);
#endif

	CommandArguments commandArguments("Demo of the QR code detector that takes as input images sequences, web cameras, or recording files");
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as input parameter");
	commandArguments.registerParameter("help", "h", "Showing this help output.");
	commandArguments.registerParameter("input", "i", "Input to be used for tracking, either a recording file or an image sequence");
	commandArguments.registerParameter("resolution", "r", "Resolution of the input, e.g. \"1280x720\"");
	commandArguments.registerParameter("loop", "l", "If set, will start the input again when it reaches the end");
	commandArguments.registerParameter("olddetector", "d", "If set, the old QR code detector will be used");

	commandArguments.parse(separatedCommandArguments);

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
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

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER
	devicePlayer_ = Detector2DWrapper_createExternalDevicePlayer(commandArguments);
#endif

	if (devicePlayer_)
	{
		if (devicePlayer_->start(Devices::DevicePlayer::SPEED_USE_STOP_MOTION))
		{
			if (devicePlayer_->frameMediums().empty())
			{
				Log::error() << "The recording does not contain frame mediums";
			}
			else
			{
				// Only select the first medium and ignore all others
				frameMedium_ = devicePlayer_->frameMediums().front();
				ocean_assert(frameMedium_);
			}
		}
		else
		{
			Log::error() << "Failed to start the recording";
		}

		if (frameMedium_.isNull())
		{
			Log::error() << "Invalid recording input";
			return;
		}
	}

	std::string input;
	if (commandArguments.hasValue("input", input, false, 0u) && !input.empty())
	{
		// Try to get an image sequence

		frameMedium_ = Media::Manager::get().newMedium(input, Media::Medium::IMAGE_SEQUENCE);

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

			frameMedium_ = Media::Manager::get().newMedium(input);
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

		// The device does not have an accessible live camera (or a necessary media plugin has not loaded successfully)
		return;
	}

	// Second, we check whether a desired frame dimension is specified for the input frame medium

	std::string resolution;
	if (commandArguments.hasValue("resolution", resolution, false, 0u))
	{
		if (resolution == "320x240")
		{
			frameMedium_->setPreferredFrameDimension(320u, 240u);
		}
		else if (resolution == "640x480")
		{
			frameMedium_->setPreferredFrameDimension(640u, 480u);
		}
		else if (resolution == "1280x720")
		{
			frameMedium_->setPreferredFrameDimension(1280u, 720u);
		}
		else if (resolution == "1920x1080")
		{
			frameMedium_->setPreferredFrameDimension(1920u, 1080u);
		}
	}
	else
	{
		frameMedium_->setPreferredFrameDimension(1280u, 720u);
	}

	// Last, we check whether the medium is intended to be looped (as long if the medium is finite)

	const bool loopMedium = commandArguments.hasValue("loop");

	const Media::FiniteMediumRef finiteMedium(frameMedium_);

	if (finiteMedium)
	{
		finiteMedium->setLoop(loopMedium);
	}

	useOldDetector_ = commandArguments.hasValue("olddetector");

	// We start the medium so that medium will deliver frames and wait for the first frame to be able to receive the matching camera calibration

	frameMedium_->start();
}

Detector2DWrapper::~Detector2DWrapper()
{
	// We do not release this instance, this should be done by the user before the application ends
}

void Detector2DWrapper::release()
{
	frameMedium_.release();

	if (devicePlayer_ && devicePlayer_->isStarted())
	{
		devicePlayer_->stop();
	}

	devicePlayer_ = nullptr;

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

bool Detector2DWrapper::detectAndDecode(Frame& outputFrame, double& time, std::vector<std::string>& messages, bool* lastFrameReached)
{
	messages.clear();

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

	SharedAnyCamera sharedAnyCamera;
	FrameRef frameRef;

	if (devicePlayer_ != nullptr && devicePlayer_->isValid())
	{
		const Timestamp frameTimestamp = devicePlayer_->playNextFrame();

		frameRef = frameMedium_->frame(frameTimestamp, &sharedAnyCamera);
	}
	else
	{
		frameRef = frameMedium_->frame(&sharedAnyCamera);
	}

	if (frameRef.isNull())
	{
		return false;
	}

	// We only handle a frame once

	if (frameRef->timestamp() == timestamp_)
	{
		return false;
	}

	const Frame& frame = *frameRef;

	timestamp_ = frame.timestamp();

	const Media::ImageSequenceRef imageSequence(frameMedium_);

	// As we will need worker objects in several function calls we simply request one for the remaining function

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, true, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, true, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	ocean_assert(yFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	if (performance_.measurements() % 20u == 0u)
	{
		performance_.reset();
	}

	CV::Detector::QRCodes::QRCodes codes;

	if (useOldDetector_)
	{
		HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance_);

		ocean_assert(yFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);
		CV::Detector::QRCodes::LegacyQRCodeDetector2D::Observations observations;
		codes = CV::Detector::QRCodes::LegacyQRCodeDetector2D::detectQRCodes(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), WorkerPool::get().scopedWorker()(), CV::Detector::QRCodes::LegacyQRCodeDetector2D::DM_STANDARD, &observations);

		scopedPerformance.release();

		CV::Detector::QRCodes::Utilities::drawObservations(rgbFrame, observations, codes);
	}
	else
	{
		HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance_);

		CV::Detector::QRCodes::QRCodeDetector2D::Observations observations;
		if (sharedAnyCamera)
		{
			codes = CV::Detector::QRCodes::QRCodeDetector2D::detectQRCodes(*sharedAnyCamera, yFrame, &observations, WorkerPool::get().scopedWorker()());
		}
		else
		{
			codes = CV::Detector::QRCodes::QRCodeDetector2D::detectQRCodes(yFrame, &observations, WorkerPool::get().scopedWorker()(), &sharedAnyCamera);
			ocean_assert(sharedAnyCamera && sharedAnyCamera->isValid());
		}

		scopedPerformance.release();

		CV::Detector::QRCodes::Utilities::drawObservations(*sharedAnyCamera, rgbFrame, observations, codes);
	}

	for (const CV::Detector::QRCodes::QRCode& code : codes)
	{
		messages.push_back(code.dataString());
	}

	outputFrame = std::move(rgbFrame);
	outputFrame.setTimestamp(timestamp_);

	time = performance_.average();

	if (imageSequence)
	{
		// In the case we use an image sequence as input we simply now activate the next frame
		// (as we use the explicit mode of the sequence)

		imageSequence->forceNextFrame();
	}

	return codes.empty() == false;
}

Detector2DWrapper& Detector2DWrapper::operator=(Detector2DWrapper&& detector2dWrapper)
{
	if (this != &detector2dWrapper)
	{
		// Only one instance of this class may exist at the same time
		ocean_assert(frameMedium_.isNull());

		useOldDetector_ = detector2dWrapper.useOldDetector_;

		devicePlayer_ = std::move(detector2dWrapper.devicePlayer_);

		frameMedium_ = std::move(detector2dWrapper.frameMedium_);
		timestamp_ = detector2dWrapper.timestamp_;
		performance_ = detector2dWrapper.performance_;
	}

	return *this;
}
