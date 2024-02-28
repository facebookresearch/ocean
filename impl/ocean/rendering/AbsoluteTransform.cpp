// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/AbsoluteTransform.h"

namespace Ocean
{

namespace Rendering
{

AbsoluteTransform::AbsoluteTransform() :
	Group()
{
	// nothing to do here
}

AbsoluteTransform::~AbsoluteTransform()
{
	// nothing to do here
}

AbsoluteTransform::TransformationType AbsoluteTransform::transformationType() const
{
	throw NotSupportedException("AbsoluteTransform::transformationType() is not supported.");
}

Vector2 AbsoluteTransform::headUpRelativePosition() const
{
	throw NotSupportedException("AbsoluteTransform::headUpRelativePosition() is not supported.");
}

bool AbsoluteTransform::setTransformationType(const TransformationType /*type*/)
{
	throw NotSupportedException("AbsoluteTransform::setTransformationType() is not supported.");
}

bool AbsoluteTransform::setHeadUpRelativePosition(const Vector2& /*position*/)
{
	throw NotSupportedException("AbsoluteTransform::setHeadUpRelativePosition() is not supported.");
}

AbsoluteTransform::ObjectType AbsoluteTransform::type() const
{
	return TYPE_ABSOLUTE_TRANSFORM;
}

}

}
