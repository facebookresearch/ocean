/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testgeometry/Utilities.h"

#include "ocean/math/Random.h"
#include "ocean/math/Sphere3.h"

namespace Ocean
{

namespace Test
{

namespace TestGeometry
{

Vectors2 Utilities::imagePoints(const PinholeCamera& pinholeCamera, const size_t number, RandomGenerator* randomGenerator)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(number > 0);

	Vectors2 result(number);

	if (randomGenerator)
	{
		for (size_t n = 0; n < number; ++n)
			result[n] = Vector2(Random::scalar(*randomGenerator, Scalar(0), Scalar(pinholeCamera.width())), Random::scalar(*randomGenerator, Scalar(0), Scalar(pinholeCamera.height())));
	}
	else
	{
		for (size_t n = 0; n < number; ++n)
			result[n] = Vector2(Random::scalar(Scalar(0), Scalar(pinholeCamera.width())), Random::scalar(Scalar(0), Scalar(pinholeCamera.height())));
	}

	return result;
}

Vectors3 Utilities::objectPoints(const Box3& boundingBox, const size_t number, RandomGenerator* randomGenerator)
{
	ocean_assert(boundingBox.isValid());
	ocean_assert(number > 0);

	Vectors3 result(number);

	for (size_t n = 0; n < number; ++n)
	{
		if (randomGenerator)
		{
			result[n] = Vector3(Random::scalar(*randomGenerator, boundingBox.lower().x(), boundingBox.higher().x()),
										Random::scalar(*randomGenerator, boundingBox.lower().y(), boundingBox.higher().y()),
										Random::scalar(*randomGenerator, boundingBox.lower().z(), boundingBox.higher().z()));
		}
		else
		{
			result[n] = Vector3(Random::scalar(boundingBox.lower().x(), boundingBox.higher().x()),
										Random::scalar(boundingBox.lower().y(), boundingBox.higher().y()),
										Random::scalar(boundingBox.lower().z(), boundingBox.higher().z()));
		}

		ocean_assert(boundingBox.isInside(result[n]));
	}

	ocean_assert(result.size() == number);
	return result;
}

HomogenousMatrix4 Utilities::viewPosition(const PinholeCamera& pinholeCamera, const Sphere3& boundingSphere, const Vector3& viewingDirection)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(boundingSphere.isValid());

	const Scalar fov_2 = min(min(pinholeCamera.fovXLeft(), pinholeCamera.fovXRight()), min(pinholeCamera.fovYTop(), pinholeCamera.fovYBottom()));
	ocean_assert(fov_2 >= 0);

	const Scalar sideDistance = boundingSphere.radius() / Numeric::sin(fov_2);
	const Scalar viewDistance = Numeric::sqrt(Numeric::sqr(sideDistance) + Numeric::sqr(boundingSphere.radius()));

	ocean_assert(Numeric::isEqual(viewingDirection.length(), 1));

	const Vector3 newViewPosition = boundingSphere.center() - viewingDirection * viewDistance;
	HomogenousMatrix4 world_T_camera(newViewPosition, Rotation(Vector3(0, 0, -1), viewingDirection));

	if (Numeric::isEqualEps(boundingSphere.radius()))
	{
		// in case the sphere is a point, we move the camera slightly back to ensure that sphere is still slightly in front of the camera)
		world_T_camera *= HomogenousMatrix4(Vector3(0, 0, Scalar(0.0001)));
	}

	return world_T_camera;
}

template <typename T>
HomogenousMatrixT4<T> Utilities::viewPosition(const AnyCameraT<T>& anyCamera, const SphereT3<T>& boundingSphere, const VectorT3<T>& viewingDirection)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(boundingSphere.isValid());

	const T fov = min(anyCamera.fovX(), anyCamera.fovY());
	const T fov_2 = fov * T(0.5);
	ocean_assert(fov_2 > NumericT<T>::eps());

	const T sideDistance = boundingSphere.radius() / NumericT<T>::sin(fov_2);
	const T viewDistance = NumericT<T>::sqrt(NumericT<T>::sqr(sideDistance) + NumericT<T>::sqr(boundingSphere.radius()));

