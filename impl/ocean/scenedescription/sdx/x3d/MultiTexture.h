/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_MULTI_TEXTURE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_MULTI_TEXTURE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DTextureNode.h"

#include "ocean/rendering/Texture.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d multi texture node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT MultiTexture : virtual public X3DTextureNode
{
	public:

		/**
		 * Creates a new x3d multi texture node.
		 * @param environment Node environment
		 */
		MultiTexture(const SDXEnvironment* environment);

	protected:

		/**
		 * Destructs a xed multi texture node.
		 */
		~MultiTexture() override;

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
		 * Translates the environment mode.
		 * @param mode X3D environment mode to translate
		 * @return Resulting rendering environment mode
		 */
		static Rendering::Texture::EnvironmentMode translateEnvironmentMode(const std::string& mode);

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// Alpha field.
		SingleFloat alpha_;

		/// Color field.
		SingleColor color_;

		/// Function field.
		MultiString function_;

		/// Mode field.
		MultiString mode_;

		/// Source field.
		MultiString source_;

		/// Texture field.
		MultiNode texture_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_MULTI_TEXTURE_H
