// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/mrroomplan/MRRoomPlanQuestExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/io/Compression.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/network/tigon/TigonClient.h"

#include "metaonly/ocean/network/verts/Manager.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Box.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Textures.h"
#include "ocean/rendering/Transform.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/tracking/mapbuilding/MultiViewMapCreator.h"
#include "ocean/tracking/mapbuilding/TrackerStereo.h"
#include "ocean/tracking/mapbuilding/Utilities.h"

namespace Ocean
{

using namespace Network;
using namespace Tracking;

namespace XRPlayground
{

void MRRoomPlanQuestExperience::InputData::updateInputData(std::shared_ptr<Frames> yFrames, SharedAnyCameras cameras, const HomogenousMatrix4& world_T_device, HomogenousMatrices4 device_T_cameras, const Timestamp& timestamp)
{
	ocean_assert(yFrames && yFrames->size() >= 1);
	ocean_assert(cameras.size() == yFrames->size());
	ocean_assert(timestamp.isValid());

	const ScopedLock scopedLock(lock_);

	timestamp_ = timestamp;
	yFrames_ = std::move(yFrames);
	cameras_ = std::move(cameras);
	world_T_device_ = world_T_device;
	device_T_cameras_ = std::move(device_T_cameras);
}

bool MRRoomPlanQuestExperience::InputData::latestInputData(Timestamp& lastTimestamp, std::shared_ptr<Frames>& yFrames, SharedAnyCameras& cameras, HomogenousMatrix4& world_T_device, HomogenousMatrices4& device_T_cameras) const
{
	const ScopedLock scopedLock(lock_);

	if (lastTimestamp >= timestamp_) // last timestamp can also be invalid
	{
		return false;
	}

	if (yFrames_ == nullptr)
	{
		return false;
	}

	ocean_assert(timestamp_.isValid());
	lastTimestamp = timestamp_;

	yFrames = yFrames_;
	cameras = cameras_;
	world_T_device = world_T_device_;
	device_T_cameras = device_T_cameras_;

	return true;
}

void MRRoomPlanQuestExperience::MapData::updateMapData(Vectors3&& objectPoints, Scalars&& objectPointStabilityFactors, std::vector<CV::Detector::FREAKDescriptors32>&& multiDescriptors)
{
	ocean_assert(objectPoints.size() == objectPointStabilityFactors.size());
	ocean_assert(objectPoints.size() == multiDescriptors.size());

	const ScopedLock scopedLock(lock_);

	objectPoints_ = std::move(objectPoints);
	objectPointStabilityFactors_ = std::move(objectPointStabilityFactors);
	multiDescriptors_ = std::move(multiDescriptors);
}

bool MRRoomPlanQuestExperience::MapData::latestMapData(Vectors3& objectPoints, Scalars& objectPointStabilityFactors, std::vector<CV::Detector::FREAKDescriptors32>& multiDescriptors) const
{
	const ScopedLock scopedLock(lock_);

	if (objectPoints_.empty())
	{
		return false;
	}

	objectPoints = std::move(objectPoints_);
	objectPointStabilityFactors = std::move(objectPointStabilityFactors_);
	multiDescriptors = std::move(multiDescriptors_);

	return true;
}

void MRRoomPlanQuestExperience::MapData::updateFeatureMap(MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap)
{
	const ScopedLock scopedLock(lock_);

	unifiedFeatureMap_ = std::move(unifiedFeatureMap);
}

bool MRRoomPlanQuestExperience::MapData::latestFeatureMap(MapBuilding::SharedUnifiedFeatureMap& unifiedFeatureMap) const
{
	const ScopedLock scopedLock(lock_);

	if (!unifiedFeatureMap_)
	{
		return false;
	}

	unifiedFeatureMap = std::move(unifiedFeatureMap_);

	return true;
}

MRRoomPlanQuestExperience::MapCreatorThread::MapCreatorThread(const InputData& inputData, MapData& mapData) :
	inputData_(inputData),
	mapData_(mapData)
{
	startThread();
}

void MRRoomPlanQuestExperience::MapCreatorThread::threadRun()
{
	MapBuilding::MultiViewMapCreator multiViewMapCreator;

	Timestamp lastTimestamp(false);

	std::shared_ptr<Frames> yFrames;
	SharedAnyCameras cameras;
	HomogenousMatrix4 world_T_device;
	HomogenousMatrices4 device_T_cameras;

	double mapExtractionInterval = 1.0;
	Timestamp nextMapExtractionTimestamp(false);

	HighPerformanceStatistic performance;

	while (!shouldThreadStop())
	{
		if (!inputData_.latestInputData(lastTimestamp, yFrames, cameras, world_T_device, device_T_cameras))
		{
			sleep(1u);
			continue;
		}

		performance.start();

#if 1
		IndexPair32 stereoCameraIndices;
		if (MapBuilding::MultiViewMapCreator::determineLowerStereoCameras(device_T_cameras, stereoCameraIndices))
		{
			const Frames yFramesSubset =
			{
				Frame((*yFrames)[stereoCameraIndices.first], Frame::ACM_USE_KEEP_LAYOUT),
				Frame((*yFrames)[stereoCameraIndices.second], Frame::ACM_USE_KEEP_LAYOUT)
			};

			const SharedAnyCameras camerasSubset =
			{
				cameras[stereoCameraIndices.first],
				cameras[stereoCameraIndices.second],
			};

			const HomogenousMatrices4 device_T_camerasSubset =
			{
				device_T_cameras[stereoCameraIndices.first],
				device_T_cameras[stereoCameraIndices.second]
			};

			multiViewMapCreator.processFrame(yFramesSubset, camerasSubset, world_T_device, device_T_camerasSubset);
		}
#else
		multiViewMapCreator.processFrame(*yFrames, cameras, world_T_device, device_T_cameras);
#endif

		performance.stop();

		if (performance.measurements() % 100u == 0u)
		{
			Log::info() << "Map creator: " << performance.averageMseconds() << "ms, " << performance.lastMseconds() << "ms";
		}

		if (lastTimestamp >= nextMapExtractionTimestamp)
		{
			Vectors3 objectPoints;
			Scalars objectPointStabilityFactors;
			std::vector<CV::Detector::FREAKDescriptors32> multiDescriptors;

			if (multiViewMapCreator.latestFeatureMap(objectPoints, &multiDescriptors, &objectPointStabilityFactors, 20, 20))
			{
				mapData_.updateMapData(std::move(objectPoints), std::move(objectPointStabilityFactors), std::move(multiDescriptors));
			}

			nextMapExtractionTimestamp = lastTimestamp + mapExtractionInterval;

			mapExtractionInterval = std::min(mapExtractionInterval * 1.05, 2.5);
		}
	}
}

MRRoomPlanQuestExperience::MapHandlingThread::MapHandlingThread(MRRoomPlanQuestExperience& owner, MapData& mapData) :
	owner_(owner),
	mapData_(mapData)
{
	startThread();
}

void MRRoomPlanQuestExperience::MapHandlingThread::threadRun()
{
	RandomGenerator randomGenerator;

	while (!shouldThreadStop())
	{
		Vectors3 objectPoints;
		Scalars objectPointStabilityFactors;
		std::vector<CV::Detector::FREAKDescriptors32> multiDescriptors;

		if (!mapData_.latestMapData(objectPoints, objectPointStabilityFactors, multiDescriptors))
		{
			sleep(1u);
			continue;
		}

		const Indices32 objectPointIds = createIndices<Index32>(objectPoints.size(), 0u);

		MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap descriptorMap;
		descriptorMap.reserve(objectPoints.size() * 3 / 2);

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			descriptorMap.emplace(objectPointIds[n], std::move(multiDescriptors[n]));
		}

		const std::shared_ptr<MapBuilding::UnifiedDescriptorMap> unifiedDescriptorMap = std::make_shared<MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256>(std::move(descriptorMap));

		using ImagePointDescriptor = MapBuilding::UnifiedDescriptor::FreakMultiDescriptor256;
		using ObjectPointDescriptor = MapBuilding::UnifiedDescriptor::FreakMultiDescriptors256;
		using ObjectPointVocabularyDescriptor = MapBuilding::UnifiedDescriptor::BinaryDescriptor<256u>;

		using UnifiedFeatureMap = MapBuilding::UnifiedFeatureMapT<ImagePointDescriptor, ObjectPointDescriptor, ObjectPointVocabularyDescriptor>;

		mapData_.updateFeatureMap(std::make_shared<UnifiedFeatureMap>(Vectors3(objectPoints), Indices32(objectPointIds), std::shared_ptr<MapBuilding::UnifiedDescriptorMap>(unifiedDescriptorMap), randomGenerator, &UnifiedFeatureMap::VocabularyForest::TVocabularyTree::determineClustersMeanForBinaryDescriptor<256u>, &Tracking::MapBuilding::UnifiedHelperFreakMultiDescriptor256::extractVocabularyDescriptorsFromMap));

		if (owner_.connectionEstablished())
		{
			std::ostringstream stringStream(std::ios::binary);
			IO::OutputBitstream bitstream(stringStream);

			if (bitstream.write<unsigned long long>(mapTag_)
					&& writeObjectPointsToStream(objectPoints, objectPointIds, bitstream)
					&& MapBuilding::Utilities::writeDescriptorMap(*unifiedDescriptorMap, bitstream))
			{
				const std::string mapData = stringStream.str();
				ocean_assert(!mapData.empty());

				IO::Compression::Buffer buffer;
				if (IO::Compression::gzipCompress(mapData.c_str(), mapData.size(), buffer))
				{
					owner_.sendMap(std::move(buffer));
				}
			}
		}
	}
}

MRRoomPlanQuestExperience::MRRoomPlanQuestExperience()
{
	// nothing to do here
}

MRRoomPlanQuestExperience::~MRRoomPlanQuestExperience()
{
	// nothing to do here
}

bool MRRoomPlanQuestExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	PlatformSpecific::get().mrPassthroughVisualizer().resumePassthrough();
#endif

	Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingText_);
	textTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2)));

	experienceScene()->addChild(textTransform);

	renderingTransformPhoneSlam_ = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.15), Scalar(0.075), Scalar(0.0075)), RGBAColor(0.7f, 0.7f, 0.7f));
	renderingTransformPhoneSlam_->setVisible(false);

	textTransform = Rendering::Utilities::createText(*engine, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.0075), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingTextPhoneInstruction_);
	textTransform->setTransformation(HomogenousMatrix4(Vector3(Scalar(-0.055), 0, Scalar(0.01))) * HomogenousMatrix4(Quaternion(Vector3(0, 0, 1), Numeric::pi_2())));
	renderingTransformPhoneSlam_->addChild(textTransform);

	experienceScene()->addChild(renderingTransformPhoneSlam_);

	renderingText_->setText(" Start 'MR Room Plan' experience \n on your iPhone with iOS 16+ ");

	renderingGroupPlanarRoomObjects_ = engine->factory().createGroup();
	experienceScene()->addChild(renderingGroupPlanarRoomObjects_);

	renderingGroupVolumetricRoomObjects_ = engine->factory().createGroup();
	experienceScene()->addChild(renderingGroupVolumetricRoomObjects_);

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
		newEntityScopedSubscription_ = vertsDriver_->addNewEntityCallback(std::bind(&MRRoomPlanQuestExperience::onNewEntity, this, std::placeholders::_1, std::placeholders::_2), "DevicePose");
		changedUsersScopedSubscription_ = vertsDriver_->addChangedUsersCallback(std::bind(&MRRoomPlanQuestExperience::onChangedUsers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		receiveContainerScopedSubscription_ = vertsDriver_->addReceiveContainerCallback(std::bind(&MRRoomPlanQuestExperience::onReceiveContainer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

		Log::info() << "Created VERTS driver with zone name '" << vertsZoneName << "'";
	}

	Verts::NodeSpecification& devicePoseSpecification = Verts::NodeSpecification::newNodeSpecification("DevicePose");
	devicePoseSpecification.registerField<std::string>("hmdWorld_T_slamCamera");
	devicePoseSpecification.registerField<std::string>("instruction");

	startThread();

	return true;
}

bool MRRoomPlanQuestExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	receiveContainerScopedSubscription_.release();
	changedUsersScopedSubscription_.release();
	newEntityScopedSubscription_.release();

	stopThreadExplicitly();

	renderingTransformPhoneSlam_.release();
	renderingText_.release();

	renderingGroupVolumetricRoomObjects_.release();
	renderingGroupPlanarRoomObjects_.release();

	vertsDriver_ = nullptr;

	return true;
}

Timestamp MRRoomPlanQuestExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);
		Buffer mapBuffer;
		std::swap(mapBuffer, mapBuffer_);

		Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap pendingRoomObjectMap;
		std::swap(pendingRoomObjectMap, pendingRoomObjectMap_);

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
		const Vector2 leftJoystickTilt = PlatformSpecific::get().trackedRemoteDevice().joystickTilt(Platform::Meta::Quest::VrApi::TrackedRemoteDevice::RT_LEFT);
		const Vector2 rightJoystickTilt = PlatformSpecific::get().trackedRemoteDevice().joystickTilt(Platform::Meta::Quest::VrApi::TrackedRemoteDevice::RT_RIGHT);
		const Vector2& joystickTilt = leftJoystickTilt.sqr() > rightJoystickTilt.sqr() ? leftJoystickTilt : rightJoystickTilt;

		if (Numeric::abs(joystickTilt.x()) >= Scalar(0.1))
		{
			volumetricObjectsAlpha_ = minmax<float>(0.1f, volumetricObjectsAlpha_ + float(joystickTilt.x()) * 0.01f, 1.0f);

			if (pendingRoomObjectMap.empty())
			{
				std::swap(pendingRoomObjectMap, roomObjectMap_);
			}
		}
