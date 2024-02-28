// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/cloud/CloudRelocalizerFusedGeoTracker6DOF.h"

#include <ocean/math/SophusUtilities.h>

#include <location_platform/location/geoanchor/GeoAnchor.hpp>
#include <sophus/se3.hpp>

#include "ocean/base/Lock.h"
#include "ocean/base/Timestamp.h"

#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/Manager.h"

#include "ocean/math/Euler.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Math.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/Vector3.h"

#include "ocean/tracking/cloud/CloudRelocalizer.h"

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

static constexpr auto kInitialTrackableId = "cloudRelocalizerFusedGeoTracker6DOF-initialGeoAnchor-trackableId";
static constexpr double kGPSRadiusTolerance = 10.0;

// Time (in seconds) to rely on cached VPS point for tracking before using GPS
static constexpr int kIndoorToOutdoorTime = 30;

// Translation matrix from from initial gps coord space to ecef coord space
static const HomogenousMatrix4& kWgsToEcefRotation()
{
	static HomogenousMatrix4 kWgsToEcefRotation =
		HomogenousMatrix4(Rotation(Vector3(0, 0, 1), Numeric::deg2rad(90))) *
		HomogenousMatrix4(Rotation(Vector3(0, 1, 0), Numeric::deg2rad(180)));
	return kWgsToEcefRotation;
}

// TODO: T123346008
static HomogenousMatrixD4 convertWGSToECEFPose(
  double latitude,
  double longitude,
  double altitude)
{
  constexpr double major_axis_radius = 6378137.0;
  constexpr double minor_axis_radius = 6356752.3142;
  const double deg2rad = atan(1.0) / 45.0;

  constexpr double a = major_axis_radius; // earth semi-major axis radius
  constexpr double b = minor_axis_radius; // earth semi-minor axis radius
  constexpr double a2 = a * a; // earth semi-major axis radius
  constexpr double b2 = b * b; // earth semi-minor axis radius
  constexpr double e2 = 1. - b2 / a2; // 1 - b^2 / a^2
  constexpr double b2_a2 = b2 / a2;

  const double latRad = latitude * deg2rad;
  const double lonRad = longitude * deg2rad;
  const double sin_phi = sin(latRad);
  const double cos_phi = cos(latRad);
  const double sin_lambda = sin(lonRad);
  const double cos_lambda = cos(lonRad);
  const double sin_phi2 = sin_phi * sin_phi;
  const double N_phi = a / sqrt(1 - e2 * sin_phi2);

  const HomogenousMatrixD4 ecefTranslation =
    HomogenousMatrixD4(VectorD3((N_phi + altitude) * cos_phi * cos_lambda,
      (N_phi + altitude) * cos_phi * sin_lambda,
      (b2_a2 * N_phi + altitude) * sin_phi));

  // Construct rotation matrix to make ECEF pose facing North
  // Example ecefRotation in Boston: HomogenousMatrixD4(QuaternionD(0.25960, 0.59178, 0.69442, -0.31653))
  const HomogenousMatrix4 ecefRotation =
    HomogenousMatrix4(Quaternion(Vector3(0, 0, 1), lonRad)) *
    HomogenousMatrix4(Quaternion(Vector3(0, -1, 0), latRad)) *
    HomogenousMatrix4(Quaternion(Vector3(0, 1, 0), Numeric::pi()));

  return ecefTranslation * HomogenousMatrixD4(ecefRotation);
}

// Zhu, J. (1994). "Conversion of Earth-centered Earth-fixed coordinates to
// geodetic coordinates". IEEE Trans. Aerospace and Electronic Systems.
// 30: 957-961. doi:10.1109/7.303772.

