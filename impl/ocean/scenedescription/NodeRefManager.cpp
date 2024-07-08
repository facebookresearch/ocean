/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/NodeRefManager.h"
#include "ocean/scenedescription/SDXNode.h"

namespace Ocean
{

namespace SceneDescription
{

NodeRefManager::~NodeRefManager()
{
#ifdef OCEAN_DEBUG
	if (!nodeMap_.empty())
	{
		Log::warning() << "SceneDescription::NodeRefManager still holds " << nodeMap_.size() << " remaining nodes";

		for (NodeMap::const_iterator iNode = nodeMap_.cbegin(); iNode != nodeMap_.cend(); ++iNode)
		{
			if (!iNode->second->type().empty())
			{
				if (!iNode->second->name().empty())
				{
					Log::warning() << "Remaining node '" << iNode->second->name() << "' of type '" << iNode->second->type() << "'";
				}
				else
				{
					Log::warning() << "Remaining a node of type '" << iNode->second->type() << "'";
				}
			}
		}
	}
#endif

	ocean_assert(nodeMap_.empty());
	ocean_assert(nameMultiMap_.empty());
}

NodeRef NodeRefManager::node(const std::string& name) const
{
	ocean_assert(!name.empty());

	const ScopedLock scopedLock(lock_);

	const NameMultiMap::const_iterator iName = nameMultiMap_.find(name);

	if (iName != nameMultiMap_.cend())
	{
		const NodeMap::const_iterator iNode = nodeMap_.find(iName->second);

		if (iNode != nodeMap_.cend())
		{
			return iNode->second;
		}
	}

	return NodeRef();
}

NodeRef NodeRefManager::node(const std::string& name, const std::string& filename) const
{
	ocean_assert(!name.empty());

	const ScopedLock scopedLock(lock_);

	const std::pair<NameMultiMap::const_iterator, NameMultiMap::const_iterator> nameRange = nameMultiMap_.equal_range(name);

	for (NameMultiMap::const_iterator iName = nameRange.first; iName != nameRange.second; ++iName)
	{
		const NodeMap::const_iterator iNode = nodeMap_.find(iName->second);

		if (iNode != nodeMap_.cend())
		{
			const SDXNodeRef sdxNode(iNode->second);

			if (sdxNode->filename() == filename)
			{
				return iNode->second;
			}
		}
	}

	return NodeRef();
}

NodeRef NodeRefManager::node(const std::string& name, const SceneId sceneId) const
{
	ocean_assert(!name.empty());

	const ScopedLock scopedLock(lock_);

	const std::pair<NameMultiMap::const_iterator, NameMultiMap::const_iterator> nameRange = nameMultiMap_.equal_range(name);

	for (NameMultiMap::const_iterator iName = nameRange.first; iName != nameRange.second; ++iName)
	{
		const NodeMap::const_iterator iNode = nodeMap_.find(iName->second);

		if (iNode != nodeMap_.cend())
		{
			const SDXNodeRef sdxNode(iNode->second);

			if (sdxNode->sceneId() == sceneId)
			{
				return iNode->second;
			}
		}
	}

	return NodeRef();
}

NodeRef NodeRefManager::node(const NodeId nodeId) const
{
	if (nodeId == invalidNodeId)
	{
		return NodeRef();
	}

	const ScopedLock scopedLock(lock_);

	const NodeMap::const_iterator iNode = nodeMap_.find(nodeId);

	if (iNode == nodeMap_.cend())
	{
		return NodeRef();
	}

	return iNode->second;
}

NodeRefs NodeRefManager::nodes(const std::string& name) const
{
	ocean_assert(!name.empty());

	const ScopedLock scopedLock(lock_);

	NodeRefs nodes;

	const std::pair<NameMultiMap::const_iterator, NameMultiMap::const_iterator> nameRange = nameMultiMap_.equal_range(name);

	for (NameMultiMap::const_iterator iName = nameRange.first; iName != nameRange.second; ++iName)
	{
		const NodeMap::const_iterator iNode = nodeMap_.find(iName->second);

		if (iNode != nodeMap_.cend())
		{
			nodes.push_back(iNode->second);
		}
	}

	return nodes;
}

SceneRef NodeRefManager::scene(const SceneId sceneId) const
{
	return node(NodeId(sceneId));
}

NodeRef NodeRefManager::registerNode(Node* node)
{
	ocean_assert(node != nullptr);

	const ScopedLock scopedLock(lock_);

	const NodeRef nodeRef(node, NodeRef::ReleaseCallback(*this, &NodeRefManager::unregisterNode));

	const std::string& name(node->name());
	const NodeId nodeId(node->id());
	ocean_assert(nodeId != invalidNodeId);

	ocean_assert(nodeMap_.find(nodeId) == nodeMap_.cend());

	if (name.empty() == false)
	{
		nameMultiMap_.emplace(name, nodeId);
	}

	nodeMap_.emplace(nodeId, nodeRef);

	return nodeRef;
}

void NodeRefManager::changeRegisteredNode(NodeId nodeId, const std::string& oldName, const std::string& newName)
{
	ocean_assert(nodeId != invalidNodeId);
	ocean_assert(oldName != newName);

	const ScopedLock scopedLock(lock_);

	ocean_assert(nodeMap_.find(nodeId) != nodeMap_.cend());

	if (!oldName.empty())
	{
#ifdef OCEAN_DEBUG
		bool debugFound = false;
#endif
		ocean_assert(nameMultiMap_.find(oldName) != nameMultiMap_.cend());

		const std::pair<NameMultiMap::iterator, NameMultiMap::iterator> nameRange = nameMultiMap_.equal_range(oldName);

		for (NameMultiMap::iterator iName = nameRange.first; iName != nameRange.second; ++iName)
		{
			if (iName->second == nodeId)
			{
				nameMultiMap_.erase(iName);

#ifdef OCEAN_DEBUG
				debugFound = true;
#endif
				break;
			}
		}

		ocean_assert(debugFound);
	}

	if (!newName.empty())
	{
		nameMultiMap_.insert(std::make_pair(newName, nodeId));
	}
}

void NodeRefManager::unregisterNode(const Node* node)
{
	ocean_assert(node != nullptr);

	const ScopedLock scopedLock(lock_);

	ocean_assert(node->id() != invalidNodeId);

	NodeMap::iterator iNode(nodeMap_.find(node->id()));
	ocean_assert(iNode != nodeMap_.cend());
	ocean_assert(iNode->second);

	if (!node->name().empty())
	{
		NameMultiMap::iterator iName(nameMultiMap_.find(node->name()));
		ocean_assert(iName != nameMultiMap_.cend());

		nameMultiMap_.erase(iName);
	}

	nodeMap_.erase(iNode);
}

}

}
