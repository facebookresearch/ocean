// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/cloud/CloudRelocalizerTracker6DOF.h"

#include "ocean/tracking/cloud/CloudRelocalizer.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

CloudRelocalizerTracker6DOF::CloudRelocalizerTracker6DOF(const bool useFrameToFrameTracking) :
	Device(deviceNameCloudRelocalizerTracker6DOF(useFrameToFrameTracking),  deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	Measurement(deviceNameCloudRelocalizerTracker6DOF(useFrameToFrameTracking), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	Tracker(deviceNameCloudRelocalizerTracker6DOF(useFrameToFrameTracking), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	OrientationTracker3DOF(deviceNameCloudRelocalizerTracker6DOF(useFrameToFrameTracking)),
	PositionTracker3DOF(deviceNameCloudRelocalizerTracker6DOF(useFrameToFrameTracking)),
	Tracker6DOF(deviceNameCloudRelocalizerTracker6DOF(useFrameToFrameTracking)),
	SceneTracker6DOF(deviceNameCloudRelocalizerTracker6DOF(useFrameToFrameTracking)),
	ObjectTracker(deviceNameCloudRelocalizerTracker6DOF(useFrameToFrameTracking), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	VisualTracker(deviceNameCloudRelocalizerTracker6DOF(useFrameToFrameTracking), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	CloudDevice(deviceNameCloudRelocalizerTracker6DOF(useFrameToFrameTracking), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	CloudRelocalizerSLAMTracker6DOF(deviceNameCloudRelocalizerTracker6DOF(useFrameToFrameTracking), useFrameToFrameTracking)
{
	// nothing to do here
}

CloudRelocalizerTracker6DOF::~CloudRelocalizerTracker6DOF()
{
	stop();
}

bool CloudRelocalizerTracker6DOF::invokeRelocalization(const AnyCamera& camera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector, const Tracking::Cloud::CloudRelocalizer::Configuration& configuration, facebook::mobile::xr::IRelocalizationClient& relocalizationClient, HomogenousMatrix4& relocalizationReference_T_camera, Vectors3& objectPoints, Vectors2& imagePoints)
{
	if (!Tracking::Cloud::CloudRelocalizer::relocalize(camera, yFrame, gpsLocation, gravityVector, configuration, relocalizationClient, relocalizationReference_T_camera, &objectPoints, &imagePoints))
	{
		return false;
	}

	const ScopedLock scopedLock(deviceLock);

	// making a copy for the scene tracker sample
	latestCloudRelocObjectPoints_ = objectPoints;
	latestCloudRelocImagePoints_ = imagePoints;

	return true;
}

bool CloudRelocalizerTracker6DOF::determineSceneElements(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& anchor_T_world, const Timestamp& /*timestamp*/, ObjectIds& objectIds, Vectors3& objects_t_camera, Quaternions& objects_q_camera, SharedSceneElements& sceneElements, Metadata& /*metadata*/)
{
	ocean_assert(world_T_camera.isValid());
	ocean_assert(anchor_T_world.isValid());

	ocean_assert(objectIds.empty() && objects_t_camera.empty() && objects_q_camera.empty() && sceneElements.empty());

	const ScopedLock scopedLock(deviceLock);

	if (objectId_ != invalidObjectId())
	{
		// we have only one object transformation (the transformation for the anchor)

		const HomogenousMatrix4 anchor_T_camera = anchor_T_world * world_T_camera;

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

	ocean_assert(objectIds.size() == objects_t_camera.size());
	ocean_assert(objectIds.size() == objects_q_camera.size());
	ocean_assert(objectIds.size() == sceneElements.size());

	return true;
}

}

}

}
