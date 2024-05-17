/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/maptexturing/NewTextureGenerator.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/Frustum.h"
#include "ocean/math/PinholeCamera.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/cv/PixelBoundingBox.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/segmentation/MaskAnalyzer.h"
#include "ocean/cv/Canvas.h"
#include "ocean/cv/segmentation/BinPacking.h"

#include "ocean/cv/FrameShrinker.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

NewTextureGenerator::Keyframe::Keyframe(SharedAnyCamera camera, const HomogenousMatrix4& world_T_camera, Frame&& frame, Frame&& depth) :
	camera_(std::move(camera)),
	world_T_camera_(world_T_camera),
	frame_(std::move(frame)),
	depth_(std::move(depth))
{
	ocean_assert(camera_ && camera_->isValid());
	ocean_assert(world_T_camera_.isValid());
	ocean_assert(frame_.isValid());

	ocean_assert(camera_->width() == frame_.width());
	ocean_assert(camera_->height() == frame_.height());
}

bool NewTextureGenerator::Keyframe::updateVotes(const BlockedMeshes& blockedMeshes)
{
	ocean_assert(!blockedMeshes.empty());

	ocean_assert(camera_ && camera_->isValid());
	ocean_assert(world_T_camera_.isValid());

	if (camera_->name() != AnyCameraPinhole::WrappedCamera::name())
	{
		ocean_assert(false && "Not supported!");
		return false;
	}

	const PinholeCamera& pinholeCamera = ((const AnyCameraPinhole&)(*camera_)).actualCamera();

	const Frustum frustum(world_T_camera_, pinholeCamera, Scalar(0.1), Scalar(3));

	const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera_));

	size_t numberTriangles = 0;
	for (const BlockedMesh& blockedMesh : blockedMeshes)
	{
		numberTriangles += blockedMesh.triangleFaces_.size();
	}

	triangleVoteMap_.clear();
	triangleVoteMap_.reserve(numberTriangles);

	acceptableTrianglesSet_.clear();

	for (Index32 nMesh = 0u; nMesh < Index32(blockedMeshes.size()); ++nMesh)
	{
		const BlockedMesh& blockedMesh = blockedMeshes[nMesh];

		if (frustum.hasIntersection(blockedMesh.boundingBox()))
		{
			for (Index32 nFace = 0u; nFace < Index32(blockedMesh.triangleFaces_.size()); ++nFace)
			{
				const Rendering::TriangleFace& triangleFace = blockedMesh.triangleFaces_[nFace];

				const Triangle3 objectTriangle(blockedMesh.vertices_[triangleFace[0]], blockedMesh.vertices_[triangleFace[1]], blockedMesh.vertices_[triangleFace[2]]);

				if (objectTriangle.isValid())
				{
					const Scalar triangleVote = determineTriangleVote(*camera_, world_T_camera_, flippedCamera_T_world, objectTriangle, depth_);

					if (triangleVote > 0)
					{
						triangleVoteMap_.emplace(BlockedMesh::makeTriangleId(nMesh, nFace), triangleVote);
					}
				}
			}
		}
	}

	return true;
}

Scalar NewTextureGenerator::Keyframe::determineTriangleVote(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& flippedCamera_T_world, const Triangle3& objectTriangle, const Frame& depth)
{
	ocean_assert(anyCamera.isValid() && flippedCamera_T_world.isValid());

	if (PinholeCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectTriangle.point0()) && PinholeCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectTriangle.point1()) && PinholeCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectTriangle.point2()))
	{
		const Vector2 projectedImagePoint0 = anyCamera.projectToImageIF(flippedCamera_T_world, objectTriangle.point0());
		const Vector2 projectedImagePoint1 = anyCamera.projectToImageIF(flippedCamera_T_world, objectTriangle.point1());
		const Vector2 projectedImagePoint2 = anyCamera.projectToImageIF(flippedCamera_T_world, objectTriangle.point2());

		constexpr Scalar border = Scalar(5);

		if (anyCamera.isInside(projectedImagePoint0, border) && anyCamera.isInside(projectedImagePoint1, border) && anyCamera.isInside(projectedImagePoint2, border))
		{
			const Scalar widthFactor = Scalar(depth.width()) / Scalar(anyCamera.width());
			const Scalar heightFactor = Scalar(depth.height()) / Scalar(anyCamera.height());

			const unsigned int xDepth0 = (unsigned int)(minmax(0, Numeric::round32(projectedImagePoint0.x() * widthFactor), int(depth.width() - 1u))); // occlusion handling
			const unsigned int yDepth0 = (unsigned int)(minmax(0, Numeric::round32(projectedImagePoint0.y() * heightFactor), int(depth.height() - 1u)));
			const float depth0 = depth.constpixel<float>(xDepth0, yDepth0)[0];

			if (NumericF::sqr(depth0 + 0.3f) < float(objectTriangle.point0().sqrDistance(world_T_camera.translation())))
			{
				return Scalar(0);
			}

			/*const unsigned int xDepth1 = minmax(0, Numeric::round32(projectedImagePoint1.x() * widthFactor), int(depth.width() - 1u));
			const unsigned int yDepth1 = minmax(0, Numeric::round32(projectedImagePoint1.y() * heightFactor), int(depth.height() - 1u));
			const float depth1 = depth.constpixel<float>(xDepth1, yDepth1)[0];

			if (NumericF::sqr(depth1 + 0.3f) < float(objectTriangle.point1().sqrDistance(world_T_camera.translation())))
			{
				return Scalar(0);
			}

			const unsigned int xDepth2 = minmax(0, Numeric::round32(projectedImagePoint2.x() * widthFactor), int(depth.width() - 1u));
			const unsigned int yDepth2 = minmax(0, Numeric::round32(projectedImagePoint2.y() * heightFactor), int(depth.height() - 1u));
			const float depth2 = depth.constpixel<float>(xDepth2, yDepth2)[0];

			if (NumericF::sqr(depth2 + 0.3f) < float(objectTriangle.point2().sqrDistance(world_T_camera.translation())))
			{
				return Scalar(0);
			}*/

			const Triangle2 projectedTriangle(projectedImagePoint0, projectedImagePoint1, projectedImagePoint2);

			if (projectedTriangle.isCounterClockwise(true))
			{
				return projectedTriangle.area2(); // !!!square area to improve perpendicularity???
			}
		}
	}

	return Scalar(0);
}

