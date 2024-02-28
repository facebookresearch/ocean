// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/PatternTrackerExperience.h"

#include "application/ocean/xrplayground/common/ContentManager.h"
#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/io/File.h"
#include "ocean/io/FileResolver.h"

#include "ocean/io/image/Image.h"

#include "ocean/media/Manager.h"

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	#include "ocean/platform/meta/quest/Device.h"
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

#include "ocean/rendering/Utilities.h"

#include "ocean/tracking/pattern/PatternTracker6DOF.h"

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

using namespace Ocean::Platform::Meta::Quest::VrApi::Application;

PatternTrackerExperience::~PatternTrackerExperience()
{
	// nothing to do here
}

bool PatternTrackerExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	frameMediumMenu_ = VRTableMenu(engine, engine->framebuffers().front());

	const FrameMediumUrlMap frameMediumUrlMap = availableFrameMediums();

	if (frameMediumUrlMap.empty())
	{
		Log::error() << "Failed to obtain a live video type. Cannot initialize any frame medium.";

		return false;
	}

	const std::string frameMediumUrl = frameMediumUrlMap.begin()->second;
	ocean_assert(!frameMediumUrl.empty());

	if (!activateFrameMedium(frameMediumUrl))
	{
		showMessage(MT_CAMERA_ACCESS_FAILED);
	}

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	PlatformSpecific::get().mrPassthroughVisualizer().resumePassthrough();
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	ContentManager::get().loadContent("assetId:496677651798835", ContentManager::LM_LOAD_ADD, std::bind(&PatternTrackerExperience::onContentHandled, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	startThread();

	return true;
}

bool PatternTrackerExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	bool allSuceeded = true;

	stopThread();

	if (!joinThread())
	{
		Log::error() << "Failed to stop the tracking thread!";
		allSuceeded = false;
	}

	boxTransforms_.clear();
	boxes_.clear();
	boxMaterials_.clear();

	additionalScene_.release();

	frameMedium_.release();

	frameMediumMenu_.release();

	return allSuceeded;
}

Timestamp PatternTrackerExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedResultLock(resultLock_);

		const bool haveResults = haveResults_;
		haveResults_ = false;

		const HomogenousMatrix4 camera_T_pattern = camera_T_pattern_;
		const Vector2 patternDimension = patternDimension_;

		const HomogenousMatrix4 device_T_camera = std::move(device_T_camera_);
		const HomogenousMatrix4 world_T_device = std::move(world_T_device_);

	scopedResultLock.release();

	if (haveResults)
	{
		ocean_assert(boxTransforms_.size() == boxes_.size() && boxTransforms_.size() == boxMaterials_.size());

		if (boxTransforms_.empty())
		{
			Rendering::BoxRef box;
			Rendering::MaterialRef boxMaterial;
			Rendering::TransformRef boxTransform = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.05), Scalar(0.05), Scalar(0.05)), RGBAColor(0.0f, 1.0f, 0.0f, 0.75f), &box, /* attributeSet */ nullptr, &boxMaterial);
			boxTransform->setVisible(true);

			experienceScene()->addChild(boxTransform);

			boxTransforms_.emplace_back(std::move(boxTransform));
			boxes_.emplace_back(std::move(box));
			boxMaterials_.emplace_back(std::move(boxMaterial));
		}

		// Define a box that will be displayed as an overlay over the pattern; because the origin of the pattern is in the top-left corner,
		const Box3 objectDimension = Box3(Vector3(0, 0, 0), Vector3(patternDimension.x(), patternDimension.length() * Scalar(0.005), patternDimension.y()));

		if (camera_T_pattern.isValid())
		{
			const HomogenousMatrix4 camera_T_recenteredPattern = camera_T_pattern * HomogenousMatrix4(Vector3(objectDimension.xDimension() * Scalar(0.5), objectDimension.yDimension() * Scalar(0.5), objectDimension.zDimension() * Scalar(0.5)));

			Rendering::BoxRef box = boxes_.front();
			Rendering::MaterialRef boxMaterial = boxMaterials_.front();
			Rendering::TransformRef boxTransform = boxTransforms_.front();

			const RGBAColor color(0.0f, 1.0f, 0.0, 1.0f);

			box->setSize(Vector3(objectDimension.xDimension(), objectDimension.yDimension(), objectDimension.zDimension()));
			boxMaterial->setAmbientColor(color);
			boxMaterial->setEmissiveColor(color);

			const HomogenousMatrix4 world_T_pattern = world_T_device * device_T_camera * camera_T_recenteredPattern;

			experienceScene()->setTransformation(world_T_pattern);
			experienceScene()->setVisible(true);

			if (additionalScene_)
			{
				additionalScene_->setTransformation(world_T_pattern);
				additionalScene_->setVisible(true);
			}
		}
		else
		{
			experienceScene()->setVisible(false);

			if (additionalScene_)
			{
				additionalScene_->setVisible(false);
			}
		}
	}

	std::string frameMediumUrl;
	if (madeSelectionFromFrameMediumMenu(frameMediumUrl, timestamp))
	{
		if (!activateFrameMedium(frameMediumUrl))
		{
			Log::error() << "Failed to activate the frame medium <" << frameMediumUrl << ">";
		}
	}

	return timestamp;
}

