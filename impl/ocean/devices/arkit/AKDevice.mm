/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arkit/AKDevice.h"
#include "ocean/devices/arkit/AKDepthTracker6DOF.h"
#include "ocean/devices/arkit/AKFaceTracker6DOF.h"
#include "ocean/devices/arkit/AKGeoAnchorsTracker6DOF.h"
#include "ocean/devices/arkit/AKPlaneTracker6DOF.h"
#include "ocean/devices/arkit/AKSceneTracker6DOF.h"
#include "ocean/devices/arkit/AKWorldTracker6DOF.h"

#include "ocean/base/StringApple.h"

#include "ocean/math/AnyCamera.h"

#include "ocean/media/LiveVideo.h"

#include "ocean/media/avfoundation/AVFLiveVideo.h"

using namespace Ocean;
using namespace Ocean::Devices::ARKit;

API_AVAILABLE(ios(11.0)) // expect iOS 11.0 or higher

@implementation AKTracker6DOFDelegate
{
	/// The AR session.
	ARSession* arSession_;

	/// The AR configuration.
	ARConfiguration* arConfiguration_;

	/// The capabilities the current ARConfiguration supports.
	AKDevice::TrackerCapabilities trackerCapabilities_;

	/// True, if the session is currently running.
	bool isRunning_;

	/// The input medium to be used.
	Media::LiveVideoRef inputLiveVideo_;

	/// The map mapping devices to reference counters.
	std::unordered_map<AKDevice*, unsigned int> deviceMap_;

	/// The map mapping object ids to anchors.
	std::unordered_map<Devices::Measurement::ObjectId, ARAnchor*> anchorMap_;

	/// The last ARGeoTrackingState value.
	API_AVAILABLE(ios(14.0)) ARGeoTrackingState lastARGeoTrackingState_;

	/// The last ARGeoTrackingStateReason value.
	API_AVAILABLE(ios(14.0)) ARGeoTrackingStateReason lastARGeoTrackingStateReason_;

	/// The last ARGeoTrackingAccuracy value.
	API_AVAILABLE(ios(14.0)) ARGeoTrackingAccuracy lastARGeoTrackingAccuracy_;

	/// The delegate's lock.
	@public Lock lock_;
}

- (id)init
{
	if (self = [super init])
	{
		arSession_ = nullptr;
		arConfiguration_ = nullptr;

		trackerCapabilities_ = AKDevice::TC_INVALID;

		isRunning_ = false;

		if (@available(iOS 14.0, *))
		{
			lastARGeoTrackingState_ = ARGeoTrackingState(-1);
			lastARGeoTrackingStateReason_ = ARGeoTrackingStateReason(-1);
			lastARGeoTrackingAccuracy_ = ARGeoTrackingAccuracy(-1);
		}
	}

	return self;
}

- (bool)isRunning
{
	return isRunning_;
}

