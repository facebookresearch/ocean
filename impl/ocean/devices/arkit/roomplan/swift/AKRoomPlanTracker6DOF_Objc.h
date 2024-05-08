/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_ROOMPLAN_SWIFT_AK_ROOM_PLAN_TRACKER_6_DOF_H
#define META_OCEAN_DEVICES_ARKIT_ROOMPLAN_SWIFT_AK_ROOM_PLAN_TRACKER_6_DOF_H

#include "ocean/devices/arkit/roomplan/swift/RoomElement.h"

#include <ARKit/ARKit.h>
#include <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

// Forward declaration to avoid including C++ headers in this file.
@class AKRoomPlanTracker6DOFOwnerContainer;

/**
 * This class implements the Objc wrapper for the room plan tracker.
 * @ingroup devicesarkit
 */
@interface AKRoomPlanTracker6DOF_Objc : NSObject

/**
 * Returns whether the tracker is supported on this platform.
 * @return True, if so
 */
+ (bool)isSupported;

/**
 * Starts the tracker.
 * @param ownerContainer The owner's delegate of this tracker
 * @return True, if succeeded
 */
- (bool)start:(AKRoomPlanTracker6DOFOwnerContainer*)ownerContainer;

/**
 * Stopps the tracker.
 * @return True, if succeeded
 */
- (bool)stop;

/**
 * Event function for frame update events.
 * @param session The sending session
 * @param frame The updated frame
 */
- (void)onSessionTest:(ARSession*)session didUpdateFrame:(ARFrame*)frame;

/**
 * Event function when the capture session has started.
 */
- (void)onCaptureSessionStarted;

/**
 * Event function for added room objects.
 * @param planarRoomObjects The new planar objects
 * @param volumetricRoomObjects The new volumetric objects
 */
- (void)onCaptureSessionAdded:(NSArray<PlanarRoomObject*>*)planarRoomObjects andVolumetric:(NSArray<VolumetricRoomObject*>*)volumetricRoomObjects;

/**
 * Event function for removed room objects.
 * @param planarRoomObjects The removed planar objects
 * @param volumetricRoomObjects The removed volumetric objects
 */
- (void)onCaptureSessionRemoved:(NSArray<PlanarRoomObject*>*)planarRoomObjects andVolumetric:(NSArray<VolumetricRoomObject*>*)volumetricRoomObjects;

/**
 * Event function for changed room objects.
 * @param planarRoomObjects The changed planar objects
 * @param volumetricRoomObjects The changed volumetric objects
 */
- (void)onCaptureSessionChanged:(NSArray<PlanarRoomObject*>*)planarRoomObjects andVolumetric:(NSArray<VolumetricRoomObject*>*)volumetricRoomObjects;

/**
 * Event function for updated room objects.
 * @param planarRoomObjects The updated planar objects
 * @param volumetricRoomObjects The updated volumetric objects
 */
- (void)onCaptureSessionUpdated:(NSArray<PlanarRoomObject*>*)planarRoomObjects andVolumetric:(NSArray<VolumetricRoomObject*>*)volumetricRoomObjects;

/**
 * Event function for instructions.
 * @param instruction The instruction which can be used to improve the capture result
 */
- (void)onCaptureSessionInstruction:(NSString*)instruction;

/**
 * Event function when the capture session has stopped.
 */
- (void)onCaptureSessionStopped;

@end

NS_ASSUME_NONNULL_END

#endif // META_OCEAN_DEVICES_ARKIT_ROOMPLAN_SWIFT_AK_ROOM_PLAN_TRACKER_6_DOF_H
