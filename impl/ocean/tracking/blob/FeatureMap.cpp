// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "ocean/tracking/blob/FeatureMap.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/cv/detector/blob/BlobFeatureDetector.h"
#include "ocean/cv/detector/blob/BlobFeatureDescriptor.h"

#include "ocean/geometry/Homography.h"

#include <algorithm>
#include <array>

namespace Ocean
{

namespace Tracking
{

namespace Blob
{

using namespace CV;
using namespace CV::Detector;
using namespace CV::Detector::Blob;

FeatureMap::FeatureMap()
{
	// nothing to do here
}

FeatureMap::FeatureMap(const BlobFeatures& features) :
	mapFeatures(features)
{
	Scalar xLower = Numeric::maxValue();
	Scalar yLower = Numeric::maxValue();
	Scalar zLower = Numeric::maxValue();

	Scalar xHigher = Numeric::minValue();
	Scalar yHigher = Numeric::minValue();
	Scalar zHigher = Numeric::minValue();

	for (BlobFeatures::const_iterator i = mapFeatures.begin(); i != mapFeatures.end(); ++i)
	{
		xLower = min(xLower, i->position().x());
		yLower = min(yLower, i->position().y());
		zLower = min(zLower, i->position().z());

		xHigher = max(xHigher, i->position().x());
		yHigher = max(yHigher, i->position().y());
		zHigher = max(zHigher, i->position().z());
	}

	mapBoundingBox = Box3(Vector3(xLower, yLower, zLower), Vector3(xHigher, yHigher, zHigher));

	if (mapBoundingBox.isPlanar(mapPlane))
	{
		Log::info() << "The Blob feature map is planar";
	}
	else
	{
		Log::info() << "The Blob feature map covers a 3D space.";
	}
}

FeatureMap::FeatureMap(const Frame& frame, const Vector2& dimension, const Scalar threshold, const bool frameIsUndistorted, const unsigned int maxFeatures, Worker* worker)
{
	ocean_assert(frame &&  dimension.x() > 0);

	Frame yFrame;
	if (!FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		Log::error() << "Failed to create a new Blob feature map: Not supported pixel format.";
		return;
	}

	create(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), dimension, threshold, frameIsUndistorted, maxFeatures, worker);
}

FeatureMap::FeatureMap(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, const Scalar threshold, const bool frameIsUndistorted, const unsigned int maxFeatures, Worker* worker)
{
	ocean_assert(yFrame && width >= 1u && height >= 1u);
	create(yFrame, width, height, yFramePaddingElements, dimension, threshold, frameIsUndistorted, maxFeatures, worker);
}

FeatureMap::FeatureMap(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const UVTextureMapping::CylinderUVTextureMapping& cylinderUVTextureMapping, const Scalar threshold, const unsigned int maxFeatures, Worker* worker) :
	cylinderUVTextureMapping_(cylinderUVTextureMapping)
{
	ocean_assert(yFrame && width >= 1u && height >= 1u);
	ocean_assert(threshold > 0);
	ocean_assert(cylinderUVTextureMapping.isValid());

	if (yFrame == nullptr || width == 0u || height == 0u || threshold <= 0 || !cylinderUVTextureMapping.isValid())
	{
		return;
	}

	// Extract features.
	mapFeatures.clear();

	Frame integralImagePatternFrame(FrameType(width + 1u, height + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
	IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame, integralImagePatternFrame.data<uint32_t>(), width, height, yFramePaddingElements, integralImagePatternFrame.paddingElements());

	ocean_assert(integralImagePatternFrame.isContinuous());

	BlobFeatureDetector::detectFeatures(integralImagePatternFrame.constdata<uint32_t>(), width, height, BlobFeatureDetector::SAMPLING_NORMAL, threshold, true, mapFeatures, worker);

	//
	// Now, compute a 3D point on the surface of the cylinder for each 2D point, and throw out invalid points.
	// NOTE (jtprice): We could also check that the features do not lie on the cylinder boundary.
	//

	{
		size_t i = 0u;
		while (i < mapFeatures.size())
		{
			Vector3 point3D;
			if (cylinderUVTextureMapping.textureCoordinateTo3DCoordinate(mapFeatures[i].observation(), point3D))
			{
				mapFeatures[i++].setPosition(point3D);
			}
			else
			{
				ocean_assert(false && "This should not happen for cylinder patterns!");
				mapFeatures[i] = mapFeatures.back(); // simply move the last element to the current
				mapFeatures.pop_back();
			}
		}
	}

	// Compute descriptors for valid points.

	if (maxFeatures != 0 && mapFeatures.size() > maxFeatures)
	{
		std::sort(mapFeatures.begin(), mapFeatures.end());
		mapFeatures.resize(maxFeatures);
	}

	BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralImagePatternFrame.constdata<uint32_t>(), width, height, FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, mapFeatures, true, worker);

	// Set the 3D bounding box to entirely surround the cylinder segment in the given coordinate frame.
	const Scalar radius = cylinderUVTextureMapping.radius();
	const Scalar cylinderHeight = cylinderUVTextureMapping.height();
	const Scalar yOffset = cylinderUVTextureMapping.yOffset();
	const Scalar zOffset = cylinderUVTextureMapping.zOffset();
	mapBoundingBox = Box3(
		Vector3(-radius, -radius + yOffset, Scalar(-0.5) * cylinderHeight + zOffset),
		Vector3(radius, radius + yOffset, Scalar(0.5) * cylinderHeight + zOffset));
	ocean_assert(mapBoundingBox.isValid());

	// Create a polygon of the cylinder surface.
	constexpr size_t kNumTriangleStrips = 18;
	Vector3 rightBottomPoint(radius, yOffset, Scalar(0.5) * cylinderHeight + zOffset);
	Vector3 rightTopPoint(radius, yOffset, Scalar(-0.5) * cylinderHeight + zOffset);

	constexpr Scalar angleStep = Numeric::pi2() / Scalar(kNumTriangleStrips);
	triangles3_.reserve(2 * kNumTriangleStrips);

	for (size_t i = 1u; i <= kNumTriangleStrips; ++i)
	{
		const Scalar x = Numeric::cos(Scalar(i) * angleStep);
		const Scalar y = Numeric::sin(Scalar(i) * angleStep);
		const Vector3 leftBottomPoint(radius * x, radius * y + yOffset, rightBottomPoint.z());
		const Vector3 leftTopPoint(radius * x, radius * y + yOffset, rightTopPoint.z());

		triangles3_.emplace_back(leftTopPoint, rightBottomPoint, rightTopPoint);
		ocean_assert(triangles3_.back().isValid());

		triangles3_.emplace_back(rightBottomPoint, leftTopPoint, leftBottomPoint);
		ocean_assert(triangles3_.back().isValid());

		rightBottomPoint = leftBottomPoint;
		rightTopPoint = leftTopPoint;
	}

	// Finally, set the canonical shape of the cylinder.
	mapCylinder = Cylinder3(
		Vector3(0, yOffset, zOffset),
		Vector3(0, 0, -1),
		radius,
		Scalar(-0.5) * cylinderHeight,
		Scalar(0.5) * cylinderHeight);
	ocean_assert(mapCylinder.isValid());
}

FeatureMap::FeatureMap(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const UVTextureMapping::ConeUVTextureMapping& coneUVTextureMapping, const Scalar threshold, const unsigned int maxFeatures, Worker* worker) :
	coneUVTextureMapping_(coneUVTextureMapping)
{
	ocean_assert(yFrame && width >= 1u && height >= 1u);
	ocean_assert(threshold > 0);
	ocean_assert(coneUVTextureMapping.isValid());

	if (yFrame == nullptr || width == 0u || height == 0u || threshold <= 0 || !coneUVTextureMapping.isValid())
	{
		return;
	}

	// Extract features.
	mapFeatures.clear();

	Frame integralImagePatternFrame(FrameType(width + 1u, height + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
	IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame, integralImagePatternFrame.data<uint32_t>(), width, height, yFramePaddingElements, integralImagePatternFrame.paddingElements());

	ocean_assert(integralImagePatternFrame.isContinuous());

	BlobFeatureDetector::detectFeatures(integralImagePatternFrame.constdata<uint32_t>(), width, height, BlobFeatureDetector::SAMPLING_NORMAL, threshold, true, mapFeatures, worker);

	//
	// Now, compute a 3D point on the surface of the cone for each 2D point, and throw out invalid points.
	// NOTE (jtprice): We could also check that the features do not lie on the cone boundary.
	//

	{
		size_t i = 0u;
		while (i < mapFeatures.size())
		{
			Vector3 point3D;
			if (coneUVTextureMapping.textureCoordinateTo3DCoordinate(mapFeatures[i].observation(), point3D))
			{
				mapFeatures[i++].setPosition(point3D);
			}
			else
			{
				mapFeatures[i] = mapFeatures.back(); // simply move the last element to the current
				mapFeatures.pop_back();
			}
		}
	}

	// Compute descriptors for valid points.

	if (maxFeatures != 0 && mapFeatures.size() > maxFeatures)
	{
		std::sort(mapFeatures.begin(), mapFeatures.end());
		mapFeatures.resize(maxFeatures);
	}

	BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralImagePatternFrame.constdata<uint32_t>(), width, height, FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, mapFeatures, true, worker);

	// Set the 3D bounding box to entirely surround the cone segment in the given coordinate frame.
	const Scalar largerRadius = coneUVTextureMapping.largerRadius();
	const Scalar smallerRadius = coneUVTextureMapping.smallerRadius();
	const Scalar coneHeight = coneUVTextureMapping.height();
	const Scalar yOffset = coneUVTextureMapping.yOffset();
	const Scalar zOffset = coneUVTextureMapping.zOffset();
	mapBoundingBox = Box3(
		Vector3(-largerRadius, -largerRadius + yOffset, Scalar(-0.5) * coneHeight + zOffset),
		Vector3(largerRadius, largerRadius + yOffset, Scalar(0.5) * coneHeight + zOffset));
	ocean_assert(mapBoundingBox.isValid());

	// Create a polygon of the cone surface. Truncated cones are approximated by rectangles, and
	// non-truncated cones by triangles all connected at the apex.
	constexpr size_t kNumTriangleStrips = 18;
	Vector3 rightBottomPoint(smallerRadius, yOffset, Scalar(0.5) * coneHeight + zOffset);
	Vector3 rightTopPoint(largerRadius, yOffset, Scalar(-0.5) * coneHeight + zOffset);

	constexpr Scalar angleStep = Numeric::pi2() / Scalar(kNumTriangleStrips);
	triangles3_.reserve((smallerRadius > Scalar(0)) ? 2 * kNumTriangleStrips : kNumTriangleStrips);
	for (size_t i = 1u; i <= kNumTriangleStrips; ++i)
	{
		const Scalar x = Numeric::cos(Scalar(i) * angleStep);
		const Scalar y = Numeric::sin(Scalar(i) * angleStep);
		const Vector3 leftTopPoint(largerRadius * x, largerRadius * y + yOffset, rightTopPoint.z());
		triangles3_.emplace_back(leftTopPoint, rightBottomPoint, rightTopPoint);
		ocean_assert(triangles3_.back().isValid());

		if (smallerRadius > Scalar(0)) // truncated cone
		{
			const Vector3 leftBottomPoint(smallerRadius * x, smallerRadius * y + yOffset, rightBottomPoint.z());
			triangles3_.emplace_back(rightBottomPoint, leftTopPoint, leftBottomPoint);
			ocean_assert(triangles3_.back().isValid());
			rightBottomPoint = leftBottomPoint;
		}

		rightTopPoint = leftTopPoint;
	}

	// Finally, set the canonical shape of the cone.
	// Note that the Ocean::Math::Cone implementation internally places the apex at the 3D origin, so we'll need to account for this with our coordinate frame in the middle.
	ocean_assert(largerRadius > smallerRadius);
	const Scalar minSignedDistanceAlongAxis = smallerRadius / (largerRadius - smallerRadius) * coneHeight; // distance to the apex, computed via similar triangles
	const Scalar maxSignedDistanceAlongAxis = coneHeight + minSignedDistanceAlongAxis;
	const Scalar apexAngle = Scalar(2.) * Numeric::atan(largerRadius / maxSignedDistanceAlongAxis);
	mapCone = Cone3(
		Vector3(0., yOffset, minSignedDistanceAlongAxis + Scalar(0.5) * coneHeight + zOffset),
		Vector3(0., 0., -1.),
		apexAngle,
		minSignedDistanceAlongAxis,
		maxSignedDistanceAlongAxis);
	ocean_assert(mapCone.isValid());
}

FeatureMap::FeatureMap(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const UVTextureMapping::MeshUVTextureMappingRef& meshUVTextureMapping, const Scalar threshold, const unsigned int maxFeatures, Worker* worker) :
	meshUVTextureMapping_(meshUVTextureMapping)
{
	ocean_assert(yFrame && width >= 1u && height >= 1u);
	ocean_assert(threshold > 0);
	ocean_assert(meshUVTextureMapping);
	ocean_assert(meshUVTextureMapping->isValid());

	if (yFrame == nullptr || width == 0u || height == 0u || threshold <= Scalar(0) || !meshUVTextureMapping || !meshUVTextureMapping->isValid())
	{
		return;
	}

	// Extract features.
	mapFeatures.clear();

	Frame integralImagePatternFrame(FrameType(width + 1u, height + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
	IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame, integralImagePatternFrame.data<uint32_t>(), width, height, yFramePaddingElements, integralImagePatternFrame.paddingElements());

	BlobFeatureDetector::detectFeatures(integralImagePatternFrame.constdata<uint32_t>(), width, height, BlobFeatureDetector::SAMPLING_NORMAL, threshold, true, mapFeatures, worker);

	//
	// Now, compute a 3D point on the surface of the mesh for each 2D point, and throw out invalid points.
	//

	triangleIndexPerFeature_.reserve(mapFeatures.size());

	{
		size_t i = 0u;
		while (i < mapFeatures.size())
		{
			Index32 triangleIndex;
			Vector3 point3D;
			if (meshUVTextureMapping->textureCoordinateTo3DCoordinate(mapFeatures[i].observation(), point3D, &triangleIndex))
			{
				ocean_assert(triangleIndex < meshUVTextureMapping->triangles3().size());

				mapFeatures[i++].setPosition(point3D);
				triangleIndexPerFeature_.push_back(triangleIndex);
			}
			else
			{
				mapFeatures[i] = mapFeatures.back(); // simply move the last element to the current
				mapFeatures.pop_back();
			}
		}

		ocean_assert(mapFeatures.size() == triangleIndexPerFeature_.size());
	}

	// Compute descriptors for valid points.

	if (maxFeatures != 0 && mapFeatures.size() > maxFeatures)
	{
		std::sort(mapFeatures.begin(), mapFeatures.end());
		mapFeatures.resize(maxFeatures);
	}

	BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralImagePatternFrame.constdata<uint32_t>(), width, height, FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, mapFeatures, true, worker);

	mapBoundingBox = meshUVTextureMapping->boundingBox();
}

Box2 FeatureMap::projectToImagePlane(const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera)
{
	const HomogenousMatrix4 flipped(PinholeCamera::flippedTransformationRightSide(pose));
	const HomogenousMatrix4 iFlipped(flipped.inverted());

	return projectToImagePlaneIF(iFlipped, pinholeCamera);
}

bool FeatureMap::projectToImagePlane(const HomogenousMatrix4& pose, const PinholeCamera& pinholeCamera, const Scalar boundary, int& left, int& top, unsigned int& width, unsigned int& height, unsigned int& leftClamped, unsigned int& topClamped, unsigned int& widthClamped, unsigned int& heightClamped)
{
	const Box2 projectedBox(projectToImagePlane(pose, pinholeCamera));
	if (!projectedBox.isValid())
	{
		return false;
	}

	left = int(projectedBox.lower().x() - boundary);
	top = int(projectedBox.lower().y() - boundary);

	int right = int(projectedBox.higher().x() + boundary);
	int bottom = int(projectedBox.higher().y() + boundary);

	if (left > right || top > bottom)
	{
		ocean_assert(false && "This should never happen");
		return false;
	}

	leftClamped = (unsigned int)max(0, left);
	unsigned int rightClamped = max(leftClamped, (unsigned int)min((int)(pinholeCamera.width() - 1), right));

	topClamped = (unsigned int)max(0, top);
	unsigned int bottomClamped = max(topClamped, (unsigned int)min((int)(pinholeCamera.height() - 1), bottom));

	ocean_assert(left <= right);
	ocean_assert(top <= bottom);

	ocean_assert(rightClamped < (unsigned int)(pinholeCamera.width()));
	ocean_assert(bottomClamped < (unsigned int)(pinholeCamera.height()));

	ocean_assert(leftClamped <= rightClamped);
	ocean_assert(topClamped <= bottomClamped);

	width = right - left;
	height = bottom - top;

	widthClamped = rightClamped - leftClamped;
	heightClamped = bottomClamped - topClamped;
	return true;
}

Box2 FeatureMap::projectToImagePlaneIF(const HomogenousMatrix4& iFlippedPose, const PinholeCamera& pinholeCamera)
{
	if (!mapBoundingBox.isValid())
	{
		return Box2();
	}

	ocean_assert(iFlippedPose.isValid());

	return pinholeCamera.projectToImageIF<true>(iFlippedPose, mapBoundingBox, false);
}

bool FeatureMap::projectToImagePlaneIF(const HomogenousMatrix4& iFlippedPose, const PinholeCamera& pinholeCamera, const Scalar boundary, unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height)
{
	const Box2 projectedBox(projectToImagePlaneIF(iFlippedPose, pinholeCamera));
	if (!projectedBox.isValid())
	{
		return false;
	}

	const int boxLeft = max(0, int(projectedBox.lower().x() - boundary));
	const int boxRight = min(int(pinholeCamera.width() - 1), int(projectedBox.higher().x() + boundary));

	const int boxTop = max(0, int(projectedBox.lower().y() - boundary));
	const int boxBottom = min(int(pinholeCamera.height() - 1), int(projectedBox.higher().y() + boundary));

	if (boxLeft > boxRight || boxTop > boxBottom)
	{
		return false;
	}

	left = (unsigned int)(boxLeft);
	top = (unsigned int)(boxTop);
	width = (unsigned int)(boxRight - boxLeft + 1);
	height = (unsigned int)(boxBottom - boxTop + 1);

	return true;
}

bool FeatureMap::createCubeFeatureMap(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingelements, const Scalar cubeSize, BlobFeatures& blobFeatures, const Scalar threshold, const unsigned int maxFeaturesPerFace, Worker* worker)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width > 0u && height > 0u);
	ocean_assert(cubeSize > 0 && threshold > 0);

