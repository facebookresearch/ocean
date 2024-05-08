/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TEXT_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TEXT_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DGeometryNode.h"

#include "ocean/rendering/Text.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d text node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT Text : virtual public X3DGeometryNode
{
	public:

		/**
		 * Creates a x3d cylinder node.
		 * @param environment Node environment
		 */
		explicit Text(const SDXEnvironment* environment);

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
		 * Applies the string field.
		 */
		void applyString(Rendering::Text& text) const;

	protected:

		/// FontStyle field.
		SingleNode fontStyle_;

		/// Lenght field.
		MultiFloat length_;

		/// MaxExtent field.
		SingleFloat maxExtent_;

		/// String field.
		MultiString string_;

		/// LineBounds field.
		MultiVector2 lineBounds_;

		/// Origin field.
		SingleVector3 origin_;

		/// TextBounds field.
		SingleVector2 textBounds_;

		/// Solid filed.
		SingleBool solid_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_TEXT_H
