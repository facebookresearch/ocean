/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/arcore/ARSessionManager.h"
#include "ocean/devices/arcore/ACDepthTracker6DOF.h"
#include "ocean/devices/arcore/ACPlaneTracker6DOF.h"
#include "ocean/devices/arcore/ACSceneTracker6DOF.h"
#include "ocean/devices/arcore/ACWorldTracker6DOF.h"

#include "ocean/cv/FrameConverterY_U_V12.h"

#include "ocean/cv/segmentation/Triangulation.h"

#include "ocean/media/android/ALiveVideo.h"

#include "ocean/platform/android/NativeInterfaceManager.h"

namespace Ocean
{

namespace Devices
{

namespace ARCore
{

ARSessionManager::Session::Session(const Media::FrameMediumRef& frameMedium) :
	frameMedium_(frameMedium)
{
	ocean_assert(frameMedium_);
}

bool ARSessionManager::Session::registerTracker(ACDevice* tracker)
{
	ocean_assert(tracker != nullptr);
	ocean_assert(trackerMap_.find(tracker) == trackerMap_.end());

	trackerMap_[tracker] = 0u;

	return true;
}

bool ARSessionManager::Session::unregisterTracker(ACDevice* tracker)
{
	ocean_assert(tracker != nullptr);
	ocean_assert(trackerMap_.find(tracker) != trackerMap_.end());

	trackerMap_.erase(tracker);

	return true;
}

bool ARSessionManager::Session::start(ACDevice* tracker)
{
	ocean_assert(frameMedium_);

	ocean_assert(tracker != nullptr);
	ocean_assert(tracker->trackerCapabilities() & ACDevice::TC_SLAM);

	const TrackerCapabilities newSessionCapabilities = TrackerCapabilities(sessionCapabilities_ | tracker->trackerCapabilities());

	if ((sessionCapabilities_ & newSessionCapabilities) != newSessionCapabilities)
	{
		// the necessary capabilities are not yet part of the existing capabilities

		JNIEnv* environment = Platform::Android::NativeInterfaceManager::get().environment();
		ocean_assert(environment != nullptr);

		jobject activity = Platform::Android::NativeInterfaceManager::get().currentActivity();
		ocean_assert(activity != nullptr);

		ScopedARSession arSession;
		const ArStatus sessionCreateStatus = ArSession_create(environment, activity, arSession.ingest());

		if (sessionCreateStatus != AR_SUCCESS)
		{
			std::string errorString = "Unknown";

			switch (sessionCreateStatus)
			{
				case AR_ERROR_FATAL:
					errorString = "AR_ERROR_FATAL";
					break;

				case AR_ERROR_CAMERA_PERMISSION_NOT_GRANTED:
					errorString = "AR_ERROR_CAMERA_PERMISSION_NOT_GRANTED";
					break;

				case AR_UNAVAILABLE_ARCORE_NOT_INSTALLED:
					errorString = "AR_UNAVAILABLE_ARCORE_NOT_INSTALLED";
					break;

				case AR_UNAVAILABLE_DEVICE_NOT_COMPATIBLE:
					errorString = "AR_UNAVAILABLE_DEVICE_NOT_COMPATIBLE";
					break;

				case AR_UNAVAILABLE_APK_TOO_OLD:
					errorString = "AR_UNAVAILABLE_APK_TOO_OLD";
					break;

				case AR_UNAVAILABLE_SDK_TOO_OLD:
					errorString = "AR_UNAVAILABLE_SDK_TOO_OLD";
					break;

				default:
					ocean_assert(false && "Unknown status!");
					break;
			}

			Log::error() << "Failed to create an ARSession, reason: " << errorString;
			return false;
		}

		if (newSessionCapabilities & ACDevice::TC_DEPTH)
		{
			int32_t isSupported = 0;
			ArSession_isDepthModeSupported(arSession, AR_DEPTH_MODE_AUTOMATIC, &isSupported);

			if (isSupported == 0)
			{
				return false;
			}
		}

		int32_t preferredCameraWidth = 1280;
		int32_t preferredCameraHeight = 720;
		if (frameMedium_->preferredFrameWidth() != 0u && frameMedium_->preferredFrameHeight() != 0u)
		{
			preferredCameraWidth = int32_t(frameMedium_->preferredFrameWidth());
			preferredCameraHeight = int32_t(frameMedium_->preferredFrameHeight());
		}

		ArCameraConfigFacingDirection arNecessaryFacingDirection = AR_CAMERA_CONFIG_FACING_DIRECTION_BACK;

		if ((frameMedium_->device_T_camera().rotation() * VectorD3(0, 0, 1)) * VectorD3(0, 0, 1) < 0.0)
		{
			arNecessaryFacingDirection = AR_CAMERA_CONFIG_FACING_DIRECTION_FRONT;
			ocean_assert(frameMedium_->url() == "LiveVideoId:1");
		}

		ScopedARCameraConfig arCameraConfigToUse;

		const unsigned int configIterations = (newSessionCapabilities & ACDevice::TC_DEPTH) ? 2u : 1u;

		for (unsigned int configIteration = 0u; !arCameraConfigToUse && configIteration < configIterations; ++configIteration)
		{
			const ScopedARCameraConfigFilter arCameraConfigFilter(arSession, ArCameraConfigFilter_create);
			const ScopedARCameraConfigList arCameraConfigList(arSession, ArCameraConfigList_create);

			if (newSessionCapabilities & ACDevice::TC_DEPTH)
			{
				if (configIteration == 0u)
				{
					// in the first iteration, we try to request depth sensor; we drop this requirement in the second iteration
					ArCameraConfigFilter_setDepthSensorUsage(arSession, arCameraConfigFilter, AR_CAMERA_CONFIG_DEPTH_SENSOR_USAGE_REQUIRE_AND_USE);
				}
			}

			ArSession_getSupportedCameraConfigsWithFilter(arSession, arCameraConfigFilter, arCameraConfigList);

			int32_t arConfigListSize = 0;
			ArCameraConfigList_getSize(arSession, arCameraConfigList, &arConfigListSize);

			if (arConfigListSize == 0)
			{
				Log::debug() << "ArCore: Did not find any camera configuration in configuration iteration " << configIteration + 1u << " (of " << configIterations << " iterations)";
			}

			for (int32_t n = 0; n < arConfigListSize; ++n)
			{
				ScopedARCameraConfig arCameraConfig(arSession, ArCameraConfig_create);

				ArCameraConfigList_getItem(arSession, arCameraConfigList, n, arCameraConfig);

				ArCameraConfigFacingDirection arFacingDirection = AR_CAMERA_CONFIG_FACING_DIRECTION_BACK;
				ArCameraConfig_getFacingDirection(arSession, arCameraConfig, &arFacingDirection);

				if (arFacingDirection != arNecessaryFacingDirection)
				{
					continue;
				}

				int32_t width = 0;
				int32_t height = 0;
				ArCameraConfig_getImageDimensions(arSession, arCameraConfig, &width, &height);

				if (width == preferredCameraWidth && height == preferredCameraHeight)
				{
					arCameraConfigToUse = std::move(arCameraConfig);
					break;
				}

				if (configIteration == configIterations - 1u && n == arConfigListSize - 1)
				{
					// we did not find a configuration with perfect match, so we take the first configuration

					ocean_assert(!arCameraConfigToUse);

					arCameraConfigToUse = ScopedARCameraConfig(arSession, ArCameraConfig_create);

					ArCameraConfigList_getItem(arSession, arCameraConfigList, 0, arCameraConfigToUse);

					Log::warning() << "ArCore: Used default camera configuration";
					break;
				}
			}

			if (!arCameraConfigToUse)
			{
				Log::debug() << "ArCore: Was not able to determine perfect camera configuration for tracker '" << tracker->name() << "' in configuration iteration " << configIteration + 1u << " (of " << configIterations << " iterations)";
			}
		}

		if (!arCameraConfigToUse)
		{
			Log::error() << "Failed to determine ArCameraConfig for ArSession";
			return false;
		}

		if (ArSession_setCameraConfig(arSession, arCameraConfigToUse) == AR_SUCCESS)
		{
			Log::info() << "ArSession configuration with resolution " << preferredCameraWidth << "x" << preferredCameraHeight;
		}
		else
		{
			Log::error() << "Failed to configure ArSession with resolution " << preferredCameraWidth << "x" << preferredCameraHeight;
		}

		const ScopedARConfig arConfig(arSession, ArConfig_create);

		ArConfig_setUpdateMode(arSession, arConfig, AR_UPDATE_MODE_LATEST_CAMERA_IMAGE);
		ArConfig_setFocusMode(arSession, arConfig, AR_FOCUS_MODE_AUTO);

		if (newSessionCapabilities & ACDevice::TC_DEPTH)
		{
			ArConfig_setDepthMode(arSession, arConfig, AR_DEPTH_MODE_AUTOMATIC);
		}

		if (newSessionCapabilities & ACDevice::TC_PLANE_DETECTION)
		{
			ArConfig_setPlaneFindingMode(arSession, arConfig, AR_PLANE_FINDING_MODE_HORIZONTAL_AND_VERTICAL);
		}

		const ArStatus sessionConfigureStatus = ArSession_configure(arSession, arConfig);

		if (sessionConfigureStatus != AR_SUCCESS)
		{
			Log::error() << "Failed to configure an ARSession: " << int(sessionConfigureStatus);
			return false;
		}

		arSession_ = std::move(arSession);
	}

	ocean_assert(arSession_);

	if (ArSession_resume(arSession_) != AR_SUCCESS)
	{
		return false;
	}

	sessionCapabilities_ = newSessionCapabilities;

	ocean_assert(trackerMap_.find(tracker) != trackerMap_.cend());
	trackerMap_[tracker] = 2u;

	return true;
}

bool ARSessionManager::Session::pause(ACDevice* tracker)
{
	ocean_assert(trackerMap_.find(tracker) != trackerMap_.cend());
	trackerMap_[tracker] = 1u;

	for (TrackerMap::iterator iTracker = trackerMap_.begin(); iTracker != trackerMap_.end(); ++iTracker)
	{
		if (iTracker->second >= 2u)
		{
			// there is still one registered tracker running, so we cannot pause the session
			return true;
		}
	}

	if (arSession_)
	{
		if (ArSession_pause(arSession_) != AR_SUCCESS)
		{
			return false;
		}
	}

	return true;
}

bool ARSessionManager::Session::stop(ACDevice* tracker)
{
	ocean_assert(trackerMap_.find(tracker) != trackerMap_.cend());
	trackerMap_[tracker] = 0u;

	for (TrackerMap::iterator iTracker = trackerMap_.begin(); iTracker != trackerMap_.end(); ++iTracker)
	{
		if (iTracker->second >= 1u)
		{
			// there is still one registered tracker paused or running, so we cannot pause the session
			return true;
		}
	}

	if (arSession_)
	{
		arSession_.release();
	}

	if (frameMedium_)
	{
		// ARCore borrowed the camera resource but does not automatically return the resource when not needed anymore
		// therefore, we need to force a restart of the camera
		frameMedium_.force<Media::Android::ALiveVideo>().forceRestart();
	}

	return true;
}

void ARSessionManager::Session::update(unsigned int textureId)
{
	if (!arSession_)
	{
		return;
	}

	if (trackerMap_.empty())
	{
		return;
	}

	ArSession_setCameraTextureName(arSession_, textureId);

	ArFrame* arFrame = nullptr;
	ArFrame_create(arSession_, &arFrame);

	ArStatus error = ArSession_update(arSession_, arFrame);

	if (error != AR_SUCCESS)
	{
		return;
	}

	int64_t currentTimestampNs = NumericT<int64_t>::minValue();
	ArFrame_getTimestamp(arSession_, arFrame, &currentTimestampNs);

	if (lastTimestampNs_ == currentTimestampNs)
	{
		return;
	}

	const Timestamp frameUnixTimestamp(true); // **TODO**

	SharedAnyCamera anyCamera;

	{
		HomogenousMatrix4 world_T_camera(false);

		HomogenousMatrixF4 world_T_device;
		if (extractPose(arSession_, arFrame, world_T_device, anyCamera))
		{
			const HomogenousMatrix4 device_T_camera(Quaternion(Vector3(0, 0, 1), -Numeric::pi_2()));
			world_T_camera = HomogenousMatrix4(world_T_device) * device_T_camera;
		}

		for (TrackerMap::iterator iTracker = trackerMap_.begin(); iTracker != trackerMap_.end(); ++iTracker)
		{
			if (iTracker->second)
			{
				ACDevice* tracker = iTracker->first;
				ocean_assert(tracker != nullptr);

				if (tracker->name() == ACWorldTracker6DOF::deviceNameACWorldTracker6DOF())
				{
					ACWorldTracker6DOF* worldTracker = dynamic_cast<ACWorldTracker6DOF*>(tracker);
					ocean_assert(worldTracker != nullptr);

					worldTracker->onNewSample(world_T_camera, frameUnixTimestamp);
				}
				else if (tracker->name() == ACSceneTracker6DOF::deviceNameACSceneTracker6DOF())
				{
					Vectors3 objectPoints;
					Indices64 objectPointIds;
					if (extractPointCloud(arSession_, arFrame, objectPoints, objectPointIds))
					{
						ACSceneTracker6DOF* sceneTracker = dynamic_cast<ACSceneTracker6DOF*>(tracker);
						ocean_assert(sceneTracker != nullptr);

						sceneTracker->onNewSample(world_T_camera, std::move(objectPoints), std::move(objectPointIds), frameUnixTimestamp);
					}
				}
				else if (tracker->name() == ACDepthTracker6DOF::deviceNameACDepthTracker6DOF())
				{
					Frame depth = extractDepth(arSession_, arFrame);

					if (depth.isValid())
					{
						depth.setTimestamp(frameUnixTimestamp);

						SharedAnyCamera depthCamera = anyCamera->clone(depth.width(), depth.height());

						if (depthCamera)
						{
							ACDepthTracker6DOF* depthTracker = dynamic_cast<ACDepthTracker6DOF*>(tracker);
							ocean_assert(depthTracker != nullptr);

							depthTracker->onNewSample(world_T_camera, std::move(depth), std::move(depthCamera), HomogenousMatrix4(frameMedium_->device_T_camera()), frameUnixTimestamp);
						}
						else
						{
							Log::error() << "ArSession: Aspect ratio of depth image does not fit camera image";
						}
					}
				}
				else if (tracker->name() == ACPlaneTracker6DOF::deviceNameACPlaneTracker6DOF())
				{
					SceneTracker6DOF::SceneElementPlanes::Planes planes;

					if (extractPlanes(arSession_, arFrame, planes))
					{
						ACPlaneTracker6DOF* planeTracker = dynamic_cast<ACPlaneTracker6DOF*>(tracker);
						ocean_assert(planeTracker != nullptr);

						planeTracker->onNewSample(world_T_camera, std::move(planes), frameUnixTimestamp);
					}
				}
			}
		}
	}

	{
		Frame frame = extractImage(arSession_, arFrame);

		if (frame.isValid())
		{
			frame.setTimestamp(frameUnixTimestamp);

			frameMedium_.force<Ocean::Media::Android::ALiveVideo>().feedNewFrame(std::move(frame), std::move(anyCamera));
		}
	}

	lastTimestampNs_ = currentTimestampNs;
}

size_t ARSessionManager::Session::registeredTrackers()
{
	return trackerMap_.size();
}

bool ARSessionManager::Session::extractPlanes(ArSession* arSession, ArFrame* arFrame, SceneTracker6DOF::SceneElementPlanes::Planes& planes)
{
	ocean_assert(arSession != nullptr);
	ocean_assert(arFrame != nullptr);

	ScopedARTrackableList arTrackableList(arSession, ArTrackableList_create);

	ArSession_getAllTrackables(arSession, AR_TRACKABLE_PLANE, arTrackableList);

	int32_t numberTrackables = 0;
	ArTrackableList_getSize(arSession, arTrackableList, &numberTrackables);

	planes.clear();
	planes.reserve(numberTrackables);

	for (int32_t n = 0; n < numberTrackables; ++n)
	{
		ScopedARTrackable arTrackable;
		ArTrackableList_acquireItem(arSession, arTrackableList, n, arTrackable.ingest());

#ifdef OCEAN_DEBUG
		ArTrackableType arTrackableType = ArTrackableType(0);
		ArTrackable_getType(arSession, arTrackable, &arTrackableType);
		ocean_assert(arTrackableType == AR_TRACKABLE_PLANE);
#endif

		ArTrackingState arTrackingState = ArTrackingState(0);
		ArTrackable_getTrackingState(arSession, arTrackable, &arTrackingState);

		if (arTrackingState == AR_TRACKING_STATE_TRACKING)
		{
			ArPlane* arPlane = ArAsPlane(arTrackable);

			ArPlane* arSubsumedPlane = nullptr;
			ArPlane_acquireSubsumedBy(arSession, arPlane, &arSubsumedPlane);

			if (arSubsumedPlane == nullptr)
			{
				arSubsumedPlane = arPlane;
			}

			ArPlaneType arPlaneType = ArPlaneType(0);
			ArPlane_getType(arSession, arSubsumedPlane, &arPlaneType);

			SceneTracker6DOF::SceneElementPlanes::Plane::PlaneType planeType = SceneTracker6DOF::SceneElementPlanes::Plane::PT_UNKNOWN;

			switch (arPlaneType)
			{
				case AR_PLANE_HORIZONTAL_DOWNWARD_FACING:
				case AR_PLANE_HORIZONTAL_UPWARD_FACING:
					planeType = SceneTracker6DOF::SceneElementPlanes::Plane::PT_HORIZONTAL;
					break;

				case AR_PLANE_VERTICAL:
					planeType = SceneTracker6DOF::SceneElementPlanes::Plane::PT_VERTICAL;
						break;

				default:
					Log::error() << "Unknown plane type!";
					ocean_assert(false && "Invalid plane type!");
			}

			// extract the bounding box

			float xExtent = -1.0f;
			float zExtent = -1.0f;
			ArPlane_getExtentX(arSession, arPlane, &xExtent);
			ArPlane_getExtentZ(arSession, arPlane, &zExtent);

			constexpr float yExtent = 0.0f;

			const Box3 boundingBox(Vector3(0, 0, 0), Scalar(xExtent), Scalar(yExtent), Scalar(zExtent));

			// extract the pose

			float poseRaw[7];
			ScopedARPose arPose;
			ArPose_create(arSession, poseRaw, arPose.ingest());

			ArPlane_getCenterPose(arSession, arPlane, arPose);

			HomogenousMatrixF4 world_T_plane(false);
			ArPose_getMatrix(arSession, arPose, world_T_plane.data());

			// extract the polygon

			int32_t numberPolygonElements = 0;
			ArPlane_getPolygonSize(arSession, arPlane, &numberPolygonElements);
			ocean_assert(numberPolygonElements % 2 == 0);

			Vectors3 boundaryVertices;
			Indices32 triangleIndices;

			if (numberPolygonElements != 0)
			{
				std::vector<float> polygonElements(numberPolygonElements);
				ArPlane_getPolygon(arSession, arPlane, polygonElements.data());

				boundaryVertices.reserve(numberPolygonElements / 2);

				std::vector<CV::PixelPositionT<int64_t>> boundaryVerticesI;
				boundaryVerticesI.reserve(numberPolygonElements / 2);

				for (int32_t n = 0; n < numberPolygonElements; n += 2)
				{
					boundaryVertices.emplace_back(Scalar(polygonElements[n]), Scalar(0), Scalar(polygonElements[n + 1]));

					boundaryVerticesI.emplace_back(int64_t(NumericD::round64(double(polygonElements[n]) * 1000.0)), int64_t(NumericD::round64(double(polygonElements[n + 1]) * 1000.0))); // integer vertices with mm precision
				}

				if (!boundaryVerticesI.empty())
				{
					const CV::Segmentation::PixelContourT<int64_t> boundaryContour(boundaryVerticesI);
					const CV::Segmentation::Triangulation::IndexTriangles indexTriangles = CV::Segmentation::Triangulation::triangulate(boundaryContour, true);

					triangleIndices.reserve(indexTriangles.size() * 3);

					for (const CV::Segmentation::Triangulation::IndexTriangle& indexTriangle : indexTriangles)
					{
						triangleIndices.emplace_back(indexTriangle[0]);
						triangleIndices.emplace_back(indexTriangle[1]);
						triangleIndices.emplace_back(indexTriangle[2]);
					}
				}
			}

			// we need an unique id for the plane

			PlaneIdMap::iterator iPlane = planeIdMap_.find(arPlane);

			if (iPlane == planeIdMap_.end())
			{
				iPlane = planeIdMap_.emplace(arPlane, Index32(planeIdMap_.size())).first;
			}

			const Index32 planeId = iPlane->second;

			Vectors3 vertices(boundaryVertices);

			planes.emplace_back(planeId, planeType, HomogenousMatrix4(world_T_plane), boundingBox, std::move(vertices), Vectors2(), std::move(triangleIndices), std::move(boundaryVertices));
		}
	}

	return true;
}

ARSessionManager::ARSessionManager()
{
	JNIEnv* environment = Platform::Android::NativeInterfaceManager::get().environment();
	ocean_assert(environment != nullptr);

	jobject activity = Platform::Android::NativeInterfaceManager::get().currentActivity();
	ocean_assert(activity != nullptr);

	if (environment == nullptr || activity == nullptr)
	{
		Log::error() << "Failed to register ARCore devices: Unknown environment or activity.";
		return;
	}

	const int32_t userRequestedInstall = 1;

	ArInstallStatus installStatus;
	ArStatus error = ArCoreApk_requestInstall(environment, activity, userRequestedInstall, &installStatus);

	if (error != AR_SUCCESS)
	{
		Log::error() << "Failed to install ARCore";
		return;
	}

	if (installStatus == AR_INSTALL_STATUS_INSTALLED)
	{
		Log::info() << "ARCore already installed.";
	}
	else
	{
		Log::error() << "ARCore not yet installed: " << int(installStatus);
	}

	ArAvailability availability;
	ArCoreApk_checkAvailability(environment, activity, &availability);

	isARCoreAvailable_ = availability == AR_AVAILABILITY_SUPPORTED_INSTALLED;

	if (!isARCoreAvailable_)
	{
		Log::error() << "ARCore is not available, error code: " << int(availability);
	}
}

bool ARSessionManager::registerTracker(ACDevice* tracker, const Media::FrameMediumRef& frameMedium)
{
	ocean_assert(tracker != nullptr);
	ocean_assert(frameMedium);

	Log::debug() << "ARSessionManager::registerTracker(): " << tracker->name();

	const ScopedLock scopedLock(lock_);

	ocean_assert(trackerMap_.find(tracker) == trackerMap_.cend());
	trackerMap_[tracker] = frameMedium->url();

	SessionMap::iterator iSession = sessionMap_.find(frameMedium->url());

	if (iSession == sessionMap_.cend())
	{
		iSession = sessionMap_.emplace(frameMedium->url(), std::make_shared<Session>(frameMedium)).first;
	}

	iSession->second->registerTracker(tracker);

	return true;
}

bool ARSessionManager::unregisterTracker(ACDevice* tracker)
{
	ocean_assert(tracker != nullptr);

	Log::debug() << "ARSessionManager::unregisterTracker(): " << tracker->name();

	const ScopedLock scopedLock(lock_);

	bool succeeded = false;

	TrackerMap::const_iterator iTracker = trackerMap_.find(tracker);
	ocean_assert(iTracker != trackerMap_.cend());

	if (iTracker != trackerMap_.cend())
	{
		SessionMap::iterator iSession = sessionMap_.find(iTracker->second);
		ocean_assert(iSession != sessionMap_.cend());

		if (iSession != sessionMap_.cend())
		{
			const bool result = iSession->second->unregisterTracker(tracker);
			ocean_assert_and_suppress_unused(result, result);

			if (iSession->second->registeredTrackers() == 0)
			{
				sessionMap_.erase(iSession);
			}

			succeeded = true;
		}
	}

	trackerMap_.erase(iTracker);

	return succeeded;
}

bool ARSessionManager::start(ACDevice* tracker)
{
	ocean_assert(tracker != nullptr);

	Log::debug() << "ARSessionManager::start(): " << tracker->name();

	const ScopedLock scopedLock(lock_);

	TrackerMap::const_iterator iTracker = trackerMap_.find(tracker);
	ocean_assert(iTracker != trackerMap_.cend());

	if (iTracker != trackerMap_.cend())
	{
		SessionMap::iterator iSession = sessionMap_.find(iTracker->second);
		ocean_assert(iSession != sessionMap_.cend());

		if (iSession != sessionMap_.cend())
		{
			if (iSession->second->start(tracker))
			{
				return true;
			}
		}
	}

	return false;
}

bool ARSessionManager::pause(ACDevice* tracker)
{
	ocean_assert(tracker != nullptr);

	Log::debug() << "ARSessionManager::pause(): " << tracker->name();

	const ScopedLock scopedLock(lock_);

	TrackerMap::const_iterator iTracker = trackerMap_.find(tracker);
	ocean_assert(iTracker != trackerMap_.cend());

	if (iTracker != trackerMap_.cend())
	{
		SessionMap::iterator iSession = sessionMap_.find(iTracker->second);
		ocean_assert(iSession != sessionMap_.cend());

		if (iSession != sessionMap_.cend())
		{
			if (iSession->second->pause(tracker))
			{
				return true;
			}
		}
	}

	return false;
}

bool ARSessionManager::stop(ACDevice* tracker)
{
	ocean_assert(tracker != nullptr);

	Log::debug() << "ARSessionManager::stop(): " << tracker->name();

	const ScopedLock scopedLock(lock_);

	TrackerMap::const_iterator iTracker = trackerMap_.find(tracker);
	ocean_assert(iTracker != trackerMap_.cend());

	if (iTracker != trackerMap_.cend())
	{
		SessionMap::iterator iSession = sessionMap_.find(iTracker->second);
		ocean_assert(iSession != sessionMap_.cend());

		if (iSession != sessionMap_.cend())
		{
			if (iSession->second->stop(tracker))
			{
				return true;
			}
		}
	}

	return false;
}

void ARSessionManager::update(unsigned int textureId)
{
	const ScopedLock scopedLock(lock_);

	for (SessionMap::iterator iSession = sessionMap_.begin(); iSession != sessionMap_.end(); ++iSession)
	{
		// possible issue: do we need individual texture ids for individual sessions?
		iSession->second->update(textureId);
	}
}

Frame ARSessionManager::extractImage(ArSession* arSession, ArFrame* arFrame)
{
	ocean_assert(arSession != nullptr);
	ocean_assert(arFrame != nullptr);

	ScopedARImage arImage;
	if (ArFrame_acquireCameraImage(arSession, arFrame, arImage.ingest()) != AR_SUCCESS)
	{
		return Frame();
	}

	ArImageFormat arImageFormat = AR_IMAGE_FORMAT_INVALID;
	ArImage_getFormat(arSession, arImage, &arImageFormat);

	Frame frame;

	if (arImageFormat == AR_IMAGE_FORMAT_YUV_420_888)
	{
		int32_t width = 0;
		ArImage_getWidth(arSession, arImage, &width);

		int32_t height = 0;
		ArImage_getHeight(arSession, arImage, &height);

		int32_t planes = 0;
		ArImage_getNumberOfPlanes(arSession, arImage, &planes);

		if (planes == 3 && width >= 2 && height >= 2 && width % 2 == 0 && height % 2 == 0)
		{
			const int32_t width_2 = width / 2;
			const int32_t height_2 = height / 2;

			const uint8_t* sources[3] = {nullptr, nullptr, nullptr};
			unsigned int sourcePaddingElements[3] = {0u, 0u, 0u};
			unsigned int sourcePixelStrides[3] = {0u, 0u, 0u};

			for (int32_t planeIndex = 0; planeIndex < 3; ++planeIndex)
			{
				const int32_t planeWidth = planeIndex == 0 ? width : width_2;
				const int32_t planeHeight = planeIndex == 0 ? height : height_2;

				int32_t planeDataLength = 0;
				int32_t planeDataRowStride = 0;
				int32_t planePixelStride = 0;
				ArImage_getPlaneData(arSession, arImage, planeIndex, sources + planeIndex, &planeDataLength);
				ArImage_getPlaneRowStride(arSession, arImage, planeIndex, &planeDataRowStride);
				ArImage_getPlanePixelStride(arSession, arImage, planeIndex, &planePixelStride);

				if (sources[planeIndex] == nullptr)
				{
					ocean_assert(false && "This must never happen!");
					return Frame();
				}

				if (planeDataRowStride < planeWidth)
				{
					ocean_assert(false && "This must never happen!");
					return Frame();
				}

				const int32_t minPlaneDataLength = planeWidth * planeHeight;

				if (planeDataLength < minPlaneDataLength)
				{
					ocean_assert(false && "This must never happen!");
					return Frame();
				}

				if (planePixelStride <= 0)
				{
					ocean_assert(false && "This must never happen!");
					return Frame();
				}

				sourcePixelStrides[planeIndex] = (unsigned int)(planePixelStride);

				sourcePaddingElements[planeIndex] = (unsigned int)(planeDataRowStride - planeWidth);
				ocean_assert(sourcePaddingElements[planeIndex] < planeDataRowStride);
			}

			int64_t timestampNs = NumericT<int64_t>::minValue();
			ArImage_getTimestamp(arSession, arImage, &timestampNs);

			const Timestamp timestamp(double(timestampNs) * 0.000000001);

			frame = Frame(FrameType((unsigned int)(width), (unsigned int)(height), FrameType::FORMAT_Y_UV12, FrameType::ORIGIN_UPPER_LEFT));
			frame.setTimestamp(timestamp);

			CV::FrameConverterY_U_V12::convertY_U_V12ToY_UV12(sources[0], sources[1], sources[2], frame.data<uint8_t>(0u), frame.data<uint8_t>(1u), frame.width(), frame.height(), sourcePaddingElements[0], sourcePaddingElements[1], sourcePaddingElements[2], frame.paddingElements(0u), frame.paddingElements(1u), sourcePixelStrides[0], sourcePixelStrides[1], sourcePixelStrides[2]);
		}
	}
	else
	{
		Log::error() << "Not supported ArImageFormat: " << int(arImageFormat);
	}

	return frame;
}

Frame ARSessionManager::extractDepth(const ArSession* arSession, const ArFrame* arFrame)
{
	ocean_assert(arSession != nullptr);
	ocean_assert(arFrame != nullptr);

	ScopedARImage arImage;
	if (ArFrame_acquireDepthImage(arSession, arFrame, arImage.ingest()) != AR_SUCCESS)
	{
		return Frame();
	}

	ArImageFormat arImageFormat = AR_IMAGE_FORMAT_INVALID;
	ArImage_getFormat(arSession, arImage, &arImageFormat);

	Frame depthFrame;

	if (arImageFormat == AR_IMAGE_FORMAT_DEPTH16)
	{
		int32_t width = 0;
		ArImage_getWidth(arSession, arImage, &width);

		int32_t height = 0;
		ArImage_getHeight(arSession, arImage, &height);

		int32_t planes = 0;
		ArImage_getNumberOfPlanes(arSession, arImage, &planes);

		if (planes == 1 && width >= 1 && height >= 1)
		{
			const uint8_t* data = nullptr;

			int32_t planeDataLength = 0;
			int32_t planeDataRowStride = 0;
			int32_t planePixelStride = 0;
			ArImage_getPlaneData(arSession, arImage, 0, &data, &planeDataLength);
			ArImage_getPlaneRowStride(arSession, arImage, 0, &planeDataRowStride);
			ArImage_getPlanePixelStride(arSession, arImage, 0, &planePixelStride);

			if (planePixelStride != 2)
			{
				ocean_assert(false && "Invalid pixel stride");
				return Frame();
			}

			unsigned int paddingElements = 0u;
			if (!Frame::strideBytes2paddingElements(FrameType::FORMAT_Y16, (unsigned int)(width), (unsigned int)(planeDataRowStride), paddingElements))
			{
				ocean_assert(false && "Invalid stride");
				return Frame();
			}

			if (size_t(data) % 2 != 0)
			{
				ocean_assert(false && "Invalid data alignment");
				return Frame();
			}

			const uint16_t* yDepth = (const uint16_t*)(data);

			const Frame yDepthFrame(FrameType((unsigned int)(width), (unsigned int)(height), FrameType::FORMAT_Y16, FrameType::ORIGIN_UPPER_LEFT), yDepth, Frame::CM_USE_KEEP_LAYOUT, paddingElements);

			// now, converting the depth information from integer with millimeter precision to float (in meter precision)

			depthFrame = Frame(FrameType(yDepthFrame, FrameType::FORMAT_F32));

			for (unsigned int y = 0u; y < yDepthFrame.height(); ++y)
			{
				const uint16_t* const sourceRow = yDepthFrame.constrow<uint16_t>(y);
				float* const targetRow = depthFrame.row<float>(y);

				for (unsigned int x = 0u; x < yDepthFrame.width(); ++x)
				{
					targetRow[x] = float(sourceRow[x]) * float(0.001f);
				}
			}

			int64_t timestampNs = NumericT<int64_t>::minValue();
			ArImage_getTimestamp(arSession, arImage, &timestampNs);

			const Timestamp timestamp(double(timestampNs) * 0.000000001);

			depthFrame.setTimestamp(timestamp);
		}
	}
	else
	{
		Log::error() << "Not supported ArImageFormat for depth estimation: " << int(arImageFormat);
	}

	return depthFrame;
}

bool ARSessionManager::extractPose(ArSession* arSession, ArFrame* arFrame, HomogenousMatrixF4& world_T_camera, SharedAnyCamera& anyCamera)
{
	ocean_assert(arSession != nullptr);
	ocean_assert(arFrame != nullptr);

	ArCamera* arCamera = nullptr;
	ArFrame_acquireCamera(arSession, arFrame, &arCamera);
	ocean_assert(arCamera != nullptr);

	ArTrackingState arTrackingState;
	ArCamera_getTrackingState(arSession, arCamera, &arTrackingState);

	if (arTrackingState != AR_TRACKING_STATE_TRACKING)
	{
		return false;
	}

	float poseRaw[7];
	ScopedARPose arPose;
	ArPose_create(arSession, poseRaw, arPose.ingest());

	ArCamera_getPose(arSession, arCamera, arPose);
	ArPose_getMatrix(arSession, arPose, world_T_camera.data());

	ScopedARCameraIntrinsics arCameraIntrinsics(arSession, ArCameraIntrinsics_create);

	ArCamera_getImageIntrinsics(arSession, arCamera, arCameraIntrinsics);

	int32_t width = 0;
	int32_t height = 0;
	ArCameraIntrinsics_getImageDimensions(arSession, arCameraIntrinsics, &width, &height);

	float xFocal = 0.0f;
	float yFocal = 0.0f;
	ArCameraIntrinsics_getFocalLength(arSession, arCameraIntrinsics, &xFocal, &yFocal);

	float xPrincipal = -1.0f;
	float yPrincipal = -1.0f;
	ArCameraIntrinsics_getPrincipalPoint(arSession, arCameraIntrinsics, &xPrincipal, &yPrincipal);

	if (width > 0 && height >= 0)
	{
		anyCamera = std::make_shared<AnyCameraPinhole>(PinholeCamera((unsigned int)(width), (unsigned int)(height), Scalar(xFocal), Scalar(yFocal), Scalar(xPrincipal), Scalar(yPrincipal), PinholeCamera::DistortionPair(0, 0), PinholeCamera::DistortionPair(0, 0)));
	}

	return world_T_camera.isValid() && anyCamera && anyCamera->isValid();
}

bool ARSessionManager::extractPointCloud(ArSession* arSession, ArFrame* arFrame, Vectors3& objectPoints, Indices64& objectPointIds)
{
	ocean_assert(arSession != nullptr);
	ocean_assert(arFrame != nullptr);

	objectPoints.clear();
	objectPointIds.clear();

	ScopedARPointCloud arPointCloud;
	if (ArFrame_acquirePointCloud(arSession, arFrame, arPointCloud.ingest()) == AR_SUCCESS)
	{
		int32_t numberPoints = 0;
		ArPointCloud_getNumberOfPoints(arSession, arPointCloud, &numberPoints);

		if (numberPoints >= 1)
		{
			objectPoints.reserve(size_t(numberPoints));
			objectPointIds.reserve(size_t(numberPoints));

			const float* points = nullptr;
			ArPointCloud_getData(arSession, arPointCloud, &points);

			const int32_t* ids = nullptr;
			ArPointCloud_getPointIds(arSession, arPointCloud, &ids);

			ocean_assert(points != nullptr && ids != nullptr);

			for (int32_t n = 0; n < numberPoints; ++n)
			{
				objectPoints.emplace_back(Scalar(points[n * 3 + 0]), Scalar(points[n * 3 + 1]), Scalar(points[n * 3 + 2]));

				ocean_assert(ids[n] >= 0);
				objectPointIds.emplace_back(Index64(ids[n]));
			}
		}
	}

	return true;
}

}

}

}
