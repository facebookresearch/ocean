// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_CLOUD_CLOUD_RELOCALIZER_SLAM_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_CLOUD_CLOUD_RELOCALIZER_SLAM_TRACKER_6_DOF_H

#include "ocean/devices/cloud/Cloud.h"
#include "ocean/devices/cloud/CloudDevice.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/tracking/SmoothedTransformation.h"

#include "ocean/tracking/cloud/CloudRelocalizer.h"

#include <atomic>

namespace Ocean
{

namespace Devices
{

namespace Cloud
{

/**
 * This class implements the abstract base class for all World Tracking-based 6DOF tracker with Cloud Relocalizer capabilities.
 * @ingroup devicescloud
 */
class CloudRelocalizerSLAMTracker6DOF :
	virtual public SceneTracker6DOF,
	virtual public ObjectTracker,
	virtual public VisualTracker,
	virtual public CloudDevice,
	protected Thread
{
	protected:

		/**
		 * Definition of an ordered map mapping timestamps to transformations.
		 */
		typedef std::map<Timestamp, HomogenousMatrix4> TransformationMap;

	public:

		/**
		 * Sets the multi-view visual input of this tracker.
		 * @see VisualTracker::setInput().
		 */
		void setInput(Media::FrameMediumRefs&& frameMediums) override;

		/**
		 * Adds a new tracking pattern.
		 * For this pattern feature based tracker the pattern must be the url of an image.
		 * @see ObjectTracker::registerObject().
		 */
		ObjectId registerObject(const std::string& description, const Vector3& dimension) override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

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

	protected:

		/**
		 * Creates a new Cloud Relocalizer-based 6DOF tracker object.
		 * @param name The name of the device
		 * @param useFrameToFrameTracking True, to run a 2D/2D tracking after each successful relocalization request; False, to apply only relocalization requests.
		 */
		CloudRelocalizerSLAMTracker6DOF(const std::string& name, const bool useFrameToFrameTracking);

		/**
		 * Destructs a Cloud Relocalizer-based 6DOF tracker object.
		 */
		~CloudRelocalizerSLAMTracker6DOF() override;

		/**
		 * Event function for new tracking samples from the world tracker.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new samples with resulting from world tracker
		 */
		void onWorldTrackerSample(const Measurement* measurement, const SampleRef& sample);

		/**
		 * Event function for new tracking object event from the world tracker.
		 * @param tracker The sender of the event, must be valid
		 * @param found True, if the object ids were found; False if the object ids were lost
		 * @param objectIds The ids of the object which are found or lost, at least one
		 * @param timestamp The timestamp of the event
		 */
		void onWorldTrackerObject(const Tracker* tracker, const bool found, const ObjectIdSet& objectIds, const Timestamp& timestamp);

		/**
		 * The thread run function.
		 * @see Thread::threadRun()
		 */
		void threadRun() override;

		/**
		 * Calculates the 7-DOF transformation between the Cloud anchor and the SLAM world.
		 * @param anchor_T_cameras The known transformations between camera(s) and Cloud anchor, at least one
		 * @return The resulting 7-DOF transformation, an invalid transformation if now corresponding transformation could be found
		 */
		HomogenousMatrix4 calculateAnchor_T_world(const TransformationMap& anchor_T_cameras);

		/**
		 * Determins the configuration for the cloud relocalization.
		 * @param configuration The resulting configuration
		 * @return True, if succeeded
		 */
		virtual bool determineConfiguration(Tracking::Cloud::CloudRelocalizer::Configuration& configuration);

		/**
		 * Determines the scene elements for the current tracking sample.
		 * @param world_T_camera The transformation between camera and the SLAM world, must be valid
		 * @param anchor_T_world The 7-DOF transformation between the SLAM world and the cloud anchor, must be valid
		 * @param timestamp The current sample timestamp, must be valid
		 * @param objectIds The resulting ids of all tracking objects, at least one
		 * @param objects_t_camera The resulting translations for the individual tracking objects, one for each object id
		 * @param objects_q_camera The resulting rotations for the individual tracking objects, one for each object id
		 * @param sceneElements The resulting scene elements for the individual tracking objects, one for each object id
		 * @param metadata The metadata for the current sample
		 * @return True, if succeeded
		 */
		virtual bool determineSceneElements(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& anchor_T_world, const Timestamp& timestamp, ObjectIds& objectIds, Vectors3& objects_t_camera, Quaternions& objects_q_camera, SharedSceneElements& sceneElements, Metadata& metadata) = 0;

		/**
		 * Invokes the relocalization for a given camera image and given configuration.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param yFrame The image to be used for relocalization, with pixel format FORMAT_Y8, must be valid
		 * @param gpsLocation The GPS location, with latitude (x) in degree and longitude (y) in degree, must be valid
		 * @param gravityVector The gravity vector with unit length, defined in the coordinate system of the camera (with a default camera pointing towards negative z space), the gravity vector points towards the center of earth, must be valid
		 * @param configuration The configuration to be used during relocalization
		 * @param relocalizationClient The HTTP client which will be used for the relocalization request, must be valid
		 * @param relocalizationReference_T_camera The resulting relocalization result providing the transformation between camera and the relocalization reference
		 * @param objectPoints The object points which have been used during relocalization, defined in the coordinate system of the relocalization reference
		 * @param imagePoints The image points which have been used during relocalization, one for each 3D object points
		 */
		virtual bool invokeRelocalization(const AnyCamera& camera, const Frame& yFrame, const VectorD2& gpsLocation, const Vector3& gravityVector, const Tracking::Cloud::CloudRelocalizer::Configuration& configuration, facebook::mobile::xr::IRelocalizationClient& relocalizationClient, HomogenousMatrix4& relocalizationReference_T_camera, Vectors3& objectPoints, Vectors2& imagePoints) = 0;

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeCloudRelocalizerSLAMTracker6DOF();

	protected:

		/// The map mapping timestamps to transformations between camera and world.
		TransformationMap world_T_cameras_;

		/// The 7-DOF transformation between world and anchor (6-DOF + 1D scale), if known.
		Tracking::SmoothedTransformation anchor_T_world_ = Tracking::SmoothedTransformation(1.0);

		/// The 6-DOF world tracker.
		Devices::Tracker6DOFRef worldTracker_;

		/// The unique id of the anchor object.
		ObjectId objectId_ = invalidObjectId();

		/// The description of the cloud map, which is the map's label.
		std::string objectDescription_;

		/// The ids of all currently tracked objects.
		ObjectIdSet trackedObjectIds_;

		/// The timestamp when World Tracking was initialized.
		Timestamp worldTrackerInitializedTimestamp_ = Timestamp(false);

		/// The subscription object for samples events from the world tracker.
		SampleEventSubscription worldTrackerSampleEventSubscription_;

		/// The subscription object for object events from the world tracker.
		TrackerObjectEventSubscription worldTrackerObjectEventSubscription_;

		/// True, to run a 2D/2D tracking after each successful relocalization request; False, to apply only relocalization requests.
		bool useFrameToFrameTracking_ = true;

		/// False, does not relocalize if less than threshold for correspondences
		/// in a frame. True, relocalizes despite this.
		bool relocalizeWithFewCorrespondences_ = false;

		/// The timestamp of the frame which has been used for the very first relocalization (successful or not).
		std::atomic<Timestamp> firstRelocalizationTimestamp_ = Timestamp(false);

		/// The timestamp of the last successful relocalization.
		std::atomic<Timestamp> lastSuccessfulRelocalizationTimestamp_ = Timestamp(false);

		/// The timestamp of the last failed relocalization.
		std::atomic<Timestamp> lastFailedRelocalizationTimestamp_ = Timestamp(false);

		/// The current number of features used during frame-to-frame tracking.
		std::atomic<size_t> currentFeatureNumberFrameToFrame_ = 0;

		/// The current number of cloud relocalization requests sent.
		int numberOfRequestsSent_ = 0;

		/// The current number of successful cloud relocalization requests received.
		int numberOfSuccessfulRequestsReceived_ = 0;
};

inline CloudRelocalizerSLAMTracker6DOF::DeviceType CloudRelocalizerSLAMTracker6DOF::deviceTypeCloudRelocalizerSLAMTracker6DOF()
{
	return DeviceType(DEVICE_TRACKER, SCENE_TRACKER_6DOF | TRACKER_VISUAL | TRACKER_OBJECT);
}

}

}

}

#endif // META_OCEAN_DEVICES_CLOUD_CLOUD_RELOCALIZER_SLAM_TRACKER_6_DOF_H
