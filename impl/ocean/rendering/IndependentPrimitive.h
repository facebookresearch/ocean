/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_INDEPENDENT_PRIMITIVE_H
#define META_OCEAN_RENDERING_INDEPENDENT_PRIMITIVE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Primitive.h"

namespace Ocean
{

namespace Rendering
{

/**
 * This class is the base class for all independent primitive objects.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT IndependentPrimitive : virtual public Primitive
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new independent primitive object.
		 */
		IndependentPrimitive();

		/**
		 * Destructs an independent primitive object.
		 */
		~IndependentPrimitive() override;
};

}

}

#endif // META_OCEAN_RENDERING_INDEPENDENT_PRIMITIVE_H
