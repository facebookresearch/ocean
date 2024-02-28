// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_VRS_VRS_VISUAL_VISUAL_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_VRS_VRS_VISUAL_VISUAL_TRACKER_6_DOF_H

#include "ocean/devices/vrs/VRS.h"
#include "ocean/devices/vrs/VRSTracker6DOF.h"

#include "ocean/devices/VisualTracker.h"

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
class OCEAN_DEVICES_VRS_EXPORT VRSVisualTracker6DOF :
	virtual public VisualTracker,
	virtual public VRSTracker6DOF
{
	friend class VRSFactory;

	public:

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeVRSVisualTracker6DOF();

	protected:

		/**
		 * Creates a new VRS 6-DOF tracker object.
         * @param name The name of the tracker
		 */
		explicit VRSVisualTracker6DOF(const std::string& name);

		/**
		 * Destructs a new VRS 6-DOF tracker object.
		 */
		~VRSVisualTracker6DOF() override;
};

inline VRSVisualTracker6DOF::DeviceType VRSVisualTracker6DOF::deviceTypeVRSVisualTracker6DOF()
{
	return DeviceType(VRSTracker6DOF::deviceTypeVRSTracker6DOF(), TRACKER_VISUAL);
}

}

}

}

#endif // META_OCEAN_DEVICES_VRS_VRS_VISUAL_VISUAL_TRACKER_6_DOF_H
