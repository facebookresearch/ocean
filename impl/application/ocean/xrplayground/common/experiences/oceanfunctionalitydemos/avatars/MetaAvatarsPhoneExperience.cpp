// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/avatars/MetaAvatarsPhoneExperience.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/geometry/Utilities.h"

#include "ocean/io/FileResolver.h"

#include "ocean/media/Manager.h"

#include "metaonly/ocean/network/verts/Manager.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/Text.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE

using namespace Platform::Meta;

MetaAvatarsPhoneExperience::~MetaAvatarsPhoneExperience()
{
	// nothing to do here
}

bool MetaAvatarsPhoneExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties)
{
	if (!MetaAvatarsExperience::load(userInterface, engine, timestamp, properties))
	{
		return false;
	}

	if (!initializeFacetracker())
	{
		Log::error() << "Failed to initialize the facetracker.";
		return false;
	}

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS
	slamTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF World Tracker");
#else
	slamTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF World Tracker");
#endif

	if (slamTracker6DOF_.isNull())
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

	if (Devices::VisualTrackerRef visualTracker = slamTracker6DOF_)
	{
		visualTracker->setInput(slamFrameMedium);
	}

	if (!slamTracker6DOF_->start())
	{
		Log::error() << "Failed to start SLAM tracker";
		return false;
	}

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS
	planeTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Plane Tracker");
#else
	planeTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF Plane Tracker");
#endif

	if (Devices::VisualTrackerRef visualTracker = planeTracker6DOF_)
	{
		visualTracker->setInput(slamFrameMedium);
	}

	if (!planeTracker6DOF_->start())
	{
		Log::warning() << "Failed to start plane tracker";
	}

	userFacingFrameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");

	if (userFacingFrameMedium_.isNull())
	{
		Log::error() << "Failed to accquire the user facing camera while running ARCore as well";
		return false;
	}

	userFacingFrameMedium_->setPreferredFrameDimension(1280u, 720u);
	userFacingFrameMedium_->start();

	if (!anchoredContentManager_.initialize(std::bind(&MetaAvatarsPhoneExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	renderingGroupAnchored_ = engine->factory().createGroup();

	ocean_assert(renderingTransformRemoteAvatars_);
	renderingGroupAnchored_->addChild(renderingTransformRemoteAvatars_);

	const Devices::Tracker6DOF::ObjectId trackerObjectId = slamTracker6DOF_->objectId("World");

	if (trackerObjectId != Devices::Tracker6DOF::invalidObjectId())
	{
		constexpr Scalar visibilityRadius = Scalar(10);
		constexpr Scalar engagementRadius = Scalar(1000);
		anchoredContentManager_.addContent(renderingGroupAnchored_, slamTracker6DOF_, trackerObjectId, visibilityRadius, engagementRadius);
	}

	zoneScopedSubscription_ = Avatars::Manager::get().joinZone(vertsPublicZoneName_);

	const Rendering::AbsoluteTransformRef absoluteTransform = engine->factory().createAbsoluteTransform();
	absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
	absoluteTransform->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.065)));
	experienceScene()->addChild(absoluteTransform);

	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, " Your are in the public zone, \n mirroring you avatar until someone joins ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0.005, 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingText_);

	absoluteTransform->addChild(textTransform);

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

#else

	// on Android, we use the front-facing camera and a standalone face tracker
	startThread();

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS


	return true;
}

bool MetaAvatarsPhoneExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	stopThreadExplicitly();

	renderingText_.release();
	renderingGroupAnchored_.release();

	avatarRenderingMap_.clear();

	anchoredContentManager_.release();

	userFacingFrameMedium_.release();

	facetrackerManager_ = nullptr;

	return MetaAvatarsExperience::unload(userInterface, engine, timestamp);
}

Timestamp MetaAvatarsPhoneExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	MetaAvatarsExperience::preUpdate(userInterface, engine, view, timestamp);

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS

	HomogenousMatrix4 world_T_head(false);

	if (faceTracker6DOF_ && slamTracker6DOF_)
	{
		const Devices::Tracker6DOF::Tracker6DOFSampleRef slamSample(slamTracker6DOF_->sample(timestamp));
		const Devices::Tracker6DOF::Tracker6DOFSampleRef faceSample(faceTracker6DOF_->sample(timestamp));

		if (slamSample && !slamSample->objectIds().empty() && faceSample && !faceSample->objectIds().empty())
		{
			const HomogenousMatrix4 world_T_camera(slamSample->positions().front(), slamSample->orientations().front());
			const HomogenousMatrix4 face_T_camera(faceSample->positions().front(), faceSample->orientations().front());

			world_T_head = world_T_camera * face_T_camera.inverted();

			world_T_head *= Quaternion(Vector3(0, 1, 0), Numeric::pi()); // ARKit's face tracker has y-axis upwards and z-axis pointing forwards away from the user
		}
	}

