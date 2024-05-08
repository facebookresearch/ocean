/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_NODE_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Library.h"
#include "ocean/scenedescription/Node.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Object.h"
#include "ocean/rendering/Scene.h"

#include <map>

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration.
class SDXNode;
// Forward declaration.
class SDXScene;

/**
 * Definition of a smart object reference for SDX scene description nodes.
 * @see SDXNode, Node.
 * @ingroup scenedescription
 */
typedef SmartObjectRef<SDXNode, Node> SDXNodeRef;

/**
 * Definition of a set holding SDX node references.
 * @ingroup scenedescription
 */
typedef std::set<SDXNodeRef> SDXNodeSet;

/**
 * This class implements the base class for all SDX scene description nodes.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT SDXNode : virtual public Node
{
	public:

		/**
		 * Definition of a map mapping node ids to reference counters.
		 */
		typedef std::unordered_map<NodeId, unsigned int> NodeIdMap;

		/**
		 * This class implements a node environment container.
		 */
		class OCEAN_SCENEDESCRIPTION_EXPORT SDXEnvironment
		{
			public:

				/**
				 * Creates an empty environment object.
				 */
				SDXEnvironment() = default;

				/**
				 * Creates a new environment object.
				 * @param engine Rendering engine used in the environment
				 * @param library Library defining the node
				 * @param filename Name of the file defining the node
				 * @param sceneId Unique id defining the scene
				 */
				inline SDXEnvironment(const Rendering::EngineRef& engine, const Library* library, const std::string& filename, const SceneId sceneId);

				/**
				 * Returns the rendering engine used in the environment.
				 * @return Environment rendering engine
				 */
				inline const Rendering::EngineRef& engine() const;

				/**
				 * Returns the name of the scene description library defining the node.
				 * @return Library name
				 */
				inline const Library* library() const;

				/**
				 * Returns the name of the file defining the node.
				 * @return Scene filename
				 */
				inline const std::string& filename() const;

				/**
				 * Returns the unique id of the scene defining the node originally.
				 * @return Scene id
				 */
				inline SceneId sceneId() const;

			private:

				/// Rendering engine the node is connected with.
				Rendering::EngineRef engine_;

				/// Owner library.
				const Library* library_ = nullptr;

				/// Name of the file defining the node.
				std::string filename_;

				/// Unique environment scene id.
				SceneId sceneId_ = invalidSceneId;
		};

	public:

		/**
		 * Returns the scene description type of this node.
		 * @see Node::descriptionType().
		 */
		DescriptionType descriptionType() const override;

		/**
		 * Returns the rendering engine used by this node.
		 * @return Rendering engine
		 */
		inline const Rendering::EngineRef& engine() const;

		/**
		 * Returns the name of the scene description library defining this node.
		 * @return Library name
		 */
		inline const std::string& library() const;

		/**
		 * Returns the name of the file defining this node.
		 * Beware: A node can be defined outside a file context and thus does not have a file!
		 * @return Filename
		 */
		inline const std::string& filename() const;

		/**
		 * Returns the unique id of the scene defining this node originally.
		 * @return Unique scene id
		 */
		inline SceneId sceneId() const;

		/**
		 * Returns the associated rendering object.
		 * @return Associated rendering object
		 */
		virtual const Rendering::ObjectRef& renderingObject() const;

		/**
		 * Sets the name of this node.
		 * @see Node::setName().
		 */
		void setName(const std::string& name) override;

		/**
		 * Sets or changes a specified (standard) field of this node.
		 * @param fieldName Name of the field to set
		 * @param field Field to replace the current field
		 * @return True, if succeeded
		 */
		virtual bool setField(const std::string& fieldName, const Field& field);

		/**
		 * Sets or changes a specified standard or dynamic field of this node.
		 * @param fieldName Name of the field to set
		 * @param field Field to replace the current field
		 * @return True, if succeeded
		 */
		virtual bool setAnyField(const std::string& fieldName, const Field& field);

		/**
		 * Returns all parent nodes of this node.
		 * @return Vector holding all father nodes of this child node
		 */
		NodeRefs parentNodes() const;

		/**
		 * Returns all ancestor nodes of this node.
		 * @return Set holding all possible ancestor nodes towards the root node.
		 */
		SDXNodeSet ancestorNodes() const;

		/**
		 * Event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @param scene Rendering scene node
		 * @param timestamp Initialization timestamp
		 * @param reinitialize True, if a reinitialization has to be done
		 */
		virtual void initialize(const Rendering::SceneRef& scene, const Timestamp timestamp, const bool reinitialize = false);

	protected:

		/**
		 * Creates a new node object.
		 * @param environment Node environment
		 */
		explicit SDXNode(const SDXEnvironment* environment);

		/**
		 * Destructs a node object.
		 */
		~SDXNode() override;

		/**
		 * Internal event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @param scene Rendering scene node
		 * @param timestamp Initialization timestamp
		 */
		virtual void onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp);

		/**
		 * Event function to inform the node about a changed field.
		 * @param fieldName Name of the changed field
		 */
		virtual void onFieldChanged(const std::string& fieldName);

		/**
		 * Registers a new parent node for this (child) node.
		 * @param parentId Id of the parent node to register
		 */
		inline void registerParentNode(const NodeId parentId);

		/**
		 * Unregisters a parent node for this (child) node.
		 * @param parentId Id of the parent node to unregister
		 */
		inline void unregisterParentNode(const NodeId parentId);

		/**
		 * Registers this node at a child as parent node.
		 * @param child New child node for this node
		 */
		void registerThisNodeAsParent(const SDXNodeRef& child);

		/**
		 * Unregisters this node from a child as parent.
		 * @param child Child node to unregister
		 */
		void unregisterThisNodeAsParent(const SDXNodeRef& child);

	protected:

		/// Object specifying the environment of this node.
		const SDXEnvironment* environment_ = nullptr;

		/// Map holding all parent nodes.
		NodeIdMap parents_;

		/// Corresponding rendering object.
		Rendering::ObjectRef renderingObject_;

		/// State determining whether the node has been initialized already.
		bool initialized_ = false;

		/// Node lock.
		mutable Lock lock_;
};

