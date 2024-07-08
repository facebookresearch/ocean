/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/barcodes/detector2d/Wrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/detector/barcodes/Barcode.h"
#include "ocean/cv/detector/barcodes/BarcodeDetector2D.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/File.h"
#include "ocean/io/Directory.h"

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

Wrapper::Wrapper(Wrapper&& barcodeDetector2DWrapper)
{
	*this = std::move(barcodeDetector2DWrapper);
}

Wrapper::Wrapper(const std::vector<std::wstring>& separatedCommandArguments)
{
#if defined(_WINDOWS)
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);
#endif

#if 0
	// Disable multi-core computation by forcing one CPU core
	Processor::get().forceCores(1);
#endif

	CommandArguments commandArguments("Demo of the barcode detector that takes as input one image sequence, a web camera, or a recording file");
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as input parameter");
	commandArguments.registerParameter("help", "h", "Showing this help output.");
	commandArguments.registerParameter("input", "i", "The input to be used for tracking, either a recording or an image sequence");
	commandArguments.registerParameter("mediumid", "s", "The index of the medium within the recording file in case a recording is used");
	commandArguments.registerParameter("fps", "f", "Optional number of frames per second the video of the processed input should be encoded with, range: [1, infinity)");
	commandArguments.registerParameter("video", "v", "Optional file name where a video of the processed input file will be stored. If not specified, will be ignored.");

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER
	Wrapper_registerExternalCommandArguments(commandArguments);
#endif

	commandArguments.parse(separatedCommandArguments);

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		exit(0);
	}

	RandomI::initialize();

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
		const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
		PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	#endif

	// Although we could use the tracking capabilities via the devices interface we invoke the trackers directly to simplify the application
	// thus, we use the media plugins only
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
#endif

	// First, we get access to the frame medium that is intended to be used for the tracking

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER
	devicePlayer_ = Wrapper_createExternalDevicePlayer(commandArguments);
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
				const Media::FrameMediumRefs frameMediums = devicePlayer_->frameMediums();

				int32_t mediumId = -1;
				if (commandArguments.hasValue("mediumid", mediumId, false) && mediumId >= 0)
				{
					if (size_t(mediumId) < frameMediums.size())
					{
						frameMedium_ = frameMediums[size_t(mediumId)];
					}
					else
					{
						Log::error() << "The medium id " << mediumId << " exceeds the number of available mediums " << frameMediums.size();
					}
				}
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

	if (frameMedium_.isNull())
	{
		std::string input;
		if (commandArguments.hasValue("input", input, false, 0u) && !input.empty())
		{
			frameMedium_ = Media::Manager::get().newMedium(input);

			// If we have a finite medium (e.g., a movie) we loop it

			const Media::FiniteMediumRef finiteMedium(frameMedium_);

			if (finiteMedium)
			{
				finiteMedium->setLoop(true);
			}
		}
		else
		{
			frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0");
		}
	}

	if (frameMedium_.isNull())
	{
		Log::error() << "Invalid input";
		return;
	}

	unsigned int framesPerSecond = 30u;

	Value fpsValue;
	if (commandArguments.hasValue("fps", &fpsValue, false) && fpsValue.isInt())
	{
		const int argument = fpsValue.intValue();

		if (argument < 1)
		{
			Platform::Utilities::showMessageBox("Error", "Camera indices must be >= 1");
			return;
		}

		framesPerSecond = (unsigned int)argument;
	}

	Value videoValue;
	if (commandArguments.hasValue("video", &videoValue, false) && videoValue.isString())
	{
		const std::string argument = videoValue.stringValue();

		IO::File outputFile(argument);

		movieRecorder_ = Media::Manager::get().newRecorder(Media::Recorder::MOVIE_RECORDER);

		if (movieRecorder_.isNull())
		{
			Log::error() << "Failed to create a recorder for the output!";
			exit(0);
		}

		if (outputFile.exists())
		{
			if (!outputFile.remove())
			{
				Log::error() << "The output \"" << outputFile() << "\" exists already and could not be deleted.";
				exit(0);
			}
		}
		else
		{
			const IO::Directory outputFileDirectory(outputFile);

			if (!outputFileDirectory.exists())
			{
				outputFileDirectory.create();
			}
		}

		movieRecorder_->setFilename(outputFile());
		movieRecorder_->setFrameFrequency(double(framesPerSecond));
		movieRecorder_->setFilenameSuffixed(false);
	}

	if (frameMedium_.isNull())
	{
		// If the user did not specify a medium, we try to get a live video (id 1 - often an external web cam, id 0 - often the built-in camera of a laptop)
		const std::vector<std::string> liveVideoIds =
		{
			"LiveVideoId:1",
			"LiveVideoId:0"
		};

		for (const std::string& liveVideoId : liveVideoIds)
		{
			frameMedium_ = Media::Manager::get().newMedium(liveVideoId);

			if (frameMedium_)
			{
				break;
			}
		}
	}

	if (frameMedium_.isNull())
	{
		Platform::Utilities::showMessageBox("Error", "Invalid number of input mediums!");

		return;
	}

	frameMedium_->setPreferredFrameDimension(1280u, 720u);
	frameMedium_->start();
}

Wrapper::~Wrapper()
{
	// We do not release this instance, this should be done by the user before the application ends
}

