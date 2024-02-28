// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/onerecognizer/OneRecognizerExperience.h"
#include "application/ocean/xrplayground/common/DownloadManager.h"
#include "application/ocean/xrplayground/common/ContentManager.h"

#include <folly/Random.h>
#include <ocean/io/File.h>
#include <ocean/io/image/Image.h>
#include <ocean/media/Manager.h>
#include <ocean/rendering/Utilities.h>
#include <vector>

#if defined XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

#include <mr/stable/runtime/ComputeSupportedCapabilities.h>
#include <insightsdk/mr_api.h>
#include <pattern_tracker/common/Types.h>

using namespace Ocean::Platform::Meta::Quest::Application;
using namespace Ocean::Platform::Meta::Quest::VrApi::Application;

namespace Ocean::XRPlayground
{

OneRecognizerExperience::OneRecognizerExperience() :
	ipcClient_([this](const recognition::RecognitionResult result) { targetsHandler(result); },
		[this](const pattern_tracker::PatternTrackerState state) { stateHandler(state); },
        mr::mrsystemservice::ipc::kServerPackageName,
        mr::mrsystemservice::ipc::kServerProcessName,
        mr::mrsystemservice::ipc::kServiceName) {}

bool OneRecognizerExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	Log::info() << "Creating MrSystemService IPC Client";
	// Connect to MrSystemService
	if (!ipcClient_.initClient()) {
		Log::info() << "Error creating MrSystemService IPC Client";
		return false;
	}

	// Start the recognition service
	{
		mr::mrsystemservice::ipc::ChangeMrCapabilityStatusReply reply = {MR_ERROR_UNKNOWN};
		mr::mrsystemservice::ipc::ChangeMrCapabilityStatusRequest request;
		request.capability = mr::RuntimeComputeCapabilities::Recognition;
		request.command = mr::mrsystemservice::ipc::MrCapabilityCommands::Start;

		ipcClient_.ChangeMrCapabilityStatusPublic(request, reply);
		MrResult mrReply = static_cast<MrResult>(reply.response);
		if (!MR_SUCCEEDED(mrReply)) {
			Log::info() << "MrSystemService failed to start recognition service: " << mrReply;
		return false;
		}
	}

	// Start the pattern tracker service
	{
		mr::mrsystemservice::ipc::ChangeMrCapabilityStatusReply reply = {MR_ERROR_UNKNOWN};
		mr::mrsystemservice::ipc::ChangeMrCapabilityStatusRequest request;
		request.capability = mr::RuntimeComputeCapabilities::PatternTracker;
		request.command = mr::mrsystemservice::ipc::MrCapabilityCommands::Start;

		ipcClient_.ChangeMrCapabilityStatusPublic(request, reply);
		auto mrReply = static_cast<MrResult>(reply.response);
		if (!MR_SUCCEEDED(mrReply)) {
			Log::info() << "MrSystemService failed to start PatternTracker service: " << mrReply;
			return false;
		}
	}

	// Init table menu
	vrTableMenu_ = VRTableMenu(engine, engine->framebuffers().front());

	// Init text visualizer
	textVisualizer_ = VRTextVisualizer(engine, engine->framebuffers().front());

	// resume passthrough
	PlatformSpecific::get().mrPassthroughVisualizer().resumePassthrough();

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

	// Load scene
	ContentManager::get().loadContent("assetId:496677651798835", ContentManager::LM_LOAD_ADD, std::bind(&OneRecognizerExperience::onContentHandled, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	ContentManager::get().loadContent("onerecognizer_demo_object.ox3dv", ContentManager::LM_LOAD_ADD, std::bind(&OneRecognizerExperience::onContentHandled, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	return true;
}

bool OneRecognizerExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	frameMedium_.release();
	for_each(additionalScenes_.begin(), additionalScenes_.end(), [&](Rendering::SceneRef& scene)
	{
		scene.release();
	});
	return true;
}

void OneRecognizerExperience::onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
}

Timestamp OneRecognizerExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (!ossdkHeadTracker_)
	{
		ossdkHeadTracker_ = OSSDK::Tracking::v8::createHeadTracker();
	}

	// Calculate matrix transformation from OSSDK to VRAPI world coordinate system
	{
		OSSDK::Optional<OSSDK::Tracking::v5::RigidBodyTrackingDataMapFromDevice> headTrackerState = ossdkHeadTracker_->getState(OSSDK::Sensors::v3::ProcessingClockTimestamp::fromSeconds(double(timestamp)));

		if (headTrackerState.hasValue() && headTrackerState.value().valid)
		{
			const OSSDK::Transform& pose = headTrackerState.value().referenceFromBody.pose;

			const QuaternionD rotation(pose.rotation.w, pose.rotation.x, pose.rotation.y, pose.rotation.z);
			ocean_assert(rotation.isValid());

			const VectorD3 translation(pose.translation.x, pose.translation.y, pose.translation.z);

			const HomogenousMatrix4 ossdkWorld_T_device = HomogenousMatrix4(HomogenousMatrixD4(translation, rotation));
			const HomogenousMatrix4 vrapiWorld_T_device = PlatformSpecific::get().world_T_device(timestamp);

			vrapiWorld_T_ossdkWorld_ = vrapiWorld_T_device * ossdkWorld_T_device.inverted();
		}
	}

	switch (state_)
	{
		case IDLE:
		{
			// Show table entries
			VRTableMenu::Entries menuEntries =
			{
				VRTableMenu::Entry("Scene Understanding Experience", "SCENE_UNDERSTANDING"),
				VRTableMenu::Entry("Target AR Experience", "TARGET_AR")
			};

			const VRTableMenu::Group menuGroup("Select experience you want to try", std::move(menuEntries));

			vrTableMenu_.setMenuEntries(menuGroup);
			vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1)));

