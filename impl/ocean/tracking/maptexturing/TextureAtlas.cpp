/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/maptexturing/TextureAtlas.h"

#include "ocean/math/Triangle2.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

TextureAtlas::TextureAtlas()
{
	diagonalSizePixels_ = Numeric::sqrt(Numeric::sqr(borderSizePixels()) * Scalar(2));

	const Scalar smallOffset = borderSizePixels();
	const Scalar bigOffset = borderSizePixels() + diagonalSizePixels_;

	const Triangle2 outerBorderTriangleEven(Vector2(0, 0), Vector2(0, Scalar(triangleSizePixels())), Vector2(Scalar(triangleSizePixels()), Scalar(triangleSizePixels())));
	const Triangle2 innerBorderTriangleEven(Vector2(smallOffset, bigOffset), Vector2(smallOffset, Scalar(triangleSizePixels()) - smallOffset), Vector2(Scalar(triangleSizePixels()) - bigOffset, Scalar(triangleSizePixels()) - smallOffset));

	const Triangle2 outerBorderTriangleOdd(Vector2(0, 0), Vector2(Scalar(triangleSizePixels()), Scalar(triangleSizePixels())), Vector2(Scalar(triangleSizePixels()), 0));
	const Triangle2 innerBorderTriangleOdd(Vector2(bigOffset, smallOffset), Vector2(Scalar(Scalar(triangleSizePixels())) - smallOffset, Scalar(Scalar(triangleSizePixels())) - bigOffset), Vector2(Scalar(triangleSizePixels()) - smallOffset, smallOffset));

	outerBorderBarycentricEven_[0] = innerBorderTriangleEven.cartesian2barycentric(outerBorderTriangleEven.point0());
	outerBorderBarycentricEven_[1] = innerBorderTriangleEven.cartesian2barycentric(outerBorderTriangleEven.point1());
	outerBorderBarycentricEven_[2] = innerBorderTriangleEven.cartesian2barycentric(outerBorderTriangleEven.point2());

	outerBorderBarycentricOdd_[0] = innerBorderTriangleOdd.cartesian2barycentric(outerBorderTriangleOdd.point0());
	outerBorderBarycentricOdd_[1] = innerBorderTriangleOdd.cartesian2barycentric(outerBorderTriangleOdd.point1());
	outerBorderBarycentricOdd_[2] = innerBorderTriangleOdd.cartesian2barycentric(outerBorderTriangleOdd.point2());
}

void TextureAtlas::triangleCameraTextureCoordiantes(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world, const Index32 meshTriangleId, const Vector3* vertices, Vector2* textureCoordinates, const bool respectBorder) const
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(flippedCamera_T_world.isValid());
	ocean_assert(vertices != nullptr && textureCoordinates != nullptr);

	Vector2 projectedVertices[3];
	anyCamera.projectToImageIF(flippedCamera_T_world, vertices, 3, projectedVertices);

	const Scalar invWidth = Scalar(1) / Scalar(anyCamera.width());
	const Scalar invHeight = Scalar(1) / Scalar(anyCamera.height());

	// now, we normalize the image points into the range [0, 1)x[0, 1)

	for (unsigned int n = 0u; n < 3u; ++n)
	{
		projectedVertices[n] = Vector2(projectedVertices[n].x() * invWidth,projectedVertices[n].y() * invHeight);
	}

	if (respectBorder)
	{
		const Triangle2 projectedTriangle(projectedVertices[0], projectedVertices[1], projectedVertices[2]);

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			textureCoordinates[n] = projectedTriangle.barycentric2cartesian(outerBorderBarycentric(meshTriangleId, n));
		}
	}
	else
	{
		for (unsigned int n = 0u; n < 3u; ++n)
		{
			textureCoordinates[n] = projectedVertices[n];
		}
	}
}

}

}

}
