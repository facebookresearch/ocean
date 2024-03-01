// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MRROOMPLAN_MR_ROOM_PLAN_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MRROOMPLAN_MR_ROOM_PLAN_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/SceneTracker6DOF.h"

#include "ocean/io/Bitstream.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Vector3.h"

#include "metaonly/ocean/network/verts/Driver.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements the base class for all MR Room Plan experiences.
 * @ingroup xrplayground
 */
class MRRoomPlan
{
	protected:

		/// The unique tag for a map.
		static constexpr uint64_t mapTag_ = IO::Tag::string2tag("_OCNMAP_");

		/// The unique tag for object points.
		static constexpr uint64_t objectPointsTag_ = IO::Tag::string2tag("_OCNOPT_");

		/// The unique tag for room objects.
		static constexpr uint64_t roomObjectsTag_ = IO::Tag::string2tag("_OCNROS_");

	protected:

		/**
		 * Writes 3D object points to a bitstream.
		 * @param objectPoints The 3D object points
		 * @param objectPointIds The ids of the 3D object points, one for each object point
		 * @param bitstream The output stream to which the points will be written
		 * @return True, if succeeded
		 */
		static bool writeObjectPointsToStream(const Vectors3& objectPoints, const Indices32& objectPointIds, IO::OutputBitstream& bitstream);

		/**
		 * Writes room objects to a bitstream.
		 * @param roomObjectMap The map with room objects to write
		 * @param bitstream output input stream to which the objects will be written
		 * @return True, if succeeded
		 */
		static bool writeRoomObjectsToStream(const Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap& roomObjectMap, IO::OutputBitstream& bitstream);

		/**
		 * Reads 3D object points from a bitstream.
		 * @param bitstream The input stream from which the points will be read
		 * @param objectPoints The resulting 3D object points
		 * @param objectPointIds The resulting ids of the object points, one for each object point
		 * @return True, if succeeded
		 */
		static bool readObjectPointsFromStream(IO::InputBitstream& bitstream, Vectors3& objectPoints, Indices32& objectPointIds);

		/**
		 * Reads room objects from a bitstream.
		 * @param bitstream The input stream from which the points will be read
		 * @param roomObjectMap The resulting map with room objects
		 * @return True, if succeeded
		 */
		static bool readRoomObjectsToStream(IO::InputBitstream& bitstream, Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap& roomObjectMap);

		/**
		 * Returns the color for a given planar room object.
		 * @param planarType The type of the planar room object
		 * @param alpha The alpha value to be used, with range [0, 1]
		 * @return The resulting color
		 */
		static RGBAColor planarColor(const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PlanarType planarType, const float alpha = 1.0f);

		/**
		 * Returns the thickness for a given planar room object.
		 * @param planarType The type of the planar room object
		 * @return The resulting thickness
		 */
		static Scalar planarThickness(const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PlanarType planarType);

		/**
		 * Returns the color for a given volumetric room object.
		 * @param volumetricType The type of the volumetric room object
		 * @param alpha The alpha value to be used, with range [0, 1]
		 * @return The resulting color
		 */
		static RGBAColor volumetricColor(const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VolumetricType volumetricType, const float alpha = 0.75f);

		/**
		 * Returns an adjusted dimension for volumetric objects.
		 * Adjustments will be minor to improve visualization.
		 * @param volumetricType The type of the volumetric room object
		 * @param dimension The dimension of the object to adjust
		 * @return The adjusted volumetric object
		 */
		static Vector3 adjustedVolumetricDimension(const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VolumetricType volumetricType, const Vector3& dimension);

	protected:

		/// The map holding the latest prending room objects which needs to be updated.
		Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap pendingRoomObjectMap_;

		/// The VERTS driver for the network communication.
		Network::Verts::SharedDriver vertsDriver_;

		/// The VERTS node containing the camera pose.
		Network::Verts::SharedNode vertsDevicePoseNode_;

		/// The subscription object for changed users events.
		Network::Verts::Driver::ChangedUsersScopedSubscription changedUsersScopedSubscription_;

		/// The subscription object for data receive events.
		Network::Verts::Driver::ReceiveContainerScopedSubscription receiveContainerScopedSubscription_;

		/// The ids of all active users.
		UnorderedIndexSet64 userIds_;

		/// The experience's lock.
		mutable Lock lock_;
};

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MRROOMPLAN_MR_ROOM_PLAN_H
