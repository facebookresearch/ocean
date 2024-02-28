// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_PROTOTYPE_DEVICES_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_PROTOTYPE_DEVICES_H

#include "application/ocean/xrplayground/XRPlayground.h"

#include "ocean/base/Thread.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/Tracker6DOF.h"
#include "ocean/devices/VisualTracker.h"
#include "ocean/devices/GPSTracker.h"

#include <location_platform/location/algorithms/LSQTransformer.hpp>
#include <location_platform/location/algorithms/SimpleProcrustes.hpp>
#include <location_platform/location/geoanchor/GeoAnchorManager.hpp>

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class allows to register and implement prototoype devices.
 * @ingroup xrplayground
 */
class PrototypeDevices
{
	public:

		/**
		 * Registers all prototype devices.
		 * @return True, if succeeded
		 */
		static bool registerPrototypeDevices();
};

/**
 * This class implements a World Tracking-based 6DOF tracker with model relocalization capabilities.
 * The 3D model is based on 3D Blob features which are used for relocalization.<br>
 * Those features can represent any kind of model (e.g., a landmark).<br>
 * The relocalization is done on device instead of using a backend system.
 * The main purpose of this devices is to demonstrate how such a tracker could be implemented in XRPlayground.
 */
class OfflineRelocalizationTracker6DOF :
	virtual public Devices::Tracker6DOF,
	virtual public Devices::ObjectTracker,
	virtual public Devices::VisualTracker,
	protected Thread
{
	friend class PrototypeDevices;

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
		 * Returns the name of the owner library.
		 * @see Devices::library().
		 */
		const std::string& library() const override;

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
		static inline std::string deviceName();

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceType();

	private:

		/**
		 * Creates a new offline relocalization 6DOF tracker object.
		 */
		OfflineRelocalizationTracker6DOF();

		/**
		 * Destructs an Offline relocalization 6DOF tracker object.
		 */
		~OfflineRelocalizationTracker6DOF() override;

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
		 * Creates a new OfflineRelocalizationTracker6DOF device object.
		 * This function will be called automatically once the device is requested.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the must be valid
		 * @return The new device
		 */
		static Device* create(const std::string& name, const Device::DeviceType& deviceType);

	private:

		/// The name of the library providing this device.
		std::string libraryName_;

		/// The features of the model to be used for relocalization (mainly a point cloud of 3D features).
		CV::Detector::Blob::BlobFeatures modelFeatures_;

		/// The map mapping timestamps to transformations between camera and world.
		TransformationMap world_T_cameras_;

		/// The transformation between world and model, if known.
		HomogenousMatrix4 model_T_world_ = HomogenousMatrix4(false);

		/// The 6-DOF world tracker.
		Devices::Tracker6DOFRef worldTracker_;

		/// The unique id of the model object.
		ObjectId modelObjectId_ = invalidObjectId();

		/// True, if the model pose has been reported to be tracked.
		bool modelIsTracked_ = false;

		/// The timestamp when World Tracking was initialized.
		Timestamp worldTrackerInitializedTimestamp_ = Timestamp(false);

		/// The subscription object for samples events from the world tracker.
		SampleEventSubscription worldTrackerSampleSubscription_;

		/// The subscription object for object events from the world tracker.
		TrackerObjectEventSubscription worldTrackerObjectSubscription_;

		/// The lock to make object thread-safe.
		Lock lock_;
};

inline std::string OfflineRelocalizationTracker6DOF::deviceName()
{
	return std::string("Offline Relocalization 6DOF Tracker");
}

inline OfflineRelocalizationTracker6DOF::DeviceType OfflineRelocalizationTracker6DOF::deviceType()
{
	return OfflineRelocalizationTracker6DOF::DeviceType(DEVICE_TRACKER, TRACKER_6DOF | TRACKER_VISUAL);
}

/**
 * This class implements a Floor tracker.
 * The floor tracker uses plane trackers on mobile platforms or Quest's floor tracker to determine the floor in relation to World.
 */
class FloorTracker6DOF :
	virtual public Devices::Tracker6DOF,
	virtual public Devices::VisualTracker
{
	friend class PrototypeDevices;

	public:

		/**
		 * Sets the multi-view visual input of this tracker.
		 * @see VisualTracker::setInput().
		 */
		void setInput(Media::FrameMediumRefs&& frameMediums) override;

		/**
		 * Returns the name of the owner library.
		 * @see Devices::library().
		 */
		const std::string& library() const override;

		/**
		 * Returns whether this device is active.
		 * @see Devices::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

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
		static inline std::string deviceName();

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceType();

	private:

		/**
		 * Creates a new foor 6DOF tracker object.
		 */
		FloorTracker6DOF();

		/**
		 * Destructs a floor tracker object.
		 */
		~FloorTracker6DOF() override;

		/**
		 * Event function for new tracking samples from the world tracker.
		 * @param measurement The measurement object sending the sample
		 * @param sample The new samples with resulting from world tracker
		 */
		void onTrackerSample(const Measurement* measurement, const SampleRef& sample);

		/**
		 * Creates a new FloorTracker6DOF device object.
		 * This function will be called automatically once the device is requested.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the must be valid
		 * @return The new device
		 */
		static Device* create(const std::string& name, const Device::DeviceType& deviceType);

	private:

		/// The name of the library providing this device.
		std::string libraryName_;

		/// The 6-DOF floor tracker.
		Devices::Tracker6DOFRef tracker6DOF_;

		/// The subscription object for sample events from the tracker.
		Devices::Measurement::SampleEventSubscription trackerSampleEventSubscription_;

		/// The timestamp when the floor was updated the last time.
		Timestamp floorTimestamp_;

		/// The object id of the floor.
		ObjectId floorObjectId_ = invalidObjectId();

		/// True, if the floor is actively tracked.
		bool floorIsTracked_ = false;

		/// The most recent world_T_floor transformation.
		HomogenousMatrix4 world_T_recentFloor_ = HomogenousMatrix4(false);
};

