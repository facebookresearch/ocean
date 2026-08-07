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
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

#ifdef OCEAN_DEBUG

	for (const NodeRef& groupNode : nodes_)
	{
		ocean_assert(node != groupNode);
	}

#endif // OCEAN_DEBUG

	registerThisObjectAsParent(node);
	nodes_.push_back(node);
}

void Group::registerLight(const LightSourceRef& light)
{
	if (light.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	ocean_assert(!lights_.contains(light));

	lights_.insert(light);
}

unsigned int Group::numberChildren() const
{
	return (unsigned int)(nodes_.size());
}

NodeRef Group::child(const unsigned int index) const
{
	const ScopedLock scopedLock(objectLock);

	if (index >= nodes_.size())
	{
		return NodeRef();
	}

	return nodes_[index];
}

void Group::removeChild(const NodeRef& node)
{
	if (node.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	const Nodes::const_iterator iNode = std::find(nodes_.cbegin(), nodes_.cend(), node);

	if (iNode != nodes_.cend())
	{
		nodes_.erase(iNode);
	}

	unregisterThisObjectAsParent(node);
}

void Group::unregisterLight(const LightSourceRef& light)
{
	if (light.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	ocean_assert(lights_.contains(light));

	lights_.erase(light);
}

void Group::clear()
{
	const ScopedLock scopedLock(objectLock);

	for (const NodeRef& groupNode : nodes_)
	{
		unregisterThisObjectAsParent(groupNode);
	}

	nodes_.clear();
}

Group::ObjectType Group::type() const
{
	return TYPE_GROUP;
}

}

}
