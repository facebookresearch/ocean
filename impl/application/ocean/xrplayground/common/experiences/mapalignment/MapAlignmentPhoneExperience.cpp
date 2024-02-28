// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/mapalignment/MapAlignmentPhoneExperience.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolator.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/geometry/AbsoluteTransformation.h"

#include "ocean/io/Compression.h"

#include "ocean/network/tigon/TigonClient.h"

#include "ocean/network/verts/Manager.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/Textures.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/tracking/mapbuilding/Utilities.h"

#define USE_STEREO_RELOCALIZATION

#ifdef  USE_STEREO_RELOCALIZATION
	#include "ocean/tracking/mapbuilding/RelocalizerStereo.h"
#else
	#include "ocean/tracking/mapbuilding/TrackerMono.h"
#endif

namespace Ocean
{

using namespace Network;
using namespace Tracking;

namespace XRPlayground
{

bool MapAlignmentPhoneExperience::FeatureMap::latestMapForRendering(Vectors3& objectPoints)
{
	const ScopedLock scopedLock(lock_);

	if (!hasLatestMapForRendering_)
	{
		return false;
	}

	hasLatestMapForRendering_ = false;

	if (hasLatestMapForRelocalization_)
	{
		// we need to make a copy
		objectPoints = mapObjectPoints_;
	}
	else
	{
		objectPoints = std::move(mapObjectPoints_);
	}

	return true;
}

bool MapAlignmentPhoneExperience::FeatureMap::latestFeatureMapForRelocalization(MapBuilding::SharedUnifiedFeatureMap& unifiedFeatureMap)
{
	const ScopedLock scopedLock(lock_);

	if (!unifiedFeatureMap_)
	{
		return false;
	}

	unifiedFeatureMap = std::move(unifiedFeatureMap_);

	return true;
}

void MapAlignmentPhoneExperience::FeatureMap::onReceiveData(IO::InputBitstream& bitstream)
{
	Vectors3 mapObjectPoints;
	Indices32 mapObjectPointIds;
	std::shared_ptr<MapBuilding::UnifiedDescriptorMap> mapUnifiedDescriptorMap;

	unsigned long long mapTag = 0u;
	if (bitstream.read<unsigned long long>(mapTag) && mapTag == mapTag_
			&& readObjectPointsFromStream(bitstream, mapObjectPoints, mapObjectPointIds)
			&& MapBuilding::Utilities::readDescriptorMap(bitstream, mapUnifiedDescriptorMap))
	{
		ocean_assert(mapObjectPoints.size() == mapObjectPointIds.size());

		const ScopedLock scopedLock(lock_);

		hasLatestMapForRendering_ = true;
		hasLatestMapForRelocalization_ = true;

		mapObjectPoints_ = std::move(mapObjectPoints);
		mapObjectPointIds_ = std::move(mapObjectPointIds);
		mapUnifiedDescriptorMap_ = std::move(mapUnifiedDescriptorMap);

		if (!isThreadActive())
		{
			startThread();
		}
	}
	else
	{
		Log::error() << "Failed to decode map";
	}
}

void MapAlignmentPhoneExperience::FeatureMap::threadRun()
{
	RandomGenerator randomGenerator;

	while (!shouldThreadStop())
	{
		sleep(1u);

		TemporaryScopedLock scopedLock(lock_);

			if (!hasLatestMapForRelocalization_)
			{
				continue;
			}

			hasLatestMapForRelocalization_ = false;

			Vectors3 mapObjectPoints;

			if (hasLatestMapForRendering_)
			{
				// we need to make a copy
				mapObjectPoints = mapObjectPoints_;
			}
			else
			{
				mapObjectPoints = std::move(mapObjectPoints_);
			}

			Indices32 mapObjectPointIds = std::move(mapObjectPointIds_);
			std::shared_ptr<MapBuilding::UnifiedDescriptorMap> mapUnifiedDescriptorMap = std::move(mapUnifiedDescriptorMap_);

		scopedLock.release();

		ocean_assert(!mapObjectPoints.empty());

		using ImagePointDescriptor = MapBuilding::UnifiedDescriptor::FreakMultiDescriptor256;
		using ObjectPointDescriptor = MapBuilding::UnifiedDescriptor::FreakMultiDescriptors256;
		using ObjectPointVocabularyDescriptor = MapBuilding::UnifiedDescriptor::BinaryDescriptor<256u>;

		using UnifiedFeatureMap = MapBuilding::UnifiedFeatureMapT<ImagePointDescriptor, ObjectPointDescriptor, ObjectPointVocabularyDescriptor>;

		MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap = std::make_shared<UnifiedFeatureMap>(std::move(mapObjectPoints), std::move(mapObjectPointIds), std::move(mapUnifiedDescriptorMap), randomGenerator, &UnifiedFeatureMap::VocabularyForest::TVocabularyTree::determineClustersMeanForBinaryDescriptor<256u>, &Tracking::MapBuilding::UnifiedHelperFreakMultiDescriptor256::extractVocabularyDescriptorsFromMap);

		scopedLock.relock(lock_);

		unifiedFeatureMap_ = std::move(unifiedFeatureMap);
	}
}

MapAlignmentPhoneExperience::~MapAlignmentPhoneExperience()
{
	// nothing to do here
}

bool MapAlignmentPhoneExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

