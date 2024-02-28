// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_AVATARS_PRIMITIVES_H
#define META_OCEAN_PLATFORM_META_AVATARS_PRIMITIVES_H

#include "ocean/platform/meta/avatars/Avatars.h"
#include "ocean/platform/meta/avatars/Images.h"
#include "ocean/platform/meta/avatars/Shaders.h"
#include "ocean/platform/meta/avatars/Utilities.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/FrameTexture2D.h"
#include "ocean/rendering/ShaderProgram.h"
#include "ocean/rendering/Textures.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/VertexSet.h"

#include <ovrAvatar2/Asset.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

/**
 * This class handles all primitives.
 * @ingroup platformmetaavatars
 */
class OCEAN_PLATFORM_META_AVATARS_EXPORT Primitives
{
	public:

		/**
		 * This class holds the relevant information of a primitive.
		 */
		class PrimitiveObject
		{
			public:

				/**
				 * Creates a new primitive object.
				 * @param vertexBufferId The id of the vertex buffer to which this primitive is connected, must be valid
				 * @param triangles The triangles of this primitive, must be valid
				 * @param textures The textures of this primitive, must be valid
				 * @param skinMatricesTexture The texture which holds the recent skin matrices of this primitive, must be valid
				 * @param triangleVertexIndices The indices of the vertices of this primitive, can be empty
				 * @param jointIndices The indices of the joints of this primitive, can be empty
				 * @param inverseBinds The inverse bind matrices of this primitive, can be empty
				 * @param jointCount The number of joints of this primitive, with range [0, infinity)
				 * @param viewFlags The view flags of this primitive, must be valid
				 * @param lodFlags The LOD flags of this primitive, must be valid
				 * @param manifestationFlags The manifest flags of this primitive, must be valid
				 */
				inline PrimitiveObject(const ovrAvatar2VertexBufferId vertexBufferId, Rendering::TrianglesRef triangles, Rendering::TexturesRef textures, Rendering::FrameTexture2DRef skinMatricesTexture, std::vector<uint16_t>&& triangleVertexIndices, Indices32&& jointIndices, HomogenousMatricesF4&& inverseBinds, const unsigned int jointCount, const ovrAvatar2EntityViewFlags viewFlags, const ovrAvatar2EntityLODFlags lodFlags, const ovrAvatar2EntityManifestationFlags manifestationFlags);

			public:

				/// The id of the vertex buffer to which this primitive is connected.
				ovrAvatar2VertexBufferId vertexBufferId_ = ovrAvatar2VertexBufferId_Invalid;

				/// The triangles of this primitive.
				Rendering::TrianglesRef triangles_;

				/// The textures of this primitive.
				Rendering::TexturesRef textures_;

				/// The texture which holds the recent skin matrices of this primitive.
				Rendering::FrameTexture2DRef skinMatricesTexture_;

				/// The indices of the vertices of this primitive.
				std::vector<uint16_t> triangleVertexIndices_;

				/// The indices of the joints of this primitive.
				Indices32 jointIndices_;

				/// The inverse bind matrices of this primitive.
				HomogenousMatricesF4 inverseBinds_;

				/// The number of joints of this primitive.
				unsigned int jointCount_ = 0u;

				/// The view flags of this primitive.
				ovrAvatar2EntityViewFlags viewFlags_ = ovrAvatar2EntityViewFlags(0u);

				/// The LOD flags of this primitive.
				ovrAvatar2EntityLODFlags lodFlags_ = ovrAvatar2EntityLODFlags(0u);

				/// The manifest flags of this primitive.
				ovrAvatar2EntityManifestationFlags_ manifestationFlags_ = ovrAvatar2EntityManifestationFlags_(0u);
		};

		/**
		 * This class combines the relevant data for a vertex buffer.
		 */
		class VertexBufferObject
		{
			public:

				/**
				 * Creates a new vertex buffer object.
				 * @param vertexSet The rendering VertexSet object holding associated with the vertex buffer
				 * @param vertices The vertices of the vertex buffer
				 * @param normals The normals of the vertex buffer
				 * @param jointIndices The joint indices of the vertex buffer
				 * @param jointWeights The joint weights of the vertex buffer
				 */
				inline VertexBufferObject(Rendering::VertexSetRef vertexSet, VectorsF3&& vertices, VectorsF3&& normals, VectorsT4<uint16_t>&& jointIndices, VectorsF4&& jointWeights);

			public:

				/// The VertexSet object holding the vertex data.
				Rendering::VertexSetRef vertexSet_;

				/// The vertices of the vertex buffer.
				VectorsF3 vertices_;

				/// The normals of the vertex buffer.
				VectorsF3 normals_;

				/// The joint indices of the vertex buffer.
				VectorsT4<uint16_t> jointIndices_;

				/// The joint weights of the vertex buffer.
				VectorsF4 jointWeights_;
		};

		/**
		 * Definition of an unordered map mapping primitive ids to primitive objects.
		 */
		typedef std::unordered_map<ovrAvatar2PrimitiveId, PrimitiveObject, Utilities> PrimitiveMap;

		/**
		 * Definition of an unordered map mapping vertex buffer ids to vertex buffer objects.
		 */
		typedef std::unordered_map<ovrAvatar2VertexBufferId, VertexBufferObject, Utilities> VertexBufferMap;

	public:

