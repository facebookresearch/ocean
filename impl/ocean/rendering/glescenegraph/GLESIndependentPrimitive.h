/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_INDEPENDENT_PRIMITIVE_H
#define META_OCEAN_RENDERING_GLES_INDEPENDENT_PRIMITIVE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESPrimitive.h"

#include "ocean/rendering/IndependentPrimitive.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements the base class for all GLESceneGraph independent primitive.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESIndependentPrimitive :
	virtual public GLESPrimitive,
	virtual public IndependentPrimitive
{
	protected:

		/**
		 * Creates a new GLESceneGraph independent primitive object.
		 */
		GLESIndependentPrimitive();

		/**
		 * Destructs a GLESceneGraph independent primitive object.
		 */
		~GLESIndependentPrimitive() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_INDEPENDENT_PRIMITIVE_H
