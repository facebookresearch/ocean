// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/tracking/oculustags/oculustagtracker/OculusTagTrackerWrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"
#include "ocean/cv/Canvas.h"

#include "ocean/io/Directory.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/media/Manager.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#include "ocean/tracking/oculustags/OculusTagDebugElements.h"
#include "ocean/tracking/oculustags/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/wic/WIC.h"
		#include "ocean/media/mediafoundation/MediaFoundation.h"
	#elif defined(__APPLE__)
		#include "ocean/media/avfoundation/AVFoundation.h"
		#include "ocean/media/imageio/ImageIO.h"
	#endif
#endif

#ifdef OCEAN_USE_DEVICES_VRS
	#include "metaonly/ocean/media/vrs/VRS.h"
#endif

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED) && defined(_WINDOWS)
	#include "ocean/platform/win/Utilities.h"
#endif

#include <unordered_map>

using namespace Tracking::OculusTags;

OculusTagTrackerWrapper::OculusTagTrackerWrapper() :
	frameTimestamp_(false),
	frameCounter_(0u)
{
	// nothing to do here
}

OculusTagTrackerWrapper::OculusTagTrackerWrapper(OculusTagTrackerWrapper&& oculusTagTrackerWrapper) noexcept
{
	*this = std::move(oculusTagTrackerWrapper);
}

