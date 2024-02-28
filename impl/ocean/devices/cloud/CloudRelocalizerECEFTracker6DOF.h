// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_CLOUD_CLOUD_RELOCALIZER_ECEF_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_CLOUD_CLOUD_RELOCALIZER_ECEF_TRACKER_6_DOF_H

#include "ocean/devices/cloud/Cloud.h"
#include "ocean/devices/cloud/CloudRelocalizerSLAMTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

/**
 * This class implements a World Tracking-based 6DOF tracker with Cloud Relocalizer capabilities using the ECEF coordinate system for relocalization.
 * @ingroup devicescloud
 */
class CloudRelocalizerECEFTracker6DOF : virtual public CloudRelocalizerSLAMTracker6DOF
{
	friend class CloudFactory;

	public:

		/**
		 * Returns the reference coordinate system of this tracker.
		 * @see Tracker::reference().
		 */
		HomogenousMatrixD4 reference() const override;

		/**
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceNameCloudRelocalizerECEFTracker6DOF();

	private:

		/**
		 * Creates a new Cloud Relocalizer-based 6DOF tracker object.
		 */
		explicit CloudRelocalizerECEFTracker6DOF();

		/**
		 * Destructs a Cloud Relocalizer-based 6DOF tracker object.
		 */
		~CloudRelocalizerECEFTracker6DOF() override;

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

		/// The ECEF reference coordinate system this tracker uses to convert 64bit transformations to 32bit transformation, invalid if not yet defined.
		HomogenousMatrixD4 ecefWorld_T_ecefReference_ = HomogenousMatrixD4(false);

		/// The latest 3D object points used by the cloud reloc service to determine the reloc pose.
		Vectors3 latestCloudRelocObjectPoints_;

		/// The latest 2D image points used by the cloud reloc service to determine the reloc pose, one for each object point.
		Vectors2 latestCloudRelocImagePoints_;
};

inline std::string CloudRelocalizerECEFTracker6DOF::deviceNameCloudRelocalizerECEFTracker6DOF()
{
	return std::string("Cloud Relocalizer ECEF 6DOF Tracker");
}

}

}

}

#endif // META_OCEAN_DEVICES_CLOUD_CLOUD_RELOCALIZER_ECEF_TRACKER_6_DOF_H