inline SDXNode::SDXEnvironment::SDXEnvironment(const Rendering::EngineRef& engine, const Library* library, const std::string& filename, const SceneId sceneId) :
	engine_(engine),
	library_(library),
	filename_(filename),
	sceneId_(sceneId)
{
	// nothing to do here
}

inline const Rendering::EngineRef& SDXNode::SDXEnvironment::engine() const
{
	return engine_;
}

inline const Library* SDXNode::SDXEnvironment::library() const
{
	return library_;
}

inline const std::string& SDXNode::SDXEnvironment::filename() const
{
	return filename_;
}

inline SceneId SDXNode::SDXEnvironment::sceneId() const
{
	return sceneId_;
}

inline const Rendering::EngineRef& SDXNode::engine() const
{
	ocean_assert(environment_);
	return environment_->engine();
}

inline const std::string& SDXNode::library() const
{
	ocean_assert(environment_);
	ocean_assert(environment_->library());

	return environment_->library()->name();
}

inline const std::string& SDXNode::filename() const
{
	ocean_assert(environment_);
	return environment_->filename();
}

inline SceneId SDXNode::sceneId() const
{
	ocean_assert(environment_);
	return environment_->sceneId();
}

inline void SDXNode::registerParentNode(const NodeId parentId)
{
	const ScopedLock scopedLock(lock_);

	++parents_.insert(std::make_pair(parentId, 0)).first->second;
}

inline void SDXNode::unregisterParentNode(const NodeId parentId)
{
	const ScopedLock scopedLock(lock_);

	NodeIdMap::iterator iNode = parents_.find(parentId);
	ocean_assert(iNode != parents_.end());

	ocean_assert(iNode->second != 0u);

	if (--iNode->second == 0u)
	{
		parents_.erase(iNode);
	}
}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_NODE_H
