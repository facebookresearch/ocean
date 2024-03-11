// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/mapalignment/MapAlignmentQuestExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/io/Compression.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

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

#include "metaonly/ocean/network/tigon/TigonClient.h"

#include "metaonly/ocean/network/verts/Manager.h"

namespace Ocean
{

using namespace Network;
using namespace Tracking;

namespace XRPlayground
{

void MapAlignmentQuestExperience::InputData::updateInputData(std::shared_ptr<Frames> yFrames, SharedAnyCameras cameras, const HomogenousMatrix4& world_T_device, HomogenousMatrices4 device_T_cameras, const Timestamp& timestamp)
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

bool MapAlignmentQuestExperience::InputData::latestInputData(Timestamp& lastTimestamp, std::shared_ptr<Frames>& yFrames, SharedAnyCameras& cameras, HomogenousMatrix4& world_T_device, HomogenousMatrices4& device_T_cameras) const
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

void MapAlignmentQuestExperience::MapData::updateMapData(Vectors3&& objectPoints, Scalars&& objectPointStabilityFactors, std::vector<CV::Detector::FREAKDescriptors32>&& multiDescriptors)
{
	ocean_assert(objectPoints.size() == objectPointStabilityFactors.size());
	ocean_assert(objectPoints.size() == multiDescriptors.size());

	const ScopedLock scopedLock(lock_);

	objectPoints_ = std::move(objectPoints);
	objectPointStabilityFactors_ = std::move(objectPointStabilityFactors);
	multiDescriptors_ = std::move(multiDescriptors);
}

bool MapAlignmentQuestExperience::MapData::latestMapData(Vectors3& objectPoints, Scalars& objectPointStabilityFactors, std::vector<CV::Detector::FREAKDescriptors32>& multiDescriptors) const
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

void MapAlignmentQuestExperience::MapData::updateFeatureMap(MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap)
{
	const ScopedLock scopedLock(lock_);

	unifiedFeatureMap_ = std::move(unifiedFeatureMap);
}

bool MapAlignmentQuestExperience::MapData::latestFeatureMap(MapBuilding::SharedUnifiedFeatureMap& unifiedFeatureMap) const
{
	const ScopedLock scopedLock(lock_);

	if (!unifiedFeatureMap_)
	{
		return false;
	}

	unifiedFeatureMap = std::move(unifiedFeatureMap_);

	return true;
}

MapAlignmentQuestExperience::MapCreatorThread::MapCreatorThread(const InputData& inputData, MapData& mapData) :
	inputData_(inputData),
	mapData_(mapData)
{
	startThread();
}

void MapAlignmentQuestExperience::MapCreatorThread::threadRun()
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

MapAlignmentQuestExperience::RelocalizerThread::RelocalizerThread(MapAlignmentQuestExperience& owner, const InputData& inputData, const MapData& mapData) :
	owner_(owner),
	inputData_(inputData),
	mapData_(mapData)
{
	startThread();
}

void MapAlignmentQuestExperience::RelocalizerThread::threadRun()
{
	MapBuilding::TrackerStereo trackerStereo;

	Timestamp lastTimestamp(false);

	HomogenousMatrix4 slamDevice_T_relocalizedDevice(false);

	std::shared_ptr<Frames> yFrames;
	SharedAnyCameras cameras;
	HomogenousMatrix4 world_T_device;
	HomogenousMatrices4 device_T_cameras;

	HighPerformanceStatistic performance;

	while (!shouldThreadStop())
	{
		if (!inputData_.latestInputData(lastTimestamp, yFrames, cameras, world_T_device, device_T_cameras))
		{
			sleep(1u);
			continue;
		}

		MapBuilding::SharedUnifiedFeatureMap unifiedFeatureMap;
		if (mapData_.latestFeatureMap(unifiedFeatureMap))
		{
			trackerStereo.setFeatureMap(std::move(unifiedFeatureMap));
		}

		IndexPair32 stereoCameraIndices;
		if (!MapBuilding::MultiViewMapCreator::determineLowerStereoCameras(device_T_cameras, stereoCameraIndices))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		const AnyCamera& cameraA = *cameras[stereoCameraIndices.first];
		const AnyCamera& cameraB = *cameras[stereoCameraIndices.second];

		const HomogenousMatrix4& device_T_cameraA = device_T_cameras[stereoCameraIndices.first];
		const HomogenousMatrix4& device_T_cameraB = device_T_cameras[stereoCameraIndices.second];

		const Frame& yFrameA = (*yFrames)[stereoCameraIndices.first];
		const Frame& yFrameB = (*yFrames)[stereoCameraIndices.second];

		HomogenousMatrix4 world_T_slamDevice(false);

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
		world_T_slamDevice = PlatformSpecific::get().world_T_device(lastTimestamp);
#endif

		HomogenousMatrix4 world_T_roughDevice(false);

		if (world_T_slamDevice.isValid() && slamDevice_T_relocalizedDevice.isValid())
		{
			world_T_roughDevice = world_T_slamDevice * slamDevice_T_relocalizedDevice;
		}

		HomogenousMatrix4 world_T_relocalizedDevice(false);
		if (trackerStereo.isValid())
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performance);

			constexpr unsigned int minimalNumberCorrespondences = 160u;
			constexpr Scalar maximalProjectionError = Scalar(1.5);

			if (trackerStereo.track(cameraA, cameraB, device_T_cameraA, device_T_cameraB, yFrameA, yFrameB, world_T_relocalizedDevice, minimalNumberCorrespondences, maximalProjectionError, world_T_roughDevice, WorkerPool::get().scopedWorker()()))
			{
				if (world_T_slamDevice.isValid())
				{
					slamDevice_T_relocalizedDevice = world_T_slamDevice.inverted() * world_T_relocalizedDevice;
				}

				owner_.renderingDevicePoseCoordinateSystem_->setTransformation(world_T_relocalizedDevice * HomogenousMatrix4(Vector3(0, 0, Scalar(-0.4)))); // 40cm in front of the user
				owner_.latestTimestampHeadsetRelocalization_ = Timestamp(true);
			}
			else
			{
				slamDevice_T_relocalizedDevice.toNull();
			}
		}