	ocean_assert(NumericT<T>::isEqual(viewingDirection.length(), 1));

	const VectorT3<T> newViewPosition = boundingSphere.center() - viewingDirection * viewDistance;
	HomogenousMatrixT4<T> world_T_camera(newViewPosition, RotationT<T>(VectorT3<T>(0, 0, -1), viewingDirection));

	if (NumericT<T>::isEqualEps(boundingSphere.radius()))
	{
		// in case the sphere is a point, we move the camera slightly back to ensure that sphere is still slightly in front of the camera)
		world_T_camera *= HomogenousMatrixT4<T>(VectorT3<T>(0, 0, T(0.0001)));
	}

	return world_T_camera;
}

template HomogenousMatrixT4<float> OCEAN_TEST_GEOMETRY_EXPORT Utilities::viewPosition(const AnyCameraT<float>& anyCamera, const SphereT3<float>& boundingSphere, const VectorT3<float>& viewingDirection);
template HomogenousMatrixT4<double> OCEAN_TEST_GEOMETRY_EXPORT Utilities::viewPosition(const AnyCameraT<double>& anyCamera, const SphereT3<double>& boundingSphere, const VectorT3<double>& viewingDirection);

HomogenousMatrix4 Utilities::viewPosition(const PinholeCamera& pinholeCamera, const Vectors3& objectPoints, const Vector3& viewingDirection, const bool circumcircle)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(!objectPoints.empty());

	const Box3 boundingBox(objectPoints);

	const Sphere3 boundingSphere(circumcircle ? Sphere3(boundingBox) : Sphere3(boundingBox.center(), max(boundingBox.xDimension(), max(boundingBox.yDimension(), boundingBox.zDimension())) * Scalar(0.75)));

	return viewPosition(pinholeCamera, boundingSphere, viewingDirection);
}

template <typename T>
HomogenousMatrixT4<T> Utilities::viewPosition(const AnyCameraT<T>& anyCamera, const VectorsT3<T>& objectPoints, const VectorT3<T>& viewingDirection, const bool circumcircle)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(!objectPoints.empty());

	const BoxT3<T> boundingBox(objectPoints);

	if (circumcircle)
	{
		return viewPosition(anyCamera, SphereT3<T>(boundingBox), viewingDirection);
	}
	else
	{
		const SphereT3<T> boundingSphere(boundingBox.center(), max(boundingBox.xDimension(), max(boundingBox.yDimension(), boundingBox.zDimension())) * T(0.75));

		return viewPosition(anyCamera, boundingSphere, viewingDirection);
	}
}

template HomogenousMatrixT4<float> OCEAN_TEST_GEOMETRY_EXPORT Utilities::viewPosition(const AnyCameraT<float>& anyCamera, const VectorsT3<float>& objectPoints, const VectorT3<float>& viewingDirection, const bool circumcircle);
template HomogenousMatrixT4<double> OCEAN_TEST_GEOMETRY_EXPORT Utilities::viewPosition(const AnyCameraT<double>& anyCamera, const VectorsT3<double>& objectPoints, const VectorT3<double>& viewingDirection, const bool circumcircle);

HomogenousMatrix4 Utilities::viewPosition(const PinholeCamera& pinholeCamera, const Vectors3& objectPoints, const bool circumcircle, RandomGenerator* randomGenerator)
{
	const Vector3 viewingDirection(randomGenerator ? Random::vector3(*randomGenerator) : Random::vector3());
	return viewPosition(pinholeCamera, objectPoints, viewingDirection, circumcircle);
}

template <typename T>
HomogenousMatrixT4<T> Utilities::viewPosition(const AnyCameraT<T>& anyCamera, const VectorsT3<T>& objectPoints, const bool circumcircle, RandomGenerator* randomGenerator)
{
	const VectorT3<T> viewingDirection(randomGenerator ? RandomT<T>::vector3(*randomGenerator) : RandomT<T>::vector3());

	return viewPosition(anyCamera, objectPoints, viewingDirection, circumcircle);
}

