/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_ABSOLUTE_TRANSFORM_H
#define META_OCEAN_RENDERING_ABSOLUTE_TRANSFORM_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class AbsoluteTransform;

/**
 * Definition of a smart object reference holding a transform node.
 * @see SmartObjectRef, AbsoluteTransform.
 * @ingroup rendering
 */
typedef SmartObjectRef<AbsoluteTransform> AbsoluteTransformRef;

/**
 * This class is the base class for all absolute transform nodes.
 * An absolute transformation provides an own and explicit transformation independent form parent nodes.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT AbsoluteTransform : virtual public Group
{
	public:

		/**
		 * Defines different transformation types.
		 */
		enum TransformationType
		{
			/// Transformation with no explicit absolute coordinate system
			TT_NONE,
			/// Transformation which is defined in relation to the view.
			TT_VIEW,
			/// Transformation providing an explicit coordinate system for head-up objects.
			TT_HEAD_UP
		};

	public:

		/**
		 * Returns the current transfomation type.
		 * Default is TT_NONE.
		 * @return Transformation type
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual TransformationType transformationType() const;

		/**
		 * Returns the relative screen position of the head-up node.
		 * The default value is (0.5, 0.5)
		 * @return Relative screen position, with range [0.0, 1.0] for each axis, -1.0 neglects this definition
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setHeadUpRelativePosition().
		 */
		virtual Vector2 headUpRelativePosition() const;

		/**
		 * Returns the transformation which is applied on top of the absolute transformation.
		 * @return The transformation between all child nodes and the absolute transformation (absolute_T_children)
		 */
		virtual HomogenousMatrix4 transformation() const = 0;

		/**
		 * Sets or changes the transfomation type.
		 * Default is TT_NONE.
		 * @param type Transformation type to set
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setTransformationType(const TransformationType type);

		/**
		 * Sets the relative screen position of the head-up node.
		 * @param position Relative screen position, with range [0.0, 1.0] for each axis, -1.0 neglects this definition
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see headUpRelativePosition().
		 */
		virtual bool setHeadUpRelativePosition(const Vector2& position);

		/**
		 * Sets an additional transformation which is applied on top of the absolute transformation.
		 * @param absolute_T_children The transformation between all child nodes and the absolute transformation, must be valid
		 */
		virtual void setTransformation(const HomogenousMatrix4& absolute_T_children) = 0;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new absolute transform node.
		 */
		AbsoluteTransform();

		/**
		 * Destructs an absolute transform node.
		 */
		~AbsoluteTransform() override;
};

}

}

#endif // META_OCEAN_RENDERING_ABSOLUTE_TRANSFORM_H
