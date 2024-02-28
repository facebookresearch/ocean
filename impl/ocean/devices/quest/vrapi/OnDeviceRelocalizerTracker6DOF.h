// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_QUEST_VRAPI_ON_DEVICE_RELOCALIZER_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_QUEST_VRAPI_ON_DEVICE_RELOCALIZER_TRACKER_6DOF_H

#include "ocean/devices/quest/vrapi/VrApi.h"
#include "ocean/devices/quest/vrapi/VrApiDevice.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/io/Bitstream.h"

#include "ocean/tracking/SmoothedTransformation.h"

#include "ocean/tracking/mapbuilding/TrackerStereo.h"

namespace Ocean
{

namespace Devices
{

namespace Quest
{

namespace VrApi
{

/**
 * This class implements an Quest 6DOF on device relocalizer tracker using VrApi.
 * @ingroup devicesquestvrapi
 */
class OCEAN_DEVICES_QUEST_VRAPI_EXPORT OnDeviceRelocalizerTracker6DOF :
	virtual public VrApiDevice,
	virtual public SceneTracker6DOF,
	virtual public ObjectTracker,
	virtual public VisualTracker,
	protected Thread
{
	friend class VrApiFactory;

	public:

		/**
		 * Adds a new tracking object.
		 * The description must be the url of a file containing the feature map
		 * @see ObjectTracker::registerObject().
		 */
		ObjectId registerObject(const std::string& description, const Vector3& dimension) override;

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
		 * Sets an abstract parameter of this device.
		 * Supprted parameters 'minimalNumberCorrespondences', 'maximalProjectionError'.
		 * @see Device::setParameter().
		 */
		bool setParameter(const std::string& parameter, const Value& value) override;

		/**
		 * Returns an abstract parameter of this device.
		 * Supprted parameters 'minimalNumberCorrespondences', 'maximalProjectionError'.
		 * @see Device::parameter().
		 */
		bool parameter(const std::string& parameter, Value& value) override;

		/**
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceNameOnDeviceRelocalizerTracker6DOF();

		/**
		 * Returns the device type of this tracker.
		 * @return Device type
		 */
		static inline DeviceType deviceTypeOnDeviceRelocalizerTracker6DOF();

	private:

		/**
		 * Creates a new floot tracker object.
		 */
		OnDeviceRelocalizerTracker6DOF();

		/**
		 * Destructs an floot tracker object.
		 */
		~OnDeviceRelocalizerTracker6DOF() override;

		/**
		 * Updates this controller object.
		 * @see QuestDevice::update().
		 */
		void update(ovrMobile* ovr, const Platform::Meta::Quest::Device::DeviceType deviceType, Platform::Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& timestamp) override;

		/**
		 * Thread function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

		/**
		 * Reads 3D object points from a bitstream.
		 * @param bitstream The input stream from which the points will be read
		 * @param objectPoints The resulting 3D object points
		 * @param objectPointIds The resulting ids of the object points, one for each object point
		 * @return True, if succeeded
		 */
		static bool readObjectPointsFromStream(IO::InputBitstream& bitstream, Vectors3& objectPoints, Indices32& objectPointIds);

	private:

		/// The map's unique id.
		ObjectId mapObjectId_ = invalidObjectId();

		/// True, if the map is currently tracked.
		std::atomic<bool> isMapTracked_ = false;

		/// The stereo tracker to be used to determine the 6-DOF pose.
		Tracking::MapBuilding::TrackerStereo trackerStereo_;

		/// The 3D object points of the relocalizer.
		Vectors3 objectPoints_;

		/// The ids of the object points of the relocalizer.
		Indices64 objectPointIds_;

		/// The latest OVR mobile instance.
		std::atomic<ovrMobile*> ovr_ = nullptr;

		/// The 7-DOF transformation between both worlds (6-DOF + 1D scale), if known.
		Tracking::SmoothedTransformation relocalizedWorld_T_headsetWorld_ = Tracking::SmoothedTransformation(1.0);

		/// The minimal number of feature coorespondences necessary to relocalizer, with range [4, infinity)
		unsigned int minimalNumberCorrespondences_ = 120u;

		/// The maximal projection error for a valid feature point, in pixel domain, with range [0, infinity)
		Scalar maximalProjectionError_ = Scalar(2.5);
};

inline std::string OnDeviceRelocalizerTracker6DOF::deviceNameOnDeviceRelocalizerTracker6DOF()
{
	return std::string("Quest On-Device Relocalizer 6DOF Tracker");
}

inline OnDeviceRelocalizerTracker6DOF::DeviceType OnDeviceRelocalizerTracker6DOF::deviceTypeOnDeviceRelocalizerTracker6DOF()
{
	return DeviceType(deviceTypeTracker6DOF(), TRACKER_VISUAL | TRACKER_OBJECT);
}

}

}

}

}

#endif // META_OCEAN_DEVICES_QUEST_VRAPI_ON_DEVICE_RELOCALIZER_TRACKER_6DOF_H
