/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/maptexturing/BlockedMesh.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

BlockedMeshes BlockedMesh::separateMesh(const Vectors3& vertices, const Rendering::TriangleFaces& triangleFaces, const int blockSize)
{
	const Box3 box(vertices);

	if (!box.isValid())
	{
		return BlockedMeshes();
	}

	const VectorT3<int> lowerBlockCoordinates = vertex2block(box.lower(), blockSize);
	const VectorT3<int> higherBlockCoordinates = vertex2block(box.higher(), blockSize);

	const int xBlocks = higherBlockCoordinates.x() - lowerBlockCoordinates.x() + 1;
	const int yBlocks = higherBlockCoordinates.y() - lowerBlockCoordinates.y() + 1;
	const int zBlocks = higherBlockCoordinates.z() - lowerBlockCoordinates.z() + 1;
	ocean_assert(xBlocks >= 1 && yBlocks >= 1 && zBlocks >= 1);

	const unsigned int xyBlocks = (unsigned int)(xBlocks * yBlocks);

	BlockedMeshes blockedMeshes(size_t(xBlocks * yBlocks * zBlocks));

	for (int z = 0u; z < zBlocks; ++z)
	{
		for (int y = 0u; y < yBlocks; ++y)
		{
			for (int x = 0u; x < xBlocks; ++x)
			{
				const unsigned int triangleBlockIndex = (unsigned int)(z) * xyBlocks + (unsigned int)(y) * (unsigned int)(xBlocks) + (unsigned int)(x);

				BlockedMesh& blockedMesh = blockedMeshes[triangleBlockIndex];

				blockedMesh.block_ = VectorI3(lowerBlockCoordinates.x() + x, lowerBlockCoordinates.y() + y, lowerBlockCoordinates.z() + z);

				const Vector3 boundingBoxLower = Vector3(Scalar(blockedMesh.block_.x()), Scalar(blockedMesh.block_.y()), Scalar(blockedMesh.block_.z())) * Scalar(blockSize);
				blockedMesh.boundingBox_ = Box3(boundingBoxLower, boundingBoxLower + Vector3(Scalar(blockSize), Scalar(blockSize), Scalar(blockSize)));
				blockedMesh.boundingSphere_ = Sphere3(blockedMesh.boundingBox_);
			}
		}
	}

	for (const Rendering::TriangleFace& triangleFace : triangleFaces)
	{
		const Index32& index0 = triangleFace[0];

		const VectorT3<int> triangleBlock = vertex2block(vertices[index0], blockSize);

		const unsigned int xTriangleBlock = (unsigned int)(triangleBlock.x() - lowerBlockCoordinates.x());
		const unsigned int yTriangleBlock = (unsigned int)(triangleBlock.y() - lowerBlockCoordinates.y());
		const unsigned int zTriangleBlock = (unsigned int)(triangleBlock.z() - lowerBlockCoordinates.z());
		ocean_assert(int(xTriangleBlock) < xBlocks && int(yTriangleBlock) < yBlocks && int(zTriangleBlock) < zBlocks);

		const unsigned int triangleBlockIndex = zTriangleBlock * xyBlocks + yTriangleBlock * (unsigned int)(xBlocks) + xTriangleBlock;

		blockedMeshes[triangleBlockIndex].addTriangle(triangleFace, vertices.data());
	}

	for (size_t n = 0; n < blockedMeshes.size(); /*noop*/)
	{
		if (blockedMeshes[n].isValid())
		{
			++n;
		}
		else
		{
			blockedMeshes[n] = std::move(blockedMeshes.back());
			blockedMeshes.pop_back();
		}
	}

	return blockedMeshes;
}