	if (!engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());

			frameMedium_ = undistortedBackground->medium();
		}
	}

	if (!anchoredContentManager_.initialize(std::bind(&MapAlignmentPhoneExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	const Rendering::AbsoluteTransformRef absoluteTransform = engine->factory().createAbsoluteTransform();
	absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
	absoluteTransform->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.065)));
	experienceScene()->addChild(absoluteTransform);

	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, " Start the Map Alignment experience \n in the headset ", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, Scalar(0.005), 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingText_);

	absoluteTransform->addChild(textTransform);

	// we need to determine a zone name which is unique for the user (user id does not work as user may be logged in with individual ids like Meta, Facebook, Oculus)

	std::string userName;
	if (!Network::Tigon::TigonClient::get().determineUserName(userName))
	{
		renderingText_->setText(" Failed to determine user name \n User needs to be logged in ");
		return true;
	}

	const std::string vertsZoneName = "XRPlayground://MAPALIGNMENT_ZONE_FOR_" + String::toAString(std::hash<std::string>()(userName));

	vertsDriver_ = Verts::Manager::get().driver(vertsZoneName);

	if (vertsDriver_)
	{
		changedUsersScopedSubscription_ = vertsDriver_->addChangedUsersCallback(std::bind(&MapAlignmentPhoneExperience::onChangedUsers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		receiveContainerScopedSubscription_ = vertsDriver_->addReceiveContainerCallback(std::bind(&MapAlignmentPhoneExperience::onReceiveContainer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

		Log::info() << "Created VERTS driver with zone name '" << vertsZoneName << "'";
	}

	Verts::NodeSpecification& devicePoseSpecification = Verts::NodeSpecification::newNodeSpecification("DevicePose");
	devicePoseSpecification.registerField<std::string>("hmdWorld_T_slamCamera");
	devicePoseSpecification.registerField<std::string>("hmdWorld_T_relocalizationCamera");

	Rendering::AbsoluteTransformRef absoluteTransformDebug = engine->factory().createAbsoluteTransform();
	absoluteTransformDebug->setTransformationType(Rendering::AbsoluteTransform::TT_VIEW);
	experienceScene()->addChild(absoluteTransformDebug);

	Rendering::AttributeSetRef attributeSet;
	renderingTransformDebug_ = Rendering::Utilities::createBox(engine, Vector3(1, 1, 0), nullptr, &attributeSet);
	renderingTransformDebug_->setVisible(false);
	absoluteTransformDebug->addChild(renderingTransformDebug_);

	Rendering::TexturesRef textures = engine->factory().createTextures();
	renderingFrameTextureDebug_ = engine->factory().createFrameTexture2D();
	textures->addTexture(renderingFrameTextureDebug_);
	attributeSet->addAttribute(textures);

	return true;
}

bool MapAlignmentPhoneExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	changedUsersScopedSubscription_.release();
	receiveContainerScopedSubscription_.release();

	stopThreadExplicitly();

	trackerSampleEventSubscription_.release();

	renderingFrameTextureDebug_.release();
	renderingTransformDebug_.release();

	renderingTransformContent_.release();

	anchoredContentManager_.release();

	slamTracker6DOF_.release();

	vertsDriver_ = nullptr;

	return true;
}

Timestamp MapAlignmentPhoneExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (!renderingTransformContent_ && objectId_ != Devices::Tracker6DOF::invalidObjectId())
	{
		renderingTransformContent_ = engine->factory().createTransform();

		constexpr Scalar visibilityRadius = Scalar(1000); // 1km
		constexpr Scalar engagementRadius = Scalar(10000);
		anchoredContentManager_.addContent(renderingTransformContent_, slamTracker6DOF_, objectId_, visibilityRadius, engagementRadius);
	}

	if (renderingTransformContent_)
	{
		const HomogenousMatrix4 hmdWorld_T_phoneWorld = hmdWorld_T_phoneWorld_.transformation(timestamp);

		if (hmdWorld_T_phoneWorld.isValid())
		{
			const HomogenousMatrix4 phoneWorld_T_hmdWorld(hmdWorld_T_phoneWorld.inverted());

			renderingTransformContent_->setTransformation(phoneWorld_T_hmdWorld);

			Vectors3 objectPoints;
			if (featureMap_.latestMapForRendering(objectPoints))
			{
				renderingTransformContent_->clear();
				renderingTransformContent_->addChild(Rendering::Utilities::createPoints(*engine, objectPoints, RGBAColor(1.0f, 0.0f, 0.0), Scalar(7)));
			}
		}
	}

#ifndef USE_STEREO_RELOCALIZATION

	if (MapBuilding::TrackerMono::RelocalizerDebugElements::get().isElementActive(MapBuilding::TrackerMono::RelocalizerDebugElements::EI_CAMERA_IMAGE_WITH_FEATURE_CORRESPONDENCES))
	{
		Frame debugFrame = MapBuilding::TrackerMono::RelocalizerDebugElements::get().element(MapBuilding::TrackerMono::RelocalizerDebugElements::EI_CAMERA_IMAGE_WITH_FEATURE_CORRESPONDENCES, true);

		if (debugFrame)
		{
			renderingTransformDebug_->setTransformation(HomogenousMatrix4(Vector3(0, 0, Scalar(-0.1)), Quaternion(Vector3(0, 0, 1), -Numeric::pi_2()), Vector3(Scalar(1), Scalar(debugFrame.height()) / Scalar(debugFrame.width()), 0) * Scalar(0.1)));
			renderingFrameTextureDebug_->setTexture(std::move(debugFrame));
		}
	}

#endif // USE_STEREO_RELOCALIZATION

	const Verts::SharedNode vertsDevicePoseNode(vertsDevicePoseNode_);

	if (vertsDevicePoseNode)
	{
		const ScopedLock scopedLock(lock_);

		if (hmdWorld_T_slamCamera_.isValid() || hmdWorld_T_relocalizationCamera_.isValid())
		{
			vertsDevicePoseNode->setField<Verts::Node::StringPointer>("hmdWorld_T_slamCamera", Verts::Node::StringPointer(HomogenousMatrixD4(hmdWorld_T_slamCamera_).data(), sizeof(HomogenousMatrixD4)));
			vertsDevicePoseNode->setField<Verts::Node::StringPointer>("hmdWorld_T_relocalizationCamera", Verts::Node::StringPointer(HomogenousMatrixD4(hmdWorld_T_relocalizationCamera_).data(), sizeof(HomogenousMatrixD4)));

			hmdWorld_T_slamCamera_.toNull();
			hmdWorld_T_relocalizationCamera_.toNull();
		}
	}
	else if (vertsDriver_->isInitialized())
	{
		Verts::SharedEntity entity = vertsDriver_->newEntity({"DevicePose"});

		if (entity)
		{
			vertsDevicePoseNode_ = entity->node("DevicePose");
		}

		if (!vertsDevicePoseNode_)
		{
			renderingText_->setText("Networking failure");
		}
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

void MapAlignmentPhoneExperience::onMouseRelease(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp)
{
	if (!isThreadActive())
	{
		return;
	}

#ifndef USE_STEREO_RELOCALIZATION

	constexpr MapBuilding::TrackerMono::RelocalizerDebugElements::ElementId elementId = MapBuilding::TrackerMono::RelocalizerDebugElements::ElementId::EI_CAMERA_IMAGE_WITH_FEATURE_CORRESPONDENCES;

	if (MapBuilding::TrackerMono::RelocalizerDebugElements::get().isElementActive(elementId))
	{
		MapBuilding::TrackerMono::RelocalizerDebugElements::get().deactivateElement(elementId);

		renderingTransformDebug_->setVisible(false);
	}
	else
	{
		MapBuilding::TrackerMono::RelocalizerDebugElements::get().activateElement(elementId);

		renderingTransformDebug_->setVisible(true);
	}

#endif // USE_STEREO_RELOCALIZATION
}

bool MapAlignmentPhoneExperience::start()
{
	const ScopedLock scopedLock(lock_);

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	slamTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF World Tracker");
#else
	slamTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF World Tracker");
#endif

	if (!slamTracker6DOF_ || !frameMedium_)
	{
		Log::error() << "MapAlignmentPhone Experience could not access the world tracker";
		return false;
	}

	if (Devices::VisualTrackerRef visualTracker = slamTracker6DOF_)
	{
		visualTracker->setInput(frameMedium_);
	}

	objectId_ = slamTracker6DOF_->objectId("World");

	trackerSampleEventSubscription_ = slamTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &MapAlignmentPhoneExperience::onTrackerSample));

	if (slamTracker6DOF_.isNull() || !slamTracker6DOF_->start())
	{
		Log::error() << "MapAlignmentPhone Experience could not start the world tracker";
		return false;
	}

	startThread();

	return true;
}

bool MapAlignmentPhoneExperience::stop()
{
	const ScopedLock scopedLock(lock_);

	stopThread();

	if (slamTracker6DOF_ && !slamTracker6DOF_->stop())
	{
		return false;
	}

	slamTracker6DOF_.release();

	return true;
}

std::unique_ptr<XRPlaygroundExperience> MapAlignmentPhoneExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new MapAlignmentPhoneExperience());
}