template HomogenousMatrixT4<float> OCEAN_TEST_GEOMETRY_EXPORT Utilities::viewPosition(const AnyCameraT<float>& anyCamera, const VectorsT3<float>& objectPoints, const bool circumcircle, RandomGenerator* randomGenerator);
template HomogenousMatrixT4<double> OCEAN_TEST_GEOMETRY_EXPORT Utilities::viewPosition(const AnyCameraT<double>& anyCamera, const VectorsT3<double>& objectPoints, const bool circumcircle, RandomGenerator* randomGenerator);

PinholeCamera Utilities::distortedCamera(const PinholeCamera& pinholeCamera, const bool principalPointDistortion, const bool radialDistortion, const bool tangentialDistortion, RandomGenerator* randomGenerator, const Scalar maximalPrincipalPointOffset, const Scalar maximalRadialDistortion, const Scalar maximalTangentialDistortion)
{
	ocean_assert(pinholeCamera.isValid());

	PinholeCamera result(pinholeCamera);

	RandomGenerator localRandomGenerator(randomGenerator);

	if (principalPointDistortion)
	{
		ocean_assert(maximalPrincipalPointOffset >= 0);
		ocean_assert(maximalPrincipalPointOffset < Scalar(pinholeCamera.width() / 4u));
		ocean_assert(maximalPrincipalPointOffset < Scalar(pinholeCamera.height() / 4u));

		const Scalar xOffset = Random::scalar(localRandomGenerator, -maximalPrincipalPointOffset, maximalPrincipalPointOffset);
		const Scalar yOffset = Random::scalar(localRandomGenerator, -maximalPrincipalPointOffset, maximalPrincipalPointOffset);

		SquareMatrix3 intrinsic(pinholeCamera.intrinsic());
		intrinsic(0, 2) += xOffset;
		intrinsic(1, 2) += yOffset;

		result = PinholeCamera(intrinsic, pinholeCamera.width(), pinholeCamera.height());
	}

	if (radialDistortion)
	{
		ocean_assert(maximalRadialDistortion >= 0);

		const Scalar k1 = Random::scalar(localRandomGenerator, -maximalRadialDistortion, maximalRadialDistortion);
		const Scalar k2 = Random::scalar(localRandomGenerator, -maximalRadialDistortion, maximalRadialDistortion);

		result.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
	}

	if (tangentialDistortion)
	{
		const Scalar p1 = Random::scalar(localRandomGenerator, -maximalTangentialDistortion, maximalTangentialDistortion);
		const Scalar p2 = Random::scalar(localRandomGenerator, -maximalTangentialDistortion, maximalTangentialDistortion);

		result.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));
	}

	return result;
}

template <typename T>
PinholeCameraT<T> Utilities::realisticPinholeCamera(const unsigned int index)
{
	ocean_assert(index <= 1u);

	if (index == 0u)
	{
		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const T focalLengthX = T(685.126);
		const T focalLengthY = T(695.00901883);

		const T principalX = T(309.097);
		const T principalY = T(232.743);

		const typename PinholeCameraT<T>::DistortionPair radialDistortion(T(0), T(0));
		const typename PinholeCameraT<T>::DistortionPair tangentialDistortion(T(0), T(0));

		return PinholeCameraT<T>(width, height,  focalLengthX, focalLengthY, principalX, principalY, radialDistortion, tangentialDistortion);
	}
	else
	{
		const unsigned int width = 1980u;
		const unsigned int height = 1080u;

		const T focalLengthX = T(1265.43456618);
		const T focalLengthY = T(1286.62742462);

		const T principalX = T(943.35628029);
		const T principalY = T(573.48833202);

		const typename PinholeCameraT<T>::DistortionPair radialDistortion(T(0), T(0));
		const typename PinholeCameraT<T>::DistortionPair tangentialDistortion(T(0), T(0));

		return PinholeCameraT<T>(width, height,  focalLengthX, focalLengthY, principalX, principalY, radialDistortion, tangentialDistortion);
	}
}