BlockedMeshes BlockedMesh::separateMesh(const Vectors3& vertices, const Vectors3& perVertexNormals, const Rendering::TriangleFaces& triangleFaces, const int blockSize)
{
	const Box3 box(vertices);

	if (!box.isValid())
	{
		return BlockedMeshes();
	}

	const VectorT3<int> lowerBlockCoordinates = vertex2block(box.lower(), blockSize);
	const VectorT3<int> higherBlockCoordinates = vertex2block(box.higher(), blockSize);

	const int xBlocks = higherBlockCoordinates.x() - lowerBlockCoordinates.x() + 1;
	const int yBlocks = higherBlockCoordinates.y() - lowerBlockCoordinates.y() + 1;
	const int zBlocks = higherBlockCoordinates.z() - lowerBlockCoordinates.z() + 1;
	ocean_assert(xBlocks >= 1 && yBlocks >= 1 && zBlocks >= 1);

	const unsigned int xyBlocks = (unsigned int)(xBlocks * yBlocks);

	BlockedMeshes blockedMeshes(size_t(xBlocks * yBlocks * zBlocks));

	for (int z = 0u; z < zBlocks; ++z)
	{
		for (int y = 0u; y < yBlocks; ++y)
		{
			for (int x = 0u; x < xBlocks; ++x)
			{
				const unsigned int triangleBlockIndex = (unsigned int)(z) * xyBlocks + (unsigned int)(y) * (unsigned int)(xBlocks) + (unsigned int)(x);

				BlockedMesh& blockedMesh = blockedMeshes[triangleBlockIndex];

				blockedMesh.block_ = VectorI3(lowerBlockCoordinates.x() + x, lowerBlockCoordinates.y() + y, lowerBlockCoordinates.z() + z);

				const Vector3 boundingBoxLower = Vector3(Scalar(blockedMesh.block_.x()), Scalar(blockedMesh.block_.y()), Scalar(blockedMesh.block_.z())) * Scalar(blockSize);
				blockedMesh.boundingBox_ = Box3(boundingBoxLower, boundingBoxLower + Vector3(Scalar(blockSize), Scalar(blockSize), Scalar(blockSize)));
				blockedMesh.boundingSphere_ = Sphere3(blockedMesh.boundingBox_);
			}
		}
	}

	for (const Rendering::TriangleFace& triangleFace : triangleFaces)
	{
		const Index32& index0 = triangleFace[0];

		const VectorT3<int> triangleBlock = vertex2block(vertices[index0], blockSize);

		const unsigned int xTriangleBlock = (unsigned int)(triangleBlock.x() - lowerBlockCoordinates.x());
		const unsigned int yTriangleBlock = (unsigned int)(triangleBlock.y() - lowerBlockCoordinates.y());
		const unsigned int zTriangleBlock = (unsigned int)(triangleBlock.z() - lowerBlockCoordinates.z());
		ocean_assert(int(xTriangleBlock) < xBlocks && int(yTriangleBlock) < yBlocks && int(zTriangleBlock) < zBlocks);

		const unsigned int triangleBlockIndex = zTriangleBlock * xyBlocks + yTriangleBlock * (unsigned int)(xBlocks) + xTriangleBlock;

		blockedMeshes[triangleBlockIndex].addTriangle(triangleFace, vertices.data(), perVertexNormals.data());
	}

	for (size_t n = 0; n < blockedMeshes.size(); /*noop*/)
	{
		if (blockedMeshes[n].isValid())
		{
			++n;
		}
		else
		{
			blockedMeshes[n] = std::move(blockedMeshes.back());
			blockedMeshes.pop_back();
		}
	}

	return blockedMeshes;
}

BlockedMeshes BlockedMesh::separateMesh(const Vectors3& vertices, const Vectors3& perVertexNormals, const RGBAColors& perVertexColors, const Rendering::TriangleFaces& triangleFaces, const int blockSize)
{
	const Box3 box(vertices);

	if (!box.isValid())
	{
		return BlockedMeshes();
	}

	const VectorT3<int> lowerBlockCoordinates = vertex2block(box.lower(), blockSize);
	const VectorT3<int> higherBlockCoordinates = vertex2block(box.higher(), blockSize);

	const int xBlocks = higherBlockCoordinates.x() - lowerBlockCoordinates.x() + 1;
	const int yBlocks = higherBlockCoordinates.y() - lowerBlockCoordinates.y() + 1;
	const int zBlocks = higherBlockCoordinates.z() - lowerBlockCoordinates.z() + 1;
	ocean_assert(xBlocks >= 1 && yBlocks >= 1 && zBlocks >= 1);

	const unsigned int xyBlocks = (unsigned int)(xBlocks * yBlocks);

	BlockedMeshes blockedMeshes(size_t(xBlocks * yBlocks * zBlocks));

	for (int z = 0u; z < zBlocks; ++z)
	{
		for (int y = 0u; y < yBlocks; ++y)
		{
			for (int x = 0u; x < xBlocks; ++x)
			{
				const unsigned int triangleBlockIndex = (unsigned int)(z) * xyBlocks + (unsigned int)(y) * (unsigned int)(xBlocks) + (unsigned int)(x);

				BlockedMesh& blockedMesh = blockedMeshes[triangleBlockIndex];

				blockedMesh.block_ = VectorI3(lowerBlockCoordinates.x() + x, lowerBlockCoordinates.y() + y, lowerBlockCoordinates.z() + z);

				const Vector3 boundingBoxLower = Vector3(Scalar(blockedMesh.block_.x()), Scalar(blockedMesh.block_.y()), Scalar(blockedMesh.block_.z())) * Scalar(blockSize);
				blockedMesh.boundingBox_ = Box3(boundingBoxLower, boundingBoxLower + Vector3(Scalar(blockSize), Scalar(blockSize), Scalar(blockSize)));
				blockedMesh.boundingSphere_ = Sphere3(blockedMesh.boundingBox_);
			}
		}
	}

	for (const Rendering::TriangleFace& triangleFace : triangleFaces)
	{
		const Index32& index0 = triangleFace[0];

		const VectorT3<int> triangleBlock = vertex2block(vertices[index0], blockSize);

		const unsigned int xTriangleBlock = (unsigned int)(triangleBlock.x() - lowerBlockCoordinates.x());
		const unsigned int yTriangleBlock = (unsigned int)(triangleBlock.y() - lowerBlockCoordinates.y());
		const unsigned int zTriangleBlock = (unsigned int)(triangleBlock.z() - lowerBlockCoordinates.z());
		ocean_assert(int(xTriangleBlock) < xBlocks && int(yTriangleBlock) < yBlocks && int(zTriangleBlock) < zBlocks);

		const unsigned int triangleBlockIndex = zTriangleBlock * xyBlocks + yTriangleBlock * (unsigned int)(xBlocks) + xTriangleBlock;

		blockedMeshes[triangleBlockIndex].addTriangle(triangleFace, vertices.data(), perVertexNormals.data(), perVertexColors.data());
	}

	for (size_t n = 0; n < blockedMeshes.size(); /*noop*/)
	{
		if (blockedMeshes[n].isValid())
		{
			++n;
		}
		else
		{
			blockedMeshes[n] = std::move(blockedMeshes.back());
			blockedMeshes.pop_back();
		}
	}

	return blockedMeshes;
}

