// (c) Meta Platform, Inc. and its affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/HyperNovaRightClickExperience.h"
#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/FrameUtilities.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	#include "ocean/io/Directory.h"
	#include "ocean/io/File.h"
	#include "ocean/media/Manager.h"
	#include "ocean/media/Utilities.h"
	#include "ocean/platform/meta/quest/Device.h"
	#include "ocean/rendering/Utilities.h"
	#include "application/ocean/xrplayground/common/PlatformSpecific.h"
	#include "ocean/platform/meta/quest/application/VRImageVisualizer.h"
	#include "ocean/platform/meta/quest/vrapi/HeadsetPoses.h"
	#include <folly/executors/CPUThreadPoolExecutor.h>
	#include <folly/executors/QueuedImmediateExecutor.h>
	#include <folly/concurrency/ConcurrentHashMap.h>
	#include <hexagon_shim/HexagonShim.h>
	#include <json/json.h>
	#include <iomanip>
	#include "ocean/math/Box2.h"
	#include "ocean/cv/Canvas.h"
	#include "ocean/cv/fonts/FontManager.h"
	#include <time.h>
	#include <tools/cxx/Resources.h>
	#include <tuple>
	#include <algorithm>
	#include <set>
	#include <iterator>
	#include <xplat/arfx/recognizer/pixel_buffer/factory/ServicePixelBufferFactory.hpp>
	#include <xplat/arfx/recognizer/activators/proxy/ProxyActivator.hpp>
	#include <xplat/arfx/recognizer/affordances/bounding_box/BoundingBoxAffordance.hpp>
	#include <xplat/arfx/recognizer/affordances/label/LabelAffordance.hpp>
	#include <xplat/arfx/recognizer/aggregators/intentaggregator/IntentAggregator.hpp>
	#include <xplat/arfx/recognizer/coordinator/RecognitionCoordinator.hpp>
	#include <xplat/arfx/recognizer/coordinator/event_listener/debug_logger/RecognitionCoordinatorDebugLogger.hpp>
	#include <xplat/arfx/recognizer/determinators/few_shot/category/FewshotCategoryDeterminator.hpp>
	#include <xplat/arfx/recognizer/determinators/few_shot/category/detector/bolt/BoltFewshotCategoryDetector.hpp>
	#include <xplat/arfx/recognizer/determinators/few_shot/instance/FewshotInstanceDeterminator.hpp>
	#include <xplat/arfx/recognizer/determinators/text_actions/TextActionsDeterminator.hpp>
	#include <xplat/arfx/recognizer/inputsource/videobuffer/inputtype/VideoBufferData.hpp>
	#include <xplat/arfx/recognizer/intents/navigation/UrlIntent.hpp>
	#include <xplat/arfx/recognizer/intents/object_instance/ObjectInstanceIntent.hpp>
	#include <xplat/arfx/recognizer/intents/scene_understanding/SceneUnderstandingIntent.hpp>
	#include <xplat/arfx/recognizer/intents/text_actions/CopyTextIntent.hpp>
	#include <xplat/arfx/recognizer/intents/text_actions/add_contact/AddContactIntent.hpp>
	#include <xplat/arfx/recognizer/interfaces/IData.hpp>
	#include <xplat/arfx/recognizer/interfaces/IIntentDeterminator.hpp>
	#include <xplat/arfx/recognizer/interfaces/RecognizedTarget.hpp>
	#include <xplat/arfx/recognizer/model_providers/models/ocr/OCRModel.hpp>
	#include <xplat/arfx/recognizer/model_providers/models/few_shot/instance/FewshotInstanceDetectorModel.hpp>
	#include <xplat/arfx/recognizer/model_providers/models/scene_understanding/SceneUnderstandingModel.hpp>
	#include <xplat/arfx/recognizer/model_providers/providers/local/LocalModelProvider.hpp>
	#include <xplat/arfx/support/event/Observable.hpp>
	#include <xplat/arfx/recognizer/model_providers/models/pytorch/PytorchModel.hpp>
	#include <xplat/arfx/recognizer/predictors/HOI/HOIPredictor.hpp>
	#include <xplat/arfx/recognizer/predictors/HOI/bolt/BoltHOIPredictor.hpp>
	#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/FrameUtilities.h"
	#include "application/ocean/xrplayground/common/experiences/hypernovarightclick/UserAccount.h"
	#include "SmartThings/component/Component.hpp"
	#include "SmartThings/CurlClient.hpp"
	#include <ego_ocr/model/arfx_one_recognizer/Resource.h>
	#include <folly/executors/InlineExecutor.h>
	#include <langtech/resources/BuildResourceManager.h>
	#include <langtech/resources/ResourceManager.h>

using namespace facebook::arfx::recognizer;
using namespace Ocean::Platform::Meta::Quest::Application;
using namespace Ocean::Platform::Meta::Quest::VrApi;
using namespace Ocean::Platform::Meta::Quest::VrApi::Application;
using facebook::cvmm::smart_things::CurlClient;
using facebook::cvmm::smart_things::IComponent;
using facebook::cvmm::smart_things::IDevice;