#else

	TemporaryScopedLock scopedLock(lock_);
		const HomogenousMatrix4 world_T_head(world_T_head_);
		world_T_head_.toNull();
	scopedLock.release();

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS

	if (world_T_head.isValid())
	{
		Avatars::Manager::get().updateLocalBodyTrackingData(Avatars::Input::createBodyTrackingData(world_T_head));
	}

	if (timestamp > groupPlaneTimestamp_ + 2.0)
	{
		if (updateGroundPlane())
		{
			ocean_assert(groundPlane_);
			Avatars::Manager::get().setGroundPlaneElevation(groundPlane_.pointOnPlane().y());
		}
	}

	if (groundPlane_)
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
					avatarTransform->addChild(Rendering::Utilities::createCylinder(engine, Scalar(0.25), Scalar(0.001), RGBAColor(0.0f, 0.0f, 0.0f, 0.2f)));
				}

				ocean_assert(avatarTransform->numberChildren() == 2u);

				const Rendering::TransformRef shadowTransform = avatarTransform->child(1u);

				const Vector3 shadowPosition = groundPlane_.projectOnPlane(world_T_avatarHead.translation());

				shadowTransform->setTransformation(HomogenousMatrix4(shadowPosition));
			}
		}
	}

	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	return anchoredContentManager_.preUpdate(engine, view, timestamp);
}

std::unique_ptr<XRPlaygroundExperience> MetaAvatarsPhoneExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new MetaAvatarsPhoneExperience());
}

bool MetaAvatarsPhoneExperience::initializeFacetracker()
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

void MetaAvatarsPhoneExperience::onAvatarCreated(const uint64_t userId, Rendering::TransformRef avatarTransform)
{
	if (avatarTransform)
	{
		if (userId == userId_)
		{
			// we place the remote user 1meter front of use looking towards us, so that we have something close to a mirror
			avatarTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -1.5), Quaternion(Vector3(0, 1, 0), Numeric::pi())));

			renderingGroupAnchored_->addChild(avatarTransform);
		}
		else
		{
			if (!remoteUserExists_)
			{
				remoteUserExists_ = true;

				// removing our own local avatar
				renderingGroupAnchored_->clear();
				renderingGroupAnchored_->addChild(renderingTransformRemoteAvatars_);

				avatarRenderingMap_.clear();

				renderingText_->setText("");
			}

			renderingTransformRemoteAvatars_->addChild(avatarTransform);
		}

		ocean_assert(avatarRenderingMap_.find(userId) == avatarRenderingMap_.cend());
		avatarRenderingMap_.emplace(userId, avatarTransform);
	}
	else
	{
		Log::error() << "Failed to create rendering instance for user " << userId;
	}
}

void MetaAvatarsPhoneExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& /*removedObjects*/)
{
	ocean_assert(false && "Objects should never be removed by the anchor manager, as we have a very large engagement radius");
}

void MetaAvatarsPhoneExperience::threadRun()
{
	ocean_assert(facetrackerManager_);
	ocean_assert(userFacingFrameMedium_);

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

	const HomogenousMatrix4 device_T_userFacingCamera = HomogenousMatrix4(userFacingFrameMedium_->device_T_camera());

	const HomogenousMatrix4 slamCamera_T_userFacingCamera = slamCamera_T_device * device_T_userFacingCamera;

	while (!shouldThreadStop())
	{
		SharedAnyCamera camera;
		FrameRef frame = userFacingFrameMedium_->frame(&camera);

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

		Quaternion device_R_camera(userFacingFrameMedium_->device_T_camera().rotation());

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
}

bool MetaAvatarsPhoneExperience::updateGroundPlane()
{
	using SceneElementPlanes = Devices::SceneTracker6DOF::SceneElementPlanes;

	const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneTrackerSample(planeTracker6DOF_->sample());

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
			Plane3 groundPlane;

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

						groundPlane = plane.worldPlane();
					}
				}
			}

			if (groundPlane.isValid())
			{
				groundPlane_ = groundPlane;

				return true;
			}
		}
	}

	return false;
}

#else // XR_PLAYGROUND_PLATFORM_BUILD_PHONE

MetaAvatarsPhoneExperience::~MetaAvatarsPhoneExperience()
{
	// nothing to do here
}

std::unique_ptr<XRPlaygroundExperience> MetaAvatarsPhoneExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new MetaAvatarsPhoneExperience());
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_PHONE

}

}
