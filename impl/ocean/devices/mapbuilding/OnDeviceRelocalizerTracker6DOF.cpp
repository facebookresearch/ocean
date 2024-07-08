/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/mapbuilding/OnDeviceRelocalizerTracker6DOF.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameShrinker.h"

#include "ocean/io/Bitstream.h"
#include "ocean/io/File.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Rotation.h"

#include "ocean/tracking/Utilities.h"

#include "ocean/tracking/mapbuilding/Utilities.h"

namespace Ocean
{

namespace Devices
{

namespace MapBuilding
{

using namespace Ocean::Tracking::MapBuilding;

OnDeviceRelocalizerTracker6DOF::OnDeviceRelocalizerTracker6DOF() :
	Device(deviceNameOnDeviceRelocalizerTracker6DOF(), deviceTypeOnDeviceRelocalizerTracker6DOF()),
	MapBuildingDevice(deviceNameOnDeviceRelocalizerTracker6DOF(), deviceTypeOnDeviceRelocalizerTracker6DOF()),
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

	if (!mapFile.exists() || mapFile.extension() != "ocean_map")
	{
		Log::info() << "The provide feature map '" << mapFile() << "' does not exist";
		return invalidObjectId();
	}

	std::ifstream stream(mapFile(), std::ios::binary);
	IO::InputBitstream inputBitstream(stream);

	Tracking::Database database;
	std::shared_ptr<Tracking::MapBuilding::UnifiedDescriptorMap> descriptorMap;

	if (!Tracking::Utilities::readDatabase(inputBitstream, database)
		|| !Tracking::MapBuilding::Utilities::readDescriptorMap(inputBitstream, descriptorMap))
	{
		Log::error() << "Failed to read feature amp file";
		return invalidObjectId();
	}

	ocean_assert(descriptorMap);

	Vectors3 databaseObjectPoints;
	Indices32 databaseObjectPointIds = database.objectPointIds<false, false>(Tracking::Database::invalidObjectPoint(), &databaseObjectPoints);

	using ImagePointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptor256;
	using ObjectPointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptors256;
	using ObjectPointVocabularyDescriptor = Tracking::MapBuilding::UnifiedDescriptor::BinaryDescriptor<256u>;

	using UnifiedFeatureMap = Tracking::MapBuilding::UnifiedFeatureMapT<ImagePointDescriptor, ObjectPointDescriptor, ObjectPointVocabularyDescriptor>;

	RandomGenerator randomGenerator;
	Tracking::MapBuilding::SharedUnifiedFeatureMap featureMap = std::make_shared<UnifiedFeatureMap>(std::move(databaseObjectPoints), std::move(databaseObjectPointIds), std::move(descriptorMap), randomGenerator, &UnifiedFeatureMap::VocabularyForest::TVocabularyTree::determineClustersMeanForBinaryDescriptor<256u>, &Tracking::MapBuilding::UnifiedHelperFreakMultiDescriptor256::extractVocabularyDescriptorsFromMap);

	if (!relocalizer_.setImageFeaturePointDetectorFunction(Tracking::MapBuilding::Relocalizer::detectFreakFeatures))
	{
		Log::error() << "Failed to set feature detection function";
		return invalidObjectId();
	}

	if (!relocalizer_.setFeatureMap(std::move(featureMap)))
	{
		Log::error() << "Failed to initialize feature map";
		return invalidObjectId();
	}

	objectPoints_ = relocalizer_.objectPoints();

	objectPointIds_.clear();
	objectPointIds_.reserve(objectPoints_.size());

	const Indices32& objectPointIds = relocalizer_.objectPointIds();

	for (const Index32& objectPointId : objectPointIds)
	{
		objectPointIds_.emplace_back(Index64(objectPointId));
	}

	ocean_assert(objectPoints_.size() == objectPointIds_.size());

	mapObjectId_ = addUniqueObjectId(description);

