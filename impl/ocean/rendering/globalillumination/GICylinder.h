/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_CYLINDER_H
#define META_OCEAN_RENDERING_GI_CYLINDER_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIShape.h"
#include "ocean/rendering/globalillumination/GITriangleStrips.h"

#include "ocean/rendering/Cylinder.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a cylinder object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GICylinder :
	virtual public GIShape,
	virtual protected GITriangleStrips,
	virtual public Cylinder
{
	friend class GIFactory;

	public:

		/**
		 * Returns the height of this cylinder.
		 * @see Cylinder::height().
		 */
		Scalar height() const override;

		/**
		 * Returns the radius of this cylinder.
		 * @see Cylinder::radius().
		 */
		Scalar radius() const override;

		/**
		 * Sets the height of this cylinder.
		 * @see Cylinder::setHeight().
		 */
		bool setHeight(const Scalar height) override;

		/**
		 * Sets the radius of this cylinder.
		 * @see Cylinder::setRadius().
		 */
		bool setRadius(const Scalar radius) override;

		/**
		 * Sets whether the top face of the cylinder is visible.
		 * @see Cylinder::setTop().
		 */
		bool setTop(const bool visible) override;

		/**
		 * Sets whether the side face of the cylinder is visible.
		 * @see Cylinder::setSide().
		 */
		bool setSide(const bool visible) override;

		/**
		 * Sets whether the bottom face of the cylinder is visible.
		 * @see Cylinder::setBottom().
		 */
		bool setBottom(const bool visible) override;

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
		 * Creates a new cylinder object.
		 */
		GICylinder();

		/**
		 * Destructs a cylinder object.
		 */
		~GICylinder() override;

		/**
		 * Rebuilds the primitive vertex, normals and face sets.
		 */
		void rebuildPrimitives();

	protected:

		/// Holds the height of this cylinder.
		Scalar cylinderHeight;

		/// Holds the radius of this cylinder.
		Scalar cylinderRadius;

		/// Holds the state whether the top face is visible.
		bool cylinderTop;

		/// Holds the state whether the side face is visible.
		bool cylinderSide;

		/// Holds the state whether the bottom face is visible.
		bool cylinderBottom;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_CYLINDER_H
