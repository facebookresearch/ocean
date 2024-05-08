/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_TRIANGLE_STRIPS_H
#define META_OCEAN_RENDERING_GI_TRIANGLE_STRIPS_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIStripPrimitive.h"

#include "ocean/rendering/TriangleStrips.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a triangle strips object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GITriangleStrips :
	virtual public GIStripPrimitive,
	virtual public TriangleStrips
{
	friend class GIFactory;

	public:

		/**
		 * Returns the number of triangle strips defined in this primitive.
		 * @see StripPrimitive::numberStrips().
		 */
		unsigned int numberStrips() const override;

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
		 * Sets the strips of this primitive object.
		 * @see StripPtrimitive::setStrips().
		 */
		void setStrips(const VertexIndexGroups& strips) override;

		/**
		 * Builds the tracing object for this node and adds it to an already existing group of tracing objects.
		 * @see GIRenderable::buildTracing().
		 */
		void buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const AttributeSetRef& attributes, const LightSources& lightSources) const override;

	protected:

		/**
		 * Creates a new triangle strips object.
		 */
		GITriangleStrips();

		/**
		 * Destructs a triangle strips object.
		 */
		~GITriangleStrips() override;

	protected:

		/// Number of strip elements.
		unsigned int triangleStripsNumberStrips;

		/// Number of indices for all strips.
		unsigned int triangleStripsNumberIndices;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_TRIANGLE_STRIPS_H
