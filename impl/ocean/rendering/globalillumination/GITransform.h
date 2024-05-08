/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_TRANSFORM_H
#define META_OCEAN_RENDERING_GI_TRANSFORM_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIGroup.h"

#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a Global Illumination transform object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GITransform :
	virtual public GIGroup,
	virtual public Transform
{
	friend class GIFactory;

	public:

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
		 * Returns the bounding box of this node.
		 * @see Node::boundingBox().
		 */
		BoundingBox boundingBox(const bool involveLocalTransformation) const override;

		/**
		 * Returns the bounding sphere of this node.
		 * @see Node::boundingSphere().
		 */
		BoundingSphere boundingSphere(const bool involveLocalTransformation) const override;

		/**
		 * Builds the tracing object for this node and adds it to an already existing group of tracing objects.
		 * @see GINode::buildTracing().
		 */
		void buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const LightSources& lightSources) const override;

	protected:

		/**
		 * Creates a new Global Illumination transform node.
		 */
		GITransform();

		/**
		 * Destructs a Global Illumination transform node.
		 */
		~GITransform() override;

	private:

		/// The transformation between this object and the parent.
		HomogenousMatrix4 parent_T_object_;

		/// The optional transform modifier.
		SharedTransformModifier transformModifier_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_TRANSFORM_H
