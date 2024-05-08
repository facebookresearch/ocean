/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIMediaTexture2D.h"

#include "ocean/base/Utilities.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/media/FrameMedium.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIMediaTexture2D::GIMediaTexture2D() :
	Texture(),
	GITexture(),
	Texture2D(),
	DynamicObject(),
	GITexture2D(),
	MediaTexture2D()
{
	// nothing to do here
}

GIMediaTexture2D::~GIMediaTexture2D()
{
	// nothing to do here
}

bool GIMediaTexture2D::isValid() const
{
	return bool(frameMedium_);
}

bool GIMediaTexture2D::textureColor(const TextureCoordinate& textureCoordinate, RGBAColor& color) const
{
	if (!textureFrame_.isValid())
	{
		return false;
	}

	ocean_assert(textureFrame_.pixelFormat() == FrameType::FORMAT_RGB24);

	constexpr float inv255 = 1.0f / 255.0f;

	if (textureMagnificationFilterMode == MAG_MODE_LINEAR)
	{
		const Vector2 position(minmax(Scalar(0), textureCoordinate.x() * Scalar(textureFrame_.width()), Scalar(textureFrame_.width())),
								minmax(Scalar(0), (Scalar(1) - textureCoordinate.y()) * Scalar(textureFrame_.height()), Scalar(textureFrame_.height())));

		uint8_t result[3];
		CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_CENTER>(textureFrame_.constdata<uint8_t>(), textureFrame_.width(), textureFrame_.height(), textureFrame_.paddingElements(), position, result);

		color = RGBAColor(float(result[0]) * inv255, float(result[1]) * inv255, float(result[2]) * inv255);
	}
	else
	{
		const Vector2 position(minmax(Scalar(0), textureCoordinate.x() * Scalar(textureFrame_.width()), Scalar(textureFrame_.width() - 1u)),
								minmax(Scalar(0), (1 - textureCoordinate.y()) * Scalar(textureFrame_.height()), Scalar(textureFrame_.height() - 1u)));

		const uint8_t* const pixel = textureFrame_.constpixel<uint8_t>((unsigned int)(position.x()), (unsigned int)(position.y()));

		color = RGBAColor(float(pixel[0]) * inv255, float(pixel[1]) * inv255, float(pixel[2]) * inv255);
	}

	return true;
}

void GIMediaTexture2D::onDynamicUpdate(const ViewRef& /*view*/, const Timestamp timestamp)
{
	const ScopedLock scopedLock(objectLock);

	if (frameMedium_)
	{
		const FrameRef frame = frameMedium_->frame(timestamp);

		if (frame && frame->timestamp() >= frameTimestamp_)
		{
			ocean_assert(*frame);

			if (CV::FrameConverter::Comfort::convert(*frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, textureFrame_, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
			{
				frameTimestamp_ = timestamp;
			}
		}
	}
}

}

}

}
