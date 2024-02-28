// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/avatars/Primitives.h"

#include "ocean/math/SquareMatrix4.h"
#include "ocean/math/Vector3.h"
#include "ocean/math/Vector4.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/Textures.h"

#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

#include <ovrAvatar2/Render.h>

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Avatars
{

bool Primitives::loadPrimitives(const Rendering::EngineRef& engine, const ovrAvatar2ResourceId resourceId, const Images& images)
{
	uint32_t primitiveCount = 0u;
	ovrAvatar2Asset_GetPrimitiveCount(resourceId, &primitiveCount);

	for (uint32_t nPrimitive = 0u; nPrimitive < primitiveCount; ++nPrimitive)
	{
		ovrAvatar2Primitive primitive;
		if (ovrAvatar2Asset_GetPrimitiveByIndex(resourceId, nPrimitive, &primitive) != ovrAvatar2Result_Success)
		{
			continue;
		}

		uint32_t primitiveViewFlags = 0u;
		if (ovrAvatar2Asset_GetViewFlags(primitive.id, &primitiveViewFlags) != ovrAvatar2Result_Success) // e.g., ovrAvatar2EntityViewFlag_FirstPerson, ovrAvatar2EntityViewFlag_ThirdPerson
		{
			continue;
		}

		uint32_t primitiveLODFlags = 0u;
		if (ovrAvatar2Asset_GetLodFlags(primitive.id, &primitiveLODFlags) != ovrAvatar2Result_Success)
		{
			continue;
		}

		uint32_t primitiveManifestationFlags = 0u;
		if (ovrAvatar2Asset_GetManifestationFlags(primitive.id, &primitiveManifestationFlags) != ovrAvatar2Result_Success)
		{
			continue;
		}

		PrimitiveMap::const_iterator iPrimitive = primitiveMap_.find(primitive.id);
		if (iPrimitive != primitiveMap_.cend())
		{
			Log::error() << "Primitive with id " << primitive.id << " exist already";
			continue;
		}

#ifdef OCEAN_DEBUG

		char name[129] = {'\0'};
		if (ovrAvatar2Asset_GetPrimitiveName(primitive.id, name, 128) == ovrAvatar2Result_Success)
		{
			Log::debug() << "Primitive name: " << name;
		}

		Log::debug() << "Index count: " << primitive.indexCount;
		Log::debug() << "alphaMode: " << primitive.alphaMode;
		Log::debug() << "textureCount: " << primitive.textureCount;
		Log::debug() << "jointCount: " << primitive.jointCount;
		Log::debug() << "skeleton: "  << primitive.skeleton;

#endif // OCEAN_DEBUG

		Log::info() << "primitive.vertexBufferId: "  << primitive.vertexBufferId;

		Rendering::TexturesRef textures = engine->factory().createTextures();

		for (unsigned int nTexture = 0u; nTexture < primitive.textureCount; ++nTexture)
		{
			ovrAvatar2MaterialTexture materialTexture;
			if (ovrAvatar2Primitive_GetMaterialTextureByIndex(primitive.id, nTexture, &materialTexture) == ovrAvatar2Result_Success)
			{
				Log::debug() << "Avatar texture type " << int(materialTexture.type) << ", " << materialTexture.factor.x << ", " << materialTexture.factor.y << ", " << materialTexture.factor.z << ", " << materialTexture.factor.w << " - " << materialTexture.imageId;

				const Rendering::FrameTexture2DRef texture = images.texture(materialTexture.imageId);

				if (texture)
				{
					switch (materialTexture.type)
					{
						case ovrAvatar2MaterialTextureType_BaseColor:
							texture->setTextureName("baseTexture");
							break;

						case ovrAvatar2MaterialTextureType_MetallicRoughness:
							texture->setTextureName("metallicRougnessTexture");
							break;

						case ovrAvatar2MaterialTextureType_Occlusion:
							texture->setTextureName("occlusionTexture");
							break;

						default:
							Log::warning() << "Invalid texture texture type: " << int(materialTexture.type);
							break;
					}

					textures->addTexture(texture);
				}
				else
				{
					Log::error() << "Texture does not exist for image";
				}
			}
			else
			{
				Log::error() << "Failed to get material texture";
			}
		}

		Rendering::FrameTexture2DRef skinMatricesTexture = engine->factory().createFrameTexture2D();
		skinMatricesTexture->setTextureName("skinMatricesTexture");
		textures->addTexture(skinMatricesTexture);

		Rendering::VertexSetRef vertexSet;

		VertexBufferMap::const_iterator iVertexBuffer = vertexBufferMap_.find(primitive.vertexBufferId);
		if (iVertexBuffer == vertexBufferMap_.cend())
		{
			VectorsF3 vertices;
			VectorsF3 normals;
			VectorsT4<uint16_t> jointIndices;
			VectorsF4 jointWeights;

			if (!loadVertexBuffer(engine, primitive.vertexBufferId, vertexSet, vertices, normals, jointIndices, jointWeights))
			{
				continue;
			}

			iVertexBuffer = vertexBufferMap_.emplace(primitive.vertexBufferId, VertexBufferObject(vertexSet, std::move(vertices), std::move(normals), std::move(jointIndices), std::move(jointWeights))).first;
		}
		else
		{
			vertexSet = iVertexBuffer->second.vertexSet_;
		}

		ocean_assert(iVertexBuffer != vertexBufferMap_.cend());
		ocean_assert(vertexSet);

		std::vector<uint16_t> triangleVertexIndices(primitive.indexCount);
		if (ovrAvatar2Primitive_GetIndexData(primitive.id, triangleVertexIndices.data(), triangleVertexIndices.size() * sizeof(uint16_t)) != ovrAvatar2Result_Success)
		{
			Log::error() << "Failed to get index data";
			continue;
		}

		if (triangleVertexIndices.size() % 3u != 0u)
		{
			Log::error() << "Invalid index buffer";
			continue;
		}

		Indices32 jointIndices;
		HomogenousMatricesF4 inverseBinds;

		std::vector<ovrAvatar2JointInfo> jointInfos;
		if (ovrAvatar2Primitive_GetJointInfo(primitive.id, nullptr, 0) != ovrAvatar2Result_DataNotAvailable)
		{
			jointInfos.resize(primitive.jointCount);
			if (ovrAvatar2Primitive_GetJointInfo(primitive.id, jointInfos.data(), jointInfos.size() * sizeof(ovrAvatar2JointInfo)) != ovrAvatar2Result_Success)
			{
				Log::error() << "Failed to get joint infos";
				continue;
			}

			jointIndices.reserve(jointInfos.size());
			inverseBinds.reserve(jointInfos.size());

			for (const ovrAvatar2JointInfo& jointInfo : jointInfos)
			{
				jointIndices.emplace_back(jointInfo.jointIndex);
				inverseBinds.emplace_back(jointInfo.inverseBind.m, false /*valuesRowAligned*/);
				ocean_assert(inverseBinds.back().isValid());
			}
		}

		const Rendering::TrianglesRef triangles = engine->factory().createTriangles();
		triangles->setVertexSet(vertexSet);

		Rendering::TriangleFaces triangleFaces;
		triangleFaces.reserve(triangleVertexIndices.size() / 3);

		for (size_t nVertexIndex = 0; nVertexIndex < triangleVertexIndices.size(); nVertexIndex += 3)
		{
			const uint16_t& vertexIndex0 = triangleVertexIndices[nVertexIndex + 0];
			const uint16_t& vertexIndex1 = triangleVertexIndices[nVertexIndex + 1];
			const uint16_t& vertexIndex2 = triangleVertexIndices[nVertexIndex + 2];

			ocean_assert(vertexIndex0 >= primitive.minIndexValue && vertexIndex0 <= primitive.maxIndexValue);
			ocean_assert(vertexIndex1 >= primitive.minIndexValue && vertexIndex1 <= primitive.maxIndexValue);
			ocean_assert(vertexIndex2 >= primitive.minIndexValue && vertexIndex2 <= primitive.maxIndexValue);

			triangleFaces.emplace_back(Index32(vertexIndex0), Index32(vertexIndex1), Index32(vertexIndex2));
		}

		triangles->setFaces(triangleFaces);

		const VectorsT4<uint16_t>& vertxBufferJointIndices = iVertexBuffer->second.jointIndices_;
		const VectorsF4& vertexBufferJointWeights = iVertexBuffer->second.jointWeights_;

		triangles->vertexSet().force<Rendering::GLESceneGraph::GLESVertexSet>().setAttribute<VectorT4<uint16_t>>("aJointIndices", vertxBufferJointIndices.data(), vertxBufferJointIndices.size());
		triangles->vertexSet().force<Rendering::GLESceneGraph::GLESVertexSet>().setAttribute<VectorF4>("aJointWeights", vertexBufferJointWeights.data(), vertexBufferJointWeights.size());

		primitiveMap_.emplace(primitive.id, PrimitiveObject(primitive.vertexBufferId, std::move(triangles), std::move(textures), std::move(skinMatricesTexture), std::move(triangleVertexIndices), std::move(jointIndices), std::move(inverseBinds), primitive.jointCount, ovrAvatar2EntityViewFlags(primitiveViewFlags), ovrAvatar2EntityLODFlags(primitiveLODFlags), ovrAvatar2EntityManifestationFlags(primitiveManifestationFlags)));
	}

	return true;
}

bool Primitives::unloadPrimitives(const ovrAvatar2ResourceId resourceId)
{
	uint32_t primitiveCount = 0u;
	ovrAvatar2Asset_GetPrimitiveCount(resourceId, &primitiveCount);

	for (uint32_t nPrimitive = 0u; nPrimitive < primitiveCount; ++nPrimitive)
	{
		ovrAvatar2Primitive primitive;
		if (ovrAvatar2Asset_GetPrimitiveByIndex(resourceId, nPrimitive, &primitive) != ovrAvatar2Result_Success)
		{
			continue;
		}
	}

	return true;
}

void Primitives::release()
{
	primitiveMap_.clear();
	vertexBufferMap_.clear();
}

bool Primitives::loadVertexBuffer(const Rendering::EngineRef& engine, const ovrAvatar2VertexBufferId vertexBufferId, Rendering::VertexSetRef& vertexSet, VectorsF3& vertices, VectorsF3& normals, VectorsT4<uint16_t>& jointIndices, VectorsF4& jointWeights)
{
	ocean_assert(engine);

	uint32_t vertexCount = 0u;
	if (ovrAvatar2VertexBuffer_GetVertexCount(vertexBufferId, &vertexCount) != ovrAvatar2Result_Success)
	{
		Log::error() << "Failed to get vertex count";
		return false;
	}

	vertices.clear();
	if (ovrAvatar2VertexBuffer_GetPositions(vertexBufferId, nullptr, 0, 0) != ovrAvatar2Result_DataNotAvailable)
	{
		vertices.resize(vertexCount);
		if (ovrAvatar2VertexBuffer_GetPositions(vertexBufferId, (ovrAvatar2Vector3f*)(vertices.data()), vertices.size() * sizeof(VectorF3), 0) != ovrAvatar2Result_Success)
		{
			Log::error() << "Failed to get positions";
			return false;
		}
	}

	normals.clear();
	if (ovrAvatar2VertexBuffer_GetNormals(vertexBufferId, nullptr, 0, 0) != ovrAvatar2Result_DataNotAvailable)
	{
		normals.resize(vertexCount);
		if (ovrAvatar2VertexBuffer_GetNormals(vertexBufferId, (ovrAvatar2Vector3f*)(normals.data()), normals.size() * sizeof(VectorF3), 0) != ovrAvatar2Result_Success)
		{
			Log::error() << "Failed to get normals";
			return false;
		}
	}

	VectorsF2 textureCoordinates;
	if (ovrAvatar2VertexBuffer_GetTexCoord0(vertexBufferId, nullptr, 0, 0) != ovrAvatar2Result_DataNotAvailable)
	{
		textureCoordinates.resize(vertexCount);
		if (ovrAvatar2VertexBuffer_GetTexCoord0(vertexBufferId, (ovrAvatar2Vector2f*)(textureCoordinates.data()), textureCoordinates.size() * sizeof(VectorF2), 0) != ovrAvatar2Result_Success)
		{
			Log::error() << "Failed to get texture coordinates";
			return false;
		}
	}

	jointIndices.clear();
	if (ovrAvatar2VertexBuffer_GetJointIndices(vertexBufferId, nullptr, 0, 0) != ovrAvatar2Result_DataNotAvailable)
	{
		jointIndices.resize(vertexCount);
		if (ovrAvatar2VertexBuffer_GetJointIndices(vertexBufferId, (ovrAvatar2Vector4us*)(jointIndices.data()), jointIndices.size() * sizeof(VectorT4<uint16_t>), 0) != ovrAvatar2Result_Success)
		{
			Log::error() << "Failed to get joint indices";
			return false;
		}
	}

	jointWeights.clear();
	if (ovrAvatar2VertexBuffer_GetJointWeights(vertexBufferId, nullptr, 0, 0) != ovrAvatar2Result_DataNotAvailable)
	{
		jointWeights.resize(vertexCount);
		if (ovrAvatar2VertexBuffer_GetJointWeights(vertexBufferId, (ovrAvatar2Vector4f*)(jointWeights.data()), jointWeights.size() * sizeof(VectorF4), 0) != ovrAvatar2Result_Success)
		{
			Log::error() << "Failed to get joint weights";
			return false;
		}
	}

	vertexSet = engine->factory().createVertexSet();

	if (!vertices.empty())
	{
		vertexSet->setVertices(Vector3::vectors2vectors(std::move(vertices)));
	}

	if (!normals.empty())
	{
		vertexSet->setNormals(Vector3::vectors2vectors(std::move(normals)));
	}

	if (!textureCoordinates.empty())
	{
		vertexSet->setTextureCoordinates(Vector2::vectors2vectors(std::move(textureCoordinates)), 0u);
	}

	return true;
}

}

}

}

}
