/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_STRIP_PRIMITIVE_H
#define META_OCEAN_RENDERING_STRIP_PRIMITIVE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Primitive.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class StripPrimitive;

/**
 * Definition of a smart object reference holding a strip primitive.
 * @see SmartObjectRef, StripPrimitive.
 * @ingroup rendering
 */
typedef SmartObjectRef<StripPrimitive> StripPrimitiveRef;

/**
 * This class is the base class for all primitive with strip organization.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT StripPrimitive : virtual public Primitive
{
	public:

		/**
		 * Returns the strips of this primitive object.
		 * @return Strips of primitive
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual VertexIndexGroups strips() const;

		/**
		 * Returns the number of triangle strips defined in this primitive.
		 * @return Number of triangle strips
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual unsigned int numberStrips() const;

		/**
		 * Sets the strips of this primitive object.
		 * @param strips The strips to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setStrips(const VertexIndexGroups& strips);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new strip primitive object.
		 */
		StripPrimitive();

		/**
		 * Destructs a strip primitive object.
		 */
		~StripPrimitive() override;
};

}

}

#endif // META_OCEAN_RENDERING_STRIP_PRIMITIVE_H
