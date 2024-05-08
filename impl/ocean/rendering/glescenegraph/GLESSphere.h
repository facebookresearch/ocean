/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_SPHERE_H
#define META_OCEAN_RENDERING_GLES_SPHERE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESTriangleStrips.h"
#include "ocean/rendering/glescenegraph/GLESShape.h"

#include "ocean/rendering/Sphere.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph spehere node.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESSphere :
	virtual public GLESShape,
	virtual protected GLESTriangleStrips,
	virtual public Sphere
{
	friend class GLESFactory;

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
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new GLESceneGraph sphere node.
		 */
		GLESSphere();

		/**
		 * Destructs a GLESceneGraph sphere node.
		 */
		~GLESSphere() override;

		/**
		 * Rebuilds the primitive vertex, normals and face sets.
		 */
		void rebuildPrimitives() override;

		/**
		 * Updates the bounding box of this primitive.
		 * @see GLESIndependentPrimitive::updateBoundingBox().
		 */
		void updateBoundingBox() override;

	private:

		/// Radius of this sphere
		Scalar radius_ = Scalar(1);
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_SPHERE_H
