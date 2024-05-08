/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// clang-format off
// @nolint

import ARKit
import Foundation
import RoomPlan

@available(iOS 16.0, *)
@objc public class AKRoomPlanTracker6DOF_Swift : NSObject, RoomCaptureSessionDelegate, ARSessionDelegate
{
	@objc public override init()
	{
		super.init()

		if (RoomCaptureSession.isSupported)
		{
			roomCaptureSession_ = RoomCaptureSession()
		}
	}

	@objc public func setOwner(owner : AKRoomPlanTracker6DOF_Objc) -> Bool
	{
		if (owner_ == nil)
		{
			owner_ = owner
			return true
		}

		return false
	}

	@objc public func isValid() -> Bool
	{
		return roomCaptureSession_ != nil;
	}

	@objc public func start() -> Bool
	{
		if (roomCaptureSession_ != nil)
		{
			var roomCaptureConfiguration = RoomCaptureSession.Configuration()
			roomCaptureConfiguration.isCoachingEnabled = true;

			roomCaptureSession_?.run(configuration: roomCaptureConfiguration)

			roomCaptureSession_?.delegate = self
			roomCaptureSession_?.arSession.delegate = self

			return true
		}

		return false
	}

	@objc public func stop() -> Bool
	{
		if (roomCaptureSession_ != nil)
		{
			roomCaptureSession_?.stop()

			return true
		}

		return false
	}

	public func captureSession(_ roomCaptureSession : RoomCaptureSession, didStartWith configuration: RoomCaptureSession.Configuration)
	{
		if (owner_ == nil)
		{
			return;
		}

		owner_?.onCaptureSessionStarted()
	}

	public func captureSession(_ roomCaptureSession : RoomCaptureSession, didAdd: CapturedRoom)
	{
		if (owner_ == nil)
		{
			return;
		}

		var planarRoomObjects = Array<PlanarRoomObject>();
		var volumetricRoomObjects = Array<VolumetricRoomObject>();
		extractRoomObjectFromCaptureRoom(capturedRoom:didAdd, planarRoomObjects:&planarRoomObjects, volumetricRoomObjects:&volumetricRoomObjects)

		if !planarRoomObjects.isEmpty || !volumetricRoomObjects.isEmpty
		{
			owner_?.onCaptureSessionAdded(planarRoomObjects, andVolumetric:volumetricRoomObjects)
		}
	}

	public func captureSession(_ roomCaptureSession : RoomCaptureSession, didRemove: CapturedRoom)
	{
		if (owner_ == nil)
		{
			return;
		}

		var planarRoomObjects = Array<PlanarRoomObject>();
		var volumetricRoomObjects = Array<VolumetricRoomObject>();
		extractRoomObjectFromCaptureRoom(capturedRoom:didRemove, planarRoomObjects:&planarRoomObjects, volumetricRoomObjects:&volumetricRoomObjects)

		if !planarRoomObjects.isEmpty || !volumetricRoomObjects.isEmpty
		{
			owner_?.onCaptureSessionRemoved(planarRoomObjects, andVolumetric:volumetricRoomObjects)
		}
	}

	public func captureSession(_ roomCaptureSession : RoomCaptureSession, didChange: CapturedRoom)
	{
		if (owner_ == nil)
		{
			return;
		}

		var planarRoomObjects = Array<PlanarRoomObject>();
		var volumetricRoomObjects = Array<VolumetricRoomObject>();
		extractRoomObjectFromCaptureRoom(capturedRoom:didChange, planarRoomObjects:&planarRoomObjects, volumetricRoomObjects:&volumetricRoomObjects)

		if !planarRoomObjects.isEmpty || !volumetricRoomObjects.isEmpty
		{
			owner_?.onCaptureSessionChanged(planarRoomObjects, andVolumetric:volumetricRoomObjects)
		}
	}

	public func captureSession(_ roomCaptureSession : RoomCaptureSession, didUpdate: CapturedRoom)
	{
		if (owner_ == nil)
		{
			return;
		}

		var planarRoomObjects = Array<PlanarRoomObject>();
		var volumetricRoomObjects = Array<VolumetricRoomObject>();
		extractRoomObjectFromCaptureRoom(capturedRoom:didUpdate, planarRoomObjects:&planarRoomObjects, volumetricRoomObjects:&volumetricRoomObjects)

		if !planarRoomObjects.isEmpty || !volumetricRoomObjects.isEmpty
		{
			owner_?.onCaptureSessionUpdated(planarRoomObjects, andVolumetric:volumetricRoomObjects)
		}
	}

	public func captureSession(_ roomCaptureSession : RoomCaptureSession, didProvide: RoomCaptureSession.Instruction)
	{
		if (owner_ == nil)
		{
			return;
		}

		var instruction : String

		switch didProvide
		{
			case RoomCaptureSession.Instruction.moveCloseToWall:
				instruction = "moveCloseToWall";

			case RoomCaptureSession.Instruction.moveAwayFromWall:
				instruction = "moveAwayFromWall";

			case RoomCaptureSession.Instruction.slowDown:
				instruction = "slowDown";

			case RoomCaptureSession.Instruction.turnOnLight:
				instruction = "turnOnLight";

			case RoomCaptureSession.Instruction.normal:
				instruction = "normal";

			case RoomCaptureSession.Instruction.lowTexture:
				instruction = "lowTexture";

			@unknown default:
				instruction = "";
		}

		owner_?.onCaptureSessionInstruction(instruction)
	}

	public func captureSession(_ roomCaptureSession : RoomCaptureSession, didEndWith: CapturedRoomData, error: Error?)
	{
		if (owner_ == nil)
		{
			return;
		}

		owner_?.onCaptureSessionStopped()
	}

