/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_ABSOLUTE_TRANSFORM_H
#define META_OCEAN_RENDERING_GLES_ABSOLUTE_TRANSFORM_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESGroup.h"

#include "ocean/rendering/AbsoluteTransform.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph absolute transform node.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESAbsoluteTransform :
	virtual public GLESGroup,
	virtual public AbsoluteTransform
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the current transfomation type.
		 * @see AbsoluteTransform::transformationType().
		 */
		TransformationType transformationType() const override;

		/**
		 * Returns the relative screen position of the head-up node.
		 * @see AbsoluteTransformation::headUpRelativePosition().
		 */
		Vector2 headUpRelativePosition() const override;

		/**
		 * Returns the transformation which is applied on top of the absolute transformation.
		 * @see AbsoluteTransformation::transformation().
		 */
		HomogenousMatrix4 transformation() const override;

		/**
		 * Sets or changes the transfomation type.
		 * @see AbsoluteTransform::setTransformationType().
		 */
		bool setTransformationType(const TransformationType type) override;

		/**
		 * Sets the relative screen position of the head-up node.
		 * @see AbsoluteTransform::setHeadUpRelativePosition().
		 */
		bool setHeadUpRelativePosition(const Vector2& position) override;

		/**
		 * Sets an additional transformation which is applied on top of the absolute transformation.
		 * @see AbsoluteTransform::setTransformation().
		 */
		void setTransformation(const HomogenousMatrix4& absolute_T_children) override;

		/**
		 * Adds this node and all child node to a traverser.
		 * @see GLESNode::addToTraverser().
		 */
		void addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_parent, const Lights& lights, GLESTraverser& traverser) const override;

	protected:

		/**
		 * Creates a new GLESceneGraph absolute transform node.
		 */
		GLESAbsoluteTransform();

		/**
		 * Destructs a GLESceneGraph absolute transform node.
		 */
		~GLESAbsoluteTransform() override;

	protected:

		/// The transformation type to be used.
		TransformationType transformationType_ = TT_NONE;

		/// The relative screen position, for headup transformations.
		Vector2 headUpPosition_ = Vector2(Scalar(0.5), Scalar(0.5));

		/// The transformation between all children and the absolute transformation.
		HomogenousMatrix4 absolute_T_children_ = HomogenousMatrix4(true);
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_ABSOLUTE_TRANSFORM_H