// This is using the Ferrari solution, see also
// https://en.wikipedia.org/wiki/Geographic_coordinate_conversion#From_ECEF_to_geodetic_coordinates
// as of Nov 19 2018
//
// Copied from
// compphoto/ocean/impl/src/application/ocean/xrplayground/common/experiences/NavigationWithSDKExperience.cpp
static VectorD3 convertECEFToWGS(
  double ecefX,
  double ecefY,
  double ecefZ)
{
  const double wgs84a = 6378137.0;
  const double wgs84b = 6356752.314245;
  const double eccSq = 1.0 - (wgs84b * wgs84b / (wgs84a * wgs84a));
  const double ecc2Sq = wgs84a * wgs84a / (wgs84b * wgs84b) - 1.0;
  const double rad2deg = 45.0 / atan(1.0);

  double ecefZSq = ecefZ * ecefZ;
  double rSq = ecefX * ecefX + ecefY * ecefY;
  double r = sqrt(rSq);
  double ESq = wgs84a * wgs84a - wgs84b * wgs84b;
  double F = 54.0 * wgs84b * wgs84b * ecefZSq;
  double G = rSq + (1.0 - eccSq) * ecefZSq - eccSq * ESq;
  double C = (eccSq * eccSq * F * rSq) / pow(G, 3.0);
  double S = cbrt(1.0 + C + sqrt(C * C + 2.0 * C));
  double P = F / (3.0 * pow((S + 1.0 / S + 1.0), 2.0) * G * G);
  double Q = sqrt(1.0 + 2.0 * eccSq * eccSq * P);
  double r0 =
    -(P * eccSq * r) / (1.0 + Q) +
    sqrt(
        0.5 * wgs84a * wgs84a * (1.0 + 1.0 / Q) -
        P * (1.0 - eccSq) * ecefZSq / (Q * (1.0 + Q)) - 0.5 * P * rSq);
  double U = sqrt(pow((r - eccSq * r0), 2.0) + ecefZSq);
  double V = sqrt(pow((r - eccSq * r0), 2.0) + (1.0 - eccSq) * ecefZSq);
  double Z0 = wgs84b * wgs84b * ecefZ / (wgs84a * V);

  const double latitude = atan((ecefZ + ecc2Sq * Z0) / r) * rad2deg;
  const double longitude = atan2(ecefY, ecefX) * rad2deg;
  // ellipsoid height
  const double height = U * (1.0 - wgs84b * wgs84b / (wgs84a * V));

  const VectorD3 WGS = VectorD3(latitude, longitude, height);
  return WGS;
}

CloudRelocalizerFusedGeoTracker6DOF::CloudRelocalizerFusedGeoTracker6DOF() :
  Device(deviceNameCloudRelocalizerFusedGeoTracker6DOF(),
    deviceTypeCloudRelocalizerSLAMTracker6DOF()),
  Measurement(deviceNameCloudRelocalizerFusedGeoTracker6DOF(),
    deviceTypeCloudRelocalizerSLAMTracker6DOF()),
  Tracker(deviceNameCloudRelocalizerFusedGeoTracker6DOF(),
    deviceTypeCloudRelocalizerSLAMTracker6DOF()),
  OrientationTracker3DOF(deviceNameCloudRelocalizerFusedGeoTracker6DOF()),
  PositionTracker3DOF(deviceNameCloudRelocalizerFusedGeoTracker6DOF()),
  Tracker6DOF(deviceNameCloudRelocalizerFusedGeoTracker6DOF()),
  SceneTracker6DOF(deviceNameCloudRelocalizerFusedGeoTracker6DOF()),
  ObjectTracker(deviceNameCloudRelocalizerFusedGeoTracker6DOF(),
    deviceTypeCloudRelocalizerSLAMTracker6DOF()),
  VisualTracker(deviceNameCloudRelocalizerFusedGeoTracker6DOF(),
    deviceTypeCloudRelocalizerSLAMTracker6DOF()),
  CloudDevice(deviceNameCloudRelocalizerFusedGeoTracker6DOF(),
    deviceTypeCloudRelocalizerSLAMTracker6DOF()),
  CloudRelocalizerSLAMTracker6DOF(
    deviceNameCloudRelocalizerFusedGeoTracker6DOF(),
    true /*useFrameToFrameTracking*/)
{
  gpsTracker_ =
    Manager::get().device(Devices::GPSTracker::deviceTypeGPSTracker());

  if (gpsTracker_.isNull()) {
    Log::warning() << "FusedGeoTracker: Failed to access GPS tracker!";

    deviceIsValid = false;
  }

  relocalizeWithFewCorrespondences_ = true;

  Log::info() << "FusedGeoTracker <init>";
}

CloudRelocalizerFusedGeoTracker6DOF::~CloudRelocalizerFusedGeoTracker6DOF()
{
  stop();
}

HomogenousMatrixD4 CloudRelocalizerFusedGeoTracker6DOF::reference() const
{
  const ScopedLock scopedLock(deviceLock);

  return ecefWorld_T_ecefReference_;
}

