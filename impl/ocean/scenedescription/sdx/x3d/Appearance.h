/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_APPEARANCE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_APPEARANCE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DAppearanceNode.h"

#include "ocean/rendering/Textures.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an x3d apperance node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT Appearance : virtual public X3DAppearanceNode
{
	public:

		/**
		 * Creates a new appearance node.
		 * @param environment Node environment
		 */
		explicit Appearance(const SDXEnvironment* environment);

		/**
		 * Destructs an appearance node.
		 */
		~Appearance() override;

		/**
		 * (Re-)Checks whether the connected rendering object must contain a blend attribute.
		 * Further, the blend attribute is added or removed if necessary.
		 * @return True, if a blend attribute is necessary
		 */
		bool checkTransparencyState();

	protected:

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
		 * Explicit changing event function for node fields.
		 * @see X3DNode::onFieldChanging().
		 */
		bool onFieldChanging(const std::string& fieldName, const Field& field) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// FillProperties field.
		SingleNode fillProperties_;

		/// LineProperties field.
		SingleNode lineProperties_;

		/// Material field.
		SingleNode material_;

		/// Shaders field.
		MultiNode shaders_;

		/// Texture field.
		SingleNode texture_;

		/// TextureTransform.
		SingleNode textureTransform_;

		/// IsPhantom field.
		SingleBool isPhantom_;

		/// Explicit rendering textures object.
		Rendering::TexturesRef explicitRenderingTextures_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_APPEARANCE_H
