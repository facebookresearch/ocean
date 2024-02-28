// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/metaportation/Metaportation.h"

#include "ocean/io/Compression.h"

#include "ocean/io/image/Image.h"

namespace Ocean
{

namespace XRPlayground
{

using namespace Tracking::MapTexturing;

void Metaportation::MeshesManager::updateMeshes(const HomogenousMatrix4& world_T_meshes, TexturedMeshMap&& texturedMeshMap, Frame&& textureFrame)
{
	ocean_assert(world_T_meshes.isValid());
	ocean_assert(!texturedMeshMap.empty());
	ocean_assert(textureFrame.isValid());

	const ScopedLock scopedLock(lock_);

	world_T_meshes_ = world_T_meshes;

	const Index32 newTextureId = ++textureIdCounter_;

	ocean_assert(textureMap_.find(newTextureId) == textureMap_.cend() && textureUsageMap_.find(newTextureId) == textureUsageMap_.cend());
	textureMap_.emplace(newTextureId, std::move(textureFrame));
	textureUsageMap_.emplace(newTextureId, Index32(texturedMeshMap.size()));

	for (TexturedMeshMap::iterator iTexturedMesh = texturedMeshMap.begin(); iTexturedMesh != texturedMeshMap.end(); ++iTexturedMesh)
	{
		TextureIdMap::iterator iTextureId = textureIdMap_.find(iTexturedMesh->first);

		if (iTextureId != textureIdMap_.cend())
		{
			// the block was assigned to an old texture, we remove the link to the old texture

			const Index32 oldTextureId = iTextureId->second;

			TextureUsageMap::iterator iTextureUsage = textureUsageMap_.find(oldTextureId);
			ocean_assert(iTextureUsage != textureUsageMap_.cend());

			ocean_assert(iTextureUsage->second >= 1u);

			if (--iTextureUsage->second == 0u)
			{
				// the old texture is not used anymore, so we remove the old texture

				textureMap_.erase(oldTextureId);
				textureUsageMap_.erase(iTextureUsage);
			}

			iTextureId->second = newTextureId;
		}
		else
		{
			textureIdMap_.emplace(iTexturedMesh->first, newTextureId);
		}

		texturedMeshMap_[iTexturedMesh->first] = std::move(iTexturedMesh->second);
	}
}

void Metaportation::MeshesManager::latestMeshes(std::vector<Meshes>& meshesGroup) const
{
	const ScopedLock scopedLock(lock_);

	Frames textureFrames;
	textureFrames.reserve(textureMap_.size());

	TextureUsageMap textureIndexMap; // id to index
	textureIndexMap.reserve(textureMap_.size());

	for (TextureMap::const_iterator iTexture = textureMap_.cbegin(); iTexture != textureMap_.cend(); ++iTexture)
	{
		textureIndexMap.emplace(iTexture->first, Index32(textureFrames.size()));

		textureFrames.emplace_back(iTexture->second, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
	}

	std::vector<TexturedMeshMap> texturedMeshMapGroups(textureMap_.size());

	for (TexturedMeshMap::const_iterator iTexturedMesh = texturedMeshMap_.cbegin(); iTexturedMesh != texturedMeshMap_.cend(); ++iTexturedMesh)
	{
		const TextureIdMap::const_iterator iTextureId = textureIdMap_.find(iTexturedMesh->first);
		ocean_assert(iTextureId != textureIdMap_.cend());

		const Index32 textureId = iTextureId->second;

		const TextureUsageMap::const_iterator iIndex = textureIndexMap.find(textureId);
		ocean_assert(iIndex != textureIndexMap.cend());

		const Index32 index = iIndex->second;

		ocean_assert(index < Index32(texturedMeshMapGroups.size()));

		texturedMeshMapGroups[index].emplace(iTexturedMesh->first, iTexturedMesh->second);
	}

	meshesGroup.clear();

	for (size_t n = 0; n < textureFrames.size(); ++n)
	{
		meshesGroup.emplace_back(world_T_meshes_, std::move(texturedMeshMapGroups[n]), std::move(textureFrames[n]));
	}
}

size_t Metaportation::MeshesManager::numberTextures() const
{
	const ScopedLock scopedLock(lock_);

	return textureMap_.size();
}

size_t Metaportation::MeshesManager::numberMeshes() const
{
	const ScopedLock scopedLock(lock_);

	return texturedMeshMap_.size();
}

Metaportation::ScopedUncompressedMemory::ScopedUncompressedMemory(const void* data, const size_t size)
{
	ocean_assert(data != nullptr & size >= 1);

	if (size >= 3)
	{
		if (((const uint8_t*)(data))[0] == 0x1f && ((const uint8_t*)(data))[1] == 0x8b)
		{
			if (IO::Compression::gzipDecompress(data, size, uncompressedMemoryBuffer_))
			{
				data_ = uncompressedMemoryBuffer_.data();
				size_ = uncompressedMemoryBuffer_.size();

				return;
			}
		}
	}

	data_ = data;
	size_ = size;
}

bool Metaportation::writeTransformationToStream(const HomogenousMatrix4& world_T_camera, IO::OutputBitstream& bitstream)
{
	if (!bitstream.write<unsigned long long>(transformationTag_))
	{
		return false;
	}

	const HomogenousMatrixD4 worldD_T_cameraD(world_T_camera);

	constexpr unsigned long long version = 1ull;

	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	if (!bitstream.write(worldD_T_cameraD.data(), sizeof(double) * 16))
	{
		return false;
	}

	return true;
}

bool Metaportation::writeMeshesToStream(const Meshes& meshes, IO::OutputBitstream& bitstream)
{
	if (meshes.texturedMeshMap_.empty())
	{
		return false;
	}

	if (!bitstream.write<unsigned long long>(meshTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 4ull;

	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	if (!writeTransformationToStream(meshes.world_T_meshes_, bitstream))
	{
		return false;
	}

	unsigned int numberMeshes = (unsigned int)(meshes.texturedMeshMap_.size());
	if (!bitstream.write<unsigned int>(numberMeshes))
	{
		return false;
	}

	for (NewTextureGenerator::TexturedMeshMap::const_iterator iMesh = meshes.texturedMeshMap_.cbegin(); iMesh != meshes.texturedMeshMap_.cend(); ++iMesh)
	{
		if (!bitstream.write(iMesh->first.data(), sizeof(VectorI3)))
		{
			return false;
		}

		if (!writeVectors3(iMesh->second.vertices_, bitstream))
		{
			return false;
		}

		if (!writeVectors2(iMesh->second.textureCoordinates_, bitstream))
		{
			return false;
		}
	}

	if (meshes.textureFrame_.isValid())
	{
		std::vector<uint8_t> encodedTextureFrameBuffer;
		if (!IO::Image::encodeImage(meshes.textureFrame_, "jpg", encodedTextureFrameBuffer))
		{
			return false;
		}

		const unsigned int textureFrameSize = (unsigned int)(encodedTextureFrameBuffer.size());
		if (!bitstream.write<unsigned int>(textureFrameSize))
		{
			return false;
		}

		if (!bitstream.write(encodedTextureFrameBuffer.data(), encodedTextureFrameBuffer.size()))
		{
			return false;
		}
	}
	else
	{
		constexpr unsigned int textureFrameSize = 0u;
		if (!bitstream.write<unsigned int>(textureFrameSize))
		{
			return false;
		}
	}

	return true;
}

bool Metaportation::writeObjectPointsToStream(const Vectors3& objectPoints, const Indices32& objectPointIds, IO::OutputBitstream& bitstream)
{
	ocean_assert(objectPoints.size() == objectPointIds.size());

	if (objectPoints.empty() || objectPoints.size() != objectPointIds.size())
	{
		return false;
	}

	if (!bitstream.write<unsigned long long>(objectPointsTag_))
	{
		return false;
	}

	constexpr unsigned long long version = 1ull;

	if (!bitstream.write<unsigned long long>(version))
	{
		return false;
	}

	if (!bitstream.write<unsigned int>((unsigned int)(objectPoints.size())))
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		if (!bitstream.write(objectPoints.data(), objectPoints.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 objectPointsF;
		objectPointsF.reserve(objectPoints.size());
		for (const Vector3& objectPoint : objectPoints)
		{
			objectPointsF.emplace_back(float(objectPoint.x()), float(objectPoint.y()), float(objectPoint.z()));
		}

		if (!bitstream.write(objectPointsF.data(), objectPointsF.size() * sizeof(VectorF3)))
		{
			return  false;
		}
	}

	if (!bitstream.write<unsigned int>((unsigned int)(objectPointIds.size())))
	{
		return false;
	}

	if (!bitstream.write(objectPointIds.data(), objectPointIds.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

bool Metaportation::writeVector3(const Vector3& vector, const unsigned long long tag, IO::OutputBitstream& bitstream)
{
	ocean_assert(tag != 0ull);

	if (!bitstream.write<unsigned long long>(tag))
	{
		return false;
	}

	if (!bitstream.write(VectorD3(vector).data(), sizeof(double) * 3))
	{
		return false;
	}

	return true;
}

bool Metaportation::writeVectors2(const Vectors2& vectors, IO::OutputBitstream& bitstream)
{
	const unsigned int numberVectors = (unsigned int)(vectors.size());
	if (!bitstream.write<unsigned int>(numberVectors))
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		if (!bitstream.write(vectors.data(), vectors.size() * sizeof(VectorF2)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF2 vectorsF;
		vectorsF.reserve(vectors.size());
		for (const Vector2& vec : vectors)
		{
			vectorsF.emplace_back(float(vec.x()), float(vec.y()));
		}

		if (!bitstream.write(vectorsF.data(), vectorsF.size() * sizeof(VectorF2)))
		{
			return  false;
		}
	}

	return true;
}

bool Metaportation::writeVectors3(const Vectors3& vectors, IO::OutputBitstream& bitstream)
{
	const unsigned int numberVectors = (unsigned int)(vectors.size());
	if (!bitstream.write<unsigned int>(numberVectors))
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		if (!bitstream.write(vectors.data(), vectors.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 vectorsF;
		vectorsF.reserve(vectors.size());
		for (const Vector3& vec : vectors)
		{
			vectorsF.emplace_back(float(vec.x()), float(vec.y()), float(vec.z()));
		}

		if (!bitstream.write(vectorsF.data(), vectorsF.size() * sizeof(VectorF3)))
		{
			return  false;
		}
	}

	return true;
}

bool Metaportation::readTransformationFromStream(IO::InputBitstream& bitstream, HomogenousMatrix4& world_T_camera)
{
	unsigned long long tag;
	if (!bitstream.read<unsigned long long>(tag) || tag != transformationTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	HomogenousMatrixD4 worldD_T_cameraD;
	if (!bitstream.read(worldD_T_cameraD.data(), sizeof(double) * 16))
	{
		return false;
	}

	world_T_camera = HomogenousMatrix4(worldD_T_cameraD);

	return world_T_camera.isValid();
}

bool Metaportation::readMeshesFromStream(IO::InputBitstream& bitstream, Meshes& meshes)
{
	constexpr unsigned int maximalVertices = 10u * 1000 * 1000u;
	constexpr unsigned int maximalImageSize = 20u * 1024u * 1024u;

	unsigned long long tag;
	if (!bitstream.read<unsigned long long>(tag) || tag != meshTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 4ull)
	{
		return false;
	}

	if (!readTransformationFromStream(bitstream, meshes.world_T_meshes_))
	{
		return false;
	}

	unsigned int numberMeshes = 0u;
	if (!bitstream.read<unsigned int>(numberMeshes))
	{
		return false;
	}

	meshes.texturedMeshMap_.clear();

	for (unsigned int nMesh = 0u; nMesh < numberMeshes; ++nMesh)
	{
		VectorI3 meshCoordinate;
		if (!bitstream.read(&meshCoordinate, sizeof(meshCoordinate)))
		{
			return false;
		}

		Vectors3 vertices;
		if (!readVectors3(bitstream, vertices, maximalVertices))
		{
			return false;
		}

		Vectors2 textureCoordinates;
		if (!readVectors2(bitstream, textureCoordinates, maximalVertices))
		{
			return false;
		}

		if (meshes.texturedMeshMap_.find(meshCoordinate) != meshes.texturedMeshMap_.cend())
		{
			return false;
		}

		meshes.texturedMeshMap_.emplace(meshCoordinate, NewTextureGenerator::TexturedMesh(std::move(vertices), std::move(textureCoordinates)));
	}

	unsigned int textureFrameSize = 0u;
	if (!bitstream.read<unsigned int>(textureFrameSize))
	{
		return false;
	}

	if (textureFrameSize != 0u)
	{
		if (textureFrameSize >= maximalImageSize)
		{
			return false;
		}

		std::vector<uint8_t> encodedTextureFrameBuffer(textureFrameSize);
		if (!bitstream.read(encodedTextureFrameBuffer.data(), encodedTextureFrameBuffer.size()))
		{
			return false;
		}

		meshes.textureFrame_ = IO::Image::decodeImage(encodedTextureFrameBuffer.data(), encodedTextureFrameBuffer.size(), "jpg");

		if (!meshes.textureFrame_.isValid())
		{
			return false;
		}
	}

	return true;
}

bool Metaportation::determineMeshIdInStream(IO::InputBitstream& bitstream, Index32& meshId)
{
	const IO::ScopedInputBitstream scopedInputStream(bitstream);

	unsigned long long tag;
	if (!bitstream.read<unsigned long long>(tag) || tag != meshTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 2ull)
	{
		return false;
	}

	if (!bitstream.read<Index32>(meshId))
	{
		return false;
	}

	return true;
}

bool Metaportation::readObjectPointsFromStream(IO::InputBitstream& bitstream, Vectors3& objectPoints, Indices32& objectPointIds)
{
	constexpr unsigned int maximalObjectPoints = 100u * 1000u;

	unsigned long long tag = 0ull;
	if (!bitstream.read<unsigned long long>(tag) || tag != objectPointsTag_)
	{
		return false;
	}

	unsigned long long version = 0ull;
	if (!bitstream.read<unsigned long long>(version) || version != 1ull)
	{
		return false;
	}

	unsigned int numberObjectPoints = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPoints))
	{
		return false;
	}

	if (numberObjectPoints > maximalObjectPoints)
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		objectPoints.resize(numberObjectPoints);

		if (!bitstream.read(objectPoints.data(), objectPoints.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 objectPointsF(numberObjectPoints);
		if (!bitstream.read(objectPointsF.data(), objectPointsF.size() * sizeof(VectorF3)))
		{
			return false;
		}

		objectPoints.clear();
		objectPoints.reserve(objectPointsF.size());

		for (VectorF3& objectPointF : objectPointsF)
		{
			objectPoints.emplace_back(Scalar(objectPointF.x()), Scalar(objectPointF.y()), Scalar(objectPointF.z()));
		}
	}

	unsigned int numberObjectPointIds = 0u;
	if (!bitstream.read<unsigned int>(numberObjectPointIds))
	{
		return false;
	}

	if (numberObjectPoints != numberObjectPointIds)
	{
		return false;
	}

	objectPointIds.resize(numberObjectPointIds);

	if (!bitstream.read(objectPointIds.data(), objectPointIds.size() * sizeof(Index32)))
	{
		return false;
	}

	return true;
}

bool Metaportation::readVector3(IO::InputBitstream& bitstream, const unsigned long long tag, Vector3& vector)
{
	unsigned long long readTag = 0ull;
	if (!bitstream.read<unsigned long long>(readTag) || readTag != tag)
	{
		return false;
	}

	VectorD3 readVector;
	if (!bitstream.read(readVector.data(), sizeof(double) * 3))
	{
		return false;
	}

	vector = Vector3(readVector);

	return true;
}

bool Metaportation::readVectors2(IO::InputBitstream& bitstream, Vectors2& vectors, const unsigned int maximalVectors)
{
	unsigned int numberVectors = 0u;
	if (!bitstream.read<unsigned int>(numberVectors) || numberVectors > maximalVectors)
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		vectors.resize(numberVectors);
		if (!bitstream.read(vectors.data(), vectors.size() * sizeof(VectorF2)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF2 vectorsF(numberVectors);
		if (!bitstream.read(vectorsF.data(), vectorsF.size() * sizeof(VectorF2)))
		{
			return false;
		}

		vectors.reserve(vectorsF.size());
		for (const VectorF2& vectorF : vectorsF)
		{
			vectors.emplace_back(Scalar(vectorF.x()), Scalar(vectorF.y()));
		}
	}

	return true;
}

bool Metaportation::readVectors3(IO::InputBitstream& bitstream, Vectors3& vectors, const unsigned int maximalVectors)
{
	unsigned int numberVectors = 0u;
	if (!bitstream.read<unsigned int>(numberVectors) || numberVectors > maximalVectors)
	{
		return false;
	}

	if (std::is_same<Scalar, float>::value)
	{
		vectors.resize(numberVectors);
		if (!bitstream.read(vectors.data(), vectors.size() * sizeof(VectorF3)))
		{
			return false;
		}
	}
	else
	{
		ocean_assert((std::is_same<Scalar, double>::value));

		VectorsF3 vectorsF(numberVectors);
		if (!bitstream.read(vectorsF.data(), vectorsF.size() * sizeof(VectorF3)))
		{
			return false;
		}

		vectors.reserve(vectorsF.size());
		for (const VectorF3& vectorF : vectorsF)
		{
			vectors.emplace_back(Scalar(vectorF.x()), Scalar(vectorF.y()), Scalar(vectorF.z()));
		}
	}

	return true;
}

}

}
