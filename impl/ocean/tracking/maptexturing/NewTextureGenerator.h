/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_NEW_TEXTURE_GENERATOR_H
#define META_OCEAN_TRACKING_MAPTEXTURING_NEW_TEXTURE_GENERATOR_H

#include "ocean/tracking/maptexturing/MapTexturing.h"
#include "ocean/tracking/maptexturing/BlockedMesh.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Thread.h"

#include "ocean/cv/PixelBoundingBox.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Frustum.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Triangle3.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

/**
 *
 * @ingroup trackingmaptexturing
 */
class OCEAN_TRACKING_MAPTEXTURING_EXPORT NewTextureGenerator : protected Thread
{
	public:

		/**
		 * Definition of an unordered map mapping triangle ids to votes.
		 */
		typedef std::unordered_map<Index64, Scalar> TriangleVoteMap;

		class OCEAN_TRACKING_MAPTEXTURING_EXPORT Keyframe
		{
			public:

				/**
				 * Creates a new keyframe object.
				 * @param anyCamera The camera profile of the keyframe, must be valid
				 * @param camera_T_world The camera pose of this keyframe, must be valid
				 * @param frame The color frame of this keyframe, must be valid
				 * @param depth The depth frame of this keyframe
				 */
				Keyframe(SharedAnyCamera anyCamera, const HomogenousMatrix4& camera_T_world, Frame&& frame, Frame&& depth);

				bool updateVotes(const BlockedMeshes& blockedMeshes);

				inline bool needsToBeStored() const;

				inline bool isValid() const;

				static Scalar determineTriangleVote(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& flippedCamera_T_world, const Triangle3& triangle, const Frame& depth);

			public:

				/// The camera profile of this keyframe.
				SharedAnyCamera camera_;

				/// The camera pose of this keyframe.
				HomogenousMatrix4 world_T_camera_;

				/// The color frame of this keyframe.
				Frame frame_;

				/// The depth frame of this keyframe.
				Frame depth_;

				/// The map mapping triangle ids to their corresponding usage votes for this keyframe.
				TriangleVoteMap triangleVoteMap_;

				/// The set holding the ids of all triangles which could be textured with this keyfame.
				UnorderedIndexSet64 acceptableTrianglesSet_;

				bool needsToBeStored_ = true;
		};

		class TexturedRegion
		{
			public:

				explicit TexturedRegion(const size_t meshes) :
					vertices_(meshes),
					textureCoordinates_(meshes),
					islandIds_(meshes)
				{

				}

			public:

				CV::PixelBoundingBoxes islandBoundingBoxes_;

				std::vector<Vectors3> vertices_;

				std::vector<Vectors2> textureCoordinates_;

				std::vector<Indices32> islandIds_;
		};

		/**
		 * This class holds the relevant information for a textured mesh.
		 */
		class TexturedMesh
		{
			public:

				/**
				 * Default constructor.
				 */
				TexturedMesh() = default;

				/**
				 * Creates a new textured mesh object.
				 * @param vertices The vertices of the mesh
				 * @param textureCoordinates The per-vertex texture coordinates, one for each vertex
				 */
				inline TexturedMesh(Vectors3&& vertices, Vectors2&& textureCoordinates);

			public:

				/// The vertices of this mesh.
				Vectors3 vertices_;

				/// The per-vertex texture coordinates of this mesh, one for each vertex.
				Vectors2 textureCoordinates_;

				/// The ids of the keyframes which are associated with this textured mesh.
				UnorderedIndexSet32 usedKeyframeIds_;
		};

		typedef std::vector<TexturedMesh> TexturedMeshes;

		typedef std::unordered_map<VectorI3, TexturedMesh, VectorI3> TexturedMeshMap;

		typedef std::vector<Keyframe> Keyframes;

		typedef std::unordered_map<Index32, Keyframe> KeyframeMap;

		typedef std::unordered_map<Index32, TexturedRegion> TexturedRegionMap;

	protected:

		/**
		 * Definition of individual execution modes.
		 */
		enum ExecutionMode : unsigned int
		{
			/// The generator is currently idling and can process new meshes.
			EM_IDLE,
			/// The generator is currently processing new meshes.
			EM_PROCESSING_MESHES,
			/// The generator has processed new meshes which can be received/accessed.
			EM_MESES_PROCESSED
		};

		typedef std::unordered_map<VectorI3, UnorderedIndexSet32, VectorI3> UsedKeyframeIdsPerMeshMap;

		/**
		 * Definition of an unordered map mapping ids of keyframes to usage counters.
		 */
		typedef std::unordered_map<Index32, Index32> KeyframeIdCounterMap;

		/**
		 * Definition of an unordered set holding block ids.
		 */
		typedef std::unordered_set<VectorI3, VectorI3> BlockCoordinateSet;

	public:

		bool processMeshes(Frustums&& cameraFrustums, Vectors3&& vertices, Rendering::TriangleFaces&& triangleFaces, Keyframes&& newKeyframes);

