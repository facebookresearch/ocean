/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Attribute.h"

namespace Ocean
{

namespace Rendering
{

Attribute::Attribute() :
	Object()
{
	// nothing to do here
}

Attribute::~Attribute()
{
	// nothing to do here
}

Attribute::ObjectType Attribute::type() const
{
	return TYPE_ATTRIBUTE;
}

}

}
