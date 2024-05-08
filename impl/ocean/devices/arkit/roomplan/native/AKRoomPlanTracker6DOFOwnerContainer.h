/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_ROOMPLAN_NATIVE_AK_ROOM_PLAN_TRACKER_6_DOF_OWNER_CONTAINER_H
#define META_OCEAN_DEVICES_ARKIT_ROOMPLAN_NATIVE_AK_ROOM_PLAN_TRACKER_6_DOF_OWNER_CONTAINER_H

#include "ocean/devices/Devices.h"

#include "ocean/devices/arkit/roomplan/native/AKRoomPlanTracker6DOFBase.h"

#include "ocean/media/LiveVideo.h"

#include <ARKit/ARKit.h>
#include <Foundation/Foundation.h>

/**
 * This interface/class implements a container for C++ data structures.
 * The container is necessary as the Swift/Objec bridge does not allow to include C++ header files in ObjC header files in the target containing the bridge.
 * @ingroup devicesarkit
 */
@interface AKRoomPlanTracker6DOFOwnerContainer : NSObject

	/// The owner of the tracker.
	@property Ocean::Devices::ARKit::AKRoomPlanTracker6DOFBase* owner_;

	/// The live video which is used for tracking.
	@property Ocean::Media::LiveVideoRef inputLiveVideo_;

@end

#endif // META_OCEAN_DEVICES_ARKIT_ROOMPLAN_NATIVE_AK_ROOM_PLAN_TRACKER_6_DOF_OWNER_CONTAINER_H
