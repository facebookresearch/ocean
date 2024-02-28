// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/XRPlaygroundSharedSpaceExperience.h"

#include "application/ocean/xrplayground/common/ContentManager.h"
#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/cv/FrameInterpolatorNearestPixel.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/SceneTracker6DOF.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/io/FileResolver.h"

#include "ocean/media/Manager.h"

#include "ocean/network/verts/Manager.h"

#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

#ifdef OCEAN_PLATFORM_BUILD_MOBILE

using namespace Platform::Meta;

XRPlaygroundSharedSpaceExperience::FloorTracker::FloorTracker(const Media::FrameMediumRef& frameMedium)
{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	ocean_assert(frameMedium.isNull());

	tracker6DOF_ = Devices::Manager::get().device("Floor 6DOF Tracker");

	if (tracker6DOF_)
	{
		if (tracker6DOF_->start())
		{
			trackerSampleEventSubscription_ = tracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &FloorTracker::onTrackerSample));

			return;
		}
	}

#else

	if (frameMedium.isNull())
	{
		Log::warning() << "Floor tracker needs frame medium on mobile platforms";
		return;
	}

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	tracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF Plane Tracker");
#else
	tracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Plane Tracker");
#endif

	if (Devices::VisualTrackerRef visualTracker = tracker6DOF_)
	{
		visualTracker->setInput(frameMedium);
	}

	if (tracker6DOF_->start())
	{
		trackerSampleEventSubscription_ = tracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &FloorTracker::onTrackerSample));

		return;
	}

#endif

	Log::warning() << "Failed to start floor tracker";
}

Plane3 XRPlaygroundSharedSpaceExperience::FloorTracker::plane() const
{
	const ScopedLock scopedLock(lock_);

	return plane_;
}

void XRPlaygroundSharedSpaceExperience::FloorTracker::onTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
{
	TemporaryScopedLock temporaryScopedLock(lock_);
		if (planeTimestamp_ + 2.0 > sample->timestamp())
		{
			return;
		}
	temporaryScopedLock.release();

	if (measurement->name() == "Floor 6DOF Tracker")
	{
		const Devices::Tracker6DOF::Tracker6DOFSampleRef trackerSample(measurement->sample());

		if (sample && !trackerSample->objectIds().empty())
		{
			const ScopedLock scopedLock(lock_);

			plane_ = Plane3(trackerSample->positions().front(), Vector3(0, 1, 0));
			planeTimestamp_ = trackerSample->timestamp();
		}
	}
	else
	{
		using SceneElementPlanes = Devices::SceneTracker6DOF::SceneElementPlanes;

		const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTrackerSample(measurement->sample());

		if (sceneTrackerSample && !sceneTrackerSample->sceneElements().empty())
		{
			const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sceneTrackerSample->sceneElements().front();

			if (sceneElement && sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_PLANES)
			{
				const HomogenousMatrix4 world_T_camera(sceneTrackerSample->positions().front(), sceneTrackerSample->orientations().front());

				const Scalar yCameraTranslation = world_T_camera.translation().y();
				const Scalar yPlaneTranslationLower = yCameraTranslation - Scalar(2); // we expect the plane to be at with range [-2, -1] below the camera
				const Scalar yPlaneTranslationUpper = yCameraTranslation - Scalar(1);

				const SceneElementPlanes& sceneElementPlanes = dynamic_cast<const SceneElementPlanes&>(*sceneElement);

				const SceneElementPlanes::Planes& planes = sceneElementPlanes.planes();

				Scalar bestPlaneArea = Scalar(2 * 2); // a plane candidate must have at least 4 square meter
				Plane3 floorPlane;

				for (const SceneElementPlanes::Plane& plane : planes)
				{
					if (plane.planeType() != SceneElementPlanes::Plane::PT_HORIZONTAL)
					{
						continue;
					}

					const Scalar yPlaneTranslation = plane.world_T_plane().translation().y();

					if (yPlaneTranslation >= yPlaneTranslationLower && yPlaneTranslation <= yPlaneTranslationUpper)
					{
						ocean_assert(plane.boundingBox().yDimension() < Scalar(0.01));

						const Scalar planeArea = plane.boundingBox().xDimension() * plane.boundingBox().zDimension();

						if (planeArea > bestPlaneArea)
						{
							bestPlaneArea = planeArea;

							floorPlane = plane.worldPlane();
						}
					}
				}

				if (floorPlane.isValid())
				{
					const ScopedLock scopedLock(lock_);

					plane_ = floorPlane;
					planeTimestamp_ = sceneTrackerSample->timestamp();
				}
			}
		}
	}
}

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

