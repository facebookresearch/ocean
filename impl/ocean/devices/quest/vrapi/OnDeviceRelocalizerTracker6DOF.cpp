// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/quest/vrapi/OnDeviceRelocalizerTracker6DOF.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/geometry/AbsoluteTransformation.h"

#include "ocean/io/Bitstream.h"
#include "ocean/io/File.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Rotation.h"

#include "ocean/platform/meta/quest/vrapi/HeadsetPoses.h"

#include "ocean/tracking/Utilities.h"

#include "ocean/tracking/mapbuilding/Utilities.h"

#include "metaonly/ocean/tracking/mapbuilding/importers/RelocChunkImporter.h"

namespace Ocean
{

using namespace Platform::Meta::Quest::VrApi;

using namespace Tracking::MapBuilding;

namespace Devices
{

namespace Quest
{

namespace VrApi
{

OnDeviceRelocalizerTracker6DOF::OnDeviceRelocalizerTracker6DOF() :
	Device(deviceNameOnDeviceRelocalizerTracker6DOF(), deviceTypeOnDeviceRelocalizerTracker6DOF()),
	VrApiDevice(deviceNameOnDeviceRelocalizerTracker6DOF(), deviceTypeOnDeviceRelocalizerTracker6DOF()),
	Measurement(deviceNameOnDeviceRelocalizerTracker6DOF(), deviceTypeOnDeviceRelocalizerTracker6DOF()),
	Tracker(deviceNameOnDeviceRelocalizerTracker6DOF(), deviceTypeOnDeviceRelocalizerTracker6DOF()),
	OrientationTracker3DOF(deviceNameOnDeviceRelocalizerTracker6DOF()),
	PositionTracker3DOF(deviceNameOnDeviceRelocalizerTracker6DOF()),
	Tracker6DOF(deviceNameOnDeviceRelocalizerTracker6DOF()),
	SceneTracker6DOF(deviceNameOnDeviceRelocalizerTracker6DOF()),
	ObjectTracker(deviceNameOnDeviceRelocalizerTracker6DOF(), deviceTypeOnDeviceRelocalizerTracker6DOF()),
	VisualTracker(deviceNameOnDeviceRelocalizerTracker6DOF(), deviceTypeOnDeviceRelocalizerTracker6DOF())
{
	// nothing do to here
}

OnDeviceRelocalizerTracker6DOF::~OnDeviceRelocalizerTracker6DOF()
{
	stopThreadExplicitly();
}

OnDeviceRelocalizerTracker6DOF::ObjectId OnDeviceRelocalizerTracker6DOF::registerObject(const std::string& description, const Vector3& /*dimension*/)
{
	if (mapObjectId_ != invalidObjectId())
	{
		return invalidObjectId();
	}

	const IO::File mapFile(description);

	if (mapFile.exists())
	{
		Vectors3 objectPoints;
		Indices32 objectPointIds;

		std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> unifiedDescriptorMap;

		if (mapFile.extension() == "ocean_map")
		{
			std::ifstream stream(mapFile(), std::ios::binary);
			IO::InputBitstream inputBitstream(stream);

			Tracking::Database database;

			if (!Tracking::Utilities::readDatabase(inputBitstream, database)
				|| !Tracking::MapBuilding::Utilities::readDescriptorMap(inputBitstream, unifiedDescriptorMap))
			{
				Log::error() << "Failed to read feature amp file";
				return invalidObjectId();
			}

			objectPointIds = database.objectPointIds<false, false>(Tracking::Database::invalidObjectPoint(), &objectPoints);
		}
		else if (mapFile.extension() == "metaportation_map")
		{
			std::ifstream stream(mapFile(), std::ios::binary);
			IO::InputBitstream bitstream(stream);

			/// The unique tag for a map.
			static constexpr uint64_t mapTag = IO::Tag::string2tag("_OCNMAP_");

			unsigned long long tag = 0u;
			if (bitstream.read<unsigned long long>(tag) && tag == mapTag)
			{
				if (!readObjectPointsFromStream(bitstream, objectPoints, objectPointIds)
			 		|| !Tracking::MapBuilding::Utilities::readDescriptorMap(bitstream, unifiedDescriptorMap))
				{
					Log::error() << "Failed to read metaportation map";
					return invalidObjectId();
				}
			}
		}
		else if (mapFile.extension() == "chunk_map")
		{
			if (!Tracking::MapBuilding::Importers::RelocChunkImporter::importFromFile(mapFile(), objectPoints, objectPointIds, unifiedDescriptorMap))
			{
				Log::error() << "Failed to read chunk map";
				return invalidObjectId();
			}
		}

		if (!objectPoints.empty())
		{
			ocean_assert(objectPoints.size() == objectPointIds.size());
			ocean_assert(unifiedDescriptorMap);

			using ImagePointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptor256;
			using ObjectPointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptors256;
			using ObjectPointVocabularyDescriptor = Tracking::MapBuilding::UnifiedDescriptor::BinaryDescriptor<256u>;

			using UnifiedFeatureMap = Tracking::MapBuilding::UnifiedFeatureMapT<ImagePointDescriptor, ObjectPointDescriptor, ObjectPointVocabularyDescriptor>;

			RandomGenerator randomGenerator;
			Tracking::MapBuilding::SharedUnifiedFeatureMap featureMap = std::make_shared<UnifiedFeatureMap>(std::move(objectPoints), std::move(objectPointIds), std::move(unifiedDescriptorMap), randomGenerator,  &UnifiedFeatureMap::VocabularyForest::TVocabularyTree::determineClustersMeanForBinaryDescriptor<256u>, &Tracking::MapBuilding::UnifiedHelperFreakMultiDescriptor256::extractVocabularyDescriptorsFromMap);

			if (!trackerStereo_.setImageFeaturePointDetectorFunction(Tracking::MapBuilding::Relocalizer::detectFreakFeatures))
			{
				Log::error() << "Failed to set feature detection function";
				return invalidObjectId();
			}

			if (!trackerStereo_.setFeatureMap(std::move(featureMap)))
			{
				Log::error() << "Failed to initialize feature map";
				return invalidObjectId();
			}

			objectPoints_ = trackerStereo_.objectPoints();

			objectPointIds_.clear();
			objectPointIds_.reserve(objectPoints_.size());

			for (const Index32& objectPointId : trackerStereo_.objectPointIds())
			{
				objectPointIds_.emplace_back(Index64(objectPointId));
			}

			ocean_assert(objectPoints_.size() == objectPointIds_.size());

			mapObjectId_ = addUniqueObjectId(description);
		}
	}

	if (mapObjectId_ == invalidObjectId())
	{
		Log::info() << "The provided feature map '" << description << "' could not be loaded";
	}

	return mapObjectId_;
}

bool OnDeviceRelocalizerTracker6DOF::isStarted() const
{
	return isThreadActive();
}

bool OnDeviceRelocalizerTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (frameMediums_.size() != 2)
	{
		return false;
	}