OculusTagTrackerWrapper::OculusTagTrackerWrapper(const std::vector<std::wstring>& separatedCommandArguments) :
	OculusTagTrackerWrapper()
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
		Media::WIC::registerWICLibrary();
		Media::MediaFoundation::registerMediaFoundationLibrary();
	#elif defined(__APPLE__)
		Media::AVFoundation::registerAVFLibrary();
		Media::ImageIO::registerImageIOLibrary();
	#endif
	#ifdef OCEAN_USE_MEDIA_VRS
		Media::VRS::registerVRSLibrary();
	#endif
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());

	// although we could use the tracking capabilities via the devices interface we invoke the trackers directly to simplify the application
	// thus, we use the media plugins only
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
#endif

	CommandArguments commandArguments;
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as input parameter");
	commandArguments.registerParameter("input", "i", "Input to be used for tracking, either a VRS file or a directory with image sequences and transformations file");
	commandArguments.registerParameter("poses", "p", "Optional explicit poses file in case the VRS files does not contain any HMD poses");
	commandArguments.registerParameter("normal-speed", "ns", "If set, will replay the input at normal speed. Otherwise it will use stop-motion.");
	commandArguments.registerParameter("fps", "f", "Optionally set the FPS of the output video.");
	commandArguments.registerParameter("output", "o", "Optional file name where a video with a visualization of the results will be stored");

	commandArguments.parse(separatedCommandArguments);

	if (commandArguments.hasValue("normal-speed"))
	{
		enableStopMotionReplay_ = false;
	}

	double framesPerSecond = 30.0;

	Value fpsValue;
	if (commandArguments.hasValue("fps", &fpsValue, false, 0u) && fpsValue.isInt() && fpsValue.intValue() > 0)
	{
		framesPerSecond = double(fpsValue.intValue());
	}

	Value outputValue;
	if (commandArguments.hasValue("output", &outputValue, false, 0u) && outputValue.isString())
	{
		const IO::File outputFile(outputValue.stringValue());

		if (outputFile.exists())
		{
			if (!outputFile.remove())
			{
				Log::error() << "The output \"" << outputFile() << "\" exists already and could not be deleted - skipping...";
				exit(0);
			}
		}

		movieRecorder_ = Media::Manager::get().newRecorder(Media::Recorder::MOVIE_RECORDER);

		if (movieRecorder_.isNull())
		{
			Log::error() << "Failed to create a recorder for the output!";
			exit(0);
		}

		movieRecorder_->setFilename(outputFile());
		movieRecorder_->setFrameFrequency(framesPerSecond);
		movieRecorder_->setFilenameSuffixed(false);
	}

	Value inputValue;
	if ((commandArguments.hasValue("input", &inputValue, false, 0u) && inputValue.isString()))
	{
		const std::string argument = inputValue.stringValue();

		const IO::File fileArgument(argument);

		if (fileArgument.exists())
		{
			if (fileArgument.extension() == "vrs")
			{
#ifdef OCEAN_USE_DEVICES_VRS

				devicePlayer_ = std::make_shared<Devices::VRS::VRSDevicePlayer>();

				if (!devicePlayer_->initialize(fileArgument()) || !devicePlayer_->start(enableStopMotionReplay_ ? 0.0f : -1.0f))
				{
					Log::error() << "Failed to load input VRS file";
				}
				else
				{
					if (devicePlayer_->frameMediums().empty())
					{
						Log::error() << "VRS files does not contain a frame medium";
					}
					else
					{
						frameMediumRefs_ = devicePlayer_->frameMediums();

#ifdef OCEAN_DEBUG
						for (Media::FrameMediumRef& frameMediumRef : frameMediumRefs_)
						{
							ocean_assert(frameMediumRef);
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

#else // OCEAN_USE_DEVICES_VRS

				Platform::Utilities::showMessageBox("Error", "This build has no VRS support.");
				exit(0);

#endif // OCEAN_USE_DEVICES_VRS
			}
		}
	}

	if (frameMediumRefs_.empty())
	{
		Platform::Utilities::showMessageBox("Error", "Failed to acquire the image data!");
		exit(0);
	}

	for (Media::FrameMediumRef& frameMediumRef : frameMediumRefs_)
	{
		frameMediumRef->start();
	}
}

OculusTagTrackerWrapper::~OculusTagTrackerWrapper()
{
	// nothing to do here
}

void OculusTagTrackerWrapper::release()
{
	if (movieRecorder_)
	{
		movieRecorder_->stop();
	}

	frameMediumRefs_.clear();

#ifdef OCEAN_DEBUG
	for (Media::FrameMediumRef& frameMediumRef : frameMediumRefs_)
	{
		ocean_assert(!frameMediumRef.isNull());
	}
#endif

#ifdef OCEAN_USE_DEVICES_VRS
	if (devicePlayer_ && devicePlayer_->isStarted())
	{
		devicePlayer_->stop();
	}

	devicePlayer_ = nullptr;
#endif

#ifdef OCEAN_RUNTIME_STATIC
	#ifdef OCEAN_USE_MEDIA_VRS
		Media::VRS::unregisterVRSLibrary();
	#endif
	#if defined(_WINDOWS)
		Media::MediaFoundation::unregisterMediaFoundationLibrary();
		Media::WIC::unregisterWICLibrary();
	#elif defined(__APPLE__)
		Media::ImageIO::unregisterImageIOLibrary();
		Media::AVFoundation::unregisterAVFLibrary();
	#endif
#else
	PluginManager::get().release();
#endif
}

bool OculusTagTrackerWrapper::trackNewFrame(Frame& resultFrame, double& time)
{
	if (frameMediumRefs_.size() < 2)
	{
		ocean_assert(false && "The input must have two or more, synchronized cameras.");
		return false;
	}

	if (enableStopMotionReplay_)
	{
#ifdef OCEAN_USE_DEVICES_VRS
		devicePlayer_->playNextFrame();
#endif
	}

	FrameRefs frameRefs;
	SharedAnyCameras anyCameras;

	bool timedOut = false;
	if (!Media::FrameMedium::syncedFrames(frameMediumRefs_, frameTimestamp_, frameRefs, anyCameras, 2u /*waitTime*/, &timedOut))
	{
		if (timedOut)
		{
			Log::warning() << "Failed to access synced camera frames for time stamp";
		}

		return false;
	}

	ocean_assert(anyCameras.size() >= 2);
	ocean_assert(anyCameras.size() == frameRefs.size());

#ifdef OCEAN_DEBUG
	for (size_t iCamera = 0; iCamera < anyCameras.size(); ++iCamera)
	{
		ocean_assert(anyCameras[iCamera] && anyCameras[iCamera]->isValid());
		ocean_assert(frameRefs[iCamera] && frameRefs[iCamera]->isValid());
		ocean_assert(anyCameras[iCamera]->width() == frameRefs[iCamera]->width() && anyCameras[iCamera]->height() == frameRefs[iCamera]->height());
	}
#endif

	HomogenousMatrices4 device_T_cameras;

	for (Media::FrameMediumRef& frameMediumRef : frameMediumRefs_)
	{
		ocean_assert(frameMediumRef && frameMediumRef->device_T_camera().isValid());
		device_T_cameras.emplace_back(frameMediumRef->device_T_camera());
	}

	ocean_assert(anyCameras.size() == device_T_cameras.size());

	const Timestamp timestamp = frameRefs.front()->timestamp();

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED)
	OculusTagDebugElements::get().activateElement(OculusTagDebugElements::EI_DETECTOR_RECTIFIED_TAG);
	OculusTagDebugElements::get().activateElement(OculusTagDebugElements::EI_TRACKER_RECTIFIED_TAG);
#endif

	if (timestamp == frameTimestamp_)
	{
		return false;
	}

	frameTimestamp_ = timestamp;

	// Only a stereo pair is required. This is an admittedly rather crude heuristic to select
	// cameras and how to rotate the frames for visualization later.
	size_t cameraIndex0 = (size_t)(-1);
	size_t cameraIndex1 = (size_t)(-1);

	bool rotateFrames = false;
	bool rotateClockwise = false;

	{
		const unsigned int width = frameRefs.front()->width();
		const unsigned int height = frameRefs.front()->height();

		if (width == 640u && height == 480u)
		{
			// Quest2
			cameraIndex0 = 1;
			cameraIndex1 = 2;

			rotateFrames = true;
			rotateClockwise = false;
		}
		else if (width == 1280u && height == 1024u)
		{
			// Quest Pro (Glacier)
			cameraIndex0 = 0;
			cameraIndex1 = 1;

			rotateFrames = true;
			rotateClockwise = true;
		}
		else
		{
			ocean_assert(false && "Never be here!");
			return false;
		}
	}

	if (cameraIndex0 >= anyCameras.size() || cameraIndex1 >= anyCameras.size())
	{
		ocean_assert(false && "Never be here!");
		return false;
	}

	HomogenousMatrixD4 world_T_deviceD;
	if (!world_T_devices_.sample(double(frameRefs.front()->timestamp()), world_T_deviceD))
	{
		Log::error() << "No transformation world_T_device is available in the sample map";

		return false;
	}

	ocean_assert(world_T_deviceD.isValid());

	const HomogenousMatrix4 world_T_device(world_T_deviceD);

	Frames rgbFrames;
	Frames yFrames;

	rgbFrames.reserve(frameRefs.size());
	yFrames.reserve(frameRefs.size());

	for (size_t iFrame = 0; iFrame < frameRefs.size(); ++iFrame)
	{
		const Frame& frame = *frameRefs[iFrame];
		ocean_assert(frame.pixelOrigin() == Frame::ORIGIN_UPPER_LEFT);

		Frame rgbFrame;
		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, rgbFrame, /* forceCopy */ true, WorkerPool::get().scopedWorker()()))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		rgbFrames.emplace_back(std::move(rgbFrame));

		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, yFrame, /* forceCopy */ false, WorkerPool::get().scopedWorker()()))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		yFrames.emplace_back(std::move(yFrame));
	}

	if (performance_.measurements() % 20u == 0u)
	{
		performance_.reset();
	}

	OculusTags trackedTags;
	bool trackerResult = false;

	if (++frameCounter_ >= 10u) // we skip the first frames to avoid invalid/inaccurate camera poses
	{
		performance_.start();
		trackerResult = oculusTagTracker_.trackTagsStereo(*anyCameras[cameraIndex0], *anyCameras[cameraIndex1], yFrames[0], yFrames[1], world_T_device, device_T_cameras[0], device_T_cameras[1], trackedTags);
		performance_.stop();
	}

	const OculusTagTracker::TrackedTagMap& trackedTagMap = oculusTagTracker_.trackedTagMap();

	for (const OculusTagTracker::TrackedTagMap::value_type& iterTrackedtag : trackedTagMap)
	{
		if (iterTrackedtag.second.trackingState_ == OculusTagTracker::TS_TRACKING || iterTrackedtag.second.trackingState_ == OculusTagTracker::TS_NEW_DETECTION)
		{
			const uint8_t* color = iterTrackedtag.second.trackingState_ == OculusTagTracker::TS_TRACKING ? CV::Canvas::yellow(rgbFrames[cameraIndex0].pixelFormat()) : CV::Canvas::red(rgbFrames[cameraIndex0].pixelFormat());
			Tracking::OculusTags::Utilities::drawOculusTag(rgbFrames[cameraIndex0], *anyCameras[cameraIndex0], HomogenousMatrix4(world_T_device), device_T_cameras[0], iterTrackedtag.second.tag_, color);
			Tracking::OculusTags::Utilities::drawOculusTag(rgbFrames[cameraIndex1], *anyCameras[cameraIndex1], HomogenousMatrix4(world_T_device), device_T_cameras[1], iterTrackedtag.second.tag_, color);
		}
	}

	std::map<double, OculusTags>::const_iterator vrsTrackedTagsIter = map_vrsTrackedTags_.find(double(frameTimestamp_));
	if (vrsTrackedTagsIter != map_vrsTrackedTags_.cend())
	{
		for (const OculusTag& vrsTag : vrsTrackedTagsIter->second)
		{
			Tracking::OculusTags::Utilities::drawOculusTag(rgbFrames[cameraIndex0], *anyCameras[cameraIndex0], HomogenousMatrix4(world_T_device), device_T_cameras[0], vrsTag, CV::Canvas::gray(rgbFrames[cameraIndex0].pixelFormat()));
			Tracking::OculusTags::Utilities::drawOculusTag(rgbFrames[cameraIndex1], *anyCameras[cameraIndex1], HomogenousMatrix4(world_T_device), device_T_cameras[1], vrsTag, CV::Canvas::gray(rgbFrames[cameraIndex1].pixelFormat()));
		}
	}

	Frames rotatedRgbFrames;

	if (rotateFrames)
	{
		rotatedRgbFrames.resize(rgbFrames.size());
		CV::FrameInterpolatorNearestPixel::Comfort::rotate90(rgbFrames[cameraIndex0], rotatedRgbFrames[cameraIndex0], rotateClockwise, WorkerPool::get().scopedWorker()());
		CV::FrameInterpolatorNearestPixel::Comfort::rotate90(rgbFrames[cameraIndex1], rotatedRgbFrames[cameraIndex1], rotateClockwise, WorkerPool::get().scopedWorker()());
	}
	else
	{
		rotatedRgbFrames = std::move(rgbFrames);
	}

	Frame rgbStereoFrame(FrameType(rotatedRgbFrames[cameraIndex0], rotatedRgbFrames[cameraIndex0].width() + rotatedRgbFrames[cameraIndex1].width(), std::max(rotatedRgbFrames[cameraIndex0].height(), rotatedRgbFrames[cameraIndex1].height())));

	CV::FrameConverter::subFrame(rotatedRgbFrames[cameraIndex0].constdata<uint8_t>(), rgbStereoFrame.data<uint8_t>(), rotatedRgbFrames[cameraIndex0].width(), rotatedRgbFrames[cameraIndex0].height(), rgbStereoFrame.width(), rgbStereoFrame.height(), rotatedRgbFrames[cameraIndex0].channels(), 0u, 0u, 0u, 0u, rotatedRgbFrames[cameraIndex0].width(), rotatedRgbFrames[cameraIndex0].height(), rotatedRgbFrames[cameraIndex0].paddingElements(), rgbStereoFrame.paddingElements());
	CV::FrameConverter::subFrame(rotatedRgbFrames[cameraIndex1].constdata<uint8_t>(), rgbStereoFrame.data<uint8_t>(), rotatedRgbFrames[cameraIndex1].width(), rotatedRgbFrames[cameraIndex1].height(), rgbStereoFrame.width(), rgbStereoFrame.height(), rotatedRgbFrames[cameraIndex0].channels(), 0u, 0u, rotatedRgbFrames[cameraIndex0].width(), 0u, rotatedRgbFrames[cameraIndex1].width(), rotatedRgbFrames[cameraIndex1].height(), rotatedRgbFrames[cameraIndex1].paddingElements(), rgbStereoFrame.paddingElements());

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED) && defined(_WINDOWS)

	if (OculusTagDebugElements::get().isElementActive(OculusTagDebugElements::EI_BOUNDARY_PATTERN_LINE_SEGMENTS))
	{
		const Frame debugFrame = OculusTagDebugElements::get().element(OculusTagDebugElements::EI_BOUNDARY_PATTERN_LINE_SEGMENTS);

		if (debugFrame.isValid())
		{
			Frame rotatedDebugFrame;
			CV::FrameInterpolatorNearestPixel::Comfort::rotate90(debugFrame, rotatedDebugFrame, false, WorkerPool::get().scopedWorker()());
			Platform::Win::Utilities::desktopFrameOutput(0, 0, rotatedDebugFrame);
		}
	}

	if (OculusTagDebugElements::get().isElementActive(OculusTagDebugElements::EI_BOUNDARY_PATTERN_LSHAPES_FINAL))
	{
		const Frame debugFrame = OculusTagDebugElements::get().element(OculusTagDebugElements::EI_BOUNDARY_PATTERN_LSHAPES_FINAL);

		if (debugFrame.isValid())
		{
			Frame rotatedDebugFrame;
			CV::FrameInterpolatorNearestPixel::Comfort::rotate90(debugFrame, rotatedDebugFrame, false, WorkerPool::get().scopedWorker()());
			Platform::Win::Utilities::desktopFrameOutput(490, 0, rotatedDebugFrame);
		}
	}

	if (OculusTagDebugElements::get().isElementActive(OculusTagDebugElements::EI_BOUNDARY_PATTERN_DETECTIONS))
	{
		const Frame debugFrame = OculusTagDebugElements::get().element(OculusTagDebugElements::EI_BOUNDARY_PATTERN_DETECTIONS);

		if (debugFrame.isValid())
		{
			Frame rotatedDebugFrame;
			CV::FrameInterpolatorNearestPixel::Comfort::rotate90(debugFrame, rotatedDebugFrame, false, WorkerPool::get().scopedWorker()());
			Platform::Win::Utilities::desktopFrameOutput(980, 0, rotatedDebugFrame);
		}
	}

	if (OculusTagDebugElements::get().isElementActive(OculusTagDebugElements::EI_TRACKER_RECTIFIED_TAG))
	{
		int32_t yOffset = 0;

		for (const OculusTag& tag : trackedTags)
		{
			ocean_assert(tag.isValid());

			const Frame rectifiedFrameA = OculusTagDebugElements::get().element(OculusTagDebugElements::EI_TRACKER_RECTIFIED_TAG, OculusTagDebugElements::Hierarchy{ String::toAString(tag.tagID()), "CAMERA_A" });
			const Frame rectifiedFrameB = OculusTagDebugElements::get().element(OculusTagDebugElements::EI_TRACKER_RECTIFIED_TAG, OculusTagDebugElements::Hierarchy{ String::toAString(tag.tagID()), "CAMERA_B" });

			if (rectifiedFrameA.isValid())
			{
				Platform::Win::Utilities::desktopFrameOutput(0, yOffset, rectifiedFrameA);
			}

			if (rectifiedFrameB.isValid())
			{
				Platform::Win::Utilities::desktopFrameOutput(int32_t(std::max(rectifiedFrameA.width(), rectifiedFrameB.width())) + 10, yOffset, rectifiedFrameB);
			}

			yOffset += int32_t(std::max(rectifiedFrameA.height(), rectifiedFrameB.height())) + 10;
		}
	}

	if (OculusTagDebugElements::get().isElementActive(OculusTagDebugElements::EI_DETECTOR_RECTIFIED_TAG))
	{
		typedef std::unordered_map<uint32_t, int32_t> YOffsetMap;
		static YOffsetMap frameYOffsetsMap;

		static int32_t maxYOffset = 0;

		for (const OculusTag& tag : trackedTags)
		{
			ocean_assert(tag.isValid());

			const Frame rectifiedFrameA = OculusTagDebugElements::get().element(OculusTagDebugElements::EI_DETECTOR_RECTIFIED_TAG, OculusTagDebugElements::Hierarchy{ String::toAString(tag.tagID()), "CAMERA_A" });
			const Frame rectifiedFrameB = OculusTagDebugElements::get().element(OculusTagDebugElements::EI_DETECTOR_RECTIFIED_TAG, OculusTagDebugElements::Hierarchy{ String::toAString(tag.tagID()), "CAMERA_B" });

			int32_t yOffset = 0;

			YOffsetMap::const_iterator yOffsetIter = frameYOffsetsMap.find(tag.tagID());

			if (yOffsetIter != frameYOffsetsMap.cend())
			{
				yOffset = yOffsetIter->second;
			}
			else
			{
				yOffset = maxYOffset;
				frameYOffsetsMap.insert(std::make_pair(tag.tagID(), yOffset));

				maxYOffset += 10 + int32_t(std::max(rectifiedFrameA.height(), rectifiedFrameB.height()));
			}

			if (rectifiedFrameA.isValid())
			{
				Platform::Win::Utilities::desktopFrameOutput(0, yOffset, rectifiedFrameA);
			}

			if (rectifiedFrameB.isValid())
			{
				Platform::Win::Utilities::desktopFrameOutput(rectifiedFrameB.width() + 10, yOffset, rectifiedFrameB);
			}
		}
	}