		/**
		 * Returns whether the generator is ready to process new meshes.
		 * @return True, if so; False, if the generator is still processing meshes
		 */
		inline bool readyToProcessMeshes() const;

		/**
		 * Returns the latest textured meshes.
		 * @param texturedMeshMap The resulting map holding the textured meshes
		 * @param textureFrame The resulting texture associated with the meshes
		 * @return True, if meshes existed; False, if the generator has now new meshes or is still processing the meshes
		 */
		inline bool latestTexturedMeshes(TexturedMeshMap& texturedMeshMap, Frame& textureFrame);

		/**
		 * Returns the current memory usage of this texture generator.
		 * The memory usage is mainly determined by the keyframes the generator stores.
		 * @return The generator's current memory usage, in bytes
		 */
		uint64_t memoryUsage() const;

		/**
		 * Down-samples a given depth frame by taking the minimal depth value of a 4-neighborhood.
		 * @param depthFrame The depth frame to down-sample, must have pixel format FORMAT_F32, must be valid
		 * @param iterations The number of down-sample iterations, with range [1, infinity)
		 * @return The down-sampled depth frame
		 */
		static Frame downsampleDepthFrame(const Frame& depthFrame, const unsigned int iterations = 2u);

	protected:

		void convertToTexture(const TexturedRegionMap& texturedRegionMap, TexturedMeshMap& texturedMeshMap, Frame& textureFrame);

		BlockedMeshes extractVisibleMeshes(const Frustums& cameraFrustums, const Vectors3& vertices, const Rendering::TriangleFaces& triangleFaces) const;

		/**
		 * The thread run function.
		 */
		void threadRun() override;

		static TriangleVoteMap determineBestVotesAcrossKeyframes(const KeyframeMap& keyframeMap);

		static void spreadBestVotesAcrossKeyframes(KeyframeMap& keyframeMap, const TriangleVoteMap& bestVoteMap, const Scalar acceptanceRatio = Scalar(0.2));

		static bool extractRegionsFromKeyframes(KeyframeMap& keyframeMap, const BlockedMeshes& blockMeshes, TexturedRegionMap& texturedRegionMap);

		static bool extractRegionsFromMostVotedKeyframe(UnorderedIndexSet32& candidateKeyframeIds, KeyframeMap& keyframeMap, const BlockedMeshes& blockMeshes, TexturedRegionMap& texturedRegionMap);

		static TexturedRegion extractRegionsFromKeyframe(const Keyframe& keyframe, const BlockedMeshes& blockedMeshes);

	protected:

		/// The current execution mode.
		ExecutionMode executionMode_ = EM_IDLE;

		/// The vertices of the meshes which are currently processed.
		Vectors3 activeVertices_;

		/// The triangle faces of the meshes which are currently processed.
		Rendering::TriangleFaces activeTriangleFaces_;

		/// The frustums of the cameras which have been used to determine vertices which are currently processed.
		Frustums activeCameraFrustums_;

		BlockedMeshes blockedMeshes_;

		/// The counter for unique keyframe ids.
		Index32 keyframeIdCounter_ = 0u;

		/// The map mapping keyframe ids to keyframes.
		KeyframeMap keyframeMap_;

		/// The set holding the ids of all currently existing blocks.
		BlockCoordinateSet blockCoordinateSet_;

		UsedKeyframeIdsPerMeshMap usedKeyframeIdsPerMeshMap_;

		/// The map mapping ids of keyframes to usage counters.
		KeyframeIdCounterMap keyframeIdCounterMap_;

		/// The map holding the latest textured meshes.
		TexturedMeshMap latestTexturedMeshMap_;

		/// The latest texture associated with the latest textured meshes.
		Frame latestTextureFrame_;

		/// The generator's lock.
		mutable Lock lock_;
};

inline bool NewTextureGenerator::Keyframe::needsToBeStored() const
{
	return needsToBeStored_;
}

inline bool NewTextureGenerator::Keyframe::isValid() const
{
	return camera_ != nullptr;
}

inline NewTextureGenerator::TexturedMesh::TexturedMesh(Vectors3&& vertices, Vectors2&& textureCoordinates) :
	vertices_(std::move(vertices)),
	textureCoordinates_(std::move(textureCoordinates))
{
	ocean_assert(vertices_.size() == textureCoordinates_.size());
}

inline bool NewTextureGenerator::readyToProcessMeshes() const
{
	const ScopedLock scopedLock(lock_);

	return executionMode_ == EM_IDLE;
}

inline bool NewTextureGenerator::latestTexturedMeshes(TexturedMeshMap& texturedMeshMap, Frame& textureFrame)
{
	const ScopedLock scopedLock(lock_);

	if (executionMode_ != EM_MESES_PROCESSED)
	{
		return false;
	}

	texturedMeshMap = std::move(latestTexturedMeshMap_);
	textureFrame = std::move(latestTextureFrame_);

	executionMode_ = EM_IDLE;

	return true;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_NEW_TEXTURE_GENERATOR_H
