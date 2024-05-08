/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/LOD.h"

namespace Ocean
{

namespace Rendering
{

LOD::LOD() :
	Group()
{
	// nothing to do here
}

LOD::~LOD()
{
	// nothing to do here
}

LOD::DistanceRanges LOD::distanceRanges() const
{
	throw NotSupportedException("LOD::distanceRanges is not supported.");
}

void LOD::setDistanceRanges(const DistanceRanges& /*ranges*/)
{
	throw NotSupportedException("LOD::setDistanceRanges is not supported.");
}

LOD::ObjectType LOD::type() const
{
	return TYPE_LOD;
}

}

}
