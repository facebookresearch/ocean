// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_TRACKING_MESH_MESH_OBJECT_TRACKER_H
#define META_OCEAN_TRACKING_MESH_MESH_OBJECT_TRACKER_H

#include "ocean/tracking/mesh/Mesh.h"
#include "ocean/tracking/mesh/MeshObjectTrackerCore.h"

#include "ocean/math/PinholeCamera.h"

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

/**
 * This class implements a 6DOF feature tracker for general meshes.
 * The class is mainly a wrapper for the MeshObjectTrackerCore class.
 * @see MeshObjectTrackerCore.
 * @ingroup trackingmesh
 */
class OCEAN_TRACKING_MESH_EXPORT MeshObjectTracker :
	virtual public VisualTracker,
	public MeshObjectTrackerCore
{
	public:

		/**
		 * Creates a new feature tracker object.
		 */
		MeshObjectTracker() = default;

		/**
		 * Destructs a feature tracker object.
		 */
		virtual ~MeshObjectTracker();

		/**
		 * Adds a new 2D tracking mesh object to the tracker.
		 * @param textureFrame The frame specifying the tracking pattern, this frame will be converted internally if the pixel format is not FORMAT_Y8, must be valid
		 * @param meshUVTextureMappingRef The mapping from the provided image into 3D coordinates
		 * @param worker Optional worker object to distribute the computation
		 * @return The id of the registered object for tracking, -1 if the object could not be added
		 */
		virtual unsigned int add(const Frame& textureFrame, const UVTextureMapping::MeshUVTextureMappingRef& meshUVTextureMappingRef, Worker* worker = nullptr);

		/**
		 * Removes a pattern from this tracker.
		 * @param patternId The id of the pattern to be removed
		 * @return True, if succeeded
		 * @see clear().
		 */
		bool remove(const unsigned int patternId);

		/**
		 * Removes all patterns from this tracker.
		 * @return True, if succeeded
		 * @see remove().
		 */
		bool clear();

		/**
		 * Executes the 6DOF tracking for a given frame.
		 * @see VisualTracker::determinePoses().
		 */
		bool determinePoses(const Frames& frames, const SharedAnyCameras& anyCameras, TransformationSamples& transformations, const Quaternion& previousCamera_R_camera = Quaternion(false), Worker* worker = nullptr) override;

	protected:

		/// Intermediate grayscale frame (used to avoid frame buffer re-allocations).
		Frame yTrackerFrame_;
};

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_MESH_MESH_OBJECT_TRACKER_H
