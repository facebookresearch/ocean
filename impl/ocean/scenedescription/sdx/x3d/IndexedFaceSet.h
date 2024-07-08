/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_INDEXED_FACE_SET_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_INDEXED_FACE_SET_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"
#include "ocean/scenedescription/sdx/x3d/X3DComposedGeometryNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements a x3d indexed face set node.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT IndexedFaceSet : virtual public X3DComposedGeometryNode
{
	public:

		/**
		 * Creates a new x3d indexed face set node.
		 * @param environment Node environment
		 */
		IndexedFaceSet(const SDXEnvironment* environment);

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
		 * Adds new normals per vertex to the rendering mesh.
		 * @param renderingNormals Rendering normals receiving the new generated normals
		 * @param normals Scene description normals providing the defined normals
		 * @param indices The indices used to arrange the scene description normals
		 * @param firstIndex Index of the first element of the specified indices
		 * @param endIndex Index of the end element (exclusively) of the specified indices
		 */
		void addNormalsPerVertex(Rendering::Normals& renderingNormals, const MultiVector3::Values& normals, const MultiInt::Values& indices, const unsigned int firstIndex, const unsigned int endIndex) const;

		/**
		 * Adds new color per vertex to the rendering mesh.
		 * @param renderingColors Rendering colors receiving the new generated color values
		 * @param colors Scene description colors providing the defined colors
		 * @param indices The indices used to arrange the scene description colors
		 * @param firstIndex Index of the first element of the specified indices
		 * @param endIndex Index of the end element (exclusively) of the specified indices
		 * @return True, if succeeded
		 */
		bool addColorsPerVertex(RGBAColors& renderingColors, const MultiColor::Values& colors, const MultiInt::Values& indices, const unsigned int firstIndex, const unsigned int endIndex);

		/**
		 * Adds new texture coordinate per vertex to the rendering mesh.
		 * @param renderingTextureCoordinates Rendering texture coordinates receiving the new generated texture coordinates
		 * @param textureCoordinates Scene description texture coordinates providing the defined texture coordinates
		 * @param indices The indices used to arrange the scene description texture coordinates
		 * @param firstIndex Index of the first element of the specified indices
		 * @param endIndex Index of the end element (exclusively) of the specified indices
		 * @return True, if succeeded
		 */
		bool addTextureCoordinatesPerVertex(Rendering::TextureCoordinates& renderingTextureCoordinates, const MultiVector2::Values& textureCoordinates, const MultiInt::Values& indices, const unsigned int firstIndex, const unsigned int endIndex);

		/**
		 * Adds new Ocean specific phantom texture coordinate per vertex to the rendering mesh.
		 * @param renderingPhantomTextureCoordinates Rendering texture coordinates receiving the new generated texture coordinates
		 * @param phantomTextureCoordinates Scene description texture coordinates providing the defined texture coordinates
		 * @param indices The indices used to arrange the scene description texture coordinates
		 * @param firstIndex Index of the first element of the specified indices
		 * @param endIndex Index of the end element (exclusively) of the specified indices
		 * @return True, if succeeded
		 */
		bool addPhantomTextureCoordinatesPerVertex(Rendering::Vertices& renderingPhantomTextureCoordinates, const MultiVector3::Values& phantomTextureCoordinates, const MultiInt::Values& indices, const unsigned int firstIndex, const unsigned int endIndex);

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

		/// Set_normalIndex field.
		MultiInt setNormalIndex_;

		/// Set_texCoordIndex.
		MultiInt setTexCoordIndex_;

		/// ColorIndex field.
		MultiInt colorIndex_;

		/// Convex field.
		SingleBool convex_;

		/// CoordIndex field.
		MultiInt coordIndex_;

		/// CreaseAngle field.
		SingleFloat creaseAngle_;

		/// NormalIndex field.
		MultiInt normalIndex_;

		/// TexCoordIndex field.
		MultiInt texCoordIndex_;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_INDEXED_FACE_SET_H
