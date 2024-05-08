/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_CYLINDER_H
#define META_OCEAN_RENDERING_GLES_CYLINDER_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESShape.h"
#include "ocean/rendering/glescenegraph/GLESTriangleStrips.h"

#include "ocean/rendering/Cylinder.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph cylinder object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESCylinder :
	virtual public GLESShape,
	virtual protected GLESTriangleStrips,
	virtual public Cylinder
{
	friend class GLESFactory;

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
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new GLESceneGraph cylinder object.
		 */
		GLESCylinder();

		/**
		 * Destructs a GLESceneGraph cylinder object.
		 */
		~GLESCylinder() override;

		/**
		 * Rebuilds the primitive vertex, normals and face sets.
		 * @see GLESShape::rebuildPrimitives().
		 */
		void rebuildPrimitives() override;

		/**
		 * Updates the bounding box of this primitive.
		 * @see GLESIndependentPrimitive::updateBoundingBox().
		 */
		void updateBoundingBox() override;

	protected:

		/// Holds the height of this cylinder.
		Scalar height_ = Scalar(1);

		/// Holds the radius of this cylinder.
		Scalar radius_ = Scalar(1);
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_CYLINDER_H
