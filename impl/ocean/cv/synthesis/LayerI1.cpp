/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/LayerI1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

LayerI1::LayerI1() :
	LayerI()
{
	// nothing to do here
}

LayerI1::LayerI1(const LayerI1& layer) :
	LayerI(layer),
	mapping_(layer.mapping_)
{
	// nothing to do here
}

LayerI1::LayerI1(LayerI1&& layer) noexcept :
	LayerI(std::move(layer)),
	mapping_(std::move(layer.mapping_))
{
	// nothing to do here
}

LayerI1::LayerI1(Frame& frame, const Frame& mask, const PixelBoundingBox& boundingBox) :
	LayerI(frame, mask, boundingBox),
	mapping_(frame.width(), frame.height())
{
	ocean_assert(frame.width() == mask.width());
	ocean_assert(frame.height() == mask.height());
	ocean_assert(frame.pixelOrigin() == mask.pixelOrigin());
}

MappingI1& LayerI1::mapping()
{
	return mapping_;
}

const MappingI1& LayerI1::mapping() const
{
	return mapping_;
}

LayerI1& LayerI1::operator=(const LayerI1& layer)
{
	LayerI::operator=(layer);
	mapping_ = layer.mapping_;

	return *this;
}

LayerI1& LayerI1::operator=(LayerI1&& layer) noexcept
{
	if (this != &layer)
	{
		mapping_ = std::move(layer.mapping_);
		ocean_assert(!layer.mapping_);

		LayerI::operator=(std::move(layer));
	}

	return *this;
}

}

}

}
