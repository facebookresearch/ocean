/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_CONE_H
#define META_OCEAN_RENDERING_GLES_CONE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESShape.h"
#include "ocean/rendering/glescenegraph/GLESTriangles.h"

#include "ocean/rendering/Cone.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph cone object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESCone :
	virtual public GLESShape,
	virtual protected GLESTriangles,
	virtual public Cone
{
	friend class GLESFactory;

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
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new GLESceneGraph cone object.
		 */
		GLESCone();

		/**
		 * Destructs a GLESceneGraph cone object.
		 */
		~GLESCone() override;

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

		/// Holds the height of this cone.
		Scalar height_ = Scalar(1);

		/// Holds the radius of this cone.
		Scalar radius_ = Scalar(1);
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_CONE_H