bool CloudRelocalizerFusedGeoTracker6DOF::isStarted() const
{
  const ScopedLock scopedLock(deviceLock);

  ocean_assert(worldTracker_ && gpsTracker_);

  return worldTracker_->isStarted() && gpsTracker_->isStarted();
}

bool CloudRelocalizerFusedGeoTracker6DOF::start()
{
  if (!CloudRelocalizerSLAMTracker6DOF::start()) {
    return false;
  }

  const ScopedLock scopedLock(deviceLock);

  ocean_assert(gpsTracker_);

  if (!gpsTracker_->start()) {
    return false;
  }

  if (!gpsTrackerSampleEventSubscription_) {
    gpsTrackerSampleEventSubscription_ = gpsTracker_->subscribeSampleEvent(
      SampleCallback::create(*this, &CloudRelocalizerFusedGeoTracker6DOF::onGPSTrackerSample));
  }

  return true;
}

bool CloudRelocalizerFusedGeoTracker6DOF::stop()
{
  if (!CloudRelocalizerSLAMTracker6DOF::stop()) {
    return false;
  }

  const ScopedLock scopedLock(deviceLock);

  gpsTrackerSampleEventSubscription_.release();

  if (gpsTracker_) {
    return gpsTracker_->stop();
  }

  return true;
}

void CloudRelocalizerFusedGeoTracker6DOF::onGPSTrackerSample(
  const Measurement* /*measurements*/, const SampleRef& sample)
{
  const ScopedLock scopedLock(deviceLock);

  const GPSTracker::GPSTrackerSampleRef gpsTrackerSample(sample);
  ocean_assert(gpsTrackerSample);

  if (gpsTrackerSample->locations().size() != 1) {
    return;
  }

  const GPSTracker::Location& gpsSampleLocation = gpsTrackerSample->locations().front();

  const double longitude = gpsSampleLocation.longitude();
  const double latitude = gpsSampleLocation.latitude();
  const double altitude = gpsSampleLocation.altitude();
  const double horizontalAccuracy = gpsSampleLocation.accuracy();
  const double altitudeAccuracy = gpsSampleLocation.altitudeAccuracy();
  const Timestamp& unixTimestamp = gpsTrackerSample->timestamp();
  const float direction = gpsSampleLocation.direction();
  const float directionAccuracy = gpsSampleLocation.directionAccuracy();

#ifdef OCEAN_DEBUG
  Log::info() << "FusedGeoTracker: onGPSTrackerSample location sample: ["
              << latitude << ", " << longitude << ", " << altitude
              << ", t=" << double(unixTimestamp) << "]";
#endif

  LocationSignal locationSignal = createEmptySignal();
  locationSignal.latitude = latitude;
  locationSignal.longitude = longitude;
  locationSignal.horizontalAccuracy = horizontalAccuracy;
  locationSignal.altitude = altitude;
  locationSignal.altitudeAccuracy = altitudeAccuracy;
  locationSignal.bearing = direction;
  locationSignal.bearingAccuracy = directionAccuracy;

  if (world_T_cameras_.empty()) {
    Log::info() << "FusedGeoTracker: world_T_cameras_ is empty, not continuing";
    return;
  }

  // Create GeoAnchor of starting GPS position
  if (initialGeoAnchorIdentifier_ == 0) {
    initialGeoAnchorIdentifier_ = geoAnchorManager_->createGeoAnchor(
      kInitialTrackableId, latitude, longitude, false, 0.0, 0.0, kGPSRadiusTolerance,
      facebook::location_platform::location::algorithms::ElevationType::CAMERA);

    // save ECEF reference of initial geoanchor
    if (!ecefWorld_T_ecefReference_.isValid()) {
      ecefWorld_T_ecefReference_ = convertWGSToECEFPose(latitude, longitude, 0.0);
    }

    ocean_assert(ecefWorld_T_ecefReference_.isValid());
  }

  // use latest of world_T_cameras_ as camera pose
  HomogenousMatrix4 T_world_camera = world_T_cameras_.rbegin()->second;
  auto cameraPose = Ocean::SophusUtilities::toSE3<Ocean::Scalar, float>(T_world_camera);

  geoAnchorManager_->updateGpsData(locationSignal);
  geoAnchorManager_->updateTransformerStates(cameraPose, double(unixTimestamp));
}

