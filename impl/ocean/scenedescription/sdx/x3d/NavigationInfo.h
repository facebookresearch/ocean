/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_NAVIGATION_INFO_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_NAVIGATION_INFO_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DBindableNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d NavigationInfo node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT NavigationInfo : virtual public X3DBindableNode
{
	public:

		/**
		 * Creates a new x3d NavigationInfo node.
		 * @param environment Node environment
		 */
		explicit NavigationInfo(const SDXEnvironment* environment);

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

	protected:

		/// AvatarSize field.
		MultiFloat avatarSize_;

		/// Headlight field.
		SingleBool headlight_;

		/// Speed field.
		SingleFloat speed_;

		/// TransitionTime field.
		SingleTime transitionTime_;

		/// TransitionType field.
		MultiString transitionType_;

		/// Type field.
		MultiString type_;

		/// VisibilityLimit field
		SingleFloat visibilityLimit_;

		/// TransitionComplete field.
		SingleBool transitionComplete_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_NAVIGATION_INFO_H
