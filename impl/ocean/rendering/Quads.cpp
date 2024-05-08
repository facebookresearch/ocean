/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Quads.h"

namespace Ocean
{

namespace Rendering
{

Quads::Quads() :
	IndependentPrimitive()
{
	// nothing to do here
}

Quads::~Quads()
{
	// nothing to do here
}

QuadFaces Quads::faces() const
{
	throw NotSupportedException("Quads::faces() is not supported.");
}

unsigned int Quads::numberFaces() const
{
	throw NotSupportedException("Quads::numberFaces() is not supported.");
}

void Quads::setFaces(const QuadFaces& /*faces*/)
{
	throw NotSupportedException("Quads::setFaces() is not supported.");
}

Quads::ObjectType Quads::type() const
{
	return TYPE_QUADS;
}

}

}