		/**
		 * Loads all primitives of a given resource.
		 * @param engine The rendering engine to be used
		 * @param resourceId The id of the resource from which all primitives will be loaded
		 * @param images The images of all avatars
		 * @return True, if succeeded
		 */
		bool loadPrimitives(const Rendering::EngineRef& engine, const ovrAvatar2ResourceId resourceId, const Images& images);

		/**
		 * Unloads all primitives for a given resource.
		 * @param resourceId The id of the resource for which all primitives will be unloaded
		 * @return True, if succeeded
		 */
		bool unloadPrimitives(const ovrAvatar2ResourceId resourceId);

		/**
		 * Returns the primitive object for a given primitive id.
		 * @param primitiveId The id of the primitive, must be valid
		 * @return The primitive object, nullptr if the object does not exist
		 */
		inline const PrimitiveObject* primitiveObject(const ovrAvatar2PrimitiveId primitiveId) const;

		/**
		 * Returns the vertex buffer object for a given vertex buffer id.
		 * @param vertexBufferId The id of the vertex buffer, must be valid
		 * @return The vertex buffer object, nullptr if the object does not exist
		 */
		inline const VertexBufferObject* vertexBufferObject(const ovrAvatar2VertexBufferId vertexBufferId) const;

		/**
		 * Releases all images.
		 */
		void release();

	protected:

		/**
		 * Loads the vertex buffer of a primitive.
		 * @param engine The rendering engine to be used
		 * @param vertexBufferId The id of the vertex buffer to load, must be valid
		 * @param vertexSet The resulting VertexSet object
		 * @param vertices The resulting vertices of the vertex buffer
		 * @param normals The resulting normals of the vertex buffer
		 * @param jointIndices The resulting joint indices of the vertex buffer
		 * @param jointWeights The resulting joint weights of the vertex buffer
		 * @return True, if succeeded
		 */
		static bool loadVertexBuffer(const Rendering::EngineRef& engine, const ovrAvatar2VertexBufferId vertexBufferId, Rendering::VertexSetRef& vertexSet, VectorsF3& vertices, VectorsF3& normals, VectorsT4<uint16_t>& jointIndices, VectorsF4& jointWeights);

	protected:

		/// The map mapping primitive ids to renderables.
		PrimitiveMap primitiveMap_;

		/// The map mapping vertex buffer ids to vertex sets.
		VertexBufferMap vertexBufferMap_;
};

inline Primitives::PrimitiveObject::PrimitiveObject(const ovrAvatar2VertexBufferId vertexBufferId, Rendering::TrianglesRef triangles, Rendering::TexturesRef textures, Rendering::FrameTexture2DRef skinMatricesTexture, std::vector<uint16_t>&& triangleVertexIndices, Indices32&& jointIndices, HomogenousMatricesF4&& inverseBinds, const unsigned int jointCount, const ovrAvatar2EntityViewFlags viewFlags, const ovrAvatar2EntityLODFlags lodFlags, const ovrAvatar2EntityManifestationFlags manifestationFlags) :
	vertexBufferId_(vertexBufferId),
	triangles_(std::move(triangles)),
	textures_(std::move(textures)),
	skinMatricesTexture_(std::move(skinMatricesTexture)),
	triangleVertexIndices_(std::move(triangleVertexIndices)),
	jointIndices_(std::move(jointIndices)),
	inverseBinds_(std::move(inverseBinds)),
	jointCount_(jointCount),
	viewFlags_(viewFlags),
	lodFlags_(lodFlags),
	manifestationFlags_(manifestationFlags)
{
	ocean_assert(vertexBufferId_ != ovrAvatar2VertexBufferId_Invalid);
	ocean_assert(triangles_ && textures_ && skinMatricesTexture_);
	ocean_assert(viewFlags_ != 0u);
	ocean_assert(lodFlags_ != 0u);
	ocean_assert(manifestationFlags_ != 0u);
}

inline Primitives::VertexBufferObject::VertexBufferObject(Rendering::VertexSetRef vertexSet, VectorsF3&& vertices, VectorsF3&& normals, VectorsT4<uint16_t>&& jointIndices, VectorsF4&& jointWeights) :
	vertexSet_(std::move(vertexSet)),
	vertices_(std::move(vertices)),
	normals_(std::move(normals)),
	jointIndices_(std::move(jointIndices)),
	jointWeights_(std::move(jointWeights))
{
	// nothing to do here
}

inline const Primitives::PrimitiveObject* Primitives::primitiveObject(const ovrAvatar2PrimitiveId primitiveId) const
{
	ocean_assert(primitiveId != ovrAvatar2PrimitiveId_Invalid);

	PrimitiveMap::const_iterator iPrimitive = primitiveMap_.find(primitiveId);
	if (iPrimitive == primitiveMap_.cend())
	{
		return nullptr;
	}

	return &iPrimitive->second;
}

inline const Primitives::VertexBufferObject* Primitives::vertexBufferObject(const ovrAvatar2VertexBufferId vertexBufferId) const
{
	ocean_assert(vertexBufferId != ovrAvatar2VertexBufferId_Invalid);

	VertexBufferMap::const_iterator iVertexBuffer = vertexBufferMap_.find(vertexBufferId);
	if (iVertexBuffer == vertexBufferMap_.cend())
	{
		return nullptr;
	}

	return &iVertexBuffer->second;
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_AVATARS_PRIMITIVES_H
