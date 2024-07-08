/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/roomplan/swift/AKRoomPlanTracker6DOF_Objc.h"

#include "ocean/devices/arkit/roomplan/native/AKRoomPlanTracker6DOFBase.h"
#include "ocean/devices/arkit/roomplan/native/AKRoomPlanTracker6DOFOwnerContainer.h"

#include "ocean/base/StringApple.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/media/LiveVideo.h"

#include "ocean/media/avfoundation/AVFLiveVideo.h"

#import <roomplan_swiftApple-Swift.h> // Swift bridge file will be auto generated

using namespace Ocean;
using namespace Ocean::Devices::ARKit;

@implementation AKRoomPlanTracker6DOF_Objc
{
	/// The actual swift-based tracker.
	API_AVAILABLE(ios(16.0)) AKRoomPlanTracker6DOF_Swift* tracker_;

	/// True, if the session is currently running.
	bool isRunning_;

	/// The owner's delegate of this tracker.
	AKRoomPlanTracker6DOFBase* owner_;

	/// The input medium to be used.
	Media::LiveVideoRef inputLiveVideo_;

	/// The trackers's lock.
	@public Lock lock_;
}

- (id)init
{
	if (self = [super init])
	{
		if (@available(iOS 16.0, *))
		{
			tracker_ = [AKRoomPlanTracker6DOF_Swift new];

			const bool result = [tracker_ setOwnerWithOwner:self];

			ocean_assert_and_suppress_unused(result, result);

			isRunning_ = false;
			owner_ = nullptr;
		}
	}

	return self;
}

+ (bool)isSupported
{
	if (@available(iOS 16.0, *))
	{
		return [AKRoomPlanTracker6DOF_Swift isSupported];
	}

	return false;
}

- (bool)start:(AKRoomPlanTracker6DOFOwnerContainer*)ownerContainer
{
	if (ownerContainer == nullptr)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (isRunning_)
	{
		ocean_assert(owner_ == ownerContainer.owner_);
		return true;
	}

	if (@available(iOS 16.0, *))
	{
		if (tracker_)
		{
			owner_ = ownerContainer.owner_;
			inputLiveVideo_ = ownerContainer.inputLiveVideo_;

			ocean_assert(owner_ != nullptr && inputLiveVideo_);

			if (owner_ != nullptr && inputLiveVideo_)
			{
				isRunning_ = [tracker_ start];
			}
		}
	}

	return isRunning_;
}

- (bool)stop
{
	const ScopedLock scopedLock(lock_);

	if (!isRunning_)
	{
		return true;
	}

	if (@available(iOS 16.0, *))
	{
		if (tracker_)
		{
			isRunning_ = ![tracker_ stop];
		}
	}

	return !isRunning_;
}

- (void)onSessionTest:(ARSession*)session didUpdateFrame:(ARFrame*)frame
{
	const NSTimeInterval uptime = [NSProcessInfo processInfo].systemUptime;
	const NSTimeInterval unixTimestamp = [[NSDate date] timeIntervalSince1970];

	const NSTimeInterval frameUptime = frame.timestamp;
	const NSTimeInterval frameUnixTimestamp = frameUptime - uptime + unixTimestamp;

	const CVPixelBufferRef capturedImage = frame.capturedImage;

	// when starting the AR session, AVFoundation looses access to the camera stream
	// therefore, we forward the camera data from the AR session to the AVFoundation live video object (so that the media object can be used as before)

	const simd_float3x3 simdIntrinsics = frame.camera.intrinsics;

	SquareMatrixF3 cameraIntrinsics;
	memcpy(cameraIntrinsics.data() + 0, &simdIntrinsics.columns[0], sizeof(float) * 3);
	memcpy(cameraIntrinsics.data() + 3, &simdIntrinsics.columns[1], sizeof(float) * 3);
	memcpy(cameraIntrinsics.data() + 6, &simdIntrinsics.columns[2], sizeof(float) * 3);

	const int width = NumericD::round32(frame.camera.imageResolution.width);
	const int height = NumericD::round32(frame.camera.imageResolution.height);

	const ScopedLock scopedLock(lock_);

	if (!isRunning_)
	{
		return;
	}

	SharedAnyCamera anyCamera;

	if (width > 0 && height > 0)
	{
		anyCamera = std::make_shared<AnyCameraPinhole>(PinholeCamera(SquareMatrix3(cameraIntrinsics), (unsigned int)(width), (unsigned int)(height)));

		ocean_assert(inputLiveVideo_);
		inputLiveVideo_.force<Media::AVFoundation::AVFLiveVideo>().feedNewSample(capturedImage, anyCamera, frameUnixTimestamp, frameUptime);
	}

	HomogenousMatrix4 world_T_camera(false);
	if (frame.camera.trackingState == ARTrackingStateNormal)
	{
		const HomogenousMatrixF4 rotatedWorld_T_cameraF([AKRoomPlanTracker6DOF_Objc extractTransform4x4:frame.camera.transform]);

		world_T_camera = HomogenousMatrix4(rotatedWorld_T_cameraF);
	}

	const Timestamp timestamp(frameUnixTimestamp);

	ocean_assert(owner_ != nullptr);
	owner_->onNewSample(world_T_camera, timestamp, frame);
}