	if (width % 3u != 0u || height % 4u != 0u || width * 4u / 3u != height)
	{
		return false;
	}

	/**
	 *  -----------------------------
	 * |         |  back   |         |
	 *  -----------------------------
	 * |  left   |   top   |  right  |
	 *  -----------------------------
	 * |         |  front  |         |
	 *  -----------------------------
	 * |         | bottom  |         |
	 *  -----------------------------
	 */

	const unsigned int faceFrameSize = width / 3u;
	ocean_assert(faceFrameSize * 4u == height);

	// we extract the individual faces in the following order:
	// back, left, top, right, front, bottom
	const std::array<IndexPair32, 6> faceFrameTopLefts =
	{{
		IndexPair32(faceFrameSize, 0u), // back
		IndexPair32(0u, faceFrameSize), // left
		IndexPair32(faceFrameSize, faceFrameSize), // top
		IndexPair32(faceFrameSize * 2u, faceFrameSize), // right
		IndexPair32(faceFrameSize, faceFrameSize * 2u), // front
		IndexPair32(faceFrameSize, faceFrameSize * 3u) // bottom
	}};

	const std::array<SquareMatrix3, 6> faceRotations =
	{{
		SquareMatrix3(Rotation(1, 0, 0, -Numeric::pi_2())), // back
		SquareMatrix3(Rotation(0, 0, 1, Numeric::pi_2())), // left
		SquareMatrix3(true), // top
		SquareMatrix3(Rotation(0, 0, 1, -Numeric::pi_2())), // right
		SquareMatrix3(Rotation(1, 0, 0, Numeric::pi_2())), // front
		SquareMatrix3(Rotation(1, 0, 0, Numeric::pi())) // bottom
	}};