namespace
{

std::shared_ptr<facebook::langtech::ResourceManager>
getEgoOcrResourceManager()
{
	auto executor = folly::getKeepAliveToken(folly::InlineExecutor::instance());
	return std::make_shared<facebook::langtech::BuildResourceManager>(
		facebook::egoocr::model::arfx_one_recognizer::getResourcePath(),
		executor);
}

} // namespace

namespace Ocean::XRPlayground
{
static const std::string kTargetActionShoplist = "shoplist";
static const std::string kBackToResults = "back_to_results";

static const std::string kSnapshotPath = "/storage/self/primary/Pictures/rightclick";

static const Vector3 instructionTransform(0, 1, -2);
static const Vector3 menuTransform(-0.2, -0.1, -1.5);
// Device status text box is shown on the right side of menu
static const Vector3 deviceStatusTransform(0.6, -0.1, -1.5);
static const Vector3 imageTransform(0.2, -0.6, -1.3);

static const RGBAColor textBackgroundColor(0.8f, 0.8f, 0.8f, 0.6f);

static const std::string TAG = "HyperNovaRightClickExperience: ";

static const IO::Directory getSnapshotDir()
{
	// Check if directory exists else create it
	const IO::Directory directory(kSnapshotPath);
	if (!directory.exists())
	{
		if (!directory.create())
		{
			Log::info() << "unable to create directory";
		}
	}
	return directory;
}

static std::string millisecondsToDatetime(int64_t rawTime)
{
	if (rawTime == 0)
	{
		return "No timestamp available";
	}
	struct tm* timeinfo;
	char buffer[80];
	time(&rawTime);
	timeinfo = localtime(&rawTime);
	strftime(buffer, 80, "%D %M %Y %I:%M%p", timeinfo);
	return std::string(buffer);
}

bool hasObjectInstanceIntent(const std::vector<std::shared_ptr<const IRecognizedIntent>>& intents)
{
	bool hasObjectInstanceIntent = false;
	for (auto const& intent : intents)
	{
		if (intent->intentType() == ObjectInstanceIntent::type())
		{
			hasObjectInstanceIntent = true;
			break;
		}
	}
	return hasObjectInstanceIntent;
}

void dedupRecognizedObjectInstance(::folly::ConcurrentHashMap<std::string, std::shared_ptr<const facebook::arfx::recognizer::RecognizedTarget>>& recognizedTargets, std::unordered_map<size_t, std::string>& deviceViewIdToDeviceId)
{

	std::unordered_map<std::string, std::tuple<std::string, float>> deviceIdToTargetIDScore;

	std::string targetId;
	float targetScore;

	std::set<std::string> targetIdsToRemove;

	for (auto& result : recognizedTargets)
	{
		if (!hasObjectInstanceIntent(result.second->intents()))
		{
			continue;
		}
		float score = 0.0;
		for (auto const& intent : result.second->intents())
		{
			if (intent->intentType() == ObjectInstanceIntent::type())
			{
				auto const objectInstanceIntent = std::static_pointer_cast<const ObjectInstanceIntent>(intent);
				score = objectInstanceIntent->score();
			}
		}

		auto deviceId = deviceViewIdToDeviceId[std::stoi(result.first)];
		if (deviceIdToTargetIDScore.find(deviceId) == deviceIdToTargetIDScore.end())
		{
			deviceIdToTargetIDScore[deviceId] = std::make_tuple(result.first, score);
		}
		else
		{
			std::tie(targetId, targetScore) = deviceIdToTargetIDScore[deviceId];
			if (score > targetScore)
			{
				deviceIdToTargetIDScore[deviceId] = std::make_tuple(result.first, score);
				targetIdsToRemove.insert(targetId);
			}
			else
			{
				targetIdsToRemove.insert(result.first);
			}
		}
	}


	for (auto& targetId : targetIdsToRemove)
	{
		recognizedTargets.erase(targetId);
	}
}

HyperNovaRightClickExperience::~HyperNovaRightClickExperience()
{
	// nothing to do here
}

bool HyperNovaRightClickExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	// Currently, load a hard-coded user account. You can change it as needed.
	initSmartHomeUserAccount();
	std::shared_ptr<UserAccount> userAccount = accounts_[user_];

	auto accessToken = userAccount->accessToken();

	curlClient_ = std::make_shared<CurlClient>(accessToken, false, false);

	bufferPool_ = facebook::bufferpool::BufferPool::createBufferPool();

	std::string mediumUrl;

	// Check device type and request different camera
	switch (Platform::Meta::Quest::Device::deviceType())
	{
		case Platform::Meta::Quest::Device::DT_QUEST:
		case Platform::Meta::Quest::Device::DT_QUEST_2:
			mediumUrl = "LiveVideoId:0";
			Log::info() << "Camera Stream on Oculus: " + mediumUrl;
			break;
		case Platform::Meta::Quest::Device::DT_QUEST_PRO:
			// Only Use RGB camera. Referece: D33991910
			mediumUrl = "LiveVideoId:12";
			Log::info() << "Camera Stream on Arcata: " + mediumUrl;
			break;
		default:
			// TODO: support other platform
			renderErrorMessage(engine, "Device type not supported");
			return true;
	}

