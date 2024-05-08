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
	for (Attributes::const_iterator i = setAttributes.begin(); i != setAttributes.end(); ++i)
		unregisterThisObjectAsParent(*i);
}

AttributeRef AttributeSet::attribute(const unsigned int index) const
{
	ScopedLock scopedLock(objectLock);

	if (index >= setAttributes.size())
		return ObjectRef();

	unsigned int n = 0;
	for (Attributes::const_iterator i = setAttributes.begin(); i != setAttributes.end(); ++i)
		if (index == n++)
			return *i;

	ocean_assert(false && "This should never happen");
	return ObjectRef();
}

AttributeRef AttributeSet::attribute(const ObjectType attributeType) const
{
	ScopedLock scopedLock(objectLock);

	for (Attributes::const_iterator i = setAttributes.begin(); i != setAttributes.end(); ++i)
	{
		ocean_assert(*i);
		if ((*i)->type() == attributeType)
			return *i;
	}

	return ObjectRef();
}

bool AttributeSet::hasAttribute(const ObjectType attributeType) const
{
	ScopedLock scopedLock(objectLock);

	for (Attributes::const_iterator i = setAttributes.begin(); i != setAttributes.end(); ++i)
	{
		ocean_assert(*i);
		if ((*i)->type() == attributeType)
			return true;
	}

	return false;
}

unsigned int AttributeSet::numberAttributes() const
{
	ScopedLock scopedLock(objectLock);

	return (unsigned int)(setAttributes.size());
}

void AttributeSet::addAttribute(const AttributeRef& attribute)
{
	if (attribute.isNull())
		return;

	ScopedLock scopedLock(objectLock);

	ocean_assert(setAttributes.find(attribute) == setAttributes.end());
	ocean_assert(AttributeSet::attribute(attribute->type()).isNull());

#ifdef OCEAN_DEBUG

	ShaderProgramRef shaderProgram(attribute);
	if (shaderProgram)
		ocean_assert(shaderProgram->isCompiled());

#endif

	registerThisObjectAsParent(attribute);
	setAttributes.insert(attribute);
}

void AttributeSet::removeAttribute(const AttributeRef& attribute)
{
	ScopedLock scopedLock(objectLock);

	ocean_assert(setAttributes.find(attribute) != setAttributes.end());

	unregisterThisObjectAsParent(attribute);
	setAttributes.erase(attribute);
}

bool AttributeSet::removeAttribute(const ObjectType attributeType)
{
	ScopedLock scopedLock(objectLock);

	for (Attributes::iterator i = setAttributes.begin(); i != setAttributes.end(); ++i)
	{
		ocean_assert(*i);
		if ((*i)->type() == attributeType)
		{
			unregisterThisObjectAsParent(*i);

			setAttributes.erase(i);
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