- (void)onCaptureSessionStarted
{
	const ScopedLock scopedLock(lock_);

	if (!isRunning_)
	{
		return;
	}

	ocean_assert(owner_ != nullptr);
	owner_->onCaptureSessionStarted();
}

- (void)onCaptureSessionAdded:(NSArray<PlanarRoomObject*>*)planarRoomObjects andVolumetric:(NSArray<VolumetricRoomObject*>*)volumetricRoomObjects;
{
	const ScopedLock scopedLock(lock_);

	if (!isRunning_)
	{
		return;
	}

	ocean_assert(owner_ != nullptr);
	owner_->onCaptureSessionAdded([AKRoomPlanTracker6DOF_Objc extractPlanarRoomObjects:planarRoomObjects], [AKRoomPlanTracker6DOF_Objc extractVolumetricRoomObjects:volumetricRoomObjects]);
}

- (void)onCaptureSessionRemoved:(NSArray<PlanarRoomObject*>*)planarRoomObjects andVolumetric:(NSArray<VolumetricRoomObject*>*)volumetricRoomObjects;
{
	const ScopedLock scopedLock(lock_);

	if (!isRunning_)
	{
		return;
	}

	ocean_assert(owner_ != nullptr);
	owner_->onCaptureSessionRemoved([AKRoomPlanTracker6DOF_Objc extractPlanarRoomObjects:planarRoomObjects], [AKRoomPlanTracker6DOF_Objc extractVolumetricRoomObjects:volumetricRoomObjects]);
}

- (void)onCaptureSessionChanged:(NSArray<PlanarRoomObject*>*)planarRoomObjects andVolumetric:(NSArray<VolumetricRoomObject*>*)volumetricRoomObjects;
{
	const ScopedLock scopedLock(lock_);

	if (!isRunning_)
	{
		return;
	}

	ocean_assert(owner_ != nullptr);
	owner_->onCaptureSessionChanged([AKRoomPlanTracker6DOF_Objc extractPlanarRoomObjects:planarRoomObjects], [AKRoomPlanTracker6DOF_Objc extractVolumetricRoomObjects:volumetricRoomObjects]);
}

- (void)onCaptureSessionUpdated:(NSArray<PlanarRoomObject*>*)planarRoomObjects andVolumetric:(NSArray<VolumetricRoomObject*>*)volumetricRoomObjects;
{
	const ScopedLock scopedLock(lock_);

	if (!isRunning_)
	{
		return;
	}

	ocean_assert(owner_ != nullptr);
	owner_->onCaptureSessionUpdated([AKRoomPlanTracker6DOF_Objc extractPlanarRoomObjects:planarRoomObjects], [AKRoomPlanTracker6DOF_Objc extractVolumetricRoomObjects:volumetricRoomObjects]);
}

