/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/StereoAttribute.h"

namespace Ocean
{

namespace Rendering
{

StereoAttribute::StereoAttribute() :
	Attribute()
{
	// nothing to do here
}

StereoAttribute::StereoVisibility StereoAttribute::stereoVisibility() const
{
	throw NotSupportedException("Stereo attribute is not supported.");
}

void StereoAttribute::setStereoVisibility(const StereoVisibility /*stereoVisibility*/)
{
	throw NotSupportedException("Stereo attribute is not supported.");
}

StereoAttribute::ObjectType StereoAttribute::type() const
{
	return TYPE_STEREO_ATTRIBUTE;
}

}

}