- (bool)restart:(AKDevice*)device withMedium:(const Media::LiveVideoRef&)inputLiveVideo
{
	if (device == nullptr || inputLiveVideo.isNull())
	{
		return false;
	}

#ifdef OCEAN_DEBUG
	for (std::unordered_map<AKDevice*, unsigned int>::const_iterator iDevice = deviceMap_.cbegin(); iDevice != deviceMap_.cend(); ++iDevice)
	{
		ocean_assert(iDevice->first == device || iDevice->first->name() != device->name());
	}
#endif

	const AKDevice::TrackerCapabilities necessaryTrackerCapabilities = device->trackerCapabilities();

	const ScopedLock scopedLock(lock_);

	if (inputLiveVideo_ && &*inputLiveVideo_ != &*inputLiveVideo)
	{
		Log::warning() << "ARKit has already been initialized with a different input medium";
		return false;
	}

	ocean_assert(ARWorldTrackingConfiguration.isSupported == TRUE);

	if (arSession_ == nil)
	{
		ocean_assert(isRunning_ == false);

		arSession_ = [ARSession new];

		if (arSession_ == nil)
		{
			return false;
		}

		arSession_.delegate = self;
	}

	bool runWithConfigurationNecessary = !isRunning_;

	if (arConfiguration_ == nullptr || (trackerCapabilities_ & necessaryTrackerCapabilities) != necessaryTrackerCapabilities)
	{
		trackerCapabilities_ = AKDevice::TC_INVALID;

		ocean_assert(inputLiveVideo);
		const bool useBackCamera = inputLiveVideo->url() == "Back Camera";

		arConfiguration_ = nullptr;

		if (useBackCamera)
		{
			if (necessaryTrackerCapabilities & AKDevice::TC_GEO_ANCHORS)
			{
				// on iOS 14 we may have access to SLAM tracking + geo anchors
				if (@available(iOS 14.0, *))
				{
					ocean_assert(ARGeoTrackingConfiguration.isSupported);

					if (ARGeoTrackingConfiguration.isSupported)
					{
						ARGeoTrackingConfiguration* arGeoTrackingConfiguration = [ARGeoTrackingConfiguration new];

#ifdef OCEAN_DEBUG
						Log::debug() << "Supported video formats for geo tracking:";

						for (size_t n = 0; n < ARGeoTrackingConfiguration.supportedVideoFormats.count; ++n)
						{
							ARVideoFormat* videoFormat = ARGeoTrackingConfiguration.supportedVideoFormats[n];

							if (@available(iOS 14.5, *))
							{
								Log::debug() << "" << StringApple::toUTF8(videoFormat.captureDeviceType) << ", " << int(videoFormat.imageResolution.width) << "x" << int(videoFormat.imageResolution.height);
							}
							else
							{
								Log::debug() << "" << int(videoFormat.imageResolution.width) << "x" << int(videoFormat.imageResolution.height);
							}
						}
#endif

						// let's try to select a video format with 720p - otherwise we use the default video format

						for (size_t n = 0; n < ARGeoTrackingConfiguration.supportedVideoFormats.count; ++n)
						{
							ARVideoFormat* videoFormat = ARGeoTrackingConfiguration.supportedVideoFormats[n];

							if (videoFormat.imageResolution.width == 1280.0f && videoFormat.imageResolution.height == 720.0f)
							{
								arGeoTrackingConfiguration.videoFormat = videoFormat;
								break;
							}
						}

						trackerCapabilities_ = Devices::ARKit::AKDevice::TrackerCapabilities(trackerCapabilities_ | Devices::ARKit::AKDevice::TC_GEO_ANCHORS);

						if (necessaryTrackerCapabilities & Devices::ARKit::AKDevice::TC_PLANE_DETECTION)
						{
							arGeoTrackingConfiguration.planeDetection = ARPlaneDetectionHorizontal | ARPlaneDetectionVertical;

							trackerCapabilities_ = Devices::ARKit::AKDevice::TrackerCapabilities(trackerCapabilities_ | Devices::ARKit::AKDevice::TC_PLANE_DETECTION);
						}

						if (necessaryTrackerCapabilities & Devices::ARKit::AKDevice::TC_MESH_RECONSTRUCTION)
						{
							Log::warning() << "ARKit's Geo Anchors cannot be combined with mesh reconstruction";
						}

						if (necessaryTrackerCapabilities & Devices::ARKit::AKDevice::TC_DEPTH)
						{
							if (@available(iOS 14.0, *))
							{
								if ([ARGeoTrackingConfiguration supportsFrameSemantics:ARFrameSemanticSceneDepth])
								{
									arGeoTrackingConfiguration.frameSemantics = ARFrameSemanticSceneDepth;
								}
							}
						}

						if (necessaryTrackerCapabilities & AKDevice::TC_FACE)
						{
							Log::warning() << "Face tracking is currently not supported";
						}

						arConfiguration_ = arGeoTrackingConfiguration;
					}
				}
			}

			if (arConfiguration_ == nullptr)
			{
				ARWorldTrackingConfiguration* arWorldTrackingConfiguration = [ARWorldTrackingConfiguration new];

				if (@available(iOS 11.3, *))
				{
#ifdef OCEAN_DEBUG
					Log::debug() << "Supported video formats for world tracking:";

					for (size_t n = 0; n < ARWorldTrackingConfiguration.supportedVideoFormats.count; ++n)
					{
						ARVideoFormat* videoFormat = ARWorldTrackingConfiguration.supportedVideoFormats[n];

						if (@available(iOS 14.5, *))
						{
							Log::debug() << "" << StringApple::toUTF8(videoFormat.captureDeviceType) << ", " << int(videoFormat.imageResolution.width) << "x" << int(videoFormat.imageResolution.height);
						}
						else
						{
							Log::debug() << "" << int(videoFormat.imageResolution.width) << "x" << int(videoFormat.imageResolution.height);
						}
					}
#endif
					// let's try to select a video format with 720p - otherwise we use the default video format

					for (size_t n = 0; n < ARWorldTrackingConfiguration.supportedVideoFormats.count; ++n)
					{
						ARVideoFormat* videoFormat = ARWorldTrackingConfiguration.supportedVideoFormats[n];

						if (videoFormat.imageResolution.width == 1280.0f && videoFormat.imageResolution.height == 720.0f)
						{
							arWorldTrackingConfiguration.videoFormat = videoFormat;
							break;
						}
					}
				}

				if (necessaryTrackerCapabilities & Devices::ARKit::AKDevice::TC_PLANE_DETECTION)
				{
					if (@available(iOS 11.3, *))
					{
						arWorldTrackingConfiguration.planeDetection = ARPlaneDetectionHorizontal | ARPlaneDetectionVertical;

						trackerCapabilities_ = Devices::ARKit::AKDevice::TrackerCapabilities(trackerCapabilities_ | Devices::ARKit::AKDevice::TC_PLANE_DETECTION);
					}
				}

				if (necessaryTrackerCapabilities & Devices::ARKit::AKDevice::TC_MESH_RECONSTRUCTION)
				{
					bool sceneReconstructionActivated = false;

					if (@available(iOS 13.4, *))
					{
						// first we try to activate scene reconstruction with classification, if this is not supported, we try to activate scene reconstruction without classification

						if ([ARWorldTrackingConfiguration supportsSceneReconstruction:ARSceneReconstructionMeshWithClassification])
						{
							arWorldTrackingConfiguration.sceneReconstruction = ARSceneReconstructionMeshWithClassification;
							sceneReconstructionActivated = true;
						}
						else if ([ARWorldTrackingConfiguration supportsSceneReconstruction:ARSceneReconstructionMesh])
						{
							arWorldTrackingConfiguration.sceneReconstruction = ARSceneReconstructionMesh;
							sceneReconstructionActivated = true;
						}
					}

					if (!sceneReconstructionActivated)
					{
						Log::warning() << "The devices does not support ARKit's scene reconstruction";
					}
				}

				if (necessaryTrackerCapabilities & Devices::ARKit::AKDevice::TC_DEPTH)
				{
					if (@available(iOS 14.0, *))
					{
						if ([ARWorldTrackingConfiguration supportsFrameSemantics:ARFrameSemanticSceneDepth])
						{
							arWorldTrackingConfiguration.frameSemantics = ARFrameSemanticSceneDepth;
						}
					}
				}

				if (necessaryTrackerCapabilities & AKDevice::TC_FACE)
				{
					if (@available(iOS 13.0, *))
					{
						if (ARWorldTrackingConfiguration.supportsUserFaceTracking)
						{
							arWorldTrackingConfiguration.userFaceTrackingEnabled = true;
						}
						else
						{
							Log::warning() << "Face tracking is currently not supported";
						}
					}
				}

				arConfiguration_ = arWorldTrackingConfiguration;
			}
		}
		else if (necessaryTrackerCapabilities & AKDevice::TC_FACE)
		{
			ARFaceTrackingConfiguration* arFaceTrackingConfiguration = [ARFaceTrackingConfiguration new];

			if (@available(iOS 11.3, *))
			{
#ifdef OCEAN_DEBUG
				Log::debug() << "Supported video formats for face tracking:";

				for (size_t n = 0; n < ARFaceTrackingConfiguration.supportedVideoFormats.count; ++n)
				{
					ARVideoFormat* videoFormat = ARFaceTrackingConfiguration.supportedVideoFormats[n];

					if (@available(iOS 14.5, *))
					{
						Log::debug() << "" << StringApple::toUTF8(videoFormat.captureDeviceType) << ", " << int(videoFormat.imageResolution.width) << "x" << int(videoFormat.imageResolution.height);
					}
					else
					{
						Log::debug() << "" << int(videoFormat.imageResolution.width) << "x" << int(videoFormat.imageResolution.height);
					}
				}
#endif

				// let's try to select a video format with 720p - otherwise we use the default video format

				for (size_t n = 0; n < ARFaceTrackingConfiguration.supportedVideoFormats.count; ++n)
				{
					ARVideoFormat* videoFormat = ARFaceTrackingConfiguration.supportedVideoFormats[n];

					if (videoFormat.imageResolution.width == 1280.0f && videoFormat.imageResolution.height == 720.0f)
					{
						arFaceTrackingConfiguration.videoFormat = videoFormat;
						break;
					}
				}
				
				if (necessaryTrackerCapabilities & AKDevice::TC_SLAM)
				{
					if (@available(iOS 13.0, *))
					{
						if (ARFaceTrackingConfiguration.supportsWorldTracking)
						{
							arFaceTrackingConfiguration.worldTrackingEnabled = true;
						}
						else
						{
							Log::warning() << "World/SLAM tracking is currently not supported";
						}
					}
				}
			}

			arConfiguration_ = arFaceTrackingConfiguration;
		}

		if (arConfiguration_ == nullptr)
		{
			return false;
		}

		arConfiguration_.worldAlignment = ARWorldAlignmentGravity;

		trackerCapabilities_ = AKDevice::TrackerCapabilities(trackerCapabilities_ | AKDevice::TC_SLAM);

		runWithConfigurationNecessary = true;
	}

	if (runWithConfigurationNecessary)
	{
		ocean_assert(arConfiguration_ != nullptr);

		arConfiguration_.lightEstimationEnabled = true;

		[arSession_ runWithConfiguration:arConfiguration_];

		if (@available(iOS 14.0, *))
		{
			lastARGeoTrackingState_ = ARGeoTrackingState(-1);
			lastARGeoTrackingStateReason_ = ARGeoTrackingStateReason(-1);
			lastARGeoTrackingAccuracy_ = ARGeoTrackingAccuracy(-1);
		}
	}

	inputLiveVideo_ = inputLiveVideo;

	isRunning_ = true;

	deviceMap_[device]++;

	return true;
}

