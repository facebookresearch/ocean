/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_COMPOSED_GEOMETRY_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_COMPOSED_GEOMETRY_NODE_H

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
 * This class implements an abstract x3d composed geometry node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DComposedGeometryNode : virtual public X3DGeometryNode
{
	protected:

		/**
		 * Creates an abstract x3d composed geometry node.
		 * @param environment Node environment
		 */
		explicit X3DComposedGeometryNode(const SDXEnvironment* environment);

		/**
		 * Destructs a x3d composed geometry node.
		 */
		~X3DComposedGeometryNode() override;

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

	protected:

		/// Attrib field.
		MultiNode attrib_;

		/// Color field.
		SingleNode color_;

		/// Coord field.
		SingleNode coord_;

		/// FogCoord field.
		SingleNode fogCoord_;

		/// Normal field.
		SingleNode normal_;

		/// TexCoord field.
		SingleNode texCoord_;

		/// CCW field.
		SingleBool ccw_;

		/// ColorPerVertex field.
		SingleBool colorPerVertex_;

		/// NormalPerVertex field.
		SingleBool normalPerVertex_;

		/// Solid field.
		SingleBool solid_;

		/// Rendering vertex set
		Rendering::VertexSetRef renderingVertexSet_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_COMPOSED_GEOMETRY_NODE_H