	if (isThreadActive())
	{
		return true;
	}

	startThread();

	Log::info() << "6DOF On-Device Relocalizer tracker started.";

	return true;
}

bool OnDeviceRelocalizerTracker6DOF::stop()
{
	stopThread();
	return true;
}

bool OnDeviceRelocalizerTracker6DOF::setParameter(const std::string& parameter, const Value& value)
{
	if (parameter == "minimalCorrespondences" && value.isInt())
	{
		const int inValue = value.intValue();

		if (inValue >= 20 && inValue < 1000)
		{
			const ScopedLock scopedLock(deviceLock);

			minimalNumberCorrespondences_ = (unsigned int)(inValue);
			return true;
		}
		else
		{
			Log::error() << "minimalCorrespondences value out of range [20, 1000]: " << value.intValue();
		}
	}
	else if (parameter == "maximalProjectionError" && value.isFloat64(true))
	{
		const double floatValue = value.float64Value(true);

		if (floatValue >= 0.0 && floatValue < 10.0)
		{
			const ScopedLock scopedLock(deviceLock);

			maximalProjectionError_ = Scalar(floatValue);
			return true;
		}
		else
		{
			Log::error() << "maximalProjectionError value out of range [0, 10]: " << value.float64Value();
		}
	}

	return Device::setParameter(parameter, value);
}

bool OnDeviceRelocalizerTracker6DOF::parameter(const std::string& parameter, Value& value)
{
	if (parameter == "minimalCorrespondences")
	{
		const ScopedLock scopedLock(deviceLock);

		value = Value(int(minimalNumberCorrespondences_));

		return true;
	}
	else if (parameter == "maximalProjectionError")
	{
		const ScopedLock scopedLock(deviceLock);

		value = Value(double(maximalProjectionError_));

		return true;
	}

	return Device::parameter(parameter, value);
}

bool OnDeviceRelocalizerTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	if (mapObjectId_ == objectId)
	{
		return isMapTracked_;
	}

