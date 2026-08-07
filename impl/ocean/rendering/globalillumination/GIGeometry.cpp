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

	const ScopedLock scopedLock(objectLock_);
	Geometry::addRenderable(renderable, attributes);
}

void GIGeometry::removeRenderable(const RenderableRef& renderable)
{
	const ScopedLock scopedLock(objectLock_);
	Geometry::removeRenderable(renderable);
}

BoundingBox GIGeometry::boundingBox(const bool /*involveLocalTransformation*/) const
{
	if (renderables_.empty())
	{
		return BoundingBox();
	}

	if (renderables_.size() == 1)
	{
		const SmartObjectRef<GIRenderable> renderable(renderables_.begin()->first);
		ocean_assert(renderable);

		return renderable->boundingBox();
	}

	BoundingBox result;

	for (const Renderables::value_type& renderablePair : renderables_)
	{
		const SmartObjectRef<GIRenderable> renderable(renderablePair.first);
		ocean_assert(renderable);

		result += renderable->boundingBox();
	}

	return result;
}

BoundingSphere GIGeometry::boundingSphere(const bool /*involveLocalTransformation*/) const
{
	if (renderables_.empty())
	{
		return BoundingSphere();
	}

	if (renderables_.size() == 1)
	{
		const SmartObjectRef<GIRenderable> renderable(renderables_.begin()->first);
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
		for (const Renderables::value_type& renderablePair : renderables_)
		{
			const SmartObjectRef<GIRenderable> renderable(renderablePair.first);
			ocean_assert(renderable);

			renderable->buildTracing(group, modelTransform, renderablePair.second, lightSources);
		}
	}
}

}

}

}
