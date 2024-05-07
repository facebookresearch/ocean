// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

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

HomogenousMatrix4 Utilities::viewPosition(const AnyCamera& anyCamera, const Sphere3& boundingSphere, const Vector3& viewingDirection)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(boundingSphere.isValid());

	const Scalar fov = min(anyCamera.fovX(), anyCamera.fovY());
	const Scalar fov_2 = fov * Scalar(0.5);
	ocean_assert(fov_2 > Numeric::eps());

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

HomogenousMatrix4 Utilities::viewPosition(const PinholeCamera& pinholeCamera, const Vectors3& objectPoints, const Vector3& viewingDirection, const bool circumcircle)
{
	ocean_assert(pinholeCamera.isValid());
	ocean_assert(!objectPoints.empty());

	const Box3 boundingBox(objectPoints);

	const Sphere3 boundingSphere(circumcircle ? Sphere3(boundingBox) : Sphere3(boundingBox.center(), max(boundingBox.xDimension(), max(boundingBox.yDimension(), boundingBox.zDimension())) * Scalar(0.75)));

	return viewPosition(pinholeCamera, boundingSphere, viewingDirection);
}

HomogenousMatrix4 Utilities::viewPosition(const AnyCamera& anyCamera, const Vectors3& objectPoints, const Vector3& viewingDirection, const bool circumcircle)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(!objectPoints.empty());

	const Box3 boundingBox(objectPoints);

	const Sphere3 boundingSphere(circumcircle ? Sphere3(boundingBox) : Sphere3(boundingBox.center(), max(boundingBox.xDimension(), max(boundingBox.yDimension(), boundingBox.zDimension())) * Scalar(0.75)));

	return viewPosition(anyCamera, boundingSphere, viewingDirection);
}

HomogenousMatrix4 Utilities::viewPosition(const PinholeCamera& pinholeCamera, const Vectors3& objectPoints, const bool circumcircle, RandomGenerator* randomGenerator)
{
	const Vector3 viewingDirection(randomGenerator ? Random::vector3(*randomGenerator) : Random::vector3());
	return viewPosition(pinholeCamera, objectPoints, viewingDirection, circumcircle);
}

HomogenousMatrix4 Utilities::viewPosition(const AnyCamera& anyCamera, const Vectors3& objectPoints, const bool circumcircle, RandomGenerator* randomGenerator)
{
	const Vector3 viewingDirection(randomGenerator ? Random::vector3(*randomGenerator) : Random::vector3());
	return viewPosition(anyCamera, objectPoints, viewingDirection, circumcircle);
}

PinholeCamera Utilities::distortedCamera(const PinholeCamera& pinholeCamera, const bool principalPointDistortion, const bool radialDistortion, const bool tangentialDistortion, const Scalar maximalPrincipalPointOffset, const Scalar maximalRadialDistortion, const Scalar maximalTangentialDistortion, RandomGenerator* randomGenerator)
{
	ocean_assert(pinholeCamera.isValid());

	PinholeCamera result(pinholeCamera);

	if (principalPointDistortion)
	{
		ocean_assert(maximalPrincipalPointOffset >= 0);
		ocean_assert(maximalPrincipalPointOffset < Scalar(pinholeCamera.width() / 4u));
		ocean_assert(maximalPrincipalPointOffset < Scalar(pinholeCamera.height() / 4u));

		const Scalar xOffset = randomGenerator ? Random::scalar(*randomGenerator, -maximalPrincipalPointOffset, maximalPrincipalPointOffset) : Random::scalar(-maximalPrincipalPointOffset, maximalPrincipalPointOffset);
		const Scalar yOffset = randomGenerator ? Random::scalar(*randomGenerator, -maximalPrincipalPointOffset, maximalPrincipalPointOffset) : Random::scalar(-maximalPrincipalPointOffset, maximalPrincipalPointOffset);

		SquareMatrix3 intrinsic(pinholeCamera.intrinsic());
		intrinsic(0, 2) += xOffset;
		intrinsic(1, 2) += yOffset;

		result = PinholeCamera(intrinsic, pinholeCamera.width(), pinholeCamera.height());
	}

	if (radialDistortion)
	{
		ocean_assert(maximalRadialDistortion >= 0);

		const Scalar k1 = randomGenerator ? Random::scalar(*randomGenerator, -maximalRadialDistortion, maximalRadialDistortion) : Random::scalar(-maximalRadialDistortion, maximalRadialDistortion);
		const Scalar k2 = randomGenerator ? Random::scalar(*randomGenerator, -maximalRadialDistortion, maximalRadialDistortion) : Random::scalar(-maximalRadialDistortion, maximalRadialDistortion);
		result.setRadialDistortion(PinholeCamera::DistortionPair(k1, k2));
	}

	if (tangentialDistortion)
	{
		const Scalar p1 = randomGenerator ? Random::scalar(*randomGenerator, -maximalTangentialDistortion, maximalTangentialDistortion) : Random::scalar(-maximalTangentialDistortion, maximalTangentialDistortion);
		const Scalar p2 = randomGenerator ? Random::scalar(*randomGenerator, -maximalTangentialDistortion, maximalTangentialDistortion) : Random::scalar(-maximalTangentialDistortion, maximalTangentialDistortion);
		result.setTangentialDistortion(PinholeCamera::DistortionPair(p1, p2));
	}

	return result;
}

