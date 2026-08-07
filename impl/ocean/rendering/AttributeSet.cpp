/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/ShaderProgram.h"

namespace Ocean
{

namespace Rendering
{

AttributeSet::AttributeSet() :
	Object()
{
	// nothing to do here
}

AttributeSet::~AttributeSet()
{
	for (const AttributeRef& attribute : attributes_)
	{
		unregisterThisObjectAsParent(attribute);
	}
}

AttributeRef AttributeSet::attribute(const unsigned int index) const
{
	const ScopedLock scopedLock(objectLock);

	if (index >= attributes_.size())
	{
		return ObjectRef();
	}

	unsigned int n = 0u;

	for (const AttributeRef& attribute : attributes_)
	{
		if (index == n++)
		{
			return attribute;
		}
	}

	ocean_assert(false && "This should never happen");
	return ObjectRef();
}

AttributeRef AttributeSet::attribute(const ObjectType attributeType) const
{
	const ScopedLock scopedLock(objectLock);

	for (const AttributeRef& attribute : attributes_)
	{
		ocean_assert(attribute);

		if (attribute->type() == attributeType)
		{
			return attribute;
		}
	}

	return ObjectRef();
}

bool AttributeSet::hasAttribute(const ObjectType attributeType) const
{
	const ScopedLock scopedLock(objectLock);

	for (const AttributeRef& attribute : attributes_)
	{
		ocean_assert(attribute);

		if (attribute->type() == attributeType)
		{
			return true;
		}
	}

	return false;
}

unsigned int AttributeSet::numberAttributes() const
{
	const ScopedLock scopedLock(objectLock);

	return (unsigned int)(attributes_.size());
}

void AttributeSet::addAttribute(const AttributeRef& attribute)
{
	if (attribute.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	ocean_assert(!attributes_.contains(attribute));
	ocean_assert(AttributeSet::attribute(attribute->type()).isNull());

#ifdef OCEAN_DEBUG

	const ShaderProgramRef shaderProgram(attribute);

	if (shaderProgram)
	{
		ocean_assert(shaderProgram->isCompiled());
	}

#endif

	registerThisObjectAsParent(attribute);
	attributes_.insert(attribute);
}

void AttributeSet::removeAttribute(const AttributeRef& attribute)
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(attributes_.contains(attribute));

	unregisterThisObjectAsParent(attribute);
	attributes_.erase(attribute);
}

bool AttributeSet::removeAttribute(const ObjectType attributeType)
{
	const ScopedLock scopedLock(objectLock);

	for (Attributes::const_iterator iAttribute = attributes_.cbegin(); iAttribute != attributes_.cend(); ++iAttribute)
	{
		ocean_assert(*iAttribute);

		if ((*iAttribute)->type() == attributeType)
		{
			unregisterThisObjectAsParent(*iAttribute);

			attributes_.erase(iAttribute);
			return true;
		}
	}

	return false;
}

AttributeSet::ObjectType AttributeSet::type() const
{
	return TYPE_ATTRIBUTE_SET;
}

}

}
