/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_BLEND_ATTRIBUTE_H
#define META_OCEAN_RENDERING_BLEND_ATTRIBUTE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Attribute.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class BlendAttribute;

/**
 * Definition of a smart object reference holding a blend attribute.
 * @see SmartObjectRef, BlendAttribute.
 * @ingroup rendering
 */
typedef SmartObjectRef<BlendAttribute> BlendAttributeRef;

/**
 * This class is the base class for all blend attributes.
 * Use this attribute to enable framebuffer blending functionalities for the associated objects.<br>
 * These attribute defines how an associated object is blended with already existing color values in the framebuffer.<br>
 * @see Attribute
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT BlendAttribute : virtual public Attribute
{
	public:

		/**
		 * Defines several blending functions.
		 */
		enum BlendingFunction
		{
			FUNCTION_INVALID,
			FUNCTION_ZERO,
			FUNCTION_ONE,
			FUNCTION_DESTINATION_COLOR,
			FUNCTION_SOURCE_COLOR,
			FUNCTION_ONE_MINUS_DESTINATION_COLOR,
			FUNCTION_ONE_MINUS_SOURCE_COLOR,
			FUNCTION_SOURCE_ALPHA,
			FUNCTION_ONE_MINUS_SOURCE_ALPHA,
			FUNCTION_DESTINATION_ALPHA,
			FUNCTION_ONE_MINUS_DESTINATION_ALPHA,
			FUNCTION_SOURCE_ALPHA_SATURATE
		};

	public:

		/**
		 * Returns the blending function for the source object.
		 * @return Blending function
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual BlendingFunction sourceFunction() const;

		/**
		 * Returns the blending function for the destination object.
		 * @return Blending function
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual BlendingFunction destinationFunction() const;

		/**
		 * Sets the blending function for the source object.
		 * @param function Blending function to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setSourceFunction(const BlendingFunction function);

		/**
		 * Sets the blending function for the destination object.
		 * @param function Blending function to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setDestinationFunction(const BlendingFunction function);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new blend attribute.
		 */
		BlendAttribute();
};

}

}

#endif // META_OCEAN_RENDERING_BLEND_ATTRIBUTE_H