XRPlaygroundSharedSpaceExperience::FaceTracker::FaceTracker(const Devices::Tracker6DOFRef& slamTracker, const Media::FrameMediumRef& slamFrameMedium)
{
	ocean_assert(slamTracker);


#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS

	faceTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Face Tracker");

	if (faceTracker6DOF_)
	{
		if (Devices::VisualTrackerRef visualTracker = faceTracker6DOF_)
		{
			visualTracker->setInput(slamFrameMedium); // although we use the front-facing camera, we need to specify the SLAM camera
		}

		faceTracker6DOF_->start();
	}
	else
	{
		Log::error() << "Failed to create face tracker";
	}

#else // OCEAN_PLATFORM_BUILD_APPLE_IOS

	frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");

	if (frameMedium_.isNull())
	{
		Log::error() << "Failed to accquire the user facing camera";
		return;
	}

	frameMedium_->setPreferredFrameDimension(1280u, 720u);
	frameMedium_->start();

	if (!initializeFacetracker())
	{
		Log::error() << "Failed to initialize the facetracker.";
		return;
	}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS

	startThread();

	slamTracker6DOF_ = slamTracker;
}

XRPlaygroundSharedSpaceExperience::FaceTracker::~FaceTracker()
{
	stopThreadExplicitly();
}

HomogenousMatrix4 XRPlaygroundSharedSpaceExperience::FaceTracker::world_T_head() const
{
	const ScopedLock scopedLock(lock_);

	return world_T_head_;
}

bool XRPlaygroundSharedSpaceExperience::FaceTracker::initializeFacetracker()
{
	ocean_assert(!facetrackerManager_);

	const std::vector<std::string> modelNames =
	{
		"face_detector_model.bin",
		"face_tracker_model.bin",
		"features_model.bin",
		"pdm_multires.bin"
	};

	std::unordered_map<std::string, boost::filesystem::path> fileMap;

	for (const std::string& model : modelNames)
	{
		const IO::Files resolvedFiles = IO::FileResolver::get().resolve(IO::File(model), true);

		if (resolvedFiles.empty())
		{
			Log::error() << "Failed to resolve file '" << model << "'";
			return false;
		}

		fileMap.emplace(model, resolvedFiles.front()());
	}

	ocean_assert(fileMap.size() == 4);

	facetrackerManager_ = std::make_shared<facebook::facetracker::FaceTrackerManagerOcean>();

	facetrackerManager_->loadModels(fileMap);

	return true;
}

