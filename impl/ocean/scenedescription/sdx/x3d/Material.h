/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_MATERIAL_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_MATERIAL_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DMaterialNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d material node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT Material : virtual public X3DMaterialNode
{
	public:

		/**
		 * Creates a new x3d material node.
		 * @param environment Node environment
		 */
		explicit Material(const SDXEnvironment* environment);

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
		 * Returns the address of the most derived object.
		 * @see Node::objectAddress().
		 */
		size_t objectAddress() const override;

	protected:

		/// AmbientIntensity field.
		SingleFloat ambientIntensity_;

		/// DiffuseColor field.
		SingleColor diffuseColor_;

		/// EmissiveColor field.
		SingleColor emissiveColor_;

		/// Shininess field.
		SingleFloat shininess_;

		/// SpecularColor field.
		SingleColor specularColor_;

		/// Transparency field.
		SingleFloat transparency_;

		/// Reflectivity field, which is not part of the X3D specification.
		SingleFloat reflectivity_;

		/// Refraction index field, which is not part of the X3D specification.
		SingleFloat refractionIndex_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_MATERIAL_H