	Media::FrameMediumRef frameMedium;
	Media::PixelImageRef frameMediumOverlaid;
	try
	{
		frameMedium = Media::Manager::get().newMedium(mediumUrl);
		frameMediumOverlaid = Media::Manager::get().newMedium("PIXEL_IMAGE", Media::Medium::PIXEL_IMAGE);
	}
	catch (const std::exception& e)
	{
		renderErrorMessage(engine, e.what());
		return true;
	}

	absoluteTransform_ = engine->factory().createAbsoluteTransform();
	absoluteTransform_->setTransformationType(Rendering::AbsoluteTransform::TT_VIEW);
	absoluteTransform_->setTransformation(HomogenousMatrix4(Vector3(0, 0, -1)));
	experienceScene()->addChild(absoluteTransform_);

	if (frameMedium && frameMediumOverlaid)
	{
		Log::info() << "Frame initialized";
		frameMedium->start();
		frameMediumOverlaid->start();

		Rendering::TransformRef renderingTransform = Rendering::Utilities::createBox(
			engine,
			Vector3(Scalar(1), Scalar(1), 0),
			frameMediumOverlaid);
		renderingTransform->setVisible(false);
		absoluteTransform_->addChild(renderingTransform);

		renderingTransform_ = std::move(renderingTransform);
		frameMedium_ = std::move(frameMedium); // type is LIVE_VIDEO
		frameMediumOverlaid_ = std::move(frameMediumOverlaid);

		const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), textBackgroundColor,
			true, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE,
			std::string(), std::string(), &instructionsText_);
		textTransform->setTransformation(HomogenousMatrix4(instructionTransform));
		absoluteTransform_->addChild(textTransform);

		// Set up device status related components
		deviceStatus_ = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), textBackgroundColor,
			true, 0, 0, Scalar(0.04), Rendering::Text::AM_LEFT, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE,
			std::string(), std::string(), &deviceStatusText_);
		deviceStatusUpdateTS_ = timestamp;
		deviceStatusUpdateInterval_ = 3.0;
		/// World lock transformation
		auto world_lock_transform = Rendering::Transform::createTransformModifier();
		world_lock_transform->setTransformation(HomogenousMatrix4(deviceStatusTransform));
		deviceStatus_->setTransformModifier(world_lock_transform);
		deviceStatus_->setVisible(false);
		experienceScene()->addChild(deviceStatus_);
	}
	else
	{
		const std::string message =
			"Failed to access " + mediumUrl +
			" cameras \nEnsure that the OS is a userdev build and \n"
			"that persist.ovr.enable.sensorproxy is set to 'true' via \n"
			"adb root; adb shell setprop persist.ovr.enable.sensorproxy true; adb reboot \n"
			"Further, you need to pass the GK 'oculus_enable_mr_data_permission' you may need to reboot your device several times ";
		renderErrorMessage(engine, message);
		return true;
	}

	auto const categoryDetectorPath = build::getResourcePath("recognizer/fewshot_category_detector.jit");
	auto const classesPath = build::getResourcePath("recognizer/fewshot_classes.txt");
	auto const instanceDetectorPath = build::getResourcePath("recognizer/fewshot_instance_detector.jit");
	auto const hoiPredictorPath = build::getResourcePath("recognizer/hoi_fbnet_boltnn_model.jit");
	auto const localizationRoiFeatures = build::getResourcePath(userAccount->localizationRoiFeaturesPath);
	auto const classificationRoiFeatures = build::getResourcePath(userAccount->classificationRoiFeaturesPath);

	pixelBufferProvider_ = std::make_shared<
		arfx::events::Observable<std::shared_ptr<const facebook::arfx::recognizer::IData>>>();

	activator_ = std::make_shared<facebook::arfx::recognizer::ProxyActivator>(
		std::unordered_set<std::string>{
			facebook::arfx::recognizer::BoundingBoxAffordance::type(),
			facebook::arfx::recognizer::LabelAffordance::type()});

	activator_->setTargetsHandler([this](const std::vector<std::shared_ptr<const facebook::arfx::recognizer::RecognizedTarget>>& targets)
		{
			if (renderingMode_ != RenderingMode::CAPTURE)
			{
				return;
			}

			recognizedTargets_.clear();

			Log::info() << "Recognized " << targets.size() << " targets";
			for (size_t i = 0; i < targets.size(); ++i)
			{
				Log::info() << "Target " << i << " : " << targets[i]->targetId();
				if (targets[i]->intents().size() < 1)
				{ // continue when there is no intent for a target
					continue;
				}
				Log::info() << "Target has " << targets[i]->intents().size() << " intents";
				auto const& intents = targets[i]->intents();
				for (auto const& intent : intents)
				{
					Log::info() << "Intent : " << intent->intentType();
					if (intent->intentType() == SceneUnderstandingIntent::type())
					{
						auto const& sceneUnderstandingIntent = std::static_pointer_cast<const SceneUnderstandingIntent>(intent);
						auto const& sceneConcept = sceneUnderstandingIntent->sceneConcept();
						Log::info() << "Scene understanding intent: " << sceneConcept << ", score: " << sceneUnderstandingIntent->score();
						recognizedTargets_.emplace(targets[i]->targetId() + "_" + std::to_string(i), targets[i]);
					}
					else if (intent->intentType() == AddContactIntent::type())
					{
						auto const& addContactIntent = std::static_pointer_cast<const AddContactIntent>(intent);
						Log::info() << "Add contact intent: " << addContactIntent->getContactStr();
						recognizedTargets_.emplace(targets[i]->targetId(), targets[i]);
					}
					else if (intent->intentType() == CopyTextIntent::type())
					{
						auto const& copyTextIntent = std::static_pointer_cast<const CopyTextIntent>(intent);
						Log::info() << "Save text to a list: " << copyTextIntent->text();
						recognizedTargets_.emplace(targets[i]->targetId(), targets[i]);
					}
					else if (intent->intentType() == ObjectInstanceIntent::type())
					{
						auto const& objectInstanceIntent = std::static_pointer_cast<const ObjectInstanceIntent>(intent);
						Log::info() << "Object Instance intent: " << objectInstanceIntent->id() << ", " << objectInstanceIntent->score();
						recognizedTargets_.emplace(targets[i]->targetId(), targets[i]);
					}
				}
			}
		});
	facebook::arfx::recognizer::ModelContainer modelContainer;
	modelContainer.emplace(std::make_pair(
		facebook::arfx::recognizer::BoltFewshotCategoryDetector::detectionModelName(),
		std::make_shared<facebook::arfx::recognizer::SceneUnderstandingModel>(
			facebook::arfx::recognizer::BoltFewshotCategoryDetector::detectionModelName(), categoryDetectorPath, classesPath)));
	modelContainer.emplace(std::make_pair(
		facebook::arfx::recognizer::FewshotInstanceDeterminator::detectionModelName(),
		std::make_shared<facebook::arfx::recognizer::FewshotInstanceDetectorModel>(
			facebook::arfx::recognizer::FewshotInstanceDeterminator::detectionModelName(), instanceDetectorPath, localizationRoiFeatures, classificationRoiFeatures, *(userAccount->deviceViewIds()))));
	modelContainer.emplace(std::make_pair(
		facebook::arfx::recognizer::HOIPredictor::detectionModelName(),
		std::make_shared<PytorchModel>(HOIPredictor::detectionModelName(), hoiPredictorPath)));

	auto modelProvider = std::make_shared<facebook::arfx::recognizer::LocalModelProvider>(std::move(modelContainer));
	coordinator_ = std::make_unique<facebook::arfx::recognizer::RecognitionCoordinator>(
		std::make_shared<folly::QueuedImmediateExecutor>(),
		std::vector<std::shared_ptr<arfx::events::Subscribable<std::shared_ptr<const facebook::arfx::recognizer::IData>>>>{
			pixelBufferProvider_},
		std::vector<std::shared_ptr<facebook::arfx::recognizer::IIntentDeterminator>>{
			std::make_shared<facebook::arfx::recognizer::FewshotCategoryDeterminator>(
				std::make_unique<folly::CPUThreadPoolExecutor>(1),
				std::make_unique<folly::CPUThreadPoolExecutor>(1),
				std::make_unique<folly::CPUThreadPoolExecutor>(1),
				modelProvider,
				std::make_unique<BoltFewshotCategoryDetector>(),
				std::make_unique<BoltHOIPredictor>()),
			std::make_shared<facebook::arfx::recognizer::FewshotInstanceDeterminator>(
				std::make_unique<folly::CPUThreadPoolExecutor>(1),
				modelProvider),
			std::make_shared<TextActionsDeterminator>(std::make_unique<folly::CPUThreadPoolExecutor>(1), getEgoOcrResourceManager()),
		},
		std::make_shared<facebook::arfx::recognizer::IntentAggregator>(),
		std::vector<std::shared_ptr<facebook::arfx::recognizer::IActivator>>{activator_},
		std::vector<std::shared_ptr<facebook::arfx::recognizer::IIntentHandler>>{});
	coordinatorLogger_ = std::make_shared<facebook::arfx::recognizer::RecognitionCoordinatorDebugLogger>();
	coordinator_->addListener(coordinatorLogger_);
	coordinator_->start();

	// results menu
	resultsMenu_ = VRTableMenu(engine, engine->framebuffers().front());
	detailsMenu_ = VRTableMenu(engine, engine->framebuffers().front());
	shoplistMenu_ = VRTableMenu(engine, engine->framebuffers().front());
	imageVisualizer_ = std::make_shared<VRImageVisualizer>(engine, engine->framebuffers().front());

	// Disable timeout/retry logic in StatelessFastRPC wrapper
	HexagonSetUseStatelessFastRPC(false);

	return true;
}

