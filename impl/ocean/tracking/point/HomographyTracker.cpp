/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/point/HomographyTracker.h"

#include "ocean/cv/advanced/AdvancedMotion.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/geometry/NonLinearOptimizationHomography.h"
#include "ocean/geometry/RANSAC.h"
#include "ocean/geometry/SpatialDistribution.h"
#include "ocean/geometry/Utilities.h"

namespace Ocean
{

namespace Tracking
{

namespace Point
{

bool HomographyTracker::resetRegion(const AnyCamera& camera, const Box2& region)
{
	ocean_assert(camera.isValid());
	ocean_assert(region.area() >= Scalar(1));

	ocean_assert(camera.isInside(region.corner(0)) && camera.isInside(region.corner(1)));
	ocean_assert(camera.isInside(region.corner(2)) && camera.isInside(region.corner(3)));

	if (!camera.isValid() || region.area() < Scalar(1) || !camera.isInside(region.center()))
	{
		return false;
	}

	reset();

	region_ = region;

	// **TODO**, we may need to reset additional parameters

	return true;
}

bool HomographyTracker::resetRegion(const AnyCamera& camera, const Box2& region, const Quaternion& cameraOrientation, const Vector3& planeNormal, HomogenousMatrix4* pose, HomogenousMatrix4* plane)
{
	ocean_assert(camera.isValid());
	ocean_assert(region.area() > Scalar(1));
	ocean_assert(camera.isInside(region.center()));
	ocean_assert(cameraOrientation.isValid());
	ocean_assert(Numeric::isEqual(planeNormal.length(), 1));

	if (!camera.isValid() || region.area() < Scalar(1) || !cameraOrientation.isValid() || !Numeric::isEqual(planeNormal.length(), 1)
			|| !camera.isInside(region.center()))
	{
		return false;
	}

	Quaternion invCameraOrientation;
	if (!cameraOrientation.invert(invCameraOrientation))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	reset();

	region_ = region;

	// the initial camera pose is in the origin of the world coordinate system (identity transformation)
	const HomogenousMatrix4 cameraPose(true);

	const Line3 ray = camera.ray(region.center(), cameraPose);

	// 3D point with distance 1 to camera (center of projection)
	const Vector3 pointOnPlane = ray.point(1);

	// cN = wTc^-1 * wN
	const Vector3 planeNormalCamera = invCameraOrientation * planeNormal;

	plane_ = Plane3(pointOnPlane, planeNormalCamera);

	if (pose)
	{
		*pose = cameraPose;
	}

	if (plane)
	{
		// cG = wTc`^-1 * wG
		const Vector3 groundNormal = invCameraOrientation * Vector3(0, 1, 0); // negative gravity vector in camera coordinate system
		ocean_assert(Numeric::isEqual(groundNormal.length(), Scalar(1)));

		if (Numeric::abs(groundNormal * planeNormalCamera) >= Numeric::cos(Numeric::deg2rad(20)))
		{
			// we have a ground plane

			const Vector3& zAxis = planeNormalCamera;
			ocean_assert(zAxis == plane_.normal());

			// we align the negative-z axis of the ground plane with the viewing direction of the camera

			Vector3 yAxis = plane_.projectOnPlane(pointOnPlane - Vector3(0, 0, 1));

			Vector3 xAxis = yAxis.cross(zAxis);
			yAxis = zAxis.cross(xAxis);

			if (!xAxis.normalize() || !yAxis.normalize())
			{
				ocean_assert(false && "This should never happen!");

				reset();
				return false;
			}

			ocean_assert(Numeric::isWeakEqual(xAxis.length(), Scalar(1)));
			ocean_assert(Numeric::isWeakEqual(yAxis.length(), Scalar(1)));
			ocean_assert(Numeric::isWeakEqual(zAxis.length(), Scalar(1)));

			*plane = HomogenousMatrix4(xAxis, yAxis, zAxis, pointOnPlane);
		}
		else
		{
			// we have a vertical plane

			const Vector3& zAxis = planeNormalCamera;
			Vector3 yAxis = groundNormal;

			Vector3 xAxis = yAxis.cross(zAxis);

			// make y perpendicular
			yAxis = zAxis.cross(xAxis);

			if (!xAxis.normalize() || !yAxis.normalize())
			{
				ocean_assert(false && "This should never happen!");

				reset();
				return false;
			}

			ocean_assert(Numeric::isWeakEqual(xAxis.length(), Scalar(1)));
			ocean_assert(Numeric::isWeakEqual(yAxis.length(), Scalar(1)));
			ocean_assert(Numeric::isWeakEqual(zAxis.length(), Scalar(1)));

			*plane = HomogenousMatrix4(xAxis, yAxis, zAxis, pointOnPlane);
		}
	}

	// **TODO**, we may need to reset additional parameters

	return true;
}

bool HomographyTracker::determineHomography(const AnyCamera& camera, const Frame& yFrame, SquareMatrix3& homography, HomogenousMatrix4* pose, const Quaternion& cameraOrientation, Worker* worker)
{
	ocean_assert(FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

	ocean_assert(!previousFramePyramid_.isValid() || previousFramePyramid_.frameType() == FrameType(yFrame, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));

	if (!yFrame.isValid() || yFrame.width() < 40u || yFrame.height() < 40u
		|| !FrameType::formatIsGeneric(yFrame.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u)
		|| (previousFramePyramid_ && previousFramePyramid_.frameType() != FrameType(yFrame, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>())))
	{
		return false;
	}

	if (camera.anyCameraType() != AnyCameraType::PINHOLE)
	{
		ocean_assert(false && "Currently not supported!");
		return false;
	}

	const unsigned int maxSize = std::max(yFrame.width(), yFrame.height());

	// we ensure that corresponding feature points can have an offset of 20.0% between to successive video frames (to get a pyramid with enough layers for any tracking situation)
	const unsigned int pyramidLayers = previousFramePyramid_ ? previousFramePyramid_.layers() : CV::FramePyramid::idealLayers(yFrame.width(), yFrame.height(), 20u, 20u, 2u, maxSize * 20u / 100u, 2u /* = smallest coarsest layer radius*/);

	ocean_assert(pyramidLayers >= 1u);
	if (pyramidLayers == 0u)
	{
		return false;
	}

	constexpr bool copyFirstLayer = true; // we need to make a copy of the first layer, as this pyramid will be used as 'previousPyramid' in the next call of resetRegion()

	currentFramePyramid_.replace8BitPerChannel11(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), 1u, yFrame.pixelOrigin(), pyramidLayers, yFrame.paddingElements(), copyFirstLayer, worker);

	homography.toNull();

	if (previousFramePyramid_ && isRegionVisible(globalCameraOrientation_, cameraOrientation))
	{
		ocean_assert(previousFramePyramid_.layers() == currentFramePyramid_.layers());

		ocean_assert(!globalHomography_.isNull());

		if (!needsReInitialization_)
		{
			SquareMatrix3 predictedLocalHomography(false); // cHp: currentPoint = predictedLocalHomography * previousPoint
			if (previousCameraOrientation_.isValid() && cameraOrientation.isValid())
			{
				if (camera.name() != AnyCameraPinhole::WrappedCamera::name())
				{
					ocean_assert(false && "Currently not supported!");
					return false;
				}

				const AnyCameraPinhole& pinholeCamera = (const AnyCameraPinhole&)(camera);

				// cHp = wTc^-1 * wTp
				predictedLocalHomography = Geometry::Homography::homographyMatrix(previousCameraOrientation_, cameraOrientation, pinholeCamera.actualCamera(), pinholeCamera.actualCamera());
			}

			if (!addNewFeaturePointsToPyramid(previousFramePyramid_, previousPointsPyramid_, initialPointsPyramid_, region_, globalHomography_, 40u, 80u, worker))
			{
				ocean_assert(false && "This must never happen!");
				return false; // **TODO** we should ensure that the track will be reset in this case
			}

			Vectors2Pyramid currentPointsPyramid;
			const HomographyQuality homographyQuality = determineHomographyWithPyramid(camera, plane_, previousFramePyramid_, currentFramePyramid_, previousPointsPyramid_, currentPointsPyramid, initialPointsPyramid_, globalHomography_, region_, homography, pose, predictedLocalHomography, initialCameraOrientation_, cameraOrientation, randomGenerator_, -1.0f, worker);

			if (homographyQuality == HQ_FAILED)
			{
				needsReInitialization_ = true;

				homography.toNull();
			}
			else
			{
				ocean_assert(homographyQuality > HQ_FAILED);

				globalHomography_ = homography;
				globalCameraOrientation_ = cameraOrientation;

				// we update the keyframe if we have a good homography quality
				if (homographyQuality >= HQ_GOOD)
				{
					Scalar cameraMotionAngle = 0;

					if (cameraOrientation.isValid() && previousCameraOrientation_.isValid())
					{
						// (wTp)^-1 * wTc = pTc
						cameraMotionAngle = cameraOrientation.smallestAngle(previousCameraOrientation_);
					}

					// we update the keyframe if we have an almost static camera motion (to avoid motion blur in the keyframe)
					if (cameraMotionAngle <= Numeric::deg2rad(0.25))
					{
						bool enoughPoints = true;

						for (size_t nLayer = 0; nLayer < currentPointsPyramid.size(); ++nLayer)
						{
							if (currentPointsPyramid[nLayer].size() > 0 && currentPointsPyramid[nLayer].size() < 25)
							{
								enoughPoints = false;
								break;
							}
						}

						if (enoughPoints)
						{
							static_assert(numberKeyFrames_ >= 2, "Invalid key frames!");

							if (isRegionVisible(camera, globalHomography_, region_))
							{
								bool needsUpdate = true;

								for (size_t n = 0; needsUpdate && n < numberKeyFrames_; ++n)
								{
									if (keyFrames_[n])
									{
										const SquareMatrix3 kHc = keyFrames_[n].globalHomography_ * globalHomography_.inverted(); // kHc = kHi * cHi^-1

										// we approximate the scale by averaging the length of the x-, and y-axis
										const Scalar scale = (Vector2(kHc(0, 0), kHc(1, 0)).length() + Vector2(kHc(0, 1), kHc(1, 1)).length()) * Scalar(0.5);

										if (scale > Scalar(0.5) && scale < Scalar(2))
										{
											needsUpdate = false;
										}
									}
								}

								if (needsUpdate)
								{
									keyFrames_[1] = KeyFrame(yFrame.timestamp(), initialPointsPyramid_, currentPointsPyramid, currentFramePyramid_, globalHomography_, cameraOrientation);
								}
							}
						}
					}
				}
			}

			previousPointsPyramid_ = std::move(currentPointsPyramid);
		}

		static_assert(numberKeyFrames_ >= 2, "Invalid key frames!");
		ocean_assert(!keyFrames_[1] || keyFrames_[0]); // key frame 0 is always the important keyframe

		if (needsReInitialization_ && keyFrames_[0])
		{
			for (size_t keyFrameIndex = 0u; keyFrameIndex < numberKeyFrames_; ++keyFrameIndex)
			{
				const KeyFrame& keyFrame = keyFrames_[keyFrameIndex];

				if (!keyFrame)
				{
					// we do not have any additional key frames to check
					break;
				}

				SquareMatrix3 predictedKeyFrameHomography(false); // cHk: currentPoint = predictedKeyFrameHomography * keyFramePoint
				if (keyFrame.cameraOrientation_.isValid() && cameraOrientation.isValid())
				{
					SquareMatrix3 predictedLocalHomography(false); // cHp: currentPoint = predictedLocalHomography * previousPoint
					if (previousCameraOrientation_.isValid() && cameraOrientation.isValid())
					{
						if (camera.name() != AnyCameraPinhole::WrappedCamera::name())
						{
							ocean_assert(false && "Currently not supported!");
							return false;
						}

						const AnyCameraPinhole& pinholeCamera = (const AnyCameraPinhole&)(camera);

						// cHl = wTc^-1 * wTl
						predictedLocalHomography = Geometry::Homography::homographyMatrix(globalCameraOrientation_, cameraOrientation, pinholeCamera.actualCamera(), pinholeCamera.actualCamera());
					}

					// cHk = cHl * lHi * kHi^-1
					predictedKeyFrameHomography = predictedLocalHomography * globalHomography_ * keyFrame.globalHomography_.inverted();
				}

				CV::FramePyramid transformedKeyFramePyramid;

				Vectors2Pyramid copyKeyFrameInitialPointsPyramid;
				Vectors2Pyramid copyKeyFramePointsPyramid;

				if (predictedKeyFrameHomography.isNull())
				{
					// we do not have a roughly predicted homography (e.g., from IMU data), so that we simply must hope that the keyframe is close to the current frame so that the sparse point tracking works
					transformedKeyFramePyramid = CV::FramePyramid(keyFrame.pyramid_, 0u, CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE, false /*copyData*/);

					copyKeyFrameInitialPointsPyramid = keyFrame.initialPointsPyramid_;
					copyKeyFramePointsPyramid = keyFrame.pointsPyramid_;
				}
				else
				{
					// cHi = cHk * kHi
					const SquareMatrix3 cHi = predictedKeyFrameHomography * keyFrame.globalHomography_;

					if (isRegionVisible(camera, cHi, region_))
					{
						copyKeyFramePointsPyramid.resize(keyFrame.pointsPyramid_.size());
						copyKeyFrameInitialPointsPyramid.resize(keyFrame.pointsPyramid_.size());

						bool enoughPointsVisible = true;

						for (size_t nLayer = 0; nLayer < keyFrame.pointsPyramid_.size(); ++nLayer)
						{
							copyKeyFramePointsPyramid[nLayer].reserve(keyFrame.pointsPyramid_[nLayer].size());
							copyKeyFrameInitialPointsPyramid[nLayer].reserve(keyFrame.pointsPyramid_[nLayer].size());

							const SquareMatrix3 predictedKeyFrameLayerHomography = Geometry::Homography::toCoarseHomography(predictedKeyFrameHomography, (unsigned int)nLayer);

							Vector2 currentPoint;

							for (size_t nPoint = 0; nPoint < keyFrame.pointsPyramid_[nLayer].size(); ++nPoint)
							{
								if (predictedKeyFrameLayerHomography.multiply(keyFrame.pointsPyramid_[nLayer][nPoint], currentPoint))
								{
									if (currentPoint.x() >= Scalar(0) && currentPoint.y() >= Scalar(0) && currentPoint.x() < Scalar(keyFrame.pyramid_[(unsigned int)(nLayer)].width()) && currentPoint.y() < Scalar(keyFrame.pyramid_[(unsigned int)(nLayer)].height()))
									{
										copyKeyFramePointsPyramid[nLayer].push_back(currentPoint);
										copyKeyFrameInitialPointsPyramid[nLayer].push_back(keyFrame.initialPointsPyramid_[nLayer][nPoint]);
									}
								}
							}

							if (copyKeyFrameInitialPointsPyramid[nLayer].size() > 0 && (copyKeyFrameInitialPointsPyramid[nLayer].size() < 20 || keyFrame.initialPointsPyramid_.size() * 50 / 100 > copyKeyFrameInitialPointsPyramid.size()))
							{
								// we do not have enough visible points which we can track from the (transformed) key frame to the current frame
								enoughPointsVisible = false;
								break;
							}
						}

						if (enoughPointsVisible)
						{
							Frame transformedKeyFrame(keyFrame.pyramid_[0].frameType());
							CV::FrameInterpolatorBilinear::homography<uint8_t, 1u>(keyFrame.pyramid_[0].constdata<uint8_t>(), keyFrame.pyramid_[0].width(), keyFrame.pyramid_[0].height(), predictedKeyFrameHomography.inverted() /* kHc */, nullptr, transformedKeyFrame.data<uint8_t>(), CV::PixelPositionI(0, 0), transformedKeyFrame.width(), transformedKeyFrame.height(), keyFrame.pyramid_[0].paddingElements(), transformedKeyFrame.paddingElements(), worker);

							transformedKeyFramePyramid.replace8BitPerChannel11(transformedKeyFrame.constdata<uint8_t>(), transformedKeyFrame.width(), transformedKeyFrame.height(), 1u, transformedKeyFrame.pixelOrigin(), keyFrame.pyramid_.layers(), transformedKeyFrame.paddingElements(), false /*copyFirstLayer*/, worker);

							// now we have a transformed keyframe image so that the prediction is actually the identity
							predictedKeyFrameHomography.toIdentity();
						}
					}
				}

				if (transformedKeyFramePyramid)
				{
#ifdef OCEAN_DEBUG
					ocean_assert(copyKeyFramePointsPyramid.size() == copyKeyFrameInitialPointsPyramid.size());
					for (size_t n = 0; n < copyKeyFramePointsPyramid.size(); ++n)
					{
						ocean_assert(copyKeyFramePointsPyramid[n].size() == copyKeyFrameInitialPointsPyramid[n].size());
					}
#endif

					Vectors2Pyramid currentPointsPyramid;

					const float explicitMaximalOffsetPercent = 0.10f; // 10 %

					const HomographyQuality homographyQuality = determineHomographyWithPyramid(camera, plane_, transformedKeyFramePyramid, currentFramePyramid_, copyKeyFramePointsPyramid, currentPointsPyramid, copyKeyFrameInitialPointsPyramid, keyFrame.globalHomography_, Box2(), homography, pose, predictedKeyFrameHomography, initialCameraOrientation_, cameraOrientation, randomGenerator_, explicitMaximalOffsetPercent, worker);

					if (homographyQuality == HQ_GOOD)
					{
						globalHomography_ = homography;
						globalCameraOrientation_ = cameraOrientation;

						previousPointsPyramid_ = std::move(currentPointsPyramid);
						initialPointsPyramid_ = std::move(copyKeyFrameInitialPointsPyramid);

						needsReInitialization_ = false;

						// we do not need to check the next key-frame
						break;
					}
				}
			}
		}
	}
	else
	{
		if (!previousFramePyramid_.isValid())
		{
			// this is the initial camera frame

			initialCameraOrientation_ = cameraOrientation;

			homography.toIdentity();
			globalHomography_.toIdentity();
			globalCameraOrientation_ = cameraOrientation;

			// we combine previousPointsPyramid_ with currentFramePyramid_ as currentFramePyramid_ will be swapped (to previousFramePyramid_) below
			if (!addNewFeaturePointsToPyramid(currentFramePyramid_, previousPointsPyramid_, initialPointsPyramid_, region_, globalHomography_, 40u, 80u, worker))
			{
				ocean_assert(false && "This must never happen!");
				return false; // **TODO** we should ensure that the track will be reset in this case
			}

			ocean_assert(!keyFrames_[0]);
			keyFrames_[0] = KeyFrame(yFrame.timestamp(), initialPointsPyramid_, previousPointsPyramid_, currentFramePyramid_, globalHomography_, cameraOrientation);

			if (pose)
			{
				pose->toIdentity();
			}
		}
	}

	std::swap(previousFramePyramid_, currentFramePyramid_);

#ifdef OCEAN_DEBUG
	ocean_assert(previousPointsPyramid_.size() == initialPointsPyramid_.size());
	for (size_t layer = 0; layer < previousPointsPyramid_.size(); ++layer)
	{
		ocean_assert(previousPointsPyramid_[layer].size() == initialPointsPyramid_[layer].size());
	}
#endif // OCEAN_DEBUG

	previousCameraOrientation_ = cameraOrientation;

	return !homography.isNull(); // 'true' if we were able to determine valid homography - otherwise 'false' (e.g., when the tracking region is out of view)
}

bool HomographyTracker::addNewFeaturePointsToPyramid(const CV::FramePyramid& yFramePyramid, Vectors2Pyramid& pointsPyramid, Vectors2Pyramid& initialPointsPyramid, const Box2& region, const SquareMatrix3& homography, const unsigned int minimalFeatures, const unsigned int maximalFeatures, Worker* worker)
{
	SquareMatrix3 invHomography; // iHc
	homography.invert(invHomography); // iHc = cHi^-1

	for (unsigned int layer = 0u; layer <= 2u && layer < yFramePyramid.layers(); layer += 2u)
	{
		if (layer >= pointsPyramid.size())
		{
			pointsPyramid.resize(layer + 1u);
			initialPointsPyramid.resize(pointsPyramid.size());
		}

		ocean_assert(initialPointsPyramid[layer].size() == pointsPyramid[layer].size());

		const unsigned int featureThreshold = layer == 0u ? 18u : 10u;

		Triangles2 layerSubRegionTriangles;

		if (region.isValid())
		{
			const unsigned int layerFactor = CV::FramePyramid::sizeFactor((unsigned int)layer);
			ocean_assert(layerFactor != 0u);
			const Scalar invLayerFactor = Scalar(1) / Scalar(layerFactor);

			const SquareMatrix3 downsampleTransformation(Vector3(invLayerFactor, 0, 0), Vector3(0, invLayerFactor, 0), Vector3(0, 0, 1));
			const SquareMatrix3 previousLayerHomography = Geometry::Homography::toCoarseHomography(homography, layer) * downsampleTransformation;

			layerSubRegionTriangles =
			{
				Triangle2(previousLayerHomography * region.corner(0), previousLayerHomography * region.corner(1), previousLayerHomography * region.corner(2)),
				Triangle2(previousLayerHomography * region.corner(0), previousLayerHomography * region.corner(2), previousLayerHomography * region.corner(3))
			};
		}

		addNewFeaturePointsToPyramidLayer(yFramePyramid, pointsPyramid, layerSubRegionTriangles, layer, featureThreshold, minimalFeatures, maximalFeatures, worker);

		// we need to determine the initial image points for the added/new feature points

		ocean_assert(initialPointsPyramid[layer].size() <= pointsPyramid[layer].size());

		if (!pointsPyramid[layer].empty())
		{
			const SquareMatrix3 invGlobalLayerHomography = Geometry::Homography::toCoarseHomography(invHomography, layer);

			Vector2 initialPoint;
			for (size_t n = initialPointsPyramid[layer].size(); n < pointsPyramid[layer].size(); ++n)
			{
				if (invGlobalLayerHomography.multiply(pointsPyramid[layer][n], initialPoint) && initialPoint.x() >= 0 && initialPoint.y() >= 0 && initialPoint.x() < Scalar(yFramePyramid.width(layer)) && initialPoint.y() < Scalar(yFramePyramid.height(layer)))
				{
					initialPointsPyramid[layer].emplace_back(initialPoint);
				}
				else
				{
					// we need to remove the point from 'pointsPyramid[layer]', instead of applying erase() we overwrite the point with the last point (as the order is not important in this case)

					pointsPyramid[layer][n] = pointsPyramid[layer].back();
					pointsPyramid[layer].pop_back();
					--n;
				}
			}

			ocean_assert(initialPointsPyramid[layer].size() == pointsPyramid[layer].size());
		}
	}

	return true;
}

bool HomographyTracker::addNewFeaturePointsToPyramidLayer(const CV::FramePyramid& yFramePyramid, Vectors2Pyramid& pointsPyramid, const Triangles2& layerSubRegionTriangles, const unsigned int layer, const unsigned int featureThreshold, const unsigned int minimalFeatures, const unsigned int /*maximalFeatures*/, Worker* worker)
{
	ocean_assert(layer < yFramePyramid.layers());
	ocean_assert(layer < pointsPyramid.size());

	const Frame& layerFrame = yFramePyramid[layer];
	Vectors2& layerPoints = pointsPyramid[layer];

	if (layerPoints.size() >= size_t(minimalFeatures))
	{
		return true;
	}

	// first, we detect feature points in the selected layer image

	CV::Detector::HarrisCorners harrisCorners;
	if (!CV::Detector::HarrisCornerDetector::detectCorners(layerFrame.constdata<uint8_t>(), layerFrame.width(), layerFrame.height(), layerFrame.paddingElements(), featureThreshold, true, harrisCorners, true, worker))
	{
		return false;
	}

	std::sort(harrisCorners.begin(), harrisCorners.end());

	// we keep any feature point inside our region of interest, and we select/filter strong feature points outside the region of interest

	Geometry::SpatialDistribution::OccupancyArray occupancyArray(Scalar(0), Scalar(0), Scalar(layerFrame.width()), Scalar(layerFrame.height()), 10u, 10u, true);

	for (const Vector2& point : layerPoints)
	{
		// just setting the occupancy array, not checking whether
		occupancyArray.addPointWithCounter(point, (unsigned int)(-1));
	}

	const Box2 layerSubRegionBox(layerSubRegionTriangles);

	for (const CV::Detector::HarrisCorner& harrisCorner : harrisCorners)
	{
		const Vector2& point = harrisCorner.observation();

		if (layerSubRegionBox.isValid() && layerSubRegionBox.isInside(point) && Triangle2::isInside(layerSubRegionTriangles, point))
		{
			if (occupancyArray.addPointWithCounter(point, 5u)) // up to 5 similar feature points in the region of interest
			{
				layerPoints.emplace_back(point);
			}
		}
		else if (occupancyArray.addPointWithCounter(point, 1u)) // at most 1 feature outside the region of interest
		{
			layerPoints.emplace_back(point);
		}
	}

	return true;
}

bool HomographyTracker::trackPoints(const CV::FramePyramid& yPreviousFramePyramid, const CV::FramePyramid& yCurrentFramePyramid, Vectors2Pyramid& previousPointsPyramid, Vectors2Pyramid& currentPointsPyramid, const unsigned int startLayer, const float maximalOffsetPercent, Indices32& validTrackedPointIndices, const SquareMatrix3& roughHomography, const unsigned int coarsestLayerRadius, const unsigned int subPixelIterations, Worker* worker)
{
	ocean_assert(coarsestLayerRadius >= 1u);

	ocean_assert(startLayer < yPreviousFramePyramid.layers());

	ocean_assert(previousPointsPyramid.size() == currentPointsPyramid.size());
	ocean_assert(startLayer < previousPointsPyramid.size());
	ocean_assert(startLayer < currentPointsPyramid.size());

	if (startLayer >= yPreviousFramePyramid.layers())
	{
		return false;
	}

	const FrameType& startLayerType = yPreviousFramePyramid[startLayer];
	const unsigned int maxLayerSize = std::max(startLayerType.width(), startLayerType.height());

	const unsigned int layers = CV::FramePyramid::idealLayers(startLayerType.width(), startLayerType.height(), 20u, 20u, 2u, (unsigned int)(float(maxLayerSize) * maximalOffsetPercent + 0.5f), coarsestLayerRadius);

	ocean_assert(layers >= 1u);
	if (layers == 0u)
	{
		return false;
	}

	// let's create new frame pyramids, starting with level 'firstPyramidLayerIndex' - while we do not copy the data
	const CV::FramePyramid hierarchyPreviousFramePyramid(yPreviousFramePyramid, startLayer, layers, false /*copyData*/);
	const CV::FramePyramid hierarchyCurrentFramePyramid(yCurrentFramePyramid, startLayer, layers, false /*copyData*/);

	Vectors2& previousLayerPoints = previousPointsPyramid[startLayer];
	Vectors2& currentLayerPoints = currentPointsPyramid[startLayer];

	ocean_assert(previousLayerPoints.size() > 0);

	// the rough locations of the current points are either a simple copy of the previous points, or a prediction based on the given homography

	Vectors2 roughCurrentLayerPoints;

	if (roughHomography.isNull())
	{
		roughCurrentLayerPoints = previousLayerPoints;
	}
	else
	{
		// the rough homography is defined in the domain of the finest pyramid layer, we need to adjust it to match with the actual tracking layer

		const SquareMatrix3 roughLayerHomography = Geometry::Homography::toCoarseHomography(roughHomography, startLayer);

		roughCurrentLayerPoints.reserve(previousLayerPoints.size());

		Vector2 roughCurrentLayerPoint;
		for (const Vector2& previousLayerPoint : previousLayerPoints)
		{
			if (roughLayerHomography.multiply(previousLayerPoint, roughCurrentLayerPoint))
			{
				roughCurrentLayerPoints.emplace_back(minmax<Scalar>(0, roughCurrentLayerPoint.x(), Scalar(startLayerType.width() - 1u)), minmax<Scalar>(0, roughCurrentLayerPoint.y(), Scalar(startLayerType.height() - 1u)));
			}
			else
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}
		}
	}

#if 1 // for now, we use a uni-directional tracking - instead of a bidirectional tracking
	validTrackedPointIndices = createIndices(previousLayerPoints.size(), 0u);

