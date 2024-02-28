// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_CLOUD_CLOUD_RELOCALIZER_FUSED_GEO_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_CLOUD_CLOUD_RELOCALIZER_FUSED_GEO_TRACKER_6_DOF_H

#include "ocean/devices/cloud/Cloud.h"
#include "ocean/devices/cloud/CloudRelocalizerSLAMTracker6DOF.h"

#include "ocean/devices/GPSTracker.h"

#include <location_platform/location/algorithms/LSQTransformer.hpp>
#include <location_platform/location/geoanchor/GeoAnchorManager.hpp>

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

using namespace facebook::location::service;

enum class FusedLocationSource {
  Unknown,
  GPS,
  VPS
};

static const facebook::location_platform::location::geoanchor::GeoAnchorManagerConfig kVpsGeoAnchorConfig = {
  .hasMockGps = false,
  .alwaysUpdateGeoAnchorHorizontal = true,
};

static const facebook::location_platform::location::algorithms::LSQTransformerConfig kVpsLsqConfig = {
  .sigmaCompass = 10,
  .maxScale = 1.75,
  .resetBadSession = false,
  .warmupSkippedCount = 2,
  .decayDist = 60,
};

/**
 * This class implements a World Tracking-based 6DOF tracker that fuses
 * Cloud Relocalizer capabilities with GeoAnchor tracking to enable placing
 * GeoAnchors in and out of CLoud Relocalizer Coverage.
 * @ingroup devicescloud
 */
class CloudRelocalizerFusedGeoTracker6DOF : virtual public CloudRelocalizerSLAMTracker6DOF
{
  friend class CloudFactory;

  public:

    /**
     * Returns the reference coordinate system of this tracker.
     * @see Tracker::reference().
     */
    HomogenousMatrixD4 reference() const override;

    /**
     * Returns whether this device is active.
     * @see Devices::isStarted().
     */
    bool isStarted() const override;

    /**
     * Starts the device.
     * @see Device::start().
     */
    bool start() override;

    /**
     * Stops the device.
     * @see Device::stop().
     */
    bool stop() override;

    /**
     * Returns the name of this tracker.
     * @return Tracker name
     */
    static inline std::string deviceNameCloudRelocalizerFusedGeoTracker6DOF();

    /**
     * Returns the name of the last location source (VPS, GPS).
     * @return Name of last location source
     */
    inline FusedLocationSource lastLocationSource() const {
      return lastLocationSource_;
    }

  private:

    /**
     * Creates a new Cloud Relocalizer-based 6DOF tracker object.
     */
    explicit CloudRelocalizerFusedGeoTracker6DOF();

    /**
     * Destructs a Cloud Relocalizer-based 6DOF tracker object.
     */
    ~CloudRelocalizerFusedGeoTracker6DOF() override;

    /**
     * Event function for new tracking samples from the GPS tracker.
     * @param measurement The measurement object sending the sample
     * @param sample The new samples with resulting from GPS tracker
     */
    void onGPSTrackerSample(const Measurement* measurement, const SampleRef& sample);

    /**
     * Invokes the relocalization for a given camera image and given configuration.
     * @see CloudRelocalizerSLAMTracker6DOF::invokeRelocalization().
     */
    bool invokeRelocalization(
      const AnyCamera& camera,
      const Frame& yFrame,
      const VectorD2& gpsLocation,
      const Vector3& gravityVector,
      const Tracking::Cloud::CloudRelocalizer::Configuration& configuration,
      facebook::mobile::xr::IRelocalizationClient& relocalizationClient,
      HomogenousMatrix4& relocalizationReference_T_camera,
      Vectors3& objectPoints,
      Vectors2& imagePoints) override;

    /**
     * Determines the scene elements for the current tracking sample.
     * @see CloudRelocalizerSLAMTracker6DOF::determineSceneElements().
     */
    bool determineSceneElements(
      const HomogenousMatrix4& world_T_camera,
      const HomogenousMatrix4& anchor_T_world,
      const Timestamp& timestamp,
      ObjectIds& objectIds,
      Vectors3& objects_t_camera,
      Quaternions& objects_q_camera,
      SharedSceneElements& sceneElements,
      Metadata& metadata) override;

  private:

    /// The ECEF reference coordinate system this tracker uses to convert 64bit transformations to 32bit transformation, invalid if not yet defined.
    HomogenousMatrixD4 ecefWorld_T_ecefReference_ = HomogenousMatrixD4(false);

    /// The latest 3D object points used by the cloud reloc service to determine the reloc pose.
    Vectors3 latestCloudRelocObjectPoints_;

    /// The latest 2D image points used by the cloud reloc service to determine the reloc pose, one for each object point.
    Vectors2 latestCloudRelocImagePoints_;

  private:

    /// Identifier of first GPS position
    Identifier initialGeoAnchorIdentifier_ = 0;

    /// The subscription object for samples events from the GeoAnchor tracker.
    SampleEventSubscription gpsTrackerSampleEventSubscription_;

    /// The GPS tracker.
    Devices::GPSTrackerRef gpsTracker_;

    std::unique_ptr<facebook::location_platform::location::geoanchor::GeoAnchorManager>
      geoAnchorManager_ =
        std::make_unique<facebook::location_platform::location::geoanchor::GeoAnchorManager>(
          kVpsGeoAnchorConfig, kVpsLsqConfig);

    /// Timestamp of last successful VPS relocalization.
    Timestamp lastVpsTimestamp_ = Timestamp(false);

    FusedLocationSource lastLocationSource_ = FusedLocationSource::Unknown;
};

inline std::string CloudRelocalizerFusedGeoTracker6DOF::deviceNameCloudRelocalizerFusedGeoTracker6DOF()
{
  return std::string("Cloud Relocalizer Fused 6DOF Tracker");
}

}

}

}

#endif // META_OCEAN_DEVICES_CLOUD_CLOUD_RELOCALIZER_FUSED_GEO_TRACKER_6_DOF_H
