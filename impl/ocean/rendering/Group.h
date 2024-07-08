/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GROUP_H
#define META_OCEAN_RENDERING_GROUP_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/LightSource.h"
#include "ocean/rendering/Node.h"
#include "ocean/rendering/ObjectRef.h"

#include <set>
#include <vector>

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Group;

/**
 * Definition of a smart object reference holding a group.
 * @see SmartObjectRef, Group.
 * @ingroup rendering
 */
typedef SmartObjectRef<Group> GroupRef;

/**
 * This is the base class for all rendering groups.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Group : virtual public Node
{
	protected:

		/**
		 * Definition of a vector holding node objects.
		 */
		typedef std::vector<NodeRef> Nodes;

		/**
		 * Definition of a set holding light objects.
		 */
		typedef std::set<LightSourceRef> LightSet;

	public:

		/**
		 * Adds a new child node to this group.
		 * @param node New child node to add
		 */
		virtual void addChild(const NodeRef& node);

		/**
		 * Registers a light lighting all childs of this group node.
		 * @param light Light to register
		 */
		virtual void registerLight(const LightSourceRef& light);

		/**
		 * Returns the number of child nodes connected to this group.
		 * @return Number of child nodes
		 */
		virtual unsigned int numberChildren() const;

		/**
		 * Returns a specific child node defined by it's index.
		 * @param index Index of the child node inside this group
		 * @return Child node
		 */
		virtual NodeRef child(const unsigned int index) const;

		/**
		 * Removes a child node connected to this group.
		 * @param node Child node to remove
		 */
		virtual void removeChild(const NodeRef& node);

		/**
		 * Unregisters a light.
		 * @param light Light to unregister
		 */
		virtual void unregisterLight(const LightSourceRef& light);

		/**
		 * Removes all child nodes connected to this group.
		 */
		virtual void clear();

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new group object.
		 */
		Group();

		/**
		 * Destructs a group object.
		 */
		~Group() override;

	protected:

		/// Child nodes connected to this group
		Nodes groupNodes;

		/// Set holding all lights lighting object in this group
		LightSet groupLights;
};

}

}

#endif // META_OCEAN_RENDERING_GROUP_H
