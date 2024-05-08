/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/application/VRImageVisualizer.h"

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

void VRImageVisualizer::visualizeImage(const unsigned int id, const HomogenousMatrix4& reference_T_image, Frame&& frame, const ObjectSize& objectSize, const bool referenceIsWorld)
{
	ocean_assert(engine_ && framebuffer_);
	if (!engine_ && !framebuffer_)
	{
		return;
	}

	// rendering objects and media objects can have unique names, so that we can create them and find them later again
	// we could also store the rendering objects and media objects as member variables

	constexpr char sceneName[] = "IMAGE_VISUALIZATION_SCENE";
	constexpr char transformNamePrefix[] = "IMAGE_VISUALIZATION_TRANSFORM_";
	constexpr char absoluteTransformNamePrefix[] = "IMAGE_VISUALIZATION_ABSOLUTETRANSFORM_";
	constexpr char pixelImageNamePrefix[] = "IMAGE_VISUALIZATION_PIXEL_IMAGE_";

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
		if (frame.isValid() == false)
		{
			// the image is invalid anyway
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
		if (frame.isValid() == false)
		{
			// the image is invalid anyway
			return;
		}

		const Media::PixelImageRef pixelImage = Media::Manager::get().newMedium(mediumName, Media::Medium::PIXEL_IMAGE, false /*useExclusive*/);
		ocean_assert(pixelImage);

		pixelImage->start();

		Rendering::MediaTexture2DRef texture;
		transform = Rendering::Utilities::createBox(engine_, Vector3(1, 1, 0.0001), pixelImage, &texture);
		ocean_assert(transform && texture);

		transform->setName(transformName);

		absoluteTransform->addChild(transform);
	}

	ocean_assert(absoluteTransform && transform);

	if (!frame.isValid() || !reference_T_image.isValid() || !objectSize.isValid())
	{
		// the visualization is not necessary anymore
		scene->removeChild(absoluteTransform);
		return;
	}

	absoluteTransform->setTransformationType(referenceIsWorld ? Rendering::AbsoluteTransform::TT_NONE : Rendering::AbsoluteTransform::TT_VIEW);

	// we request the existing PixelImage - as we specify useExclusive = false
	Media::PixelImageRef pixelImage = Media::Manager::get().newMedium(mediumName, Media::Medium::PIXEL_IMAGE, false /*useExclusive*/);
	ocean_assert(pixelImage);

	if (pixelImage.isNull())
	{
		return;
	}

	const Vector2 size = objectSize.size(Scalar(frame.width()), Scalar(frame.height()));

	// ensuring that the frame is the owner of the image content - just to ensure that the memory is not overwritten before forwarded to the GPU
	frame.makeOwner();

	// ensuring that we always set the latest timestamp so that the rendering engine updates the texture
	frame.setTimestamp(engine_->timestamp());

	pixelImage->setPixelImage(std::move(frame));

	const HomogenousMatrix4 scale(Vector3(0, 0, 0), Vector3(size, 1));

	transform->setTransformation(reference_T_image * scale);
}

}

}

}

}

}