bool NewTextureGenerator::processMeshes(Frustums &&cameraFrustums, Vectors3&& vertices, Rendering::TriangleFaces&& triangleFaces, Keyframes&& newKeyframes)
{
	if (cameraFrustums.empty() || vertices.empty() || triangleFaces.empty())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (executionMode_ != EM_IDLE)
	{
		return false;
	}

	for (Keyframe& newKeyframe : newKeyframes)
	{
		ocean_assert(newKeyframe.isValid());

		const Index32 newKeyframeId = ++keyframeIdCounter_;
		ocean_assert(keyframeMap_.find(newKeyframeId) == keyframeMap_.cend());

		keyframeMap_.emplace(newKeyframeId, std::move(newKeyframe));
	}

#ifdef OCEAN_DEBUG
	for (KeyframeMap::const_iterator i = keyframeMap_.cbegin(); i != keyframeMap_.cend(); ++i)
	{
		ocean_assert(i->second.isValid());
	}
#endif

	activeCameraFrustums_ = std::move(cameraFrustums);
	activeVertices_ = std::move(vertices);
	activeTriangleFaces_ = std::move(triangleFaces);

	executionMode_ = EM_PROCESSING_MESHES;

	if (!isThreadActive())
	{
		startThread();
	}

	newKeyframes.clear();

	return true;
}

uint64_t NewTextureGenerator::memoryUsage() const
{
	const ScopedLock scopedLock(lock_);

	uint64_t result = 0;

	for (KeyframeMap::const_iterator iKeyframe = keyframeMap_.cbegin(); iKeyframe != keyframeMap_.cend(); ++iKeyframe)
	{
		const Keyframe& keyframe = iKeyframe->second;

		constexpr unsigned int planeIndex = 0u;

		ocean_assert(keyframe.frame_.numberPlanes() == 1u);
		result += uint64_t(keyframe.frame_.size(planeIndex));

		ocean_assert(keyframe.depth_.numberPlanes() == 1u);
		result += uint64_t(keyframe.depth_.size(planeIndex));
	}

	return result;
}


NewTextureGenerator::TriangleVoteMap NewTextureGenerator::determineBestVotesAcrossKeyframes(const KeyframeMap& keyframeMap)
{
	size_t expectedSize = 0;
	for (KeyframeMap::const_iterator iMap = keyframeMap.cbegin(); iMap != keyframeMap.cend(); ++iMap)
	{
		expectedSize = std::max(expectedSize, iMap->second.triangleVoteMap_.size());
	}

	TriangleVoteMap bestTriangleVoteMap;
	bestTriangleVoteMap.reserve(expectedSize);

	for (KeyframeMap::const_iterator iMap = keyframeMap.cbegin(); iMap != keyframeMap.cend(); ++iMap)
	{
		const TriangleVoteMap& triangleVoteMap = iMap->second.triangleVoteMap_;

		for (TriangleVoteMap::const_iterator iVote = triangleVoteMap.cbegin(); iVote != triangleVoteMap.cend(); ++iVote)
		{
			Scalar& bestVote = bestTriangleVoteMap[iVote->first];

			if (iVote->second > bestVote)
			{
				bestVote = iVote->second;
			}
		}
	}

	return bestTriangleVoteMap;
}