- (bool)pause:(AKDevice*)tracker
{
	return true;
}

- (bool)stop:(AKDevice*)tracker
{
	std::unordered_map<AKDevice*, unsigned int>::iterator iDevice = deviceMap_.find(tracker);
	ocean_assert(iDevice != deviceMap_.cend());
	ocean_assert(iDevice->second >= 1u);

	iDevice->second--;
	if (iDevice->second == 0u)
	{
		deviceMap_.erase(iDevice);
	}

	if (!deviceMap_.empty())
	{
		// the ARKit tracker is actually still used by another AKDevice tracker
		return true;
	}

	if (arSession_ == nullptr)
	{
		ocean_assert(isRunning_ == false);
		ocean_assert(arConfiguration_ == nullptr);

		return false;
	}

	for (std::pair<Devices::Measurement::ObjectId, ARAnchor*> pair : anchorMap_)
	{
		[arSession_ removeAnchor:pair.second];
	}

	anchorMap_.clear();

	[arSession_ pause];

	arConfiguration_ = nullptr;
	arSession_ = nullptr;

	trackerCapabilities_ = AKDevice::TC_INVALID;

	isRunning_ = false;

	inputLiveVideo_.release();

	return true;
}

- (bool)addGeoAnchor:(Devices::Measurement::ObjectId)objectId withLatitude:(double)latitude withLongitude:(double)longitude withAltitude:(double)altitude
{
	if (arSession_ == nil)
	{
		return false;
	}

	if ((trackerCapabilities_ & AKDevice::TC_GEO_ANCHORS) != AKDevice::TC_GEO_ANCHORS)
	{
		return false;
	}

	if (anchorMap_.find(objectId) != anchorMap_.cend())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (@available(iOS 14.0, *))
	{
		CLLocationCoordinate2D coordinate;
		coordinate.latitude = latitude;
		coordinate.longitude = longitude;

		[ARGeoTrackingConfiguration checkAvailabilityAtCoordinate:coordinate completionHandler:^(BOOL isAvailable, NSError* error)
		{
			if (isAvailable == YES)
			{
#ifdef OCEAN_DEBUG
				Log::debug() << "Geo location " << latitude << ", " << longitude << " is available";
#endif
			}
			else
			{
				Log::warning() << "Geo location " << latitude << ", " << longitude << " is not available: " << StringApple::toUTF8(error.domain);
			}
		}];

		const std::string anchorIdentifier = String::toAString(objectId);

		ARGeoAnchor* geoAnchor = nullptr;

		if (altitude == NumericD::minValue())
		{
			geoAnchor = [[ARGeoAnchor alloc] initWithName:StringApple::toNSString(anchorIdentifier) coordinate:coordinate];
		}
		else
		{
			geoAnchor = [[ARGeoAnchor alloc] initWithName:StringApple::toNSString(anchorIdentifier) coordinate:coordinate altitude:altitude];
		}

		[arSession_ addAnchor:geoAnchor];

		anchorMap_.emplace(objectId, geoAnchor);

		return true;
	}

	return false;
}