	return false;
}

void OnDeviceRelocalizerTracker6DOF::update(ovrMobile* ovr, const Platform::Meta::Quest::Device::DeviceType /*deviceType*/, TrackedRemoteDevice& /*trackedRemoteDevice*/, const Timestamp& timestamp)
{
	ocean_assert(ovr != nullptr);
	ocean_assert(timestamp.isValid());

	ovr_ = ovr;

	TemporaryScopedLock temporaryScopedLock(deviceLock);
		const ObjectId mapObjectId(mapObjectId_);
	temporaryScopedLock.release();

	if (mapObjectId == invalidObjectId())
	{
		return;
	}

	const HomogenousMatrix4 headsetWorld_T_device = HeadsetPoses::world_T_device(ovr, timestamp);

	if (!headsetWorld_T_device.isValid())
	{
		return;
	}

	const HomogenousMatrix4 relocalizedWorld_T_headsetWorld(relocalizedWorld_T_headsetWorld_.transformation(timestamp)); // 7-DOF transformation

	if (!relocalizedWorld_T_headsetWorld.isValid())
	{
		return;
	}

	const HomogenousMatrix4 relocalizedWorld_T_device(relocalizedWorld_T_headsetWorld * headsetWorld_T_device);

	ObjectIds sampleObjectIds(1, mapObjectId);
	Tracker6DOFSample::Positions samplePositions(1, relocalizedWorld_T_device.translation());
	Tracker6DOFSample::Orientations sampleOrientations(1, relocalizedWorld_T_device.rotation());
	SharedSceneElements sceneElements(1, nullptr);

	if (!isMapTracked_.load())
	{
		postFoundTrackerObjects({mapObjectId}, timestamp);

		const ScopedLock scopedLock(deviceLock);

		if (!objectPoints_.empty())
		{
			sceneElements[0] = std::make_shared<SceneElementObjectPoints>(Vectors3(objectPoints_));
		}
	}

	postNewSample(SampleRef(new SceneTracker6DOFSample(timestamp, RS_OBJECT_IN_DEVICE, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions), std::move(sceneElements))));