void NewTextureGenerator::spreadBestVotesAcrossKeyframes(KeyframeMap& keyframeMap, const TriangleVoteMap& bestVoteMap, const Scalar acceptanceRatio)
{
	ocean_assert(acceptanceRatio > 0 && acceptanceRatio <= 1);

	for (KeyframeMap::iterator iMap = keyframeMap.begin(); iMap != keyframeMap.end(); ++iMap)
	{
		const TriangleVoteMap& triangleVoteMap = iMap->second.triangleVoteMap_;

		UnorderedIndexSet64& acceptableTrianglesSet = iMap->second.acceptableTrianglesSet_;
		ocean_assert(acceptableTrianglesSet.empty());

		acceptableTrianglesSet.reserve(triangleVoteMap.size() * 50 / 100);

		for (TriangleVoteMap::const_iterator iVote = triangleVoteMap.cbegin(); iVote != triangleVoteMap.cend(); ++iVote)
		{
			const Index64& triangleId = iVote->first;

			ocean_assert(bestVoteMap.find(triangleId) != bestVoteMap.cend());

			const TriangleVoteMap::const_iterator iBestVote = bestVoteMap.find(triangleId);
			ocean_assert(iBestVote != bestVoteMap.cend());

			const Scalar& bestVote = iBestVote->second;

			if (iVote->second >= bestVote * acceptanceRatio)
			{
				acceptableTrianglesSet.emplace(triangleId);
			}
		}
	}
}

bool NewTextureGenerator::extractRegionsFromKeyframes(KeyframeMap& keyframeMap, const BlockedMeshes& blockedMeshes, TexturedRegionMap& texturedRegionMap)
{
	//static HighPerformanceStatistic p0, p1, p2;

	//p0.start();
		const TriangleVoteMap bestVoteMap(determineBestVotesAcrossKeyframes(keyframeMap));
	//p0.stop();

	//p1.start();
		spreadBestVotesAcrossKeyframes(keyframeMap, bestVoteMap);
	//p1.stop();

	//p2.start();

	UnorderedIndexSet32 candidateKeyframeIds;
	candidateKeyframeIds.reserve(keyframeMap.size());

	for (KeyframeMap::const_iterator iKeyframe = keyframeMap.cbegin(); iKeyframe != keyframeMap.cend(); ++iKeyframe)
	{
		if (!iKeyframe->second.acceptableTrianglesSet_.empty())
		{
			candidateKeyframeIds.emplace(iKeyframe->first);
		}
	}

	texturedRegionMap.clear();
	texturedRegionMap.reserve(keyframeMap.size());

	while (!candidateKeyframeIds.empty())
	{
		if (!extractRegionsFromMostVotedKeyframe(candidateKeyframeIds, keyframeMap, blockedMeshes, texturedRegionMap))
		{
			break;
		}
	}

	//p2.stop();
	//Log::info() << "Regions: " << texturedRegionMap.size();
	//Log::info() << p0.lastMseconds() << ",  " << p1.lastMseconds() << ",  " << p2.lastMseconds();

	return true;
}

bool NewTextureGenerator::extractRegionsFromMostVotedKeyframe(UnorderedIndexSet32& candidateKeyframeIds, KeyframeMap& keyframeMap, const BlockedMeshes& blockedMeshes, TexturedRegionMap& texturedRegionMap)
{
	ocean_assert(!candidateKeyframeIds.empty());

	Index32 bestKeyframeId = Index32(-1);
	size_t bestKeyframeSize = 0;

	for (const Index32& candidateKeyframeId : candidateKeyframeIds)
	{
		const KeyframeMap::const_iterator iKeyframe = keyframeMap.find(candidateKeyframeId);
		ocean_assert(iKeyframe != keyframeMap.cend());

		const Keyframe& keyframe = iKeyframe->second;

		ocean_assert(!keyframe.acceptableTrianglesSet_.empty());

		if (keyframe.acceptableTrianglesSet_.size() > bestKeyframeSize)
		{
			bestKeyframeId = candidateKeyframeId;
			bestKeyframeSize = keyframe.acceptableTrianglesSet_.size();
		}
	}

	if (bestKeyframeId == Index32(-1))
	{
		return false;
	}

	ocean_assert(keyframeMap.find(bestKeyframeId) != keyframeMap.cend());
	Keyframe& bestKeyframe = keyframeMap.find(bestKeyframeId)->second;

	bestKeyframe.needsToBeStored_ = bestKeyframeSize > 10; // keyframes with less than 10 triangles do not get stored in the database

	TexturedRegion texturedRegion = extractRegionsFromKeyframe(bestKeyframe, blockedMeshes);

	// now we remove the used/textured triangles from all other keyframes

	for (const Index32& notUsedKeyframeId : candidateKeyframeIds)
	{
		if (notUsedKeyframeId != bestKeyframeId)
		{
			ocean_assert(keyframeMap.find(notUsedKeyframeId) != keyframeMap.cend());
			Keyframe& notUsedKeyframe = keyframeMap.find(notUsedKeyframeId)->second;

			for (const Index64& triangleId : bestKeyframe.acceptableTrianglesSet_)
			{
				notUsedKeyframe.acceptableTrianglesSet_.erase(triangleId);
			}
		}
	}

	bestKeyframe.acceptableTrianglesSet_.clear();

	// we update the set of candidate keyframes

	ocean_assert(texturedRegionMap.find(bestKeyframeId) == texturedRegionMap.cend());
	texturedRegionMap.emplace(bestKeyframeId, std::move(texturedRegion));

	UnorderedIndexSet32 newCandidateKeyframeIds;
	newCandidateKeyframeIds.reserve(candidateKeyframeIds.size());

	for (const Index32& oldCandidateKeyframeId : candidateKeyframeIds)
	{
		const KeyframeMap::const_iterator iKeyframe = keyframeMap.find(oldCandidateKeyframeId);
		ocean_assert(iKeyframe != keyframeMap.cend());

		if (!iKeyframe->second.acceptableTrianglesSet_.empty())
		{
			newCandidateKeyframeIds.emplace(oldCandidateKeyframeId);
		}
	}

	candidateKeyframeIds = std::move(newCandidateKeyframeIds);

	return true;
}