#endif

		const float volumetricObjectsAlpha(volumetricObjectsAlpha_);
	scopedLock.release();

	const Timestamp currentTimestamp(true);
	const Timestamp latestTimestampPhoneSlam(latestTimestampPhoneSlam_);

	renderingTransformPhoneSlam_->setVisible(currentTimestamp <= latestTimestampPhoneSlam + 0.2);

	if (vertsDevicePoseNode_ && vertsDevicePoseNode_->hasChanged())
	{
		ocean_assert(vertsDevicePoseNode_->nodeSpecification().fieldHasIndex<Verts::Node::StringPointer>("hmdWorld_T_slamCamera", 0u));
		const Verts::Node::StringPointer data = vertsDevicePoseNode_->field<Verts::Node::StringPointer>(0u);

		if (data)
		{
			ocean_assert(data.size() == sizeof(HomogenousMatrixD4));
			if (data.size() == sizeof(HomogenousMatrixD4))
			{
				double values[16];
				memcpy(values, data.data(), sizeof(double) * 16);

				const HomogenousMatrixD4 hmdWorld_T_slamCamera(values);

				if (hmdWorld_T_slamCamera.isValid())
				{
					renderingText_->setText("");

					renderingTransformPhoneSlam_->setTransformation(HomogenousMatrix4(hmdWorld_T_slamCamera) * HomogenousMatrix4(Vector3(Scalar(0.065), Scalar(-0.02), 0)));

					latestTimestampPhoneSlam_ = Timestamp(true);
				}
			}
		}

		const std::string instruction = vertsDevicePoseNode_->field<std::string>("instruction");

		if (instruction == "normal")
		{
			renderingTextPhoneInstruction_->setText("\n  Continue scanning  \n");
		}
		else
		{
			renderingTextPhoneInstruction_->setText("\n  Guidance:  \n  " + instruction + "  \n");
		}
	}

	if (!pendingRoomObjectMap.empty())
	{
		renderingGroupPlanarRoomObjects_->clear();
		renderingGroupVolumetricRoomObjects_->clear();

		for (Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap::const_iterator iRoomObject = pendingRoomObjectMap.cbegin(); iRoomObject != pendingRoomObjectMap.cend(); ++iRoomObject)
		{
			const Devices::SceneTracker6DOF::SceneElementRoom::SharedRoomObject& roomObject = iRoomObject->second;

			if (roomObject->objectType() == Devices::SceneTracker6DOF::SceneElementRoom::RoomObject::OT_PLANAR)
			{
				const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject& planarRoomObject = (const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject&)(*roomObject);

				Vector3 dimension = roomObject->dimension();

				const Scalar thickness = planarThickness(planarRoomObject.planarType());
				dimension = Vector3(std::max(dimension.x(), thickness), std::max(dimension.y(), thickness), std::max(dimension.z(), thickness));

				Rendering::TransformRef transform = Rendering::Utilities::createBox(engine, dimension, planarColor(planarRoomObject.planarType()));

				transform->setTransformation(roomObject->world_T_object());

				renderingGroupPlanarRoomObjects_->addChild(transform);
			}
			else
			{
				ocean_assert(roomObject->objectType() == Devices::SceneTracker6DOF::SceneElementRoom::RoomObject::OT_VOLUMETRIC);
				const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject& volumetricRoomObject = (const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject&)(*roomObject);

				const Vector3 dimension = adjustedVolumetricDimension(volumetricRoomObject.volumetricType(), roomObject->dimension());

				Rendering::TransformRef transform = Rendering::Utilities::createBox(engine, dimension, volumetricColor(volumetricRoomObject.volumetricType(), volumetricObjectsAlpha));

				const std::string volumeticType = Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::translateVolumetricType(volumetricRoomObject.volumetricType());
				const float confidence = volumetricRoomObject.confidence();

				const Rendering::TransformRef text = Rendering::Utilities::createText(*engine, " " + volumeticType + ", " + String::toAString(confidence, 1u) + " ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, 0.075, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
				text->setTransformation(HomogenousMatrix4(Vector3(0, dimension.y() * Scalar(0.5) + Scalar(0.05), 0)));

				transform->addChild(text);

				const Rendering::TransformRef backsideText = engine->factory().createTransform();
				backsideText->addChild(text);
				backsideText->setTransformation(HomogenousMatrix4(Quaternion(Vector3(0, 1, 0), Numeric::pi())));

				transform->addChild(backsideText);

				transform->setTransformation(roomObject->world_T_object());

				renderingGroupVolumetricRoomObjects_->addChild(transform);
			}
		}

		scopedLock.relock(lock_);
			roomObjectMap_ = std::move(pendingRoomObjectMap);
		scopedLock.release();
	}

	if (!mapBuffer.empty() && vertsDriver_ && vertsDriver_->isInitialized())
	{
		static unsigned int mapVersion = 0u;
		vertsDriver_->sendContainer("map", ++mapVersion, mapBuffer.data(), mapBuffer.size());
	}

	return timestamp;
}

