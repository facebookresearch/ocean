/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_PRIMITIVE_H
#define META_OCEAN_RENDERING_GLES_PRIMITIVE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESRenderable.h"

#include "ocean/rendering/Primitive.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements the base class for all GLESceneGraph primitive objects.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESPrimitive :
	virtual public GLESRenderable,
	virtual public Primitive
{
	public:

		/**
		 * Sets the vetex set of this primitive object.
		 * @see Primitive::setVertexSet().
		 */
		void setVertexSet(const VertexSetRef& vertexSet) override;

	protected:

		/**
		 * Creates a new GLESceneGraph primitive object.
		 */
		GLESPrimitive();

		/**
		 * Destructs a GLESceneGraph primitive object.
		 */
		~GLESPrimitive() override;

		/**
		 * Updates the bounding box of this primitive.
		 */
		virtual void updateBoundingBox() = 0;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_PRIMITIVE_H