void PatternTrackerExperience::onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	if (key == "X")
	{
		if (frameMediumMenu_.isShown())
		{
			frameMediumMenu_.hide();
		}
		else
		{
			showFrameMediumMenu(timestamp);
		}
	}
}

void PatternTrackerExperience::threadRun()
{
	const IO::Files resolvedFiles = IO::FileResolver::get().resolve(IO::File("sift800x640.jpg"), /* checkExistence */ true);

	if (resolvedFiles.empty())
	{
		Log::error() << "No pattern file has been found. The tracker cannot be started";
		ocean_assert(false && "This should never happen!");

		return;
	}

	const IO::File& patternFile = resolvedFiles.front(); // Using the first element as pattern.

	const Frame patternFrame = IO::Image::readImage(patternFile());

	if (!patternFrame.isValid())
	{
		Log::error() << "Failed to load the pattern frame: " << patternFile();

		return;
	}

	constexpr Scalar patternWidth = Scalar(0.225); // Hard-code the pattern size to approx. the size of A4/US-letter paper.

	ocean_assert(patternFrame.width() != 0u);
	const Vector2 patternDimension = Vector2(patternWidth, patternWidth * Scalar(patternFrame.height()) / Scalar(patternFrame.width()));

	const std::string trackerName = "Pattern 6DOF Tracker";

	Tracking::VisualTrackerRef visualTracker;

	if (visualTracker.isNull() || trackerName == std::string("Pattern 6DOF Tracker"))
	{
		visualTracker = Tracking::VisualTrackerRef(new Tracking::Pattern::PatternTracker6DOF());
		visualTracker.force<Tracking::Pattern::PatternTracker6DOF>().addPattern(patternFrame, patternDimension, WorkerPool::get().scopedWorker()());
	}

	if (visualTracker.isNull())
	{
		Log::error() << "Failed to initialize the visual tracker";

		return;
	}

	Timestamp previousTimestamp(false);

	HighPerformanceStatistic statistic;

	while (!shouldThreadStop())
	{
		SharedAnyCamera anyCamera;
		HomogenousMatrix4 device_T_camera;
		Frame yFrame;

		{
			ScopedLock scopedFrameMediumLock(frameMediumLock_);

			const FrameRef frameRef = frameMedium_->frame(&anyCamera);

			if (frameRef)
			{
				device_T_camera = HomogenousMatrix4(frameMedium_->device_T_camera());
				ocean_assert(device_T_camera.isValid());

				const Frame& frame = *frameRef;
				ocean_assert(frame.isValid());

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

				yFrame.setTimestamp(frameRef->timestamp());

				ocean_assert(anyCamera->width() == yFrame.width() && anyCamera->height() == yFrame.height());
			}
		}

		const Timestamp frameTimestamp = yFrame.timestamp();

		if (!anyCamera || !anyCamera->isValid() || !yFrame.isValid())
		{
			Thread::sleep(1u);
			continue;
		}

		if (previousTimestamp.isValid() && previousTimestamp >= frameTimestamp)
		{
			// Process every frame only once.
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

		Tracking::VisualTracker::TransformationSamples transformationSamples;

		const Frames yFrames = { yFrame };
		const SharedAnyCameras anyCameras = { anyCamera };

		statistic.start();
		const bool trackingSuccessful = visualTracker->determinePoses(yFrames, anyCameras, transformationSamples, Quaternion(false), WorkerPool::get().scopedWorker()()) && !transformationSamples.empty();
		statistic.stop();

		double averageDetectionTimeMs = -1.0;
		if (statistic.measurements() % 15u == 0)
		{
			averageDetectionTimeMs = statistic.averageMseconds();
			statistic.reset();
		}

		previousTimestamp = frameTimestamp;

		const ScopedLock scopedResultLock(resultLock_);

		if (trackingSuccessful)
		{
			ocean_assert(!transformationSamples.empty());

			const HomogenousMatrix4& pattern_T_camera = transformationSamples.front().transformation();

			camera_T_pattern_ = pattern_T_camera.inverted();
			patternDimension_ = patternDimension;
		}
		else
		{
			camera_T_pattern_ = HomogenousMatrix4(false);
			patternDimension_ = Vector2();
		}

		anyCamera_ = std::move(anyCamera);
		device_T_camera_ = std::move(device_T_camera);
		world_T_device_ = std::move(world_T_device);

		detectionTimestamp_ = frameTimestamp;

		if (averageDetectionTimeMs != -1.0)
		{
			averageDetectionTimeMs_ = averageDetectionTimeMs;
		}

		haveResults_ = true;
	}
}

std::unique_ptr<XRPlaygroundExperience> PatternTrackerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new PatternTrackerExperience());
}

