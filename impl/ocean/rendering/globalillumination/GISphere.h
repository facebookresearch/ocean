/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_SPHERE_H
#define META_OCEAN_RENDERING_GI_SPHERE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIShape.h"
#include "ocean/rendering/globalillumination/GITriangleStrips.h"

#include "ocean/rendering/Sphere.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a spehere node.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GISphere :
	virtual public GIShape,
	virtual protected GITriangleStrips,
	virtual public Sphere
{
	friend class GIFactory;

	public:

		/**
		 * Returns the radius of this sphere.
		 * @see Sphere::radius().
		 */
		Scalar radius() const override;

		/**
		 * Sets the radius of this sphere.
		 * @see Sphere::setRadius().
		 */
		bool setRadius(const Scalar radius) override;

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
		 * Creates a new sphere node.
		 */
		GISphere();

		/**
		 * Destructs a sphere node.
		 */
		~GISphere() override;

		/**
		 * Rebuilds the primitive vertex, normals and face sets.
		 */
		void rebuildPrimitives();

	private:

		/// Radius of this sphere
		Scalar sphereRadius;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_SPHERE_H
