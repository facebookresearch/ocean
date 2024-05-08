/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESGeometry.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"
#include "ocean/rendering/glescenegraph/GLESRenderable.h"
#include "ocean/rendering/glescenegraph/GLESShape.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESGeometry::GLESGeometry() :
	GLESNode(),
	Geometry()
{
	// nothing to do here
}

GLESGeometry::~GLESGeometry()
{
	// nothing to do here
}

BoundingBox GLESGeometry::boundingBox(const bool /*involveLocalTransformation*/) const
{
	BoundingBox result;

	for (Renderables::const_iterator i = geometryRenderables.cbegin(); i != geometryRenderables.cend(); ++i)
	{
		const SmartObjectRef<GLESRenderable> renderable(i->first);
		ocean_assert(renderable);

		if (renderable->boundingBox().isValid())
		{
			result += renderable->boundingBox();
		}
	}

	return result;
}

void GLESGeometry::addRenderable(const RenderableRef& renderable, const AttributeSetRef& attributes)
{
	if (renderable.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	Geometry::addRenderable(renderable, attributes);
}

void GLESGeometry::removeRenderable(const RenderableRef& renderable)
{
	const ScopedLock scopedLock(objectLock);

	Geometry::removeRenderable(renderable);
}

void GLESGeometry::addToTraverser(const GLESFramebuffer& /*framebuffer*/, const SquareMatrix4& /*projectionMatrix*/, const HomogenousMatrix4& camera_T_object, const Lights& lights, GLESTraverser& traverser) const
{
	const ScopedLock scopedLock(objectLock);

	if (!visible_ || geometryRenderables.empty())
	{
		return;
	}

	const SquareMatrix3 normalMatrix(camera_T_object.rotationMatrix().inverted().transposed());

	for (Renderables::const_iterator i = geometryRenderables.cbegin(); i != geometryRenderables.cend(); ++i)
	{
		const SmartObjectRef<GLESRenderable> renderable(i->first);
		ocean_assert(renderable);

		traverser.addRenderable(i->first, i->second, camera_T_object, normalMatrix, lights);
	}
}

}

}

}
