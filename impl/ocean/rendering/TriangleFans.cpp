// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/TriangleFans.h"

namespace Ocean
{

namespace Rendering
{

TriangleFans::TriangleFans() :
	StripPrimitive()
{
	// nothing to do here
}

TriangleFans::~TriangleFans()
{
	// nothing to do here
}

TriangleFans::ObjectType TriangleFans::type() const
{
	return TYPE_TRIANGLE_FANS;
}


}

}
