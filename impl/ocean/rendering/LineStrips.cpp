// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/rendering/LineStrips.h"

namespace Ocean
{

namespace Rendering
{

	LineStrips::LineStrips() :
	Primitive()
{
	// nothing to do here
}

	LineStrips::~LineStrips()
{
	// nothing to do here
}

	LineStrips::ObjectType LineStrips::type() const
{
	return TYPE_LINE_STRIPS;
}


}

}
