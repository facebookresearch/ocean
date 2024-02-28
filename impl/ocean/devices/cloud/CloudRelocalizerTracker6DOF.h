// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_CLOUD_CLOUD_RELOCALIZER_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_CLOUD_CLOUD_RELOCALIZER_TRACKER_6_DOF_H

#include "ocean/devices/cloud/Cloud.h"
#include "ocean/devices/cloud/CloudRelocalizerSLAMTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

/**
 * This class implements a World Tracking-based 6DOF tracker with Cloud Relocalizer capabilities.
 * @ingroup devicescloud
 */
class CloudRelocalizerTracker6DOF final : virtual public CloudRelocalizerSLAMTracker6DOF
{
	friend class CloudFactory;

	public:

		/**
		 * Returns the name of this tracker.
		 * param useFrameToFrameTracking True, to run a 2D/2D tracking after each successful relocalization request; False, to apply only relocalization requests.
		 * @return Tracker name
		 */
		static inline std::string deviceNameCloudRelocalizerTracker6DOF(const bool useFrameToFrameTracking);

	private:

		/**
		 * Creates a new Cloud Relocalizer-based 6DOF tracker object.
		 * param useFrameToFrameTracking True, to run a 2D/2D tracking after each successful relocalization request; False, to apply only relocalization requests.
		 */
		explicit CloudRelocalizerTracker6DOF(const bool useFrameToFrameTracking);

		/**
		 * Destructs a Cloud Relocalizer-based 6DOF tracker object.
		 */
		~CloudRelocalizerTracker6DOF() override;

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

		/// The latest 3D object points used by the cloud reloc service to determine the reloc pose.
		Vectors3 latestCloudRelocObjectPoints_;

		/// The latest 2D image points used by the cloud reloc service to determine the reloc pose, one for each object point.
		Vectors2 latestCloudRelocImagePoints_;
};

inline std::string CloudRelocalizerTracker6DOF::deviceNameCloudRelocalizerTracker6DOF(const bool useFrameToFrameTracking)
{
	if (useFrameToFrameTracking)
	{
		return std::string("Cloud Relocalizer 6DOF Tracker");
	}
	else
	{
		return std::string("Cloud Relocalizer 6DOF Tracker without f2f");
	}
}

}

}

}

#endif // META_OCEAN_DEVICES_CLOUD_CLOUD_RELOCALIZER_TRACKER_6_DOF_H
