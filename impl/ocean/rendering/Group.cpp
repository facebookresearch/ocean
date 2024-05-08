/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace Rendering
{

Group::Group() :
	Node()
{
	// nothing to do here
}

Group::~Group()
{
	clear();
}

void Group::addChild(const NodeRef& node)
{
	if (node.isNull())
		return;

	const ScopedLock scopedLock(objectLock);

#ifdef OCEAN_DEBUG

	for (Nodes::iterator i = groupNodes.begin(); i != groupNodes.end(); ++i)
		ocean_assert(node != *i);

#endif // OCEAN_DEBUG

	registerThisObjectAsParent(node);
	groupNodes.push_back(node);
}

void Group::registerLight(const LightSourceRef& light)
{
	if (light.isNull())
		return;

	const ScopedLock scopedLock(objectLock);

	ocean_assert(groupLights.find(light) == groupLights.end());

	groupLights.insert(light);
}

unsigned int Group::numberChildren() const
{
	return (unsigned int)(groupNodes.size());
}

NodeRef Group::child(const unsigned int index) const
{
	const ScopedLock scopedLock(objectLock);

	if (index >= groupNodes.size())
		return NodeRef();

	return groupNodes[index];
}

void Group::removeChild(const NodeRef& node)
{
	if (node.isNull())
		return;

	const ScopedLock scopedLock(objectLock);

	for (Nodes::iterator i = groupNodes.begin(); i != groupNodes.end(); ++i)
		if (node == *i)
		{
			groupNodes.erase(i);
			break;
		}

	unregisterThisObjectAsParent(node);
}

void Group::unregisterLight(const LightSourceRef& light)
{
	if (light.isNull())
		return;

	const ScopedLock scopedLock(objectLock);

	ocean_assert(groupLights.find(light) != groupLights.end());

	groupLights.erase(light);
}

void Group::clear()
{
	const ScopedLock scopedLock(objectLock);

	for (Nodes::iterator i = groupNodes.begin(); i != groupNodes.end(); ++i)
		unregisterThisObjectAsParent(*i);

	groupNodes.clear();
}

Group::ObjectType Group::type() const
{
	return TYPE_GROUP;
}

}

}
