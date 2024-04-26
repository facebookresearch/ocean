// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/detector/qrcodes/detector3d/Detector3DWrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/LegacyQRCodeDetector2D.h"
#include "ocean/cv/detector/qrcodes/QRCodeDetector2D.h"
#include "ocean/cv/detector/qrcodes/QRCodeDetector3D.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/io/File.h"
#include "ocean/io/Directory.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/ImageSequence.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#include "metaonly/ocean/media/vrs/VRS.h"

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

Detector3DWrapper::Detector3DWrapper(Detector3DWrapper&& detector3DWrapper)
{
	*this = std::move(detector3DWrapper);
}

Detector3DWrapper::Detector3DWrapper(const std::vector<std::wstring>& separatedCommandArguments)
{
#if defined(_WINDOWS)
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);
#endif

#if 0
	// Disable multi-core computation by forcing one CPU core
	Processor::get().forceCores(1);
#endif

	CommandArguments commandArguments("Demo of the QR code detector that takes as input images sequences, web cameras, or VRS files");
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as input parameter");
	commandArguments.registerParameter("help", "h", "Showing this help output.");
	commandArguments.registerParameter("input", "i", "Input to be used for tracking, either a VRS file or an image sequence");
	commandArguments.registerParameter("camera0", "c0", "The index of the first camera stream from the VRS file that will be used");
	commandArguments.registerParameter("camera1", "c1", "The index of the first camera stream from the VRS file that will be used");
	commandArguments.registerParameter("fps", "f", "Optional number of frames per second the video of the processed input should be encoded with, range: [1, infinity)");
	commandArguments.registerParameter("video", "v", "Optional file name where a video of the processed input file will be stored. If not specified, will be ignored.");

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

	Value inputValue;
	if (commandArguments.hasValue("input", &inputValue, false, 0u) && inputValue.isString())
	{
		const std::string argument = inputValue.stringValue();

		IO::File fileArgument(argument);

		if (fileArgument.exists() && fileArgument.extension() == "vrs")
		{
			devicePlayer_ = std::make_shared<Devices::VRS::VRSDevicePlayer>();

			if (!devicePlayer_->initialize(fileArgument()) || !devicePlayer_->start(/* speed */ 0.0f))
			{
				Log::error() << "Failed to load input VRS file";
			}
			else
			{
				if (devicePlayer_->frameMediums().size() < 2)
				{
					Log::error() << "VRS files does not contain enough frame mediums";
				}
				else
				{
					frameMediums_ = devicePlayer_->frameMediums();

#if defined(OCEAN_DEBUG)
					for (const Media::FrameMediumRef& frameMedium : frameMediums_)
					{
						ocean_assert(!frameMedium.isNull());
					}
#endif

					// Extract all device poses from the VRS file and use it later to retrieve the pose for each frame.
					if (!Media::VRS::extractValuesFromVRS(fileArgument(), Media::VRS::translateRecordableTypeid("PoseRecordableClass"), "world_T_device", world_T_devices_))
					{
						Platform::Utilities::showMessageBox("Error", "VRS files does not contain device transformations");
						return;
					}
				}
			}
		}
	}

	Value camera0Value;
	if (commandArguments.hasValue("camera0", &camera0Value, false) && camera0Value.isInt())
	{
		const int argument = camera0Value.intValue();

		if (argument >= 0)
		{
			if (size_t(argument) < frameMediums_.size())
			{
				vrsCameraIndex0_ = (unsigned int)argument;
			}
			else
			{
				Platform::Utilities::showMessageBox("Error", "The camera index exceeds the number of available streams");
				return;
			}
		}
		else
		{
			Platform::Utilities::showMessageBox("Error", "Camera indices must be >= 0");
			return;
		}
	}

	Value camera1Value;
	if (commandArguments.hasValue("camera1", &camera1Value, false) && camera1Value.isInt())
	{
		const int argument = camera1Value.intValue();

		if (argument >= 0)
		{
			if (size_t(argument) < frameMediums_.size())
			{
				vrsCameraIndex1_ = (unsigned int)argument;
			}
			else
			{
				Platform::Utilities::showMessageBox("Error", "The camera index exceeds the number of available streams");
				return;
			}
		}
		else
		{
			Platform::Utilities::showMessageBox("Error", "Camera indices must be >= 0");
			return;
		}
	}

	if (vrsCameraIndex0_ == vrsCameraIndex1_)
	{
		Platform::Utilities::showMessageBox("Error", "The selected cameras must have different indices");
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

	if (frameMediums_.size() < 2)
	{
		Platform::Utilities::showMessageBox("Error", "Invalid number of input mediums!");

		return;
	}

	// We start the medium so that medium will deliver frames and wait for the first frame to be able to receive the matching camera calibration

	for (Media::FrameMediumRef& frameMedium : frameMediums_)
	{
		frameMedium->start();
	}
}

