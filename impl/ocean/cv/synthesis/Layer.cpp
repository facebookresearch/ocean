// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
	legacyMask_(layer.legacyMask_, true),
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

Layer::Layer(Frame& frame, const LegacyFrame& mask, const PixelBoundingBox& boundingBox) :
	width_(0u),
	height_(0u),
	frame_(frame, Frame::ACM_USE_KEEP_LAYOUT),
	legacyMask_(mask, true),
	boundingBox_(boundingBox)
{
	ocean_assert(!boundingBox.isValid() || (boundingBox.right() < frame_.width() && boundingBox.bottom() < frame_.height()));

	if ((frame_.width() == legacyMask_.width() && frame_.height() == legacyMask_.height() && frame_.pixelOrigin() == legacyMask_.pixelOrigin())
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
	legacyMask_ = LegacyFrame(layer.legacyMask_, true);

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
		legacyMask_ = std::move(layer.legacyMask_);

		boundingBox_ = layer.boundingBox_;
		layer.boundingBox_ = PixelBoundingBox();
	}

	return *this;
}

}

}

}
