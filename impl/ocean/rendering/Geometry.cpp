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
	for (Renderables::value_type& renderablePair : renderables_)
	{
		unregisterThisObjectAsParent(renderablePair.first);
		unregisterThisObjectAsParent(renderablePair.second);
	}
}

void Geometry::addRenderable(const RenderableRef& renderable, const AttributeSetRef& attributes)
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(!renderables_.contains(renderable));

	registerThisObjectAsParent(renderable);
	registerThisObjectAsParent(attributes);

	renderables_[renderable] = attributes;
}

unsigned int Geometry::numberRenderables() const
{
	return (unsigned int)(renderables_.size());
}

RenderableRef Geometry::renderable(const unsigned int index) const
{
	const ScopedLock scopedLock(objectLock);

	unsigned int n = 0u;

	for (const Renderables::value_type& renderablePair : renderables_)
	{
		if (n++ == index)
		{
			return renderablePair.first;
		}
	}

	return RenderableRef();
}

AttributeSetRef Geometry::attributeSet(const unsigned int index) const
{
	const ScopedLock scopedLock(objectLock);

	unsigned int n = 0u;

	for (const Renderables::value_type& renderablePair : renderables_)
	{
		if (n++ == index)
		{
			return renderablePair.second;
		}
	}

	return AttributeSetRef();
}

void Geometry::removeRenderable(const RenderableRef& renderable)
{
	ocean_assert(renderable);

	const ScopedLock scopedLock(objectLock);

	const Renderables::const_iterator iRenderable = renderables_.find(renderable);

	if (iRenderable == renderables_.cend())
	{
		return;
	}

	unregisterThisObjectAsParent(iRenderable->first);
	unregisterThisObjectAsParent(iRenderable->second);

	renderables_.erase(iRenderable);
}

Geometry::ObjectType Geometry::type() const
{
	return TYPE_GEOMETRY;
}

}

}