	const Scalar observation2Position = cubeSize / Scalar(faceFrameSize);

	const Frame yFrameInternal(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, Frame::CM_USE_KEEP_LAYOUT, yFramePaddingelements);

	Frame yReusableWarpedFaceFrame(FrameType(yFrameInternal, faceFrameSize, faceFrameSize));
	Frame reusableIntegralImageFaceFrame(FrameType(yReusableWarpedFaceFrame.width() + 1u, yReusableWarpedFaceFrame.height() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));

	BlobFeatures faceFeatures, faceFeaturesPerspective;
	blobFeatures.clear();

	for (unsigned int f = 0u; f < 6u; ++f)
	{
		const unsigned int faceFrameLeft = faceFrameTopLefts[f].first;
		const unsigned int faceFrameTop = faceFrameTopLefts[f].second;

		const Frame yFaceFrame(yFrameInternal.subFrame(faceFrameLeft, faceFrameTop, faceFrameSize, faceFrameSize, Frame::CM_USE_KEEP_LAYOUT));

		const SquareMatrix3& faceRotation = faceRotations[f];

		faceFeatures.clear();

		for (unsigned int n = 0u; n < 5u; ++n)
		{
			faceFeaturesPerspective.clear();

			if (n == 0u)
			{
				ocean_assert(reusableIntegralImageFaceFrame.isContinuous());
				determineAndDescribeFeatures(yFaceFrame.constdata<uint8_t>(), yFaceFrame.width(), yFaceFrame.height(), yFaceFrame.paddingElements(), faceFeaturesPerspective, threshold, true, reusableIntegralImageFaceFrame.data<uint32_t>(), worker);
			}
			else
			{
				ocean_assert(yReusableWarpedFaceFrame.isContinuous());
				ocean_assert(reusableIntegralImageFaceFrame.isContinuous());
				determineAndDescribeFeaturesForPerspective(Numeric::pi_2() * Scalar(n - 1u), Numeric::pi_4(), Vector2(cubeSize, cubeSize), yFaceFrame.constdata<uint8_t>(), yFaceFrame.paddingElements(), yFaceFrame.width(), yFaceFrame.height(), faceFeaturesPerspective, threshold, yReusableWarpedFaceFrame.data<uint8_t>(), reusableIntegralImageFaceFrame.data<uint32_t>(), worker);
			}

			for (BlobFeatures::iterator i = faceFeaturesPerspective.begin(); i != faceFeaturesPerspective.end(); ++i)
			{
				const Vector2 observation = i->observation() * observation2Position - Vector2(cubeSize, cubeSize) * Scalar(0.5);

				const Vector3 facePosition(observation.x(), cubeSize * Scalar(0.5), observation.y());
				const Vector3 worldPoition(faceRotation * facePosition);

				i->setPosition(worldPoition);
			}

			faceFeatures.insert(faceFeatures.end(), faceFeaturesPerspective.begin(), faceFeaturesPerspective.end());
		}

		size_t faceFeaturesToUse = faceFeatures.size();

		if (maxFeaturesPerFace != 0u && maxFeaturesPerFace < faceFeatures.size())
		{
			std::sort(faceFeatures.begin(), faceFeatures.end());
			faceFeaturesToUse = size_t(maxFeaturesPerFace);
		}

		blobFeatures.insert(blobFeatures.end(), faceFeatures.begin(), faceFeatures.begin() + faceFeaturesToUse);
	}

