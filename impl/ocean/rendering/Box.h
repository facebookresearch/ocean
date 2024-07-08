/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_BOX_H
#define META_OCEAN_RENDERING_BOX_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/ObjectRef.h"
#include "ocean/rendering/Shape.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Box;

/**
 * Definition of a smart object reference holding a box.
 * @see SmartObjectRef, Box.
 * @ingroup rendering
 */
typedef SmartObjectRef<Box> BoxRef;

/**
 * This class is the base class for all boxes.<br>
 * The box is defined about the origin with edge length (1, 1, 1).
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Box : virtual public Shape
{
	public:

		/**
		 * Returns the dimensions of the box.
		 * @return Dimensions of the box
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Vector3 size() const;

		/**
		 * Sets the dimensions of the box.
		 * @param size Dimension of the box in x-, y-, and z-direction
		 * @return True, if the specified size was valid and could be set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setSize(const Vector3& size);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new box object with default edge length (1, 1, 1)<br>
		 * The box will be defined about the origin.
		 */
		Box();

		/**
		 * Destructs a box object.
		 */
		~Box() override;
};

}

}

#endif // META_OCEAN_RENDERING_BOX_H
