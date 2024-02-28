// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/DepthTrackerExperience.h"

#include "ocean/cv/FrameConverterColorMap.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"
#include "ocean/rendering/Textures.h"

namespace Ocean
{

namespace XRPlayground
{

DepthTrackerExperience::~DepthTrackerExperience()
{
	// nothing to do here
}

bool DepthTrackerExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	assert(engine);

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	sceneTracker6DOF_ = Devices::Manager::get().device("ARCore 6DOF Depth Tracker");
#else
	sceneTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Depth Tracker");
#endif

	if (sceneTracker6DOF_.isNull())
	{
		Log::error() << "Failed to access World tracker";
		return false;
	}

	const Devices::VisualTrackerRef visualTracker(sceneTracker6DOF_);
	if (visualTracker && !engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());

			const Media::FrameMediumRef frameMedium(undistortedBackground->medium());

			if (frameMedium)
			{
				visualTracker->setInput(frameMedium);
			}
		}
	}

	sceneTracker6DOF_->start();

	const Rendering::AbsoluteTransformRef absoluteTransform = engine->factory().createAbsoluteTransform();
	absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
	absoluteTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -1)));

	experienceScene()->addChild(absoluteTransform);

	Rendering::AttributeSetRef attributeSet;
	renderingBoxTransform_ = Rendering::Utilities::createBox(engine, Vector3(1, 1, 0), nullptr, &attributeSet);
	absoluteTransform->addChild(renderingBoxTransform_);

	Rendering::TexturesRef textures = engine->factory().createTextures();
	renderingFrameTexture_ = engine->factory().createFrameTexture2D();
	textures->addTexture(renderingFrameTexture_);

	attributeSet->addAttribute(textures);

	return true;
}

bool DepthTrackerExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	renderingFrameTexture_.release();

	sceneTracker6DOF_.release();

	return true;
}

Timestamp DepthTrackerExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sample = sceneTracker6DOF_->sample();

	if (sample && sample->timestamp() > recentDepthImageTimestamp_ && sample->objectIds().size() == 1)
	{
		const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sample->sceneElements().front();

		if (sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_DEPTH)
		{
			const Devices::SceneTracker6DOF::SceneElementDepth& sceneElementDepth = (const Devices::SceneTracker6DOF::SceneElementDepth&)(*sceneElement);

			std::shared_ptr<Frame> confidence;
			std::shared_ptr<Frame> depthImage = sceneElementDepth.depth(&confidence);

			if (depthImage->isPixelFormatCompatible(FrameType::FORMAT_F32))
			{
				Frame rgbFrame(FrameType(*depthImage, FrameType::FORMAT_RGB24));

				constexpr float minDistance = 0.05f;
				constexpr float maxDistance = 10.0f;

				switch (visualizationMode_)
				{
					case VM_JET:
					{
						CV::FrameConverterColorMap::convertJetFloat1ChannelToRGB24(depthImage->constdata<float>(), rgbFrame.data<uint8_t>(), depthImage->width(), depthImage->height(), CV::FrameConverterColorMap::CONVERT_NORMAL, depthImage->paddingElements(), rgbFrame.paddingElements(), minDistance, maxDistance);
						break;
					}

					case VM_TURBO:
					{
						CV::FrameConverterColorMap::convertTurboFloat1ChannelToRGB24(depthImage->constdata<float>(), rgbFrame.data<uint8_t>(), depthImage->width(), depthImage->height(), CV::FrameConverterColorMap::CONVERT_NORMAL, depthImage->paddingElements(), rgbFrame.paddingElements(), minDistance, maxDistance);
						break;
					}

					case VM_INFERNO:
					{
						CV::FrameConverterColorMap::convertInfernoFloat1ChannelToRGB24(depthImage->constdata<float>(), rgbFrame.data<uint8_t>(), depthImage->width(), depthImage->height(), CV::FrameConverterColorMap::CONVERT_NORMAL, depthImage->paddingElements(), rgbFrame.paddingElements(), minDistance, maxDistance);
						break;
					}

					case VM_SPIRAL:
					{
						CV::FrameConverterColorMap::convertSpiralFloat1ChannelToRGB24(depthImage->constdata<float>(), rgbFrame.data<uint8_t>(), depthImage->width(), depthImage->height(), CV::FrameConverterColorMap::CONVERT_NORMAL, depthImage->paddingElements(), rgbFrame.paddingElements(), minDistance, maxDistance, 1.0f);
						break;
					}

					case VM_JET_CONFIDENCE:
					{
						if (confidence)
						{
							CV::FrameConverterColorMap::Comfort::convert1ChannelToRGB24(*confidence, rgbFrame, CV::FrameConverterColorMap::CM_JET);
						}
						else
						{
							rgbFrame.setValue(0xFF);
						}

                        break;
					}

					default:
						ocean_assert(false && "This should never happen!");
						rgbFrame.setValue(0xFF);
				}

				const Scalar invMaxLength = Scalar(1) / Scalar(std::max(rgbFrame.width(), rgbFrame.height()));

				const Vector3 scale(Scalar(rgbFrame.width()) * invMaxLength, Scalar(rgbFrame.height()) * invMaxLength, 1);
				renderingBoxTransform_->setTransformation(sceneElementDepth.device_T_depth() * HomogenousMatrix4(Vector3(0, 0, 0), scale));

				renderingFrameTexture_->setTexture(std::move(rgbFrame));
			}
		}

		recentDepthImageTimestamp_ = sample->timestamp();
	}

	return timestamp;
}

void DepthTrackerExperience::onMousePress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& /*screenPosition*/, const Line3& /*ray*/, const Timestamp /*timestamp*/)
{
	visualizationMode_ = VisualizationMode((visualizationMode_ + 1u) % VM_END);
}

std::unique_ptr<XRPlaygroundExperience> DepthTrackerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new DepthTrackerExperience());
}

}

}
