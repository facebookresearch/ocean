// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/OculusTagTrackerExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/media/Manager.h"

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	#include "ocean/platform/meta/quest/Device.h"
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

void OculusTagTrackerExperience::TagVisualization::visualizeInWorld(const HomogenousMatrix4& world_T_code, const Scalar tagSize, const unsigned int tagId)
{
	ocean_assert(world_T_code.isValid());
	ocean_assert(tagSize > Scalar(0));

	const RGBAColor valueSupportTypeColors[3] =
	{
		// VST_UNSUPPORTED
		RGBAColor(1.0f, 0.0f, 0.0f, 1.0f),

		// VST_MARGINAL
		RGBAColor(1.0f, 1.0f, 0.0f, 1.0f),

		// VST_NOMINAL
		RGBAColor(0.0f, 1.0f, 0.0f, 1.0f),
	};

	const Scalar tagSize_2 = Scalar(0.5) * tagSize;

	// Top-side of the code

	textTagId_->setText(String::toAString(tagId) + " (" + String::toAString(tagSize * Scalar(100), 1u) + " cm)");
	textTagIdTransform_->setTransformation(HomogenousMatrix4(Vector3(0, -tagSize_2 - Scalar(0.01), 0)));

	box_->setSize(Vector3(tagSize, tagSize, /* height = 1 mm */ Scalar(0.002)));
	boxMaterial_->setAmbientColor(valueSupportTypeColors[2]);
	boxMaterial_->setEmissiveColor(valueSupportTypeColors[2]);
	boxTransform_->setTransformation(world_T_code);

	boxTransform_->setVisible(true);
}

void OculusTagTrackerExperience::TagVisualization::hide()
{
	boxTransform_->setVisible(false);
}

OculusTagTrackerExperience::TagVisualization OculusTagTrackerExperience::TagVisualization::create(const Rendering::EngineRef& engine, const Rendering::SceneRef& parentScene)
{
	ocean_assert(!engine.isNull());
	ocean_assert(!parentScene.isNull());

	TagVisualization tagVisualization;

	// Inside the code area or above it

	tagVisualization.boxTransform_ = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.05), Scalar(0.05), Scalar(0.05)), RGBAColor(1.0f, 1.0f, 0.0f, 0.75f), &tagVisualization.box_, /* attributeSet */ nullptr, &tagVisualization.boxMaterial_);
	tagVisualization.boxTransform_->setVisible(false);

	parentScene->addChild(tagVisualization.boxTransform_);

	tagVisualization.coordinateSystem_ = Rendering::Utilities::createCoordinateSystem(engine, /* length */ Scalar(0.1), /* topLength */ 0.01, /* radius */ 0.0025);
	ocean_assert(!tagVisualization.coordinateSystem_.isNull());

	tagVisualization.boxTransform_->addChild(tagVisualization.coordinateSystem_);

	// Bottom-side of the code

	tagVisualization.textTagIdTransform_ = Rendering::Utilities::createText(*engine, "UNDEFINED", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.01), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, /* fontFamily */ std::string(), /* styleName */ std::string(), &tagVisualization.textTagId_);
	tagVisualization.boxTransform_->addChild(tagVisualization.textTagIdTransform_);

	return tagVisualization;
}

OculusTagTrackerExperience::~OculusTagTrackerExperience()
{
	// nothing to do here
}

bool OculusTagTrackerExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	bool allCamerasAccessed = true;

	std::vector<std::string> mediumUrls;

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	switch (Platform::Meta::Quest::Device::deviceType())
	{
		case Platform::Meta::Quest::Device::DT_QUEST:
		case Platform::Meta::Quest::Device::DT_QUEST_2:
			mediumUrls =
			{
				"StereoCamera0Id:0",
				"StereoCamera0Id:1",
			};
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_PRO:
			mediumUrls =
			{
				"StereoCamera0Id:0",
				"StereoCamera0Id:1",

				// Enabling a second generic slot in order to achieve 30 FPS
				"StereoCamera2Id:0",
				"StereoCamera2Id:1",
			};
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_3:
		case Platform::Meta::Quest::Device::DT_VENTURA:
			mediumUrls =
			{
				"StereoCamera0Id:0",
				"StereoCamera0Id:1",
			};
			break;

		case Platform::Meta::Quest::Device::DT_UNKNOWN:
			Log::error() << "Unknown or unsupported device: \"" << Platform::Meta::Quest::Device::deviceName(Platform::Meta::Quest::Device::deviceType()) << "\"";
			return false;

		// Intentionally no default case!
	}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	for (const std::string& mediumUrl : mediumUrls)
	{
		Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(mediumUrl);

		if (frameMedium)
		{
			frameMedium->start();
			frameMediums_.emplace_back(std::move(frameMedium));
		}
		else
		{
			Log::warning() << "Failed to access '" << mediumUrl << "'";
			allCamerasAccessed = false;
		}
	}

	if (!allCamerasAccessed)
	{
		showMessage(MT_CAMERA_ACCESS_FAILED);
	}

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	PlatformSpecific::get().mrPassthroughVisualizer().resumePassthrough();
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	startThread();

	return true;
}

