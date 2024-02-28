// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/uvtexturemapping/Utilities.h"

namespace Ocean
{

namespace Tracking
{

namespace UVTextureMapping
{

MeshUVTextureMappingRef Utilities::createCubemapUVTextureMapping(Scalar width, Scalar height, Scalar depth, Scalar imageWidth, Scalar imageHeight)
{
	// Our assumed cubemap-type layout looks like this:
	//
	//                           3D Coordinates with w=width/2, h=height/2, and d=depth/2
	//    <--H--><--W--><--H-->  0 = (-w, -h, -d) = v0
	//  ^ -------0-----1-------  1 = (w, -h, -d) = v1
	//  | |      |   / |      |  2 = 0
	//  H |      | 0/1 |      |  3 = (-w, h, -d) = v2
	//  | |      | /   |      |  4 = (w, h, -d) = v3
	//  - 2------3/----4------5  5 = 1
	//  | |\     |\    |\     |  6 = (-w, -h, d) = v4
	//  D | 6\7  | 8\9 | 3\2  |  7 = (-w, h, d) = v6
	//  | |    \ |    \|    \ |  8 = (w, h, d) = v7
	//  - 6------7-----8------9  9 = (w, -h, d) = v5
	//  | |      |    /|      |
	//  H |      | 5/4 |      |   <- Faces 5 and 4 make up the front. X is right, Y is up, and
	//  | |      | /   |      |      Z is forward. The origin is in the cube's center.
	//  - |     10----11      |                     Y
	//  | |      |\    |      | 10 = 6           7--^--8      }
	//  D |      |11\10|      | 11 = 9           |  |  |      } For the front face of the cube,
	//  | |      |    \|      | 12 = 0           |  .---->X   } Z = depth / 2.
	//  V ------12----13------- 13 = 1           |     |      }
	//                                          10----11      }

	ocean_assert(width > 0 && height > 0 && depth > 0);
	ocean_assert(imageWidth > 0 && imageHeight > 0);

	if (width <= Scalar(0) || height <= Scalar(0) || depth <= Scalar(0) || imageWidth <= Scalar(0) || imageHeight <= Scalar(0))
	{
		return MeshUVTextureMappingRef();
	}
	const Scalar halfWidth = Scalar(0.5) * width;
	const Scalar halfHeight = Scalar(0.5) * height;
	const Scalar halfDepth = Scalar(0.5) * depth;

	const Vectors3 vertices = {{-halfWidth, -halfHeight, -halfDepth},
										{halfWidth, -halfHeight, -halfDepth},
										{-halfWidth, halfHeight, -halfDepth},
										{halfWidth, halfHeight, -halfDepth},
										{-halfWidth, -halfHeight, halfDepth},
										{halfWidth, -halfHeight, halfDepth},
										{-halfWidth, halfHeight, halfDepth},
										{halfWidth, halfHeight, halfDepth}};

	const Scalar cubemapXScale = imageWidth / (width + Scalar(2.0) * height);
	const Scalar cubemapYScale = imageHeight / (Scalar(2.0) * (height + depth));
	const Scalar xOffset1 = height * cubemapXScale;
	const Scalar xOffset2 = (width + height) * cubemapXScale;
	const Scalar yOffset1 = height * cubemapYScale;
	const Scalar yOffset2 = (depth + height) * cubemapYScale;
	const Scalar yOffset3 = (depth + Scalar(2.0) * height) * cubemapYScale;

	const Vectors2 textureCoordinates = {{xOffset1, Scalar(0.0)},
												{xOffset2, Scalar(0.0)},
												{Scalar(0.0), yOffset1},
												{xOffset1, yOffset1},
												{xOffset2, yOffset1},
												{static_cast<float>(imageWidth), yOffset1},
												{Scalar(0.0), yOffset2},
												{xOffset1, yOffset2},
												{xOffset2, yOffset2},
												{static_cast<float>(imageWidth), yOffset2},
												{xOffset1, yOffset3},
												{xOffset2, yOffset3},
												{xOffset1, static_cast<float>(imageHeight)},
												{xOffset2, static_cast<float>(imageHeight)}};

	const MeshUVTextureMapping::TriangleFaces faces =
	{
		{0u, 2u, 1u},
		{1u, 2u, 3u},
		{1u, 3u, 5u},
		{5u, 3u, 7u},
		{5u, 7u, 4u},
		{4u, 7u, 6u},
		{4u, 6u, 0u},
		{0u, 6u, 2u},
		{6u, 7u, 2u},
		{2u, 7u, 3u},
		{5u, 4u, 1u},
		{1u, 4u, 0u}
	};

	const MeshUVTextureMapping::TriangleFaces textureFaces =
	{
		{0u, 3u, 1u},
		{1u, 3u, 4u},
		{5u, 4u, 9u},
		{9u, 4u, 8u},
		{11u, 8u, 10u},
		{10u, 8u, 7u},
		{6u, 7u, 2u},
		{2u, 7u, 3u},
		{7u, 8u, 3u},
		{3u, 8u, 4u},
		{11u, 10u, 13u},
		{13u, 10u, 12u}
	};

	return MeshUVTextureMappingRef(new MeshUVTextureMapping(
		vertices, textureCoordinates, faces, textureFaces));
}

} // namespace UVTextureMapping

} // namespace Tracking

} // namespace Ocean
