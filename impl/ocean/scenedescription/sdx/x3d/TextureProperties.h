/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TEXTURE_PROPERTIES_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TEXTURE_PROPERTIES_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DNode.h"

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
 * This class implements a x3d texture properties node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT TextureProperties : virtual public X3DNode
{
	public:

		/**
		 * Creates a new x3d texture properties node.
		 * @param environment Node environment
		 */
		explicit TextureProperties(const SDXEnvironment* environment);

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
		 * Event function to inform the node about a changed field.
		 * @see SDXNode::onFieldChanged().
		 */
		void onFieldChanged(const std::string& fieldName) override;

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

		/**
		 * Translates a string holding the minification filter parameter into a rendering filter value.
		 * @param parameter Minification filter parameter to translate
		 * @return Rendering filter value
		 */
		static Rendering::Texture::MinFilterMode translateMinification(const std::string parameter);

		/**
		 * Translates a string holding the magnification filter parameter into a rendering filter value.
		 * @param parameter Magnification filter parameter to translate
		 * @return Rendering filter value
		 */
		static Rendering::Texture::MagFilterMode translateMagnification(const std::string parameter);

	protected:

		/// AnisotropicDegree field.
		SingleFloat anisotropicDegree_;

		/// BorderColor field.
		SingleColor borderColor_;

		/// BorderWidth field.
		SingleInt borderWidth_;

		/// BoundaryModeS field.
		SingleString boundaryModeS_;

		/// BoundaryModeT field.
		SingleString boundaryModeT_;

		/// BoundaryModeR field.
		SingleString boundaryModeR_;

		/// MagnificsationFilter field.
		SingleString magnificationFilter_;

		/// MinificationFilter field.
		SingleString minificationFilter_;

		/// TextureCompression field.
		SingleString textureCompression_;

		/// TexturePriority field.
		SingleFloat texturePriority_;

		/// GenerateMipMaps field.
		SingleBool generateMipMaps_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TEXTURE_PROPERTIES_H