template PinholeCameraT<float> OCEAN_TEST_GEOMETRY_EXPORT Utilities::realisticPinholeCamera(const unsigned int index);
template PinholeCameraT<double> OCEAN_TEST_GEOMETRY_EXPORT Utilities::realisticPinholeCamera(const unsigned int index);

template <typename T>
FisheyeCameraT<T> Utilities::realisticFisheyeCamera(const unsigned int index)
{
	ocean_assert(index <= 1u);

	if (index == 0u)
	{
		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const T focalLength = T(191.787);

		const T principalX = T(314.304);
		const T principalY = T(243.711);

		const std::vector<T> radialDistortions = {T(0.29192), T(0.00329052), T(-0.151158), T(0.0952214), T(-0.0230753), T(0.00194645)};
		const std::vector<T> tangentialDistortions = {T(0.0003358), T(-0.000474032)};

		return FisheyeCameraT<T>(width, height,  focalLength, focalLength, principalX, principalY, radialDistortions.data(), tangentialDistortions.data());
	}
	else
	{
		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const T focalLength = T(191.3389);

		const T principalX = T(318.2509);
		const T principalY = T(244.1959);

		const std::vector<T> radialDistortions = {T(0.286137), T(0.0375097), T(-0.229221), T(0.178376), T(-0.0642987), T(0.00963856)};
		const std::vector<T> tangentialDistortions = {T(0.000138048), T(-0.000289689)};

		return FisheyeCameraT<T>(width, height,  focalLength, focalLength, principalX, principalY, radialDistortions.data(), tangentialDistortions.data());
	}
}

template FisheyeCameraT<float> OCEAN_TEST_GEOMETRY_EXPORT Utilities::realisticFisheyeCamera(const unsigned int index);
template FisheyeCameraT<double> OCEAN_TEST_GEOMETRY_EXPORT Utilities::realisticFisheyeCamera(const unsigned int index);

template <typename T>
SharedAnyCameraT<T> Utilities::realisticAnyCamera(const AnyCameraType anyCameraType, const unsigned int index)
{
	ocean_assert(index <= 1u);

	switch (anyCameraType)
	{
		case AnyCameraType::INVALID:
			ocean_assert(false && "Invalid camera type!");
			return nullptr;

		case AnyCameraType::PINHOLE:
			return std::make_shared<AnyCameraPinholeT<T>>(realisticPinholeCamera<T>(index));

		case AnyCameraType::FISHEYE:
			return std::make_shared<AnyCameraFisheyeT<T>>(realisticFisheyeCamera<T>(index));
	}

	ocean_assert(false && "Invalid camera type!");
	return nullptr;
}

template SharedAnyCameraT<float> OCEAN_TEST_GEOMETRY_EXPORT Utilities::realisticAnyCamera(const AnyCameraType anyCameraType, const unsigned int index);
template SharedAnyCameraT<double> OCEAN_TEST_GEOMETRY_EXPORT Utilities::realisticAnyCamera(const AnyCameraType anyCameraType, const unsigned int index);

std::vector<AnyCameraType> Utilities::realisticCameraTypes()
{
	return {AnyCameraType::PINHOLE, AnyCameraType::FISHEYE};
}

IndexSet32 Utilities::randomIndices(const size_t size, const size_t randomIndices, RandomGenerator* randomGenerator)
{
	ocean_assert(randomIndices <= size);

	IndexSet32 result;

	if (randomIndices < size)
	{
		if (randomGenerator)
		{
			while (result.size() < randomIndices)
			{
				result.insert(RandomI::random(*randomGenerator, (unsigned int)size - 1u));
			}
		}
		else
		{
			while (result.size() < randomIndices)
			{
				result.insert(RandomI::random((unsigned int)size - 1u));
			}
		}
	}
	else
	{
		for (unsigned int n = 0u; n < (unsigned int)randomIndices; ++n)
		{
			result.insert(n);
		}
	}

	return result;
}

}

}

}
