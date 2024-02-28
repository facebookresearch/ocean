// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/QuestCameraAnalyzerExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

QuestCameraAnalyzerExperience::FrameStatistic::FrameStatistic(FrameStatistic&& frameStatistic)
{
	*this = std::move(frameStatistic);
}

bool QuestCameraAnalyzerExperience::FrameStatistic::addFrame(const std::string& cameraName, const Frame& yFrame)
{
	if (!yFrame.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		Log::error() << "Pixel format is not supported " << FrameType::translatePixelFormat(yFrame.pixelFormat());
		return false;
	}

	unsigned int saturatedPixels = 0u;

	uint64_t sumIntensity = 0ull;

	for (unsigned int y = 0u; y < yFrame.height(); ++y)
	{
		const uint8_t* yRow = yFrame.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < yFrame.width(); ++x)
		{
			if (yRow[x] == uint8_t(0xFFu))
			{
				++saturatedPixels;
			}

			sumIntensity += uint64_t(yRow[x]);
		}
	}

	const double meanIntensity = double(sumIntensity) / double(yFrame.pixels());

	const ScopedLock scopedLock(lock_);

	cameraName_ = cameraName;

	meanIntensity_ = meanIntensity;
	saturatedPixels_ = double(saturatedPixels) / double(yFrame.pixels());

	meanIntensityVariance_.add(meanIntensity_);
	meanIntensityQueue_.push(meanIntensity_);

	while (meanIntensityQueue_.size() > 60)
	{
		meanIntensityVariance_.remove(meanIntensityQueue_.front());
		meanIntensityQueue_.pop();
	}

	updated_ = true;

	return true;
}

bool QuestCameraAnalyzerExperience::FrameStatistic::recentResults(std::string& cameraName, double& meanIntensity, double& saturatedPixels, double& averageMeanIntensity, double& deviationMeanIntensity) const
{
	const ScopedLock scopedLock(lock_);

	if (updated_)
	{
		cameraName = std::move(cameraName_);

		meanIntensity = meanIntensity_;
		saturatedPixels = saturatedPixels_;

		averageMeanIntensity = meanIntensityVariance_.average();
		deviationMeanIntensity = meanIntensityVariance_.deviation();

		updated_ = false;

		return true;
	}

	return false;
}

void QuestCameraAnalyzerExperience::FrameStatistic::reset()
{
	const ScopedLock scopedLock(lock_);

	meanIntensity_ = -1.0;
	saturatedPixels_ = -1.0;

	meanIntensityVariance_ = VarianceD();
	meanIntensityQueue_ = Queue();

	updated_ = false;
}

QuestCameraAnalyzerExperience::FrameStatistic& QuestCameraAnalyzerExperience::FrameStatistic::operator=(FrameStatistic&& frameStatistic)
{
	if (this != &frameStatistic)
	{
		meanIntensity_ = frameStatistic.meanIntensity_;
		saturatedPixels_ = frameStatistic.saturatedPixels_;

		frameStatistic.meanIntensity_ = -1.0;
		frameStatistic.saturatedPixels_ = -1.0;
	}

	return *this;
}

QuestCameraAnalyzerExperience::~QuestCameraAnalyzerExperience()
{
	// nothing to do here
}

bool QuestCameraAnalyzerExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	bool allCamerasAccessed = true;

	for (unsigned int cameraIndex = 0u; cameraIndex < 2u; ++cameraIndex)
	{
		const std::string mediumUrl = "StereoCamera0Id:" + String::toAString(cameraIndex);

		Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(mediumUrl);

		if (frameMedium && frameMedium->start())
		{
			frameMediums_.emplace_back(std::move(frameMedium));

			Rendering::TextRef text;

			Rendering::TransformRef renderingTransformText = Rendering::Utilities::createText(*engine, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.05), Rendering::Text::AM_LEFT, Rendering::Text::HA_CENTER, Rendering::Text::VA_TOP, "", "", &text);
			renderingTransformText->setVisible(false);

			experienceScene()->addChild(renderingTransformText);

			renderingTransformTexts_.emplace_back(std::move(renderingTransformText));
			renderingTexts_.emplace_back(std::move(text));
		}
		else
		{
			Log::warning() << "Failed to access '" << mediumUrl << "'";
			allCamerasAccessed = false;
		}
	}

	if (allCamerasAccessed)
	{
		frameStatistics_.resize(frameMediums_.size());

		newCamerasAvailable_ = true;

		startThread();
	}
	else
	{
		showMessage(MT_CAMERA_ACCESS_FAILED);
	}

	return true;
}

bool QuestCameraAnalyzerExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	stopThreadExplicitly();

	frameMediums_.clear();

	renderingTransformCameras_.clear();
	renderingTransformTexts_.clear();
	renderingTexts_.clear();

	return true;
}

Timestamp QuestCameraAnalyzerExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	{
		const ScopedLock scopedLock(frameMediumsLock_);

		if (newCamerasAvailable_)
		{
			for (const Rendering::TransformRef& renderingTransformCamera : renderingTransformCameras_)
			{
				experienceScene()->removeChild(renderingTransformCamera);
			}

			renderingTransformCameras_.clear();

			for (const Media::FrameMediumRef& frameMedium : frameMediums_)
			{
				Rendering::TransformRef renderingTransformCamera = Rendering::Utilities::createBox(engine, Vector3(Scalar(1), Scalar(1), 0), frameMedium);

				experienceScene()->addChild(renderingTransformCamera);

				renderingTransformCameras_.emplace_back(std::move(renderingTransformCamera));
			}

			newCamerasAvailable_ = false;
		}

		ocean_assert(renderingTransformCameras_.size() == frameMediums_.size());
		ocean_assert(renderingTransformCameras_.size() == renderingTransformTexts_.size());

		for (size_t n = 0; n < renderingTransformCameras_.size(); ++n)
		{
			const Rendering::TransformRef& renderingTransformCamera = renderingTransformCameras_[n];
			const Rendering::TransformRef& renderingTransformText = renderingTransformTexts_[n];
			const Media::FrameMediumRef& frameMedium = frameMediums_[n];

			const FrameRef frame = frameMedium->frame(); // latest frame

			if (frame)
			{

				HomogenousMatrixD4 device_T_camera = frameMedium->device_T_camera();
				device_T_camera = HomogenousMatrixD4(device_T_camera.translation() * 12.5, device_T_camera.rotationMatrix()); // increasing the translational part

				const Scalar factor = Scalar(0.9) / std::max(frame->width(), frame->height());

				const Scalar scaleX = Scalar(frame->width()) * factor;
				const Scalar scaleY = Scalar(frame->height()) * factor;

				HomogenousMatrix4 transformationCamera = HomogenousMatrix4(Vector3(0, 0, -1)) * HomogenousMatrix4(device_T_camera);
				transformationCamera.applyScale(Vector3(scaleX, scaleY, Scalar(1)));

				renderingTransformCamera->setTransformation(transformationCamera);
				renderingTransformCamera->setVisible(true);

				const HomogenousMatrix4 transformationText = HomogenousMatrix4(Vector3(0, 0, -1)) * HomogenousMatrix4(Vector3(device_T_camera.translation()) + Vector3(0, Scalar(0.9), Scalar(0.01)));
				renderingTransformText->setTransformation(transformationText);
				renderingTransformText->setVisible(true);
			}
		}
	}

	for (size_t n = 0; n < renderingTexts_.size(); ++n)
	{
		std::string cameraName;

		double meanIntensity;
		double saturatedPixels;
		double averageMeanIntensity;
		double deviationMeanIntensity;

		if (frameStatistics_[n].recentResults(cameraName, meanIntensity, saturatedPixels, averageMeanIntensity, deviationMeanIntensity))
		{
			std::string text = cameraName + ":\n\n"
									+ "Mean intensity: " + String::toAString(meanIntensity, 1u) + "\n"
									+ "     moving average: " + String::toAString(averageMeanIntensity, 1u) + "\n"
									+ "     moving deviation: " + String::toAString(deviationMeanIntensity, 1u) + "\n\n"
									+ "Saturated pixels: " + String::toAString(saturatedPixels * 100.0, 2u) + "%";

			renderingTexts_[n]->setText(text);
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

void QuestCameraAnalyzerExperience::onKeyPress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& key, const Timestamp /*timestamp*/)
{
	if (key == "A" || key == "X")
	{
		showInWorld_ = !showInWorld_;
	}

	if (key == "B" || key == "Y")
	{
		toggleCameras_ = true;
	}
}

std::unique_ptr<XRPlaygroundExperience> QuestCameraAnalyzerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new QuestCameraAnalyzerExperience());
}

void QuestCameraAnalyzerExperience::threadRun()
{
	unsigned int stereoCameraId = 0u;
	std::string cameraName = "Generic Slot 0";

	Timestamp lastFrameTimestamp(false);

	while (!shouldThreadStop())
	{
		if (toggleCameras_.exchange(false))
		{
			const unsigned int nextStereoCameraId = (stereoCameraId + 1u) % 2u;

			Media::FrameMediumRefs frameMediums;

			for (unsigned int cameraIndex = 0u; cameraIndex < 2u; ++cameraIndex)
			{
				const std::string mediumUrl = "StereoCamera" + String::toAString(nextStereoCameraId) + "Id:" + String::toAString(cameraIndex);

				Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(mediumUrl);

				if (frameMedium && frameMedium->start())
				{
					frameMediums.emplace_back(std::move(frameMedium));
				}
				else
				{
					break;
				}
			}

			if (frameMediums.size() == frameMediums_.size())
			{
				ocean_assert(frameMediums_.size() == frameStatistics_.size());

				for (size_t n = 0; n < frameStatistics_.size(); ++n)
				{
					frameStatistics_[n].reset();
				}

				stereoCameraId = nextStereoCameraId;

				const ScopedLock scopedLock(frameMediumsLock_);

				frameMediums_ = std::move(frameMediums);

				cameraName = "Generic Slot " + String::toAString(stereoCameraId);

				newCamerasAvailable_ = true;
			}
			else
			{
				showMessage("Failed to toggle cameras");
			}
		}

		FrameRefs frameRefs;
		SharedAnyCameras cameras;

		if (Media::FrameMedium::syncedFrames(frameMediums_, lastFrameTimestamp, frameRefs, cameras))
		{
			ocean_assert(!frameRefs.empty() && frameMediums_.size() == frameRefs.size());
			ocean_assert(frameMediums_.size() == frameStatistics_.size());

			for (size_t n = 0; n < frameRefs.size(); ++n)
			{
				ocean_assert(n < frameStatistics_.size());

				if (!frameStatistics_[n].addFrame(cameraName, *(frameRefs[n])))
				{
					showMessage("Failed to analyzer the camera frames");

					break;
				}
			}

			lastFrameTimestamp = frameRefs.front()->timestamp();

			continue;
		}

		sleep(1u);
	}
}

}

}
