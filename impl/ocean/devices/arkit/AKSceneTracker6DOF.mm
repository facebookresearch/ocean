/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/AKSceneTracker6DOF.h"

#include "ocean/base/StringApple.h"

#include "ocean/media/avfoundation/AVFoundation.h"

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

AKSceneTracker6DOF::AKSceneTracker6DOF() :
	Device(deviceNameAKSceneTracker6DOF(), deviceTypeAKSceneTracker6DOF()),
	AKDevice(TrackerCapabilities(TC_SLAM | TC_MESH_RECONSTRUCTION), deviceNameAKSceneTracker6DOF(), deviceTypeAKSceneTracker6DOF()),
	Measurement(deviceNameAKSceneTracker6DOF(), deviceTypeAKSceneTracker6DOF()),
	Tracker(deviceNameAKSceneTracker6DOF(), deviceTypeAKSceneTracker6DOF()),
	OrientationTracker3DOF(deviceNameAKSceneTracker6DOF()),
	PositionTracker3DOF(deviceNameAKSceneTracker6DOF()),
	Tracker6DOF(deviceNameAKSceneTracker6DOF()),
	SceneTracker6DOF(deviceNameAKSceneTracker6DOF()),
	VisualTracker(deviceNameAKSceneTracker6DOF(), deviceTypeAKSceneTracker6DOF())
{
	worldObjectId_ = addUniqueObjectId("World");
}

AKSceneTracker6DOF::~AKSceneTracker6DOF()
{
	const ScopedLock scopedLock(deviceLock);

	stop();
}

bool AKSceneTracker6DOF::start()
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
		Log::error() << "AKSceneTracker6DOF needs a valid input medium before it can be started";
	}

	return isStarted_;
}

bool AKSceneTracker6DOF::pause()
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

bool AKSceneTracker6DOF::stop()
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

bool AKSceneTracker6DOF::isObjectTracked(const ObjectId& objectId) const
{
	const ScopedLock scopedLock(deviceLock);

	ocean_assert(objectId == worldObjectId_);

	return worldIsTracked_ && objectId == worldObjectId_;
}

void AKSceneTracker6DOF::setInput(Media::FrameMediumRefs&& frameMediums)
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

void AKSceneTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_rotatedWorld, const Timestamp& timestamp, ARFrame* arFrame)
{
	ocean_assert(arFrame != nullptr);

	if (!world_T_camera.isValid())
	{
		onNewSample(HomogenousMatrix4(false), SharedSceneElements(1, nullptr), timestamp, Metadata());
		return;
	}

	SharedSceneElements sceneElements;
	sceneElements.reserve(2);

	if (arFrame.rawFeaturePoints != nullptr)
	{
		ARPointCloud* pointCloud = arFrame.rawFeaturePoints;

		Vectors3 objectPoints;
		objectPoints.reserve(pointCloud.count);

		if (world_T_rotatedWorld.isIdentity())
		{
			for (size_t n = 0; n < pointCloud.count; ++n)
			{
				const simd_float3& point = pointCloud.points[n];
				objectPoints.emplace_back(Scalar(point[0]), Scalar(point[1]), Scalar(point[2]));
			}
		}
		else
		{
			ocean_assert(world_T_rotatedWorld == HomogenousMatrix4(Quaternion(Vector3(0, 1, 0), -Numeric::pi_2())));
			for (size_t n = 0; n < pointCloud.count; ++n)
			{
				const simd_float3& point = pointCloud.points[n];
				objectPoints.emplace_back(-Scalar(point[2]), Scalar(point[1]), Scalar(point[0])); // manual flipped instead of multiplication world_T_rotatedWorld * worldPoint
			}
		}

		Indices64 objectPointIds;
		objectPointIds.reserve(pointCloud.count);
		for (size_t n = 0; n < pointCloud.count; ++n)
		{
			objectPointIds.emplace_back(Index64(pointCloud.identifiers[n]));
		}

		sceneElements.emplace_back(std::make_shared<AKSceneTracker6DOF::SceneElementObjectPoints>(std::move(objectPoints), std::move(objectPointIds)));
	}

	if (@available(iOS 13.4, *))
	{
		SceneElementMeshes::SharedMeshes meshes;

		for (ARAnchor* anchor in arFrame.anchors)
		{
			if (![anchor isKindOfClass:[ARMeshAnchor class]])
			{
				continue;
			}

			ARMeshAnchor* meshAnchor = (ARMeshAnchor*)(anchor);

			const std::string meshIdentifier = StringApple::toUTF8(meshAnchor.identifier.UUIDString);

			TemporaryScopedLock scopedLock(deviceLock);

				IdentifierMap::iterator iMesh = identifierMap_.find(meshIdentifier);
				if (iMesh == identifierMap_.cend())
				{
					iMesh = identifierMap_.insert(std::make_pair(meshIdentifier, ++meshIdCounter_)).first;
				}
				const Index32 meshId = iMesh->second;

			scopedLock.release();

			const simd_float4x4 simdTransform = meshAnchor.transform;
			HomogenousMatrixF4 world_T_mesh;

			memcpy(world_T_mesh.data() +  0, &simdTransform.columns[0], sizeof(float) * 4);
			memcpy(world_T_mesh.data() +  4, &simdTransform.columns[1], sizeof(float) * 4);
			memcpy(world_T_mesh.data() +  8, &simdTransform.columns[2], sizeof(float) * 4);
			memcpy(world_T_mesh.data() + 12, &simdTransform.columns[3], sizeof(float) * 4);

			ARMeshGeometry* meshGeometry = meshAnchor.geometry;

			Vectors3 vertices;
			Vectors3 perVertexNormals;
			Indices32 triangleIndices;

			if (extractVectors3(meshGeometry.vertices, vertices) && extractVectors3(meshGeometry.normals, perVertexNormals) && extractIndices(meshGeometry.faces, triangleIndices))
			{
#ifdef OCEAN_DEBUG
				ocean_assert(vertices.size() == perVertexNormals.size());
				for (const Index32& index : triangleIndices)
				{
					ocean_assert(index < vertices.size());
				}
#endif

				meshes.emplace_back(std::make_shared<SceneElementMeshes::Mesh>(meshId, HomogenousMatrix4(world_T_mesh), std::move(vertices), std::move(perVertexNormals), std::move(triangleIndices)));
			}
		}

		if (!meshes.empty())
		{
			sceneElements.emplace_back(std::make_shared<SceneElementMeshes>(std::move(meshes)));
		}
	}

	if (sceneElements.empty())
	{
		sceneElements.emplace_back(nullptr); // adding a pure 6-DOF pose scene element
	}

	Metadata metadata;
	if (arFrame.lightEstimate != nullptr)
	{
		metadata["ambientColorTemperature"] = Value(arFrame.lightEstimate.ambientColorTemperature);
		metadata["ambientIntensity"] = Value(arFrame.lightEstimate.ambientIntensity);
	}

	onNewSample(world_T_camera, std::move(sceneElements), timestamp, std::move(metadata));
}

