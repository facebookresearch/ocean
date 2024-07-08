/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_FACTORY_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_FACTORY_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DLibrary.h"

#include "ocean/scenedescription/SDXNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d node factory.
 * The factory is able to create x3d nodes by their given type names.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT Factory
{
	private:

		/**
		 * Definition of a function pointer creating nodes.
		 */
		typedef SDXNodeRef(*CreateNodeFunction)(const SDXNode::SDXEnvironment* environment);

		/**
		 * Definition of an unordered map mapping names of node types to functions creating the nodes.
		 */
		typedef std::unordered_map<std::string, CreateNodeFunction> CreateNodeMap;

	public:

		/**
		 * Creates a new x3d node by the given type name of the node.
		 * @param type The type name of the x3d node to create, must be valid
		 * @param environment The environment of the node to be created, must be valid
		 */
		static SDXNodeRef createNode(const std::string& type, const SDXNode::SDXEnvironment* environment);

	private:

		/**
		 * Creates a new node with a specific type.
		 * @param environment The environment of the node to be created, must be valid
		 * @tparam T The data type of the node
		 */
		template <typename T>
		static SDXNodeRef createNode(const SDXNode::SDXEnvironment* environment);
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_FACTORY_H
