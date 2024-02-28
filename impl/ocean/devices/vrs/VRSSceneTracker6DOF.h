// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_DEVICES_VRS_VRS_SCENE_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_VRS_VRS_SCENE_TRACKER_6_DOF_H

#include "ocean/devices/vrs/VRS.h"
#include "ocean/devices/vrs/VRSTracker6DOF.h"

#include "ocean/devices/SceneTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace VRS
{

// Forward declaration.
class VRSSceneTracker6DOF;

/**
 * Definition of a smart object reference for a VRS 6DOF scene tracker.
 * @see VRSSceneTracker6DOF.
 * @ingroup devices
 */
typedef SmartDeviceRef<VRSSceneTracker6DOF> VRSSceneTracker6DOFRef;

/**
 * This class implements a VRS 6-DOF scene tracker.
 * @ingroup devicesvrs
 */
class OCEAN_DEVICES_VRS_EXPORT VRSSceneTracker6DOF :
	virtual public SceneTracker6DOF,
	virtual public VRSTracker6DOF
{
	friend class VRSFactory;

	public:

		/**
		 * Forwards sample events.
		 * @param internalObjectIds The ids of all (internal) objects for which a sample is known
		 * @param orientations The orientations of the sample, one for each object id
		 * @param positions The positions of the sample, one for each object id
		 * @param sceneElements The scene elements to forward
		 * @param referenceSystem The reference system of the sample
		 * @param timestamp The timestamp of the event
		 * @param metadata Optional metadata of the sample
		 */
		void forwardSampleEvent(const ObjectIds& internalObjectIds, Quaternions&& orientations, Vectors3&& positions, SceneTracker6DOF::SharedSceneElements&& sceneElements, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata);

		/**
		 * Returns the type of this tracker.
		 * @return Tracker type
		 */
		static inline DeviceType deviceTypeVRSSceneTracker6DOF();

	protected:

		/**
		 * Creates a new VRS 6-DOF scene tracker object.
         * @param name The name of the tracker
		 * @param type The type of the new tracker
		 */
		explicit VRSSceneTracker6DOF(const std::string& name, const DeviceType& type);

		/**
		 * Destructs a new VRS 6-DOF scene tracker object.
		 */
		~VRSSceneTracker6DOF() override;
};

inline VRSSceneTracker6DOF::DeviceType VRSSceneTracker6DOF::deviceTypeVRSSceneTracker6DOF()
{
	return deviceTypeSceneTracker6DOF();
}

}

}

}

#endif // META_OCEAN_DEVICES_VRS_VRS_SCENE_TRACKER_6_DOF_H
