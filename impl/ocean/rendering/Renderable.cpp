/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Renderable.h"

namespace Ocean
{

namespace Rendering
{

Renderable::Renderable() :
	Object()
{
	// nothing to do here
}

Renderable::~Renderable()
{
	// nothing to do here
}

Renderable::ObjectType Renderable::type() const
{
	return TYPE_RENDERABLE;
}

}

}
