/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/uvtexturemapping/MeshUVTextureMapping.h"

#include "ocean/math/Box2.h"

namespace Ocean
{

namespace Tracking
{

namespace UVTextureMapping
{

MeshUVTextureMapping::MeshUVTextureMapping()
{
	// nothing to do here
}

MeshUVTextureMapping::MeshUVTextureMapping(const Vectors3& vertices, const Vectors2& textureCoordinates, const TriangleFaces& vertexTriangleFaces, const TriangleFaces& textureTriangleFaces) :
	boundingBox_(vertices)
{
	ocean_assert(vertexTriangleFaces.size() == textureTriangleFaces.size());

	if (vertexTriangleFaces.size() != textureTriangleFaces.size())
	{
		Log::error() << "Number of mesh 3D triangles does not equal the number of texture triangles!";
		return;
	}

	//
	// Initialize our spatial distribution array for querying the potential triangles overlapping a
	// given region.
	//

	constexpr Scalar kBinSize = Scalar(5.); // in pixels; TODO (jtprice): Maybe make this a configurable parameter?
	constexpr Scalar kMaxTextureArea = Scalar(2.68e8); // ~256MiB; TODO (jtprice): Make this device-dependent

	const Box2 textureCoordinatesBbox(textureCoordinates);

	ocean_assert(textureCoordinatesBbox.isValid() && !Numeric::isEqualEps(textureCoordinatesBbox.width()) && !Numeric::isEqualEps(textureCoordinatesBbox.height()) && textureCoordinatesBbox.width() * textureCoordinatesBbox.height() <= kMaxTextureArea);

	if (!textureCoordinatesBbox.isValid() ||
		Numeric::isEqualEps(textureCoordinatesBbox.width()) ||
		Numeric::isEqualEps(textureCoordinatesBbox.height()) ||
		textureCoordinatesBbox.width() * textureCoordinatesBbox.height() > kMaxTextureArea)
	{
		Log::error() << "Invalid texture coordinate range (left, top, right, bottom, area): "
					 << textureCoordinatesBbox.left() << ", "
					 << textureCoordinatesBbox.top() << ", "
					 << textureCoordinatesBbox.right() << ", "
					 << textureCoordinatesBbox.bottom() << ", "
					 << textureCoordinatesBbox.width() * textureCoordinatesBbox.height();
		return; // invalid
	}

	const Scalar horizontalBins = Numeric::ceil(textureCoordinatesBbox.width() / kBinSize);
	const Scalar verticalBins = Numeric::ceil(textureCoordinatesBbox.height() / kBinSize);
	distributionArray_ = Geometry::SpatialDistribution::DistributionArray(
		textureCoordinatesBbox.left(),
		textureCoordinatesBbox.top(),
		horizontalBins * kBinSize,
		verticalBins * kBinSize,
		static_cast<unsigned int>(horizontalBins),
		static_cast<unsigned int>(verticalBins));

	//
	// Now, actually load the faces.
	//

	triangles3_.reserve(vertexTriangleFaces.size());
	triangles2_.reserve(textureTriangleFaces.size());

	for (size_t faceIndex = 0u; faceIndex < vertexTriangleFaces.size(); ++faceIndex)
	{
		const TriangleFace& vertexFace = vertexTriangleFaces[faceIndex];
		const TriangleFace& textureFace = textureTriangleFaces[faceIndex];

		ocean_assert(vertexFace[0] < vertices.size());
		ocean_assert(vertexFace[1] < vertices.size());
		ocean_assert(vertexFace[2] < vertices.size());
		ocean_assert(textureFace[0] < textureCoordinates.size());
		ocean_assert(textureFace[1] < textureCoordinates.size());
		ocean_assert(textureFace[2] < textureCoordinates.size());

		if (vertexFace[0] >= vertices.size() ||
				vertexFace[1] >= vertices.size() ||
				vertexFace[2] >= vertices.size() ||
				textureFace[0] >= textureCoordinates.size() ||
				textureFace[1] >= textureCoordinates.size() ||
				textureFace[2] >= textureCoordinates.size())
		{
			continue; // invalid indices
		}

		const Triangle3 vertexTriangle(vertices[vertexFace[0]], vertices[vertexFace[1]], vertices[vertexFace[2]]);
		const Triangle2 textureTriangle(textureCoordinates[textureFace[0]], textureCoordinates[textureFace[1]], textureCoordinates[textureFace[2]]);

		// Simply ignore invalid triangles, but do not invalidate the entire UV mapping if they exist.
		ocean_assert(vertexTriangle.isValid());
		ocean_assert(textureTriangle.isValid());
		if (!vertexTriangle.isValid() || !textureTriangle.isValid())
		{
			continue;
		}

		// Register the 2D triangle in the distribution array.
		const unsigned int xMin = distributionArray_.clampedHorizontalBin(textureTriangle.left());
		const unsigned int xMax = distributionArray_.clampedHorizontalBin(textureTriangle.right());
		const unsigned int yMin = distributionArray_.clampedVerticalBin(textureTriangle.top());
		const unsigned int yMax = distributionArray_.clampedVerticalBin(textureTriangle.bottom());

		const Triangle2 binSpaceTriangle = Triangle2(textureTriangle.point0() / kBinSize, textureTriangle.point1() / kBinSize, textureTriangle.point2() / kBinSize) - textureCoordinatesBbox.lower() / kBinSize;

		for (unsigned int y = yMin; y <= yMax; ++y)
		{
			for (unsigned int x = xMin; x <= xMax; ++x)
			{
				// Split the bin into two triangles and see if the current triangle overlaps either.
				const Triangle2 upperTriangle(Vector2(Scalar(x), Scalar(y)), Vector2(Scalar(x), Scalar(y + 1u)), Vector2(Scalar(x + 1u), Scalar(y)));
				const Triangle2 lowerTriangle(Vector2(Scalar(x), Scalar(y + 1u)), Vector2(Scalar(x + 1u), Scalar(y)), Vector2(Scalar(x + 1u), Scalar(y + 1u)));
				if (binSpaceTriangle.intersects(upperTriangle) || binSpaceTriangle.intersects(lowerTriangle))
				{
					distributionArray_(x, y).push_back(static_cast<Index32>(triangles2_.size()));
				}
			}
		}

		triangles3_.push_back(vertexTriangle);
		triangles2_.push_back(textureTriangle);
	}
}

bool MeshUVTextureMapping::textureCoordinateTo3DCoordinate(const Vector2& point2D, Vector3& point3D, Index32* containingTriangleIndex) const
{
	const Box2 textureCoordinatesBbox(distributionArray_.width(), distributionArray_.height(), Vector2(distributionArray_.left(), distributionArray_.top()));

	if (textureCoordinatesBbox.isInside(point2D))
	{
		const unsigned int horizontalBin = distributionArray_.horizontalBin(point2D.x());
		const unsigned int verticalBin = distributionArray_.verticalBin(point2D.y());

		// Try every triangle that overlaps the specified bin.
		for (const Index32& triangleIndex : distributionArray_(horizontalBin, verticalBin))
		{
			if (textureCoordinateTo3DCoordinateForTriangle(point2D, triangleIndex, point3D))
			{
				if (containingTriangleIndex)
				{
					*containingTriangleIndex = triangleIndex;
				}
				return true;
			}
		}
	}

	return false;
}

bool MeshUVTextureMapping::textureCoordinateTo3DCoordinateForTriangle(const Vector2& point2D, Index32 triangleIndex, Vector3& point3D) const
{
	ocean_assert(triangleIndex < triangles2_.size());

	if (triangleIndex < triangles2_.size())
	{
		const Vector3 barycentricReferencePosition(triangles2_[triangleIndex].cartesian2barycentric(point2D));
		point3D = triangles3_[triangleIndex].barycentric2cartesian(barycentricReferencePosition);

		return triangles2_[triangleIndex].isBarycentricInside(barycentricReferencePosition);
	}

	return false;
}

} // namespace UVTextureMapping

} // namespace Tracking

} // namespace Ocean