bool CloudRelocalizerFusedGeoTracker6DOF::invokeRelocalization(
  const AnyCamera& camera,
  const Frame& yFrame,
  const VectorD2& gpsLocation,
  const Vector3& gravityVector,
  const Tracking::Cloud::CloudRelocalizer::Configuration& configuration,
  facebook::mobile::xr::IRelocalizationClient& relocalizationClient,
  HomogenousMatrix4& relocalizationReference_T_camera,
  Vectors3& objectPoints,
  Vectors2& imagePoints)
{
  HomogenousMatrixD4 ecefWorld_T_camera(false);

  VectorsD3 ecefObjectPoints;
  VectorsD2 ecefImagePoints;

  const bool cloudRelocalizeSuccess =
    Tracking::Cloud::CloudRelocalizer::relocalizeToECEF(
      camera,
      yFrame,
      gpsLocation,
      gravityVector,
      configuration,
      relocalizationClient,
      ecefWorld_T_camera,
      &ecefObjectPoints,
      &ecefImagePoints);

  const Timestamp timestamp(true);

  if (cloudRelocalizeSuccess) {

    Log::info() << "FusedGeoTracker: using vps tracking";

    TemporaryScopedLock scopedLock(deviceLock);

    if (!ecefWorld_T_ecefReference_.isValid()) {
      // as reference we use the very first relocalization pose
      ecefWorld_T_ecefReference_ = ecefWorld_T_camera;
    }

    ocean_assert(ecefWorld_T_ecefReference_.isValid());

    const HomogenousMatrixD4 ecefReference_T_ecefWorld(ecefWorld_T_ecefReference_.inverted());

    scopedLock.release();

    relocalizationReference_T_camera = HomogenousMatrix4(ecefReference_T_ecefWorld * ecefWorld_T_camera);

    objectPoints.clear();
    objectPoints.reserve(ecefObjectPoints.size());
    for (const VectorD3& ecefObjectPoint : ecefObjectPoints) {
      // the object points are stored in relation to the ECEF reference coordinate system
      objectPoints.emplace_back(ecefReference_T_ecefWorld * ecefObjectPoint);
    }

    imagePoints.clear();
    imagePoints.reserve(ecefImagePoints.size());
    for (const VectorD2& ecefImagePoint : ecefImagePoints) {
      imagePoints.emplace_back(ecefImagePoint);
    }

    lastVpsTimestamp_ = timestamp;

    // get WGS coordinates and bearing from ECEF matrix
    const VectorD3 ecefTranslation = ecefWorld_T_camera.translation();
    const Quaternion ecefRotation = Quaternion(ecefWorld_T_camera.rotation());

    const VectorD3 wgsCoordinates = convertECEFToWGS(
      ecefTranslation.x(),
      ecefTranslation.y(),
      ecefTranslation.z());
    const float ecefBearing = Numeric::rad2deg(Euler(ecefRotation).yaw());

    LocationSignal locationSignal = createEmptySignal();
    locationSignal.latitude = wgsCoordinates.x();
    locationSignal.longitude = wgsCoordinates.y();
    locationSignal.horizontalAccuracy = 0.0;
    locationSignal.altitude = 0.0; // do not use altitude
    locationSignal.altitudeAccuracy = 0.0;
    locationSignal.bearing = ecefBearing;
    locationSignal.bearingAccuracy = 0.0;
    locationSignal.declination = 0.0;

    // in future, may need to change
    geoAnchorManager_->updateGpsData(locationSignal);

    HomogenousMatrix4 T_world_camera = world_T_cameras_.rbegin()->second;
    auto cameraPose = Ocean::SophusUtilities::toSE3<Ocean::Scalar, float>(T_world_camera);

    // add cloud tracker position to GeoAnchorManager
    geoAnchorManager_->updateTransformerStates(cameraPose, double(timestamp));

    lastLocationSource_ = FusedLocationSource::VPS;

  } else { // failed to use cloud relocalizer, fallback to GeoAnchor tracking

    // if there was a recent vps sample, we don't need to use gps for some time
    if (lastVpsTimestamp_.isValid() &&
        (double(Timestamp(true) - lastVpsTimestamp_) < kIndoorToOutdoorTime)) {
      return false;
    }

    Log::info() << "FusedGeoTracker: using gps tracking";

    // return if world tracking has not started
    if (world_T_cameras_.empty()) {
      return false;
    }

    // return if we have not received any gps samples
    if (!ecefWorld_T_ecefReference_.isValid()) {
      return false;
    }

    TemporaryScopedLock scopedLock(deviceLock);

    HomogenousMatrix4 T_world_camera = world_T_cameras_.rbegin()->second;
    auto cameraPose = Ocean::SophusUtilities::toSE3<Ocean::Scalar, float>(T_world_camera);
    const bool cameraHasMoved = geoAnchorManager_->updateCameraPosition(cameraPose);

    // update GeoAnchors
    for (auto& [identifier, geoAnchor] : geoAnchorManager_->getGeoAnchors())
    {
      if (geoAnchorManager_->shouldUpdateGeoAnchorHorizontal(geoAnchor, cameraHasMoved)) {
        geoAnchorManager_->updateGeoAnchorPose(geoAnchor, cameraPose, double(timestamp));
      } else {
        geoAnchorManager_->updateGeoAnchorHeightOnly(geoAnchor, cameraPose, double(timestamp));
      }

      auto T_world_anchor = geoAnchorManager_->getGeoAnchorInterpolatedPose(
        identifier, int64_t(double(timestamp) * 1000.0));

      geoAnchorManager_->setGeoAnchorLastArWorld(identifier, T_world_anchor);
    }

    const Sophus::SE3f T_world_anchor = geoAnchorManager_->getGeoAnchorInterpolatedPose(
      initialGeoAnchorIdentifier_, int64_t(double(timestamp) * 1000.));

    const HomogenousMatrix4 T_anchor_world =
      Ocean::SophusUtilities::toHomogenousMatrix4<float, Ocean::Scalar>(
        T_world_anchor.inverse());

    // get T_world_camera as latest pose in world_T_cameras
    T_world_camera = world_T_cameras_.rbegin()->second;

    HomogenousMatrix4 T_anchor_camera = T_anchor_world * T_world_camera;

    relocalizationReference_T_camera = kWgsToEcefRotation() * T_anchor_camera;

    objectPoints.clear();
    imagePoints.clear();

    lastLocationSource_ = FusedLocationSource::GPS;
  }

  // making a copy for the scene tracker sample
  latestCloudRelocObjectPoints_ = objectPoints;
  latestCloudRelocImagePoints_ = imagePoints;

  return true;
}

