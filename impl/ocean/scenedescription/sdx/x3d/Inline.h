/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_INLINE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_INLINE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DChildNode.h"
#include "ocean/scenedescription/sdx/x3d/X3DBoundedObject.h"
#include "ocean/scenedescription/sdx/x3d/X3DUrlObject.h"

#include "ocean/scenedescription/SDXScene.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d inline node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT Inline :
	virtual public X3DChildNode,
	virtual public X3DBoundedObject,
	virtual public X3DUrlObject
{
	public:

		/**
		 * Creates a new x3d inline node.
		 * @param environment Node environment
		 */
		Inline(const SDXEnvironment* environment);

		/**
		 * Returns a reference of the child scene.
		 * @return Loaded child scene
		 */
		inline SDXSceneRef childScene() const;

	protected:

		/**
		 * Destructs a x3d inline node.
		 */
		~Inline() override;

		/**
		 * Specifies the node type and the fields of this node.
		 * @return Unique node specification of this node
		 */
		NodeSpecification specifyNode();

		/**
		 * Event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @see SDXNode::onInitialize().
		 */
		void onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp) override;

		/**
		 * Event function to inform the node about a changed field.
		 * @see SDXNode::onFieldChanged().
		 */
		void onFieldChanged(const std::string& fieldName) override;

		/**
		 * Loads all scenes specified in the url field.
		 * @param timestamp The timestamp at which the scenes will be loaded, must be valid
		 */
		void loadScenes(const Timestamp& timestamp);

		/**
		 * Unloades all currently loaded scenes.
		 */
		void unloadScenes();

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// Load field.
		SingleBool load_;

		/// State determining whether the inline node has loaded scenes.
		bool hasScenes_;

		/// Scene reference of a possible permanent scene object.
		SDXSceneRef permanentScene_;
};

inline SDXSceneRef Inline::childScene() const
{
	return permanentScene_;
}

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_INLINE_H