#endif // OCN_OCULUSTAG_DEBUGGING_ENABLED && _WINDOWS

	if (movieRecorder_)
	{
		if (!movieRecorder_->frameType().isValid())
		{
			movieRecorder_->setPreferredFrameType(rgbStereoFrame.frameType());
			movieRecorder_->start();
		}

		Frame recorderFrame;
		if (movieRecorder_->lockBufferToFill(recorderFrame, /* respectFrameFrequency */ false))
		{
			if (!CV::FrameConverter::Comfort::convertAndCopy(rgbStereoFrame, recorderFrame))
			{
				Log::error() << "Failed to record frame";
				ocean_assert(false);
			}

			movieRecorder_->unlockBufferToFill();
		}
	}

	resultFrame = std::move(rgbStereoFrame);
	resultFrame.setTimestamp(frameTimestamp_);

	time = performance_.average();

	return true;
}

OculusTagTrackerWrapper& OculusTagTrackerWrapper::operator=(OculusTagTrackerWrapper&& oculusTagTrackerWrapper) noexcept
{
	if (this != &oculusTagTrackerWrapper)
	{
		// the tracker is designed to exist only once

		frameMediumRefs_ = std::move(oculusTagTrackerWrapper.frameMediumRefs_);

		frameTimestamp_ = oculusTagTrackerWrapper.frameTimestamp_;

		oculusTagTracker_ = std::move(oculusTagTrackerWrapper.oculusTagTracker_);

		frameCounter_ = oculusTagTrackerWrapper.frameCounter_;

#ifdef OCEAN_USE_DEVICES_VRS
		devicePlayer_ = std::move(oculusTagTrackerWrapper.devicePlayer_);
#endif
	}

	return *this;
}