void MRRoomPlanQuestExperience::onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	if (key == "A" || key == "X")
	{
		renderingGroupPlanarRoomObjects_->setVisible(!renderingGroupPlanarRoomObjects_->visible());
	}
}

std::unique_ptr<XRPlaygroundExperience> MRRoomPlanQuestExperience::createExperience()
{
    return std::unique_ptr<XRPlaygroundExperience>(new MRRoomPlanQuestExperience());
}

void MRRoomPlanQuestExperience::threadRun()
{
	std::vector<Media::FrameMediumRef> frameMediums;

	for (size_t cameraId = 0; cameraId < 4; ++cameraId)
	{
		Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium("LiveVideoId:" + String::toAString(cameraId));

		if (frameMedium.isNull())
		{
			Log::error() << "Failed to access headset camera " << cameraId;

			renderingText_->setText(" Failed to access the cameras \n see https://fburl.com/access_cameras ");

			return;
		}

		frameMedium->start();
		frameMediums.emplace_back(std::move(frameMedium));
	}

	Timestamp lastFrameTimestamp(false);

	InputData inputData;
	MapData mapData;

	MapCreatorThread mapCreatorThread(inputData, mapData);
	MapHandlingThread mapHandlingThread(*this, mapData);

	FrameRefs frames;
	SharedAnyCameras cameras;

	while (!shouldThreadStop())
	{
		bool timedOut = false;
		if (!Media::FrameMedium::syncedFrames(frameMediums, lastFrameTimestamp, frames, cameras, 2u /*waitTime*/, &timedOut))
		{
			if (timedOut)
			{
				Log::warning() << "Failed to access synced camera frames for timestamp";
			}

			continue;
		}

		ocean_assert(frameMediums.size() == frames.size() && frameMediums.size() == cameras.size());

		lastFrameTimestamp = frames.front()->timestamp();

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
		const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(lastFrameTimestamp);
		if (!world_T_device.isValid())
		{
			Log::info() << "Failed to determine headset pose for timestamp " << double(lastFrameTimestamp);
			continue;
		}
#else
		ocean_assert(false && "This should never happen!");
		const HomogenousMatrix4 world_T_device(false);
		break;
#endif

		HomogenousMatrices4 device_T_cameras(frameMediums.size());
		Frames yFrames(frameMediums.size());

		for (size_t cameraId = 0; cameraId < frameMediums.size(); ++cameraId)
		{
			if (!CV::FrameConverter::Comfort::convert(*frames[cameraId], FrameType(*frames[cameraId], FrameType::FORMAT_Y8), yFrames[cameraId], CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, nullptr, CV::FrameConverter::Options(0.6f, true)))
			{
				ocean_assert(false && "This should never happen!");
				return;
			}

			device_T_cameras[cameraId] = HomogenousMatrix4(frameMediums[cameraId]->device_T_camera());
		}

		inputData.updateInputData(std::make_shared<Frames>(std::move(yFrames)), cameras, world_T_device, device_T_cameras, lastFrameTimestamp);
	}
}

