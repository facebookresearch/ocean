/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/LayerF1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

LayerF1::LayerF1() :
	LayerF()
{
	// nothing to do here
}

LayerF1::LayerF1(const LayerF1& layer) :
	LayerF(layer),
	mapping_(layer.mapping_)
{
	// nothing to do here
}

LayerF1::LayerF1(LayerF1&& layer) noexcept :
	LayerF(std::move(layer)),
	mapping_(std::move(layer.mapping_))
{
	ocean_assert(!layer.mapping_);
}

LayerF1::LayerF1(Frame& frame, const Frame& mask, const PixelBoundingBox& boundingBox) :
	LayerF(frame, mask, boundingBox),
	mapping_(frame.width(), frame.height())
{
	ocean_assert(frame.width() == mask.width());
	ocean_assert(frame.height() == mask.height());
	ocean_assert(frame.pixelOrigin() == mask.pixelOrigin());
}

const MappingF1& LayerF1::mapping() const
{
	return mapping_;
}

MappingF1& LayerF1::mapping()
{
	return mapping_;
}

LayerF1& LayerF1::operator=(const LayerF1& layer)
{
	LayerF::operator=(layer);
	mapping_ = layer.mapping_;

	return *this;
}

LayerF1& LayerF1::operator=(const LayerI1& layer)
{
	width_ = layer.width();
	height_ = layer.height();

	frame_ = Frame(layer.frame(), Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
	mask_ = Frame(layer.mask(),Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	boundingBox_ = layer.boundingBox();

	mapping_ = MappingF1(width_, height_);

	for (unsigned int n = 0; n < width_ * height_; ++n)
	{
		mapping_()[n] = Vector2(Scalar(layer.mappingI1()()[n].x()), Scalar(layer.mappingI1()()[n].y()));
	}

	return *this;
}

LayerF1& LayerF1::operator=(LayerF1&& layer) noexcept
{
	if (this != &layer)
	{
		mapping_ = std::move(layer.mapping_);
		ocean_assert(!layer.mapping_);

		LayerF::operator=(std::move(layer));
	}

	return *this;
}

}

}

}
