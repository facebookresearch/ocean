/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/Layer.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

Layer::Layer(const Layer& layer) :
	width_(layer.width_),
	height_(layer.height_),
	frame_(layer.frame_, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT),
	mask_(layer.mask_, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT),
	boundingBox_(layer.boundingBox_)
{
	// nothing to do here
}

Layer::Layer(Layer&& layer) noexcept :
	width_(0u),
	height_(0u)
{
	*this = std::move(layer);
}

Layer::Layer(Frame& frame, const Frame& mask, const PixelBoundingBox& boundingBox) :
	width_(0u),
	height_(0u),
	frame_(frame, Frame::ACM_USE_KEEP_LAYOUT),
	mask_(mask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT),
	boundingBox_(boundingBox)
{
	ocean_assert(!boundingBox.isValid() || (boundingBox.right() < frame_.width() && boundingBox.bottom() < frame_.height()));

	if ((frame_.width() == mask_.width() && frame_.height() == mask_.height() && frame_.pixelOrigin() == mask_.pixelOrigin())
		&& (!boundingBox_ || (boundingBox_.right() < frame_.width() && boundingBox_.bottom() < frame_.height())))
	{
		width_ = frame_.width();
		height_ = frame_.height();
	}
}

Layer::~Layer()
{
	// nothing to do here
}

Layer& Layer::operator=(const Layer& layer)
{
	width_ = layer.width_;
	height_ = layer.height_;

	frame_ = Frame(layer.frame_, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
	mask_ = Frame(layer.mask_, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	boundingBox_ = layer.boundingBox_;

	return *this;
}

Layer& Layer::operator=(Layer&& layer) noexcept
{
	if (this != &layer)
	{
		width_ = layer.width_;
		layer.width_ = 0u;

		height_ = layer.height_;
		layer.height_ = 0u;

		frame_ = std::move(layer.frame_);
		mask_ = std::move(layer.mask_);

		boundingBox_ = layer.boundingBox_;
		layer.boundingBox_ = PixelBoundingBox();
	}

	return *this;
}

}

}

}
