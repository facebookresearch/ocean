/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_BOX_H
#define META_OCEAN_RENDERING_GI_BOX_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIShape.h"
#include "ocean/rendering/globalillumination/GITriangles.h"

#include "ocean/rendering/Box.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a GLESceneGraph box object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GI_EXPORT GIBox :
	virtual public GIShape,
	virtual protected GITriangles,
	virtual public Box
{
	friend class GIFactory;

	public:

		/**
		 * Returns the dimensions of the box.
		 * @see Box::size().
		 */
		Vector3 size() const override;

		/**
		 * Sets the dimensions of the box.
		 * @see Box::setSize().
		 */
		bool setSize(const Vector3& size) override;

		/**
		 * Returns the bounding box of this renderable object.
		 * @see GIRenderable boundingBox().
		 */
		BoundingBox boundingBox() const override;

		/**
		 * Returns the bounding sphere of this renderable object.
		 * @see GIRenderable::boundingSphere().
		 */
		BoundingSphere boundingSphere() const override;

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

		/**
		 * Builds the tracing object for this node and adds it to an already existing group of tracing objects.
		 * @see GIRenderable::buildTracing().
		 */
		void buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const AttributeSetRef& attributes, const LightSources& lightSources) const override;

	protected:

		/**
		 * Creates a new box object.
		 */
		GIBox();

		/**
		 * Destructs a box object.
		 */
		~GIBox() override;

		/**
		 * Rebuilds the primitive vertex, normals and face sets.
		 */
		void rebuildPrimitives();

	protected:

		/// Dimension of the box.
		Vector3 boxSize;

		/// Diagonal of the box.
		Scalar boxDiagonal;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_BOX_H