bool HyperNovaRightClickExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	frameMedium_.release();
	frameMediumOverlaid_.release();
	renderingTransform_.release();
	absoluteTransform_.release();
	resultsMenu_.release();
	detailsMenu_.release();
	shoplistMenu_.release();
	instructionsText_.release();
	deviceStatusText_.release();

	return true;
}

void HyperNovaRightClickExperience::initSmartHomeUserAccount()
{
	// Zhicheng's account
	std::shared_ptr<UserAccount> zhichengYanAccount = std::make_shared<UserAccount>(
		build::getResourcePath("recognizer/few_shot_instance/users/zhichengyan/accounts.json"));
	std::shared_ptr<UserAccount> chenchenZhuAccount = std::make_shared<UserAccount>(
		build::getResourcePath("recognizer/few_shot_instance/users/chenchenzhu/accounts.json"));
	accounts_["Zhicheng Yan"] = zhichengYanAccount;
	accounts_["Chenchen Zhu"] = chenchenZhuAccount;

	// Add more user accounts below
}


void HyperNovaRightClickExperience::onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	switch (renderingMode_)
	{
		case RenderingMode::NONE:
			if (key == "A" || key == "X")
			{
				renderingMode_ = RenderingMode::CAPTURE;
			}
			if (key == "B" || key == "Y")
			{
				renderingMode_ = RenderingMode::SHOW_SHOPLIST;
			}
			break;
		case RenderingMode::CAPTURE:
			if (key == "A" || key == "X")
			{
				renderingMode_ = RenderingMode::SHOW_RESULTS;
			}
			if (key == "B" || key == "Y")
			{
				renderingMode_ = RenderingMode::NONE;
			}
			break;
		case RenderingMode::USER_SELECTING_RESULTS:
		case RenderingMode::USER_SELECTING_DETAILS:
		case RenderingMode::SHOW_SHOPLIST:
			if (key == "B" || key == "Y")
			{
				renderingMode_ = RenderingMode::NONE;
			}
			break;
		default:
			return;
	}
}

