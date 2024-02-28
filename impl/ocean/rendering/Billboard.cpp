// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/Billboard.h"

namespace Ocean
{

namespace Rendering
{

Billboard::Billboard() :
	Group()
{
	// nothing to do here
}

Billboard::~Billboard()
{
	// nothing to do here
}

Billboard::AlignmentType Billboard::alignmentType() const
{
	throw NotSupportedException("Billboard alignment type is not supported.");
}

Vector3 Billboard::alignmentAxis() const
{
	throw NotSupportedException("Billboard alignment axis is not supported.");
}

void Billboard::setAlignmentType(const AlignmentType /*type*/)
{
	throw NotSupportedException("Billboard alignment type is not supported.");
}

void Billboard::setAlignmentAxis(const Vector3& /*axis*/)
{
	throw NotSupportedException("Billboard alignment axis is not supported.");
}

Billboard::ObjectType Billboard::type() const
{
	return TYPE_BILLBOARD;
}

}

}
