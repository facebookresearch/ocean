/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_TEXTURE_ATLAS_H
#define META_OCEAN_TRACKING_MAPTEXTURING_TEXTURE_ATLAS_H

#include "ocean/tracking/maptexturing/MapTexturing.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

/**
 * This class implements a texture atlas for triangles with regular shape.
 * @ingroup trackingmaptexturing
 */
class OCEAN_TRACKING_MAPTEXTURING_EXPORT TextureAtlas
{
	public:

		/**
		 * Default constructor.
		 */
		TextureAtlas();

		/**
		 * Returns the camera texture coordinates of a triangle within a camera frame.
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param flippedCamera_T_world
		 * @param meshTriangleId The id of the triangle for which the texture coordinates will be returned, with range [0, numberTriangles() - 1]
		 * @param vertices The three vertices for which the texture coordinates will be returned, must be valid
		 * @param textureCoordinates The resulting three texture coordinates, one for each given vertex, must be valid
		 * @param respectBorder True, to respect the border of the triangles; False, to ignore the border of the triangles
		 */
		void triangleCameraTextureCoordiantes(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world, const Index32 meshTriangleId, const Vector3* vertices, Vector2* textureCoordinates, const bool respectBorder) const;

		/**
		 * Returns the atlas texture coordinates of a triangle within this atlas.
		 * @param meshTriangleId The id of the triangle for which the texture coordinates will be returned, with range [0, numberTriangles() - 1]
		 * @param textureCoordinates The resulting three texture coordinates, must be valid
		 * @tparam tInnerTriangleWithBorder True, to determine the texture coordinate for the inner triangle; False, to determine the texture coordinate for the outer triangle
		 */
		template <bool innerTriangleWithBorder>
		void triangleAtlasTextureCoordinates(const Index32 meshTriangleId, Vector2* textureCoordinates) const;

		/**
		 * Returns the barycentric triangle coordinates for the outer triangle.
		 * @param triangleId The id of the triangle for which the coordinate will be returned
		 * @param index The vertex index for which the barycentric coordinate will be returned, with range [0, 2]
		 * @return The resulting barycentric triangle coordinates
		 */
		inline const Vector3& outerBorderBarycentric(const Index32 triangleId, const Index32 index) const;

		/**
		 * Returns the texture size of this atlas.
		 * @return The atlas' texture width and height, in pixels
		 */
		static constexpr unsigned int textureSizePixels();

		/**
		 * Returns the major edge size of all triangles of this atlas.
		 * @return The atlas' triangle major edge size, in pixels
		 */
		static constexpr unsigned int triangleSizePixels();

		/**
		 * The border size of the triangles.
		 * @return The triangles' border size, in pixels
		 */
		static constexpr Scalar borderSizePixels();

		/**
		 * Returns the number of bins this atlas holds.
		 * @return The atlas' bins
		 */
		static constexpr unsigned int numberBins();

		/**
		 * Returns the number of triangles this atlas can hold.
		 * @return The atlas' triangle capacity
		 */
		static constexpr unsigned int numberTriangles();

	protected:

		/// The barycentric triangle coordinates for the outer triangles with even id.
		Vector3 outerBorderBarycentricEven_[3];

		/// The barycentric triangle coordinates for the outer triangles with odd id.
		Vector3 outerBorderBarycentricOdd_[3];

		/// The diagonal border size of the triangles, in pixels.
		Scalar diagonalSizePixels_ = Scalar(0);
};

inline const Vector3& TextureAtlas::outerBorderBarycentric(const Index32 triangleId, const Index32 index) const
{
	ocean_assert(index < 3u);

	if (triangleId % 2u == 0u)
	{
		return outerBorderBarycentricEven_[index];
	}
	else
	{
		return outerBorderBarycentricOdd_[index];
	}
}

constexpr unsigned int TextureAtlas::textureSizePixels()
{
#ifdef OCEAN_PLATFORM_BUILD_MOBILE
	return 1792u;
#else
	return 4096u;
#endif
}

