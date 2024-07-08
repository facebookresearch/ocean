/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_BACKGROUND_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_BACKGROUND_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DBackgroundNode.h"

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
 * This class implements a x3d background node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT Background : virtual public X3DBackgroundNode
{
	public:

		/**
		 * Creates a new x3d background node.
		 * @param environment Node environment
		 */
		explicit Background(const SDXEnvironment* environment);

	protected:

		/**
		 * Destructs a background object.
		 */
		~Background() override;

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
		 * Creates a textures object by given texture urls.
		 * @param urls Texture urls to create a textures object from
		 * @return Resulting textures object
		 */
		Rendering::TexturesRef createTextures(const MultiString::Values& urls) const;

	protected:

		/// BackUrl field.
		MultiString backUrl_;

		/// BottomUrl field.
		MultiString bottomUrl_;

		/// FrontUrl field.
		MultiString frontUrl_;

		/// LeftUrl field.
		MultiString leftUrl_;

		/// RightUrl field.
		MultiString rightUrl_;

		/// TopUrl field.
		MultiString topUrl_;

		/// Transparency field.
		SingleFloat transparency_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_BACKGROUND_H