Timestamp HyperNovaRightClickExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (!frameMedium_)
	{
		return timestamp;
	}
	// On Arcata, frame pixel format is FORMAT_Y_UV12
	FrameRef frame = frameMedium_->frame(); // latest frame

	if (!frame)
	{
		return timestamp;
	}
	// render frame
	HomogenousMatrixD4 device_T_camera = frameMedium_->device_T_camera();
	device_T_camera = HomogenousMatrixD4(device_T_camera.translation() * 12.5, device_T_camera.rotationMatrix()); // increasing the translational part

	const Scalar scaleX = Scalar(frame->width()) * Scalar(0.002);
	const Scalar scaleY = Scalar(frame->height()) * Scalar(0.002);

	HomogenousMatrix4 transformation = HomogenousMatrix4(Vector3(0, 0, -2)) * HomogenousMatrix4(device_T_camera);
	transformation.applyScale(Vector3(scaleX, scaleY, 0));

	renderingTransform_->setTransformation(transformation);
	renderingTransform_->setVisible(true);

	bool useFrameOverlaid = false;
	auto deviceViewIdToDeviceId = *accounts_[user_]->deviceViewIdToDeviceId();
	switch (renderingMode_)
	{
		case RenderingMode::NONE: // Idle mode, waiting for user to trigger recognition
			{
				resultsMenu_.hide();
				detailsMenu_.hide();
				shoplistMenu_.hide();
				deviceStatus_->setVisible(false);
				clearSnapshot();
				instructionsText_->setText("Press A or X to capture\nPress B or Y to display your shopping list");
				break;
			}
		case RenderingMode::CAPTURE: // Recognition in progress
			{
				resultsMenu_.hide();
				detailsMenu_.hide();
				shoplistMenu_.hide();
				deviceStatus_->setVisible(false);
				instructionsText_->setText("Running recognition (Press A or X to show results. Press B or Y to cancel)");
				Log::info() << "running recognition";

				// pass frame to recognizer
				auto bgraFrameOpt = FrameUtilities::convertToBGRA(*frame);
				if (!bgraFrameOpt)
				{
					Log::info() << "image conversion failed";
					return timestamp;
				}
				auto bgraFrame = bgraFrameOpt.value();

				arfx::engine::fx::ImageMetadata imgMetadata;
				imgMetadata.width = bgraFrame.width();
				imgMetadata.height = bgraFrame.height();

				auto const pixelBufI420 = ServicePixelBufferFactory::fromARGBBuffer(
					bufferPool_,
					bgraFrame.constdata<uint8_t>(),
					imgMetadata,
					msqrd::PresentationTime(std::chrono::milliseconds(100)));

				if (pixelBufI420 == nullptr)
				{
					renderErrorMessage(engine, "Error creating I420 pixel buffer");
					return timestamp;
				}
				lastFrame_ = frame; // set lastFrame_ to the latest frame ingested to O.R.

				pixelBufferProvider_->notify(std::make_shared<facebook::arfx::recognizer::VideoBufferData>(pixelBufI420));

				// Caution: we display current frame while showing the detected instance bounding box from the closest prior frame where we run the instance detectionl.
				const size_t fontSize = 30;
				const Ocean::CV::Fonts::SharedFont font = Ocean::CV::Fonts::FontManager::get().font("Roboto", fontSize, "Regular");

				std::unordered_map<std::string, float> device_score_map;

				Log::info() << "before dedup, num targets " << recognizedTargets_.size();
				dedupRecognizedObjectInstance(recognizedTargets_, deviceViewIdToDeviceId);
				Log::info() << "after dedup, num targets " << recognizedTargets_.size();

				for (auto& result : recognizedTargets_)
				{
					auto bboxAffordance = result.second->affordanceOfType<BoundingBoxAffordance>();
					if (bboxAffordance == nullptr)
					{
						continue;
					}
					// Draw bounding box. If more intents are supported, use one color per intent
					// Use different color if detected object is HAND
					const uint8_t* color = nullptr;
					if (result.first.rfind("HAND_CONTACT", 0) == 0)
					{
						color = CV::Canvas::red((bgraFrame).pixelFormat());
					}
					else if (result.first.rfind("HAND", 0) == 0)
					{
						color = CV::Canvas::blue((bgraFrame).pixelFormat());
					}
					else
					{
						color = CV::Canvas::yellow((bgraFrame).pixelFormat());
					}
					auto bbox = bboxAffordance->boundingBox();
					CV::Canvas::box<5u>(bgraFrame, Ocean::Box2(bbox.width(), bbox.height(), Vector2(bbox.minX(), bbox.minY())), color);
					// Draw label nearby bounding box
					auto white = CV::Canvas::white((bgraFrame).pixelFormat());
					auto gray = CV::Canvas::gray((bgraFrame).pixelFormat());
					if (hasObjectInstanceIntent(result.second->intents()))
					{
						auto deviceId = deviceViewIdToDeviceId[std::stoi(result.first)];
						auto deviceName = accounts_[user_]->devices()[deviceId]->deviceName();
						font->drawText(bgraFrame, deviceName, bbox.minX(), bbox.minY(), white, gray);
					}
					else
					{
						auto labelAffordance = result.second->affordanceOfType<LabelAffordance>();
						if (labelAffordance != nullptr)
						{
							font->drawText(bgraFrame, labelAffordance->label(), bbox.minX(), bbox.minY(), white, gray);
						}
					}
				}

				frameOverlaid_ = std::move(FrameUtilities::convertFromBGRAToNV12(bgraFrame, bufferPool_).value());
				// We must copy timestamps. Otherwise, the frame won't be displayed
				frameOverlaid_.setTimestamp(frame->timestamp());
				frameOverlaid_.setRelativeTimestamp(frame->relativeTimestamp());
				useFrameOverlaid = true;

				break;
			}
		case RenderingMode::SHOW_RESULTS:
			{
				instructionsText_->setText("Select a result (Press B or Y to cancel)");
				VRTableMenu::Entries menuEntries = {};

				for (auto const& result : recognizedTargets_)
				{
					if (result.second->intents().empty() || result.second->affordancesByType().empty())
					{
						continue;
					}
					else if (result.first.rfind("HAND", 0) == 0)
					{
						continue;
					}
					for (auto const& affordance : result.second->affordancesByType())
					{
						auto type = affordance.first;
						if (type == LabelAffordance::type())
						{
							std::string label;
							if (hasObjectInstanceIntent(result.second->intents()))
							{
								auto deviceId = deviceViewIdToDeviceId[std::stoi(result.first)];
								label = accounts_[user_]->devices()[deviceId]->deviceName();
							}
							else
							{
								auto const labelAffordance = std::static_pointer_cast<const LabelAffordance>(affordance.second);
								label = labelAffordance->label();
							}
							menuEntries.push_back(VRTableMenu::Entry(label, result.first));
						}
					}
				}
				showVRTableMenu(resultsMenu_, std::move(menuEntries), "Recognized Results", timestamp);
				renderingMode_ = RenderingMode::USER_SELECTING_RESULTS;
				break;
			}
		case RenderingMode::USER_SELECTING_RESULTS:
			{
				detailsMenu_.hide();
				deviceStatus_->setVisible(false);
				if (!resultsMenu_.isShown())
				{
					resultsMenu_.show(resultsMenu_.base_T_menu());
				}
				selectedTarget_ = "";
				std::string entryUrl;
				if (resultsMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
				{
					resultsMenu_.hide();
					Log::info() << "results menu select: " << entryUrl.c_str();
					selectedTarget_ = entryUrl;
					renderingMode_ = RenderingMode::SHOW_DETAILS;
				}
				break;
			}
		case RenderingMode::SHOW_DETAILS:
			{
				instructionsText_->setText("Choose an action (Press B or Y to cancel)");
				ocean_assert(!resultsMenu_.isShown());
				ocean_assert(!selectedTarget_.empty());
				ocean_assert(recognizedTargets_.find(selectedTarget_) != recognizedTargets_.end());
				auto const target = recognizedTargets_[selectedTarget_];
				VRTableMenu::Entries menuEntries = {};
				std::string menuTitle = target->targetId();
				for (auto const& intent : target->intents())
				{
					auto type = intent->intentType();
					if (type == SceneUnderstandingIntent::type())
					{
						auto const sceneUnderstandingIntent = std::static_pointer_cast<const SceneUnderstandingIntent>(intent);
						menuEntries.push_back(VRTableMenu::Entry("Add to shopping list", kTargetActionShoplist));
						menuEntries.push_back(VRTableMenu::Entry("Back to results", kBackToResults));
					}
					else if (type == AddContactIntent::type())
					{
						auto const addContactIntent = std::static_pointer_cast<const AddContactIntent>(intent);
						menuEntries.push_back(VRTableMenu::Entry(
							"Add " + addContactIntent->getContactStr() + " to contacts", addContactIntent->getContactStr()));
					}
					else if (type == CopyTextIntent::type())
					{
						auto const copyTextIntent = std::static_pointer_cast<const CopyTextIntent>(intent);
						menuEntries.push_back(VRTableMenu::Entry("Save to list", copyTextIntent->text()));
					}
					else if (type == UrlIntent::type())
					{
						auto const urlIntent = std::static_pointer_cast<const UrlIntent>(intent);
						menuEntries.push_back(VRTableMenu::Entry("Open " + urlIntent->url(), urlIntent->url()));
					}
					else if (type == ObjectInstanceIntent::type())
					{
						auto deviceId = deviceViewIdToDeviceId[std::stoi(target->targetId())];
						deviceName_ = accounts_[user_]->devices()[deviceId]->deviceName();
						smartDevice_ = accounts_[user_]->devices()[deviceId]->device();
						std::shared_ptr<IComponent> mainComponent = smartDevice_->components()["main"];

						auto capabilities = mainComponent->capabilities();
						for (auto& capab : capabilities)
						{
							for (auto command : capab.second->commands())
							{
								auto commandStr = command->representation();
								auto url = type + std::string("/") + capab.first + std::string("/") + commandStr;
								menuEntries.push_back(VRTableMenu::Entry(commandStr, url));
								smartThingsCommands_[commandStr] = command;
							}
						}
						menuEntries.push_back(VRTableMenu::Entry("Back to results", kBackToResults));
						// Display device status. Need to get world-locking transform on-the-fly
						std::string deviceNameWithStatus = queryDeviceStatus();
						deviceStatusText_->setText(deviceNameWithStatus);
						deviceStatus_->setTransformation(HomogenousMatrix4(HeadsetPoses::world_T_device(PlatformSpecific::get().ovr(), timestamp)));
						deviceStatus_->setVisible(true);
					}
					else
					{
						Log::info() << "Unsupported intent";
					}
				}
				showVRTableMenu(detailsMenu_, std::move(menuEntries), deviceName_, timestamp);
				renderingMode_ = RenderingMode::USER_SELECTING_DETAILS;
				break;
			}
		case RenderingMode::USER_SELECTING_DETAILS:
			{
				ocean_assert(detailsMenu_.isShown());
				std::string entryUrl;
				// Update device status continously (every 3 seconds)
				if (smartDevice_ && double(timestamp - deviceStatusUpdateTS_) > deviceStatusUpdateInterval_)
				{
					deviceStatusText_->setText(queryDeviceStatus());
					deviceStatusUpdateTS_ = timestamp;
				}
				if (detailsMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
				{
					Log::info() << "detail menu select: " << entryUrl.c_str() << " for " << selectedTarget_;
					if (entryUrl == kTargetActionShoplist)
					{ // add to shopping list
						addToShoplist(selectedTarget_);
						renderingMode_ = RenderingMode::SHOW_SHOPLIST;
					}
					else if (entryUrl == kBackToResults)
					{ // back to results menu
						renderingMode_ = RenderingMode::USER_SELECTING_RESULTS;
						smartDevice_ = nullptr;
					}
					else
					{
						std::vector<std::string> urlParts;
						folly::split("/", entryUrl, urlParts);
						if (urlParts.size() > 0 && urlParts[0] == ObjectInstanceIntent::type())
						{
							Log::info() << TAG << "url parts: " << urlParts[0] << " " << urlParts[1] << " " << urlParts[2];
							ocean_assert(urlParts.size() == 3);
							auto capability = urlParts[1];
							auto commandStr = urlParts[2];
							assert(smartThingsCommands_.find(commandStr) != smartThingsCommands_.end());
							auto command = smartThingsCommands_[commandStr];
							std::string mainComponentId = "main";

							curlClient_->PostCommand(smartDevice_, mainComponentId, capability, *command);
							// update device status
							// deviceStatusText_->setText(queryDeviceStatus());
							Log::info() << TAG << "Done with command ";
						}
					}
				}
				break;
			}
		case RenderingMode::SHOW_SHOPLIST:
			{
				resultsMenu_.hide();
				detailsMenu_.hide();
				deviceStatus_->setVisible(false);
				instructionsText_->setText("Press B or Y to go back");
				auto shoplistItems = getShoplist();
				VRTableMenu::Entries menuEntries = {};
				for (auto it = shoplistItems.begin(); it != shoplistItems.end(); it++)
				{
					auto item = it->second;
					menuEntries.push_back(VRTableMenu::Entry(item.getMenuText(), it->first));
				}
				showVRTableMenu(shoplistMenu_, menuEntries, "Shopping List", timestamp);
				std::string entryUrl;
				if (shoplistMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
				{
					auto selectedItem = shoplistItems[entryUrl];
					auto frame = selectedItem.getImageFrame();
					if (frame.has_value())
					{
						showSnapshot(&frame.value());
					}
				}

				break;
			}
	}
	if (useFrameOverlaid)
	{
		frameMediumOverlaid_->setPixelImage(frameOverlaid_);
	}
	else
	{
		frameMediumOverlaid_->setPixelImage(*frame);
	}

	return timestamp;
}

std::unique_ptr<XRPlaygroundExperience> HyperNovaRightClickExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new HyperNovaRightClickExperience());
}

