/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_SHAPE_H
#define META_OCEAN_RENDERING_SHAPE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Renderable.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Shape;

/**
 * Definition of a smart object reference holding a shape.
 * @see SmartObjectRef, Shape.
 * @ingroup rendering
 */
typedef SmartObjectRef<Shape> ShapeRef;

/**
 * This class is the base class for all geometries.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Shape : virtual public Renderable
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new shape node.
		 */
		Shape();

		/**
		 * Destructs a shape node.
		 */
		~Shape() override;
};

}

}

#endif // META_OCEAN_RENDERING_GEOMETRY_H