	public func session(_ session: ARSession, didUpdate frame: ARFrame)
	{
		if (owner_ == nil)
		{
			return;
		}

		owner_?.onSessionTest(session, didUpdate:frame)
	}

	@objc public class func isSupported() -> Bool
	{
		return RoomCaptureSession.isSupported
	}

	private func extractRoomObjectFromCaptureRoom(capturedRoom : CapturedRoom, planarRoomObjects : inout Array<PlanarRoomObject>, volumetricRoomObjects : inout Array<VolumetricRoomObject>)
	{
		extractPlanarRoomObjectFromSurfaces(surfaces:capturedRoom.walls, roomObjects:&planarRoomObjects)
		extractPlanarRoomObjectFromSurfaces(surfaces:capturedRoom.doors, roomObjects:&planarRoomObjects)
		extractPlanarRoomObjectFromSurfaces(surfaces:capturedRoom.windows, roomObjects:&planarRoomObjects)
		extractPlanarRoomObjectFromSurfaces(surfaces:capturedRoom.openings, roomObjects:&planarRoomObjects)

#if swift(>=5.9) // can be removed once Xcode 15.0 is used
		if #available(iOS 17.0, *)
		{
			extractPlanarRoomObjectFromSurfaces(surfaces:capturedRoom.floors, roomObjects:&planarRoomObjects)
		}
#endif

		extractVolumetricRoomObjectFromObjects(objects:capturedRoom.objects, roomObjects:&volumetricRoomObjects)
	}

	private func extractPlanarRoomObjectFromSurfaces(surfaces : [CapturedRoom.Surface], roomObjects : inout Array<PlanarRoomObject>)
	{
		for surface in surfaces
		{
			let roomObject = PlanarRoomObject();

			switch surface.category
			{
				case CapturedRoom.Surface.Category.wall:
					roomObject.type_ = "wall"

				case CapturedRoom.Surface.Category.door:
					roomObject.type_ = "door"

				case CapturedRoom.Surface.Category.window:
					roomObject.type_ = "window"

				case CapturedRoom.Surface.Category.opening:
					roomObject.type_ = "opening"

				default:
					roomObject.type_ = ""

#if swift(>=5.9) // can be removed once Xcode 15.0 is used
					if #available(iOS 17.0, *)
					{
						if (surface.category == CapturedRoom.Surface.Category.floor)
						{
							roomObject.type_ = "floor"
						}
					}
#endif // swift(>=5.9)
			}

			var confidence : Float = 0.0

			switch surface.confidence
			{
				case CapturedRoom.Confidence.low:
					confidence = 0.2

				case CapturedRoom.Confidence.medium:
					confidence = 0.5

				case CapturedRoom.Confidence.high:
					confidence = 1.0

				@unknown default:
					confidence = 0.0
			}

			roomObject.identifier_ = surface.identifier.uuidString
			roomObject.confidence_ = confidence;
			roomObject.transform_ = surface.transform;
			roomObject.dimension_ = surface.dimensions;

			roomObjects.append(roomObject);
		}
	}

	private func extractVolumetricRoomObjectFromObjects(objects : [CapturedRoom.Object], roomObjects : inout Array<VolumetricRoomObject>)
	{
		for object in objects
		{
			let roomObject = VolumetricRoomObject();

			switch (object.category)
			{
				case CapturedRoom.Object.Category.storage:
					roomObject.type_ = "storage"

				case CapturedRoom.Object.Category.refrigerator:
					roomObject.type_ = "refrigerator"

				case CapturedRoom.Object.Category.stove:
					roomObject.type_ = "stove"

				case CapturedRoom.Object.Category.bed:
					roomObject.type_ = "bed"

				case CapturedRoom.Object.Category.sink:
					roomObject.type_ = "sink"

				case CapturedRoom.Object.Category.washerDryer:
					roomObject.type_ = "washerDryer"

				case CapturedRoom.Object.Category.toilet:
					roomObject.type_ = "toilet"

				case CapturedRoom.Object.Category.bathtub:
					roomObject.type_ = "bathtub"

				case CapturedRoom.Object.Category.oven:
					roomObject.type_ = "oven"

				case CapturedRoom.Object.Category.dishwasher:
					roomObject.type_ = "dishwasher"

				case CapturedRoom.Object.Category.table:
					roomObject.type_ = "table"

				case CapturedRoom.Object.Category.sofa:
					roomObject.type_ = "sofa"

				case CapturedRoom.Object.Category.chair:
					roomObject.type_ = "chair"

				case CapturedRoom.Object.Category.fireplace:
					roomObject.type_ = "fireplace"

				case CapturedRoom.Object.Category.television:
					roomObject.type_ = "television"

				case CapturedRoom.Object.Category.stairs:
					roomObject.type_ = "stairs"

				@unknown default:
					roomObject.type_ = ""
			}

			var confidence : Float = 0.0

			switch object.confidence
			{
				case CapturedRoom.Confidence.low:
					confidence = 0.2

				case CapturedRoom.Confidence.medium:
					confidence = 0.5

				case CapturedRoom.Confidence.high:
					confidence = 1.0

				@unknown default:
					confidence = 0.0
			}

			roomObject.identifier_ = object.identifier.uuidString
			roomObject.confidence_ = confidence;
			roomObject.transform_ = object.transform
			roomObject.dimension_ = object.dimensions

			roomObjects.append(roomObject);
		}
	}

	/// The Room capture session
	var roomCaptureSession_ : RoomCaptureSession?

	/// The ObjC-based owner of this Swift-based tracker.
	var owner_ : AKRoomPlanTracker6DOF_Objc?
}
