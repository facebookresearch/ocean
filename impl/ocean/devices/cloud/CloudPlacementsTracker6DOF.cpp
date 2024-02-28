// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/cloud/CloudPlacementsTracker6DOF.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/tracking/cloud/CloudRelocalizer.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

CloudPlacementsTracker6DOF::CloudPlacementsTracker6DOF() :
	Device(deviceNameCloudPlacementsTracker6DOF(),  deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	Measurement(deviceNameCloudPlacementsTracker6DOF(), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	Tracker(deviceNameCloudPlacementsTracker6DOF(), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	OrientationTracker3DOF(deviceNameCloudPlacementsTracker6DOF()),
	PositionTracker3DOF(deviceNameCloudPlacementsTracker6DOF()),
	Tracker6DOF(deviceNameCloudPlacementsTracker6DOF()),
	SceneTracker6DOF(deviceNameCloudPlacementsTracker6DOF()),
	ObjectTracker(deviceNameCloudPlacementsTracker6DOF(), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	VisualTracker(deviceNameCloudPlacementsTracker6DOF(), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	CloudDevice(deviceNameCloudPlacementsTracker6DOF(), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	CloudRelocalizerSLAMTracker6DOF(deviceNameCloudPlacementsTracker6DOF(), true /*useFrameToFrameTracking*/)
{
	// nothing to do here
}

CloudPlacementsTracker6DOF::~CloudPlacementsTracker6DOF()
{
	stop();
}

bool CloudPlacementsTracker6DOF::invokeRelocalization(const AnyCamera& camera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector, const Tracking::Cloud::CloudRelocalizer::Configuration& configuration, facebook::mobile::xr::IRelocalizationClient& relocalizationClient, HomogenousMatrix4& relocalizationReference_T_camera, Vectors3& objectPoints, Vectors2& imagePoints)
{
	Tracking::Cloud::CloudRelocalizer::Placements placements;

	const bool result = Tracking::Cloud::CloudRelocalizer::relocalizeWithPlacements(camera, yFrame, gpsLocation, gravityVector, configuration, relocalizationClient, relocalizationReference_T_camera, placements, &objectPoints, &imagePoints);

	const ScopedLock scopedLock(deviceLock);

	latestPlacements_ = std::move(placements);

	// making a copy for the scene tracker sample
	latestCloudRelocObjectPoints_ = objectPoints;
	latestCloudRelocImagePoints_ = imagePoints;

	return result;
}

bool CloudPlacementsTracker6DOF::determineSceneElements(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& anchor_T_world, const Timestamp& timestamp, ObjectIds& objectIds, Vectors3& objects_t_camera, Quaternions& objects_q_camera, SharedSceneElements& sceneElements, Metadata& metadata)
{
	ocean_assert(world_T_camera.isValid());
	ocean_assert(anchor_T_world.isValid());

	ocean_assert(objectIds.empty() && objects_t_camera.empty() && objects_q_camera.empty() && sceneElements.empty());

	const HomogenousMatrix4 anchor_T_camera = anchor_T_world * world_T_camera;

	const ScopedLock scopedLock(deviceLock);

	objectIds.reserve(latestPlacements_.size() + 1);
	objects_t_camera.reserve(latestPlacements_.size() + 1);
	objects_q_camera.reserve(latestPlacements_.size() + 1);
	sceneElements.reserve(latestPlacements_.size() + 1);

	if (objectId_ != invalidObjectId())
	{
		objectIds.emplace_back(objectId_);
		objects_t_camera.emplace_back(anchor_T_camera.translation());
		objects_q_camera.emplace_back(anchor_T_camera.rotation());

		ocean_assert(latestCloudRelocObjectPoints_.size() == latestCloudRelocImagePoints_.size());

		if (latestCloudRelocObjectPoints_.empty())
		{
			sceneElements.emplace_back(nullptr); // a pure pose scene element
		}
		else
		{
			sceneElements.emplace_back(std::make_shared<SceneElementFeatureCorrespondences>(std::move(latestCloudRelocObjectPoints_), std::move(latestCloudRelocImagePoints_)));
		}
	}

	for (const Tracking::Cloud::CloudRelocalizer::Placement& placement : latestPlacements_)
	{
		ocean_assert(placement.anchor_T_placement().isValid());

		const HomogenousMatrix4 placement_T_anchor = placement.anchor_T_placement().inverted();
		const HomogenousMatrix4 placement_T_camera = placement_T_anchor * anchor_T_camera;

		ObjectId placementObjectId = objectId(placement.identification());

		if (placementObjectId == invalidObjectId())
		{
			placementObjectId = addUniqueObjectId(placement.identification());
		}

		objectIds.emplace_back(placementObjectId);
		objects_t_camera.emplace_back(placement_T_camera.translation());
		objects_q_camera.emplace_back(placement_T_camera.rotation());

		sceneElements.emplace_back(nullptr); // a pure pose scene element
	}

	ocean_assert(objectIds.size() == objects_t_camera.size());
	ocean_assert(objectIds.size() == objects_q_camera.size());
	ocean_assert(objectIds.size() == sceneElements.size());

	return true;
}

}

}

}
