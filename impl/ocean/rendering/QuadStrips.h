/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_QUAD_STRIPS_H
#define META_OCEAN_RENDERING_QUAD_STRIPS_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/QuadFace.h"
#include "ocean/rendering/StripPrimitive.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class QuadStrips;

/**
 * Definition of a smart object reference holding a quad strips object.
 * @see SmartObjectRef, QuadStrips.
 * @ingroup rendering
 */
typedef SmartObjectRef<QuadStrips> QuadStripsRef;

/**
 * This class is the base class for all stripped quad objects.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT QuadStrips : public virtual StripPrimitive
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new stripped quad object.
		 */
		QuadStrips();

		/**
		 * Destructs a stripped quad object.
		 */
		~QuadStrips() override;
};

}

}

#endif // META_OCEAN_RENDERING_QUAD_STRIPS_H
