// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/QuestCameraExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

QuestCameraExperience::~QuestCameraExperience()
{
	// nothing to do here
}

bool QuestCameraExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	bool allCamerasAccessed = true;

	for (unsigned int cameraIndex = 0u; cameraIndex < 4u; ++cameraIndex)
	{
		const std::string mediumUrl = "LiveVideoId:" + String::toAString(cameraIndex);

		Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(mediumUrl);

		if (frameMedium)
		{
			frameMedium->start();

			Rendering::TransformRef renderingTransform = Rendering::Utilities::createBox(engine, Vector3(Scalar(1), Scalar(1), 0), frameMedium);
			renderingTransform->setVisible(false);

			experienceScene()->addChild(renderingTransform);

			renderingTransforms_.emplace_back(std::move(renderingTransform));
			frameMediums_.emplace_back(std::move(frameMedium));
		}
		else
		{
			Log::warning() << "Failed to access '" << mediumUrl << "'";
			allCamerasAccessed = false;
		}
	}

	if (!allCamerasAccessed)
	{
		showMessage(MT_CAMERA_ACCESS_FAILED);
	}

	return true;
}

bool QuestCameraExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	frameMediums_.clear();

	renderingTransforms_.clear();

	return true;
}

Timestamp QuestCameraExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(renderingTransforms_.size() == frameMediums_.size());

	for (size_t n = 0; n < renderingTransforms_.size(); ++n)
	{
		const Rendering::TransformRef& renderingTransform = renderingTransforms_[n];
		const Media::FrameMediumRef& frameMedium = frameMediums_[n];

		const FrameRef frame = frameMedium->frame(); // latest frame

		if (frame)
		{
			HomogenousMatrixD4 device_T_camera = frameMedium->device_T_camera();
			device_T_camera = HomogenousMatrixD4(device_T_camera.translation() * 12.5, device_T_camera.rotationMatrix()); // increasing the translational part

			const Scalar factor = Scalar(0.9) / std::max(frame->width(), frame->height());

			const Scalar scaleX = Scalar(frame->width()) * factor;
			const Scalar scaleY = Scalar(frame->height()) * factor;

			HomogenousMatrix4 transformation = HomogenousMatrix4(Vector3(0, 0, -1)) * HomogenousMatrix4(device_T_camera);
			transformation.applyScale(Vector3(scaleX, scaleY, Scalar(1)));

			renderingTransform->setTransformation(transformation);
			renderingTransform->setVisible(true);
		}
	}

	HomogenousMatrix4 world_T_scene(true);

	if (!showInWorld_)
	{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
		world_T_scene = PlatformSpecific::get().world_T_device(timestamp);
#endif
	}

	if (world_T_scene.isValid())
	{
		experienceScene()->setVisible(true);
		experienceScene()->setTransformation(world_T_scene);
	}
	else
	{
		experienceScene()->setVisible(false);
	}

	return timestamp;
}

void QuestCameraExperience::onKeyPress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& key, const Timestamp /*timestamp*/)
{
	if (key == "A" || key == "X")
	{
		showInWorld_ = !showInWorld_;
	}
}

std::unique_ptr<XRPlaygroundExperience> QuestCameraExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new QuestCameraExperience());
}

}

}