Detector3DWrapper::~Detector3DWrapper()
{
	// We do not release this instance, this should be done by the user before the application ends
}

void Detector3DWrapper::release()
{
	for (Media::FrameMediumRef& frameMedium : frameMediums_)
	{
		frameMedium.release();
	}

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

bool Detector3DWrapper::detectAndDecode(Frame& outputFrame, double& time, std::vector<std::string>& messages, bool* lastFrameReached)
{
	messages.clear();

	if (lastFrameReached)
	{
		*lastFrameReached = false;
	}

	if (frameMediums_.size() < 2)
	{
		return false;
	}

	if (lastFrameReached)
	{
		for (const Media::FrameMediumRef& frameMedium : frameMediums_)
		{
			if (frameMedium->stopTimestamp().isValid())
			{
				*lastFrameReached = true;
				break;
			}
		}
	}

	// We request the most recent frames from our input mediums
	Timestamp frameTimestamp(false);

	SharedAnyCameras anyCameras;
	FrameRefs frameRefs;
	HomogenousMatricesD4 device_T_camerasD;

	anyCameras.reserve(frameMediums_.size());
	frameRefs.reserve(frameMediums_.size());
	device_T_camerasD.reserve(frameMediums_.size());

	if (devicePlayer_ != nullptr && devicePlayer_->isValid())
	{
		frameTimestamp = devicePlayer_->playNextFrame();

		for (const Media::FrameMediumRef& frameMedium : frameMediums_)
		{
			SharedAnyCamera anyCamera;
			FrameRef frameRef = frameMedium->frame(frameTimestamp, &anyCamera);

			if (!frameRef.isNull() && anyCamera != nullptr)
			{
				ocean_assert(frameRef->isValid());
				ocean_assert(anyCamera->isValid());

				frameRefs.emplace_back(std::move(frameRef));
				anyCameras.emplace_back(std::move(anyCamera));
				device_T_camerasD.emplace_back(frameMedium->device_T_camera());
			}
		}
	}

	if (frameRefs.size() < 2 || frameRefs.size() != anyCameras.size())
	{
		return false;
	}

	if (frameRefs.size() > 2)
	{
		const unsigned int cameraIndex0 = vrsCameraIndex0_;
		const unsigned int cameraIndex1 = vrsCameraIndex1_;

		ocean_assert(cameraIndex0 != cameraIndex1);
		ocean_assert(cameraIndex0 < (unsigned int)anyCameras.size());
		ocean_assert(cameraIndex1 < (unsigned int)anyCameras.size());

		SharedAnyCameras selectedAnyCameras = {anyCameras[cameraIndex0], anyCameras[cameraIndex1]};
		FrameRefs selectedFrameRefs = {frameRefs[cameraIndex0], frameRefs[cameraIndex1]};
		HomogenousMatricesD4 selectedDevice_T_cameras = {device_T_camerasD[cameraIndex0], device_T_camerasD[cameraIndex1]};

		anyCameras = std::move(selectedAnyCameras);
		frameRefs = std::move(selectedFrameRefs);
		device_T_camerasD = std::move(selectedDevice_T_cameras);
	}

	HomogenousMatrixD4 world_T_deviceD;
	if (!world_T_devices_.sample(double(frameRefs.front()->timestamp()), world_T_deviceD))
	{
		Log::error() << "No transformation world_T_device is available in the sample map";
		return false;
	}

	// We handle each frame only once.

	// TODO - Some of the frames from Arcata sometimes have slightly different time stamps
	#if defined(OCEAN_DEBUG)
		for (const FrameRef& frameRef : frameRefs)
		{
			// All frames must be synchronized.
			if (frameRef->timestamp() != frameRefs.front()->timestamp())
			{
				Log::warning() << "Deviating timestamps - frames not fully synchronized";
			}
		}
	#endif

	if (frameRefs.front()->timestamp() == timestamp_)
	{
		return false;
	}

	timestamp_ = frameRefs.front()->timestamp();

#if defined(OCEAN_DEBUG)
	for (const FrameRef& frameRef : frameRefs)
	{
		// All frames must use the same pixel format.
		ocean_assert(frameRef->pixelFormat() == frameRefs.front()->pixelFormat());
	}
#endif

	Frames yFrames;

	yFrames.reserve(frameRefs.size());

	for (const FrameRef& frameRef : frameRefs)
	{
		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(*frameRef, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, true, WorkerPool::get().scopedWorker()()))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		yFrames.emplace_back(std::move(yFrame));
	}

	const unsigned int frameWidth = frameRefs.front()->width();
	const unsigned int frameHeight= frameRefs.front()->height();

	const unsigned int resultFrameWidth = frameWidth * (unsigned int)(frameRefs.size());
	const unsigned int resultFrameHeight = frameHeight;

	Frame resultFrame(FrameType(resultFrameWidth, resultFrameHeight, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

	const HomogenousMatrix4 world_T_device(world_T_deviceD);
	const HomogenousMatrices4 device_T_cameras = HomogenousMatrix4::matrices2matrices(device_T_camerasD);

	CV::Detector::QRCodes::QRCodes codes;
	HomogenousMatrices4 world_T_codes;
	Scalars codeSizes;

	if (!CV::Detector::QRCodes::QRCodeDetector3D::detectQRCodesWithPyramids(anyCameras, yFrames, world_T_device, device_T_cameras, codes, world_T_codes, codeSizes, WorkerPool::get().scopedWorker()()))
	{
		Log::error() << "QR code detection failed.";

		return false;
	}

	unsigned int numberDetectedCodes = 0u;

	for (unsigned int i = 0u; i < (unsigned int)(frameRefs.size()); ++i)
	{
		ocean_assert(size_t(i) < anyCameras.size());
		ocean_assert(size_t(i) < yFrames.size());

		const SharedAnyCamera anyCamera = anyCameras[i];
		const Frame& yFrame = yFrames[i];

		ocean_assert_and_suppress_unused(yFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT, yFrame);

		Frame rgbFrame = resultFrame.subFrame(i * frameWidth, 0u, frameWidth, frameHeight);

		if (!CV::FrameConverter::Comfort::convert(*frameRefs[i], FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, /* forceCopy */ true, WorkerPool::get().scopedWorker()()))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		ocean_assert(rgbFrame.data<uint8_t>() == resultFrame.subFrame(i * frameWidth, 0u, frameWidth, frameHeight).data<uint8_t>() && "The frame has been reinitialized.");

		if (performance_.measurements() % 20u == 0u)
		{
			performance_.reset();
		}

		// Draw the current code
		const HomogenousMatrix4 world_T_camera = world_T_device * device_T_cameras[i];

		// TODO Draw a coordinate system as well, https://fburl.com/code/rcd5l9iz

		for (size_t j = 0; j < codes.size(); ++j)
		{
			const CV::Detector::QRCodes::QRCode& code = codes[j];

			const unsigned int version = code.version();

			const HomogenousMatrix4& flippedCamera_T_code = PinholeCamera::standard2InvertedFlipped(world_T_codes[j].inverted() * world_T_camera);

			const Vector3 translation = flippedCamera_T_code.translation();
			Log::info() << "distance code to camera: " << String::toAString(translation.length(), 5u);

			const unsigned int modulesPerSide = CV::Detector::QRCodes::QRCode::modulesPerSide(version);

			const CV::Detector::QRCodes::Utilities::CoordinateSystem coordinateSystem(version, codeSizes[j] * Scalar(0.5));

			for (unsigned int yModule = 0u; yModule < modulesPerSide; ++yModule)
			{
				const Scalar y = coordinateSystem.convertCodeSpaceToObjectSpaceY(Scalar(yModule) + Scalar(0.5));

				for (unsigned int xModule = 0u; xModule < modulesPerSide; ++xModule)
				{
					const Scalar x = coordinateSystem.convertCodeSpaceToObjectSpaceX(Scalar(xModule) + Scalar(0.5));

					const Vector2 imagePoint = anyCamera->projectToImageIF(flippedCamera_T_code, Vector3(x, y, Scalar(0)));

					if (anyCamera->isInside(imagePoint, Scalar(0.5)))
					{
						CV::Canvas::point<3u>(rgbFrame, imagePoint, CV::Canvas::green(rgbFrame.pixelFormat()));
						CV::Canvas::point<1u>(rgbFrame, imagePoint, CV::Canvas::red(rgbFrame.pixelFormat()));
					}
				}
			}
		}
	}

	if (movieRecorder_)
	{
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

	time = performance_.average();

	return numberDetectedCodes != 0u;
}

Detector3DWrapper& Detector3DWrapper::operator=(Detector3DWrapper&& detector3DWrapper)
{
	if (this != &detector3DWrapper)
	{
		// Only one instance of this class may exist at the same time
		ocean_assert(frameMediums_.empty());

		devicePlayer_ = std::move(detector3DWrapper.devicePlayer_);
		frameMediums_ = std::move(detector3DWrapper.frameMediums_);
		world_T_devices_ = std::move(detector3DWrapper.world_T_devices_);
		timestamp_ = detector3DWrapper.timestamp_;
		performance_ = detector3DWrapper.performance_;
		movieRecorder_ = std::move(detector3DWrapper.movieRecorder_);
	}

	return *this;
}
