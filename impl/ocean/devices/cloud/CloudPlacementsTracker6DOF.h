// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_CLOUD_CLOUD_PLACEMENTS_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_CLOUD_CLOUD_PLACEMENTS_TRACKER_6_DOF_H

#include "ocean/devices/cloud/Cloud.h"
#include "ocean/devices/cloud/CloudRelocalizerSLAMTracker6DOF.h"

#include "ocean/tracking/cloud/CloudRelocalizer.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

/**
 * This class implements a World Tracking-based 6DOF tracker with Cloud Relocalizer capabilities supporting several placements.
 * @ingroup trackingcloud
 */
class CloudPlacementsTracker6DOF final : virtual public CloudRelocalizerSLAMTracker6DOF
{
	friend class CloudFactory;

	public:

		/**
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceNameCloudPlacementsTracker6DOF();

	private:

		/**
		 * Creates a new Cloud Placements 6DOF tracker object.
		 */
		explicit CloudPlacementsTracker6DOF();

		/**
		 * Destructs a Cloud Placements 6DOF tracker object.
		 */
		~CloudPlacementsTracker6DOF() override;

		/**
		 * Invokes the relocalization for a given camera image and given configuration.
		 * @see CloudRelocalizerSLAMTracker6DOF::invokeRelocalization().
		 */
		bool invokeRelocalization(const AnyCamera& camera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector, const Tracking::Cloud::CloudRelocalizer::Configuration& configuration, facebook::mobile::xr::IRelocalizationClient& relocalizationClient, HomogenousMatrix4& relocalizationReference_T_camera, Vectors3& objectPoints, Vectors2& imagePoints) override;

		/**
		 * Determines the scene elements for the current tracking sample.
		 * @see CloudRelocalizerSLAMTracker6DOF::determineSceneElements().
		 */
		bool determineSceneElements(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& anchor_T_world, const Timestamp& timestamp, ObjectIds& objectIds, Vectors3& objects_t_camera, Quaternions& objects_q_camera, SharedSceneElements& sceneElements, Metadata& metadata) override;

	private:

		/// The most recent cloud placements.
		Tracking::Cloud::CloudRelocalizer::Placements latestPlacements_;

		/// The latest 3D object points used by the cloud reloc service to determine the reloc pose.
		Vectors3 latestCloudRelocObjectPoints_;

		/// The latest 2D image points used by the cloud reloc service to determine the reloc pose, one for each object point.
		Vectors2 latestCloudRelocImagePoints_;
};

inline std::string CloudPlacementsTracker6DOF::deviceNameCloudPlacementsTracker6DOF()
{
	return std::string("Cloud Placements 6DOF Tracker");
}

}

}

}

#endif // META_OCEAN_DEVICES_CLOUD_CLOUD_PLACEMENTS_TRACKER_6_DOF_H
