/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Geometry.h"

namespace Ocean
{

namespace Rendering
{

Geometry::Geometry() :
	Node()
{
	// nothing to do here
}

Geometry::~Geometry()
{
	for (Renderables::const_iterator i = geometryRenderables.begin(); i != geometryRenderables.end(); ++i)
	{
		unregisterThisObjectAsParent(i->first);
		unregisterThisObjectAsParent(i->second);
	}
}

void Geometry::addRenderable(const RenderableRef& renderable, const AttributeSetRef& attributes)
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(geometryRenderables.find(renderable) == geometryRenderables.end());

	registerThisObjectAsParent(renderable);
	registerThisObjectAsParent(attributes);
	geometryRenderables[renderable] = attributes;
}

unsigned int Geometry::numberRenderables() const
{
	return (unsigned int)(geometryRenderables.size());
}

RenderableRef Geometry::renderable(const unsigned int index) const
{
	const ScopedLock scopedLock(objectLock);

	unsigned int n = 0;
	for (Renderables::const_iterator i = geometryRenderables.begin(); i != geometryRenderables.end(); ++i)
		if (n++ == index)
			return i->first;

	return RenderableRef();
}

AttributeSetRef Geometry::attributeSet(const unsigned int index) const
{
	const ScopedLock scopedLock(objectLock);

	unsigned int n = 0;
	for (Renderables::const_iterator i = geometryRenderables.begin(); i != geometryRenderables.end(); ++i)
		if (n++ == index)
			return i->second;

	return AttributeSetRef();
}

void Geometry::removeRenderable(const RenderableRef& renderable)
{
	const ScopedLock scopedLock(objectLock);

	Renderables::iterator i = geometryRenderables.find(renderable);
	ocean_assert(i != geometryRenderables.end());

	unregisterThisObjectAsParent(i->first);
	unregisterThisObjectAsParent(i->second);

	geometryRenderables.erase(i);
}

Geometry::ObjectType Geometry::type() const
{
	return TYPE_GEOMETRY;
}

}

}