void PatternTrackerExperience::onContentHandled(const std::string& content, const bool succeeded, const std::vector<Rendering::SceneRef>& scenes)
{
	if (!succeeded || scenes.size() != 1 || !scenes.front())
	{
		Log::error() << "Failed to handle content!";
		return;
	}

	additionalScene_ = scenes.front();
	additionalScene_->setVisible(true);
}

void PatternTrackerExperience::showFrameMediumMenu(const Timestamp& timestamp)
{
	const FrameMediumUrlMap frameMediumUrlMap = availableFrameMediums();
	ocean_assert(!frameMediumUrlMap.empty());

	VRTableMenu::Entries menuEntries;

	for (const FrameMediumUrlMap::value_type& frameMediumUrlPair : frameMediumUrlMap)
	{
		menuEntries.emplace_back(frameMediumUrlPair.first, frameMediumUrlPair.second);
	}

	const VRTableMenu::Group menuGroup("Live Video Types", std::move(menuEntries));

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)

	const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(timestamp);

#else

	const HomogenousMatrix4 world_T_device(false);
	Log::error() << "Unsupported platform";
	return;

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	// create a pitch/roll-free device transformation

	const Vector3 yAxis(0, 1, 0);

	Vector3 zAxis = world_T_device.xAxis().cross(yAxis);
	Vector3 xAxis = yAxis.cross(zAxis);

	if (!zAxis.normalize() || !xAxis.normalize())
	{
		ocean_assert(false && "This should never happen!");

		return;
	}

	const HomogenousMatrix4 world_T_deviceYaw(xAxis, yAxis, zAxis, world_T_device.translation());

	const HomogenousMatrix4 world_T_menu = world_T_deviceYaw * HomogenousMatrix4(Vector3(0, 0, Scalar(-0.7)));
	ocean_assert(world_T_menu.isValid());

	frameMediumMenu_.setMenuEntries(menuGroup);
	frameMediumMenu_.show(world_T_menu);
}

