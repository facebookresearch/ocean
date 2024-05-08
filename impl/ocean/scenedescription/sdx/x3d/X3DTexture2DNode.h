/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_TEXTURE_2D_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_TEXTURE_2D_NODE_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DTextureNode.h"

#include "ocean/io/FileResolver.h"

#include "ocean/media/FrameMedium.h"

#include <string>
#include <vector>

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements an abstract x3d texture 2D node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DTexture2DNode : virtual public X3DTextureNode
{
	protected:

		/**
		 * Definition of a vector holding strings.
		 */
		typedef std::vector<std::string> StringVector;

	protected:

		/**
		 * Creates a new abstract x3d texture 2D node.
		 * @param environment Node environment
		 */
		explicit X3DTexture2DNode(const SDXEnvironment* environment);

		/**
		 * Destructs a x3d texture 2D node.
		 */
		~X3DTexture2DNode() override;

		/**
		 * Registers the fields of this node.
		 * @param specification Node specification receiving the field informations
		 */
		void registerFields(NodeSpecification& specification);

		/**
		 * Event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @see SDXNode::onInitialize().
		 */
		void onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp) override;

		/**
		 * Applies the current url.
		 * @param resolvedURLs Resolved URLs to be create a medium from
		 * @param mediumType Type of the framed medium to be created
		 * @param start Determines whether the framed medium will be started directly
		 */
		void applyUrl(const StringVector& resolvedURLs, const Media::FrameMedium::Type mediumType, const bool start = true);

		/**
		 * Event function to modify properties of a new medium before it will be started and used as texture.
		 * @param medium Medium object which can be modified
		 */
		virtual void onMediumChanged(const Media::MediumRef& medium);

	protected:

		/// RepeatS field.
		SingleBool repeatS_;

		/// RepeatT field.
		SingleBool repeatT_;

		/// TextureProperties field.
		SingleNode textureProperties_;

		/// Texture medium.
		Media::FrameMediumRef textureMedium_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_TEXTURE_2D_NODE_H