	// finally we want all features having the same priority
	std::sort(blobFeatures.begin(), blobFeatures.end());

	return true;
}

bool FeatureMap::create(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const Vector2& dimension, const Scalar threshold, const bool frameIsUndistorted, const unsigned int maxFeatures, Worker* worker)
{
	mapFeatures.clear();
	mapBoundingBox.clear();
	mapPlane = Plane3();

	ocean_assert(yFrame && width >= 1u && height >= 1u && dimension.x() > 0);
	if (yFrame == nullptr || width == 0u || height == 0u || dimension.x() <= 0)
	{
		return false;
	}

	Frame integralImagePatternFrame(FrameType(width + 1u, height + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
	IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame, integralImagePatternFrame.data<uint32_t>(), width, height, yFramePaddingElements, integralImagePatternFrame.paddingElements());

	ocean_assert(integralImagePatternFrame.isContinuous());

	BlobFeatureDetector::detectFeatures(integralImagePatternFrame.constdata<uint32_t>(), width, height, BlobFeatureDetector::SAMPLING_NORMAL, threshold, frameIsUndistorted, mapFeatures, worker);

	if (maxFeatures != 0 && mapFeatures.size() > maxFeatures)
	{
		std::sort(mapFeatures.begin(), mapFeatures.end());
		mapFeatures.resize(maxFeatures);
	}

	BlobFeatureDescriptor::calculateOrientationsAndDescriptors(integralImagePatternFrame.constdata<uint32_t>(), width, height, FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, mapFeatures, true, worker);

	const Scalar factorX = dimension.x() / Scalar(width);
	const Scalar factorY = dimension.y() > 0 ? dimension.y() / Scalar(height) : factorX;

	for (BlobFeatures::iterator i = mapFeatures.begin(); i != mapFeatures.end(); ++i)
	{
		i->setPosition(Vector3(i->observation().x() * factorX, 0, i->observation().y() * factorY));
	}

	mapBoundingBox = Box3(Vector3(0, 0, 0), Vector3(Scalar(width) * factorX, 0, Scalar(height) * factorY));

	mapPlane = Plane3(Vector3(0, 0, 0), Vector3(0, 1, 0));

	return true;
}

void FeatureMap::determineAndDescribeFeatures(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElemnents, CV::Detector::Blob::BlobFeatures& blobFeatures, const Scalar threshold, const bool frameIsUndistorted, uint32_t* reusableIntegralFrame, Worker* worker)
{
	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(threshold > 0 && blobFeatures.empty());
	ocean_assert(reusableIntegralFrame != nullptr);

	constexpr unsigned int reusableIntegralFramePaddingElements = 0u;

	IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame, reusableIntegralFrame, width, height, yFramePaddingElemnents, reusableIntegralFramePaddingElements);

	BlobFeatureDetector::detectFeatures(reusableIntegralFrame, width, height, BlobFeatureDetector::SAMPLING_NORMAL, threshold, frameIsUndistorted, blobFeatures, worker);

	BlobFeatureDescriptor::calculateOrientationsAndDescriptors(reusableIntegralFrame, width, height, FrameType::ORIGIN_UPPER_LEFT, BlobFeature::ORIENTATION_SLIDING_WINDOW, blobFeatures, true, worker);
}

