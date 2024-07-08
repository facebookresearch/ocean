/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_NODE_REF_MANAGER_H
#define META_OCEAN_SCENEDESCRIPTION_NODE_REF_MANAGER_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Node.h"
#include "ocean/scenedescription/Scene.h"

#include "ocean/base/ObjectRef.h"
#include "ocean/base/SmartObjectRef.h"

#include <map>
#include <vector>

namespace Ocean
{

namespace SceneDescription
{

/**
 * This class implements a scene description node reference manager.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT NodeRefManager
{
	protected:

		/**
		 * Definition of a multimap mapping node names to node ids.
		 */
		typedef std::unordered_multimap<std::string, NodeId> NameMultiMap;

		/**
		 * Definition of a map mapping node ids to node references.
		 */
		typedef std::unordered_map<NodeId, NodeRef> NodeMap;

	public:

		/**
		 * Destructs the manager.
		 */
		~NodeRefManager();

		/**
		 * Returns a node by a given node id.
		 * If the node does not exist an empty reference is returned.
		 * @param nodeId Id of the node to return
		 * @return Node reference of the requested node
		 */
		NodeRef node(const NodeId nodeId) const;

		/**
		 * Returns the first node specified by a given name.
		 * If the node does not exist an empty reference is returned.
		 * @param name The name of the node
		 * @return Node reference of the requested node
		 */
		NodeRef node(const std::string& name) const;

		/**
		 * Returns the first node specified by a given name defined in a specific file.
		 * If the node does not exist an empty reference is returned.
		 * @param name The name of the node
		 * @param filename Name of the file the node must be defined inside
		 * @return Node reference of the requested node
		 */
		NodeRef node(const std::string& name, const std::string& filename) const;

		/**
		 * Returns the first node specified by a given name defined in specific scene originally.
		 * If the node does not exist an empty reference is returned.
		 * @param name The name of the node
		 * @param sceneId Id of the scene main owner of the node
		 * @return Node reference of the requested node
		 */
		NodeRef node(const std::string& name, const SceneId sceneId) const;

		/**
		 * Returns all nodes specified by a given name.
		 * @param name The name of the nodes to return
		 * @return Specified nodes
		 */
		NodeRefs nodes(const std::string& name) const;

		/**
		 * Returns a scene specified by a scene id.
		 * @param sceneId Id of the scene to return
		 * @return Specified scene
		 */
		SceneRef scene(const SceneId sceneId) const;

		/**
		 * Registers a new node.
		 * @param node Node to manage
		 * @return Node reference
		 */
		NodeRef registerNode(Node* node);

		/**
		 * Changes the name of a registered node.
		 * @param nodeId Id of the node to change
		 * @param oldName Old node name
		 * @param newName New node name
		 */
		void changeRegisteredNode(NodeId nodeId, const std::string& oldName, const std::string& newName);

		/**
		 * Unregisters a node.
		 */
		void unregisterNode(const Node* node);

	protected:

		/// Map holding all node references.
		NodeMap nodeMap_;

		/// Map mapping node names to node pointers.
		NameMultiMap nameMultiMap_;

		/// Lock for the node map
		mutable Lock lock_;
};

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_NODE_REF_MANAGER_H
