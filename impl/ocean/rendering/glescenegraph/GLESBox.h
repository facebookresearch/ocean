/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_BOX_H
#define META_OCEAN_RENDERING_GLES_BOX_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESShape.h"
#include "ocean/rendering/glescenegraph/GLESTriangles.h"

#include "ocean/rendering/Box.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph box object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESBox :
	virtual public GLESShape,
	virtual protected GLESTriangles,
	virtual public Box
{
	friend class GLESFactory;

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
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new GLESceneGraph box object.
		 */
		GLESBox();

		/**
		 * Destructs a GLESceneGraph box object.
		 */
		~GLESBox() override;

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

		/// The dimension of the box.
		Vector3 size_ = Vector3(1, 1, 1);
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_BOX_H
