/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/PnP.h"
#include "ocean/geometry/AbsoluteTransformation.h"

#include "ocean/math/Matrix.h"

namespace Ocean
{

namespace Geometry
{

bool PnP::pose(const AnyCamera& camera, const Vector3* objectPoints, const Vector2* imagePoints, const size_t correspondences, HomogenousMatrix4& world_T_camera)
{
	ocean_assert(objectPoints != nullptr && imagePoints != nullptr);
	ocean_assert(correspondences >= 5);

	if (correspondences < 5)
	{
		return false;
	}

	Vectors3 imageRays;
	imageRays.reserve(correspondences);

	for (size_t n = 0; n < correspondences; ++n)
	{
		imageRays.emplace_back(camera.vector(imagePoints[n], true /*makeUnitVector*/));

		ocean_assert(imageRays.back().isUnit());
	}

	Scalars distancesBetweenCameraAndObjectPoints;
	if (!calculatePointDistances(objectPoints, imageRays.data(), correspondences, distancesBetweenCameraAndObjectPoints))
	{
		return false;
	}

	ocean_assert(distancesBetweenCameraAndObjectPoints.size() == correspondences);

	Vectors3 cameraObjectPoints; // 3D object point defined in camera
	cameraObjectPoints.reserve(correspondences);

	for (unsigned int n = 0; n < correspondences; n++)
	{
		cameraObjectPoints.emplace_back(imageRays[n] * distancesBetweenCameraAndObjectPoints[n]);
	}

	return AbsoluteTransformation::calculateTransformation(cameraObjectPoints.data(), objectPoints /*aka worldObjectPoints*/, correspondences, world_T_camera);
}

bool PnP::calculatePointDistances(const Vector3* objectPoints, const Vector3* imageRays, const size_t correspondences, Scalars &distances)
{
	ocean_assert(objectPoints != nullptr && imageRays != nullptr);
	ocean_assert(correspondences >= 5);

	const size_t rows = ((correspondences - 1) * (correspondences - 2)) / 2;

	Matrix matrix(rows, 5);

	size_t row = 0;
	for (size_t n = 1; n < correspondences; n++)
	{
		for (size_t m = n + 1; m < correspondences; m++)
		{
			const Vector3& object1 = objectPoints[0];
			const Vector3& object2 = objectPoints[n];
			const Vector3& object3 = objectPoints[m];

			const Vector3& imageRays1 = imageRays[0];
			const Vector3& imageRays2 = imageRays[n];
			const Vector3& imageRays3 = imageRays[m];

			const Scalar d12_2 = (object1 - object2).sqr();
			const Scalar d13_2 = (object1 - object3).sqr();
			const Scalar d23_2 = (object2 - object3).sqr();

			ocean_assert(imageRays1.isUnit() && imageRays2.isUnit() && imageRays3.isUnit());

			const Scalar cos12 = imageRays1 * imageRays2;
			const Scalar cos13 = imageRays1 * imageRays3;
			const Scalar cos23 = imageRays2 * imageRays3;

			ocean_assert(Numeric::isEqual(cos12, Numeric::cos(imageRays1.angle(imageRays2))));
			ocean_assert(Numeric::isEqual(cos13, Numeric::cos(imageRays1.angle(imageRays3))));
			ocean_assert(Numeric::isEqual(cos23, Numeric::cos(imageRays2.angle(imageRays3))));

			Scalar a0, a1, a2, a3, a4;

			calculateResultantOptimized(cos12, cos13, cos23, d12_2, d13_2, d23_2, a0, a1, a2, a3, a4);

			matrix(row, 0) = a0;
			matrix(row, 1) = a1;
			matrix(row, 2) = a2;
			matrix(row, 3) = a3;
			matrix(row, 4) = a4;

			row++;
		}
	}

	Matrix u, w, v;
	if (matrix.singularValueDecomposition(u, w, v) == false)
	{
		return false;
	}

	size_t lowestSingularValueIndex = size_t(-1);
	Scalar debugSmallestValue = Numeric::maxValue();

	ocean_assert(w.columns() == 1);
	for (size_t n = 0; n < w.rows(); n++)
	{
		if (Numeric::isEqualEps(w(n)))
		{
			lowestSingularValueIndex = n;
			debugSmallestValue = w(n);
			break;
		}
	}
	lowestSingularValueIndex = min(lowestSingularValueIndex, v.columns() - 1);
	OCEAN_SUPPRESS_UNUSED_WARNING(debugSmallestValue);

	// calculate the distance of the first point to the camera
	Scalar x1 = 0;
	unsigned int validValues = 0u;
	for (size_t n = 0; n < v.rows() - 1; n++)
	{
		const Scalar denominator = v(n, lowestSingularValueIndex);

		if (Numeric::isNotEqualEps(denominator))
		{
			x1 += v(n + 1, lowestSingularValueIndex) / denominator;
			validValues++;
		}
	}

	// can happen e.g., with 32 bit floating point accuracy
	if (validValues == 0u)
	{
		return false;
	}

	const Scalar x1_2 = x1 / Scalar(validValues);
	if (x1_2 < 0.0)
	{
		return false;
	}
	x1 = Numeric::sqrt(x1_2);

	distances.push_back(x1);

	// calculate the two possible distances of the second point to the camera
	const Scalar d12_2 = (objectPoints[0] - objectPoints[1]).sqr();
	const Scalar cos12 = imageRays[0] * imageRays[1];

	Scalar x2a, x2b;
	if (calculateCosineSolutions(x1, x1_2, cos12, d12_2, x2a, x2b) == false)
	{
		return false;
	}

	const Scalar x2a_2 = x2a * x2a;
	const Scalar x2b_2 = x2b * x2b;

	// calculate the two possible distances of the third point to the camera
	const Scalar d13_2 = (objectPoints[0] - objectPoints[2]).sqr();
	const Scalar cos13 = imageRays[0] * imageRays[2];

	Scalar x3a, x3b;
	if (calculateCosineSolutions(x1, x1_2, cos13, d13_2, x3a, x3b) == false)
	{
		return false;
	}

	const Scalar x3a_2 = x3a * x3a;
	const Scalar x3b_2 = x3b * x3b;

	// find the real solutions of the second and third distances
	const Scalar d23_2 = (objectPoints[1] - objectPoints[2]).sqr();
	const Scalar cos23 = imageRays[1] * imageRays[2];

	const Scalar resultaa = Numeric::abs(calculateCosineResults(x2a, x2a_2, x3a, x3a_2, cos23, d23_2));
	const Scalar resultab = Numeric::abs(calculateCosineResults(x2a, x2a_2, x3b, x3b_2, cos23, d23_2));
	const Scalar resultba = Numeric::abs(calculateCosineResults(x2b, x2b_2, x3a, x3a_2, cos23, d23_2));
	const Scalar resultbb = Numeric::abs(calculateCosineResults(x2b, x2b_2, x3b, x3b_2, cos23, d23_2));

	const Scalar result = min(resultaa, min(resultab, min(resultba, resultbb)));

	if (result == resultaa)
	{
		distances.push_back(x2a);
		distances.push_back(x3a);
	}
	else if (result == resultab)
	{
		distances.push_back(x2a);
		distances.push_back(x3b);
	}
	else if (result == resultba)
	{
		distances.push_back(x2b);
		distances.push_back(x3a);
	}
	else
	{
		distances.push_back(x2b);
		distances.push_back(x3b);
	}

	const Scalar x2 = distances[1];
	const Scalar x2_2 = x2 * x2;

	// find the real solutions of the remaining distances
	for (size_t n = 3; n < correspondences; n++)
	{
		// calculate the two possible distances of the nth point to the camera
		const Scalar d1n_2 = (objectPoints[0] - objectPoints[n]).sqr();
		const Scalar cos1n = imageRays[0] * imageRays[n];

		Scalar xna, xnb;
		if (calculateCosineSolutions(x1, x1_2, cos1n, d1n_2, xna, xnb) == false)
		{
			return false;
		}

		const Scalar xna_2 = xna * xna;
		const Scalar xnb_2 = xnb * xnb;

		// find the real solutions of the nth distance
		const Scalar d2n_2 = (objectPoints[1] - objectPoints[n]).sqr();
		const Scalar cos2n = imageRays[1] * imageRays[n];

		const Scalar resulta = Numeric::abs(calculateCosineResults(x2, x2_2, xna, xna_2, cos2n, d2n_2));
		const Scalar resultb = Numeric::abs(calculateCosineResults(x2, x2_2, xnb, xnb_2, cos2n, d2n_2));

		if (resulta < resultb)
		{
			distances.push_back(xna);
		}
		else
		{
			distances.push_back(xnb);
		}
	}

	return true;
}

bool PnP::calculateResultant(const Scalar cos12, const Scalar cos13, const Scalar cos23, const Scalar d12_2, const Scalar d13_2, const Scalar d23_2, Scalar& a0, Scalar& a1, Scalar& a2, Scalar& a3, Scalar& a4)
{
	/**
	 * f12 := x1^2 + x2^2 - 2 x1 x2 cos12 - d12^2
	 * f13 := x1^2 + x3^2 - 2 x1 x3 cos13 - d13^2
	 * f23 := x2^2 + x3^2 - 2 x2 x3 cos23 - d23^2
	 *
	 * h12(x1, x3) = Resultant[f13, f23, x3]
	 * g(x1) = Resultant[f12, h12, x2]
	 * g(x) = a4 x^8 + a3 x^6 + a2 x^4 + a1 x1^2 + a0 = 0
	 */

	const Scalar d12_4 = d12_2 * d12_2;
	const Scalar d13_4 = d13_2 * d13_2;
	const Scalar d23_4 = d23_2 * d23_2;

	const Scalar d12_6 = d12_4 * d12_2;
	const Scalar d13_6 = d13_4 * d13_2;
	const Scalar d23_6 = d23_4 * d23_2;

	const Scalar d12_8 = d12_4 * d12_4;
	const Scalar d13_8 = d13_4 * d13_4;
	const Scalar d23_8 = d23_4 * d23_4;

	const Scalar cos12_2 = cos12 * cos12;
	const Scalar cos13_2 = cos13 * cos13;
	const Scalar cos23_2 = cos23 * cos23;

	const Scalar cos12_3 = cos12_2 * cos12;
	const Scalar cos13_3 = cos13_2 * cos13;
	const Scalar cos23_3 = cos23_2 * cos23;

	const Scalar cos12_4 = cos12_2 * cos12_2;
	const Scalar cos13_4 = cos13_2 * cos13_2;
	const Scalar cos23_4 = cos23_2 * cos23_2;

	a0 = d12_8 + 4 * d12_6 * d13_2 - 8 * cos23_2 * d12_6 * d13_2 + 6 * d12_4 * d13_4
		- 16 * cos23_2 * d12_4 * d13_4 + 16 * cos23_4 * d12_4 * d13_4 + 4 * d12_2 * d13_6
		- 8 * cos23_2 * d12_2 * d13_6 + d13_8 - 4 * d12_6 * d23_2 - 12 * d12_4 * d13_2 * d23_2
		+ 16 * cos23_2 * d12_4 * d13_2 * d23_2 - 12 * d12_2 * d13_4 * d23_2
		+ 16 * cos23_2 * d12_2 * d13_4 * d23_2 - 4 * d13_6 * d23_2 + 6 * d12_4 * d23_4
		+ 12 * d12_2 * d13_2 * d23_4 - 8 * cos23_2 * d12_2 * d13_2 * d23_4 + 6 * d13_4 * d23_4
		- 4 * d12_2 * d23_6 - 4 * d13_2 * d23_6 + d23_8;

	a1 = -8 * d12_6 + 8 * cos13_2 * d12_6 + 8 * cos12 * cos13 * cos23 * d12_6 + 8 * cos23_2 * d12_6
		- 16 * cos13_2 * cos23_2 * d12_6 - 24 * d12_4 * d13_2
		+ 8 * cos12_2 * d12_4 * d13_2 + 16 * cos13_2 * d12_4 * d13_2
		- 40 * cos12 * cos13 * cos23 * d12_4 * d13_2 + 56 * cos23_2 * d12_4 * d13_2
		- 16 * cos12_2 * cos23_2 * d12_4 * d13_2
		+ 32 * cos12 * cos13 * cos23_3 * d12_4 * d13_2 - 32 * cos23_4 * d12_4 * d13_2
		- 24 * d12_2 * d13_4 + 16 * cos12_2 * d12_2 * d13_4
		+ 8 * cos13_2 * d12_2 * d13_4 - 40 * cos12 * cos13 * cos23 * d12_2 * d13_4
		+ 56 * cos23_2 * d12_2 * d13_4 - 16 * cos13_2 * cos23_2 * d12_2 * d13_4
		+ 32 * cos12 * cos13 * cos23_3 * d12_2 * d13_4 - 32 * cos23_4 * d12_2 * d13_4
		- 8 * d13_6 + 8 * cos12_2 * d13_6 + 8 * cos12 * cos13 * cos23 * d13_6
		+ 8 * cos23_2 * d13_6 - 16 * cos12_2 * cos23_2 * d13_6
		+ 24 * d12_4 * d23_2 - 8 * cos12_2 * d12_4 * d23_2
		- 24 * cos13_2 * d12_4 * d23_2 - 8 * cos12 * cos13 * cos23 * d12_4 * d23_2
		- 16 * cos23_2 * d12_4 * d23_2 + 32 * cos13_2 * cos23_2 * d12_4 * d23_2
		+ 48 * d12_2 * d13_2 * d23_2 - 32 * cos12_2 * d12_2 * d13_2 * d23_2
		- 32 * cos13_2 * d12_2 * d13_2 * d23_2
		+ 48 * cos12 * cos13 * cos23 * d12_2 * d13_2 * d23_2
		- 64 * cos23_2 * d12_2 * d13_2 * d23_2
		+ 32 * cos12 * cos13 * cos23_3 * d12_2 * d13_2 * d23_2 + 24 * d13_4 * d23_2
		- 24 * cos12_2 * d13_4 * d23_2 - 8 * cos13_2 * d13_4 * d23_2
		- 8 * cos12 * cos13 * cos23 * d13_4 * d23_2 - 16 * cos23_2 * d13_4 * d23_2
		+ 32 * cos12_2 * cos23_2 * d13_4 * d23_2 - 24 * d12_2 * d23_4
		+ 16 * cos12_2 * d12_2 * d23_4 + 24 * cos13_2 * d12_2 * d23_4
		- 8 * cos12 * cos13 * cos23 * d12_2 * d23_4 + 8 * cos23_2 * d12_2 * d23_4
		- 16 * cos13_2 * cos23_2 * d12_2 * d23_4 - 24 * d13_2 * d23_4
		+ 24 * cos12_2 * d13_2 * d23_4 + 16 * cos13_2 * d13_2 * d23_4
		- 8 * cos12 * cos13 * cos23 * d13_2 * d23_4 + 8 * cos23_2 * d13_2 * d23_4
		- 16 * cos12_2 * cos23_2 * d13_2 * d23_4 + 8 * d23_6
		- 8 * cos12_2 * d23_6 - 8 * cos13_2 * d23_6 + 8 * cos12 * cos13 * cos23 * d23_6;

	a2 = 24 * d12_4 - 8 * cos12_2 * d12_4 - 40 * cos13_2 * d12_4
		+ 16 * cos12_2 * cos13_2 * d12_4 + 16 * cos13_4 * d12_4
		+ 16 * cos12 * cos13 * cos23 * d12_4 - 32 * cos12 * cos13_3 * cos23 * d12_4
		- 40 * cos23_2 * d12_4 + 16 * cos12_2 * cos23_2 * d12_4
		+ 48 * cos13_2 * cos23_2 * d12_4 - 32 * cos12 * cos13 * cos23_3 * d12_4
		+ 16 * cos23_4 * d12_4 + 48 * d12_2 * d13_2
		- 48 * cos12_2 * d12_2 * d13_2 - 48 * cos13_2 * d12_2 * d13_2
		+ 160 * cos12 * cos13 * cos23 * d12_2 * d13_2
		- 32 * cos12_3 * cos13 * cos23 * d12_2 * d13_2
		- 32 * cos12 * cos13_3 * cos23 * d12_2 * d13_2 - 112 * cos23_2 * d12_2 * d13_2
		+ 32 * cos12_2 * cos23_2 * d12_2 * d13_2 + 32 * cos13_2 * cos23_2 * d12_2 * d13_2
		+ 64 * cos12_2 * cos13_2 * cos23_2 * d12_2 * d13_2
		- 128 * cos12 * cos13 * cos23_3 * d12_2 * d13_2 + 64 * cos23_4 * d12_2 * d13_2
		+ 24 * d13_4 - 40 * cos12_2 * d13_4 + 16 * cos12_4 * d13_4
		- 8 * cos13_2 * d13_4 + 16 * cos12_2 * cos13_2 * d13_4
		+ 16 * cos12 * cos13 * cos23 * d13_4 - 32 * cos12_3 * cos13 * cos23 * d13_4
		- 40 * cos23_2 * d13_4 + 48 * cos12_2 * cos23_2 * d13_4
		+ 16 * cos13_2 * cos23_2 * d13_4 - 32 * cos12 * cos13 * cos23_3 * d13_4
		+ 16 * cos23_4 * d13_4 - 48 * d12_2 * d23_2
		+ 48 * cos12_2 * d12_2 * d23_2 + 80 * cos13_2 * d12_2 * d23_2
		- 64 * cos12_2 * cos13_2 * d12_2 * d23_2 - 32 * cos13_4 * d12_2 * d23_2
		- 32 * cos12 * cos13 * cos23 * d12_2 * d23_2
		- 32 * cos12_3 * cos13 * cos23 * d12_2 * d23_2
		+ 64 * cos12 * cos13_3 * cos23 * d12_2 * d23_2 + 48 * cos23_2 * d12_2 * d23_2
		- 64 * cos13_2 * cos23_2 * d12_2 * d23_2
		+ 64 * cos12_2 * cos13_2 * cos23_2 * d12_2 * d23_2
		- 32 * cos12 * cos13 * cos23_3 * d12_2 * d23_2 - 48 * d13_2 * d23_2
		+ 80 * cos12_2 * d13_2 * d23_2 - 32 * cos12_4 * d13_2 * d23_2
		+ 48 * cos13_2 * d13_2 * d23_2 - 64 * cos12_2 * cos13_2 * d13_2 * d23_2
		- 32 * cos12 * cos13 * cos23 * d13_2 * d23_2
		+ 64 * cos12_3 * cos13 * cos23 * d13_2 * d23_2
		- 32 * cos12 * cos13_3 * cos23 * d13_2 * d23_2 + 48 * cos23_2 * d13_2 * d23_2
		- 64 * cos12_2 * cos23_2 * d13_2 * d23_2
		+ 64 * cos12_2 * cos13_2 * cos23_2 * d13_2 * d23_2
		- 32 * cos12 * cos13 * cos23_3 * d13_2 * d23_2 + 24 * d23_4
		- 40 * cos12_2 * d23_4 + 16 * cos12_4 * d23_4 - 40 * cos13_2 * d23_4
		+ 48 * cos12_2 * cos13_2 * d23_4 + 16 * cos13_4 * d23_4
		+ 16 * cos12 * cos13 * cos23 * d23_4 - 32 * cos12_3 * cos13 * cos23 * d23_4
		- 32 * cos12 * cos13_3 * cos23 * d23_4 - 8 * cos23_2 * d23_4
		+ 16 * cos12_2 * cos23_2 * d23_4 + 16 * cos13_2 * cos23_2 * d23_4;

	a3 = -32 * d12_2 + 32 * cos12_2 * d12_2 + 64 * cos13_2 * d12_2
		- 32 * cos12_2 * cos13_2 * d12_2 - 32 * cos13_4 * d12_2
		- 96 * cos12 * cos13 * cos23 * d12_2 + 32 * cos12_3 * cos13 * cos23 * d12_2
		+ 96 * cos12 * cos13_3 * cos23 * d12_2 + 64 * cos23_2 * d12_2
		- 32 * cos12_2 * cos23_2 * d12_2 - 64 * cos13_2 * cos23_2 * d12_2
		- 64 * cos12_2 * cos13_2 * cos23_2 * d12_2 + 96 * cos12 * cos13 * cos23_3 * d12_2
		- 32 * cos23_4 * d12_2 - 32 * d13_2 + 64 * cos12_2 * d13_2
		- 32 * cos12_4 * d13_2 + 32 * cos13_2 * d13_2
		- 32 * cos12_2 * cos13_2 * d13_2 - 96 * cos12 * cos13 * cos23 * d13_2
		+ 96 * cos12_3 * cos13 * cos23 * d13_2 + 32 * cos12 * cos13_3 * cos23 * d13_2
		+ 64 * cos23_2 * d13_2 - 64 * cos12_2 * cos23_2 * d13_2
		- 32 * cos13_2 * cos23_2 * d13_2 - 64 * cos12_2 * cos13_2 * cos23_2 * d13_2
		+ 96 * cos12 * cos13 * cos23_3 * d13_2 - 32 * cos23_4 * d13_2 + 32 * d23_2
		- 64 * cos12_2 * d23_2 + 32 * cos12_4 * d23_2 - 64 * cos13_2 * d23_2
		+ 128 * cos12_2 * cos13_2 * d23_2 - 64 * cos12_4 * cos13_2 * d23_2
		+ 32 * cos13_4 * d23_2 - 64 * cos12_2 * cos13_4 * d23_2
		+ 32 * cos12 * cos13 * cos23 * d23_2 - 32 * cos12_3 * cos13 * cos23 * d23_2
		- 32 * cos12 * cos13_3 * cos23 * d23_2 + 128 * cos12_3 * cos13_3 * cos23 * d23_2
		- 32 * cos23_2 * d23_2 + 32 * cos12_2 * cos23_2 * d23_2
		+ 32 * cos13_2 * cos23_2 * d23_2 - 128 * cos12_2 * cos13_2 * cos23_2 * d23_2
		+ 32 * cos12 * cos13 * cos23_3 * d23_2;

	a4 = 16 - 32 * cos12_2 + 16 * cos12_4 - 32 * cos13_2 + 32 * cos12_2 * cos13_2
		+ 16 * cos13_4 + 64 * cos12 * cos13 * cos23 - 64 * cos12_3 * cos13 * cos23
		- 64 * cos12 * cos13_3 * cos23 - 32 * cos23_2 + 32 * cos12_2 * cos23_2
		+ 32 * cos13_2 * cos23_2 + 64 * cos12_2 * cos13_2 * cos23_2
		- 64 * cos12 * cos13 * cos23_3 + 16 * cos23_4;

	return true;
}

bool PnP::calculateResultantOptimized(const Scalar cos12, const Scalar cos13, const Scalar cos23, const Scalar d12_2, const Scalar d13_2, const Scalar d23_2, Scalar& a0, Scalar& a1, Scalar& a2, Scalar& a3, Scalar& a4)
{
	/**
	 * f12 := x1^2 + x2^2 - 2 x1 x2 cos12 - d12^2
	 * f13 := x1^2 + x3^2 - 2 x1 x3 cos13 - d13^2
	 * f23 := x2^2 + x3^2 - 2 x2 x3 cos23 - d23^2
	 *
	 * h12(x1, x3) = Resultant[f13, f23, x3]
	 * g(x1) = Resultant[f12, h12, x2]
	 * g(x) = a4 x^8 + a3 x^6 + a2 x^4 + a1 x1^2 + a0 = 0
	 */

	const Scalar d12_4 = d12_2 * d12_2;
	const Scalar d13_4 = d13_2 * d13_2;
	const Scalar d23_4 = d23_2 * d23_2;

	const Scalar d12_6 = d12_4 * d12_2;

	const Scalar cos12_2 = cos12 * cos12;
	const Scalar cos13_2 = cos13 * cos13;
	const Scalar cos23_2 = cos23 * cos23;

	const Scalar cos12_4 = cos12_2 * cos12_2;
	const Scalar cos13_4 = cos13_2 * cos13_2;
	const Scalar cos23_4 = cos23_2 * cos23_2;

	const Scalar cos123 = cos12 * cos13 * cos23;

	const Scalar cos12_2_2 = 2 * cos12_2;
	const Scalar cos13_2_2 = 2 * cos13_2;
	const Scalar cos23_2_2 = 2 * cos23_2;

	// a0 = (d12^4 + (d13^2 - d23^2)^2 - 2 d12^2 ((-1 + 2 cos23^2) d13^2 + d23^2))^2
	const Scalar a0_0 = d13_2 - d23_2;
	const Scalar a0_1 = d12_4 + a0_0 * a0_0 - 2 * d12_2 * ((-1 + cos23_2_2) * d13_2 + d23_2);
	a0 = a0_1 * a0_1;

	// a1 = -8 ((1 - cos13^2 - cos12 cos13 cos23 - cos23^2 + 2 cos13^2 cos23^2) d12^6 + (d13^2 - d23^2)^2 ((1 - cos12 cos13 cos23 - cos23^2 + cos12^2 (-1 + 2 cos23^2)) d13^2 + (-1 + cos12^2 + cos13^2 - cos12 cos13 cos23) d23^2) + d12^4 ((3 - 2 cos13^2 - 7 cos23^2 + 4 cos23^4 + cos12 cos13 cos23 (5 - 4 cos23^2) + cos12^2 (-1 + 2 cos23^2)) d13^2 + (-3 + cos12^2 + cos12 cos13 cos23 + 2 cos23^2 + cos13^2 (3 - 4 cos23^2)) d23^2) + d12^2 (-(-3 + 2 cos12^2 + cos13^2 + cos23^2 (7 - 2 cos13^2 - 4 cos23^2) + cos12 cos13 cos23 (-5 + 4 cos23^2)) d13^4 + 2 (-3 + 2 cos12^2 + 2 cos13^2 + 4 cos23^2 - cos12 cos13 cos23 (3 + 2 cos23^2)) d13^2 d23^2 + (3 - 2 cos12^2 + cos12 cos13 cos23 - cos23^2 + cos13^2 (-3 + 2 cos23^2)) d23^4))
	const Scalar a1_0 = d13_2 - d23_2;
	a1 = -8 * ((1 - cos13_2 - cos123 - cos23_2 + cos13_2_2 * cos23_2) * d12_6
		+ a1_0 * a1_0 * ((1 - cos123 - cos23_2 + cos12_2 * (-1 + cos23_2_2)) * d13_2
		+ (-1 + cos12_2 + cos13_2 - cos123) * d23_2)
		+ d12_4 * ((3 - cos13_2_2 - 7 * cos23_2 + 4 * cos23_4 + cos123 * (5 - 4 * cos23_2)
		+ cos12_2 * (-1 + cos23_2_2)) * d13_2 + (-3 + cos12_2 + cos123
		+ cos23_2_2 + cos13_2 * (3 - 4 * cos23_2)) * d23_2)
		+ d12_2 * (-(-3 + cos12_2_2 + cos13_2 + cos23_2 * (7 - cos13_2_2 - 4 * cos23_2) + cos123 * (-5 + 4 * cos23_2)) * d13_4
		+ 2 * (-3 + cos12_2_2 + cos13_2_2 + 4 * cos23_2 - cos123 * (3 + cos23_2_2)) * d13_2 * d23_2
		+ (3 - cos12_2_2 + cos123 - cos23_2 + cos13_2 * (-3 + cos23_2_2)) * d23_4));

	// a2 = 8 ((3 + 2 cos13^4 - 5 cos23^2 + 2 cos23^4 + cos12^2 (-1 + 2 cos13^2 + 2 cos23^2) - 2 cos12 cos13 cos23 (-1 + 2 cos13^2 + 2 cos23^2) + cos13^2 (-5 + 6 cos23^2)) d12^4 + (3 + 2 cos12^4 - 4 cos12^3 cos13 cos23 - 5 cos23^2 + 2 cos23^4 + 2 cos12 cos13 cos23 (1 - 2 cos23^2) + cos13^2 (-1 + 2 cos23^2) + cos12^2 (-5 + 2 cos13^2 + 6 cos23^2)) d13^4 - 2 (2 cos12^4 - 4 cos12^3 cos13 cos23 - 3 (-1 + cos13^2 + cos23^2) + 2 cos12 cos13 cos23 (1 + cos13^2 + cos23^2) + cos12^2 (-5 + 4 cos23^2 - 4 cos13^2 (-1 + cos23^2))) d13^2 d23^2 + (3 + 2 cos12^4 + 2 cos13^4 - 4 cos12^3 cos13 cos23 + 2 cos12 cos13 (1 - 2 cos13^2) cos23 - cos23^2 + cos13^2 (-5 + 2 cos23^2) + cos12^2 (-5 + 6 cos13^2 + 2 cos23^2)) d23^4 - 2 d12^2 ((3 (-1 + cos12^2 + cos13^2) + 2 cos12 cos13 (-5 + cos12^2 + cos13^2) cos23 - (-7 + 2 cos13^2 + cos12^2 (2 + 4 cos13^2)) cos23^2 + 8 cos12 cos13 cos23^3 - 4 cos23^4) d13^2 + (3 - 5 cos13^2 + 2 cos13^4 + 2 cos12^3 cos13 cos23 + (-3 + 4 cos13^2) cos23^2 + 2 cos12 cos13 cos23 (1 - 2 cos13^2 + cos23^2) + cos12^2 (-3 - 4 cos13^2 (-1 + cos23^2))) d23^2))
	a2 = 8 * ((3 + 2 * cos13_4 - 5 * cos23_2 + 2 * cos23_4 + cos12_2 * (-1 + cos13_2_2 + cos23_2_2)
		- 2 * cos123 * (-1 + cos13_2_2 + cos23_2_2) + cos13_2 * (-5 + 6 * cos23_2)) * d12_4
		+ (3 + 2 * cos12_4 - 4 * cos12_2 * cos123 - 5 * cos23_2 + 2 * cos23_4 + 2 * cos123 * (1 - cos23_2_2)
		+ cos13_2 * (-1 + cos23_2_2) + cos12_2 * (-5 + cos13_2_2 + 6 * cos23_2)) * d13_4
		- 2 * (2 * cos12_4 - 4 * cos12_2 * cos123 - 3 * (-1 + cos13_2 + cos23_2) + 2 * cos123 * (1 + cos13_2 + cos23_2)
		+ cos12_2 * (-5 + 4 * cos23_2 - 4 * cos13_2 * (-1 + cos23_2))) * d13_2 * d23_2
		+ (3 + 2 * cos12_4 + 2 * cos13_4 - 4 * cos12_2 * cos123 + 2 * cos12 * cos13 * (1 - cos13_2_2) * cos23
		- cos23_2 + cos13_2 * (-5 + cos23_2_2) + cos12_2 * (-5 + 6 * cos13_2 + cos23_2_2)) * d23_4
		- 2 * d12_2 * ((3 * (-1 + cos12_2 + cos13_2) + 2 * cos12 * cos13 * (-5 + cos12_2 + cos13_2) * cos23
		- (-7 + cos13_2_2 + cos12_2 * (2 + 4 * cos13_2)) * cos23_2 + 8 * cos123 * cos23_2 - 4 * cos23_4) * d13_2
		+ (3 - 5 * cos13_2 + 2 * cos13_4 + cos12_2_2 * cos123 + (-3 + 4 * cos13_2) * cos23_2
		+ 2 * cos123 * (1 - cos13_2_2 + cos23_2) + cos12_2 * (-3 - 4 * cos13_2 * (-1 + cos23_2))) * d23_2));

	// a3 = 32 (-1 + cos12^2 + cos13^2 - 2 cos12 cos13 cos23 + cos23^2) (-(-1 + cos13^2 - cos12 cos13 cos23 + cos23^2) d12^2 - (-1 + cos12^2 - cos12 cos13 cos23 + cos23^2) d13^2 + (-1 + cos13^2 + cos12 (cos12 - 2 cos12 cos13^2 + cos13 cos23)) d23^2)
	a3 = 32 * (-1 + cos12_2 + cos13_2 - 2 * cos123 + cos23_2) * (-(-1 + cos13_2 - cos123 + cos23_2) * d12_2
		- (-1 + cos12_2 - cos123 + cos23_2) * d13_2 + (-1 + cos13_2 + cos12 * (cos12 - 2 * cos12 * cos13_2 + cos13 * cos23)) * d23_2);

	// a4 = 16 (-1 + cos12^2 + cos13^2 - 2 cos12 cos13 cos23 + cos23^2)^2
	const Scalar x4_0 = -1 + cos12_2 + cos13_2 - 2 * cos123 + cos23_2;
	a4 = 16 * x4_0 * x4_0;

	return true;
}

}

}