			state_ = SELECTING_MODE;
			break;
		}
		case SELECTING_MODE:
		{
			ocean_assert(vrTableMenu_.isShown());
			std::string entryUrl;
			if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
			{
				vrTableMenu_.hide();
				// default recognition type
				auto recognitionType = recognition::RecognitionType::SCENE_UNDERSTANDING;
				std::unordered_map<std::string, std::string> options;
				if (entryUrl == "SCENE_UNDERSTANDING")
				{
					state_ = SCENE_UNDERSTANDING;
				}
				else if (entryUrl == "TARGET_AR")
				{
					state_ = TARGET_AR_RECOGNIZING;
					recognitionType = recognition::RecognitionType::TARGET_AR;
					options.insert({"domain", "target_domain"});
					textVisualizer_.visualizeTextInWorld(
						1,
						HomogenousMatrix4(Vector3(0, 0.55, -1)),
						"Recognizing....",
						VRTextVisualizer::ObjectSize(0, 0.25)
					);
				}
				std::vector<recognition::RecognitionConfig> configs = {
					recognition::RecognitionConfig{recognitionType, options}
				};
				bool result = false;
				ipcClient_.onRecognitionPipelineChange(recognition::RecognitionConfigs{configs}, result);
				if (!result)
				{
					Log::info() << "Unable to set recognition config";
				}
				Log::info() << "Set recognition config, start processing results";
			}
			break;
		}
		case SCENE_UNDERSTANDING:
		{
			ipcClient_.PollAndHandleClientEvents();
			std::string text = "Detected concepts:\n";
			std::vector<std::string> sortedConcepts;
			sortedConcepts.resize(targets.size());
			for (auto const& target : targets)
			{
				sortedConcepts.push_back(target.targetId);
			}
			std::sort(sortedConcepts.begin(), sortedConcepts.end(), [](const std::string& a, const std::string& b) {
				return a < b; }
			);
			for (auto const& sceneConcept : sortedConcepts)
			{
				text += "    " + sceneConcept + "\n";
			}
			textVisualizer_.visualizeTextInWorld(
				1,
				HomogenousMatrix4(Vector3(0, 0.45, -1)),
				text,
				VRTextVisualizer::ObjectSize(0, 0.25)
			);
			break;
		}
		case TARGET_AR_RECOGNIZING:
		{
			// Start the tracking thread
			ipcClient_.PollAndHandleClientEvents();

			std::vector<pattern_tracker::PatternConfiguration> patternConfigurations;

			for (auto const& target : targets)
			{
				if (!target.blockId) {
					continue;
				}
				auto& blockId = target.blockId.value();
				if (targetAREntities.find(blockId) == targetAREntities.end()) {
					// Downloading image from url
					auto fileFuture = DownloadManager::get().downloadAssetFromMetaCdnUrl(blockId, target.targetUrl.value());

					if (fileFuture.valid())
					{
						const IO::File downloadedFile = fileFuture.get();
						if (downloadedFile.isValid())
						{
							Log::info() << "Successful downloaded asset file " << downloadedFile.name() << " Path:" << downloadedFile();
							Frame patternFrame = IO::Image::readImage(downloadedFile());
							if (patternFrame.isValid())
							{
								Log::info() << "Add new pattern to tracker";

								world_T_patterns_.emplace_back(HomogenousMatrix4(false));
								Vector2 patternDimension = Vector2(Scalar(0.225), Scalar(0.255));
								if (target.physicalHeight && target.physicalWidth) {
									Log::info() << "Add pattern with physical dimension - height: " << target.physicalHeight.value() << ", width: " << target.physicalWidth.value();
									patternDimension = Vector2(Scalar(target.physicalWidth.value()), Scalar(target.physicalHeight.value()));
								}
								// Adding bbox to the scenes
								Rendering::BoxRef box;
								Rendering::MaterialRef boxMaterial;
								Rendering::TransformRef boxTransform = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.05), Scalar(0.05), Scalar(0.05)), RGBAColor(0.0f, 1.0f, 0.0f, 0.75f), &box, /* attributeSet */ nullptr, &boxMaterial);
								boxTransform->setVisible(true);
								int sceneIndex = patternsDimension_.size() % additionalScenes_.size();
								const Box3 objectDimension = Box3(Vector3(0, 0, 0), Vector3(patternDimension.x(), patternDimension.length() * Scalar(0.005), patternDimension.y()));
								const RGBAColor color(0.0f, 1.0f, 0.0, 0.1f);
								box->setSize(Vector3(objectDimension.xDimension(), objectDimension.yDimension(), objectDimension.zDimension()));
								boxMaterial->setAmbientColor(color);
								boxMaterial->setEmissiveColor(color);
								additionalScenes_[sceneIndex]->addChild(boxTransform);
								patternsDimension_.emplace_back(patternDimension);

								std::vector<uint8_t> patternImageDataVector;
								Ocean::IO::Image::encodeImage(patternFrame, "jpg", patternImageDataVector);
								pattern_tracker::PatternConfiguration configuration{folly::Random::rand64(), patternImageDataVector, target.physicalWidth.value(), target.physicalHeight.value()};
								patternConfigurations.push_back(configuration);

								// cache the result
								targetAREntities.emplace(blockId, target);

								std::stringstream message("Tracking with\n");
								for (const auto &entity : targetAREntities)
								{
									message << "BlockId/AR Experience ID:" << entity.first << "\n";
								}
								textVisualizer_.visualizeTextInWorld(
									1,
									HomogenousMatrix4(Vector3(0, 0.55, -1)),
									message.str(),
									VRTextVisualizer::ObjectSize(0, 0.25)
								);
							} else {
								Log::info() << "Failed to load pattern frame";
							}
						}
					} else {
						Log::info() << "Failed downloaded asset for Target AR";
					}
				}
			}

			if (!patternConfigurations.empty()) {
				pattern_tracker::TrackPatternRequest request{patternConfigurations};
				bool result = false;
				ipcClient_.onPatternTrackingRequest(request, result);
				if (!result) {
					Log::error() << "failed to call onPatternTrackingRequest";
				}
			}

			break;
		}
	}
	return timestamp;
}

