/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_SCENE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_SCENE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DGroupingNode.h"

#include "ocean/scenedescription/SDXScene.h"

#include "ocean/rendering/Scene.h"

#include <map>

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a scene object holding an entire x3d scene.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DScene :
	virtual public X3DGroupingNode,
	virtual public SDXScene
{
	public:

		/**
		 * Creates a new x3d scene object.
		 * @param filename Name of the scene file, if any
		 * @param library Library providing this scene node
		 * @param engine Rendering engine corresponding with this scene and all child nodes
		 */
		explicit X3DScene(const std::string& filename, const Library& library, const Rendering::EngineRef& engine);

		/**
		 * Returns the associated rendering object.
		 * @see SDXNode::renderingObject().
		 */
		const Rendering::ObjectRef& renderingObject() const override;

	protected:

		/**
		 * Internal event function to inform the scene that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @see SDXScene::onInitialize().
		 */
		void onInitialize(const Timestamp timestamp) override;

		/**
		 * Internal event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * The scene node has an own onInitialize() function, use this instead
		 * @see SDXNode::onInitialize().
		 */
		void onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp) override;

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	private:

		/// Additional rendering object to handle a scene and a group object in common.
		Rendering::SceneRef renderingSceneObject_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_SCENE_H