bool OculusTagTrackerExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	bool allSuceeded = true;

	stopThread();

	if (!joinThread())
	{
		Log::error() << "Failed to stop the tracking thread!";
		allSuceeded = false;
	}

	frameMediums_.clear();

	return allSuceeded;
}

Timestamp OculusTagTrackerExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedResultLock(resultLock_);

		const bool haveResults = haveResults_;
		haveResults_ = false;

		const Tracking::OculusTags::OculusTags oculusTags = std::move(oculusTags_);

	scopedResultLock.release();

	if (haveResults)
	{
		ocean_assert(codeBoxTransforms_.size() == codeBoxes_.size());

		while (tagVisualizations_.size() < oculusTags.size())
		{
			tagVisualizations_.emplace_back(TagVisualization::create(engine, experienceScene()));
		}

		for (size_t iTag = 0; iTag < oculusTags.size(); ++iTag)
		{
			ocean_assert(iTag < tagVisualizations_.size());

			const Tracking::OculusTags::OculusTag& oculusTag = oculusTags[iTag];

			// TODO Move the tag origin to its center.
			const Scalar tagSize_2 = oculusTag.tagSize() * Scalar(0.5);
			const HomogenousMatrix4 world_T_tag = oculusTag.world_T_tag() * HomogenousMatrix4(Vector3(tagSize_2, -tagSize_2, Scalar(0)));

			tagVisualizations_[iTag].visualizeInWorld(world_T_tag, oculusTag.tagSize(), oculusTag.tagID());
		}

		for (size_t iTag = oculusTags.size(); iTag < tagVisualizations_.size(); ++iTag)
		{
			tagVisualizations_[iTag].hide();
		}
	}

	return timestamp;
}

void OculusTagTrackerExperience::threadRun()
{
	Tracking::OculusTags::OculusTagTracker tracker;

	Timestamp previousTimestamp(false);

	HighPerformanceStatistic statistic;

	while (!shouldThreadStop())
	{
		FrameRefs frameRefs;
		SharedAnyCameras anyCameras;
		HomogenousMatricesD4 device_T_camerasD;

		bool timedOut = false;
		if (!Media::FrameMedium::syncedFrames(frameMediums_, previousTimestamp, frameRefs, anyCameras, /* waitTime */ 2u, &timedOut, &device_T_camerasD))
		{
			if (timedOut)
			{
				Log::warning() << "Failed to access synced camera frames for timestamp";
			}

			Thread::sleep(1u);
			continue;
		}

		HomogenousMatrices4 device_T_cameras;
		device_T_cameras.reserve(device_T_camerasD.size());

		for (const HomogenousMatrixD4& device_T_cameraD : device_T_camerasD)
		{
			device_T_cameras.emplace_back(HomogenousMatrix4(device_T_cameraD));
		}

		ocean_assert(!frameRefs.empty());
		ocean_assert(frameRefs.size() == anyCameras.size());
		ocean_assert(frameRefs.size() == device_T_cameras.size());

		ocean_assert(!frameRefs[0].isNull() && frameRefs[0]->isValid());
		const Timestamp frameTimestamp = frameRefs[0]->timestamp();

		if (previousTimestamp.isValid() && previousTimestamp >= frameTimestamp)
		{
			// Only process each frame once.
			Thread::sleep(1u);
			continue;
		}

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)

		const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(frameTimestamp);

#else

		const HomogenousMatrix4 world_T_device(false);
		Log::error() << "Unsupported platform";
		return;

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
		Frames yFrames;
		yFrames.reserve(frameRefs.size());

		for (size_t iFrame = 0; iFrame < frameRefs.size(); ++iFrame)
		{
			const Frame& frame = *frameRefs[iFrame];
			ocean_assert(frame.isValid());

			Frame yFrame;

			CV::FrameConverter::Options options;

			if (FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y10) ||
				FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y10_PACKED))
			{
				options = CV::FrameConverter::Options(/* gamma */ 0.6f, /* allowApproximations */ true);
			}

			if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()(), options))
			{
				ocean_assert(false && "This should never happen!");
			}

			yFrame.setTimestamp(frameTimestamp);

			yFrames.emplace_back(std::move(yFrame));
		}

		Tracking::OculusTags::OculusTags oculusTags;

		statistic.start();
		tracker.trackTagsStereo(*anyCameras[0], *anyCameras[1], yFrames[0], yFrames[1], world_T_device, device_T_cameras[0], device_T_cameras[1], oculusTags);
		statistic.stop();

		double averageDetectionTimeMs = -1.0;
		if (statistic.measurements() % 15u == 0)
		{
			averageDetectionTimeMs = statistic.averageMseconds();
			statistic.reset();
		}

		previousTimestamp = frameTimestamp;

		const ScopedLock scopedLock(resultLock_);

		oculusTags_ = oculusTags;

		detectionTimestamp_ = frameTimestamp;

		if (averageDetectionTimeMs != -1.0)
		{
			averageDetectionTimeMs_ = averageDetectionTimeMs;
		}

		haveResults_ = true;
	}
}

std::unique_ptr<XRPlaygroundExperience> OculusTagTrackerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new OculusTagTrackerExperience());
}

}

}