void BlockedMesh::addTriangle(const Rendering::TriangleFace& triangleFace, const Vector3* vertices)
{
	Rendering::TriangleFace localTriangleFace;

	for (unsigned int n = 0u; n < 3u; ++n)
	{
		const Index32& globalIndex = triangleFace[n];

		const IndexMap::const_iterator i = indexMap_.find(globalIndex);

		if (i == indexMap_.cend())
		{
			const Index32 localIndex = Index32(vertices_.size());

			indexMap_.emplace(globalIndex, localIndex);

			vertices_.emplace_back(vertices[globalIndex]);

			localTriangleFace[n] = localIndex;
		}
		else
		{
			const Index32& localIndex = i->second;

			localTriangleFace[n] = localIndex;
		}
	}

	triangleFaces_.emplace_back(localTriangleFace);
}

void BlockedMesh::addTriangle(const Rendering::TriangleFace& triangleFace, const Vector3* vertices, const Vector3* perVertexNormals)
{
	Rendering::TriangleFace localTriangleFace;

	for (unsigned int n = 0u; n < 3u; ++n)
	{
		const Index32& globalIndex = triangleFace[n];

		const IndexMap::const_iterator i = indexMap_.find(globalIndex);

		if (i == indexMap_.cend())
		{
			const Index32 localIndex = Index32(vertices_.size());

			indexMap_.emplace(globalIndex, localIndex);

			vertices_.emplace_back(vertices[globalIndex]);
			perVertexNormals_.emplace_back(perVertexNormals[globalIndex]);

			localTriangleFace[n] = localIndex;
		}
		else
		{
			const Index32& localIndex = i->second;

			localTriangleFace[n] = localIndex;
		}
	}

	triangleFaces_.emplace_back(localTriangleFace);
}

void BlockedMesh::addTriangle(const Rendering::TriangleFace& triangleFace, const Vector3* vertices, const Vector3* perVertexNormals, const RGBAColor* perVertexColors)
{
	Rendering::TriangleFace localTriangleFace;

	for (unsigned int n = 0u; n < 3u; ++n)
	{
		const Index32& globalIndex = triangleFace[n];

		const IndexMap::const_iterator i = indexMap_.find(globalIndex);

		if (i == indexMap_.cend())
		{
			const Index32 localIndex = Index32(vertices_.size());

			indexMap_.emplace(globalIndex, localIndex);

			vertices_.emplace_back(vertices[globalIndex]);
			perVertexNormals_.emplace_back(perVertexNormals[globalIndex]);
			perVertexColors_.emplace_back(perVertexColors[globalIndex]);

			localTriangleFace[n] = localIndex;
		}
		else
		{
			const Index32& localIndex = i->second;

			localTriangleFace[n] = localIndex;
		}
	}

	triangleFaces_.emplace_back(localTriangleFace);
}

VectorT3<int> BlockedMesh::vertex2block(const Vector3& vertex, const int blockSize)
{
	const int xBlock = vertex.x() >= Scalar(0) ? int(vertex.x()) / blockSize : int(vertex.x()) / blockSize - 1;
	const int yBlock = vertex.y() >= Scalar(0) ? int(vertex.y()) / blockSize : int(vertex.y()) / blockSize - 1;
	const int zBlock = vertex.z() >= Scalar(0) ? int(vertex.z()) / blockSize : int(vertex.z()) / blockSize - 1;

	return VectorT3(xBlock, yBlock, zBlock);
}

}

}

}
