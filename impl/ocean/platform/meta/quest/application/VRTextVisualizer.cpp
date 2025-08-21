/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/application/VRTextVisualizer.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/Canvas.h"

#include "ocean/cv/fonts/FontManager.h"

#include "ocean/media/Manager.h"
#include "ocean/media/PixelImage.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/Manager.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace Application
{

void VRTextVisualizer::visualizeText(const unsigned int id, const HomogenousMatrix4& reference_T_text, const std::string& text, const ObjectSize& objectSize, const Timestamp& /*workaroundTimestamp*/, const bool referenceIsWorld, const std::string& fontName, const RGBAColor& backgroundColor)
{
	ocean_assert(engine_ && framebuffer_);
	if (!engine_ && !framebuffer_)
	{
		return;
	}

	const bool validInput = !text.empty() && reference_T_text.isValid()  && objectSize.isValid();

	// rendering objects and media objects can have unique names, so that we can create them and find them later again
	// we could also store the rendering objects and media objects as member variables

	constexpr char sceneName[] = "TEXT_VISUALIZATION_SCENE";
	constexpr char transformNamePrefix[] = "TEXT_VISUALIZATION_TRANSFORM_";
	constexpr char absoluteTransformNamePrefix[] = "TEXT_VISUALIZATION_ABSOLUTETRANSFORM_";
	constexpr char pixelImageNamePrefix[] = "TEXT_VISUALIZATION_PIXEL_IMAGE_";

	Rendering::SceneRef scene = engine_->object(sceneName);

	if (!scene)
	{
		scene = engine_->factory().createScene();
		scene->setName(sceneName);

		framebuffer_->addScene(scene);
	}

	ocean_assert(scene);

	const std::string idString = String::toAString(id);

	const std::string transformName = transformNamePrefix + idString;
	const std::string absoluteTransformName = absoluteTransformNamePrefix + idString;
	const std::string mediumName = pixelImageNamePrefix + idString;

	Rendering::AbsoluteTransformRef absoluteTransform = engine_->object(absoluteTransformName);

	if (!absoluteTransform)
	{
		if (!validInput)
		{
			return;
		}

		absoluteTransform = engine_->factory().createAbsoluteTransform();
		ocean_assert(absoluteTransform);

		absoluteTransform->setName(absoluteTransformName);

		scene->addChild(absoluteTransform);
	}

	ocean_assert(absoluteTransform);

	Rendering::TransformRef transform = engine_->object(transformName);

	if (!transform)
	{
		if (!validInput)
		{
			return;
		}

		const Media::PixelImageRef pixelImage = Media::Manager::get().newMedium(mediumName, Media::Medium::PIXEL_IMAGE, false /*useExclusive*/);
		ocean_assert(pixelImage);

		pixelImage->start();

		transform = Rendering::Utilities::createBox(engine_, Vector3(1, 1, 0.0001), pixelImage);
		ocean_assert(transform);

		transform->setName(transformName);

		absoluteTransform->addChild(transform);
	}

	ocean_assert(absoluteTransform && transform);

	if (!validInput)
	{
		// the visualization is not necessary anymore
		scene->removeChild(absoluteTransform);
		return;
	}

	const CV::Fonts::SharedFont font = CV::Fonts::FontManager::get().font(fontName, 40, "Regular");

	if (!font)
	{
		static std::set<std::string> invalidFonts;

		if (invalidFonts.find(fontName) == invalidFonts.cend())
		{
			invalidFonts.insert(fontName);

			Log::error() << "Could not create font '" << fontName << "', using backup font";
		}
	}

	unsigned int textWidth = 0u;
	unsigned int textHeight = 0u;

	if (font)
	{
		if (!font->textExtent(text, textWidth, textHeight) || textWidth == 0u || textHeight == 0u)
		{
			// the given text is invalid, visualization is not necessary anymore
			scene->removeChild(absoluteTransform);
			return;
		}
	}
	else
	{
		if (!CV::Canvas::textExtent(text, textWidth, textHeight) || textWidth == 0u || textHeight == 0u)
		{
			// the given text is invalid, visualization is not necessary anymore
			scene->removeChild(absoluteTransform);
			return;
		}
	}

	absoluteTransform->setTransformationType(referenceIsWorld ? Rendering::AbsoluteTransform::TT_NONE : Rendering::AbsoluteTransform::TT_VIEW);

	// we request the existing PixelImage - as we specify useExclusive = false
	Media::PixelImageRef pixelImage = Media::Manager::get().newMedium(mediumName, Media::Medium::PIXEL_IMAGE, false /*useExclusive*/);
	ocean_assert(pixelImage);

	if (pixelImage.isNull())
	{
		return;
	}

	ocean_assert(validInput);

	const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_RGBA32;

	Frame frame(FrameType(textWidth, textHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
	
	const uint8_t r = uint8_t(std::clamp(backgroundColor.red() * 255.0f, 0.0f, 255.0f));
	const uint8_t g = uint8_t(std::clamp(backgroundColor.green() * 255.0f, 0.0f, 255.0f));
	const uint8_t b = uint8_t(std::clamp(backgroundColor.blue() * 255.0f, 0.0f, 255.0f));
	const uint8_t a = uint8_t(std::clamp(backgroundColor.alpha() * 255.0f, 0.0f, 255.0f));
	
	const Frame::PixelType<uint8_t, 4u> rgbaPixel({r, g, b, a});
	frame.setValue<uint8_t, 4u>(rgbaPixel);

	if (font)
	{
		font->drawText(frame, text, 0, 0, CV::Canvas::white());
	}
	else
	{
		CV::Canvas::drawText(frame, text, 0, 0, CV::Canvas::white());
	}

	frame.setTimestamp(engine_->timestamp());

	const Vector2 size = objectSize.size(Scalar(frame.width()), Scalar(frame.height()));
	const HomogenousMatrix4 scale(Vector3(0, 0, 0), Vector3(size, 1));

	pixelImage->setPixelImage(std::move(frame));

	transform->setTransformation(reference_T_text * scale);
}

}

}

}

}

}
