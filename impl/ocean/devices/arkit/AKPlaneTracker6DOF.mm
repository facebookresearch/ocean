/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/AKPlaneTracker6DOF.h"

#include "ocean/base/StringApple.h"

#include "ocean/media/avfoundation/AVFoundation.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

AKPlaneTracker6DOF::AKPlaneTracker6DOF() :
	Device(deviceNameAKPlaneTracker6DOF(), deviceTypeAKPlaneTracker6DOF()),
	AKDevice(TrackerCapabilities(TC_SLAM | TC_PLANE_DETECTION), deviceNameAKPlaneTracker6DOF(), deviceTypeAKPlaneTracker6DOF()),
	Measurement(deviceNameAKPlaneTracker6DOF(), deviceTypeAKPlaneTracker6DOF()),
	Tracker(deviceNameAKPlaneTracker6DOF(), deviceTypeAKPlaneTracker6DOF()),
	OrientationTracker3DOF(deviceNameAKPlaneTracker6DOF()),
	PositionTracker3DOF(deviceNameAKPlaneTracker6DOF()),
	Tracker6DOF(deviceNameAKPlaneTracker6DOF()),
	SceneTracker6DOF(deviceNameAKPlaneTracker6DOF()),
	VisualTracker(deviceNameAKPlaneTracker6DOF(), deviceTypeAKPlaneTracker6DOF())
{
	worldObjectId_ = addUniqueObjectId("World");
}

AKPlaneTracker6DOF::~AKPlaneTracker6DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool AKPlaneTracker6DOF::start()
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
		Log::error() << "AKPlaneTracker6DOF needs one valid input medium before it can be started";
	}

	return isStarted_;
}

bool AKPlaneTracker6DOF::pause()
{
	TemporaryScopedLock scopedLock(deviceLock);

	if (!ARSessionManager::get().pause(this))
	{
		return false;
	}

	ObjectIdSet lostObjects;

	if (worldIsTracked_)
	{
		lostObjects.emplace(worldObjectId_);
		worldIsTracked_ = false;
	}

	scopedLock.release();

	postLostTrackerObjects(lostObjects, Timestamp(true));

	return true;
}

bool AKPlaneTracker6DOF::stop()
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

	if (!worldIsTracked_)
	{
		return true;
	}

	ObjectIdSet lostObjects = {worldObjectId_};
	worldIsTracked_ = false;

	isStarted_ = false;

	scopedLock.release();

	postLostTrackerObjects(lostObjects, Timestamp(true));

	return true;
}

bool AKPlaneTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(objectId == worldObjectId_);

	return worldIsTracked_ && objectId == worldObjectId_;
}

void AKPlaneTracker6DOF::setInput(Media::FrameMediumRefs&& frameMediums)
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

void AKPlaneTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, const Timestamp& timestamp, ARFrame* arFrame)
{
	ocean_assert(arFrame != nullptr);

	if (!world_T_camera.isValid())
	{
		onNewSample(HomogenousMatrix4(false), nullptr, timestamp, Metadata());
		return;
	}

	SceneElementPlanes::Planes planes;
	planes.reserve(arFrame.anchors.count);

	for (ARAnchor* anchor in arFrame.anchors)
	{
		if (![anchor isKindOfClass:[ARPlaneAnchor class]])
		{
			continue;
		}

		ARPlaneAnchor* planeAnchor = (ARPlaneAnchor*)(anchor);

		const std::string planeIdentifier = StringApple::toUTF8(planeAnchor.identifier.UUIDString);

		TemporaryScopedLock scopedLock(deviceLock);

			IdentifierMap::iterator iPlane = identifierMap_.find(planeIdentifier);
			if (iPlane == identifierMap_.cend())
			{
				iPlane = identifierMap_.insert(std::make_pair(planeIdentifier, ++planeIdCounter_)).first;
			}
			const Index32 planeId = iPlane->second;

		scopedLock.release();

		SceneElementPlanes::Plane::PlaneType planeType = SceneElementPlanes::Plane::PT_UNKNOWN;
		if (planeAnchor.alignment == ARPlaneAnchorAlignmentHorizontal)
		{
			planeType = SceneElementPlanes::Plane::PT_HORIZONTAL;
		}
		else if (planeAnchor.alignment == ARPlaneAnchorAlignmentVertical)
		{
			planeType = SceneElementPlanes::Plane::PT_VERTICAL;
		}

		// ARKit planes are defined in the x-z plane

		const simd_float4x4 simdTransform = planeAnchor.transform;
		HomogenousMatrixF4 world_T_plane;

		memcpy(world_T_plane.data() +  0, &simdTransform.columns[0], sizeof(float) * 4);
		memcpy(world_T_plane.data() +  4, &simdTransform.columns[1], sizeof(float) * 4);
		memcpy(world_T_plane.data() +  8, &simdTransform.columns[2], sizeof(float) * 4);
		memcpy(world_T_plane.data() + 12, &simdTransform.columns[3], sizeof(float) * 4);

		ocean_assert(NumericF::isWeakEqual(world_T_plane[15], 1.0f));

		world_T_plane[15] = 1.0f; // the plane transformation can be quite inaccurate, re-calculating it to ensure that no asserts get fired
		world_T_plane = HomogenousMatrixF4(world_T_plane.translation(), world_T_plane.rotation());

		const Vector3 center(Scalar(planeAnchor.center[0]), Scalar(planeAnchor.center[1]), Scalar(planeAnchor.center[2]));
		const Vector3 extent(Scalar(planeAnchor.extent[0]), Scalar(planeAnchor.extent[1]), Scalar(planeAnchor.extent[2]));

		const Box3 boundingBoxInPlane(center, extent.x(), extent.y(), extent.z());

		ARPlaneGeometry* geometry = planeAnchor.geometry;

		Vectors3 vertices;
		vertices.reserve(geometry.vertexCount);

		for (size_t n = 0; n < geometry.vertexCount; ++n)
		{
			vertices.emplace_back(Scalar(geometry.vertices[n][0]), Scalar(geometry.vertices[n][1]), Scalar(geometry.vertices[n][2]));
		}

		Vectors2 textureCoordinates;
		textureCoordinates.reserve(geometry.textureCoordinateCount);

		for (size_t n = 0; n < geometry.textureCoordinateCount; ++n)
		{
			textureCoordinates.emplace_back(Scalar(geometry.textureCoordinates[n][0]), Scalar(geometry.textureCoordinates[n][1]));
		}

		Indices32 triangleIndices;
		triangleIndices.reserve(geometry.triangleCount * 3);

		for (size_t n = 0; n < geometry.triangleCount; ++n)
		{
			const Index32 index0 = Index32(geometry.triangleIndices[n * 3u + 0u]);
			const Index32 index1 = Index32(geometry.triangleIndices[n * 3u + 1u]);
			const Index32 index2 = Index32(geometry.triangleIndices[n * 3u + 2u]);

			if (index0 >= vertices.size() || index1 >= vertices.size() || index2 >= vertices.size())
			{
#ifdef OCEAN_DEBUG
				Log::warning() << "<debug> ARKit's vertex index out of range";
#endif
				continue;
			}

			triangleIndices.emplace_back(index0);
			triangleIndices.emplace_back(index1);
			triangleIndices.emplace_back(index2);
		}

		Vectors3 boundaryVertices;
		boundaryVertices.reserve(geometry.boundaryVertexCount);

		for (size_t n = 0; n < geometry.boundaryVertexCount; ++n)
		{
			boundaryVertices.emplace_back(Scalar(geometry.boundaryVertices[n][0]), Scalar(geometry.boundaryVertices[n][1]), Scalar(geometry.boundaryVertices[n][2]));
		}

		planes.emplace_back(planeId, planeType, HomogenousMatrix4(world_T_plane), boundingBoxInPlane, std::move(vertices), std::move(textureCoordinates), std::move(triangleIndices), std::move(boundaryVertices));
	}

	Metadata metadata;
	if (arFrame.lightEstimate != nullptr)
	{
		metadata["ambientColorTemperature"] = Value(arFrame.lightEstimate.ambientColorTemperature);
		metadata["ambientIntensity"] = Value(arFrame.lightEstimate.ambientIntensity);
	}

	if (!planes.empty())
	{
		onNewSample(world_T_camera, std::make_shared<SceneElementPlanes>(std::move(planes)), timestamp, std::move(metadata));
	}
	else
	{
		onNewSample(world_T_camera, nullptr, timestamp, std::move(metadata));
	}
}

void AKPlaneTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, SharedSceneElement&& sceneElement, const Timestamp& timestamp, Metadata&& metadata)
{
	TemporaryScopedLock scopedLock(deviceLock);

	if (world_T_camera.isValid())
	{
		ocean_assert(worldObjectId_ != invalidObjectId());

		ObjectIdSet foundObjectIds;

		ObjectIds sampleObjectIds(1, worldObjectId_);
		Tracker6DOFSample::Positions samplePositions(1, world_T_camera.translation());
		Tracker6DOFSample::Orientations sampleOrientations(1, world_T_camera.rotation());

		if (!worldIsTracked_)
		{
			foundObjectIds = {worldObjectId_};
			worldIsTracked_ = true;
		}

		scopedLock.release();

		SharedSceneElements sceneElements(1, std::move(sceneElement));

		postFoundTrackerObjects(foundObjectIds, timestamp);

		postNewSample(SampleRef(new SceneTracker6DOFSample(timestamp, RS_DEVICE_IN_OBJECT, std::move(sampleObjectIds), std::move(sampleOrientations), std::move(samplePositions), std::move(sceneElements), std::move(metadata))));
	}
	else
	{
		ObjectIdSet lostObjectIds;

		if (worldIsTracked_)
		{
			lostObjectIds = {worldObjectId_};
			worldIsTracked_ = false;
		}

		scopedLock.release();

		postLostTrackerObjects(lostObjectIds, timestamp);
	}
}

}

}

}
