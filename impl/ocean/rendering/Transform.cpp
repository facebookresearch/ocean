/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace Rendering
{

Transform::Transform() :
	Group()
{
	// nothing to do here
}

Transform::~Transform()
{
	// nothing to do here
}

Transform::ObjectType Transform::type() const
{
	return TYPE_TRANSFORM;
}

}

}
