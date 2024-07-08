/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_STRIP_PRIMITIVE_H
#define META_OCEAN_RENDERING_GLES_STRIP_PRIMITIVE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESPrimitive.h"

#include "ocean/rendering/StripPrimitive.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class is the base class for all GLESceneGraph stripped primitives.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESStripPrimitive :
	virtual public GLESPrimitive,
	virtual public StripPrimitive
{
	public:

		/**
		 * Returns the strips of this primitive object.
		 * @see StripPrimitive::strips().
		 */
		VertexIndexGroups strips() const override;

	protected:

		/**
		 * Creates a new GLESceneGraph stripped primitive object.
		 */
		GLESStripPrimitive();

		/**
		 * Destructs a GLESceneGraph stripped primitive object.
		 */
		~GLESStripPrimitive() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_STRIP_PRIMITIVE_H
