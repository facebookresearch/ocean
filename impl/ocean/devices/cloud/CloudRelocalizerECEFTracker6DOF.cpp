// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/cloud/CloudRelocalizerECEFTracker6DOF.h"

#include "ocean/base/Lock.h"
#include "ocean/tracking/cloud/CloudRelocalizer.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

CloudRelocalizerECEFTracker6DOF::CloudRelocalizerECEFTracker6DOF() :
	Device(deviceNameCloudRelocalizerECEFTracker6DOF(),  deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	Measurement(deviceNameCloudRelocalizerECEFTracker6DOF(), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	Tracker(deviceNameCloudRelocalizerECEFTracker6DOF(), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	OrientationTracker3DOF(deviceNameCloudRelocalizerECEFTracker6DOF()),
	PositionTracker3DOF(deviceNameCloudRelocalizerECEFTracker6DOF()),
	Tracker6DOF(deviceNameCloudRelocalizerECEFTracker6DOF()),
	SceneTracker6DOF(deviceNameCloudRelocalizerECEFTracker6DOF()),
	ObjectTracker(deviceNameCloudRelocalizerECEFTracker6DOF(), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	VisualTracker(deviceNameCloudRelocalizerECEFTracker6DOF(), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	CloudDevice(deviceNameCloudRelocalizerECEFTracker6DOF(), deviceTypeCloudRelocalizerSLAMTracker6DOF()),
	CloudRelocalizerSLAMTracker6DOF(deviceNameCloudRelocalizerECEFTracker6DOF(), true /*useFrameToFrameTracking*/)
{
	// nothing to do here
}

CloudRelocalizerECEFTracker6DOF::~CloudRelocalizerECEFTracker6DOF()
{
	stop();
}

HomogenousMatrixD4 CloudRelocalizerECEFTracker6DOF::reference() const
{
	const ScopedLock scopedLock(deviceLock);

	return ecefWorld_T_ecefReference_;
}

bool CloudRelocalizerECEFTracker6DOF::invokeRelocalization(const AnyCamera& camera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector, const Tracking::Cloud::CloudRelocalizer::Configuration& configuration, facebook::mobile::xr::IRelocalizationClient& relocalizationClient, HomogenousMatrix4& relocalizationReference_T_camera, Vectors3& objectPoints, Vectors2& imagePoints)
{
	HomogenousMatrixD4 ecefWorld_T_camera(false);

	VectorsD3 ecefObjectPoints;
	VectorsD2 ecefImagePoints;

	if (!Tracking::Cloud::CloudRelocalizer::relocalizeToECEF(camera, yFrame, gpsLocation, gravityVector, configuration, relocalizationClient, ecefWorld_T_camera, &ecefObjectPoints, &ecefImagePoints))
	{
		return false;
	}

	TemporaryScopedLock scopedLock(deviceLock);

		if (!ecefWorld_T_ecefReference_.isValid())
		{
			// as reference we use the very first relocalization pose
			ecefWorld_T_ecefReference_ = ecefWorld_T_camera;
		}

		ocean_assert(ecefWorld_T_ecefReference_.isValid());

		const HomogenousMatrixD4 ecefReference_T_ecefWorld(ecefWorld_T_ecefReference_.inverted());

	scopedLock.release();

	relocalizationReference_T_camera = HomogenousMatrix4(ecefReference_T_ecefWorld * ecefWorld_T_camera);

	objectPoints.clear();
	objectPoints.reserve(ecefObjectPoints.size());

	for (const VectorD3& ecefObjectPoint : ecefObjectPoints)
	{
		// the object points are stored in relation to the ECEF reference coordinate system
		objectPoints.emplace_back(ecefReference_T_ecefWorld * ecefObjectPoint);
	}

	imagePoints.clear();
	imagePoints.reserve(ecefImagePoints.size());

	for (const VectorD2& ecefImagePoint : ecefImagePoints)
	{
		imagePoints.emplace_back(ecefImagePoint);
	}

	// making a copy for the scene tracker sample
	latestCloudRelocObjectPoints_ = objectPoints;
	latestCloudRelocImagePoints_ = imagePoints;

	return true;
}

bool CloudRelocalizerECEFTracker6DOF::determineSceneElements(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& anchor_T_world, const Timestamp& /*timestamp*/, ObjectIds& objectIds, Vectors3& objects_t_camera, Quaternions& objects_q_camera, SharedSceneElements& sceneElements, Metadata& /*metadata*/)
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

		Vectors3 latestCloudRelocObjectPoints(std::move(latestCloudRelocObjectPoints_));
		Vectors2 latestCloudRelocImagePoints(std::move(latestCloudRelocImagePoints_));
		ocean_assert(latestCloudRelocObjectPoints.size() == latestCloudRelocImagePoints.size());

		if (latestCloudRelocObjectPoints.empty())
		{
			sceneElements.emplace_back(nullptr); // a pure pose scene element
		}
		else
		{
			sceneElements.emplace_back(std::make_shared<SceneElementFeatureCorrespondences>(std::move(latestCloudRelocObjectPoints), std::move(latestCloudRelocImagePoints)));
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
