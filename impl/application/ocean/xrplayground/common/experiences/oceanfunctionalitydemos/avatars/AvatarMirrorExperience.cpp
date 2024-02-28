// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/avatars/AvatarMirrorExperience.h"

#include "ocean/cv/FrameInterpolatorNearestPixel.h"

#include "ocean/io/FileResolver.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/rendering/UndistortedBackground.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE
	#include <facetracker/manager/FaceTrackerManager_Ocean.h>
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE

using namespace Platform::Meta;

bool AvatarMirrorExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp /*timestamp*/, const std::string& /*properties*/)
{
	const uint64_t userId = Avatars::Manager::get().userId(); // id of local user

	if (userId != 0ull)
	{
		avatarScopedSubscription_ = Avatars::Manager::get().createAvatar(userId, std::bind(&AvatarMirrorExperience::onAvatarCreated, this, std::placeholders::_1, std::placeholders::_2));
		ocean_assert(avatarScopedSubscription_);
	}

	if (!avatarScopedSubscription_)
	{
		Log::error() << "Failed to determine local user";
		return false;
	}

	// by default, the background is showing the back-facing camera, we just stop the video stream

	if (!engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());
			undistortedBackground->setMedium(Media::FrameMediumRef());
			undistortedBackground->setVisible(false);
		}
	}

	faceTrackerFrameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");

	if (faceTrackerFrameMedium_.isNull() || !faceTrackerFrameMedium_->start())
	{
		Log::error() << "Failed to start the user-facing camera";
		return false;
	}

	startThread();

	return true;
}

bool AvatarMirrorExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	stopThreadExplicitly();

	faceTrackerFrameMedium_.release();
	avatarScopedSubscription_.release();

	// resetting, the background is showing the back-facing camera, we just stop the video stream

	if (!engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());

			Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium("LiveVideoId:0");

			if (frameMedium)
			{
				undistortedBackground->setVisible(true);
				undistortedBackground->setMedium(frameMedium);
				frameMedium->start();
			}
			else
			{
				Log::error() << "Failed to reset background video";
			}
		}
	}

	return true;
}

Timestamp AvatarMirrorExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);
			HomogenousMatrix4 userFacingCamera_T_head(userFacingCamera_T_head_);
			userFacingCamera_T_head_.toNull();
	scopedLock.release();

	if (userFacingCamera_T_head.isValid())
	{
		Avatars::Manager::get().updateLocalBodyTrackingData(Avatars::Input::createBodyTrackingData(userFacingCamera_T_head));
	}

	return timestamp;
}

std::unique_ptr<XRPlaygroundExperience> AvatarMirrorExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new AvatarMirrorExperience());
}

void AvatarMirrorExperience::onAvatarCreated(const uint64_t userId, Rendering::TransformRef avatarTransform)
{
	if (avatarTransform)
	{
		// placing the Avatar in front of the view

		avatarTransform->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.15), Scalar(-0.25))));
		experienceScene()->addChild(avatarTransform);

		Log::info() << "Created avatar for user '" << userId << "'";
	}
	else
	{
		Log::error() << "Failed to create rendering instance for avatar";
	}
}

void AvatarMirrorExperience::threadRun()
{
	const Media::FrameMediumRef frameMedium(faceTrackerFrameMedium_);
	ocean_assert(frameMedium);

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
			return;
		}

		fileMap.emplace(model, resolvedFiles.front()());
	}

	ocean_assert(fileMap.size() == 4);

	std::shared_ptr<facebook::facetracker::FaceTrackerManagerOcean> facetrackerManager = std::make_shared<facebook::facetracker::FaceTrackerManagerOcean>();

	facetrackerManager->loadModels(fileMap);

	Timestamp frameTimestamp(false);

	while (!shouldThreadStop())
	{
		SharedAnyCamera camera;
		FrameRef frame = frameMedium->frame(&camera);

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

		Quaternion device_R_camera(frameMedium->device_T_camera().rotation());

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

		facetrackerManager->setCalibration(float(camera->focalLengthX()), float(camera->focalLengthY()), float(camera->principalPointX()), float(camera->principalPointY()));
		facetrackerManager->run((uint8_t*)(yFrame.constdata<uint8_t>()), int(yFrame.width()), int(yFrame.height()), int(yFrame.strideBytes()), 0, false, trackedFaces, 1);

		if (!trackedFaces.empty())
		{
			const facebook::facetracker::TrackedFaceOcean& trackedFace = trackedFaces.front();

			HomogenousMatrix4 flippedUserFacingCamera_T_head(trackedFace.head_pose_mat.data(), false /*rowAligned*/);
			flippedUserFacingCamera_T_head.setTranslation(flippedUserFacingCamera_T_head.translation() * Scalar(0.001)); // approx mm to meter

			HomogenousMatrix4 userFacingCamera_T_head(AnyCamera::flippedTransformationLeftSide(flippedUserFacingCamera_T_head));

			userFacingCamera_T_head *= Quaternion(Vector3(0, 1, 0), Numeric::pi()); // Face tracker has y-axis upwards and z-axis pointing forwards away from the user

			const ScopedLock scopedLock(lock_);

			userFacingCamera_T_head_ = userFacingCamera_T_head;
		}
	}
}

#else // XR_PLAYGROUND_PLATFORM_BUILD_PHONE

AvatarMirrorExperience::~AvatarMirrorExperience()
{
	// nothing to do here
}

std::unique_ptr<XRPlaygroundExperience> AvatarMirrorExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new AvatarMirrorExperience());
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_PHONE

}

}
