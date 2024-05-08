/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_LINE_STRIPS_H
#define META_OCEAN_RENDERING_LINE_STRIPS_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/StripPrimitive.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class LineStrips;

/**
 * Definition of a smart object reference holding a line strips node.
 * @see SmartObjectRef, LineStrips.
 * @ingroup rendering
 */
typedef SmartObjectRef<LineStrips> LineStripsRef;

/**
 * This class is the base for all rendering line strips.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT LineStrips : virtual public StripPrimitive
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new line strips object.
		 */
		LineStrips();

		/**
		 * Destructs a line strips object.
		 */
		~LineStrips() override;
};

}

}

#endif // META_OCEAN_RENDERING_LINE_STRIPS_H
