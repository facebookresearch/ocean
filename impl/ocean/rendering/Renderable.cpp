// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
