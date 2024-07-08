/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_TRANSFORM_H
#define META_OCEAN_RENDERING_TRANSFORM_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Group.h"

#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Transform;

/**
 * Definition of a smart object reference holding a transform node.
 * @see SmartObjectRef, Transform.
 * @ingroup rendering
 */
typedef SmartObjectRef<Transform> TransformRef;

/**
 * Definition of a vector of references holding transform nods
 * @ingroup rendering
 */
typedef std::vector<TransformRef> TransformRefs;

/**
 * This class is the base class for all transform nodes.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Transform : virtual public Group
{
	public:

		/**
		 * Definition of a modifier for transform objects.
		 * Modifiers can be applies to modify the transformation of one (or several) transformations concurrently based on the identical transformation without changing the scene graph hierarchy.<br>
		 * When applied to a transform, the transform's transformation is adjusted as followed:<br>
		 * <pre>
		 * parent_T_transform = parent_T_transform * transform_T_modifiedTransform
		 * </pre>
		 */
		class TransformModifier
		{
			friend class Transform;

			public:

				/**
				 * Returns the transformation of this modifier.
				 * @return The transformation between the modified transform and the original transform (transform_T_modifiedTransform)
				 */
				inline HomogenousMatrix4 transformation() const;

				/**
				 * Sets the transformation of this modifier.
				 * @param transform_T_modifiedTransform The transformation between the modified transform and the original transform, must be valid
				 */
				inline void setTransformation(const HomogenousMatrix4& transform_T_modifiedTransform);

			protected:

				/**
				 * Creates a new modifier with identity transformation.
				 */
				TransformModifier() = default;

			protected:

				/// The transformation of this modifier.
				HomogenousMatrix4 parent_T_transform_ = HomogenousMatrix4(true);
		};

		/**
		 * Definition of a shared pointer holding a TransformModifier object.
		 * @see TransformModifier.
		 */
		typedef std::shared_ptr<TransformModifier> SharedTransformModifier;

	public:

		/**
		 * Returns the transformation of this node relative to the parent node.
		 * @return The relative transformation (parent_T_transform)
		 */
		virtual HomogenousMatrix4 transformation() const = 0;

		/**
		 * Returns the TransformModifier object of this transform.
		 * @return The transform's modifier, nullptr if no modifier is set
		 */
		virtual SharedTransformModifier transformModifier() const = 0;

		/**
		 * Sets the transformation of this node relative to the parent node.
		 * @param parent_T_transform The transformation between this node and the parent node
		 */
		virtual void setTransformation(const HomogenousMatrix4& parent_T_transform) = 0;

		/**
		 * Sets the TransformModifier object for this transformation.
		 * @param transformModifier The TransformModifier to be set, nullptr to remove a previously set modifier
		 */
		virtual void setTransformModifier(SharedTransformModifier transformModifier) = 0;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

		/**
		 * Creates a TransformModifier object.
		 * @return The new transform modifier object
		 */
		static inline SharedTransformModifier createTransformModifier();

	protected:

		/**
		 * Creates a new transform node.
		 */
		Transform();

		/**
		 * Destructs a transform node.
		 */
		~Transform() override;
};

inline HomogenousMatrix4 Transform::TransformModifier::transformation() const
{
	ocean_assert(parent_T_transform_.isValid());

	return parent_T_transform_;
}

inline void Transform::TransformModifier::setTransformation(const HomogenousMatrix4& parent_T_transform)
{
	ocean_assert(parent_T_transform.isValid());

	parent_T_transform_ =  parent_T_transform;
}

inline Transform::SharedTransformModifier Transform::createTransformModifier()
{
	return SharedTransformModifier(new TransformModifier());
}

}

}

#endif // META_OCEAN_RENDERING_TRANSFORM_H
