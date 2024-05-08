/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/AKFaceTracker6DOF.h"

#include "ocean/media/avfoundation/AVFoundation.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

AKFaceTracker6DOF::AKFaceTracker6DOF() :
	Device(deviceNameAKFaceTracker6DOF(), deviceTypeAKFaceTracker6DOF()),
	AKDevice(TrackerCapabilities(TC_FACE | TC_SLAM), deviceNameAKFaceTracker6DOF(), deviceTypeAKFaceTracker6DOF()),
	Measurement(deviceNameAKFaceTracker6DOF(), deviceTypeAKFaceTracker6DOF()),
	Tracker(deviceNameAKFaceTracker6DOF(), deviceTypeAKFaceTracker6DOF()),
	OrientationTracker3DOF(deviceNameAKFaceTracker6DOF()),
	PositionTracker3DOF(deviceNameAKFaceTracker6DOF()),
	Tracker6DOF(deviceNameAKFaceTracker6DOF()),
	VisualTracker(deviceNameAKFaceTracker6DOF(), deviceTypeAKFaceTracker6DOF())
{
	faceObjectId_ = addUniqueObjectId("Face");
}

AKFaceTracker6DOF::~AKFaceTracker6DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool AKFaceTracker6DOF::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (isStarted_)
	{
		return true;
	}

	if (frameMediums_.size() == 1 && frameMediums_.front())
	{
		isStarted_ = ARSessionManager::get().start(this, frameMediums_.front());
	}
	else
	{
		Log::error() << "AKFaceTracker6DOF needs a valid input medium before it can be started";
	}

	return isStarted_;
}

bool AKFaceTracker6DOF::pause()
{
	TemporaryScopedLock scopedLock(deviceLock);

	if (!ARSessionManager::get().pause(this))
	{
		return false;
	}

	ObjectIdSet lostObjects;

	if (faceIsTracked_)
	{
		lostObjects.emplace(faceObjectId_);
		faceIsTracked_ = false;
	}

	scopedLock.release();

	postLostTrackerObjects(lostObjects, Timestamp(true));

	return true;
}

bool AKFaceTracker6DOF::stop()
{
	TemporaryScopedLock scopedLock(deviceLock);

	if (!isStarted_)
	{
		return true;
	}

	if (!ARSessionManager::get().stop(this))
	{
		return false;
	}

	if (!faceIsTracked_)
	{
		return true;
	}

	ObjectIdSet lostObjects = {faceObjectId_};
	faceIsTracked_ = false;

	isStarted_ = false;

	scopedLock.release();

	postLostTrackerObjects(lostObjects, Timestamp(true));

	return true;
}

bool AKFaceTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(objectId == faceObjectId_);

	return faceIsTracked_ && objectId == faceObjectId_;
}

void AKFaceTracker6DOF::setInput(Media::FrameMediumRefs&& frameMediums)
{
	if (frameMediums.size() == 1 && frameMediums.front())
	{
		const Media::FrameMediumRef& frameMedium = frameMediums.front();

		if (frameMedium->library() == Media::AVFoundation::nameAVFLibrary() && (frameMedium->type() & Media::Medium::LIVE_VIDEO) == Media::Medium::LIVE_VIDEO)
		{
			if (frameMedium->url() == "Back Camera" || frameMedium->url() == "Front Camera")
			{
				VisualTracker::setInput(std::move(frameMediums));
				return;
			}
		}
	}

	ocean_assert(false && "Invalid frameMedium!");
}

void AKFaceTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp, ARFrame* arFrame)
{
	ocean_assert(arFrame != nullptr);

	HomogenousMatrixF4 world_T_face(false);

	for (ARAnchor* anchor in arFrame.anchors)
	{
		if (![anchor isKindOfClass:[ARFaceAnchor class]])
		{
			continue;
		}

		ARFaceAnchor* faceAnchor = (ARFaceAnchor*)(anchor);

		const simd_float4x4 simdTransform = faceAnchor.transform;

		memcpy(world_T_face.data() +  0, &simdTransform.columns[0], sizeof(float) * 4);
		memcpy(world_T_face.data() +  4, &simdTransform.columns[1], sizeof(float) * 4);
		memcpy(world_T_face.data() +  8, &simdTransform.columns[2], sizeof(float) * 4);
		memcpy(world_T_face.data() + 12, &simdTransform.columns[3], sizeof(float) * 4);

		ocean_assert(NumericF::isWeakEqual(world_T_face.rotationMatrix().determinant(), -1.0f));

		// ARKit's pose is mirrored, so that we need to fix the pose
		world_T_face.applyScale(Vector3(-1, 1, 1));

		ocean_assert(NumericF::isWeakEqual(world_T_face.rotationMatrix().determinant(), 1.0f));

		break;
	}

	TemporaryScopedLock scopedLock(deviceLock);

	if (world_T_camera.isValid() && world_T_face.isValid())
	{
		ocean_assert(faceObjectId_ != invalidObjectId());

		const HomogenousMatrix4 face_T_camera = HomogenousMatrix4(world_T_face).inverted() * world_T_camera;

		ObjectIdSet foundObjectIds;

		ObjectIds sampleObjectIds(1, faceObjectId_);
		Tracker6DOFSample::Positions samplePositions(1, face_T_camera.translation());
		Tracker6DOFSample::Orientations sampleOrientations(1, face_T_camera.rotation());

		if (!faceIsTracked_)
		{
			foundObjectIds = {faceObjectId_};
			faceIsTracked_ = true;
		}

		scopedLock.release();

		postFoundTrackerObjects(foundObjectIds, timestamp);

		Metadata metadata;
		if (arFrame.lightEstimate != nullptr)
		{
			metadata["ambientColorTemperature"] = Value(arFrame.lightEstimate.ambientColorTemperature);
			metadata["ambientIntensity"] = Value(arFrame.lightEstimate.ambientIntensity);
		}

		postNewSample(SampleRef(new Tracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions), std::move(metadata))));
	}
	else
	{
		ObjectIdSet lostObjectIds;

		if (faceIsTracked_)
		{
			lostObjectIds = {faceObjectId_};
			faceIsTracked_ = false;
		}

		scopedLock.release();

		postLostTrackerObjects(lostObjectIds, timestamp);
	}
}

}

}

}