bool PatternTrackerExperience::madeSelectionFromFrameMediumMenu(std::string& frameMediumUrl, const Timestamp& timestamp)
{
	ocean_assert(timestamp.isValid());

	if (!frameMediumMenu_.isShown())
	{
		return false;
	}

	std::string entryUrl;
	if (frameMediumMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
	{
		frameMediumMenu_.hide();

		for (const FrameMediumUrlMap::value_type& availableLiveVideoType : availableFrameMediums())
		{
			if (entryUrl == availableLiveVideoType.second)
			{
				frameMediumUrl = availableLiveVideoType.second;

				return true;
			}
		}

		ocean_assert(false && "This should never happen!");
	}

	return false;
}

bool PatternTrackerExperience::activateFrameMedium(const std::string& frameMediumUrl)
{
	bool foundFrameMediumUrl = false;

	for (const FrameMediumUrlMap::value_type& availableLiveVideoType : availableFrameMediums())
	{
		if (frameMediumUrl == availableLiveVideoType.second)
		{
			foundFrameMediumUrl = true;

			break;
		}
	}

	if (!foundFrameMediumUrl)
	{
		Log::error() << "The frame medium <" << frameMediumUrl << "> is not available";
		ocean_assert(false && "This should never happen!");

		return false;
	}

	ScopedLock scopedLock(frameMediumLock_);

	if (frameMedium_)
	{
		if (frameMedium_->url() == frameMediumUrl)
		{
			// The requested frame medium is already selected. There is nothing to do.
			return true;
		}
	}

	Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(frameMediumUrl);

	if (frameMedium)
	{
		frameMedium->start();

		frameMedium_.release();

		frameMedium_ = std::move(frameMedium);

		return true;
	}

	Log::error() << "Failed to initialize the frame medium <" << frameMediumUrl << ">.";

	return false;
}

PatternTrackerExperience::FrameMediumUrlMap PatternTrackerExperience::availableFrameMediums()
{
	FrameMediumUrlMap frameMediumUrlMap;

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	switch (Platform::Meta::Quest::Device::deviceType())
	{
		case Platform::Meta::Quest::Device::DT_QUEST:
		case Platform::Meta::Quest::Device::DT_QUEST_2:
			frameMediumUrlMap["IOT camera (lower left)"] = "StereoCamera0Id:0";
			frameMediumUrlMap["IOT camera (lower right)"] = "StereoCamera0Id:1";
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_PRO:
			frameMediumUrlMap["Color camera"] = "ColorCameraId:0";
			frameMediumUrlMap["Glacier (left) "] = "StereoCamera0Id:0";
			frameMediumUrlMap["Glacier (right)"] = "StereoCamera0Id:1";
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_3:
			frameMediumUrlMap["Left color camera"] = "ColorCameraId:0";
			frameMediumUrlMap["Right color camera"] = "ColorCameraId:1";
			frameMediumUrlMap["Glacier (left) "] = "StereoCamera0Id:0";
			frameMediumUrlMap["Glacier (right)"] = "StereoCamera0Id:1";
			break;

		case Platform::Meta::Quest::Device::DT_VENTURA:
			frameMediumUrlMap["Left color camera"] = "ColorCameraId:0";
			frameMediumUrlMap["Right color camera"] = "ColorCameraId:1";
			break;

		case Platform::Meta::Quest::Device::DT_UNKNOWN:
			Log::error() << "Unknown or unsupported device: \"" << Platform::Meta::Quest::Device::deviceName(Platform::Meta::Quest::Device::deviceType()) << "\"";
			break;

		// Intentionally no default case!
	}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	ocean_assert(!frameMediumUrlMap.empty());

	return frameMediumUrlMap;
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

}