void HyperNovaRightClickExperience::showVRTableMenu(VRTableMenu& menu,
	VRTableMenu::Entries entries,
	const std::string& title,
	const Timestamp timestamp)
{
	const VRTableMenu::Group menuGroup(title, std::move(entries));
	menu.setMenuEntries(menuGroup, textBackgroundColor, textBackgroundColor);
	menu.show(HomogenousMatrix4(menuTransform), HeadsetPoses::world_T_device(PlatformSpecific::get().ovr(), timestamp));
}

void HyperNovaRightClickExperience::showSnapshot(Frame* frame)
{
	// Clear frame
	clearSnapshot();

	// Show frame slightly to the right of the menu
	imageVisualizer_->visualizeImageInView(0, HomogenousMatrix4(imageTransform), *frame, Scalar(0.75));
}

void HyperNovaRightClickExperience::clearSnapshot()
{
	// Clear frame
	imageVisualizer_->visualizeImageInView(0, HomogenousMatrix4(imageTransform), Frame(), Scalar(0.75));
}

void HyperNovaRightClickExperience::renderErrorMessage(const Rendering::EngineRef& engine, const std::string errorMessage)
{
	Log::error() << errorMessage;
	const Rendering::TransformRef transform = Rendering::Utilities::createText(
		*engine,
		errorMessage,
		RGBAColor(0.0f, 0.0f, 0.0f),
		RGBAColor(0.5f, 0.5f, 0.5f),
		true,
		0,
		0,
		Scalar(0.1),
		Rendering::Text::AM_CENTER,
		Rendering::Text::HA_CENTER,
		Rendering::Text::VA_MIDDLE);

	transform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2)));

	absoluteTransform_->addChild(transform);
}