void XRPlaygroundSharedSpaceExperience::FaceTracker::threadRun()
{

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS

	while (!shouldThreadStop())
	{
		if (faceTracker6DOF_ && slamTracker6DOF_)
		{
			const Devices::Tracker6DOF::Tracker6DOFSampleRef slamSample(slamTracker6DOF_->sample());
			const Devices::Tracker6DOF::Tracker6DOFSampleRef faceSample(faceTracker6DOF_->sample());

			if (slamSample && !slamSample->objectIds().empty() && faceSample && !faceSample->objectIds().empty())
			{
				const HomogenousMatrix4 world_T_camera(slamSample->positions().front(), slamSample->orientations().front());
				const HomogenousMatrix4 face_T_camera(faceSample->positions().front(), faceSample->orientations().front());

				HomogenousMatrix4 world_T_head = world_T_camera * face_T_camera.inverted();

				world_T_head *= Quaternion(Vector3(0, 1, 0), Numeric::pi()); // ARKit's face tracker has y-axis upwards and z-axis pointing forwards away from the user

				const ScopedLock scopedLock(lock_);

				world_T_head_ = world_T_head;
			}
		}

		sleep(1u);
	}

#else // OCEAN_PLATFORM_BUILD_APPLE_IOS

	ocean_assert(facetrackerManager_);
	ocean_assert(frameMedium_);

	Timestamp frameTimestamp(false);

	HomogenousMatrix4 slamCamera_T_device(true);

	if (Devices::VisualTrackerRef visualTracker = slamTracker6DOF_)
	{
		const Media::FrameMediumRefs frameMediums = visualTracker->input();

		if (frameMediums.size() == 1)
		{
			const HomogenousMatrix4 device_T_slamCamera(frameMediums.front()->device_T_camera());

			slamCamera_T_device = device_T_slamCamera.inverted();
		}
	}

	const HomogenousMatrix4 device_T_userFacingCamera = HomogenousMatrix4(frameMedium_->device_T_camera());

	const HomogenousMatrix4 slamCamera_T_userFacingCamera = slamCamera_T_device * device_T_userFacingCamera;

	while (!shouldThreadStop())
	{
		SharedAnyCamera camera;
		FrameRef frame = frameMedium_->frame(&camera);

		if (!frame || !frame->isValid() || !camera || !camera->isValid() || frame->timestamp() == frameTimestamp)
		{
			sleep(1u);
			continue;
		}

		frameTimestamp = frame->timestamp();

		Frame yFrame;
		if (!CV::FrameConverter::Comfort::convert(*frame, FrameType(*frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE))
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		Quaternion device_R_camera(frameMedium_->device_T_camera().rotation());

		if ((device_R_camera * Vector3(0, 0, 1)) * Vector3(0, 0, 1) < 0)
		{
			// the camera is pointing towards the opposite direction of the display (e.g., user-facing camera)
			device_R_camera = Quaternion(Vector3(0, 1, 0), Numeric::pi()) * device_R_camera;
		}

		const Euler cameraRotationEuler(device_R_camera);
		ocean_assert(Numeric::angleIsEqual(cameraRotationEuler.yaw(), 0, Numeric::deg2rad(1)));
		ocean_assert(Numeric::angleIsEqual(cameraRotationEuler.pitch(), 0, Numeric::deg2rad(1)));

		const int32_t zAxisRotationAngle = Numeric::round32(Numeric::rad2deg(cameraRotationEuler.roll()));
		ocean_assert(zAxisRotationAngle == 0 || zAxisRotationAngle == 90 || zAxisRotationAngle == -90);

		if (zAxisRotationAngle != 0)
		{
			const bool clockwise = zAxisRotationAngle < 0;

			Frame yRotatedFrame;
			if (!CV::FrameInterpolatorNearestPixel::Comfort::rotate90(yFrame, yRotatedFrame, clockwise))
			{
				ocean_assert(false && "This should never happen!");
				return;
			}
			std::swap(yRotatedFrame, yFrame);

			Scalar rotatedPrincipalX = Scalar(camera->height()) - camera->principalPointY();
			Scalar rotatedPrincipalY = camera->principalPointX();

			if (!clockwise)
			{
				rotatedPrincipalX = camera->principalPointY();
				rotatedPrincipalY = Scalar(camera->width()) - camera->principalPointX();
			}

			camera = std::make_shared<AnyCameraPinhole>(PinholeCamera(camera->height(), camera->width(), camera->focalLengthY(), camera->focalLengthX(), rotatedPrincipalX, rotatedPrincipalY));
		}

		std::vector<facebook::facetracker::TrackedFaceOcean> trackedFaces;

		facetrackerManager_->setCalibration(float(camera->focalLengthX()), float(camera->focalLengthY()), float(camera->principalPointX()), float(camera->principalPointY()));
		facetrackerManager_->run((uint8_t*)(yFrame.constdata<uint8_t>()), int(yFrame.width()), int(yFrame.height()), int(yFrame.strideBytes()), 0, false, trackedFaces, 1);

		if (!trackedFaces.empty())
		{
			const facebook::facetracker::TrackedFaceOcean& trackedFace = trackedFaces.front();

			HomogenousMatrix4 flippedUserFacingCamera_T_head(trackedFace.head_pose_mat.data(), false /*rowAligned*/);
			flippedUserFacingCamera_T_head.setTranslation(flippedUserFacingCamera_T_head.translation() * Scalar(0.001)); // approx mm to meter

			HomogenousMatrix4 userFacingCamera_T_head(AnyCamera::flippedTransformationLeftSide(flippedUserFacingCamera_T_head));

			userFacingCamera_T_head = HomogenousMatrix4(Quaternion(Vector3(0, 0, 1), Numeric::deg2rad(Scalar(-zAxisRotationAngle)))) * userFacingCamera_T_head; // adjustments due to rotated image

			userFacingCamera_T_head *= Quaternion(Vector3(0, 1, 0), Numeric::pi()); // Face tracker has y-axis upwards and z-axis pointing forwards away from the user

			// we access the SLAM camera pose for the current camera frame

			const Devices::Tracker6DOF::Tracker6DOFSampleRef sample = slamTracker6DOF_->sample(frameTimestamp);

			if (sample && !sample->objectIds().empty())
			{
				const HomogenousMatrix4 world_T_slamCamera(sample->positions().front(), sample->orientations().front());

				if (world_T_slamCamera.isValid())
				{
					const HomogenousMatrix4 world_T_head(world_T_slamCamera * slamCamera_T_userFacingCamera * userFacingCamera_T_head);

					const ScopedLock scopedLock(lock_);

					world_T_head_ = world_T_head;
				}
			}
		}
	}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS

}

#endif // #ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

XRPlaygroundSharedSpaceExperience::XRPlaygroundSharedSpaceExperience(const bool avatarsAtFloorLevel) :
	avatarsAtFloorLevel_(avatarsAtFloorLevel)
{
	// nothing to do here
}

bool XRPlaygroundSharedSpaceExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp /*timestamp*/, const std::string& /*properties*/)
{
	Log::debug() << "XRPlaygroundSharedSpaceExperience::load()";

	localUserId_ = Avatars::Manager::get().userId();

	if (localUserId_ != 0ull)
	{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
		Avatars::Manager::AvatarScopedSubscription avatarScopedSubscription = Avatars::Manager::get().createAvatar(localUserId_, std::bind(&XRPlaygroundSharedSpaceExperience::onAvatarCreated, this, std::placeholders::_1, std::placeholders::_2));
#else
		Avatars::Manager::AvatarScopedSubscription avatarScopedSubscription = Avatars::Manager::get().loadUser(localUserId_); // we do not render the local user on mobile
#endif
		ocean_assert(avatarScopedSubscription);

		if (avatarScopedSubscription)
		{
			avatarScopedSubscriptionMap_.emplace(localUserId_, std::move(avatarScopedSubscription));

			HomogenousMatrix4 remoteHeadsetWorld_T_remoteAvatar(true);
			onInitializeLocalUser(remoteHeadsetWorld_T_remoteAvatar);

			Avatars::Manager::get().setRemoteHeadsetWorld_T_remoteAvatar(remoteHeadsetWorld_T_remoteAvatar);
		}
	}
	else
	{
		Log::error() << "Failed to determine local user";
		return false;
	}

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	renderingTransformSharedSpace_ = engine->factory().createTransform();
	experienceScene()->addChild(renderingTransformSharedSpace_);

	floorTracker_ = std::make_shared<FloorTracker>();

#else

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	slamTracker_ = Devices::Manager::get().device("ARCore 6DOF World Tracker");
#else
	slamTracker_ = Devices::Manager::get().device("ARKit 6DOF World Tracker");
#endif

	if (slamTracker_.isNull())
	{
		Log::error() << "Failed to access World tracker";
		return false;
	}

	Media::FrameMediumRef slamFrameMedium;

	if (!engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());

			slamFrameMedium = undistortedBackground->medium();
		}
	}

	if (slamFrameMedium.isNull())
	{
		Log::error() << "Failed to determine SLAM medium";
		return false;
	}

	if (Devices::VisualTrackerRef visualTracker = slamTracker_)
	{
		visualTracker->setInput(slamFrameMedium);
	}

	if (!slamTracker_->start())
	{
		Log::error() << "Failed to start SLAM tracker";
		return false;
	}

	if (!anchoredContentManager_.initialize(std::bind(&XRPlaygroundSharedSpaceExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	renderingTransformSharedSpace_ = engine->factory().createTransform();

	const Devices::Tracker6DOF::ObjectId trackerObjectId = slamTracker_->objectId("World");

	if (trackerObjectId != Devices::Tracker6DOF::invalidObjectId())
	{
		constexpr Scalar visibilityRadius = Scalar(10);
		constexpr Scalar engagementRadius = Scalar(1000);
		anchoredContentManager_.addContent(renderingTransformSharedSpace_, slamTracker_, trackerObjectId, visibilityRadius, engagementRadius);
	}

	floorTracker_ = std::make_shared<FloorTracker>(slamFrameMedium);

	faceTracker_ = std::make_shared<FaceTracker>(slamTracker_, slamFrameMedium);

#endif //XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	ocean_assert(floorTracker_);
	ocean_assert(renderingTransformSharedSpace_);

	renderingGroupLocalAvatar_ = engine->factory().createGroup();
	experienceScene()->addChild(renderingGroupLocalAvatar_);

	renderingGroupRemoteAvatars_ = engine->factory().createGroup();
	renderingTransformSharedSpace_->addChild(renderingGroupRemoteAvatars_);

	renderingTransformEnvironmentContent_ = engine->factory().createTransform();
	renderingTransformSharedSpace_->addChild(renderingTransformEnvironmentContent_);

	renderingTransformEnvironmentContentFloorLevel_ = engine->factory().createTransform();
	renderingTransformEnvironmentContentFloorLevel_->setVisible(false); // waiting until we have determined the floor
	renderingTransformSharedSpace_->addChild(renderingTransformEnvironmentContentFloorLevel_);

	return true;
}

bool XRPlaygroundSharedSpaceExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	Log::debug() << "XRPlaygroundSharedSpaceExperience::unload()";

	ocean_assert(engine);

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	faceTracker_ = nullptr;
	slamTracker_.release();

	anchoredContentManager_.release();
#endif

	avatarRenderingMap_.clear();

	floorTracker_ = nullptr;

	avatarScopedSubscriptionMap_.clear();
	zoneScopedSubscription_.release();

	changedAvatarsScopedSubscription_.release();
	vertsDriver_ = nullptr;

	if (!currentContent_.empty())
	{
		ContentManager::get().unloadContent(currentContent_);
	}

	renderingTransformEnvironmentContent_.release();
	renderingTransformEnvironmentContentFloorLevel_.release();
	renderingGroupRemoteAvatars_.release();
	renderingGroupLocalAvatar_.release();
	renderingTransformSharedSpace_.release();

	return true;
}

Timestamp XRPlaygroundSharedSpaceExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	const HomogenousMatrix4 remoteHeadsetWorld_T_remoteAvatar = Avatars::Manager::get().remoteHeadsetWorld_T_remoteAvatar(localUserId_);

	if (remoteHeadsetWorld_T_remoteAvatar.isValid())
	{
		ocean_assert(renderingTransformSharedSpace_);
		renderingTransformSharedSpace_->setTransformation(remoteHeadsetWorld_T_remoteAvatar.inverted());
	}

	const Plane3 floorPlane = floorTracker_->plane();

	if (floorPlane.isValid())
	{
		const Scalar groundPlaneElevation = avatarsAtFloorLevel_ ? floorPlane.pointOnPlane().y() : 0;

		Avatars::Manager::get().setGroundPlaneElevation(groundPlaneElevation);

		renderingTransformEnvironmentContentFloorLevel_->setTransformation(HomogenousMatrix4(Vector3(0, floorPlane.pointOnPlane().y(), 0)));
		renderingTransformEnvironmentContentFloorLevel_->setVisible(true);

		hasValidFloor_ = true;
	}

	if (isUserMovementEnabled())
	{
		handleUserMovement(timestamp);
	}

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	const HomogenousMatrix4 world_T_head(faceTracker_->world_T_head());

	if (world_T_head.isValid())
	{
		Avatars::Manager::get().updateLocalBodyTrackingData(Avatars::Input::createBodyTrackingData(world_T_head));
	}

	if (floorPlane.isValid())
	{
		// let's render a shadow below the avatars for better depth perception

		for (AvatarRenderingMap::const_iterator iAvatar = avatarRenderingMap_.cbegin(); iAvatar != avatarRenderingMap_.cend(); ++iAvatar)
		{
			const HomogenousMatrix4 world_T_avatarHead = Avatars::Manager::get().world_T_head(iAvatar->first);

			if (world_T_avatarHead.isValid())
			{
				const Rendering::TransformRef& avatarTransform = iAvatar->second;

				if (avatarTransform->numberChildren() == 1u)
				{
					avatarTransform->addChild(Rendering::Utilities::createCylinder(engine, Scalar(0.25), Scalar(0.02), RGBAColor(0.0f, 0.0f, 0.0f, 0.2f)));
				}

				ocean_assert(avatarTransform->numberChildren() == 2u);

				const Rendering::TransformRef shadowTransform = avatarTransform->child(1u);

				const Vector3 shadowPosition = floorPlane.projectOnPlane(world_T_avatarHead.translation());

				shadowTransform->setTransformation(HomogenousMatrix4(shadowPosition));
			}
		}
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);

#else

	return timestamp;

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

bool XRPlaygroundSharedSpaceExperience::hasValidFloor() const
{
	return hasValidFloor_;
}

bool XRPlaygroundSharedSpaceExperience::initializeNetworkZone(const std::string& zoneName)
{
	ocean_assert(!zoneScopedSubscription_);
	ocean_assert(!changedAvatarsScopedSubscription_);

	vertsDriver_ = Network::Verts::Manager::get().driver(zoneName);

	zoneScopedSubscription_ = Avatars::Manager::get().joinZone(zoneName);

	changedAvatarsScopedSubscription_ = Avatars::Manager::get().addChangedAvatarsCallback(std::bind(&XRPlaygroundSharedSpaceExperience::onChangedAvatars, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	return zoneScopedSubscription_ && changedAvatarsScopedSubscription_;
}

bool XRPlaygroundSharedSpaceExperience::loadContent(const std::string& content, const bool floorLevel)
{
	Log::debug() << "Loading content '" << content << "'";

	ocean_assert(renderingTransformEnvironmentContent_ && renderingTransformEnvironmentContentFloorLevel_);
	renderingTransformEnvironmentContent_->clear();
	renderingTransformEnvironmentContentFloorLevel_->clear();

	if (!currentContent_.empty())
	{
		ContentManager::get().unloadContent(currentContent_);
		currentContent_.clear();
	}

	return ContentManager::get().loadContent(content, ContentManager::LM_LOAD_ONLY, std::bind(&XRPlaygroundSharedSpaceExperience::onContentHandled, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, floorLevel));
}

bool XRPlaygroundSharedSpaceExperience::loadContent(const Rendering::NodeRef& node, const bool floorLevel)
{
	ocean_assert(renderingTransformEnvironmentContent_ && renderingTransformEnvironmentContentFloorLevel_);
	renderingTransformEnvironmentContent_->clear();
	renderingTransformEnvironmentContentFloorLevel_->clear();

	if (!currentContent_.empty())
	{
		ContentManager::get().unloadContent(currentContent_);
		currentContent_.clear();
	}

	if (node)
	{
		Log::debug() << "Loading content given as a rendering node";

		if (floorLevel)
		{
			renderingTransformEnvironmentContentFloorLevel_->addChild(node);
		}
		else
		{
			renderingTransformEnvironmentContent_->addChild(node);
		}

		return true;
	}

	return false;
}

bool XRPlaygroundSharedSpaceExperience::localAvatarVisible() const
{
	ocean_assert(renderingGroupLocalAvatar_);
	return renderingGroupLocalAvatar_->visible();
}

void XRPlaygroundSharedSpaceExperience::setLocalAvatarVisible(const bool state)
{
	ocean_assert(renderingGroupLocalAvatar_);
	renderingGroupLocalAvatar_->setVisible(state);
}

void XRPlaygroundSharedSpaceExperience::setUserMovementEnabled(const bool enable)
{
	userMovementEnabled_ = enable;

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	world_T_slamCameraStart_.toNull();
#endif
}

bool XRPlaygroundSharedSpaceExperience::isUserMovementEnabled() const
{
	return userMovementEnabled_;
}

void XRPlaygroundSharedSpaceExperience::handleUserMovement(const Timestamp& timestamp)
{
	ocean_assert(isUserMovementEnabled());

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	// let's see whether the user want's to move

	const Vector2 joystickTiltLeft = PlatformSpecific::get().trackedRemoteDevice().joystickTilt(Platform::Meta::Quest::VrApi::TrackedRemoteDevice::RT_LEFT);
	const Vector2 joystickTiltRight = PlatformSpecific::get().trackedRemoteDevice().joystickTilt(Platform::Meta::Quest::VrApi::TrackedRemoteDevice::RT_RIGHT);

	if (joystickTiltLeft.sqr() > joystickTiltRight.sqr())
	{
		// left joystick supports smoothly flying through the space

		const Vector2& joystickTilt = joystickTiltLeft;

		const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(timestamp);

		// create a pitch/roll-free device transformation

		const Vector3 yAxis(0, 1, 0);

		Vector3 zAxis = world_T_device.xAxis().cross(yAxis);
		Vector3 xAxis = yAxis.cross(zAxis);

		if (zAxis.normalize() && xAxis.normalize())
		{
			const Quaternion world_T_deviceYaw = HomogenousMatrix4(xAxis, yAxis, zAxis).rotation();

			const Vector3 translationDevice(-joystickTilt.x(), 0, joystickTilt.y());

			HomogenousMatrix4 worldHeadset_T_worldPortal = Avatars::Manager::get().remoteHeadsetWorld_T_remoteAvatar(localUserId_);
			worldHeadset_T_worldPortal.setTranslation(worldHeadset_T_worldPortal.translation() + world_T_deviceYaw * translationDevice * Scalar(0.01));

			Avatars::Manager::get().setRemoteHeadsetWorld_T_remoteAvatar(worldHeadset_T_worldPortal);
		}
	}
	else if (jumpStartTimestamp_.isInvalid())
	{
		// right controller supports jump movements (to address motion sickness)

		const Vector2& joystickTilt = joystickTiltRight;

		if (Numeric::abs(joystickTilt.x()) > Scalar(0.6) || Numeric::abs(joystickTilt.y()) > Scalar(0.6))
		{
			const HomogenousMatrix4 world_T_device = PlatformSpecific::get().world_T_device(timestamp);

			// create a pitch/roll-free device transformation

			const Vector3 yAxis(0, 1, 0);

			Vector3 zAxis = world_T_device.xAxis().cross(yAxis);
			Vector3 xAxis = yAxis.cross(zAxis);

			if (zAxis.normalize() && xAxis.normalize())
			{
				const Quaternion world_T_deviceYaw = HomogenousMatrix4(xAxis, yAxis, zAxis).rotation();

				Vector3 translationDevice(-joystickTilt.x(), 0, joystickTilt.y());
				translationDevice.normalize();

				HomogenousMatrix4 worldHeadset_T_worldPortal = Avatars::Manager::get().remoteHeadsetWorld_T_remoteAvatar(localUserId_);
				worldHeadset_T_worldPortal.setTranslation(worldHeadset_T_worldPortal.translation() + world_T_deviceYaw * translationDevice * Scalar(1)); // make 1 meter jumps

				Avatars::Manager::get().setRemoteHeadsetWorld_T_remoteAvatar(worldHeadset_T_worldPortal);

				jumpStartTimestamp_ = timestamp;

				// for the jump, we will disable mesh rendering for a short time
				renderingTransformEnvironmentContent_->setVisible(false);
				renderingTransformEnvironmentContentFloorLevel_->setVisible(false);
			}
		}
	}

	if (jumpStartTimestamp_.isValid() && timestamp > jumpStartTimestamp_ + 0.25)
	{
		renderingTransformEnvironmentContent_->setVisible(true);
		renderingTransformEnvironmentContentFloorLevel_->setVisible(true);

		if (joystickTiltLeft.isNull() && joystickTiltRight.isNull())
		{
			// allow a new jump movement
			jumpStartTimestamp_.toInvalid();
		}
	}

#else // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	const Devices::Tracker6DOF::Tracker6DOFSampleRef sample = slamTracker_->sample(timestamp);

	if (sample && !sample->objectIds().empty())
	{
		const HomogenousMatrix4 world_T_slamCamera(sample->positions().front(), sample->orientations().front());

		if (!world_T_slamCameraStart_.isValid())
		{
			world_T_slamCameraStart_ = world_T_slamCamera;
		}

		const HomogenousMatrix4 startWorld_T_currentWorld = world_T_slamCameraStart_ * world_T_slamCamera.inverted();

		HomogenousMatrix4 worldHeadset_T_worldPortal = Avatars::Manager::get().remoteHeadsetWorld_T_remoteAvatar(localUserId_);
		worldHeadset_T_worldPortal.setTranslation(worldHeadset_T_worldPortal.translation() + Vector3(startWorld_T_currentWorld.translation().x(), 0, startWorld_T_currentWorld.translation().z()));
		Avatars::Manager::get().setRemoteHeadsetWorld_T_remoteAvatar(worldHeadset_T_worldPortal);

		world_T_slamCameraStart_ = world_T_slamCamera;
	}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
}

void XRPlaygroundSharedSpaceExperience::onInitializeLocalUser(HomogenousMatrix4& /*remoteHeadsetWorld_T_remoteAvatar*/)
{
	// nothing to do here
}

void XRPlaygroundSharedSpaceExperience::onChangedAvatars(const std::string& zoneName, const Avatars::Manager::UserPairs& addedAvatars, const Indices64& removedAvatars)
{
	for (const Avatars::Manager::UserPair& userPair : addedAvatars)
	{
		const uint64_t userId = userPair.first;
		const Avatars::Manager::UserType userType = userPair.second;

		ocean_assert(avatarScopedSubscriptionMap_.find(userId) == avatarScopedSubscriptionMap_.cend());

		Avatars::Manager::AvatarScopedSubscription avatarScopedSubscription = Avatars::Manager::get().createAvatar(userId, std::bind(&XRPlaygroundSharedSpaceExperience::onAvatarCreated, this, std::placeholders::_1, std::placeholders::_2), userType);

		if (avatarScopedSubscription)
		{
			avatarScopedSubscriptionMap_.emplace(userId, std::move(avatarScopedSubscription));
		}
	}

	for (const uint64_t userId : removedAvatars)
	{
		ocean_assert(avatarScopedSubscriptionMap_.find(userId) != avatarScopedSubscriptionMap_.cend());
		avatarScopedSubscriptionMap_.erase(userId);

		onAvatarRemoved(userId);
	}
}

void XRPlaygroundSharedSpaceExperience::onAvatarCreated(const uint64_t userId, Rendering::TransformRef avatarTransform)
{
	if (avatarTransform)
	{
		if (userId == localUserId_)
		{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
			PlatformSpecific::get().vrHandVisualizer().hide();
			PlatformSpecific::get().vrControllerVisualizer().hide();

			renderingGroupLocalAvatar_->addChild(avatarTransform);
#else
			ocean_assert(false && "The local user should not be rendered on mobile");
#endif
		}
		else
		{
			renderingGroupRemoteAvatars_->addChild(avatarTransform);

			ocean_assert(avatarRenderingMap_.find(userId) == avatarRenderingMap_.cend());
			avatarRenderingMap_.emplace(userId, avatarTransform);
		}
	}
	else
	{
		Log::error() << "Failed to create rendering instance for user " << userId;
	}
}

void XRPlaygroundSharedSpaceExperience::onAvatarRemoved(const uint64_t userId)
{
	AvatarRenderingMap::iterator iAvatar = avatarRenderingMap_.find(userId);

	ocean_assert(iAvatar != avatarRenderingMap_.cend());

	if (iAvatar != avatarRenderingMap_.cend())
	{
		renderingGroupRemoteAvatars_->removeChild(iAvatar->second);

		avatarRenderingMap_.erase(iAvatar);
	}
}

void XRPlaygroundSharedSpaceExperience::onContentHandled(const std::string& content, const bool succeeded, const std::vector<Rendering::SceneRef>& scenes, const bool floorLevel)
{
	if (!succeeded || scenes.size() != 1)
	{
		Log::error() << "Failed to handle content!";
		return;
	}

	Log::debug() << "Successfully loaded content '" << content << "'";

	ocean_assert(renderingTransformEnvironmentContent_ && renderingTransformEnvironmentContentFloorLevel_);

	if (floorLevel)
	{
		renderingTransformEnvironmentContentFloorLevel_->addChild(scenes.front());
	}
	else
	{
		renderingTransformEnvironmentContent_->addChild(scenes.front());
	}

	ocean_assert(currentContent_.empty());
	currentContent_ = content;
}

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

void XRPlaygroundSharedSpaceExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Objects should never be removed by the anchor manager, as we have a very large engagement radius");
}

#endif

#endif // OCEAN_PLATFORM_BUILD_MOBILE

}

}
