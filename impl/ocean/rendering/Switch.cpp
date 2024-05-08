/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Switch.h"

namespace Ocean
{

namespace Rendering
{

Switch::Switch() :
	Group()
{
	// nothing to do here
}

Switch::~Switch()
{
	// nothing to do here
}

Index32 Switch::activeNode() const
{
	throw NotSupportedException("Switch::activeNode() is not supported.");
}

void Switch::setActiveNode(const Index32 /*index*/)
{
	throw NotSupportedException("Switch::setActiveNode() is not supported.");
}

Switch::ObjectType Switch::type() const
{
	return TYPE_SWITCH;
}

}

}