inline std::string FloorTracker6DOF::deviceName()
{
	return std::string("XRPlayground Floor 6DOF Tracker");
}

inline FloorTracker6DOF::DeviceType FloorTracker6DOF::deviceType()
{
	return FloorTracker6DOF::DeviceType(DEVICE_TRACKER, TRACKER_6DOF | TRACKER_VISUAL);
}

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
 * This class implements GeoAnchor tracking.
 * @ingroup devicespattern
 */
class GeoAnchorTracker6DOF :
    virtual public Devices::Tracker6DOF,
    virtual public Devices::ObjectTracker,
    virtual public Devices::VisualTracker
{
	friend class PrototypeDevices;

    public:

		struct GeoAnchor {
			GeoAnchorTracker6DOF::ObjectId objectId;
			double latitude;
			double longitude;
			double altitude{0};
			double bearing{0};
			bool isAdded;
			facebook::location_platform::location::algorithms::ElevationType
				elevationType{
					facebook::location_platform::location::algorithms::ElevationType::CAMERA};
		};

        /// which camera is used for SLAM
		void setInput(Media::FrameMediumRefs&& frameMediums) override;

        /// this function gets called for each new anchor location
		ObjectId registerObject(const std::string& description, const Vector3& dimension) override;

		/**
		 * Returns the name of the owner library.
		 * @see Devices::library().
		 */
		const std::string& library() const override;

        bool isStarted() const override;
        bool start() override;
        bool stop() override;

		/**
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceName();

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceType();

		/**
		 * Determines whether or not to ignore GPS and use an overrided location source.
		 * @see overrideLocation() to update location data.
		 */
		bool shouldOverrideGPS = false;

		/**
		 * Determines when first relocalization (if overriding GPS) is successful to start world tracker.
		 * @see onWorldTrackerSample() implementation.
		 */
		bool hasFirstReloc = false;

		/**
		 * Function to override location source input.
		 * @param location A vector containing latitude, longitude, and bearing.
		 * @param timestamp The timestamp at which the location was measured.
		 * @see shouldOverrideGPS - must be set to true in order for the location value to be used.
		*/
		void overrideLocation(const VectorD3& location, Timestamp timestamp);

	private:

        GeoAnchorTracker6DOF();
        ~GeoAnchorTracker6DOF() override;

        // input event function for GPS locations
        void onGPSTrackerSample(const Measurement* sender, const SampleRef& sample);

        // input event function for 6-DOF world tracking poses
        void onWorldTrackerSample(const Measurement* sender, const SampleRef& sample);

        // output function to send anchor 6-DOF poses out for "customers"
        void reportAnchorPoses(const Timestamp& timestamp /* TODO some location service data*/);

		/**
		 * Creates a new GeoAnchorTracker6DOF device object.
		 * This function will be called automatically once the device is requested.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the must be valid
		 * @return The new device
		 */
		static Device* create(const std::string& name, const Device::DeviceType& deviceType);

	private:

		/// The name of the library providing this device.
		std::string libraryName_;

		/// The 6-DOF world tracker.
		Devices::Tracker6DOFRef worldTracker_;

		/// The GPS tracker.
		Devices::GPSTrackerRef gpsTracker_;

		Sophus::SE3f cameraPose_;
		HomogenousMatrix4 world_T_camera_;

		std::unique_ptr<facebook::location_platform::location::geoanchor::GeoAnchorManager>
			geoAnchorManager_ =
				std::make_unique<facebook::location_platform::location::geoanchor::GeoAnchorManager>(
					kVpsGeoAnchorConfig, kVpsLsqConfig);

		std::unordered_set<ObjectId> geoAnchorsToAdd;

		/// The subscription object for samples events from the world tracker.
		SampleEventSubscription worldTrackerSampleSubscription_;

		/// The subscription object for samples events from the gps tracker.
		SampleEventSubscription gpsTrackerSampleSubscription_;

		/// The lock to make object thread-safe.
		Lock lock_;
};

inline std::string GeoAnchorTracker6DOF::deviceName()
{
	return std::string("GeoAnchor 6DOF Tracker");
}

inline GeoAnchorTracker6DOF::DeviceType GeoAnchorTracker6DOF::deviceType()
{
	return GeoAnchorTracker6DOF::DeviceType(DEVICE_TRACKER, TRACKER_6DOF | TRACKER_VISUAL);
}


}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_PROTOTYPE_DEVICES_H
