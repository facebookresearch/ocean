/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/PhantomAttribute.h"

namespace Ocean
{

namespace Rendering
{

PhantomAttribute::PhantomAttribute() :
	Attribute()
{
	// nothing to do here
}

PhantomAttribute::ObjectType PhantomAttribute::type() const
{
	return TYPE_PHANTOM_ATTRIBUTE;
}

}

}