bool CloudRelocalizerFusedGeoTracker6DOF::determineSceneElements(
  const HomogenousMatrix4& world_T_camera,
  const HomogenousMatrix4& anchor_T_world,
  const Timestamp& /*timestamp*/,
  ObjectIds& objectIds,
  Vectors3& objects_t_camera,
  Quaternions& objects_q_camera,
  SharedSceneElements& sceneElements,
  Metadata& /*metadata*/)
{
  ocean_assert(world_T_camera.isValid());
  ocean_assert(anchor_T_world.isValid());

  ocean_assert(
    objectIds.empty() && objects_t_camera.empty() &&
    objects_q_camera.empty() && sceneElements.empty());

  const ScopedLock scopedLock(deviceLock);

  if (objectId_ != invalidObjectId()) {
    // we have only one object transformation (the transformation for the anchor)

    const HomogenousMatrix4 anchor_T_camera = anchor_T_world * world_T_camera;

    objectIds.emplace_back(objectId_);
    objects_t_camera.emplace_back(anchor_T_camera.translation());
    objects_q_camera.emplace_back(anchor_T_camera.rotation());

    Vectors3 latestCloudRelocObjectPoints(std::move(latestCloudRelocObjectPoints_));
    Vectors2 latestCloudRelocImagePoints(std::move(latestCloudRelocImagePoints_));

    ocean_assert(
      latestCloudRelocObjectPoints.size() == latestCloudRelocImagePoints.size());

    if (latestCloudRelocObjectPoints.empty()) {
      sceneElements.emplace_back(nullptr); // a pure pose scene element
    } else {
      sceneElements.emplace_back(
        std::make_shared<SceneElementFeatureCorrespondences>(
          std::move(latestCloudRelocObjectPoints),
          std::move(latestCloudRelocImagePoints)));
    }
  }

  ocean_assert(objectIds.size() == objects_t_camera.size());
  ocean_assert(objectIds.size() == objects_q_camera.size());
  ocean_assert(objectIds.size() == sceneElements.size());

  return true;
}

} // namespace Cloud

} // namespace Devices

} // namespace Ocean
