// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_VRS_VRS_VISUAL_OBJECT_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_VRS_VRS_VISUAL_OBJECT_TRACKER_6_DOF_H

#include "ocean/devices/vrs/VRS.h"
#include "ocean/devices/vrs/VRSVisualTracker6DOF.h"

#include "ocean/devices/ObjectTracker.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

/**
 * This class implements a VRS 6-DOF tracker.
 * @ingroup devicesvrs
 */
class OCEAN_DEVICES_VRS_EXPORT VRSVisualObjectTracker6DOF :
	virtual public ObjectTracker,
	virtual public VRSVisualTracker6DOF
{
	friend class VRSFactory;

	public:

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeVRSVisualObjectTracker6DOF();

		/**
		 * Registers (adds) a new object to be tracked.
		 * @see ObjectTracker::regiserObject().
		 */
		ObjectId registerObject(const std::string& description, const Vector3& dimension) override;

		/**
		 * Unregisters (removes) a previously registered object.
		 * @see ObjectTracker::unregisterObject().
		 */
		bool unregisterObject(const ObjectId objectId) override;

	protected:

		/**
		 * Creates a new VRS 6-DOF tracker object.
         * @param name The name of the tracker
		 */
		explicit VRSVisualObjectTracker6DOF(const std::string& name);

		/**
		 * Destructs a new VRS 6-DOF tracker object.
		 */
		~VRSVisualObjectTracker6DOF() override;
};

inline VRSTracker6DOF::DeviceType VRSVisualObjectTracker6DOF::deviceTypeVRSVisualObjectTracker6DOF()
{
	return DeviceType(VRSVisualTracker6DOF::deviceTypeVRSVisualTracker6DOF(), TRACKER_OBJECT);
}

}

}

}

#endif // META_OCEAN_DEVICES_VRS_VRS_VISUAL_OBJECT_TRACKER_6_DOF_H
