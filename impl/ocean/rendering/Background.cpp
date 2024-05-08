/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Background.h"

namespace Ocean
{

namespace Rendering
{

Background::Background() :
	Node(),
	backgroundDistance(1000)
{
	// nothing to do here
}

Background::~Background()
{
	// nothing to do here
}

Scalar Background::distance() const
{
	return backgroundDistance;
}

Vector3 Background::position() const
{
	throw NotSupportedException("Background position is not supported.");
}

Quaternion Background::orientation() const
{
	throw NotSupportedException("Background orientation is not supported.");
}

bool Background::setDistance(const Scalar distance)
{
	if (distance <= Scalar(0))
	{
		return false;
	}

	backgroundDistance = distance;
	return true;
}

void Background::setPosition(const Vector3& /*position*/)
{
	throw NotSupportedException("Background position is not supported.");
}

void Background::setOrientation(const Quaternion& /*orientation*/)
{
	throw NotSupportedException("Background orientation is not supported.");
}

Background::ObjectType Background::type() const
{
	return TYPE_BACKGROUND;
}

}

}