bool MRRoomPlanQuestExperience::connectionEstablished() const
{
	const ScopedLock scopedLock(lock_);

	return !userIds_.empty();
}

void MRRoomPlanQuestExperience::sendMap(Buffer&& buffer)
{
	const ScopedLock scopedLock(lock_);

	mapBuffer_ = std::move(buffer);
}

void MRRoomPlanQuestExperience::onNewEntity(Network::Verts::Driver& driver, const Network::Verts::SharedEntity& entity)
{
	vertsDevicePoseNode_ = entity->node("DevicePose");
	ocean_assert(vertsDevicePoseNode_);
}

void MRRoomPlanQuestExperience::onChangedUsers(Network::Verts::Driver& driver, const UnorderedIndexSet64& addedUsers, const UnorderedIndexSet64& removedUsers)
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
		renderingText_->setText(" Connection started \n Move iPhone slowly ");
	}
	else if (usersBefore != 0 && userIds_.empty())
	{
		renderingText_->setText(" Connection stopped ");
	}
}

void MRRoomPlanQuestExperience::onReceiveContainer(Network::Verts::Driver& driver, const uint32_t sessionId, const uint64_t userId, const std::string& identifier, const unsigned int version, const Network::Verts::Driver::SharedBuffer& buffer)
{
	if (identifier != "roomobjects")
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

		Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap roomObjectMap;
		if (readRoomObjectsToStream(bitstream, roomObjectMap))
		{
			const ScopedLock scopedLock(lock_);

			pendingRoomObjectMap_ = std::move(roomObjectMap);
		}
		else
		{
			Log::error() << "Failed to read room objects in container with version " << version;
		}
	}
}

}

}