- (bool)removeGeoAnchor:(Devices::Measurement::ObjectId)objectId
{
	const std::unordered_map<Devices::Measurement::ObjectId, ARAnchor*>::const_iterator iAnchor = anchorMap_.find(objectId);

	if (iAnchor == anchorMap_.cend())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	[arSession_ removeAnchor:iAnchor->second];

	anchorMap_.erase(iAnchor);

	return true;
}

- (void)session:(ARSession *)session didUpdateFrame:(ARFrame *)frame
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

	HomogenousMatrix4 world_T_rotatedWorld(true);

	if (trackerCapabilities_ & AKDevice::TC_GEO_ANCHORS)
	{
		// when using Geo Anchors, ARKit suddently rotated the world coordinate system (and anchor coordinate system) by 90 degree compared to a standard world tracking

		world_T_rotatedWorld = HomogenousMatrix4(Quaternion(Vector3(0, 1, 0), -Numeric::pi_2()));
	}

	HomogenousMatrix4 world_T_camera(false);
	if (frame.camera.trackingState == ARTrackingStateNormal)
	{
		simd_float4x4 simdTransform = frame.camera.transform;

		HomogenousMatrixF4 rotatedWorld_T_cameraF(false);
		memcpy(rotatedWorld_T_cameraF.data() +  0, &simdTransform.columns[0], sizeof(float) * 4);
		memcpy(rotatedWorld_T_cameraF.data() +  4, &simdTransform.columns[1], sizeof(float) * 4);
		memcpy(rotatedWorld_T_cameraF.data() +  8, &simdTransform.columns[2], sizeof(float) * 4);
		memcpy(rotatedWorld_T_cameraF.data() + 12, &simdTransform.columns[3], sizeof(float) * 4);

		world_T_camera = world_T_rotatedWorld * HomogenousMatrix4(rotatedWorld_T_cameraF);
	}

	const Timestamp timestamp(frameUnixTimestamp);

	bool containsGeoTracker = false;

	for (std::unordered_map<AKDevice*, unsigned int>::const_iterator iDevice = deviceMap_.cbegin(); iDevice != deviceMap_.cend(); ++iDevice)
	{
		AKDevice* device = iDevice->first;

		if (device->name() == AKWorldTracker6DOF::deviceNameAKWorldTracker6DOF())
		{
			AKWorldTracker6DOF* worldTracker = dynamic_cast<AKWorldTracker6DOF*>(device);
			ocean_assert(worldTracker != nullptr);

			worldTracker->onNewSample(world_T_camera, timestamp, frame);
		}
		else if (device->name() == AKDepthTracker6DOF::deviceNameAKDepthTracker6DOF())
		{
			if (@available(iOS 14.0, *))
			{
				if (anyCamera)
				{
					AKDepthTracker6DOF* depthTracker = dynamic_cast<AKDepthTracker6DOF*>(device);
					ocean_assert(depthTracker != nullptr);

					depthTracker->onNewSample(world_T_camera, timestamp, anyCamera, HomogenousMatrix4(inputLiveVideo_->device_T_camera()), frame);
				}
			}
		}
		else if (device->name() == AKSceneTracker6DOF::deviceNameAKSceneTracker6DOF())
		{
			if (@available(iOS 11.3, *))
			{
				AKSceneTracker6DOF* sceneTracker = dynamic_cast<AKSceneTracker6DOF*>(device);
				ocean_assert(sceneTracker != nullptr);

				sceneTracker->onNewSample(world_T_camera, world_T_rotatedWorld, timestamp, frame);
			}
		}
		else if (device->name() == AKPlaneTracker6DOF::deviceNameAKPlaneTracker6DOF())
		{
			if (@available(iOS 11.3, *))
			{
				AKPlaneTracker6DOF* planeTracker = dynamic_cast<AKPlaneTracker6DOF*>(device);
				ocean_assert(planeTracker != nullptr);

				planeTracker->onNewSample(world_T_camera, timestamp, frame);
			}
		}
		else if (device->name() == AKGeoAnchorsTracker6DOF::deviceNameAKGeoAnchorsTracker6DOF())
		{
			if (@available(iOS 14.0, *))
			{
				if (!containsGeoTracker)
				{
					if (frame.anchors.count != 0)
					{
						if (frame.geoTrackingStatus.state != lastARGeoTrackingState_ || frame.geoTrackingStatus.accuracy != lastARGeoTrackingAccuracy_ || frame.geoTrackingStatus.stateReason != lastARGeoTrackingStateReason_)
						{
							Log::info() << "Changed ARKit Geo Anchor States:";
							Log::info() << "State: " << AKDevice::translateGeoTrackingState(frame.geoTrackingStatus.state);
							Log::info() << "Accuracy: " + AKDevice::translateGeoTrackingAccuracy(frame.geoTrackingStatus.accuracy);
							Log::info() << "Reason: " + AKDevice::translateGeoTrackingStateReason(frame.geoTrackingStatus.stateReason);
							Log::info().newLine();

							lastARGeoTrackingState_ = frame.geoTrackingStatus.state;
							lastARGeoTrackingAccuracy_ = frame.geoTrackingStatus.accuracy;
							lastARGeoTrackingStateReason_ = frame.geoTrackingStatus.stateReason;
						}
					}

					containsGeoTracker = true;
				}

				AKGeoAnchorsTracker6DOF* geoAnchorsTracker = dynamic_cast<AKGeoAnchorsTracker6DOF*>(device);
				ocean_assert(geoAnchorsTracker != nullptr);

				geoAnchorsTracker->onNewSample(world_T_camera, world_T_rotatedWorld, timestamp, frame);
			}
		}
		else if (device->name() == AKFaceTracker6DOF::deviceNameAKFaceTracker6DOF())
		{
			if (@available(iOS 13.0, *))
			{
				AKFaceTracker6DOF* faceTracker = dynamic_cast<AKFaceTracker6DOF*>(device);
				ocean_assert(faceTracker != nullptr);

				faceTracker->onNewSample(world_T_camera, timestamp, frame);
			}
		}
	}
}

