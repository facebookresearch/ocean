/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_TRIANGLES_H
#define META_OCEAN_RENDERING_GI_TRIANGLES_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIIndependentPrimitive.h"

#include "ocean/rendering/Triangles.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements triangles.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GITriangles :
	virtual public GIIndependentPrimitive,
	virtual public Triangles
{
	friend class GIFactory;

	public:

		/**
		 * Returns the faces of this triangles object.
		 * @see Triangles::faces().
		 */
		TriangleFaces faces() const override;

		/**
		 * Returns the number of triangle faces defined in this primitive.
		 * @see Triangles::numberFaces().
		 */
		unsigned int numberFaces() const override;

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
		 * Sets the faces of this triangles object.
		 * @see Triangles::setFaces().
		 */
		void setFaces(const TriangleFaces& faces) override;

		/**
		 * Builds the tracing object for this node and adds it to an already existing group of tracing objects.
		 * @see GIRenderable::buildTracing().
		 */
		void buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const AttributeSetRef& attributes, const LightSources& lightSources) const override;

	protected:

		/**
		 * Creates a new triangles object.
		 */
		GITriangles();

		/**
		 * Destructs a triangles object.
		 */
		~GITriangles() override;

	protected:

		/// Faces of the triangles.
		TriangleFaces trianglesFaces;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_TRIANGLES_H