	return mapObjectId_;
}

bool OnDeviceRelocalizerTracker6DOF::isStarted() const
{
	return isThreadActive();
}

bool OnDeviceRelocalizerTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
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

bool OnDeviceRelocalizerTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	if (mapObjectId_ == objectId)
	{
		return isMapTracked_;
	}

	return false;
}

void OnDeviceRelocalizerTracker6DOF::threadRun()
{
	TemporaryScopedLock temporaryScopedLock(deviceLock);
		if (frameMediums_.size() != 1 || frameMediums_.front().isNull())
		{
			return;
		}

		const Media::FrameMediumRef frameMedium = frameMediums_.front();
	temporaryScopedLock.release();

	ocean_assert(mapObjectId_ != invalidObjectId());
	ocean_assert(isMapTracked_ == false);

	Timestamp frameTimestamp(false);
	bool objectPointsPublished = false;

	HighPerformanceStatistic performance;

	HomogenousMatrix4 world_T_previousCamera(false);

	while (shouldThreadStop() == false)
	{
		SharedAnyCamera frameAnyCamera;
		const FrameRef frame = frameMedium->frame(&frameAnyCamera);

		if (frame && frameAnyCamera && frame->timestamp() > frameTimestamp)
		{
			ocean_assert(frame->isValid() && frameAnyCamera->isValid());

			frameTimestamp = frame->timestamp();

			if (performance.measurements() >= 100u)
			{
				Log::info() << "Performance: " << performance.averageMseconds() << "ms";
				performance.reset();
			}

			HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

			HomogenousMatrix4 world_T_camera(false);

			ocean_assert(relocalizer_.isValid());

			if (!CV::FrameConverter::Comfort::convert(*frame, FrameType::FORMAT_Y8, yFrame_, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
			{
				ocean_assert(false && "This should never happen!");
				break;

			}

			while (yFrame_.width() * yFrame_.height() > 1280u * 720u)
			{
				CV::FrameShrinker::downsampleByTwo11(yFrame_);
			}

			SharedAnyCamera anyCamera = frameAnyCamera;

			if (yFrame_.width() != anyCamera->width() || yFrame_.height() != anyCamera->height())
			{
				anyCamera = frameAnyCamera->clone(yFrame_.width(), yFrame_.height());

				if (!anyCamera)
				{
					ocean_assert(false && "Failed to scale camera profile!");
					break;
				}
			}

			constexpr unsigned int minimalNumberCorrespondences = 20u;
			constexpr Scalar maximalProjectionError = Scalar(3.5);
			constexpr Scalar inlierRate = Scalar(0.15);

			if (relocalizer_.relocalize(*anyCamera, yFrame_, world_T_camera, minimalNumberCorrespondences, maximalProjectionError, inlierRate, world_T_previousCamera, WorkerPool::get().scopedWorker()()))
			{
				scopedPerformance.release();

				if (!isMapTracked_)
				{
					postFoundTrackerObjects({mapObjectId_}, frameTimestamp);
				}

				SharedSceneElements sceneElements(1, nullptr);

				if (!objectPointsPublished)
				{
					sceneElements[0] = std::make_shared<SceneElementObjectPoints>(Vectors3(objectPoints_), Indices64(objectPointIds_));

					objectPointsPublished = true;
				}

				postNewSample(SampleRef(new SceneTracker6DOFSample(frameTimestamp, RS_DEVICE_IN_OBJECT, {mapObjectId_}, {world_T_camera.rotation()}, {world_T_camera.translation()}, std::move(sceneElements))));
			}
			else if (isMapTracked_)
			{
				scopedPerformance.release();

				postLostTrackerObjects({mapObjectId_}, frameTimestamp);
			}

			isMapTracked_ = world_T_camera.isValid();

			world_T_previousCamera = world_T_camera; // can be valid or invalid
		}
		else
		{
			sleep(1);
		}
	}

	const Timestamp stopTimestamp(true);

	if (isMapTracked_)
	{
		postLostTrackerObjects({mapObjectId_}, frameTimestamp);

		isMapTracked_ = false;
	}
}

}

}

}
