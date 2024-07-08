/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_SHAPE_H
#define META_OCEAN_RENDERING_GLES_SHAPE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESRenderable.h"

#include "ocean/rendering/ObjectRef.h"
#include "ocean/rendering/Shape.h"
#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class is the base class for all GLESceneGraph shape objects.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESShape :
	virtual public GLESRenderable,
	virtual public Shape
{
	protected:

		/**
		 * Creates a new GLESceneGraph shape object.
		 */
		GLESShape();

		/**
		 * Destructs a GLESceneGrpah shape object.
		 */
		~GLESShape() override;

		/**
		 * Rebuilds the primitive vertex, normals and face sets.
		 */
		virtual void rebuildPrimitives() = 0;

	protected:

		/// Vertex set holding vertices and normals
		VertexSetRef shapeVertexSet;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_SHAPE_H