- (void)session:(ARSession *)session didAddAnchors:(NSArray<__kindof ARAnchor *> *)anchors
{
	if (@available(iOS 14.0, *))
	{
#ifdef OCEAN_DEBUG
		for (ARAnchor* anchor in anchors)
		{
			const std::string anchorName = StringApple::toUTF8(anchor.name);
			if (!anchorName.empty())
			{
				Log::debug() << "Added anchor with id '" << anchorName;
			}
		}
#endif
	}
}

- (void)session:(ARSession *)session didRemoveAnchors:(NSArray<__kindof ARAnchor *> *)anchors
{
	if (@available(iOS 14.0, *))
	{
#ifdef OCEAN_DEBUG
		for (ARAnchor* anchor in anchors)
		{
			const std::string anchorName = StringApple::toUTF8(anchor.name);
			if (!anchorName.empty())
			{
				Log::debug() << "Removed anchor with id '" << anchorName;
			}
		}
#endif
	}
}

@end

namespace Ocean
{

namespace Devices
{

namespace ARKit
{

AKDevice::ARSessionManager::ARSessionManager()
{
	akTracker6DOFDelegate_ = [AKTracker6DOFDelegate new];
}

bool AKDevice::ARSessionManager::start(AKDevice* tracker, const Media::FrameMediumRef& frameMedium)
{
	ocean_assert(tracker != nullptr);
	ocean_assert(frameMedium);
	ocean_assert(akTracker6DOFDelegate_ != nullptr);

	const ScopedLock scopedLock(akTracker6DOFDelegate_->lock_);

	return [akTracker6DOFDelegate_ restart:tracker withMedium:frameMedium];
}

bool AKDevice::ARSessionManager::pause(AKDevice* tracker)
{
	ocean_assert(tracker != nullptr);
	ocean_assert(akTracker6DOFDelegate_ != nullptr);

	const ScopedLock scopedLock(akTracker6DOFDelegate_->lock_);

	return [akTracker6DOFDelegate_ pause:tracker];
}

bool AKDevice::ARSessionManager::stop(AKDevice* tracker)
{
	ocean_assert(tracker != nullptr);
	ocean_assert(akTracker6DOFDelegate_ != nullptr);

	const ScopedLock scopedLock(akTracker6DOFDelegate_->lock_);

	if (![akTracker6DOFDelegate_ stop:tracker])
	{
		return false;
	}

	if (![akTracker6DOFDelegate_ isRunning])
	{
		// releasing the current delegate and creating a new delegate to ensure that the camera stream arrivates though AV Foundation again
		akTracker6DOFDelegate_ = [AKTracker6DOFDelegate new];
	}

	return true;
}

bool AKDevice::ARSessionManager::registerGeoAnchor(const Measurement::ObjectId& objectId, const double latitude, const double longitude, const double altitude)
{
	ocean_assert(objectId != Measurement::invalidObjectId());
	ocean_assert(latitude >= -90.0 && latitude <= 90.0);
	ocean_assert(longitude >= -180.0 && longitude <= 180.0);
	ocean_assert(altitude == NumericD::minValue() || (altitude >= -10000.0 && altitude <= 30000.0));

	ocean_assert(akTracker6DOFDelegate_ != nullptr);

	const ScopedLock scopedLock(akTracker6DOFDelegate_->lock_);

	return [akTracker6DOFDelegate_ addGeoAnchor:objectId withLatitude:latitude withLongitude:longitude withAltitude:altitude];
}

bool AKDevice::ARSessionManager::unregisterGeoAnchor(const Measurement::ObjectId& objectId)
{
	ocean_assert(objectId != Measurement::invalidObjectId());
	ocean_assert(akTracker6DOFDelegate_ != nullptr);

	const ScopedLock scopedLock(akTracker6DOFDelegate_->lock_);

	return [akTracker6DOFDelegate_ removeGeoAnchor:objectId];
}

AKDevice::AKDevice(const TrackerCapabilities trackerCapabilities, const std::string& name, const DeviceType type) :
	Device(name, type),
	trackerCapabilities_(trackerCapabilities)
{
	// nothing to do here
}

const std::string& AKDevice::library() const
{
	return nameARKitLibrary();
}

API_AVAILABLE(ios(14.0))
std::string AKDevice::translateGeoTrackingState(const ARGeoTrackingState& state)
{
	switch (state)
	{
		case ARGeoTrackingStateInitializing:
			return std::string("ARGeoTrackingStateInitializing");

		case ARGeoTrackingStateLocalized:
			return std::string("ARGeoTrackingStateLocalized");

		case ARGeoTrackingStateLocalizing:
			return std::string("ARGeoTrackingStateLocalizing");

		case ARGeoTrackingStateNotAvailable:
			return std::string("ARGeoTrackingStateNotAvailable");
	}

	ocean_assert(false && "Unknown");
	return std::string("Unknown");
}

API_AVAILABLE(ios(14.0))
std::string AKDevice::translateGeoTrackingStateReason(const ARGeoTrackingStateReason& stateReason)
{
	switch (stateReason)
	{
		case ARGeoTrackingStateReasonNone:
			return std::string("ARGeoTrackingStateReasonNone");

		case ARGeoTrackingStateReasonNotAvailableAtLocation:
			return std::string("ARGeoTrackingStateReasonNotAvailableAtLocation");

		case ARGeoTrackingStateReasonNeedLocationPermissions:
			return std::string("ARGeoTrackingStateReasonNeedLocationPermissions");

		case ARGeoTrackingStateReasonDevicePointedTooLow:
			return std::string("ARGeoTrackingStateReasonDevicePointedTooLow");

		case ARGeoTrackingStateReasonWorldTrackingUnstable:
			return std::string("ARGeoTrackingStateReasonWorldTrackingUnstable");

		case ARGeoTrackingStateReasonWaitingForLocation:
			return std::string("ARGeoTrackingStateReasonWaitingForLocation");

		case ARGeoTrackingStateReasonWaitingForAvailabilityCheck:
			return std::string("ARGeoTrackingStateReasonWaitingForAvailabilityCheck");

		case ARGeoTrackingStateReasonGeoDataNotLoaded:
			return std::string("ARGeoTrackingStateReasonGeoDataNotLoaded");

		case ARGeoTrackingStateReasonVisualLocalizationFailed:
			return std::string("ARGeoTrackingStateReasonVisualLocalizationFailed");
	}

	ocean_assert(false && "Unknown");
	return std::string("Unknown");
}

API_AVAILABLE(ios(14.0))
std::string AKDevice::translateGeoTrackingAccuracy(const ARGeoTrackingAccuracy& accuracy)
{
	switch (accuracy)
	{
		case ARGeoTrackingAccuracyHigh:
			return std::string("ARGeoTrackingAccuracyHigh");

		case ARGeoTrackingAccuracyUndetermined:
			return std::string("ARGeoTrackingAccuracyUndetermined");

		case ARGeoTrackingAccuracyLow:
			return std::string("ARGeoTrackingAccuracyLow");

		case ARGeoTrackingAccuracyMedium:
			return std::string("ARGeoTrackingAccuracyMedium");
	}

	ocean_assert(false && "Unknown");
	return std::string("Unknown");
}

}

}

}