constexpr unsigned int TextureAtlas::triangleSizePixels()
{
#ifdef OCEAN_PLATFORM_BUILD_MOBILE
	return 14u;
#else
	return 32u;
#endif
}

constexpr Scalar TextureAtlas::borderSizePixels()
{
	return Scalar(1.25);
}

constexpr unsigned int TextureAtlas::numberBins()
{
	return textureSizePixels() / triangleSizePixels();
}

constexpr unsigned int TextureAtlas::numberTriangles()
{
	constexpr unsigned int bins = numberBins();

	return bins * bins * 2u;
}

template <bool tInnerTriangleWithBorder>
void TextureAtlas::triangleAtlasTextureCoordinates(const Index32 meshTriangleId, Vector2* textureCoordinates) const
{
	constexpr unsigned int bins = numberBins();

	ocean_assert(meshTriangleId < numberTriangles());
	ocean_assert(textureCoordinates != nullptr);

	const unsigned int binId = meshTriangleId / 2u;

	const unsigned int xBin = binId % bins;
	const unsigned int yBin = binId / bins;

	const unsigned int leftBinPixels = xBin * triangleSizePixels();
	const unsigned int topBinPixels = yBin * triangleSizePixels();

	//  ------------   ------------
	// |\           | |\           |
	// |   \    odd | |   \    odd |
	// |     \      | |     \      | .....
	// |  even \    | |  even \    |
	// |         \  | |         \  |
	//  ------------   ------------
	//              ....

	const Scalar leftBinTexture = Scalar(leftBinPixels) / Scalar(textureSizePixels());
	const Scalar rightBinTexture = Scalar(leftBinPixels + triangleSizePixels()) / Scalar(textureSizePixels());

	const Scalar topBinTexture = Scalar(1) - Scalar(topBinPixels) / Scalar(textureSizePixels());
	const Scalar bottomBinTexture = Scalar(1) - Scalar(topBinPixels + triangleSizePixels()) / Scalar(textureSizePixels());

	if constexpr (tInnerTriangleWithBorder)
	{
		const Scalar smallOffsetTexture = borderSizePixels() / Scalar(textureSizePixels());
		const Scalar bigOffsetTexture = (borderSizePixels() + diagonalSizePixels_) / Scalar(textureSizePixels());

		if (meshTriangleId % 2u == 0u)
		{
			// lower left triangle of bin

			textureCoordinates[0] = Vector2(leftBinTexture + smallOffsetTexture, topBinTexture - bigOffsetTexture);
			textureCoordinates[1] = Vector2(leftBinTexture + smallOffsetTexture, bottomBinTexture + smallOffsetTexture);
			textureCoordinates[2] = Vector2(rightBinTexture - bigOffsetTexture, bottomBinTexture + smallOffsetTexture);
		}
		else
		{
			// upper right triangle of bin

			textureCoordinates[0] = Vector2(leftBinTexture + bigOffsetTexture, topBinTexture - smallOffsetTexture);
			textureCoordinates[1] = Vector2(rightBinTexture - smallOffsetTexture, bottomBinTexture + bigOffsetTexture);
			textureCoordinates[2] = Vector2(rightBinTexture - smallOffsetTexture, topBinTexture - smallOffsetTexture);
		}
	}
	else
	{
		if (meshTriangleId % 2u == 0u)
		{
			// lower left triangle of bin

			textureCoordinates[0] = Vector2(leftBinTexture, topBinTexture);
			textureCoordinates[1] = Vector2(leftBinTexture, bottomBinTexture);
			textureCoordinates[2] = Vector2(rightBinTexture, bottomBinTexture);
		}
		else
		{
			// upper right triangle of bin

			textureCoordinates[0] = Vector2(leftBinTexture, topBinTexture);
			textureCoordinates[1] = Vector2(rightBinTexture, bottomBinTexture);
			textureCoordinates[2] = Vector2(rightBinTexture, topBinTexture);
		}
	}
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_TEXTURE_ATLAS_H