void MapAlignmentPhoneExperience::onTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
{
	const Devices::Tracker6DOF::Tracker6DOFSampleRef trackerSample = sample;
	ocean_assert(trackerSample);

	if (trackerSample->objectIds().empty())
	{
		return;
	}

	const HomogenousMatrix4 hmdWorld_T_phoneWorld = hmdWorld_T_phoneWorld_.transformation(trackerSample->timestamp());

	if (!hmdWorld_T_phoneWorld.isValid())
	{
		return;
	}

	ocean_assert(trackerSample->referenceSystem() == Devices::Tracker6DOF::RS_DEVICE_IN_OBJECT);
	const HomogenousMatrix4 phoneWorld_T_camera(trackerSample->positions().front(), trackerSample->orientations().front());

	HomogenousMatrix4 hmdWorld_T_camera = hmdWorld_T_phoneWorld * phoneWorld_T_camera;

	// we remove the scale from the transformation
	hmdWorld_T_camera = HomogenousMatrix4(hmdWorld_T_camera.translation(), hmdWorld_T_camera.rotation());

	const ScopedLock scopedLock(lock_);

	hmdWorld_T_slamCamera_ = hmdWorld_T_camera;
}

void MapAlignmentPhoneExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

void MapAlignmentPhoneExperience::threadRun()
{
	ocean_assert(frameMedium_);

#ifdef USE_STEREO_RELOCALIZATION
	MapBuilding::RelocalizerStereo relocalizerStereo(Tracking::MapBuilding::Relocalizer::detectFreakFeatures);
#else
	MapBuilding::TrackerMono trackerMono(Tracking::MapBuilding::Relocalizer::detectFreakFeatures);
#endif

	Timestamp lastFrameTimestamp(false);

	HomogenousMatrices4 phoneWorld_T_cameras;
	HomogenousMatrices4 hmdWorld_T_cameras;

	phoneWorld_T_cameras.reserve(128);
	hmdWorld_T_cameras.reserve(128);

	RandomGenerator randomGenerator;

#ifdef USE_STEREO_RELOCALIZATION
	Frame yFrameA;
	Frame yFrameB;

	SharedAnyCamera cameraA;
	SharedAnyCamera cameraB;

	HomogenousMatrix4 device_T_cameraA(false);
	HomogenousMatrix4 device_T_cameraB(false);

	HomogenousMatrix4 phoneWorld_T_cameraA(false);
	HomogenousMatrix4 phoneWorld_T_cameraB(false);
#endif

	while (!shouldThreadStop())
	{
		MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap;

#ifdef USE_STEREO_RELOCALIZATION
		if (featureMap_.latestFeatureMapForRelocalization(unifiedFeatureMap))
		{
			relocalizerStereo.setFeatureMap(std::move(unifiedFeatureMap));
		}

		if (!relocalizerStereo.isValid())
		{
			sleep(5u);
			continue;
		}
#else
		if (featureMap_.latestFeatureMapForRelocalization(unifiedFeatureMap))
		{
			trackerMono.setFeatureMap(std::move(unifiedFeatureMap));
		}

		if (!trackerMono.isValid())
		{
			sleep(5u);
			continue;
		}
#endif // USE_STEREO_RELOCALIZATION

		SharedAnyCamera camera;
		const FrameRef frame = frameMedium_->frame(&camera);

		if (frame.isNull() || frame->timestamp() <= lastFrameTimestamp)
		{
			sleep(1u);
			continue;
		}

		ocean_assert(camera);

		lastFrameTimestamp = frame->timestamp();

		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(*frame, FrameType(*frame, FrameType::FORMAT_Y8), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		CV::FrameInterpolator::resize(yFrame, yFrame.width() * 375u / 1000u, yFrame.height() * 375u / 1000u);

		camera = camera->clone(yFrame.width(), yFrame.height());
		if (!camera)
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		const Devices::Tracker6DOF::Tracker6DOFSampleRef sample = slamTracker6DOF_->sample(lastFrameTimestamp);

		if (sample.isNull() || sample->timestamp() != lastFrameTimestamp || sample->objectIds().empty())
		{
			Log::debug() << "Missing pose for camera frame, e.g., SLAM not initialized";

			continue;
		}

		ocean_assert(sample->referenceSystem() == Devices::Tracker6DOF::RS_DEVICE_IN_OBJECT);
		const HomogenousMatrix4 phoneWorld_T_camera(sample->positions().front(), sample->orientations().front());
		ocean_assert(phoneWorld_T_camera.isValid());

#ifdef USE_STEREO_RELOCALIZATION

		if (yFrameA.isNull())
		{
			yFrameA = std::move(yFrame);
			cameraA = std::move(camera);
			device_T_cameraA.toIdentity();
			phoneWorld_T_cameraA = phoneWorld_T_camera;

			continue;
		}

		const HomogenousMatrix4 cameraA_T_cameraB(phoneWorld_T_cameraA.inverted() * phoneWorld_T_camera);

		Vector3 cameraA_t_cameraB(cameraA_T_cameraB.translation());
		cameraA_t_cameraB.z() = 0; // we want baseline not along depth

		if (cameraA_t_cameraB.sqr() < Numeric::sqr(Scalar(0.04)))
		{
			// too close to each other
			continue;
		}

		yFrameB = std::move(yFrame);
		cameraB = std::move(camera);
		device_T_cameraB = cameraA_T_cameraB;
		phoneWorld_T_cameraB = phoneWorld_T_camera;

		HomogenousMatrix4 hmdWorld_T_roughDevice(false);

		const Timestamp latestTransformationTimestamp = hmdWorld_T_phoneWorld_.timestamp();

		if (latestTransformationTimestamp + 2.0 > lastFrameTimestamp)
		{
			HomogenousMatrix4 hmdWorld_T_phoneWorld = hmdWorld_T_phoneWorld_.transformation(lastFrameTimestamp);
			if (hmdWorld_T_phoneWorld.isValid())
			{
				const HomogenousMatrix4 cameraB_T_device(device_T_cameraB.inverted());

				hmdWorld_T_roughDevice = hmdWorld_T_phoneWorld * phoneWorld_T_cameraB * cameraB_T_device;
				hmdWorld_T_roughDevice = HomogenousMatrix4(hmdWorld_T_roughDevice.translation(), hmdWorld_T_roughDevice.rotation()); // removing potential scale
			}
		}

		HomogenousMatrix4 hmdWorld_T_device(false);

		constexpr unsigned int minimalNumberCorrespondences = 160u;
		constexpr Scalar maximalProjectionError = Scalar(4.0);
		constexpr Scalar inlierRate = Scalar(0.15);

		if (relocalizerStereo.relocalize(*cameraA, *cameraB, device_T_cameraA, device_T_cameraB, yFrameA, yFrameB, hmdWorld_T_device, minimalNumberCorrespondences, maximalProjectionError, inlierRate, hmdWorld_T_roughDevice))
		{
			ocean_assert(hmdWorld_T_device.isValid());

			const HomogenousMatrix4 hmdWorld_T_camera(hmdWorld_T_device * device_T_cameraB);

			TemporaryScopedLock scopedLock(lock_);
				hmdWorld_T_relocalizationCamera_ = hmdWorld_T_camera;
			scopedLock.release();

			phoneWorld_T_cameras.emplace_back(phoneWorld_T_camera);
			hmdWorld_T_cameras.emplace_back(hmdWorld_T_camera);

			constexpr size_t maxPosePairs = 200;

			if (phoneWorld_T_cameras.size() >= maxPosePairs * 10)
			{
				// removing the unused poses
				phoneWorld_T_cameras = HomogenousMatrices4(phoneWorld_T_cameras.data() + phoneWorld_T_cameras.size() - maxPosePairs, phoneWorld_T_cameras.data() + phoneWorld_T_cameras.size());
				hmdWorld_T_cameras = HomogenousMatrices4(hmdWorld_T_cameras.data() + hmdWorld_T_cameras.size() - maxPosePairs, hmdWorld_T_cameras.data() + hmdWorld_T_cameras.size());
			}

			const size_t posePairs = std::min(phoneWorld_T_cameras.size(), maxPosePairs);
			const size_t poseOffset = phoneWorld_T_cameras.size() - posePairs;
			ocean_assert(poseOffset + posePairs == phoneWorld_T_cameras.size());

			HomogenousMatrix4 hmdWorld_T_phoneWorld(false);
			Scalar scale;
			if (Geometry::AbsoluteTransformation::calculateTransformationWithOutliers(phoneWorld_T_cameras.data() + poseOffset, hmdWorld_T_cameras.data() + poseOffset, posePairs, hmdWorld_T_phoneWorld, Scalar(0.5), Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric, &scale) && scale >= Scalar(0.9) && scale <= Scalar(1.1))
			{
				hmdWorld_T_phoneWorld.applyScale(Vector3(scale, scale, scale));

				hmdWorld_T_phoneWorld_.setTransformation(hmdWorld_T_phoneWorld, lastFrameTimestamp);
			}
		}

		yFrameA.release();
		yFrameB.release();

#else // USE_STEREO_RELOCALIZATION

		HomogenousMatrix4 hmdWorld_T_roughCamera(false);

		const Timestamp latestTransformationTimestamp = hmdWorld_T_phoneWorld_.timestamp();

		if (latestTransformationTimestamp + 2.0 > lastFrameTimestamp)
		{
			HomogenousMatrix4 hmdWorld_T_phoneWorld = hmdWorld_T_phoneWorld_.transformation(lastFrameTimestamp);
			if (hmdWorld_T_phoneWorld.isValid())
			{
				hmdWorld_T_roughCamera = hmdWorld_T_phoneWorld * phoneWorld_T_camera;
				hmdWorld_T_roughCamera = HomogenousMatrix4(hmdWorld_T_roughCamera.translation(), hmdWorld_T_roughCamera.rotation()); // removing potential scale
			}
		}

		HomogenousMatrix4 hmdWorld_T_camera(false);

		constexpr unsigned int minimalNumberCorrespondences = 65u;
		constexpr Scalar maximalProjectionError = Scalar(3.5);

		if (trackerMono.track(*camera, yFrame, hmdWorld_T_camera, minimalNumberCorrespondences, maximalProjectionError, hmdWorld_T_roughCamera))
		{
			ocean_assert(hmdWorld_T_camera.isValid());

			{
				std::ostringstream stringStream(std::ios::binary);
				IO::OutputBitstream bitstream(stringStream);

				if (writeTransformationToStream(hmdWorld_T_camera, false /*slamBased*/, bitstream))
				{
					const std::string data = stringStream.str();
					ocean_assert(!data.empty());

					if (udpClient_.send(address, udpPort, data.c_str(), data.size()) != Network::Socket::SR_SUCCEEDED)
					{
						Log::error() << "Failed to send device pose";
					}
				}
			}

			phoneWorld_T_cameras.emplace_back(phoneWorld_T_camera);
			hmdWorld_T_cameras.emplace_back(hmdWorld_T_camera);

			constexpr size_t maxPosePairs = 200;

			if (phoneWorld_T_cameras.size() >= maxPosePairs * 10)
			{
				// removing the unused poses
				phoneWorld_T_cameras = HomogenousMatrices4(phoneWorld_T_cameras.data() + phoneWorld_T_cameras.size() - maxPosePairs, phoneWorld_T_cameras.data() + phoneWorld_T_cameras.size());
				hmdWorld_T_cameras = HomogenousMatrices4(hmdWorld_T_cameras.data() + hmdWorld_T_cameras.size() - maxPosePairs, hmdWorld_T_cameras.data() + hmdWorld_T_cameras.size());
			}

			const size_t posePairs = std::min(phoneWorld_T_cameras.size(), maxPosePairs);
			const size_t poseOffset = phoneWorld_T_cameras.size() - posePairs;
			ocean_assert(poseOffset + posePairs == phoneWorld_T_cameras.size());

			HomogenousMatrix4 hmdWorld_T_phoneWorld(false);
			Scalar scale;
			if (Geometry::AbsoluteTransformation::calculateTransformationWithOutliers(phoneWorld_T_cameras.data() + poseOffset, hmdWorld_T_cameras.data() + poseOffset, posePairs, hmdWorld_T_phoneWorld, Scalar(0.5), Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric, &scale) && scale >= Scalar(0.9) && scale <= Scalar(1.1))
			{
				hmdWorld_T_phoneWorld.applyScale(Vector3(scale, scale, scale));

				hmdWorld_T_phoneWorld_.setTransformation(hmdWorld_T_phoneWorld, lastFrameTimestamp);
			}
		}

#endif // USE_STEREO_RELOCALIZATION
	}
}

void MapAlignmentPhoneExperience::onChangedUsers(Network::Verts::Driver& driver, const UnorderedIndexSet64& addedUsers, const UnorderedIndexSet64& removedUsers)
{
	const ScopedLock scopedLock(lock_);

	const size_t usersBefore = userIds_.size();

	for (const Index64 userId : addedUsers)
	{
		userIds_.emplace(userId);
	}

	for (const Index64 userId : removedUsers)
	{
		userIds_.erase(userId);
	}

	if (usersBefore == 0 && !userIds_.empty())
	{
		renderingText_->setText("");

		start();
	}
	else if (usersBefore != 0 && userIds_.empty())
	{
		stop();

		renderingText_->setText(" Start Map Alignment \n experience on your headset ");
	}
}

void MapAlignmentPhoneExperience::onReceiveContainer(Network::Verts::Driver& driver, const uint32_t sessionId, const uint64_t userId, const std::string& identifier, const unsigned int version, const Network::Verts::Driver::SharedBuffer& buffer)
{
	static unsigned int previousVersion = 0u;

	if (version <= previousVersion)
	{
		// we skip this map, as we have received a newer map already
		return;
	}

	previousVersion = version;

	std::vector<uint8_t> decompressedBuffer;
	if (IO::Compression::gzipDecompress(buffer->data(), buffer->size(), decompressedBuffer))
	{
		const std::string inputString((const char*)(decompressedBuffer.data()), decompressedBuffer.size());

		std::istringstream stringStream(inputString, std::ios::binary);
		IO::InputBitstream bitstream(stringStream);

		unsigned long long tag = 0u;
		if (bitstream.look<unsigned long long>(tag))
		{
			if (tag == mapTag_)
			{
				featureMap_.onReceiveData(bitstream);
			}
		}
	}
}

}

}