		if (performance.measurements() > 0u && performance.measurements() % 100u == 0u)
		{
			Log::info() << "Stereo tracker: " << performance.averageMseconds() << "ms, " << performance.lastMseconds() << "ms";
		}
	}
}

MapAlignmentQuestExperience::MapHandlingThread::MapHandlingThread(MapAlignmentQuestExperience& owner, MapData& mapData) :
	owner_(owner),
	mapData_(mapData)
{
	startThread();
}

void MapAlignmentQuestExperience::MapHandlingThread::threadRun()
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

		const MapBuilding::SharedUnifiedDescriptorMap unifiedDescriptorMap = std::make_shared<MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256>(std::move(descriptorMap));

		using ImagePointDescriptor = MapBuilding::UnifiedDescriptor::FreakMultiDescriptor256;
		using ObjectPointDescriptor = MapBuilding::UnifiedDescriptor::FreakMultiDescriptors256;
		using ObjectPointVocabularyDescriptor = MapBuilding::UnifiedDescriptor::BinaryDescriptor<256u>;

		using UnifiedFeatureMap = MapBuilding::UnifiedFeatureMapT<ImagePointDescriptor, ObjectPointDescriptor, ObjectPointVocabularyDescriptor>;

		mapData_.updateFeatureMap(std::make_shared<UnifiedFeatureMap>(Vectors3(objectPoints), Indices32(objectPointIds), MapBuilding::SharedUnifiedDescriptorMap(unifiedDescriptorMap), randomGenerator, &UnifiedFeatureMap::VocabularyForest::TVocabularyTree::determineClustersMeanForBinaryDescriptor<256u>, &Tracking::MapBuilding::UnifiedHelperFreakMultiDescriptor256::extractVocabularyDescriptorsFromMap));

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

		const ScopedLock scopedLock(owner_.lock_);

		owner_.latestFeaturePoints_ = std::move(objectPoints);
		owner_.latestFeaturePointStabilityFactors_ = std::move(objectPointStabilityFactors);
	}
}

MapAlignmentQuestExperience::MapAlignmentQuestExperience()
{
	// nothing to do here
}

MapAlignmentQuestExperience::~MapAlignmentQuestExperience()
{
	// nothing to do here
}