void OneRecognizerExperience::onContentHandled(const std::string& content, const bool succeeded, const std::vector<Rendering::SceneRef>& scenes)
{
	if (!succeeded || scenes.size() != 1 || !scenes.front())
	{
		Log::error() << "Failed to handle content!";
		return;
	}
	additionalScenes_.push_back(scenes.front());
	additionalScenes_.back()->setVisible(false);
}

std::unique_ptr<XRPlaygroundExperience> OneRecognizerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new OneRecognizerExperience());
}

void OneRecognizerExperience::targetsHandler(const recognition::RecognitionResult result)
{
	Log::info() << "OR App received " << result.targets.size() << " targets";
	targets.clear();
	for (auto const& target : result.targets)
	{
		targets.emplace_back(target);
	}
}


void OneRecognizerExperience::stateHandler(const pattern_tracker::PatternTrackerState state) {
    if (state.trackedPatterns.empty()) {
        return;
    }

	for (const auto& trackedPattern : state.trackedPatterns)
	{
		const auto& ossdkWorld_T_pattern = Ocean::HomogenousMatrix4(trackedPattern.second.pose.data(), true /*valuesRowAligned*/);
		if (ossdkWorld_T_pattern.isValid() && vrapiWorld_T_ossdkWorld_.isValid())
		{
			// Hardcoding index 0 for demo, only track one pattern at a time
			additionalScenes_[0]->setTransformation(vrapiWorld_T_ossdkWorld_ * ossdkWorld_T_pattern);
			additionalScenes_[0]->setVisible(true);
		}
	}
}

bool OneRecognizerExperience::activateFrameMedium(const std::string& frameMediumUrl)
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

	Log::info() << "The frame medium <" << frameMediumUrl << ">";

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

OneRecognizerExperience::FrameMediumUrlMap OneRecognizerExperience::availableFrameMediums()
{
	FrameMediumUrlMap frameMediumUrlMap;

	switch (Platform::Meta::Quest::Device::deviceType())
	{
		case Platform::Meta::Quest::Device::DT_QUEST_PRO:
			frameMediumUrlMap["Color camera"] = "ColorCameraId:0";
			frameMediumUrlMap["Glacier (left) "] = "StereoCamera0Id:0";
			frameMediumUrlMap["Glacier (right)"] = "StereoCamera0Id:1";
			break;

		case Platform::Meta::Quest::Device::DT_QUEST_3:
			frameMediumUrlMap["Color camera (left)"] = "ColorCameraId:0";
			frameMediumUrlMap["Color camera (right)"] = "ColorCameraId:1";
			frameMediumUrlMap["Glacier (left) "] = "StereoCamera0Id:0";
			frameMediumUrlMap["Glacier (right)"] = "StereoCamera0Id:1";
			break;

		default:
			Log::error() << "Missing case for <" << Platform::Meta::Quest::Device::deviceName(Platform::Meta::Quest::Device::deviceType()) << ">";
			break;
	}

	ocean_assert(!frameMediumUrlMap.empty());

	return frameMediumUrlMap;
}

} // namespace Ocean::XRPlayground

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