	return CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsSubPixelMirroredBorder<1u, 7u>(hierarchyPreviousFramePyramid, hierarchyCurrentFramePyramid, previousLayerPoints, roughCurrentLayerPoints, currentLayerPoints, coarsestLayerRadius, subPixelIterations, worker);
#else
	return CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointsBidirectionalSubPixelMirroredBorderWithRoughLocations<1u, 7u>(hierarchyPreviousFramePyramid, hierarchyCurrentFramePyramid, coarsestLayerRadius, previousLayerPoints, roughCurrentLayerPoints, currentLayerPoints, Scalar(0.9 * 0.9), worker, &validTrackedPointIndices, subPixelIterations);
#endif
}

HomographyTracker::HomographyQuality HomographyTracker::determineHomographyWithPyramid(const AnyCamera& camera, const Plane3& plane, const CV::FramePyramid& yPreviousFramePyramid, const CV::FramePyramid& yCurrentFramePyramid, Vectors2Pyramid& previousPointsPyramid, Vectors2Pyramid& currentPointsPyramid, Vectors2Pyramid& initialPointsPyramid, const SquareMatrix3& previousHomography, const Box2& region, SquareMatrix3& homography, HomogenousMatrix4* pose, const SquareMatrix3& predictedLocalHomography, const Quaternion& initialCameraOrientation, const Quaternion& currentCameraOrientation, RandomGenerator& randomGenerator, const float explicitMaximalOffsetPercent, Worker* worker)
{
	ocean_assert(previousPointsPyramid.size() >= 3);
	if (previousPointsPyramid.size() < 3)
	{
		return HQ_FAILED;
	}

	currentPointsPyramid = Vectors2Pyramid(previousPointsPyramid.size());
	Indices32 validTrackedPointIndices;

	SquareMatrix3 roughHomography(false); // (finestCurrentPoint = roughHomography * finestPreviousPoint)

	unsigned int lastSuccessfulLayer = (unsigned int)(-1);

	for (unsigned int layer = (unsigned int)previousPointsPyramid.size() - 1u; layer < (unsigned int)previousPointsPyramid.size(); --layer)
	{
		validTrackedPointIndices.clear();

		if (previousPointsPyramid[layer].size() >= 10)
		{
			float maximalOffsetPercent = 0.0175f; // 1.75%

			if (roughHomography.isNull())
			{
				// point tracking on coarse pyramid layer

				if (predictedLocalHomography.isNull())
				{
					// no IMU-based camera motion
					maximalOffsetPercent = 0.05f; // 5%
				}
				else
				{
					// IMU-based camera motion
					maximalOffsetPercent = 0.0275f; // 2.75%

					roughHomography = predictedLocalHomography;
				}
			}

			if (explicitMaximalOffsetPercent > 0.0f)
			{
				maximalOffsetPercent = explicitMaximalOffsetPercent;
			}

			const unsigned int coarsestLayerRadius = layer == 0u ? 2u : 4u;
			const unsigned int subPixelIterations = layer == 0u ? 2u : 1u;

			if (trackPoints(yPreviousFramePyramid, yCurrentFramePyramid, previousPointsPyramid, currentPointsPyramid, layer, maximalOffsetPercent, validTrackedPointIndices, roughHomography, coarsestLayerRadius, subPixelIterations, worker))
			{
				ocean_assert(previousPointsPyramid[layer].size() == currentPointsPyramid[layer].size());

				if (validTrackedPointIndices.size() >= 10)
				{
					const Vectors2 trackedPreviousPoints = Subset::subset(previousPointsPyramid[layer], validTrackedPointIndices);
					const Vectors2 trackedCurrentPoints = Subset::subset(currentPointsPyramid[layer], validTrackedPointIndices);

					if (camera.isValid() && plane.isValid())
					{
						// we know the 3D geometry of the area in the background (the planar area)
						// therefore, we can use a P3P and 6DOF camera pose optimization approach instead of determining the homography (P3P is faster and more reliable)

						const Vectors2 trackedInitialPoints = Subset::subset(initialPointsPyramid[layer], validTrackedPointIndices);
						const SharedAnyCamera layerCamera = camera.clone(yCurrentFramePyramid[layer].width(), yCurrentFramePyramid[layer].height());

						ocean_assert(layerCamera);
						if (!layerCamera)
						{
							return HQ_FAILED;
						}

#ifdef OCEAN_DEBUG
						for (const Vector2& trackedInitialPoint : trackedInitialPoints)
						{
							ocean_assert(layerCamera->isInside(trackedInitialPoint));
						}
#endif

						const Vectors3 objectPoints = Geometry::Utilities::backProjectImagePoints(*layerCamera, HomogenousMatrix4(true), plane, trackedInitialPoints.data(), trackedInitialPoints.size());

						Indices32 validPoseIndices;
						HomogenousMatrix4 layerPose;

						if (Geometry::RANSAC::p3p(*layerCamera, ConstArrayAccessor<Vector3>(objectPoints), ConstArrayAccessor<Vector2>(trackedCurrentPoints), randomGenerator, layerPose, 10u, true, 40u, Scalar(3 * 3), &validPoseIndices))
						{
							const Scalar maximalAngle = Numeric::deg2rad(7.5); // this threshold is quite generous as IMU-based orientations can be quite faulty on Android devices

							if (validPoseIndices.size() >= 10 && isPosePlausible(layerPose, initialCameraOrientation, currentCameraOrientation, maximalAngle))
							{
								// we update the points in the current pyramid layer so that we keep well trackable points only

								previousPointsPyramid[layer] = Subset::subset(trackedPreviousPoints, validPoseIndices);
								currentPointsPyramid[layer] = Subset::subset(trackedCurrentPoints, validPoseIndices);

								SquareMatrix3 layerHomography(false); // this is a different way to determine the local homography
								if (Geometry::Homography::homographyMatrixLinearWithoutOptimations(previousPointsPyramid[layer].data(), currentPointsPyramid[layer].data(), previousPointsPyramid[layer].size(), layerHomography)) // layerHomography = cHp (for the layer)
								{
									roughHomography = Geometry::Homography::toFinestHomography(layerHomography, layer);

									// we have to shrink the set of initial image points so that it fits with the set of all tracked points

									Vectors2 validInitialPoints;
									validInitialPoints.reserve(validPoseIndices.size());

									for (const Index32& validHomographyIndex : validPoseIndices)
									{
										validInitialPoints.emplace_back(initialPointsPyramid[layer][validTrackedPointIndices[validHomographyIndex]]);
									}

									initialPointsPyramid[layer] = std::move(validInitialPoints);

									lastSuccessfulLayer = layer;

									if (pose)
									{
										*pose = layerPose;
									}
								}
							}
						}
					}
					else
					{
						Indices32 validHomographyIndices;
						SquareMatrix3 layerHomography;

						if (Geometry::RANSAC::homographyMatrix<true, false>(trackedPreviousPoints.data(), trackedCurrentPoints.data(), trackedPreviousPoints.size(), randomGenerator, layerHomography, 4u, 80u, Scalar(3 * 3), &validHomographyIndices, worker))
						{
							if (validHomographyIndices.size() >= 10 && (!region.isValid() || isHomographyPlausible(previousHomography, Geometry::Homography::toFinestHomography(layerHomography, layer) * previousHomography, region)))
							{
								roughHomography = Geometry::Homography::toFinestHomography(layerHomography, layer);

								// we update the points in the current pyramid layer so that we keep well trackable points only

								previousPointsPyramid[layer] = Subset::subset(trackedPreviousPoints, validHomographyIndices);
								currentPointsPyramid[layer] = Subset::subset(trackedCurrentPoints, validHomographyIndices);

								// we have to shrink the set of initial image points so that it fits with the set of all tracked points

								Vectors2 validInitialPoints;
								validInitialPoints.reserve(validHomographyIndices.size());

								for (const Index32& validHomographyIndex : validHomographyIndices)
								{
									validInitialPoints.emplace_back(initialPointsPyramid[layer][validTrackedPointIndices[validHomographyIndex]]);
								}

								initialPointsPyramid[layer] = std::move(validInitialPoints);

								lastSuccessfulLayer = layer;
							}
						}
					}
				}
			}
		}

		if (lastSuccessfulLayer != layer)
		{
			// this layer could not be used for homography estimation, therefore we may delete all feature points to avoid using them next time

			previousPointsPyramid[layer].clear();
			initialPointsPyramid[layer].clear();
			currentPointsPyramid[layer].clear();
		}

		ocean_assert(previousPointsPyramid[layer].size() == initialPointsPyramid[layer].size());
		ocean_assert(previousPointsPyramid[layer].size() == currentPointsPyramid[layer].size());
	}

	if (lastSuccessfulLayer == 0u || (lastSuccessfulLayer == 2u && previousPointsPyramid[2].size() > 20))
	{
		const Vector2* initialImagePoints = initialPointsPyramid[lastSuccessfulLayer].data();
		const Vector2* currentImagePoints = currentPointsPyramid[lastSuccessfulLayer].data();
		const size_t numberImagePoints = initialPointsPyramid[lastSuccessfulLayer].size();

#ifdef OCEAN_DEBUG
		for (size_t n = 0; n < numberImagePoints; ++n)
		{
			ocean_assert(camera.isInside(initialImagePoints[n]));
			ocean_assert(camera.isInside(currentImagePoints[n]));
		}
#endif

		ocean_assert(numberImagePoints >= 5);

		const SquareMatrix3 roughGlobalLayerHomography = Geometry::Homography::toCoarseHomography(roughHomography * previousHomography, lastSuccessfulLayer); // cHp * pHi

		SquareMatrix3 optimizedLayerHomography;
		Scalar initialError = Numeric::maxValue();
		Scalar finalError = Numeric::maxValue();
		Scalars intermediateErrors;

		if (Geometry::NonLinearOptimizationHomography::optimizeHomography<Geometry::Estimator::ET_SQUARE>(roughGlobalLayerHomography, initialImagePoints, currentImagePoints, numberImagePoints, 9u, optimizedLayerHomography, 20u, Scalar(0.001), Scalar(5), &initialError, &finalError, nullptr, &intermediateErrors))
		{
			homography = Geometry::Homography::toFinestHomography(optimizedLayerHomography, lastSuccessfulLayer);
			ocean_assert(homography.isHomography());
		}

		ocean_assert(previousPointsPyramid[lastSuccessfulLayer].size() == initialPointsPyramid[lastSuccessfulLayer].size());

		if (lastSuccessfulLayer == 0u && previousPointsPyramid[0].size() > 25)
		{
			return HQ_GOOD;
		}
		else
		{
			return HQ_MODERATE;
		}
	}

	return HQ_FAILED;
}

bool HomographyTracker::isRegionVisible(const Quaternion& wRi, const Quaternion& wRc, const Scalar maximalAngle)
{
	ocean_assert(maximalAngle > Scalar(0) && maximalAngle <= Numeric::pi_2());

	if (wRi.isValid() == false || wRc.isValid() == false)
	{
		// we do not have a camera orientation for the initialization frame or for the current frame
		// therefore, we cannot decide whether the region is visible
		return true;
	}

	const Vector3 zAxis(0, 0, 1);

	const Scalar cosValue = (wRi * zAxis) * (wRc * zAxis);

	return cosValue >= Numeric::cos(maximalAngle);
}

bool HomographyTracker::isRegionVisible(const AnyCamera& camera, const SquareMatrix3& globalHomography, const Box2& initialRegion)
{
	ocean_assert(camera.isValid());
	ocean_assert(!globalHomography.isSingular());
	ocean_assert(initialRegion.area() >= Scalar(1));

	const Box2 transformedRegion = initialRegion * globalHomography;
	const Box2 cameraBoundingBox(Scalar(0), Scalar(0), Scalar(camera.width()), Scalar(camera.height()));

	const Box2 intersectedTransformedRegion = cameraBoundingBox.intersection(transformedRegion);

	if (intersectedTransformedRegion.isValid() == false)
	{
		return false;
	}

	// either the transformed region fits entirely into the camera image (90%), or the transformed region covers the entire camera image (50%)
	return intersectedTransformedRegion.area() >= transformedRegion.area() * Scalar(0.9) || intersectedTransformedRegion.area() >= cameraBoundingBox.area() * Scalar(0.5);
}

bool HomographyTracker::isHomographyPlausible(const SquareMatrix3& pHi, const SquareMatrix3& cHi, const Box2& initialRegion, const Scalar maximalAngleChange)
{
	ocean_assert(pHi.isHomography() && cHi.isHomography() && initialRegion.area() > Scalar(1));
	ocean_assert(maximalAngleChange >= Scalar(0) && maximalAngleChange <= Numeric::pi_4());

	// we compare the inner angles of the transformed region of interest to ensure that the angles do not change too extreme between the previous and the current frame

	Vector2 previousCorners[4];
	Vector2 currentCorners[4];

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		if (!pHi.multiply(initialRegion.corner(n), previousCorners[n]) || !cHi.multiply(initialRegion.corner(n), currentCorners[n]))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}