PinholeCamera Utilities::realisticPinholeCamera(const unsigned int index)
{
	ocean_assert(index <= 1u);

	if (index == 0u)
	{
		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar focalLengthX = Scalar(685.126);
		const Scalar focalLengthY = Scalar(695.00901883);

		const Scalar principalX = Scalar(309.097);
		const Scalar principalY = Scalar(232.743);

		const PinholeCamera::DistortionPair radialDistortion(Scalar(0), Scalar(0));
		const PinholeCamera::DistortionPair tangentialDistortion(Scalar(0), Scalar(0));

		return PinholeCamera(width, height,  focalLengthX, focalLengthY, principalX, principalY, radialDistortion, tangentialDistortion);
	}
	else
	{
		const unsigned int width = 1980u;
		const unsigned int height = 1080u;

		const Scalar focalLengthX = Scalar(1265.43456618);
		const Scalar focalLengthY = Scalar(1286.62742462);

		const Scalar principalX = Scalar(943.35628029);
		const Scalar principalY = Scalar(573.48833202);

		const PinholeCamera::DistortionPair radialDistortion(Scalar(0), Scalar(0));
		const PinholeCamera::DistortionPair tangentialDistortion(Scalar(0), Scalar(0));

		return PinholeCamera(width, height,  focalLengthX, focalLengthY, principalX, principalY, radialDistortion, tangentialDistortion);
	}
}

FisheyeCamera Utilities::realisticFisheyeCamera(const unsigned int index)
{
	ocean_assert(index <= 1u);

	if (index == 0u)
	{
		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar focalLength = Scalar(191.787);

		const Scalar principalX = Scalar(314.304);
		const Scalar principalY = Scalar(243.711);

		const Scalars radialDistortions = {Scalar(0.29192), Scalar(0.00329052), Scalar(-0.151158), Scalar(0.0952214), Scalar(-0.0230753), Scalar(0.00194645)};
		const Scalars tangentialDistortions = {Scalar(0.0003358), Scalar(-0.000474032)};

		return FisheyeCamera(width, height,  focalLength, focalLength, principalX, principalY, radialDistortions.data(), tangentialDistortions.data());
	}
	else
	{
		const unsigned int width = 640u;
		const unsigned int height = 480u;

		const Scalar focalLength = Scalar(191.3389);

		const Scalar principalX = Scalar(318.2509);
		const Scalar principalY = Scalar(244.1959);

		const Scalars radialDistortions = {Scalar(0.286137), Scalar(0.0375097), Scalar(-0.229221), Scalar(0.178376), Scalar(-0.0642987), Scalar(0.00963856)};
		const Scalars tangentialDistortions = {Scalar(0.000138048), Scalar(-0.000289689)};

		return FisheyeCamera(width, height,  focalLength, focalLength, principalX, principalY, radialDistortions.data(), tangentialDistortions.data());
	}
}

SharedAnyCamera Utilities::realisticAnyCamera(const AnyCameraType anyCameraType, const unsigned int index)
{
	ocean_assert(index <= 1u);

	switch (anyCameraType)
	{
		case AnyCameraType::INVALID:
			ocean_assert(false && "Invalid camera type!");
			return nullptr;

		case AnyCameraType::PINHOLE:
			return std::make_shared<AnyCameraPinhole>(realisticPinholeCamera(index));

		case AnyCameraType::FISHEYE:
			return std::make_shared<AnyCameraFisheye>(realisticFisheyeCamera(index));
	}


	ocean_assert(false && "Invalid camera type!");
	return nullptr;
}

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
