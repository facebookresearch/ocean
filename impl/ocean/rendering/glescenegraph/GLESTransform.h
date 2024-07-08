/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_TRANSFORM_H
#define META_OCEAN_RENDERING_GLES_TRANSFORM_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESGroup.h"

#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph transform object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESTransform :
	virtual public GLESGroup,
	virtual public Transform
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the bounding box of this node.
		 * @see Node::boundingBox().
		 */
		BoundingBox boundingBox(const bool involveLocalTransformation) const override;

		/**
		 * Returns the transformation of this node relative to the parent node.
		 * @see Transform::transformation().
		 */
		HomogenousMatrix4 transformation() const override;

		/**
		 * Returns the TransformModifier object of this transform.
		 * @see Transform::transformModifier().
		 */
		SharedTransformModifier transformModifier() const override;

		/**
		 * Sets the transformation of this node relative to the parent node.
		 * @see Transform::setTransformation().
		 */
		void setTransformation(const HomogenousMatrix4& parent_T_transform) override;

		/**
		 * Sets the TransformModifier object for this transformation.
		 * @see Transform::setTransformModifier().
		 */
		void setTransformModifier(SharedTransformModifier transformModifier) override;

		/**
		 * Adds this node and all child node to a traverser.
		 * @see GLESNode::addToTraverser().
		 */
		void addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_parent, const Lights& lights, GLESTraverser& traverser) const override;

	protected:

		/**
		 * Creates a new GLESceneGraph transform node.
		 */
		GLESTransform();

		/**
		 * Destructs a GLESceneGraph transform node.
		 */
		~GLESTransform() override;

	protected:

		/// The transformation between this object and the parent.
		HomogenousMatrix4 parent_T_object_;

		/// The optional transform modifier.
		SharedTransformModifier transformModifier_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_TRANSFORM_H
