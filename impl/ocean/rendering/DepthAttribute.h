/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_DEPTH_ATTRIBUTE_H
#define META_OCEAN_RENDERING_DEPTH_ATTRIBUTE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Attribute.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class DepthAttribute;

/**
 * Definition of a smart object reference holding a depth attribute.
 * @see SmartObjectRef, DepthAttribute.
 * @ingroup rendering
 */
typedef SmartObjectRef<DepthAttribute> DepthAttributeRef;

/**
 * This class implements a depth attribute specifying the depth buffer behavior for associated objects.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT DepthAttribute : virtual public Attribute
{
	public:

		/**
		 * Definition of different depth comparison functions.
		 */
		enum DepthFunction
		{
			/// Always accepted.
			FUNCTION_ALWAYS,
			/// Accepted if the incomming depth value is equal to the stored one.
			FUNCTION_EQUAL,
			/// Accepted if the incomming depth value is greater than the stored one.
			FUNCTION_GREATER,
			/// Accepted if the incomming depth value is greater than or equal to the stored one.
			FUNCTION_GREATER_EQUAL,
			/// Accepted if the incomming depth value is less than the stored one.
			FUNCTION_LESS,
			/// Accepted if the incomming depth value is less than or equal to the stored one.
			FUNCTION_LESS_EQUAL,
			/// Never accepted.
			FUNCTION_NEVER,
			/// Accepted if the incomming depth value is not equal to the stored one.
			FUNCTION_NOT_EQUAL
		};

	public:

		/**
		 * Returns whether the depth buffer is used for occlusion tests.
		 * @return True, if so
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool testingEnabled() const;

		/**
		 * Returns whether the depth buffer writing is enabled.
		 * @return True, if so
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool writingEnabled() const;

		/**
		 * Returns the depth function of this attribute.
		 * @return Depth function
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual DepthFunction depthFunction() const;

		/**
		 * Sets whether the depth buffer is used for occlusion tests.
		 * @param state Depth buffer enable state
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setTestingEnabled(const bool state);

		/**
		 * Sets whether the depth buffer is writable.
		 * @param state Depth buffer writable state
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setWritingEnabled(const bool state);

		/**
		 * Sets the depth function of this attribute.
		 * @param function Depth function to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setDepthFunction(const DepthFunction function);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new depth attribute object.
		 */
		DepthAttribute();
};

}

}

#endif // META_OCEAN_RENDERING_DEPTH_ATTRIBUTE_H
