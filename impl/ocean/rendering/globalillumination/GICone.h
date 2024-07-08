/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_CONE_H
#define META_OCEAN_RENDERING_GI_CONE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIShape.h"
#include "ocean/rendering/globalillumination/GITriangles.h"

#include "ocean/rendering/Cone.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a cone object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GICone :
	virtual public GIShape,
	virtual protected GITriangles,
	virtual public Cone
{
	friend class GIFactory;

	public:

		/**
		 * Returns the radius of this cone.
		 * @see Cone::radius().
		 */
		Scalar radius() const override;

		/**
		 * Returns the height of this cone.
		 * @see Cone::height().
		 */
		Scalar height() const override;

		/**
		 * Sets the radius of this cone.
		 * @see Cone::setRadius().
		 */
		bool setRadius(const Scalar radius) override;

		/**
		 * Sets the height of this cone.
		 * @see Cone::setHeight().
		 */
		bool setHeight(const Scalar height) override;

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

	protected:

		/**
		 * Creates a new cone object.
		 */
		GICone();

		/**
		 * Destructs a cone object.
		 */
		~GICone() override;

		/**
		 * Rebuilds the primitive vertex, normals and face sets.
		 */
		void rebuildPrimitives();

	protected:

		/// Holds the height of this cone.
		Scalar coneHeight;

		/// Holds the radius of this cone.
		Scalar coneRadius;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_CONE_H
