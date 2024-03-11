// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/mrroomplan/MRRoomPlanPhoneExperience.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolator.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/geometry/AbsoluteTransformation.h"

#include "ocean/io/Compression.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/Textures.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/tracking/mapbuilding/Utilities.h"

#include "metaonly/ocean/network/tigon/TigonClient.h"

#include "metaonly/ocean/network/verts/Manager.h"

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

bool MRRoomPlanPhoneExperience::FeatureMap::latestMapForRendering(Vectors3& objectPoints)
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

bool MRRoomPlanPhoneExperience::FeatureMap::latestFeatureMapForRelocalization(MapBuilding::SharedUnifiedFeatureMap& unifiedFeatureMap)
{
	const ScopedLock scopedLock(lock_);

	if (!unifiedFeatureMap_)
	{
		return false;
	}

	unifiedFeatureMap = std::move(unifiedFeatureMap_);

	return true;
}

void MRRoomPlanPhoneExperience::FeatureMap::onReceiveData(IO::InputBitstream& bitstream)
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

void MRRoomPlanPhoneExperience::FeatureMap::threadRun()
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

MRRoomPlanPhoneExperience::~MRRoomPlanPhoneExperience()
{
	// nothing to do here
}

bool MRRoomPlanPhoneExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
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

	if (!anchoredContentManager_.initialize(std::bind(&MRRoomPlanPhoneExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	const Rendering::AbsoluteTransformRef absoluteTransform = engine->factory().createAbsoluteTransform();
	absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
	absoluteTransform->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.065)));
	experienceScene()->addChild(absoluteTransform);

	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, " Start the 'MR Room Plan' \n experience on your headset ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.0004), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingText_);

	absoluteTransform->addChild(textTransform);

	// we need to determine a zone name which is unique for the user (user id does not work as user may be logged in with individual ids like Meta, Facebook, Oculus)

	std::string userName;
	if (!Network::Tigon::TigonClient::get().determineUserName(userName))
	{
		renderingText_->setText(" Failed to determine user name \n User needs to be logged in ");
		return true;
	}

	const std::string vertsZoneName = "XRPlayground://MRROOMPLAN_ZONE_FOR_" + String::toAString(std::hash<std::string>()(userName));

	vertsDriver_ = Verts::Manager::get().driver(vertsZoneName);

	if (vertsDriver_)
	{
		changedUsersScopedSubscription_ = vertsDriver_->addChangedUsersCallback(std::bind(&MRRoomPlanPhoneExperience::onChangedUsers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		receiveContainerScopedSubscription_ = vertsDriver_->addReceiveContainerCallback(std::bind(&MRRoomPlanPhoneExperience::onReceiveContainer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

		Log::info() << "Created VERTS driver with zone name '" << vertsZoneName << "'";
	}

	Verts::NodeSpecification& devicePoseSpecification = Verts::NodeSpecification::newNodeSpecification("DevicePose");
	devicePoseSpecification.registerField<std::string>("hmdWorld_T_slamCamera");
	devicePoseSpecification.registerField<std::string>("instruction");

	return true;
}

bool MRRoomPlanPhoneExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	changedUsersScopedSubscription_.release();
	receiveContainerScopedSubscription_.release();

	stopThreadExplicitly();

	trackerSampleEventSubscription_.release();

	renderingTransformContent_.release();

	anchoredContentManager_.release();

	roomPlanTracker6DOF_.release();

	vertsDriver_ = nullptr;

	return true;
}

Timestamp MRRoomPlanPhoneExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);
		Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap pendingRoomObjectMap;
		std::swap(pendingRoomObjectMap, pendingRoomObjectMap_);
	scopedLock.release();

	if (!renderingTransformContent_ && objectId_ != Devices::Tracker6DOF::invalidObjectId())
	{
		renderingTransformContent_ = engine->factory().createTransform();

		constexpr Scalar visibilityRadius = Scalar(1000); // 1km
		constexpr Scalar engagementRadius = Scalar(10000);
		anchoredContentManager_.addContent(renderingTransformContent_, roomPlanTracker6DOF_, objectId_, visibilityRadius, engagementRadius);
	}

	if (renderingTransformContent_ && !pendingRoomObjectMap.empty())
	{
		renderingTransformContent_->clear();

		for (Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap::const_iterator iRoomObject = pendingRoomObjectMap.cbegin(); iRoomObject != pendingRoomObjectMap.cend(); ++iRoomObject)
		{
			const Devices::SceneTracker6DOF::SceneElementRoom::SharedRoomObject& roomObject = iRoomObject->second;

			Rendering::TransformRef transform;

			if (roomObject->objectType() == Devices::SceneTracker6DOF::SceneElementRoom::RoomObject::OT_PLANAR)
			{
				const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject& planarRoomObject = (const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject&)(*roomObject);

				Vector3 dimension = roomObject->dimension();

				const Scalar thickness = planarThickness(planarRoomObject.planarType());
				dimension = Vector3(std::max(dimension.x(), thickness), std::max(dimension.y(), thickness), std::max(dimension.z(), thickness));

				transform = Rendering::Utilities::createBox(engine, dimension, planarColor(planarRoomObject.planarType()));
			}
			else
			{
				const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject& volumetricRoomObject = (const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject&)(*roomObject);

				const Vector3 dimension = adjustedVolumetricDimension(volumetricRoomObject.volumetricType(), roomObject->dimension());

				transform = Rendering::Utilities::createBox(engine, dimension, volumetricColor(volumetricRoomObject.volumetricType()));

				const std::string volumeticType = Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::translateVolumetricType(volumetricRoomObject.volumetricType());
				const float confidence = volumetricRoomObject.confidence();

				const Rendering::TransformRef text = Rendering::Utilities::createText(*engine, " " + volumeticType + ", " + String::toAString(confidence, 1u) + " ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, 0.075, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
				text->setTransformation(HomogenousMatrix4(Vector3(0, dimension.y() * Scalar(0.5) + Scalar(0.05), 0)));

				transform->addChild(text);

				const Rendering::TransformRef backsideText = engine->factory().createTransform();
				backsideText->addChild(text);
				backsideText->setTransformation(HomogenousMatrix4(Quaternion(Vector3(0, 1, 0), Numeric::pi())));

				transform->addChild(backsideText);
			}

			transform->setTransformation(roomObject->world_T_object());

			renderingTransformContent_->addChild(transform);
		}
	}

	const Verts::SharedNode vertsDevicePoseNode(vertsDevicePoseNode_);

	if (vertsDevicePoseNode)
	{
		scopedLock.relock(lock_);

		if (hmdWorld_T_slamCamera_.isValid())
		{
			vertsDevicePoseNode->setField<Verts::Node::StringPointer>(0u, Verts::Node::StringPointer(HomogenousMatrixD4(hmdWorld_T_slamCamera_).data(), sizeof(HomogenousMatrixD4)));

			hmdWorld_T_slamCamera_.toNull();
		}

		vertsDevicePoseNode->setField<std::string>(1u, instruction_);

		scopedLock.release();
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

	if (!pendingRoomObjectMap.empty() && vertsDriver_ && vertsDriver_->isInitialized())
	{
		if (sentNextRoomObjectsTimestamp_.isInvalid() || timestamp >= sentNextRoomObjectsTimestamp_)
		{
			std::ostringstream stringStream(std::ios::binary);
			IO::OutputBitstream bitstream(stringStream);

			const HomogenousMatrix4 hmdWorld_T_phoneWorld(hmdWorld_T_phoneWorld_.transformation(timestamp));

			if (hmdWorld_T_phoneWorld.isValid())
			{
				for (Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap::iterator iObject = pendingRoomObjectMap.begin(); iObject != pendingRoomObjectMap.end(); ++iObject)
				{
					const HomogenousMatrix4& phoneWorld_T_object = iObject->second->world_T_object();
					iObject->second->setWorld_T_object(hmdWorld_T_phoneWorld * phoneWorld_T_object);
				}

				if (writeRoomObjectsToStream(pendingRoomObjectMap, bitstream))
				{
					const std::string roomData = stringStream.str();
					ocean_assert(!roomData.empty());

					IO::Compression::Buffer compressedBuffer;
					if (IO::Compression::gzipCompress(roomData.c_str(), roomData.size(), compressedBuffer))
					{
						static unsigned int roomObjectVersion = 0u;
						vertsDriver_->sendContainer("roomobjects", ++roomObjectVersion, compressedBuffer.data(), compressedBuffer.size());

						sentNextRoomObjectsTimestamp_ = timestamp + 2.0;
					}
				}
			}
		}
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

bool MRRoomPlanPhoneExperience::start()
{
	const ScopedLock scopedLock(lock_);

	roomPlanTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Room Plan Tracker");

	if (!roomPlanTracker6DOF_ || !frameMedium_)
	{
		Log::error() << "MRRoomPlanPhone Experience could not access the room plan tracker, need iOS 16.0+";
		return false;
	}

	if (Devices::VisualTrackerRef visualTracker = roomPlanTracker6DOF_)
	{
		visualTracker->setInput(frameMedium_);
	}

	objectId_ = roomPlanTracker6DOF_->objectId("World");

	trackerSampleEventSubscription_ = roomPlanTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &MRRoomPlanPhoneExperience::onTrackerSample));

	if (roomPlanTracker6DOF_.isNull() || !roomPlanTracker6DOF_->start())
	{
		Log::error() << "MRRoomPlanPhone Experience could not start the world tracker";
		return false;
	}

	startThread();

	return true;
}

bool MRRoomPlanPhoneExperience::stop()
{
	const ScopedLock scopedLock(lock_);

	stopThread();

	if (roomPlanTracker6DOF_ && !roomPlanTracker6DOF_->stop())
	{
		return false;
	}

	roomPlanTracker6DOF_.release();

	return true;
}

std::unique_ptr<XRPlaygroundExperience> MRRoomPlanPhoneExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new MRRoomPlanPhoneExperience());
}