bool MapAlignmentQuestExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	ocean_assert(engine);

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	PlatformSpecific::get().mrPassthroughVisualizer().resumePassthrough();
#endif

	renderingGroup_ = engine->factory().createGroup();
	experienceScene()->addChild(renderingGroup_);

	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingText_);
	textTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2)));

	experienceScene()->addChild(textTransform);

	renderingTransformPhoneRelocalization_ = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.15), Scalar(0.075), Scalar(0.0075)), RGBAColor(1.0f, 0.0f, 0.0f));
	renderingTransformPhoneRelocalization_->setVisible(false);
	experienceScene()->addChild(renderingTransformPhoneRelocalization_);

	renderingTransformPhoneSlam_ = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.15), Scalar(0.075), Scalar(0.0075)), RGBAColor(0.7f, 0.7f, 0.7f));
	renderingTransformPhoneSlam_->setVisible(false);
	experienceScene()->addChild(renderingTransformPhoneSlam_);

	renderingDevicePoseCoordinateSystem_ = Rendering::Utilities::createCoordinateSystem(engine, Scalar(0.045), Scalar(0.005), Scalar(0.0025));
	experienceScene()->addChild(renderingDevicePoseCoordinateSystem_);

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

	renderingText_->setText(" Start 'Map Alignment' \n experience on your phone ");

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
		newEntityScopedSubscription_ = vertsDriver_->addNewEntityCallback(std::bind(&MapAlignmentQuestExperience::onNewEntity, this, std::placeholders::_1, std::placeholders::_2), "DevicePose");
		changedUsersScopedSubscription_ = vertsDriver_->addChangedUsersCallback(std::bind(&MapAlignmentQuestExperience::onChangedUsers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		Log::info() << "Created VERTS driver with zone name '" << vertsZoneName << "'";
	}

	Verts::NodeSpecification& devicePoseSpecification = Verts::NodeSpecification::newNodeSpecification("DevicePose");
	devicePoseSpecification.registerField<std::string>("hmdWorld_T_slamCamera");
	devicePoseSpecification.registerField<std::string>("hmdWorld_T_relocalizationCamera");

	startThread();

	return true;
}

bool MapAlignmentQuestExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	newEntityScopedSubscription_.release();
	changedUsersScopedSubscription_.release();

	stopThreadExplicitly();

	renderingFrameTextureDebug_.release();
	renderingTransformDebug_.release();
	renderingGroup_.release();

	vertsDriver_ = nullptr;

	return true;
}

Timestamp MapAlignmentQuestExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);
		Vectors3 latestFeaturePoints(std::move(latestFeaturePoints_));
		Scalars latestFeaturePointStabilityFactors(std::move(latestFeaturePointStabilityFactors_));

		Buffer mapBuffer;
		std::swap(mapBuffer, mapBuffer_);
	scopedLock.release();

	if (!latestFeaturePoints.empty())
	{
		ocean_assert(latestFeaturePoints.size() == latestFeaturePointStabilityFactors.size());

		renderingGroup_->clear();

		RGBAColors latestFeaturePointColors;
		latestFeaturePointColors.reserve(latestFeaturePointStabilityFactors.size());

		for (size_t n = 0; n < latestFeaturePointStabilityFactors.size(); ++n)
		{
			const float factor = float(latestFeaturePointStabilityFactors[n]);

			latestFeaturePointColors.emplace_back(RGBAColor(0.0f, 1.0f, 0.0f).damped(factor).combined(RGBAColor(1.0f, 0.0f, 0.0f).damped(1.0f - factor)));
		}

		renderingGroup_->addChild(Rendering::Utilities::createPoints(*engine, latestFeaturePoints, RGBAColor(false), Scalar(2), latestFeaturePointColors));
	}

	const Timestamp currentTimestamp(true);
	const Timestamp latestTimestampPhoneSlam(latestTimestampPhoneSlam_);
	const Timestamp latestTimestampPhoneRelocalization(latestTimestampPhoneRelocalization_);
	const Timestamp latestTimestampHeadsetRelocalization(latestTimestampHeadsetRelocalization_);

	renderingTransformPhoneSlam_->setVisible(currentTimestamp <= latestTimestampPhoneSlam + 0.2);
	renderingTransformPhoneRelocalization_->setVisible(showPhoneRelocalization_ && currentTimestamp <= latestTimestampPhoneRelocalization + 0.2);

	renderingDevicePoseCoordinateSystem_->setVisible(showHeadsetRelocalization_ && currentTimestamp <= latestTimestampHeadsetRelocalization + 0.2);

	if (MapBuilding::TrackerStereo::RelocalizerDebugElements::get().isElementActive(MapBuilding::TrackerStereo::RelocalizerDebugElements::EI_CAMERA_IMAGES_WITH_FEATURE_CORRESPONDENCES))
	{
		Frame debugFrame = MapBuilding::TrackerStereo::RelocalizerDebugElements::get().element(MapBuilding::TrackerStereo::RelocalizerDebugElements::EI_CAMERA_IMAGES_WITH_FEATURE_CORRESPONDENCES, true);

		if (debugFrame)
		{
			renderingTransformDebug_->setTransformation(HomogenousMatrix4(Vector3(0, 0, Scalar(-0.75)), Vector3(Scalar(1), Scalar(debugFrame.height()) / Scalar(debugFrame.width()), 0)));
			renderingFrameTextureDebug_->setTexture(std::move(debugFrame));
		}
	}

	if (vertsDevicePoseNode_ && vertsDevicePoseNode_->hasChanged())
	{
		Verts::Node::StringPointer data = vertsDevicePoseNode_->field<Verts::Node::StringPointer>("hmdWorld_T_slamCamera");

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
					renderingTransformPhoneSlam_->setTransformation(HomogenousMatrix4(hmdWorld_T_slamCamera) * HomogenousMatrix4(Vector3(Scalar(0.065), Scalar(-0.02), 0)));

					latestTimestampPhoneSlam_ = Timestamp(true);
				}
			}
		}

		data = vertsDevicePoseNode_->field<Verts::Node::StringPointer>("hmdWorld_T_relocalizationCamera");

		if (data)
		{
			ocean_assert(data.size() == sizeof(HomogenousMatrixD4));
			if (data.size() == sizeof(HomogenousMatrixD4))
			{
				double values[16];
				memcpy(values, data.data(), sizeof(double) * 16);

				const HomogenousMatrixD4 hmdWorld_T_relocalizationCamera(values);

				if (hmdWorld_T_relocalizationCamera.isValid())
				{
					renderingTransformPhoneRelocalization_->setTransformation(HomogenousMatrix4(hmdWorld_T_relocalizationCamera) * HomogenousMatrix4(Vector3(Scalar(0.065), Scalar(-0.02), 0)));

					latestTimestampPhoneRelocalization_ = Timestamp(true);
				}
			}
		}
	}

	if (!mapBuffer.empty() && vertsDriver_ && vertsDriver_->isInitialized())
	{
		static unsigned int mapVersion = 0u;
		vertsDriver_->sendContainer("map", ++mapVersion, mapBuffer.data(), mapBuffer.size());
	}

	return timestamp;
}