void Wrapper::release()
{
	frameMedium_.release();

	if (devicePlayer_ && devicePlayer_->isStarted())
	{
		devicePlayer_->stop();
	}

	devicePlayer_ = nullptr;

	if (movieRecorder_ && movieRecorder_->isRecording())
	{
		movieRecorder_->stop();
	}

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

bool Wrapper::detectAndDecode(Frame& outputFrame, double& time, std::vector<std::string>& messages, bool* lastFrameReached)
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

	if (lastFrameReached)
	{
		if (frameMedium_->stopTimestamp().isValid())
		{
			*lastFrameReached = true;
		}
	}

	// We request the most recent frames from our input mediums
	SharedAnyCamera anyCamera;
	FrameRef frameRef = frameMedium_->frame(&anyCamera);

	if (anyCamera == nullptr || !anyCamera->isValid())
	{
		// we still need to request the correct camera profile for our input medium
		// therefore, we need to know the dimensions of the input medium (the delivered frames respectively)

		if (frameRef.isNull())
		{
			frameRef = frameMedium_->frame();
		}

		if (frameRef.isNull())
		{
			// if we cannot extract the first frame within 5 seconds since we started the medium, something must be wrong

			if (frameMedium_->startTimestamp() + 5.0 < Timestamp(true))
			{
				Platform::Utilities::showMessageBox("Error", "Could not extract a valid frame from the input source!\nDefine a different source as input.");

				// we release the medium to ensure that we stop immediately the next time this function is called
				frameMedium_.release();
			}

			return false;
		}

		// the camera calibration manager will either provided the calibrated profile (if existing) or will provide a default profile

		anyCamera = std::make_shared<AnyCameraPinhole>(IO::CameraCalibrationManager::get().camera(frameMedium_->url(), frameRef->width(), frameRef->height(), nullptr, Numeric::deg2rad(60)));
	}

	//ocean_assert(anyCamera && anyCamera->isValid());

	if (frameRef.isNull())
	{
		return false;
	}

	// We handle each frame only once.
	if (frameRef->timestamp() == timestamp_)
	{
		return false;
	}

	timestamp_ = frameRef->timestamp();

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(*frameRef, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, true, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	CV::Detector::Barcodes::BarcodeDetector2D::Observations observations;
	FiniteLines2 scanlines;

	performance_.start();
		const CV::Detector::Barcodes::Barcodes barcodes = CV::Detector::Barcodes::BarcodeDetector2D::detectBarcodes(yFrame, CV::Detector::Barcodes::BarcodeDetector2D::DF_ENABLE_EVERYTHING, CV::Detector::Barcodes::BarcodeTypeSet(), 50u, &observations, &scanlines);
	performance_.stop();

	// Draw the results

	Frame resultFrame;
	if (!CV::FrameConverter::Comfort::convert(*frameRef, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, resultFrame, /* forceCopy */ true, WorkerPool::get().scopedWorker()()))
	{
	 	ocean_assert(false && "This should never happen!");
	 	return false;
	}

	// TODO Draw the results
	for (const FiniteLine2& scanline : scanlines)
	{
		CV::Canvas::line<1u>(resultFrame, scanline, CV::Canvas::red(resultFrame.pixelFormat()));
	}

	for (const CV::Detector::Barcodes::BarcodeDetector2D::Observation& observation : observations)
	{
		CV::Canvas::line<3u>(resultFrame, observation.location(), CV::Canvas::yellow(resultFrame.pixelFormat()));
	}

	std::vector<std::string> localMessages;

	for (const CV::Detector::Barcodes::Barcode& barcode : barcodes)
	{
		ocean_assert(barcode.isValid());

		std::string message = barcode.data() + " (" + CV::Detector::Barcodes::Barcode::translateBarcodeType(barcode.barcodeType()) + ")";

		localMessages.emplace_back(std::move(message));
	}

	if (movieRecorder_)
	{
		// Add the result frame to the move recorder

		if (!movieRecorder_->frameType().isValid())
		{
			movieRecorder_->setPreferredFrameType(resultFrame.frameType());
			movieRecorder_->start();
		}

		Frame recorderFrame;
		if (movieRecorder_->lockBufferToFill(recorderFrame, /* respectFrameFrequency */ false))
		{
			CV::FrameConverter::Comfort::convertAndCopy(resultFrame, recorderFrame);

			movieRecorder_->unlockBufferToFill();
		}
	}

	outputFrame = std::move(resultFrame);
	outputFrame.setTimestamp(timestamp_);

	messages = std::move(localMessages);

	time = performance_.average();

	if (performance_.measurements() % 20u == 0u)
	{
	 	performance_.reset();
	}

	return true;
}

Wrapper& Wrapper::operator=(Wrapper&& barcodeDetector2DWrapper)
{
	if (this != &barcodeDetector2DWrapper)
	{
		// Only one instance of this class may exist at the same time
		ocean_assert(frameMedium_.isNull());

		devicePlayer_ = std::move(barcodeDetector2DWrapper.devicePlayer_);

		frameMedium_ = std::move(barcodeDetector2DWrapper.frameMedium_);
		timestamp_ = barcodeDetector2DWrapper.timestamp_;
		performance_ = barcodeDetector2DWrapper.performance_;
	}

	return *this;
}