	isMapTracked_ = true;
}

void OnDeviceRelocalizerTracker6DOF::threadRun()
{
	TemporaryScopedLock temporaryScopedLock(deviceLock);
		const Media::FrameMediumRefs frameMediums(frameMediums_);

		const unsigned int minimalNumberCorrespondences(minimalNumberCorrespondences_);
		const Scalar maximalProjectionError(maximalProjectionError_);
	temporaryScopedLock.release();

	if (frameMediums.size() != 2)
	{
		Log::error() << "Relocalizer needs two input mediums, got " << frameMediums.size();
		return;
	}

	ocean_assert(mapObjectId_ != invalidObjectId());
	ocean_assert(isMapTracked_.load() == false);

	Frame yFrameA;
	Frame yFrameB;

	Timestamp frameTimestamp(false);

	HomogenousMatrices4 relocalizedWorld_T_devices;
	HomogenousMatrices4 headsetWorld_T_devices;

	HomogenousMatrix4 relocalizedWorld_T_headsetWorld(false);

	FrameRefs frames;
	SharedAnyCameras cameras;

	while (shouldThreadStop() == false)
	{
		if (!trackerStereo_.isValid())
		{
			sleep(5u);
			continue;
		}

		frames.clear();
		cameras.clear();

		bool timedOut = false;
		if (!Media::FrameMedium::syncedFrames(frameMediums, frameTimestamp, frames, cameras, 2u /*waitTime*/, &timedOut))
		{
			if (timedOut)
			{
				Log::warning() << "Failed to access synced camera frames for timestamp";
			}

			continue;
		}

		ocean_assert(frames.size() == 2 && cameras.size() == 2);

		frameTimestamp = frames.front()->timestamp();

		if (!CV::FrameConverter::Comfort::convert(*frames[0], FrameType(*frames[0], FrameType::FORMAT_Y8), yFrameA, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, nullptr, CV::FrameConverter::Options(0.6f, true))
			|| !CV::FrameConverter::Comfort::convert(*frames[1], FrameType(*frames[1], FrameType::FORMAT_Y8), yFrameB, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, nullptr, CV::FrameConverter::Options(0.6f, true)))
		{
			ocean_assert(false && "This should never happen!");
			break;
		}

		ovrMobile* ovr = ovr_;

		if (ovr == nullptr)
		{
			sleep(1u);
			continue;
		}

		const HomogenousMatrix4 headsetWorld_T_device = HeadsetPoses::world_T_device(ovr, frameTimestamp);

		if (!headsetWorld_T_device.isValid())
		{
			sleep(1u);
			continue;
		}

		const SharedAnyCamera& cameraA = cameras[0];
		const SharedAnyCamera& cameraB = cameras[1];

		const HomogenousMatrix4 device_T_cameraA(frameMediums[0]->device_T_camera());
		const HomogenousMatrix4 device_T_cameraB(frameMediums[1]->device_T_camera());

		HomogenousMatrix4 relocalizedWorld_T_roughDevice(false);

		if (relocalizedWorld_T_headsetWorld.isValid())
		{
			relocalizedWorld_T_roughDevice = relocalizedWorld_T_headsetWorld * headsetWorld_T_device;
		}

		HomogenousMatrix4 relocalizedWorld_T_device(false);
		if (trackerStereo_.track(*cameraA, *cameraB, device_T_cameraA, device_T_cameraB, yFrameA, yFrameB, relocalizedWorld_T_device, minimalNumberCorrespondences, maximalProjectionError, relocalizedWorld_T_roughDevice, WorkerPool::get().scopedWorker()()))
		{
			relocalizedWorld_T_devices.emplace_back(relocalizedWorld_T_device);
			headsetWorld_T_devices.emplace_back(headsetWorld_T_device);

			Scalar scale;
			if (Geometry::AbsoluteTransformation::calculateTransformationWithOutliers(headsetWorld_T_devices.data(), relocalizedWorld_T_devices.data(), headsetWorld_T_devices.size(), relocalizedWorld_T_headsetWorld, Scalar(0.5), Geometry::AbsoluteTransformation::ScaleErrorType::Symmetric, &scale) && scale >= Scalar(0.9) && scale <= Scalar(1.1))
			{
				relocalizedWorld_T_headsetWorld.applyScale(Vector3(scale, scale, scale));

				relocalizedWorld_T_headsetWorld_.setTransformation(relocalizedWorld_T_headsetWorld, frameTimestamp);
			}
		}
		else
		{
			relocalizedWorld_T_headsetWorld.toNull();
		}
	}

	if (isMapTracked_.load())
	{
		postLostTrackerObjects({mapObjectId_}, frameTimestamp);
	}

	isMapTracked_ = false;
}

bool OnDeviceRelocalizerTracker6DOF::readObjectPointsFromStream(IO::InputBitstream& bitstream, Vectors3& objectPoints, Indices32& objectPointIds)
{
	constexpr unsigned int maximalObjectPoints = 100u * 1000u;

	/// The unique tag for object points.
	static constexpr uint64_t objectPointsTag = IO::Tag::string2tag("_OCNOPT_");

	unsigned long long tag = 0ull;
	if (!bitstream.read<unsigned long long>(tag) || tag != objectPointsTag)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberObjectPoints = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPoints))
	{
		return false;
	}

	if (numberObjectPoints > maximalObjectPoints)
	{
		return false;
	}

	VectorsF3 objectPointsF(numberObjectPoints);
	if (!bitstream.read(objectPointsF.data(), objectPointsF.size() * sizeof(VectorF3)))
	{
		return false;
	}

	objectPoints = Vector3::vectors2vectors(std::move(objectPointsF));

	unsigned int numberObjectPointIds = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPointIds))
	{
		return false;
	}

	if (numberObjectPoints != numberObjectPointIds)
	{
		return false;
	}

	objectPointIds.resize(numberObjectPointIds);

	if (!bitstream.read(objectPointIds.data(), objectPointIds.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

}

}

}

}
