/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/DepthAttribute.h"

namespace Ocean
{

namespace Rendering
{

DepthAttribute::DepthAttribute() :
	Attribute()
{
	// nothing to do here
}

bool DepthAttribute::testingEnabled() const
{
	throw NotSupportedException("Depth attribute testing enabled is not supported.");
}

bool DepthAttribute::writingEnabled() const
{
	throw NotSupportedException("Depth attribute writing enabled is not supported.");
}

DepthAttribute::DepthFunction DepthAttribute::depthFunction() const
{
	throw NotSupportedException("Depth attribute depth function is not supported.");
}

void DepthAttribute::setTestingEnabled(const bool /*state*/)
{
	throw NotSupportedException("Depth attribute testing enabled is not supported.");
}

void DepthAttribute::setWritingEnabled(const bool /*state*/)
{
	throw NotSupportedException("Depth attribute writing enabled is not supported.");
}

void DepthAttribute::setDepthFunction(const DepthFunction /*function*/)
{
	throw NotSupportedException("Depth attribute depth function is not supported.");
}

DepthAttribute::ObjectType DepthAttribute::type() const
{
	return TYPE_DEPTH_ATTRIBUTE;
}

}

}
