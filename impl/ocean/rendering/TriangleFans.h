/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_TRIANGLE_FANS_H
#define META_OCEAN_RENDERING_TRIANGLE_FANS_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/StripPrimitive.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class TriangleFans;

/**
 * Definition of a smart object reference holding a triangle fans node.
 * @see SmartObjectRef, TriangleFans.
 * @ingroup rendering
 */
typedef SmartObjectRef<TriangleFans> TriangleFansRef;

/**
 * This class is the base class for all triangle fan objects.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT TriangleFans : virtual public StripPrimitive
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new triangle fans object.
		 */
		TriangleFans();

		/**
		 * Destructs a triangle fans object.
		 */
		~TriangleFans() override;
};

}

}

#endif // META_OCEAN_RENDERING_TRIANGLE_FANS_H
