// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_TRACKING_MESH_MESH_H
#define META_OCEAN_TRACKING_MESH_MESH_H

#include "ocean/tracking/Tracker.h"

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

/**
 * @ingroup tracking
 * @defgroup trackingmesh Ocean Textured Mesh Object Tracking Interface
 * @{
 * The Ocean Mesh Tracker Library allows for 6DOF object tracking for triangulated, textured surfaces.
 * You can use the MeshTracker6DOF class which is derived from VisualTracker, or you can use the actual implementation (MeshTrackerCore6DOF) directly if you want to avoid the object-oriented implementation.
 *
 * The tracker provides camera poses for individual mesh objects.<br>
 * Each mesh is connected with an id allowing to distinguish between the individual meshes.<br>
 * For each camera frame, the tracker determines pairs of mesh ids and camera poses (for all visible meshes).<br>
 * Each resulting camera pose transforms points defined in the camera coordinate system to points defined in the coordinate system of the corresponding mesh object.
 *
 * @see MeshTracker6DOF, MeshTrackerCore6DOF.
 * @}
 */

/**
 * @namespace Ocean::Tracking::Mesh Namespace of the Mesh Tracker library.<p>
 * The Namespace Ocean::Tracking::Mesh is used in the entire Ocean Mesh Tracker Library.
 */

// Defines OCEAN_TRACKING_MESH_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TRACKING_MESH_EXPORT
		#define OCEAN_TRACKING_MESH_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TRACKING_MESH_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TRACKING_MESH_EXPORT
#endif

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_MESH_MESH_H
