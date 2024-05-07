// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_DEVICES_BLOB_TRACKER_6DOF_H
#define META_OCEAN_DEVICES_BLOB_TRACKER_6DOF_H

#include "ocean/devices/blob/Blob.h"
#include "ocean/devices/blob/BlobDevice.h"

#include "ocean/base/Thread.h"

#include "ocean/devices/ObjectTracker.h"
#include "ocean/devices/Tracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/geometry/Geometry.h"

#include "ocean/tracking/blob/BlobTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Blob
{

/**
 * This class implements an Blob feature based tracker.
 * @ingroup devicesblob
 */
class OCEAN_DEVICES_BLOB_EXPORT BlobTracker6DOF :
	virtual public BlobDevice,
	virtual public Tracker6DOF,
	virtual public ObjectTracker,
	virtual public VisualTracker,
	protected Thread
{
	friend class BlobFactory;

	public:

		/**
		 * Adds a new tracking pattern.
		 * For this blob feature based tracker the pattern must be the url of an image.
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
		 * Returns whether a specific object is currently actively tracked by this tracker.
		 * @see Tracker::isObjectTracked().
		 */
		bool isObjectTracked(const ObjectId& objectId) const override;

		/**
		 * Returns the name of this tracker.
		 * @return Tracker name
		 */
		static inline std::string deviceNameBlobTracker6DOF();

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeBlobTracker6DOF();

	private:

		/**
		 * Creates a new Blob feature based 6DOF tracker object.
		 */
		BlobTracker6DOF();

		/**
		 * Destructs an Blob feature based 6DOF tracker object.
		 */
		~BlobTracker6DOF() override;

		/**
		 * Thread function.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	private:

		/// Frame timestamp.
		Timestamp frameTimestamp_;

		/// Internal 6DOF tracker object.
		Tracking::Blob::BlobTracker6DOF featureTracker_;

		/// The unique object id of the pattern.
		ObjectId patternObjectId_ = invalidObjectId();
};

inline std::string BlobTracker6DOF::deviceNameBlobTracker6DOF()
{
	return std::string("Blob Feature Based 6DOF Tracker");
}

inline BlobTracker6DOF::DeviceType BlobTracker6DOF::deviceTypeBlobTracker6DOF()
{
	return BlobTracker6DOF::DeviceType(BlobTracker6DOF::deviceTypeTracker6DOF(), TRACKER_VISUAL | TRACKER_OBJECT);
}

}

}

}

#endif // META_OCEAN_DEVICES_BLOB_TRACKER_6DOF_H