- (void)onCaptureSessionInstruction:(NSString*)instruction
{
	const ScopedLock scopedLock(lock_);

	if (!isRunning_)
	{
		return;
	}

	AKRoomPlanTracker6DOFBase::InstructionValue instructionValue = AKRoomPlanTracker6DOFBase::IV_UNKNOWN;

	if ([instruction isEqualToString:@"moveCloseToWall"])
	{
		instructionValue = AKRoomPlanTracker6DOFBase::IV_MOVE_CLOSE_TO_WALL;
	}
	else if ([instruction isEqualToString:@"moveAwayFromWall"])
	{
		instructionValue = AKRoomPlanTracker6DOFBase::IV_MOVE_AWAY_FROM_WALL;
	}
	else if ([instruction isEqualToString:@"slowDown"])
	{
		instructionValue = AKRoomPlanTracker6DOFBase::IV_SLOW_DOWN;
	}
	else if ([instruction isEqualToString:@"turnOnLight"])
	{
		instructionValue = AKRoomPlanTracker6DOFBase::IV_TURN_ON_LIGHT;
	}
	else if ([instruction isEqualToString:@"normal"])
	{
		instructionValue = AKRoomPlanTracker6DOFBase::IV_NORMAL;
	}
	else if ([instruction isEqualToString:@"lowTexture"])
	{
		instructionValue = AKRoomPlanTracker6DOFBase::IV_LOW_TEXTURE;
	}
	else
	{
		Log::error() << "AKRoomPlanTracker6DOF: Unknown instruction";
		ocean_assert(false && "AKRoomPlanTracker6DOF: Unknown instruction");
	}

	ocean_assert(owner_ != nullptr);
	owner_->onCaptureSessionInstruction(instructionValue);
}

- (void)onCaptureSessionStopped
{
	const ScopedLock scopedLock(lock_);

	if (!isRunning_)
	{
		return;
	}

	ocean_assert(owner_ != nullptr);
	owner_->onCaptureSessionStopped();
}

+ (HomogenousMatrixF4) extractTransform4x4:(simd_float4x4)transform
{
	HomogenousMatrixF4 matrix(false);
	memcpy(matrix.data() +  0, &transform.columns[0], sizeof(float) * 4);
	memcpy(matrix.data() +  4, &transform.columns[1], sizeof(float) * 4);
	memcpy(matrix.data() +  8, &transform.columns[2], sizeof(float) * 4);
	memcpy(matrix.data() + 12, &transform.columns[3], sizeof(float) * 4);

	return matrix;
}

+ (AKRoomPlanTracker6DOFBase::SharedPlanarRoomObjects) extractPlanarRoomObjects:(NSArray<PlanarRoomObject*>*)planarRoomObjects
{
	AKRoomPlanTracker6DOFBase::SharedPlanarRoomObjects internalPlanarRoomObjects;
	internalPlanarRoomObjects.reserve(planarRoomObjects.count);

	for (PlanarRoomObject* object in planarRoomObjects)
	{
		std::string identifier = StringApple::toUTF8(object.identifier_);

		AKRoomPlanTracker6DOFBase::PlanarRoomObject::PlanarType planarType = [AKRoomPlanTracker6DOF_Objc translatePlanarType:object.type_];

		const float confidence = object.confidence_;

		const HomogenousMatrixF4 world_T_object = [AKRoomPlanTracker6DOF_Objc extractTransform4x4:object.transform_];
		const VectorF3 dimension(object.dimension_[0], object.dimension_[1], object.dimension_[2]);

		std::shared_ptr<AKRoomPlanTracker6DOFBase::PlanarRoomObject> planarRoomObject = std::make_shared<AKRoomPlanTracker6DOFBase::PlanarRoomObject>(std::move(identifier), planarType, confidence, world_T_object, dimension);

		internalPlanarRoomObjects.emplace_back(std::move(planarRoomObject));
	}

	return internalPlanarRoomObjects;
}