NewTextureGenerator::TexturedRegion NewTextureGenerator::extractRegionsFromKeyframe(const Keyframe& keyframe, const BlockedMeshes& blockedMeshes)
{
	const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(keyframe.world_T_camera_));

	Vector2 imagePoints[3];
	Vector3 objectPoints[3];

	TexturedRegion texturedRegion(blockedMeshes.size());

	const unsigned int width = keyframe.frame_.width();
	const unsigned int height = keyframe.frame_.height();

	const unsigned int width1 = width - 1u;
	const unsigned int height1 = height - 1u;

	const unsigned int width_4 = width / 4u;
	const unsigned int height_4 = height / 4u;

	Frame maskFrame(FrameType(width_4, height_4, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	maskFrame.setValue(0x00u);

	uint8_t* const maskData = maskFrame.data<uint8_t>();

	const UnorderedIndexSet64& triangleIds = keyframe.acceptableTrianglesSet_;
	ocean_assert(!triangleIds.empty());

	CV::PixelBoundingBoxes downsampledTriangleBoundingBoxes;
	downsampledTriangleBoundingBoxes.reserve(triangleIds.size());

	for (const Index64& triangleId : triangleIds)
	{
		Index32 meshId;
		Index32 triangleIndex;
		BlockedMesh::separateTriangleId(triangleId, meshId, triangleIndex);

		const BlockedMesh& blockedMesh = blockedMeshes[meshId];

		const Rendering::TriangleFace& triangleFace = blockedMesh.triangleFaces_[triangleIndex];

		objectPoints[0] = blockedMesh.vertices_[triangleFace[0]];
		objectPoints[1] = blockedMesh.vertices_[triangleFace[1]];
		objectPoints[2] = blockedMesh.vertices_[triangleFace[2]];

		keyframe.camera_->projectToImageIF(flippedCamera_T_world, objectPoints, 3, imagePoints);
		ocean_assert(keyframe.camera_->isInside(imagePoints[0]));
		ocean_assert(keyframe.camera_->isInside(imagePoints[1]));
		ocean_assert(keyframe.camera_->isInside(imagePoints[2]));

		const Scalar left = std::min(imagePoints[0].x(), std::min(imagePoints[1].x(), imagePoints[2].x()));
		const Scalar right = std::max(imagePoints[0].x(), std::max(imagePoints[1].x(), imagePoints[2].x()));

		const Scalar top = std::min(imagePoints[0].y(), std::min(imagePoints[1].y(), imagePoints[2].y()));
		const Scalar bottom = std::max(imagePoints[0].y(), std::max(imagePoints[1].y(), imagePoints[2].y()));

		const unsigned int intLeft = (unsigned int)(std::max(0, int(left) - 1)) / 4u;
		const unsigned int intTop = (unsigned int)(std::max(0, int(top) - 1)) / 4u;

		const unsigned int intRight = std::min((unsigned int)(right + Scalar(0.5)) + 1u, width1) / 4u;
		const unsigned int intBottom = std::min((unsigned int)(bottom + Scalar(0.5)) + 1u, height1) / 4u;

		downsampledTriangleBoundingBoxes.emplace_back(intLeft, intTop, intRight, intBottom);

		for (unsigned int y = intTop; y <= intBottom; ++y)
		{
			memset(maskData + y * width_4 + intLeft, 0xFFu, intRight - intLeft + 1u);
		}

		ocean_assert(meshId < texturedRegion.vertices_.size());
		Vectors3& vertices = texturedRegion.vertices_[meshId];

		ocean_assert(meshId < texturedRegion.textureCoordinates_.size());
		Vectors2& textureCoordinates = texturedRegion.textureCoordinates_[meshId];

		vertices.emplace_back(objectPoints[0]);
		vertices.emplace_back(objectPoints[1]);
		vertices.emplace_back(objectPoints[2]);

		textureCoordinates.emplace_back(imagePoints[0]);
		textureCoordinates.emplace_back(imagePoints[1]);
		textureCoordinates.emplace_back(imagePoints[2]);
	}

	CV::PixelBoundingBoxes& boundingBoxes = texturedRegion.islandBoundingBoxes_;

	boundingBoxes = CV::Segmentation::MaskAnalyzer::detectBoundingBoxes(maskFrame.constdata<uint8_t>(), maskFrame.width(), maskFrame.height(), maskFrame.paddingElements(), 0xFFu, false);
	ocean_assert(boundingBoxes.size() < 255 && "Not yet supported");

	ocean_assert(downsampledTriangleBoundingBoxes.size() == triangleIds.size());

	UnorderedIndexSet64::const_iterator iTriangleId = triangleIds.cbegin();

	for (size_t n = 0; n < triangleIds.size(); ++n)
	{
		const Index64 triangleId = *iTriangleId++;
		const CV::PixelBoundingBox& triangleBoundingBox = downsampledTriangleBoundingBoxes[n];

		ocean_assert(triangleBoundingBox.right() < width_4 && triangleBoundingBox.bottom() < height_4);

		Index32 meshId;
		Index32 triangleIndex;
		BlockedMesh::separateTriangleId(triangleId, meshId, triangleIndex);

		Indices32& islandIds = texturedRegion.islandIds_[meshId];

		Index32 foundIslandId = Index32(-1);

		for (Index32 islandId = 0; islandId < Index32(boundingBoxes.size()); ++islandId)
		{
			if (boundingBoxes[islandId].isInside(triangleBoundingBox)) // check if entirely inside
			{
				foundIslandId = islandId;
				break;
			}
		}

		ocean_assert(foundIslandId != Index32(-1));

		islandIds.emplace_back(foundIslandId);
	}

	for (size_t nBox = 0; nBox < boundingBoxes.size(); ++nBox)
	{
		CV::PixelBoundingBox& boundingBox = boundingBoxes[nBox];

		boundingBox = CV::PixelBoundingBox(CV::PixelPosition(boundingBox.left() * 4u, boundingBox.top() * 4u), boundingBox.width() * 4u, boundingBox.height() * 4u);
		boundingBox = boundingBox.extended(16u, 0u, 0u, width1, height1);
	}

#ifdef OCEAN_DEBUG
	for (size_t nMesh = 0; nMesh < texturedRegion.vertices_.size(); ++nMesh)
	{
		const Vectors2& textureCoordinates = texturedRegion.textureCoordinates_[nMesh];
		const Indices32& islandIds = texturedRegion.islandIds_[nMesh];

		ocean_assert(islandIds.size() * 3 == textureCoordinates.size());

		for (size_t n = 0; n < islandIds.size(); ++n)
		{
			const Index32 islandId = islandIds[n];

			ocean_assert(islandId < boundingBoxes.size());
			const CV::PixelBoundingBox& islandBox = boundingBoxes[islandId];

			for (unsigned int i = 0u; i < 3u; ++i)
			{
				const Vector2& textureCoordinate = textureCoordinates[n * 3u + i];

				ocean_assert(textureCoordinate.x() >= Scalar(islandBox.left()));
				ocean_assert(textureCoordinate.y() >= Scalar(islandBox.top()));
				ocean_assert(textureCoordinate.x() < Scalar(islandBox.rightEnd()));
				ocean_assert(textureCoordinate.y() < Scalar(islandBox.bottomEnd()));
			}
		}
	}
#endif

	return texturedRegion;
}

void NewTextureGenerator::convertToTexture(const TexturedRegionMap& texturedRegionMap, TexturedMeshMap& texturedMeshMap, Frame& textureFrame)
{
	Frames subFrames;
	subFrames.reserve(texturedRegionMap.size());

	CV::PixelBoundingBoxes islandBoxes;
	islandBoxes.reserve(texturedRegionMap.size() * 4);

	std::vector<Indices32> regionIslandIndices2globalIndices;

	Indices32 usedKeyframeIds;
	usedKeyframeIds.reserve(texturedRegionMap.size());

	TexturedMeshes texturedMeshes(blockedMeshes_.size());

	for (TexturedRegionMap::const_iterator iMap = texturedRegionMap.cbegin(); iMap != texturedRegionMap.cend(); ++iMap)
	{
		const TexturedRegion& region = iMap->second;

		if (!region.islandBoundingBoxes_.empty())
		{
			usedKeyframeIds.emplace_back(iMap->first);

			const TexturedRegion& texturedRegion = iMap->second;

			for (size_t nMesh = 0; nMesh < texturedRegion.vertices_.size(); ++nMesh)
			{
				const Vectors3& sourceVertices = texturedRegion.vertices_[nMesh];

				if (sourceVertices.empty())
				{
					continue;
				}

				ocean_assert(nMesh < texturedMeshes.size());
				TexturedMesh& texturedMesh = texturedMeshes[nMesh];

				texturedMesh.usedKeyframeIds_.emplace(iMap->first);
			}
		}

		const KeyframeMap::const_iterator iKeyframe = keyframeMap_.find(iMap->first);

		ocean_assert(iKeyframe != keyframeMap_.cend());
		if (iKeyframe == keyframeMap_.cend())
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		const Keyframe& keyframe = iKeyframe->second;

		Indices32 globalMeshIndices;
		globalMeshIndices.reserve(region.islandBoundingBoxes_.size());

		for (const CV::PixelBoundingBox& islandBox : region.islandBoundingBoxes_)
		{
			unsigned int left = islandBox.left();
			unsigned int top = islandBox.top();

			unsigned int width = islandBox.width();
			unsigned int height = islandBox.height();

			// let's ensure that the corner coordinates are a multiple of 2 (due to pixel formats like e.g., Y_UV12)

			if (left % 2u != 0u)
			{
				ocean_assert(left >= 1u);

				--left;
				width += 1u;
			}

			if (top % 2u != 0u)
			{
				ocean_assert(top >= 1u);

				--top;
				height += 1u;
			}

			if (width % 2u != 0u)
			{
				width += 1u;
			}

			if (height % 2u != 0u)
			{
				height += 1u;
			}

			globalMeshIndices.emplace_back(Index32(islandBoxes.size()));

			islandBoxes.emplace_back(CV::PixelPosition((unsigned int)(left), (unsigned int)(top)), width, height);

			subFrames.emplace_back(keyframe.frame_.subFrame((unsigned int)(left), (unsigned int)(top), width, height, Frame::CM_USE_KEEP_LAYOUT));
		}

		regionIslandIndices2globalIndices.emplace_back(std::move(globalMeshIndices));
	}

	ocean_assert(!islandBoxes.empty());
	if (islandBoxes.empty())
	{
		ocean_assert(false && "This should never happen");
		return;
	}

	unsigned int textureWidth = 0u;
	unsigned int textureHeight = 0u;
	const CV::Segmentation::BinPacking::Packings packings = CV::Segmentation::BinPacking::binPacking(islandBoxes, false, &textureWidth, &textureHeight);

	if (!textureFrame.set(FrameType(textureWidth, textureHeight, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), true, true))
	{
		ocean_assert(false && "This should never happen");
		return;
	}

	textureFrame.setValue(0x00u);

	CV::PixelPositionsI islandOffsets(islandBoxes.size());

	for (const CV::Segmentation::BinPacking::Packing& packing : packings)
	{
		const Index32& islandIndex = packing.boxIndex();

		CV::PixelBoundingBox& islandBox = islandBoxes[islandIndex];
		const CV::PixelPosition& islandTopLeft = packing.topLeft();

		islandOffsets[islandIndex] = CV::PixelPositionI(int(islandTopLeft.x()) - int(islandBox.left()), int(islandTopLeft.y()) - int(islandBox.top()));

		const Frame& subFrame = subFrames[islandIndex];

		Frame textureAreaFrame = textureFrame.subFrame(islandTopLeft.x(), islandTopLeft.y(), subFrame.width(), subFrame.height(), Frame::CM_USE_KEEP_LAYOUT);

		if (!CV::FrameConverter::Comfort::convertAndCopy(subFrame, textureAreaFrame, nullptr, CV::FrameConverter::Options(true /*allowApproximation*/)))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}
	}

	const Scalar invTextureWidth = Scalar(1) / Scalar(textureWidth);
	const Scalar invTextureHeight = Scalar(1) / Scalar(textureHeight);



	size_t regionCounter = 0;

	for (TexturedRegionMap::const_iterator iMap = texturedRegionMap.cbegin(); iMap != texturedRegionMap.cend(); ++iMap)
	{
		ocean_assert(regionCounter < regionIslandIndices2globalIndices.size());
		const Indices32& globalIndices = regionIslandIndices2globalIndices[regionCounter];

		++regionCounter;

		const TexturedRegion& texturedRegion = iMap->second;

		for (size_t nMesh = 0; nMesh < texturedRegion.vertices_.size(); ++nMesh)
		{
			const Indices32& islandIndices = texturedRegion.islandIds_[nMesh];
			const Vectors3& sourceVertices = texturedRegion.vertices_[nMesh];

			if (sourceVertices.empty())
			{
				continue;
			}

			ocean_assert(islandIndices.size() == sourceVertices.size() / 3);

			const Vectors2& sourceTextureCoordinates = texturedRegion.textureCoordinates_[nMesh];

			TexturedMesh& texturedMesh = texturedMeshes[nMesh];

			texturedMesh.vertices_.insert(texturedMesh.vertices_.cend(), sourceVertices.cbegin(), sourceVertices.cend());

			for (size_t n = 0; n < sourceTextureCoordinates.size(); ++n)
			{
				const Index32 localIslandIndex = islandIndices[n / 3];
				const Vector2& textureCoordinate = sourceTextureCoordinates[n];

				if (localIslandIndex == Index32(-1))
				{
					ocean_assert(false && "Must never happen, but happens rarely");
					// **TDOO** HACK
					texturedMesh.textureCoordinates_.emplace_back(Scalar(0.5), Scalar(0.5));
					continue;
				}

				ocean_assert(localIslandIndex < globalIndices.size());
				const Index32 globalIslandIndex = globalIndices[localIslandIndex];

#ifdef OCEAN_DEBUG
				const CV::PixelBoundingBox& islandBox = islandBoxes[globalIslandIndex];
				ocean_assert(textureCoordinate.x() >= Scalar(islandBox.left()));
				ocean_assert(textureCoordinate.y() >= Scalar(islandBox.top()));
				ocean_assert(textureCoordinate.x() < Scalar(islandBox.rightEnd()));
				ocean_assert(textureCoordinate.y() < Scalar(islandBox.bottomEnd()));
#endif

				ocean_assert(globalIslandIndex < islandBoxes.size());
				const CV::PixelPositionI& islandOffset = islandOffsets[globalIslandIndex];

				const Scalar x = (textureCoordinate.x() + Scalar(islandOffset.x())) * invTextureWidth;
				const Scalar y = (textureCoordinate.y() + Scalar(islandOffset.y())) * invTextureHeight;

				ocean_assert(x >= 0 && x <= 1);
				ocean_assert(y >= 0 && y <= 1);

				texturedMesh.textureCoordinates_.emplace_back(x, Scalar(1) - y);
			}
		}
	}

	texturedMeshMap.clear();

	for (size_t nMesh = 0; nMesh < texturedMeshes.size(); ++nMesh)
	{
		TexturedMesh& texturedMesh = texturedMeshes[nMesh];

		if (!texturedMesh.vertices_.empty())
		{
			const BlockedMesh& blockedMesh = blockedMeshes_[nMesh];

			ocean_assert(texturedMeshMap.find(blockedMesh.block()) == texturedMeshMap.cend());

			texturedMeshMap.emplace(blockedMesh.block(), std::move(texturedMesh));
		}
	}
}

