/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Node.h"
#include "ocean/rendering/Engine.h"

#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace Rendering
{

Node::Node() :
	Object()
{
	// nothing to do here
}

Node::~Node()
{
	// nothing to do here
}

Node::ObjectType Node::type() const
{
	return TYPE_NODE;
}

bool Node::visible() const
{
	return false;
}

BoundingBox Node::boundingBox(const bool /*involveLocalTransformation*/) const
{
	ocean_assert(false && "Missing implementation!");
	return BoundingBox();
}

BoundingSphere Node::boundingSphere(const bool involveLocalTransformation) const
{
	const BoundingBox box = boundingBox(involveLocalTransformation);

	if (box.isValid())
	{
		return BoundingSphere(box);
	}

	return BoundingSphere();
}

HomogenousMatrix4 Node::worldTransformation() const
{
	const ScopedLock scopedLock(objectLock);

	const ObjectRefSet parents(parentNodes());

	HomogenousMatrix4 world_T_node(true);

	if (!parents.empty())
	{
		const NodeRef parent(*parents.begin());
		ocean_assert(parent);

		world_T_node = parent->worldTransformation();
	}

	// in case our node is a transform node, we need to concatenate our transformation

	const Transform* transformNode = dynamic_cast<const Transform*>(this);

	if (transformNode)
	{
		const HomogenousMatrix4 parent_T_node(transformNode->transformation());

		world_T_node *= parent_T_node;
	}

	return world_T_node;
}

HomogenousMatrices4 Node::worldTransformations() const
{
	const ScopedLock scopedLock(objectLock);

	const ObjectRefSet parentObjects(parentNodes());

	HomogenousMatrices4 world_T_nodes;
	world_T_nodes.reserve(parentObjects.size());

	for (const ObjectRef& parentObject : parentObjects)
	{
		const NodeRef parentNode(parentObject);
		ocean_assert(parentNode);

		const HomogenousMatrices4 world_T_parents(parentNode->worldTransformations());
		world_T_nodes.insert(world_T_nodes.end(), world_T_parents.begin(), world_T_parents.end());
	}

	if (world_T_nodes.empty())
	{
		world_T_nodes.emplace_back(true); // identity
	}

	// in case our node is a transform node, we need to concatenate our transformation

	const Transform* transformNode = dynamic_cast<const Transform*>(this);

	if (transformNode)
	{
		const HomogenousMatrix4 parent_T_node(transformNode->transformation());

		for (HomogenousMatrix4& world_T_node : world_T_nodes)
		{
			world_T_node *= parent_T_node;
		}
	}

	return world_T_nodes;
}

void Node::setVisible(const bool /*visible*/)
{
	ocean_assert(false && "Should be implemented in a derived class!");
}

}

}