void MapAlignmentQuestExperience::onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
	if (key == "A")
	{
		showHeadsetRelocalization_ = !showHeadsetRelocalization_;
	}
	else if (key == "X")
	{
		showPhoneRelocalization_ = !showPhoneRelocalization_;
	}
	else if (key == "Y")
	{
		constexpr MapBuilding::TrackerStereo::RelocalizerDebugElements::ElementId elementId = MapBuilding::TrackerStereo::RelocalizerDebugElements::ElementId::EI_CAMERA_IMAGES_WITH_FEATURE_CORRESPONDENCES;

		if (MapBuilding::TrackerStereo::RelocalizerDebugElements::get().isElementActive(elementId))
		{
			MapBuilding::TrackerStereo::RelocalizerDebugElements::get().deactivateElement(elementId);

			renderingTransformDebug_->setVisible(false);
		}
		else
		{
			MapBuilding::TrackerStereo::RelocalizerDebugElements::get().activateElement(elementId);

			renderingTransformDebug_->setVisible(true);
		}
	}
}

std::unique_ptr<XRPlaygroundExperience> MapAlignmentQuestExperience::createExperience()
{
    return std::unique_ptr<XRPlaygroundExperience>(new MapAlignmentQuestExperience());
}

void MapAlignmentQuestExperience::threadRun()
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
	RelocalizerThread relocalizerThread(*this, inputData, mapData);
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

bool MapAlignmentQuestExperience::connectionEstablished() const
{
	const ScopedLock scopedLock(lock_);

	return !userIds_.empty();
}

void MapAlignmentQuestExperience::sendMap(Buffer&& buffer)
{
	const ScopedLock scopedLock(lock_);

	mapBuffer_ = std::move(buffer);
}

void MapAlignmentQuestExperience::onNewEntity(Network::Verts::Driver& driver, const Network::Verts::SharedEntity& entity)
{
	vertsDevicePoseNode_ = entity->node("DevicePose");
	ocean_assert(vertsDevicePoseNode_);
}

void MapAlignmentQuestExperience::onChangedUsers(Network::Verts::Driver& driver, const UnorderedIndexSet64& addedUsers, const UnorderedIndexSet64& removedUsers)
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
		renderingText_->setText(" Connection started \n Move phone slowly ");
	}
	else if (usersBefore != 0 && userIds_.empty())
	{
		renderingText_->setText(" Connection stopped ");
	}
}

}

}
