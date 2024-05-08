/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_TRIANGLE_STRIPS_H
#define META_OCEAN_RENDERING_TRIANGLE_STRIPS_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/StripPrimitive.h"
#include "ocean/rendering/TriangleFace.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class TriangleStrips;

/**
 * Definition of a smart object reference holding a triangle strips node.
 * @see SmartObjectRef, TriangleStrips.
 * @ingroup rendering
 */
typedef SmartObjectRef<TriangleStrips> TriangleStripsRef;

/**
 * This class is the base for all rendering triangle strips.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT TriangleStrips : virtual public StripPrimitive
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new triangle strips object.
		 */
		TriangleStrips();

		/**
		 * Destructs a triangle strips object.
		 */
		~TriangleStrips() override;
};

}

}

#endif // META_OCEAN_RENDERING_TRIANGLE_STRIPS_H
