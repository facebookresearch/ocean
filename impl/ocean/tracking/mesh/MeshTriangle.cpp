// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/mesh/MeshTriangle.h"

#include "ocean/base/Subset.h"

#include "ocean/cv/SubRegion.h"

#include "ocean/cv/advanced/AdvancedMotion.h"
#include "ocean/cv/advanced/FrameRectification.h"

#include "ocean/tracking/mesh/Utilities.h"

namespace Ocean
{

namespace Tracking
{

namespace Mesh
{

MeshTriangle::MeshTriangle(const unsigned int numberPyramidLayers) :
	texturePyramidReferencePoints_(numberPyramidLayers),
	previousTexturePyramidLevel_(kInvalidPyramidLevel),
	lastRectificationTimestamp_(true)
{
	// nothing to do, here
}

void MeshTriangle::addTexturePyramidReferencePoint(const Index32 pyramidLayer, const Vector2& referencePoint)
{
	ocean_assert(pyramidLayer < texturePyramidReferencePoints_.size());
	texturePyramidReferencePoints_[pyramidLayer].push_back(referencePoint);
}

void MeshTriangle::distributeAndFilterTexturePyramidReferencePoints(const Triangle2& triangle2)
{
	ocean_assert(triangle2.isValid());

	constexpr Scalar kPointSpacingAtLevelZero = Scalar(32.0);

	Box2 textureSubregion(triangle2);
	Scalar pointSpacing = kPointSpacingAtLevelZero;

	const unsigned int numberHorizontalBins = (textureSubregion.width() > pointSpacing) ? (unsigned int)(textureSubregion.width() / pointSpacing + Scalar(0.5)) : 1u;
	const unsigned int numberVerticalBins = (textureSubregion.height() > pointSpacing) ? (unsigned int)(textureSubregion.height() / pointSpacing + Scalar(0.5)) : 1u;

	for (Vectors2& referencePoints : texturePyramidReferencePoints_)
	{
		if (!referencePoints.empty())
		{
#ifdef OCEAN_DEBUG
			for (const Vector2& referencePoint : referencePoints)
			{
				ocean_assert(textureSubregion.isInside(referencePoint));
			}
#endif // OCEAN_DEBUG

			referencePoints = Geometry::SpatialDistribution::distributeAndFilter(referencePoints.data(), referencePoints.size(), textureSubregion.left(), textureSubregion.top(), textureSubregion.width(), textureSubregion.height(), numberHorizontalBins, numberVerticalBins);
		}

		textureSubregion *= Scalar(0.5);
		pointSpacing *= Scalar(0.5);
	}
}

bool MeshTriangle::rectifyTriangleAndIdentifyReferenceCorrespondences(
	const PinholeCamera& pinholeCamera,
	const Frame& yFrame,
	const CV::FramePyramid& texturePyramid,
	const HomogenousMatrix4& pose_cameraFlipped_T_world,
	const UVTextureMapping::MeshUVTextureMappingRef& meshUVTextureMappingRef,
	const Index32 triangleIndex,
	Worker* worker)
{
	constexpr unsigned int kNumChannels = 1u; // always use grayscale images
	// TODO (jtprice): move this to options
	constexpr unsigned int kNumPyramidLayersForTracking = 3u;

	ocean_assert(pinholeCamera.isValid());
	ocean_assert(yFrame.isValid());
	ocean_assert(pose_cameraFlipped_T_world.isValid());
	ocean_assert(kNumPyramidLayersForTracking <= texturePyramid.layers());

	const Triangles3& triangles3 = meshUVTextureMappingRef->triangles3();
	const Triangles2& uvTriangles2 = meshUVTextureMappingRef->triangles2();

	ocean_assert(triangleIndex < triangles3.size());
	ocean_assert(triangleIndex < uvTriangles2.size());

	if (!pinholeCamera.isValid() || !yFrame.isValid() || !pose_cameraFlipped_T_world.isValid() || kNumPyramidLayersForTracking > texturePyramid.layers() || triangleIndex >= triangles3.size() || triangleIndex >= uvTriangles2.size())
	{
		return false;
	}

	const Triangle3& triangle3 = triangles3[triangleIndex];
	const Triangle2& uvTriangle2 = uvTriangles2[triangleIndex];

	Triangle2 imageTriangle2;

	if (!Utilities::triangleIsVisible(triangle3, pinholeCamera, pose_cameraFlipped_T_world, imageTriangle2))
	{
		previousTexturePyramidLevel_ = kInvalidPyramidLevel;
		return false;
	}

	// For the given triangle, we'll attempt to warp the image region covered by that triangle into
	// the texture space. Rectified matching works best when the pixel resolution of the texture
	// space matches the resolution of the image, so we'll need to select this. Here, we'll simply
	// compute the scale difference based on the area ratio for the UV and projected triangles. If the
	// image-space resolution exceeds the resolution in the UV space, we'll just use the finest UV
	// resolution (scale = 1).

	const Scalar projectedSquaredArea = imageTriangle2.area2();

	constexpr Scalar kMinimumSquaredArea = Scalar(25.0); // TODO (jtprice): could make this, e.g., 100
	if (projectedSquaredArea < kMinimumSquaredArea)
	{
		previousTexturePyramidLevel_ = kInvalidPyramidLevel;
		return false;
	}

	const Scalar squaredAreaRatio = uvTriangle2.area2() / projectedSquaredArea;

	// Ignore this image triangle if it is too low-res. More specifically, we require at least
	// kNumPyramidLayersForTracking pyramid layers to be available, starting with the selected layer.
	// If there aren't enough coarser pyramid layers above the current layer, then the selected layer
	// is too low-res. We perform this check now to also avoid any overflow issues when converting to
	// unsigned int, below.
	ocean_assert(texturePyramid.layers() < 32);
	constexpr Scalar kSqrt2 = Scalar(1.4142135623730950);
	const Scalar maximumScaleRatio = (Scalar)(1u << (texturePyramid.layers() - kNumPyramidLayersForTracking)) * kSqrt2; // multiply by sqrt(2) to allow for rounding, below
	const Scalar maximumAreaRatio = maximumScaleRatio * maximumScaleRatio;
	if (squaredAreaRatio >= maximumAreaRatio * maximumAreaRatio)
	{
		previousTexturePyramidLevel_ = kInvalidPyramidLevel;
		return false; // guard against a bad cast to unsigned int, below
	}

	// Assuming that the pyramid scale space is halved at each level, compute the pyramid level as
	// floor(log_2(s)), where s is the relative scaling factor from the projected triangle to the
	// texture triangle. Note that the squared area ratio corresponds to (s^2)^2 = s^4.
	const unsigned int texturePyramidLayer = squaredAreaRatio > Scalar(1.0) ? Numeric::round32(Scalar(0.25) * Numeric::log2(squaredAreaRatio)) : 0u;

	ocean_assert(texturePyramidLayer <= texturePyramid.layers() - kNumPyramidLayersForTracking);

	// Also rule out any triangle that is very close to oblique.
	const Vector3 cameraPosition = pose_cameraFlipped_T_world.inverted().translation();
	const Vector3 normal = (triangle3.point1() - triangle3.point0()).cross(triangle3.point2() - triangle3.point1()).normalizedOrZero();
	const auto cosineIncidentAngle = [&](const Vector3& point)
	{
		return (cameraPosition - point).normalizedOrZero() * normal;
	};

	constexpr Scalar kMinimumCosineIncidentAngle = Scalar(0.1736); // ~ cos(80 degrees)

	if (cosineIncidentAngle(triangle3.point0()) < kMinimumCosineIncidentAngle || cosineIncidentAngle(triangle3.point1()) < kMinimumCosineIncidentAngle || cosineIncidentAngle(triangle3.point2()) < kMinimumCosineIncidentAngle)
	{
		previousTexturePyramidLevel_ = kInvalidPyramidLevel;
		return false;
	}

	// Ignore this triangle if it was previously processed at this pyramid level.
	constexpr Scalar kMaximumTimeSinceLastRectificationSeconds = Scalar(0.8);
	const Timestamp currentTimestamp(true);

	ocean_assert(lastRectificationTimestamp_.isValid());
	if (double(currentTimestamp - lastRectificationTimestamp_) < kMaximumTimeSinceLastRectificationSeconds && numberTrackedPoints() > 0u && previousTexturePyramidLevel_ == texturePyramidLayer)
	{
		return false;
	}

	previousTexturePyramidLevel_ = texturePyramidLayer;
	lastRectificationTimestamp_ = currentTimestamp;

	// Ignore this triangle if the texture has no reference points at this pyramid level.
	if (texturePyramidReferencePoints_[texturePyramidLayer].empty())
	{
		return false;
	}

	const Scalar pyramidScaleFactor = Scalar(1.0) / (Scalar)(1u << texturePyramidLayer);

	// Warp the image into the UV pattern space for this triangle. When doing this, pad the UV
	// triangle a bit so that feature matching can be applied near the triangle edges. The warped
	// output image will bound this triangle.

	// TODO (jtprice): check the padding amount
	constexpr Scalar kPaddingPixels = Scalar(15.0); // fixed for all pyramid levels

	const Triangle2 scaledUvTriangle2(
		uvTriangle2.point0() * pyramidScaleFactor,
		uvTriangle2.point1() * pyramidScaleFactor,
		uvTriangle2.point2() * pyramidScaleFactor);

	const Triangle2 paddedScaledUvTriangle2 = scaledUvTriangle2.padded(kPaddingPixels);

	// Compute the bounding box for the texture region covered by the padded triangle, taking into
	// account that the padding may go outside of the texture boundary.
	constexpr unsigned int kSSDWindowSize = 7u; // used for SSD matching, below
	constexpr unsigned int kCoarsestLayerSearchRadius = 2u; // used for SSD matching, below
	constexpr unsigned int kLowerBoundPyramidLayerSize = kSSDWindowSize + 2u * kCoarsestLayerSearchRadius - 1u; // exclusive; disallow pyramid levels that are too small

	Vector2 textureSubregionTopLeft;
	unsigned int textureSubregionWidth, textureSubregionHeight;
	{
		int left, top;
		const bool paddedTriangleOverlapsWithTexture = Box2(paddedScaledUvTriangle2).box2integer(
			0, // constraintLeft
			0, // constraintTop
			(int)(texturePyramid.layer(texturePyramidLayer).width()) - 1,
			(int)(texturePyramid.layer(texturePyramidLayer).height()) - 1,
			left,
			top,
			textureSubregionWidth,
			textureSubregionHeight);

		ocean_assert(paddedTriangleOverlapsWithTexture);
		ocean_assert(left >= 0); // if false, there's a bug in box2integer
		ocean_assert(top >= 0);
		ocean_assert(textureSubregionWidth > 0u);
		ocean_assert(textureSubregionHeight > 0u);

		// If there's no overlap, there must be an incorrect triangle in the user-defined texture
		// coordinates. Otherwise, if the width or height is too small, then the projected triangle is
		// probably just too small to be useful.
		if (!paddedTriangleOverlapsWithTexture || CV::FramePyramid::idealLayers(textureSubregionWidth, textureSubregionHeight, kLowerBoundPyramidLayerSize) < kNumPyramidLayersForTracking)
		{
			return false;
		}

		textureSubregionTopLeft = Vector2((Scalar)left, (Scalar)top);
	}

	// Create the rectified output image and its mask. The rectified frame will have the same
	// dimensions as the texture region covered by the padded triangle.
	Frame rectifiedFrame(FrameType(textureSubregionWidth, textureSubregionHeight, FrameType::FORMAT_Y8, yFrame.pixelOrigin()));
	Frame rectifiedFrameMask(rectifiedFrame.frameType());

	rectifiedFrame.setValue(0x00u);
	rectifiedFrameMask.setValue(0x00u);

	// Apply rectification, mapping the image into the texture space at the appropriate resolution.
	const Vector3 paddedBarycentricCoordinate0 = scaledUvTriangle2.cartesian2barycentric(paddedScaledUvTriangle2.point0());
	const Vector3 paddedBarycentricCoordinate1 = scaledUvTriangle2.cartesian2barycentric(paddedScaledUvTriangle2.point1());
	const Vector3 paddedBarycentricCoordinate2 = scaledUvTriangle2.cartesian2barycentric(paddedScaledUvTriangle2.point2());
	const Vector3 paddedTrianglePoint0 = triangle3.barycentric2cartesian(paddedBarycentricCoordinate0);
	const Vector3 paddedTrianglePoint1 = triangle3.barycentric2cartesian(paddedBarycentricCoordinate1);
	const Vector3 paddedTrianglePoint2 = triangle3.barycentric2cartesian(paddedBarycentricCoordinate2);
	const Triangle3 paddedTriangle3(paddedTrianglePoint0, paddedTrianglePoint1, paddedTrianglePoint2);

	// TODO (jtprice): investigate making this a minimum of 4; and consider if there are better ways of
	// computing this value
	const unsigned int lookupTableBinSize = minmax(1u, (unsigned int)(std::min(textureSubregionWidth, textureSubregionHeight) / 4u), 50u);
	const unsigned char kMaskValue = 0xFF;
	Frame continuousYFrame(yFrame, Frame::ACM_USE_KEEP_LAYOUT);
	continuousYFrame.makeContinuous();
	rectifiedFrame.makeContinuous();
	rectifiedFrameMask.makeContinuous();
	CV::Advanced::FrameRectification::triangleObjectMaskIF8BitPerChannel<kNumChannels>(continuousYFrame.constdata<uint8_t>(), continuousYFrame.paddingElements(), continuousYFrame.pixelOrigin(), AnyCameraPinhole(pinholeCamera), pose_cameraFlipped_T_world, paddedScaledUvTriangle2 - textureSubregionTopLeft, paddedTriangle3, rectifiedFrame.data<uint8_t>(), rectifiedFrameMask.data<uint8_t>(), textureSubregionWidth, textureSubregionHeight, rectifiedFrame.paddingElements(), rectifiedFrameMask.paddingElements(), worker, kMaskValue, lookupTableBinSize);
	ocean_assert(rectifiedFrame.width() == textureSubregionWidth);
	ocean_assert(rectifiedFrame.height() == textureSubregionHeight);

	// Run feature matching between the rectified image and the local crop of the UV texture.

	const CV::FramePyramid rectifiedPyramid(rectifiedFrame, kNumPyramidLayersForTracking, false /*copyFirstLayer*/, worker);

	// Choose only one reference point in each local region of the UV texture, after filtering
	// points based on visibility.
	Vectors2 texturePoints;
	{
		const CV::SubRegion maskSubRegion(rectifiedFrameMask, CV::PixelBoundingBox(), 0xFF);

		const Vectors2& candidateTexturePoints = texturePyramidReferencePoints_[texturePyramidLayer];
		texturePoints.reserve(candidateTexturePoints.size());
		for (const Vector2& point : candidateTexturePoints)
		{
			// Check that the point lies within the mask created from the padded triangle (allowing
			// us to rule out points that are outside of the camera frame).
			if (maskSubRegion.isInside(point - textureSubregionTopLeft))
			{
				texturePoints.emplace_back(point);
			}
		}
	}

	if (texturePoints.empty())
	{
		return false;
	}

	Vectors2 texturePointsInRectifiedFrame(texturePoints);
	for (Vector2& point : texturePointsInRectifiedFrame)
	{
		point -= textureSubregionTopLeft; // place relative to the top-left corner of the rectified image
		ocean_assert(point.x() > 0 && point.y() > 0 && point.x() < textureSubregionWidth && point.y() < textureSubregionHeight);
	}

	// Maximum round-trip error in bidirectional point matching.
	constexpr Scalar kMaximumSquaredErrorAtLevel0 = Scalar(8.0 * 8.0);
	const Scalar maximumSquaredError = kMaximumSquaredErrorAtLevel0 * pyramidScaleFactor;

	// Find corresponding rectified points given the rough locations of texturePointsInRectifiedFrame.
	Vectors2 rectifiedPoints;
	Indices32 validPointIndices;
	const bool matchingResult = CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<kNumChannels, kSSDWindowSize>(
		CV::FramePyramid::create8BitPerChannel<false>(texturePyramid, texturePyramidLayer, rectifiedPyramid.layers()),
		rectifiedPyramid,
		kCoarsestLayerSearchRadius,
		texturePoints,
		texturePointsInRectifiedFrame,
		rectifiedPoints,
		maximumSquaredError,
		worker,
		&validPointIndices,
		2u); // subPixelIterations

	// For each found point in the rectified image, compute (1) the 3D point associated with the
	// matched 2D texture point, (2) the 3D point associated with the 2D rectified point, and (3)
	// the projection of this second 3D point back into the image based on the current pose estimate.
	// (Note that the rectification also uses this pose estimate, so we're really just applying the
	// inverse homography.)
	if (!matchingResult || validPointIndices.empty())
	{
		return false;
	}

	texturePoints = Subset::subset(texturePoints, validPointIndices);
	rectifiedPoints = Subset::subset(rectifiedPoints, validPointIndices);

	ocean_assert(texturePoints.size() == rectifiedPoints.size());

	Vectors3 newObjectPoints;
	Vectors2 newImagePoints;
	newObjectPoints.reserve(texturePoints.size());
	newImagePoints.reserve(texturePoints.size());

	const Scalar inversePyramidScaleFactor = (Scalar)(1u << texturePyramidLayer);

	for (unsigned int i = 0; i < texturePoints.size(); ++i)
	{
		// Convert to finest-resolution texture coordinates before mapping to 3D.
		const Vector2 rectifiedPoint =
			(rectifiedPoints[i] + textureSubregionTopLeft) * inversePyramidScaleFactor;
		Vector3 rectifiedObjectPoint;

		if (!meshUVTextureMappingRef->textureCoordinateTo3DCoordinateForTriangle(rectifiedPoint, triangleIndex, rectifiedObjectPoint))
		{
			continue; // discard points that are near the triangle but ultimately outside of it
		}

		const Vector2 texturePoint = texturePoints[i] * inversePyramidScaleFactor;
		Vector3 point3D;

		if (!meshUVTextureMappingRef->textureCoordinateTo3DCoordinateForTriangle(texturePoint, triangleIndex, point3D))
		{
			continue; // in rare cases, the reference texture point can be moved outside of the triangle, as well
		}

		const Vector2 imagePoint = pinholeCamera.projectToImageIF<true>(pose_cameraFlipped_T_world, rectifiedObjectPoint, pinholeCamera.hasDistortionParameters());

		if (pinholeCamera.isInside(imagePoint))
		{
			newObjectPoints.push_back(point3D);
			newImagePoints.push_back(imagePoint);
		}
	}

	ocean_assert(newObjectPoints.size() == newImagePoints.size());

	// If there are no points that were added, we'll ignore the update and simply fall back to the
	// points that were already being tracked.
	if (newObjectPoints.size() == 0)
	{
		return false;
	}

	objectPoints_ = std::move(newObjectPoints);
	imagePoints_ = std::move(newImagePoints);

	return true;
}

} // namespace Mesh

} // namespace Tracking

} // namespace Ocean