bool FeatureMap::determineAndDescribeFeaturesForPerspective(const Scalar yAngle, const Scalar xAngle, const Vector2& dimension, const uint8_t* yFrame, const unsigned int yFramePaddingElements, const unsigned int width, const unsigned int height, BlobFeatures& blobFeatures, const Scalar threshold, uint8_t* resuableWarpedFrame, uint32_t* reusableIntegralFrame, Worker* worker)
{
	ocean_assert(yAngle >= 0 && yAngle < Numeric::pi2());
	ocean_assert(xAngle > 0 && xAngle < Numeric::pi_2());
	ocean_assert(dimension.x() > 0 && dimension.y() > 0);

	ocean_assert(yFrame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(threshold > 0 && blobFeatures.empty());
	ocean_assert(reusableIntegralFrame != nullptr);

	// **NOTE** we should improve the definition of the camera (resolution, fovx, translation, etc.)

	const PinholeCamera perfectCamera(width, height, Numeric::deg2rad(60));
	const Vector3 translationCamera(0, max(dimension.x(), dimension.y()), dimension.y() * Scalar(0.25));

	// we define a top pose of the camera above the tracking pattern and looking downwards
	const HomogenousMatrix4 topPose(translationCamera, Rotation(1, 0, 0, -Numeric::pi_2()));

	// now we can rotate the camera
	const HomogenousMatrix4 perspectivePose(HomogenousMatrix4(Rotation(0, 1, 0, yAngle) * Rotation(1, 0, 0, xAngle)) * topPose);

	// we calculate the homography between 'flat' pattern and the perspectively warped pattern
	const SquareMatrix3 perspectiveHomographyFlat = Geometry::Homography::homographyMatrix(perfectCamera, perspectivePose, Vector3(-dimension.x() * Scalar(0.5), 0, -dimension.y() * Scalar(0.5)), Vector3(-dimension.x() * Scalar(0.5), 0, dimension.y() * Scalar(0.5)), Vector3(dimension.x() * Scalar(0.5), 0, -dimension.y() * Scalar(0.5)), width, height);

	SquareMatrix3 flatHomographyPerspective;
	if (!perspectiveHomographyFlat.invert(flatHomographyPerspective))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	constexpr unsigned int resuableWarpedFramePaddingElements = 0u;

	const uint8_t white = 0xFF;
	CV::FrameInterpolatorBilinear::homography<uint8_t, 1u>(yFrame, width, height, flatHomographyPerspective, &white, resuableWarpedFrame, CV::PixelPositionI(0, 0), width, height, yFramePaddingElements, resuableWarpedFramePaddingElements, worker);

	determineAndDescribeFeatures(resuableWarpedFrame, width, height, resuableWarpedFramePaddingElements, blobFeatures, threshold, true, reusableIntegralFrame, worker);

	// we have to correct the observation of the feature points
	// we want the location in the 'flat' pattern and not in the perspectively warped image
	for (BlobFeatures::iterator i = blobFeatures.begin(); i != blobFeatures.end(); ++i)
	{
		i->setObservation(flatHomographyPerspective * i->observation(), i->distortionState());
	}

	return true;
}

}

}

}