std::map<std::string, HyperNovaRightClickExperience::ShoplistItem> HyperNovaRightClickExperience::getShoplist()
{
	return storage_;
}

void HyperNovaRightClickExperience::addToShoplist(const std::string& itemName)
{
	ShoplistItem item{itemName, millisecondsToDatetime(clock()), saveSnapshot()};
	storage_.emplace(itemName, item);
}

std::string HyperNovaRightClickExperience::saveSnapshot()
{
	if (!lastFrame_)
	{
		Log::warning() << "Last frame not available for snapshot";
		return "";
	}
	Log::info() << "Capture Sanpshot from last frame";

	const auto filename = "snapshot_" + String::toAString(double(lastFrame_->timestamp()), 6u) + ".jpg";
	const auto directory = getSnapshotDir();
	const IO::File imageFile(directory + IO::File(filename));
	if (!Media::Utilities::saveImage(*lastFrame_, imageFile(), false))
	{
		Log::warning() << "Failed to save snapshot to file: " << imageFile();
		return "";
	}
	return imageFile();
}

std::string HyperNovaRightClickExperience::queryDeviceStatus(const std::string componentID)
{
	if (!smartDevice_ || deviceName_.empty())
	{
		return "Smart Device not available.";
	}
	else if (!curlClient_)
	{
		return "curl client not avaiable.";
	}

	Json::Value status = curlClient_->GetStatus(smartDevice_, componentID);
	Json::Value displayStatus = smartDevice_->components()[componentID]->processStatus(status);
	std::stringstream deviceStatusStr;
	deviceStatusStr << deviceName_ << " Status:\n";
	for (const auto& key : displayStatus.getMemberNames())
	{
		deviceStatusStr << key << " : " << displayStatus[key] << "\n";
	}
	return deviceStatusStr.str();
}

} // namespace Ocean::XRPlayground

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
