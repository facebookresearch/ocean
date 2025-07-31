/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIGeometry.h"
#include "ocean/rendering/globalillumination/GIRenderable.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIGeometry::GIGeometry() :
	GINode(),
	Geometry()
{
	// nothing to do here
}

GIGeometry::~GIGeometry()
{
	// nothing to do here
}

void GIGeometry::addRenderable(const RenderableRef& renderable, const AttributeSetRef& attributes)
{
	if (renderable.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);
	Geometry::addRenderable(renderable, attributes);
}

void GIGeometry::removeRenderable(const RenderableRef& renderable)
{
	const ScopedLock scopedLock(objectLock);
	Geometry::removeRenderable(renderable);
}

BoundingBox GIGeometry::boundingBox(const bool /*involveLocalTransformation*/) const
{
	if (geometryRenderables.empty())
	{
		return BoundingBox();
	}

	if (geometryRenderables.size() == 1)
	{
		const SmartObjectRef<GIRenderable> renderable(geometryRenderables.begin()->first);
		ocean_assert(renderable);

		return renderable->boundingBox();
	}

	BoundingBox result;

	for (Renderables::const_iterator i = geometryRenderables.begin(); i != geometryRenderables.end(); ++i)
	{
		const SmartObjectRef<GIRenderable> renderable(i->first);
		ocean_assert(renderable);

		result += renderable->boundingBox();
	}

	return result;
}

BoundingSphere GIGeometry::boundingSphere(const bool /*involveLocalTransformation*/) const
{
	if (geometryRenderables.empty())
		return BoundingSphere();

	if (geometryRenderables.size() == 1)
	{
		const SmartObjectRef<GIRenderable> renderable(geometryRenderables.begin()->first);
		ocean_assert(renderable);

		return renderable->boundingSphere();
	}

	return BoundingSphere(boundingBox());
}

void GIGeometry::buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const LightSources& lightSources) const
{
	ocean_assert(modelTransform.isValid());

	if (nodeVisible)
	{
		for (Renderables::const_iterator i = geometryRenderables.begin(); i != geometryRenderables.end(); ++i)
		{
			const SmartObjectRef<GIRenderable> renderable(i->first);
			ocean_assert(renderable);

			renderable->buildTracing(group, modelTransform, i->second, lightSources);
		}
	}
}

}

}

}