void AKSceneTracker6DOF::onNewSample(const HomogenousMatrix4& world_T_camera, SharedSceneElements&& sceneElements, const Timestamp& timestamp, Metadata&& metadata)
{
	ocean_assert(!sceneElements.empty());

	TemporaryScopedLock scopedLock(deviceLock);

	if (world_T_camera.isValid())
	{
		ocean_assert(worldObjectId_ != invalidObjectId());

		ObjectIdSet foundObjectIds;

		ObjectIds sampleObjectIds(sceneElements.size(), worldObjectId_); // setting all object ids to the same value
		Tracker6DOFSample::Positions samplePositions(sceneElements.size(), world_T_camera.translation());
		Tracker6DOFSample::Orientations sampleOrientations(sceneElements.size(), world_T_camera.rotation());

		if (!worldIsTracked_)
		{
			foundObjectIds = {worldObjectId_};
			worldIsTracked_ = true;
		}

		scopedLock.release();

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

bool AKSceneTracker6DOF::extractVectors3(ARGeometrySource* geometrySource, Vectors3& vectors)
{
	ocean_assert(geometrySource != nullptr);

	if (geometrySource == nullptr)
	{
		return false;
	}

	const size_t components = geometrySource.componentsPerVector;
	ocean_assert(components == 3);

	const MTLVertexFormat format = geometrySource.format;
	ocean_assert(format == MTLVertexFormatFloat3);

	if (components != 3 || format != MTLVertexFormatFloat3)
	{
		return false;
	}

	const size_t offsetBytes = geometrySource.offset;

	const uint8_t* data = (const uint8_t*)(geometrySource.buffer.contents);
	const float* vertexData = (const float*)(data + offsetBytes);

	vectors.clear();
	vectors.reserve(geometrySource.count);

	for (size_t n = 0; n < geometrySource.count; ++n)
	{
		vectors.emplace_back(Scalar(vertexData[3 * n + 0]), Scalar(vertexData[3 * n + 1]), Scalar(vertexData[3 * n + 2]));
	}

	return true;
}

bool AKSceneTracker6DOF::extractIndices(ARGeometryElement* geometryElement, Indices32& indices)
{
	ocean_assert(geometryElement != nullptr);

	if (geometryElement == nullptr)
	{
		return false;
	}

	if (geometryElement.primitiveType == ARGeometryPrimitiveTypeTriangle)
	{
		ocean_assert(geometryElement.bytesPerIndex == 4);
		ocean_assert(geometryElement.indexCountPerPrimitive == 3);

		if (geometryElement.bytesPerIndex != 4 || geometryElement.indexCountPerPrimitive != 3)
		{
			return false;
		}

		const uint8_t* data = (const uint8_t*)(geometryElement.buffer.contents);
		const uint32_t* indexData = (const uint32_t*)(data);

		indices.resize(geometryElement.count * 3);
		memcpy(indices.data(), indexData, sizeof(uint32_t) * indices.size());

		return true;
	}

	return false;
}

}

}

}