	for (unsigned int n = 0u; n < 4u; ++n)
	{
		const unsigned int nMinus = (unsigned int)(modulo<int>(int(n) - 1, 4));
		const unsigned int nPlus = (unsigned int)(modulo<int>(int(n) + 1, 4));

		const Vector2 previousDirectionMinus = previousCorners[nMinus] - previousCorners[n];
		const Vector2 previousDirectionPlus = previousCorners[nPlus] - previousCorners[n];

		const Vector2 currentDirectionMinus = currentCorners[nMinus] - currentCorners[n];
		const Vector2 currentDirectionPlus = currentCorners[nPlus] - currentCorners[n];

		if (previousDirectionMinus.isNull() || previousDirectionPlus.isNull() || currentDirectionMinus.isNull() || currentDirectionPlus.isNull())
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		const Scalar previousAngle = previousDirectionMinus.angle(previousDirectionPlus);
		const Scalar currentAngle = currentDirectionMinus.angle(currentDirectionPlus);

		if (!Numeric::angleIsEqual(previousAngle, currentAngle, maximalAngleChange))
		{
			return false;
		}
	}

	return true;
}

bool HomographyTracker::isPosePlausible(const HomogenousMatrix4& currentPose, const Quaternion& initialCameraOrientation, const Quaternion& currentCameraOrientation, const Scalar maximalAngle)
{
	ocean_assert(currentPose.isValid());
	ocean_assert(maximalAngle >= Scalar(0) && maximalAngle <= Numeric::pi_2());

	if (!initialCameraOrientation.isValid() || !currentCameraOrientation.isValid())
	{
		// we do not have an IMU-based camera orientation, so we do not rate the pose
		return true;
	}

	// iRc = (wRi)^-1 * wRc
	const Quaternion offsetCameraOrientation = initialCameraOrientation.inverted() * currentCameraOrientation;

	// iRc == rotation(pose)
	return currentPose.rotation().smallestAngle(offsetCameraOrientation) < maximalAngle;
}

}

}

}