void MRRoomPlanPhoneExperience::onTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
{
	const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneSample(sample);
	ocean_assert(sceneSample);

	const Devices::SceneTracker6DOF::SharedSceneElements& sceneElements = sceneSample->sceneElements();

	if (sceneElements.empty())
	{
		return;
	}

	ocean_assert(sceneElements.size() == 1);

	const Devices::Measurement::Metadata::const_iterator iInstruction = sceneSample->metadata().find("instruction");
	if (iInstruction != sceneSample->metadata().cend())
	{
		std::string instruction = iInstruction->second.stringValue();

		if (!instruction.empty())
		{
			if (instruction == "normal")
			{
				renderingText_->setText(" Continue scanning ");
			}
			else
			{
				renderingText_->setText(" Guidance: \n " + instruction + " ");
			}
		}

		const ScopedLock scopedLock(lock_);

		instruction_ = std::move(instruction);
	}

	ocean_assert(sceneElements.size() == 1);
	const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sceneElements.front();

	if (sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_ROOM)
	{
		const Devices::SceneTracker6DOF::SceneElementRoom& sceneElementRoom = dynamic_cast<const Devices::SceneTracker6DOF::SceneElementRoom&>(*sceneElement);

		const ScopedLock scopedLock(lock_);

		if (!sceneElementRoom.addedRoomObjects().empty() || !sceneElementRoom.removedRoomObjects().empty() || !sceneElementRoom.changedRoomObjects().empty() || !sceneElementRoom.updatedRoomObjects().empty())
		{
			pendingRoomObjectMap_ = sceneElementRoom.roomObjectMap();
		}
	}

	const HomogenousMatrix4 hmdWorld_T_phoneWorld = hmdWorld_T_phoneWorld_.transformation(sceneSample->timestamp());

	if (!hmdWorld_T_phoneWorld.isValid())
	{
		return;
	}

	ocean_assert(sceneSample->referenceSystem() == Devices::Tracker6DOF::RS_DEVICE_IN_OBJECT);
	const HomogenousMatrix4 phoneWorld_T_camera(sceneSample->positions().front(), sceneSample->orientations().front());

	HomogenousMatrix4 hmdWorld_T_camera = hmdWorld_T_phoneWorld * phoneWorld_T_camera;

	// we remove the scale from the transformation
	hmdWorld_T_camera = HomogenousMatrix4(hmdWorld_T_camera.translation(), hmdWorld_T_camera.rotation());

	const ScopedLock scopedLock(lock_);

	hmdWorld_T_slamCamera_ = hmdWorld_T_camera;
}

void MRRoomPlanPhoneExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Should never happen as our engagement radius is very large!");
}

void MRRoomPlanPhoneExperience::threadRun()
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

		const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sample = roomPlanTracker6DOF_->sample(lastFrameTimestamp);

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

void MRRoomPlanPhoneExperience::onChangedUsers(Network::Verts::Driver& driver, const UnorderedIndexSet64& addedUsers, const UnorderedIndexSet64& removedUsers)
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

		renderingText_->setText(" Start the 'MR Room Plan' \n experience on your headset ");
	}
}

void MRRoomPlanPhoneExperience::onReceiveContainer(Network::Verts::Driver& driver, const uint32_t sessionId, const uint64_t userId, const std::string& identifier, const unsigned int version, const Network::Verts::Driver::SharedBuffer& buffer)
{
	if (identifier != "map")
	{
		ocean_assert(false && "Invalid identifier!");
		return;
	}

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
