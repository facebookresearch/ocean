/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Rotation.h"

namespace Ocean
{

template <typename T>
PinholeCameraT<T>::DistortionLookup::DistortionLookup(const PinholeCameraT<T>& camera, const unsigned int binSize)
{
	ocean_assert(camera.isValid() && binSize >= 1u);

	if (camera.isValid() && binSize >= 1u)
	{
		distortionLookupTable = LookupTable(camera.width(), camera.height(), max(1u, camera.width() / binSize), max(1u, camera.height() / binSize));

		if (camera.hasDistortionParameters())
		{
			for (size_t y = 0; y < distortionLookupTable.binsY(); ++y)
			{
				for (size_t x = 0; x < distortionLookupTable.binsX(); ++x)
				{
					const VectorT2<T> distortedPosition(T(distortionLookupTable.binCenterPositionX(x)), T(distortionLookupTable.binCenterPositionY(y)));
					const VectorT2<T> undistortedPosition(camera.undistort<true>(distortedPosition));

					/**
					 * Store differences between distorted and undistorted positions.
					 * A direct mapping from distorted to undistorted positions is not possible, as the clamping
					 * produces equal undistorted positions at borders, which leads to rendering artifacts near at frame buffer borders.
					 */
					distortionLookupTable.setBinCenterValue(x, y, undistortedPosition - distortedPosition);
				}
			}
		}
		else
		{
			for (size_t y = 0; y < distortionLookupTable.binsY(); ++y)
			{
				for (size_t x = 0; x < distortionLookupTable.binsX(); ++x)
				{
					distortionLookupTable.setBinCenterValue(x, y, VectorT2<T>(0, 0));
				}
			}
		}
	}
}

template <typename T>
PinholeCameraT<T>::PinholeCameraT(const unsigned int width, const unsigned int height, const PinholeCameraT<T>& camera) :
	intrinsics_(false),
	invertedIntrinsics_(false),
	width_(width),
	height_(height),
	radialDistortion_(camera.radialDistortion_),
	tangentialDistortion_(camera.tangentialDistortion_)
{
	ocean_assert(camera.isValid());

	const T xFactor = T(width) / T(camera.width());
	const T yFactor = T(height) / T(camera.height());

	const T newPrincipalX = camera.principalPointX() * xFactor;
	const T newPrincipalY = camera.principalPointY() * yFactor;

	const T newFocalLengthX = camera.focalLengthX() * xFactor;
	const T newFocalLengthY = camera.focalLengthY() * yFactor;

	intrinsics_(0, 0) = newFocalLengthX;
	intrinsics_(1, 1) = newFocalLengthY;
	intrinsics_(0, 2) = newPrincipalX;
	intrinsics_(1, 2) = newPrincipalY;
	intrinsics_(2, 2) = 1;

	calculateInverseIntrinsic();
}

template <typename T>
PinholeCameraT<T>::PinholeCameraT(const T subFrameLeft, const T subFrameTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const PinholeCameraT<T>& camera) :
	intrinsics_(camera.intrinsics_),
	invertedIntrinsics_(false),
	width_(subFrameWidth),
	height_(subFrameHeight),
	radialDistortion_(camera.radialDistortion_),
	tangentialDistortion_(camera.tangentialDistortion_)
{
	ocean_assert(width_ >= 1u && height_ >= 1u);

	intrinsics_(0, 2) -= subFrameLeft;
	intrinsics_(1, 2) -= subFrameTop;

	calculateInverseIntrinsic();
}

template <typename T>
PinholeCameraT<T>::PinholeCameraT(const unsigned int width, const unsigned int height, const T focalX, const T focalY, const T principalX, const T principalY) :
	intrinsics_(false),
	invertedIntrinsics_(false),
	width_(width),
	height_(height),
	radialDistortion_(DistortionPair(T(0), T(0))),
	tangentialDistortion_(DistortionPair(T(0), T(0)))
{
	intrinsics_(0, 0) = focalX;
	intrinsics_(1, 1) = focalY;
	intrinsics_(0, 2) = principalX;
	intrinsics_(1, 2) = principalY;
	intrinsics_(2, 2) = 1;

	calculateInverseIntrinsic();
}

template <typename T>
PinholeCameraT<T>::PinholeCameraT(unsigned int width, unsigned int height, T focalX, T focalY, T principalX, T principalY, const DistortionPair& radial, const DistortionPair& tangential) :
	intrinsics_(false),
	invertedIntrinsics_(false),
	width_(width),
	height_(height),
	radialDistortion_(radial),
	tangentialDistortion_(tangential)
{
	intrinsics_(0, 0) = focalX;
	intrinsics_(1, 1) = focalY;
	intrinsics_(0, 2) = principalX;
	intrinsics_(1, 2) = principalY;
	intrinsics_(2, 2) = 1;

	calculateInverseIntrinsic();
}

template <typename T>
PinholeCameraT<T>::PinholeCameraT(const unsigned int width, const unsigned int height, const T* parameters, const bool radialDistortion, const bool tangentialDistortion) :
	intrinsics_(false),
	invertedIntrinsics_(false),
	width_(width),
	height_(height),
	radialDistortion_(radialDistortion ? DistortionPair(parameters[4], parameters[5]) : DistortionPair(T(0), T(0))),
	tangentialDistortion_(tangentialDistortion ? DistortionPair(parameters[6], parameters[7]) : DistortionPair(T(0), T(0)))
{
	intrinsics_(0, 0) = parameters[0];
	intrinsics_(1, 1) = parameters[1];
	intrinsics_(0, 2) = parameters[2];
	intrinsics_(1, 2) = parameters[3];
	intrinsics_(2, 2) = 1;

	calculateInverseIntrinsic();
}

template <typename T>
PinholeCameraT<T>::PinholeCameraT(const SquareMatrixT3<T>& intrinsic) :
	intrinsics_(intrinsic),
	invertedIntrinsics_(false),
	width_(0),
	height_(0),
	radialDistortion_(DistortionPair(T(0), T(0))),
	tangentialDistortion_(DistortionPair(T(0), T(0)))
{
	calculateInverseIntrinsic();
}

template <typename T>
PinholeCameraT<T>::PinholeCameraT(const SquareMatrixT3<T>& intrinsic, unsigned int width, unsigned int height) :
	intrinsics_(intrinsic),
	invertedIntrinsics_(false),
	width_(width),
	height_(height),
	radialDistortion_(DistortionPair(T(0), T(0))),
	tangentialDistortion_(DistortionPair(T(0), T(0)))
{
	calculateInverseIntrinsic();
}

template <typename T>
PinholeCameraT<T>::PinholeCameraT(const SquareMatrixT3<T>& intrinsic, unsigned int width, unsigned int height, const DistortionPair& radial, const DistortionPair& tangential) :
	intrinsics_(intrinsic),
	invertedIntrinsics_(false),
	width_(width),
	height_(height),
	radialDistortion_(radial),
	tangentialDistortion_(tangential)
{
	calculateInverseIntrinsic();
}

template <typename T>
PinholeCameraT<T>::PinholeCameraT(unsigned int width, unsigned int height, T fovX) :
	intrinsics_(false),
	invertedIntrinsics_(false),
	width_(width),
	height_(height),
	radialDistortion_(DistortionPair(T(0), T(0))),
	tangentialDistortion_(DistortionPair(T(0), T(0)))
{
	ocean_assert(width_ > 0u && height_ > 0u);
	ocean_assert(fovX > NumericT<T>::eps() && fovX < NumericT<T>::pi());

	const T principalX = T(width) * T(0.5);
	const T principalY = T(height) * T(0.5);

	const T focalLength = CameraT<T>::fieldOfViewToFocalLength(width, fovX);

#ifdef OCEAN_DEBUG
	const T debugAspectRatio = T(width) / T(height);
	const T debugFovY = CameraT<T>::fovX2Y(fovX, debugAspectRatio);

	const T debugFocalX = principalX / NumericT<T>::tan(fovX * T(0.5));
	const T debugFocalY = principalY / NumericT<T>::tan(debugFovY * T(0.5));

	ocean_assert(NumericT<T>::isWeakEqual(debugFocalX, focalLength));
	ocean_assert(NumericT<T>::isWeakEqual(debugFocalY, focalLength));
#endif

	intrinsics_(0, 0) = focalLength;
	intrinsics_(1, 1) = focalLength;

	intrinsics_(0, 2) = principalX;
	intrinsics_(1, 2) = principalY;

	intrinsics_(2, 2) = 1;

	calculateInverseIntrinsic();
}

template <typename T>
PinholeCameraT<T>::PinholeCameraT(unsigned int width, unsigned int height, T fovX, const T principalX, const T principalY) :
	intrinsics_(false),
	invertedIntrinsics_(false),
	width_(width),
	height_(height),
	radialDistortion_(DistortionPair(T(0), T(0))),
	tangentialDistortion_(DistortionPair(T(0), T(0)))
{
	ocean_assert(width_ > 0u && height_ > 0u);
	ocean_assert(fovX > 0 && fovX < NumericT<T>::pi());

	const T focal = (T(width) * T(0.5)) / NumericT<T>::tan(fovX * T(0.5));

	intrinsics_(0, 0) = focal;
	intrinsics_(1, 1) = focal;
	intrinsics_(2, 2) = 1;
	intrinsics_(0, 2) = principalX;
	intrinsics_(1, 2) = principalY;

	calculateInverseIntrinsic();
}

template <typename T>
T PinholeCameraT<T>::fovX() const
{
	/**
	 * x = Fx * X / Z + mx
	 *
	 * (x - mx) / Fx = X / Z
	 */

	// the real fovx depends on the principal point

	//       px
	// ---------------------------
	// \      |                  /
	//  \     |               /
	//   \ fa |            /
	//    \   |  fb     /
	//     \  |      /
	//      \ |   /
	//       \|/
	//
	// fov = fa + fb (if principal point is in the perfect center we have fa == fb)


	// in case the principal point is outside of the frame
	//
	//  px
	//          -------------
	//   |     /            /
	//   |    /          /
	//   |fa /        /
	//   |  /      /
	//   | / fb /
	//   |/  /
	//   |/
	//
	// fov = -fa + fb

	if (NumericT<T>::isEqualEps(focalLengthX()))
	{
		return T(0);
	}

	const T leftAngle = NumericT<T>::atan(principalPointX() * inverseFocalLengthX());

	const T rightAngle = NumericT<T>::atan((T(width_) - principalPointX()) * inverseFocalLengthX());

	ocean_assert(leftAngle + rightAngle > NumericT<T>::eps());

	return leftAngle + rightAngle;
}

template <typename T>
T PinholeCameraT<T>::fovY() const
{
	/**
	 * y = Fy * Y / Z + my
	 *
	 * (y - my) / Fy = Y / Z
	 */

	if (NumericT<T>::isEqualEps(focalLengthY()))
	{
		return T(0);
	}

	const T topAngle = NumericT<T>::atan(principalPointY() * inverseFocalLengthY());

	const T bottomAngle = NumericT<T>::atan((T(height_) - principalPointY()) * inverseFocalLengthY());

	ocean_assert(topAngle + bottomAngle > NumericT<T>::eps());

	return topAngle + bottomAngle;
}

template <typename T>
T PinholeCameraT<T>::fovXLeft() const
{
	if (NumericT<T>::isEqualEps(focalLengthX()))
	{
		return 0;
	}

	return NumericT<T>::atan2(principalPointX(), focalLengthX());
}

template <typename T>
T PinholeCameraT<T>::fovXRight() const
{
	if (NumericT<T>::isEqualEps(focalLengthX()))
	{
		return 0;
	}

	return NumericT<T>::atan2((T(width_) - principalPointX()), focalLengthX());
}

template <typename T>
T PinholeCameraT<T>::fovYTop() const
{
	if (NumericT<T>::isEqualEps(focalLengthY()))
	{
		return 0;
	}

	return NumericT<T>::atan2(principalPointY(), focalLengthY());
}

template <typename T>
T PinholeCameraT<T>::fovYBottom() const
{
	if (NumericT<T>::isEqualEps(focalLengthY()))
	{
		return 0;
	}

	return NumericT<T>::atan2((T(height_) - principalPointY()), focalLengthY());
}

template <typename T>
T PinholeCameraT<T>::fovDiagonal() const
{
	const VectorT2<T> topLeft(-principalPointX(), -principalPointY());
	const VectorT2<T> bottomRight(principalPointX(), principalPointY());

	const T diagonal = (topLeft - bottomRight).length();
	const T halfDiagonal = diagonal * T(0.5);

	const T focalLength = (focalLengthX() + focalLengthY()) * T(0.5);

	return T(2) * NumericT<T>::abs(NumericT<T>::atan(halfDiagonal / focalLength));
}

template <typename T>
bool PinholeCameraT<T>::rotation(const VectorT2<T>& undistortedPosition, T& angleX, T& angleY) const
{
	angleX = -NumericT<T>::atan((undistortedPosition(0) - principalPointX()) * inverseFocalLengthX());
	angleY = -NumericT<T>::atan((undistortedPosition(1) - principalPointY()) * inverseFocalLengthY());

	return true;
}

template <typename T>
void PinholeCameraT<T>::copyElements(T* arrayValues, const bool copyRadialDistortion, const bool copyTangentialDistortion) const
{
	ocean_assert(arrayValues);
	ocean_assert(!copyTangentialDistortion || copyRadialDistortion);

	arrayValues[0] = focalLengthX();
	arrayValues[1] = focalLengthY();
	arrayValues[2] = principalPointX();
	arrayValues[3] = principalPointY();

	if (copyRadialDistortion)
	{
		arrayValues[4] = radialDistortion().first;
		arrayValues[5] = radialDistortion().second;

		if (copyTangentialDistortion)
		{
			arrayValues[6] = tangentialDistortion().first;
			arrayValues[7] = tangentialDistortion().second;
		}
	}
}

template <typename T>
bool PinholeCameraT<T>::setIntrinsic(const SquareMatrixT3<T>& intrinsic)
{
	if (NumericT<T>::isNotEqualEps(intrinsic(1, 0)) || NumericT<T>::isNotEqualEps(intrinsic(2, 0))
		|| NumericT<T>::isNotEqualEps(intrinsic(2, 1)) || NumericT<T>::isNotEqualEps(intrinsic(0, 1))
		|| NumericT<T>::isNotEqual(intrinsic(2, 2), 1))
	{
		ocean_assert(false && "Invalid intrinsic!");
		return false;
	}

	intrinsics_ = intrinsic;
	calculateInverseIntrinsic();
	return true;
}

template <typename T>
void PinholeCameraT<T>::applyZoomFactor(const T relativeZoom)
{
	ocean_assert(relativeZoom > NumericT<T>::eps());

	intrinsics_(0, 0) *= relativeZoom;
	intrinsics_(1, 1) *= relativeZoom;

	calculateInverseIntrinsic();
}

template <typename T>
bool PinholeCameraT<T>::isDistortionPlausible(const T symmetricFocalLengthRatio, const T modelAccuracy, const T symmetricDistortionRatio) const
{
	ocean_assert(isValid());

	ocean_assert(symmetricFocalLengthRatio >= 1);
	ocean_assert(modelAccuracy > 0 && modelAccuracy < 1);
	ocean_assert(symmetricDistortionRatio >= 1);

	if (focalLengthX() >= focalLengthY())
	{
		if (focalLengthX() > focalLengthY() * symmetricFocalLengthRatio)
		{
			return false;
		}
	}
	else
	{
		if (focalLengthY() > focalLengthX() * symmetricFocalLengthRatio)
		{
			return false;
		}
	}

	const T modelAccuracyPixel = T(max(width_, height_)) * modelAccuracy;

	// we distort the corner positions in the frame and check whether the re-distorted positions match with the start positions (with a small tolerance value)

	if (distort<false>(undistort<false>(VectorT2<T>(0, 0))).sqrDistance(VectorT2<T>(0, 0)) > NumericT<T>::sqr(modelAccuracyPixel)
			|| distort<false>(undistort<false>(VectorT2<T>(T(width_), 0))).sqrDistance(VectorT2<T>(T(width_), 0)) > NumericT<T>::sqr(modelAccuracyPixel)
			|| distort<false>(undistort<false>(VectorT2<T>(0, T(height_)))).sqrDistance(VectorT2<T>(0, T(height_))) > NumericT<T>::sqr(modelAccuracyPixel)
			|| distort<false>(undistort<false>(VectorT2<T>(T(width_), T(height_)))).sqrDistance(VectorT2<T>(T(width_), T(height_))) > NumericT<T>::sqr(modelAccuracyPixel))
	{
		return false;
	}

	// we check whether we have an almost symmetric distortion by ensuring that the distance between the distorted frame corner positions and the center of the camera frame is almost identical

	const VectorT2<T> cameraCenter(T(width_) * T(0.5), T(height_) * T(0.5));

	const T distortedCornerDistances[4] =
	{
		cameraCenter.distance(distort<false>(VectorT2<T>(0, 0))),
		cameraCenter.distance(distort<false>(VectorT2<T>(T(width_), 0))),
		cameraCenter.distance(distort<false>(VectorT2<T>(0, T(height_)))),
		cameraCenter.distance(distort<false>(VectorT2<T>(T(width_), T(height_))))
	};

	for (unsigned int a = 0u; a < 3u; ++a)
	{
		for (unsigned int b = a + 1u; b < 4u; ++b)
		{
			if (distortedCornerDistances[a] >= distortedCornerDistances[b] * symmetricDistortionRatio || distortedCornerDistances[b] >= distortedCornerDistances[a] * symmetricDistortionRatio)
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T>
VectorT2<T> PinholeCameraT<T>::undistortDamped(const VectorT2<T>& distorted, const T dampingFactor, const unsigned int iterations, const T zoom) const
{
	ocean_assert(dampingFactor >= 0 && zoom > NumericT<T>::eps());
	ocean_assert(iterations >= 1u && iterations <= 1000);

	// check whether the camera is distortion free
	if (!hasDistortionParameters())
	{
		return distorted;
	}

	const T invZoom = T(1) / zoom;

	const VectorT2<T> nDistorted((distorted.x() - principalPointX()) * inverseFocalLengthX() * invZoom, (distorted.y() - principalPointY()) * inverseFocalLengthY() * invZoom);

	const VectorT2<T> nMainOffset(distortNormalizedDamped(nDistorted, dampingFactor, invZoom) - nDistorted);
	VectorT2<T> nIntermediateUndistorted(nDistorted - nMainOffset);

	unsigned int i = 0u;

	while (i++ < iterations)
	{
		const VectorT2<T> nIntermediateDistorted(distortNormalizedDamped(nIntermediateUndistorted, dampingFactor, invZoom));
		const VectorT2<T> nIntermediateOffset(nDistorted - nIntermediateDistorted);

		nIntermediateUndistorted = nIntermediateUndistorted + nIntermediateOffset * T(0.75);

		const T offsetPixelX = NumericT<T>::abs(nIntermediateOffset.x() * focalLengthX() * zoom);
		const T offsetPixelY = NumericT<T>::abs(nIntermediateOffset.y() * focalLengthY() * zoom);

		if (offsetPixelX < 0.05 && offsetPixelY < 0.05)
		{
			break;
		}

		if (offsetPixelX > T(width_ * 10u) || offsetPixelY > T(height_ * 10u))
		{
			return distorted;
		}
	}

	return VectorT2<T>(nIntermediateUndistorted.x() * focalLengthX() * zoom + principalPointX(), nIntermediateUndistorted.y() * focalLengthY() * zoom + principalPointY());
}

template <typename T>
VectorT2<T> PinholeCameraT<T>::distortDamped(const VectorT2<T>& undistorted, const T dampingFactor, const T zoom) const
{
	ocean_assert(isValid() && dampingFactor >= 0 && zoom > NumericT<T>::eps());

	if (!hasDistortionParameters())
	{
		return undistorted;
	}

	const T invZoom = T(1) / zoom;

	const VectorT2<T> nUndistorted((undistorted.x() - principalPointX()) * inverseFocalLengthX() * invZoom, (undistorted.y() - principalPointY()) * inverseFocalLengthY() * invZoom);

	const VectorT2<T> nDampedUndistorted(dampedNormalized(nUndistorted, dampingFactor, invZoom));
	const T dampedSqr = nDampedUndistorted.sqr();

	const T radialDistortionFactor = radialDistortion_.first * dampedSqr + radialDistortion_.second * NumericT<T>::sqr(dampedSqr);

	const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * nDampedUndistorted.x() * nDampedUndistorted.y()
														+ tangentialDistortion_.second * (dampedSqr + 2 * NumericT<T>::sqr(nDampedUndistorted.x()));

	const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (dampedSqr + 2 * NumericT<T>::sqr(nDampedUndistorted.y()))
														+ tangentialDistortion_.second * 2 * nDampedUndistorted.x() * nDampedUndistorted.y();

	return VectorT2<T>((nUndistorted.x() + nDampedUndistorted.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() * zoom + principalPointX(),
						(nUndistorted.y() + nDampedUndistorted.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() * zoom + principalPointY());
}

template <typename T>
T PinholeCameraT<T>::calculateAngleBetween(const VectorT2<T>& first, const VectorT2<T>& second) const
{
	const VectorT3<T> firstOnPlane((first(0) - principalPointX()) * inverseFocalLengthX(), (first(1) - principalPointY()) * inverseFocalLengthY(), 1);
	const VectorT3<T> secondOnPlane((second(0) - principalPointX()) * inverseFocalLengthX(), (second(1) - principalPointY()) * inverseFocalLengthY(), 1);

	return firstOnPlane.angle(secondOnPlane);
}

template <typename T>
T PinholeCameraT<T>::calculateCosBetween(const VectorT2<T>& first, const VectorT2<T>& second) const
{
	const VectorT3<T> firstOnPlane((first(0) - principalPointX()) * inverseFocalLengthX(), (first(1) - principalPointY()) * inverseFocalLengthY(), 1);
	const VectorT3<T> secondOnPlane((second(0) - principalPointX()) * inverseFocalLengthX(), (second(1) - principalPointY()) * inverseFocalLengthY(), 1);

	T value = firstOnPlane * secondOnPlane;

	ocean_assert(NumericT<T>::isNotEqualEps(firstOnPlane.length()) && NumericT<T>::isNotEqualEps(secondOnPlane.length()));
	value /= firstOnPlane.length();
	value /= secondOnPlane.length();

	// cosine rule c^2 = a^2 + b^2 - 2ab cos
	ocean_assert(NumericT<T>::isEqual(value, (firstOnPlane.sqr() + secondOnPlane.sqr() - firstOnPlane.sqrDistance(secondOnPlane)) / (2 * firstOnPlane.length() * secondOnPlane.length())));

	return value;
}

template <typename T>
VectorT2<T> PinholeCameraT<T>::projectToImageDampedIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const VectorT3<T>& objectPoint, const bool distortImagePoint, const T dampingFactor, const T zoom) const
{
	ocean_assert(iFlippedExtrinsic.isValid() && dampingFactor >= 0 && zoom > NumericT<T>::eps());

	const VectorT3<T> transformedObjectPoint(iFlippedExtrinsic * objectPoint);

	ocean_assert(NumericT<T>::isNotEqualEps(transformedObjectPoint.z()));
	const T factor = T(1) / transformedObjectPoint.z();

	const VectorT2<T> normalizedImagePoint(transformedObjectPoint.x() * factor, transformedObjectPoint.y() * factor);

	if (!distortImagePoint)
	{
		return VectorT2<T>(normalizedImagePoint.x() * focalLengthX() * zoom + principalPointX(), normalizedImagePoint.y() * focalLengthY() * zoom + principalPointY());
	}

	const T invZoom = T(1) / zoom;

	const VectorT2<T> dampedNormalizedImagePoint(dampedNormalized(normalizedImagePoint, dampingFactor, invZoom));

	const T sqr = dampedNormalizedImagePoint.sqr();

	const T radialDistortionFactor = radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

	const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * dampedNormalizedImagePoint.x() * dampedNormalizedImagePoint.y()
															+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(dampedNormalizedImagePoint.x()));

	const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(dampedNormalizedImagePoint.y()))
															+ tangentialDistortion_.second * 2 * dampedNormalizedImagePoint.x() * dampedNormalizedImagePoint.y();

	return VectorT2<T>((normalizedImagePoint.x() + dampedNormalizedImagePoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() * zoom + principalPointX(),
						(normalizedImagePoint.y() + dampedNormalizedImagePoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() * zoom + principalPointY());
}

template <typename T>
BoxT2<T> PinholeCameraT<T>::projectToImageDampedIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const BoxT3<T>& objectBox, const bool distortImagePoint, const T dampingFactor, const T zoom) const
{
	ocean_assert(iFlippedExtrinsic.isValid() && dampingFactor >= 0 && zoom > NumericT<T>::eps());
	ocean_assert(objectBox.isValid());

	VectorT3<T> boxImagePoints[8];
	const unsigned int numberBoxImagePoints = objectBox.corners(boxImagePoints);

	BoxT2<T> result;
	for (unsigned int n = 0; n < numberBoxImagePoints; ++n)
	{
		result += projectToImageDampedIF(iFlippedExtrinsic, boxImagePoints[n], distortImagePoint, dampingFactor, zoom);
	}

	return result;
}

template <typename T>
TriangleT2<T> PinholeCameraT<T>::projectToImageDampedIF(const HomogenousMatrixT4<T>& iFlippedExtrinsic, const TriangleT3<T>& objectTriangle, const bool distortImagePoint, const T dampingFactor, const T zoom) const
{
	ocean_assert(iFlippedExtrinsic.isValid() && dampingFactor >= 0 && zoom > NumericT<T>::eps());
	ocean_assert(objectTriangle.isValid());

	return TriangleT2<T>(projectToImageDampedIF(iFlippedExtrinsic, objectTriangle.point0(), distortImagePoint, dampingFactor, zoom),
						projectToImageDampedIF(iFlippedExtrinsic, objectTriangle.point1(), distortImagePoint, dampingFactor, zoom),
						projectToImageDampedIF(iFlippedExtrinsic, objectTriangle.point2(), distortImagePoint, dampingFactor, zoom));
}

template <typename T>
void PinholeCameraT<T>::projectToImageDampedIF(const HomogenousMatrixT4<T>& invertedFlippedExtrinsic, const VectorT3<T>* objectPoints, const size_t numberObjectPoints, const bool distortImagePoints, VectorT2<T>* imagePoints, const T dampingFactor, const T zoom) const
{
	ocean_assert(invertedFlippedExtrinsic.isValid() && dampingFactor >= 0 && zoom > NumericT<T>::eps());
	ocean_assert((objectPoints && imagePoints) || numberObjectPoints == 0u);

	if (distortImagePoints && hasDistortionParameters())
	{
		const T invZoom = T(1) / zoom;

		const T leftNormalizedBorder = -principalPointX() * inverseFocalLengthX() * invZoom;
		const T rightNormalizedBorder = (T(width_) - T(1) - principalPointX()) * inverseFocalLengthX() * invZoom;

		const T topNormalizedBorder = -principalPointY() * inverseFocalLengthY() * invZoom;
		const T bottomNormalizedBorder = (T(height_) - T(1) - principalPointY()) * inverseFocalLengthY() * invZoom;

		// if the camera does not provide tangential distortion
		if (tangentialDistortion_.first == 0 && tangentialDistortion_.second == 0)
		{
			for (size_t n = 0; n < numberObjectPoints; ++n)
			{
				const VectorT3<T> transformedObjectPoint(invertedFlippedExtrinsic * objectPoints[n]);

				ocean_assert(NumericT<T>::isNotEqualEps(transformedObjectPoint.z()));
				const T factor = T(1) / transformedObjectPoint.z();

				const VectorT2<T> normalizedImagePoint(transformedObjectPoint.x() * factor, transformedObjectPoint.y() * factor);
				const VectorT2<T> dampedNormalizedImagePoint(dampedNormalized(normalizedImagePoint, dampingFactor, leftNormalizedBorder, rightNormalizedBorder, topNormalizedBorder, bottomNormalizedBorder));

				const T sqr = dampedNormalizedImagePoint.sqr();
				const T radialDistortionFactor = radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

				imagePoints[n] = VectorT2<T>((normalizedImagePoint.x() + dampedNormalizedImagePoint.x() * radialDistortionFactor) * focalLengthX() * zoom + principalPointX(),
												(normalizedImagePoint.y() + dampedNormalizedImagePoint.y() * radialDistortionFactor) * focalLengthY() * zoom + principalPointY());
			}
		}
		else
		{
			ocean_assert(tangentialDistortion_.first != 0 || tangentialDistortion_.second != 0);

			for (size_t n = 0; n < numberObjectPoints; ++n)
			{
				const VectorT3<T> transformedObjectPoint(invertedFlippedExtrinsic * objectPoints[n]);

				ocean_assert(NumericT<T>::isNotEqualEps(transformedObjectPoint.z()));
				const T factor = T(1) / transformedObjectPoint.z();

				const VectorT2<T> normalizedImagePoint(transformedObjectPoint.x() * factor, transformedObjectPoint.y() * factor);
				const VectorT2<T> dampedNormalizedImagePoint(dampedNormalized(normalizedImagePoint, dampingFactor, leftNormalizedBorder, rightNormalizedBorder, topNormalizedBorder, bottomNormalizedBorder));

				const T sqr = dampedNormalizedImagePoint.sqr();
				const T radialDistortionFactor = radialDistortion_.first * sqr + radialDistortion_.second * NumericT<T>::sqr(sqr);

				const T tangentialDistortionCorrectionX = tangentialDistortion_.first * 2 * dampedNormalizedImagePoint.x() * dampedNormalizedImagePoint.y()
																		+ tangentialDistortion_.second * (sqr + 2 * NumericT<T>::sqr(dampedNormalizedImagePoint.x()));

				const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (sqr + 2 * NumericT<T>::sqr(dampedNormalizedImagePoint.y()))
																		+ tangentialDistortion_.second * 2 * dampedNormalizedImagePoint.x() * dampedNormalizedImagePoint.y();

				imagePoints[n] = VectorT2<T>((normalizedImagePoint.x() + dampedNormalizedImagePoint.x() * radialDistortionFactor + tangentialDistortionCorrectionX) * focalLengthX() * zoom + principalPointX(),
												(normalizedImagePoint.y() + dampedNormalizedImagePoint.y() * radialDistortionFactor + tangentialDistortionCorrectionY) * focalLengthY() * zoom + principalPointY());
			}
		}
	}
	else
	{
		// create one transformation matrix covering the entire pipeline (transformation and then projection)
		const HomogenousMatrixT4<T> transformationIF(transformationMatrixIF(invertedFlippedExtrinsic, zoom));
		ocean_assert(transformationIF.isValid());

		for (size_t n = 0; n < numberObjectPoints; ++n)
		{
			const VectorT3<T> transformedObjectPoint(transformationIF * objectPoints[n]);

			ocean_assert(NumericT<T>::isNotEqualEps(transformedObjectPoint.z()));
			const T factor = 1 / transformedObjectPoint.z();

			imagePoints[n] = VectorT2<T>(transformedObjectPoint.x() * factor, transformedObjectPoint.y() * factor);
		}
	}
}

template <typename T>
SquareMatrixT4<T> PinholeCameraT<T>::frustumMatrix(const T nearDistance, const T farDistance) const
{
	ocean_assert(nearDistance > 0 && nearDistance < farDistance);

	/**
	 * -principalPointX / focalLengthX = left / nearDistance
	 * (width - principalPointX) / focalLengthX = right / nearDistance
	 *
	 * principalPointY / focalLengthY = top / nearDistance
	 * -(height - principalPointY) / focalLengthY = bottom / nearDistance
	 */

	const T left = -principalPointX() * inverseFocalLengthX() * nearDistance;
	const T right = (T(width_) - principalPointX()) * inverseFocalLengthX() * nearDistance;

	const T top = principalPointY() * inverseFocalLengthY() * nearDistance;
	const T bottom = -(T(height_) - principalPointY()) * inverseFocalLengthY() * nearDistance;

	ocean_assert(left < right);
	ocean_assert(bottom < top);

	return SquareMatrixT4<T>::frustumMatrix(left, right, top, bottom, nearDistance, farDistance);
}

template <typename T>
bool PinholeCameraT<T>::isEqual(const PinholeCameraT<T>& camera, const T eps) const
{
	return width_ == camera.width_ && height_ == camera.height_
				&& intrinsics_.isEqual(camera.intrinsics_, eps)
				&& NumericT<T>::isEqual(radialDistortion_.first, camera.radialDistortion_.first, eps)
				&& NumericT<T>::isEqual(radialDistortion_.second, camera.radialDistortion_.second, eps)
				&& NumericT<T>::isEqual(tangentialDistortion_.first, camera.tangentialDistortion_.first, eps)
				&& NumericT<T>::isEqual(tangentialDistortion_.second, camera.tangentialDistortion_.second, eps);
}

template <typename T>
bool PinholeCameraT<T>::operator==(const PinholeCameraT<T>& camera) const
{
	return isEqual(camera, NumericT<T>::eps());
}

template <typename T>
VectorT2<T> PinholeCameraT<T>::distortNormalizedDamped(const VectorT2<T>& undistortedNormalized, const T dampingFactor, const T invZoom) const
{
	ocean_assert(dampingFactor >= 0 && invZoom >= NumericT<T>::eps());

	if (!hasDistortionParameters())
	{
		return undistortedNormalized;
	}

	const VectorT2<T> nDampedUndistorted(dampedNormalized(undistortedNormalized, dampingFactor, invZoom));
	const T dampedSqr = nDampedUndistorted.sqr();

	const T radialDistortionFactor = radialDistortion_.first * dampedSqr + radialDistortion_.second * NumericT<T>::sqr(dampedSqr);

	const T tangentialDistortionCorrectionX = tangentialDistortion_.first * T(2) * nDampedUndistorted.x() * nDampedUndistorted.y()
																+ tangentialDistortion_.second * (dampedSqr + T(2) * NumericT<T>::sqr(nDampedUndistorted.x()));

	const T tangentialDistortionCorrectionY = tangentialDistortion_.first * (dampedSqr + T(2) * NumericT<T>::sqr(nDampedUndistorted.y()))
																+ tangentialDistortion_.second * T(2) * nDampedUndistorted.x() * nDampedUndistorted.y();

	return VectorT2<T>(undistortedNormalized.x() + nDampedUndistorted.x() * radialDistortionFactor + tangentialDistortionCorrectionX,
								undistortedNormalized.y() + nDampedUndistorted.y() * radialDistortionFactor + tangentialDistortionCorrectionY);
}

template <typename T>
VectorT2<T> PinholeCameraT<T>::dampedNormalized(const VectorT2<T>& normalized, const T dampingFactor, const T invZoom) const
{
	ocean_assert(dampingFactor >= 0 && invZoom >= NumericT<T>::eps());

	const T u0 = -principalPointX() * inverseFocalLengthX() * invZoom;
	const T u1 = (T(width_) - T(1) - principalPointX()) * inverseFocalLengthX() * invZoom;

	const T v0 = -principalPointY() * inverseFocalLengthY() * invZoom;
	const T v1 = (T(height_) - T(1) - principalPointY()) * inverseFocalLengthY() * invZoom;

	VectorT2<T> nDampedUndistorted(normalized);

	if (nDampedUndistorted.x() < u0)
	{
		ocean_assert(NumericT<T>::isNotEqualEps(normalized.x() + u0 * (dampingFactor - 1)));
		nDampedUndistorted.x() = u0 * (1 + dampingFactor) - (dampingFactor * dampingFactor * u0 * u0) / (normalized.x() + u0 * (dampingFactor - 1));
	}
	else if (nDampedUndistorted.x() > u1)
	{
		ocean_assert(NumericT<T>::isNotEqualEps(normalized.x() + u1 * (dampingFactor - 1)));
		nDampedUndistorted.x() = u1 * (1 + dampingFactor) - (dampingFactor * dampingFactor * u1 * u1) / (normalized.x() + u1 * (dampingFactor - 1));
	}

	if (nDampedUndistorted.y() < v0)
	{
		ocean_assert(NumericT<T>::isNotEqualEps(normalized.y() + v0 * (dampingFactor - 1)));
		nDampedUndistorted.y() = v0 * (1 + dampingFactor) - (dampingFactor * dampingFactor * v0 * v0) / (normalized.y() + v0 * (dampingFactor - 1));
	}
	else if (nDampedUndistorted.y() > v1)
	{
		ocean_assert(NumericT<T>::isNotEqualEps(normalized.y() + v1 * (dampingFactor - 1)));
		nDampedUndistorted.y() = v1 * (1 + dampingFactor) - (dampingFactor * dampingFactor * v1 * v1) / (normalized.y() + v1 * (dampingFactor - 1));
	}

	return nDampedUndistorted;
}

template <typename T>
VectorT2<T> PinholeCameraT<T>::dampedNormalized(const VectorT2<T>& normalized, const T dampingFactor, const T leftNormalizedBorder, const T rightNormalizedBorder, const T topNormalizedBorder, const T bottomNormalizedBorder) const
{
	ocean_assert(dampingFactor >= 0);

	VectorT2<T> nDampedUndistorted(normalized);

	if (nDampedUndistorted.x() < leftNormalizedBorder)
	{
		ocean_assert(NumericT<T>::isNotEqualEps(normalized.x() + leftNormalizedBorder * (dampingFactor - 1)));
		nDampedUndistorted.x() = leftNormalizedBorder * (1 + dampingFactor) - (dampingFactor * dampingFactor * leftNormalizedBorder * leftNormalizedBorder) / (normalized.x() + leftNormalizedBorder * (dampingFactor - 1));
	}
	else if (nDampedUndistorted.x() > rightNormalizedBorder)
	{
		ocean_assert(NumericT<T>::isNotEqualEps(normalized.x() + rightNormalizedBorder * (dampingFactor - 1)));
		nDampedUndistorted.x() = rightNormalizedBorder * (1 + dampingFactor) - (dampingFactor * dampingFactor * rightNormalizedBorder * rightNormalizedBorder) / (normalized.x() + rightNormalizedBorder * (dampingFactor - 1));
	}

	if (nDampedUndistorted.y() < topNormalizedBorder)
	{
		ocean_assert(NumericT<T>::isNotEqualEps(normalized.y() + topNormalizedBorder * (dampingFactor - 1)));
		nDampedUndistorted.y() = topNormalizedBorder * (1 + dampingFactor) - (dampingFactor * dampingFactor * topNormalizedBorder * topNormalizedBorder) / (normalized.y() + topNormalizedBorder * (dampingFactor - 1));
	}
	else if (nDampedUndistorted.y() > bottomNormalizedBorder)
	{
		ocean_assert(NumericT<T>::isNotEqualEps(normalized.y() + bottomNormalizedBorder * (dampingFactor - 1)));
		nDampedUndistorted.y() = bottomNormalizedBorder * (1 + dampingFactor) - (dampingFactor * dampingFactor * bottomNormalizedBorder * bottomNormalizedBorder) / (normalized.y() + bottomNormalizedBorder * (dampingFactor - 1));
	}

	return nDampedUndistorted;
}

// Explicit instantiations
template class OCEAN_MATH_EXPORT PinholeCameraT<float>;
template class OCEAN_MATH_EXPORT PinholeCameraT<double>;

}
