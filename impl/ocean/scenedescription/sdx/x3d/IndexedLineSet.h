/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_INDEXED_LINE_SET_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_INDEXED_LINE_SET_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DGeometryNode.h"

#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d indexed line set node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT IndexedLineSet : virtual public X3DGeometryNode
{
	public:

		/**
		 * Creates a new x3d indexed face set node.
		 * @param environment Node environment
		 */
		IndexedLineSet(const SDXEnvironment* environment);

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
		 * Explicit changing event function for node fields.
		 * @see X3DNode::onFieldChanging().
		 */
		bool onFieldChanging(const std::string& fieldName, const Field& field) override;

		/**
		 * Applies all recent changes to the rendering engine object.
		 */
		void apply();

		/**
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// Set_colorIndex field.
		MultiInt setColorIndex_;

		/// Set_coordIndex field.
		MultiInt setCoordIndex_;

		/// Attrib field.
		MultiNode attrib_;

		/// Color field.
		SingleNode color_;

		/// Coord field.
		SingleNode coord_;

		/// FogCoord field.
		SingleNode fogCoord_;

		/// ColorIndex field.
		MultiInt colorIndex_;

		/// ColorPerVertex field.
		SingleBool colorPerVertex_;

		/// CoordIndex field.
		MultiInt coordIndex_;

		/// Rendering vertex set
		Rendering::VertexSetRef vertexSet_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_INDEXED_LINE_SET_H
