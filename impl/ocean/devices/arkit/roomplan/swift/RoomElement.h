/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ARKIT_ROOMPLAN_SWIFT_ROOM_ELEMENT_H
#define META_OCEAN_DEVICES_ARKIT_ROOMPLAN_SWIFT_ROOM_ELEMENT_H

#include <ARKit/ARKit.h>
#include <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * Definition of the base class of all room objects.
 * @ingroup devicesarkit
 */
@interface RoomObject : NSObject

	/// The type of the object.
	@property NSString* type_;

	/// The unique identifier of the object.
	@property NSString* identifier_;

	/// The confidence of this object, with range [0, 1], higher is better;
	@property float confidence_;

	/// A matrix that defines the object’s position and orientation in the scene.
	@property simd_float4x4 transform_;

	/// A bounding box that contains the object.
	@property simd_float3 dimension_;

@end

/**
 * Definition of a planar (2D) room object.
 * @ingroup devicesarkit
 */
@interface PlanarRoomObject : RoomObject
@end

/**
 * Definition of a volumetric (3D) room object.
 * @ingroup devicesarkit
 */
@interface VolumetricRoomObject : RoomObject
@end

NS_ASSUME_NONNULL_END

#endif // META_OCEAN_DEVICES_ARKIT_ROOMPLAN_SWIFT_ROOM_ELEMENT_H