+ (AKRoomPlanTracker6DOFBase::SharedVolumetricRoomObjects) extractVolumetricRoomObjects:(NSArray<VolumetricRoomObject*>*)volumetricRoomObjects
{
	AKRoomPlanTracker6DOFBase::SharedVolumetricRoomObjects internalVolumetricRoomObjects;
	internalVolumetricRoomObjects.reserve(volumetricRoomObjects.count);

	for (VolumetricRoomObject* object in volumetricRoomObjects)
	{
		std::string identifier = StringApple::toUTF8(object.identifier_);

		AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VolumetricType volumetricType = [AKRoomPlanTracker6DOF_Objc translateVolumetricType:object.type_];

		const float confidence = object.confidence_;

		const HomogenousMatrixF4 world_T_object = [AKRoomPlanTracker6DOF_Objc extractTransform4x4:object.transform_];
		const VectorF3 dimension(object.dimension_[0], object.dimension_[1], object.dimension_[2]);

		std::shared_ptr<AKRoomPlanTracker6DOFBase::VolumetricRoomObject> volumetricRoomObject = std::make_shared<AKRoomPlanTracker6DOFBase::VolumetricRoomObject>(std::move(identifier), volumetricType, confidence, world_T_object, dimension);

		internalVolumetricRoomObjects.emplace_back(std::move(volumetricRoomObject));
	}

	return internalVolumetricRoomObjects;
}

+ (AKRoomPlanTracker6DOFBase::PlanarRoomObject::PlanarType) translatePlanarType:(NSString*)planarType
{
	typedef std::unordered_map<std::string, AKRoomPlanTracker6DOFBase::PlanarRoomObject::PlanarType> TypeMap;

	const static TypeMap typeMap =
	{
		{"wall", AKRoomPlanTracker6DOFBase::PlanarRoomObject::PT_WALL},
		{"door", AKRoomPlanTracker6DOFBase::PlanarRoomObject::PT_DOOR},
		{"window", AKRoomPlanTracker6DOFBase::PlanarRoomObject::PT_WINDOW},
		{"opening", AKRoomPlanTracker6DOFBase::PlanarRoomObject::PT_OPENING},
		{"floor", AKRoomPlanTracker6DOFBase::PlanarRoomObject::PT_FLOOR}
	};

	ocean_assert(typeMap.size() + 1 == AKRoomPlanTracker6DOFBase::PlanarRoomObject::PT_END);

	const TypeMap::const_iterator i = typeMap.find(StringApple::toUTF8(planarType));

	if (i == typeMap.cend())
	{
		ocean_assert(false && "Unknown planar type!");
		return AKRoomPlanTracker6DOFBase::PlanarRoomObject::PT_UNKNOWN;
	}

	return i->second;
}

+ (AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VolumetricType) translateVolumetricType:(NSString*)volumetricType
{
	typedef std::unordered_map<std::string, AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VolumetricType> TypeMap;

	const static TypeMap typeMap =
	{
		{"storage", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_STORAGE},
		{"refrigerator", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_REFRIGERATOR},
		{"stove", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_STOVE},
		{"bed", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_BED},
		{"sink", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_SINK},
		{"washerDryer", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_WASHER_DRYER},
		{"toilet", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_TOILET},
		{"bathtub", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_BATHTUB},
		{"oven", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_OVEN},
		{"dishwasher", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_DISHWASHER},
		{"table", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_TABLE},
		{"sofa", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_SOFA},
		{"chair", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_CHAIR},
		{"fireplace", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_FIREPLACE},
		{"television", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_TELEVISION},
		{"stairs", AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_STAIRS}
	};

	ocean_assert(typeMap.size() + 1 == AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_END);

	const TypeMap::const_iterator i = typeMap.find(StringApple::toUTF8(volumetricType));

	if (i == typeMap.cend())
	{
		ocean_assert(false && "Unknown volumetric type!");
		return AKRoomPlanTracker6DOFBase::VolumetricRoomObject::VT_UNKNOWN;
	}

	return i->second;
}

@end