void NewTextureGenerator::threadRun()
{
	ocean_assert(executionMode_ == EM_PROCESSING_MESHES);

	HighPerformanceStatistic performanceUpdate;
	HighPerformanceStatistic performanceTexturing;
	HighPerformanceStatistic performanceActual;

	while (!shouldThreadStop())
	{
		TemporaryScopedLock scopedLock(lock_);
			if (executionMode_ != EM_PROCESSING_MESHES)
			{
				scopedLock.release();

				sleep(1u);
				continue;
			}
		scopedLock.release();

		ocean_assert(!activeCameraFrustums_.empty() && !activeVertices_.empty() && !activeTriangleFaces_.empty());

		blockedMeshes_ = extractVisibleMeshes(activeCameraFrustums_, activeVertices_, activeTriangleFaces_);

		ocean_assert(!keyframeMap_.empty() && !blockedMeshes_.empty());

		// first, we determine the votes for all mesh triangles across all keyframes

		performanceUpdate.start();
			for (KeyframeMap::iterator iMap = keyframeMap_.begin(); iMap != keyframeMap_.end(); ++iMap)
			{
				iMap->second.updateVotes(blockedMeshes_);
			}
		performanceUpdate.stop();

		performanceTexturing.start();
			TexturedRegionMap texturedRegionMap;
			extractRegionsFromKeyframes(keyframeMap_, blockedMeshes_, texturedRegionMap);
		performanceTexturing.stop();

		if (texturedRegionMap.empty())
		{
			scopedLock.relock(lock_);
			executionMode_ = EM_IDLE;

			continue;
		}

		performanceActual.start();
			TexturedMeshMap texturedMeshMap;
			Frame textureFrame;
			convertToTexture(texturedRegionMap, texturedMeshMap, textureFrame);
		performanceActual.stop();

		UnorderedIndexSet32 usedKeyframeIds;

		for (TexturedMeshMap::const_iterator iTexturedMesh = texturedMeshMap.cbegin(); iTexturedMesh != texturedMeshMap.end(); ++iTexturedMesh)
		{
			usedKeyframeIds.reserve(iTexturedMesh->second.usedKeyframeIds_.size());
			usedKeyframeIds.clear();

			for (const Index32& usedKeyframeId : iTexturedMesh->second.usedKeyframeIds_)
			{
				ocean_assert(keyframeMap_.find(usedKeyframeId) != keyframeMap_.cend());

				if (keyframeMap_.find(usedKeyframeId)->second.needsToBeStored())
				{
					usedKeyframeIds.emplace(usedKeyframeId);
				}
			}

			// we increase the counter for the current ids

			for (const Index32& id : usedKeyframeIds)
			{
				++keyframeIdCounterMap_[id];
			}

			// we decease the counter for the old ids

			UnorderedIndexSet32& oldUsedKeyframeIds = usedKeyframeIdsPerMeshMap_[iTexturedMesh->first];

			for (const Index32& id : oldUsedKeyframeIds)
			{
				auto i = keyframeIdCounterMap_.find(id);
				ocean_assert(i != keyframeIdCounterMap_.end());

				ocean_assert(i->second >= 1u);
				--i->second;

				if (i->second == 0u)
				{
					keyframeIdCounterMap_.erase(i);
				}
			}

			oldUsedKeyframeIds = usedKeyframeIds;
		}

		// remove all keyframes not used
		for (KeyframeMap::iterator iKeyframe = keyframeMap_.begin(); iKeyframe != keyframeMap_.end(); /*noop*/)
		{
			if (keyframeIdCounterMap_.find(iKeyframe->first) == keyframeIdCounterMap_.cend())
			{
				iKeyframe = keyframeMap_.erase(iKeyframe);
			}
			else
			{
				++iKeyframe;
			}
		}

		if (performanceUpdate.measurements() % 20u == 0u)
		{
			Log::info() << " ";
			Log::info() << "performanceUpdate: " << performanceUpdate.medianMseconds() << "ms, " << performanceUpdate.percentileMseconds(0.95) << "ms, " << performanceUpdate.lastMseconds() << "ms";
			Log::info() << "performanceTexturing: " << performanceTexturing.medianMseconds() << "ms, " << performanceTexturing.percentileMseconds(0.95) << "ms, " <<  performanceTexturing.lastMseconds() << "ms";
			Log::info() << "performanceActual: " << performanceActual.medianMseconds() << "ms, " << performanceActual.percentileMseconds(0.95) << "ms, " <<  performanceActual.lastMseconds() << "ms";

			if (!keyframeMap_.empty())
			{
				Log::info() << "Current keyframes: " << keyframeMap_.size() << ", total size: " << keyframeMap_.size() * keyframeMap_.begin()->second.frame_.size() / (1024 * 1024) << "MB";
			}

			Log::info() << " ";
		}

		scopedLock.relock(lock_);

		for (TexturedMeshMap::const_iterator iMesh = texturedMeshMap.begin(); iMesh != texturedMeshMap.cend(); ++iMesh)
		{
			blockCoordinateSet_.emplace(iMesh->first);
		}

		latestTexturedMeshMap_ = std::move(texturedMeshMap);
		latestTextureFrame_ = std::move(textureFrame);

		executionMode_ = EM_MESES_PROCESSED;
	}
}

BlockedMeshes NewTextureGenerator::extractVisibleMeshes(const Frustums& cameraFrustums, const Vectors3& vertices, const Rendering::TriangleFaces& triangleFaces) const
{
	BlockedMeshes blockedMeshes = BlockedMesh::separateMesh(vertices, triangleFaces);

	for (size_t nMesh = 0; nMesh < blockedMeshes.size(); /*noop*/)
	{
		BlockedMesh& blockedMesh = blockedMeshes[nMesh];

		if (blockCoordinateSet_.find(blockedMesh.block()) == blockCoordinateSet_.cend())
		{
			// we have not yet seen this block

			++nMesh;
		}
		else
		{
			bool meshIsVisible = false;

			for (const Frustum& cameraFrustum : cameraFrustums)
			{
				if (cameraFrustum.hasIntersection(blockedMesh.boundingSphere()) && cameraFrustum.hasIntersection(blockedMesh.boundingBox()))
				{
					meshIsVisible = true;
					break;
				}
			}

			if (meshIsVisible)
			{
				++nMesh;
			}
			else
			{
				blockedMesh = std::move(blockedMeshes.back());
				blockedMeshes.pop_back();
			}
		}
	}

	return blockedMeshes;
}

Frame NewTextureGenerator::downsampleDepthFrame(const Frame& depthFrame, const unsigned int iterations)
{
	ocean_assert(depthFrame.isValid() && depthFrame.isPixelFormatCompatible(FrameType::FORMAT_F32));
	ocean_assert(iterations >= 1u);

	Frame tmpFrame(depthFrame, Frame::ACM_USE_KEEP_LAYOUT);

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		const unsigned int targetWidth = tmpFrame.width() / 2u;
		const unsigned int targetHeight = tmpFrame.height() / 2u;

		if (targetWidth % 2u != 0u || targetHeight % 2u != 0u)
		{
			break;
		}

		Frame downsampled(FrameType(tmpFrame, targetWidth, targetHeight));

		for (unsigned int y = 0u; y < targetHeight; ++y)
		{
			const float* const source0 = tmpFrame.constrow<float>(y * 2u + 0u);
			const float* const source1 = tmpFrame.constrow<float>(y * 2u + 1u);

			float* const target = downsampled.row<float>(y);

			for (unsigned int x = 0u; x < targetWidth; ++x)
			{
				target[x] = std::min(std::min(source0[x * 2u + 0u], source0[x * 2u + 1u]), std::min(source1[x * 2u + 0u], source1[x * 2u + 1u]));
			}
		}

		tmpFrame = std::move(downsampled);
	}

	return tmpFrame;
}

}

}

}
