/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/P4P.h"
#include "ocean/geometry/AbsoluteTransformation.h"

#include "ocean/math/Matrix.h"

namespace Ocean
{

namespace Geometry
{

bool P4P::pose(const AnyCamera& camera, const Vector3* objectPoints, const Vector2* imagePoints, HomogenousMatrix4& world_T_camera)
{
	ocean_assert(objectPoints != nullptr && imagePoints != nullptr);

	Vector3 imageRays[4];

	for (size_t n = 0; n < 4; ++n)
	{
		imageRays[n] = camera.vector(imagePoints[n], true /*makeUnitVector*/);

		ocean_assert(imageRays[n].isUnit());
	}

	Scalar distancesBetweenCameraAndObjectPoints[4];

	if (!calculatePointDistances(objectPoints, imageRays, distancesBetweenCameraAndObjectPoints))
	{
		return false;
	}

	Vector3 cameraObjectPoints[4];

	for (unsigned int n = 0; n < 4; n++)
	{
		cameraObjectPoints[n] = imageRays[n] * distancesBetweenCameraAndObjectPoints[n];
	}

	return AbsoluteTransformation::calculateTransformation(cameraObjectPoints, objectPoints /*aka worldObjectPoints*/, 4, world_T_camera);
}

bool P4P::calculatePointDistances(const Vector3* objectPoints, const Vector3* imageRays, Scalar distances[4])
{
	ocean_assert(objectPoints != nullptr && imageRays != nullptr);

	// beware: The paper has some mistakes in the definition of the 24x24 polynomial matrix

	const Scalar c12 = -Scalar(2) * (imageRays[0] * imageRays[1]);
	const Scalar c13 = -Scalar(2) * (imageRays[0] * imageRays[2]);
	const Scalar c14 = -Scalar(2) * (imageRays[0] * imageRays[3]);
	const Scalar c23 = -Scalar(2) * (imageRays[1] * imageRays[2]);
	const Scalar c24 = -Scalar(2) * (imageRays[1] * imageRays[3]);
	const Scalar c34 = -Scalar(2) * (imageRays[2] * imageRays[3]);

	ocean_assert(Numeric::isEqual(c12, -Scalar(2) * Numeric::cos(imageRays[0].angle(imageRays[1]))));
	ocean_assert(Numeric::isEqual(c13, -Scalar(2) * Numeric::cos(imageRays[0].angle(imageRays[2]))));
	ocean_assert(Numeric::isEqual(c14, -Scalar(2) * Numeric::cos(imageRays[0].angle(imageRays[3]))));
	ocean_assert(Numeric::isEqual(c23, -Scalar(2) * Numeric::cos(imageRays[1].angle(imageRays[2]))));
	ocean_assert(Numeric::isEqual(c24, -Scalar(2) * Numeric::cos(imageRays[1].angle(imageRays[3]))));
	ocean_assert(Numeric::isEqual(c34, -Scalar(2) * Numeric::cos(imageRays[2].angle(imageRays[3]))));

	const Scalar d12 = (objectPoints[0] - objectPoints[1]).sqr();
	const Scalar d13 = (objectPoints[0] - objectPoints[2]).sqr();
	const Scalar d14 = (objectPoints[0] - objectPoints[3]).sqr();
	const Scalar d23 = (objectPoints[1] - objectPoints[2]).sqr();
	const Scalar d24 = (objectPoints[1] - objectPoints[3]).sqr();
	const Scalar d34 = (objectPoints[2] - objectPoints[3]).sqr();

	/**
	 * The following code is in general not good as we should obviously should use a sparse solution
	 * Further, the following code may be responsible for a very slow compilation time (> 60min for some compilers)
	 * As initial workaround we replace the definition of the array by an explicit assignment of the individual non-zero values
	 */

#ifdef OCEAN_INTENSIVE_DEBUG

	// row aligned matrix values
	const Scalar debugMatrixValues[576] = {
			1, 0, 0, 0,     c12, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0,     d12, 0, 0, 0,
			0, 1, 0, 0,     1, 0, 0, c12, 0, 0, 0, 0, 0, 0, 0, 0,     0, 0, 0, 0,     0, d12, 0, 0,
			0, 0, 0, 0,     0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,       c12, 0, 0, 0,   0, 0, d12, 0,
			0, 0, 0, 0,     0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0,       0, c12, 0, 0,   0, 0, 0, d12,

			1, 0, 0, 0,     0, c13, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,     0, 0, 0, 0,     d13, 0, 0, 0,
			0, 0, 0, 0,     1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,       c13, 0, 0, 0,   0, d13, 0, 0,
			0, 0, 1, 0,     0, 1, 0, 0, 0, 0, 0, c13, 0 ,0, 0, 0,     0, 0, 0, 0,     0, 0, d13, 0,
			0, 0, 0, 0,     0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,       0, 0, c13, 0,   0, 0, 0, d13,

			1, 0, 0, 0,     0, 0, c14, 0, 0, 0, 0, 0, 0, 0, 0, 1,     0, 0, 0, 0,     d14, 0, 0, 0,
			0, 0, 0, 0,     1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,       0, c14, 0, 0,   0, d14, 0, 0,
			0, 0, 0, 0,     0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,       0, 0, c14, 0,   0, 0, d14, 0,
			0, 0, 0, 1,     0, 0, 1, 0, 0, 0, 0, 0, 0 ,0 ,0, c14,     0, 0, 0, 0,     0, 0, 0, d14,

			0, 0, 0, 0,     0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,       c23, 0, 0, 0,   d23, 0, 0, 0,
			0, 1, 0, 0,     0, 0, 0, 0, c23, 0, 1, 0, 0, 0, 0, 0,     0, 0, 0, 0,     0, d23, 0, 0,
			0, 0, 1, 0,     0, 0, 0, 0, 1, 0, c23, 0, 0, 0, 0, 0,     0, 0, 0, 0,     0, 0, d23, 0,
			0, 0, 0, 0,     0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0,       0, 0, 0, c23,   0, 0, 0, d23,

			0, 0, 0, 0,     0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,       0, c24, 0, 0,   d24, 0, 0, 0,
			0, 1, 0, 0,     0, 0, 0, 0, 0, c24, 0, 0, 0, 1, 0, 0,     0, 0, 0, 0,     0, d24, 0, 0,
			0, 0, 0, 0,     0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,       0, 0, 0, c24,   0, 0, d24, 0,
			0, 0, 0, 1,     0, 0, 0, 0, 0, 1, 0, 0, 0, c24, 0, 0,     0, 0, 0, 0,     0, 0, 0, d24,

			0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,       0, 0, c34, 0,   d34, 0, 0, 0,
			0, 0, 0, 0,     0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0,       0, 0, 0, c34,   0, d34, 0, 0,
			0, 0, 1, 0,     0, 0, 0, 0, 0, 0, 0, 0, c34, 0, 1, 0,     0, 0, 0, 0,     0, 0, d34, 0,
			0, 0, 0, 1,     0, 0, 0, 0, 0, 0, 0, 0, 1, 0, c34, 0,     0, 0, 0, 0,     0, 0, 0, d34};

	const Matrix debugMatrix(24, 24, debugMatrixValues);

#endif // OCEAN_INTENSIVE_DEBUG

	Matrix matrix(24, 24, false);

	// first column block
	matrix( 0, 0) = 1;
	matrix( 1, 1) = 1;
	matrix( 4, 0) = 1;
	matrix( 6, 2) = 1;
	matrix( 8, 0) = 1;
	matrix(11, 3) = 1;
	matrix(13, 1) = 1;
	matrix(14, 2) = 1;
	matrix(17, 1) = 1;
	matrix(19, 3) = 1;
	matrix(22, 2) = 1;
	matrix(23, 3) = 1;

	// second column block
	matrix(0, 4) = c12;
	matrix(0, 7) = 1;
	matrix(1, 4) = 1;
	matrix(1, 7) = c12;
	matrix(2, 5) = 1;
	matrix(2, 8) = 1;
	matrix(3, 6) = 1;
	matrix(3, 9) = 1;

	matrix(4,  5) = c13;
	matrix(4, 11) = 1;
	matrix(5,  4) = 1;
	matrix(5, 10) = 1;
	matrix(6,  5) = 1;
	matrix(6, 11) = c13;
	matrix(7,  6) = 1;
	matrix(7, 12) = 1;

	matrix( 8,  6) = c14;
	matrix( 8, 15) = 1;
	matrix( 9,  4) = 1;
	matrix( 9, 13) = 1;
	matrix(10,  5) = 1;
	matrix(10, 14) = 1;
	matrix(11,  6) = 1;
	matrix(11, 15) = c14;

	matrix(12,  7) = 1;
	matrix(12, 11) = 1;
	matrix(13,  8) = c23;
	matrix(13, 10) = 1;
	matrix(14,  8) = 1;
	matrix(14, 10) = c23;
	matrix(15,  9) = 1;
	matrix(15, 12) = 1;

	matrix(16,  7) = 1;
	matrix(16, 15) = 1;
	matrix(17,  9) = c24;
	matrix(17, 13) = 1;
	matrix(18,  8) = 1;
	matrix(18, 14) = 1;
	matrix(19,  9) = 1;
	matrix(19, 13) = c24;

	matrix(20, 11) = 1;
	matrix(20, 15) = 1;
	matrix(21, 10) = 1;
	matrix(21, 13) = 1;
	matrix(22, 12) = c34;
	matrix(22, 14) = 1;
	matrix(23, 12) = 1;
	matrix(23, 14) = c34;

	//third column block
	matrix(2, 16) = c12;
	matrix(3, 17) = c12;
	matrix(5, 16) = c13;
	matrix(7, 18) = c13;
	matrix(9, 17) = c14;
	matrix(10, 18) = c14;
	matrix(12, 16) = c23;
	matrix(15, 19) = c23;
	matrix(16, 17) = c24;
	matrix(18, 19) = c24;
	matrix(20, 18) = c34;
	matrix(21, 19) = c34;

	// last column block
	matrix( 0, 20) = d12;
	matrix( 1, 21) = d12;
	matrix( 2, 22) = d12;
	matrix( 3, 23) = d12;

	matrix( 4, 20) = d13;
	matrix( 5, 21) = d13;
	matrix( 6, 22) = d13;
	matrix( 7, 23) = d13;

	matrix( 8, 20) = d14;
	matrix( 9, 21) = d14;
	matrix(10, 22) = d14;
	matrix(11, 23) = d14;

	matrix(12, 20) = d23;
	matrix(13, 21) = d23;
	matrix(14, 22) = d23;
	matrix(15, 23) = d23;

	matrix(16, 20) = d24;
	matrix(17, 21) = d24;
	matrix(18, 22) = d24;
	matrix(19, 23) = d24;

	matrix(20, 20) = d34;
	matrix(21, 21) = d34;
	matrix(22, 22) = d34;
	matrix(23, 23) = d34;

#ifdef OCEAN_INTENSIVE_DEBUG
	ocean_assert(matrix == debugMatrix);
#endif // OCEAN_INTENSIVE_DEBUG

	Matrix u, w, v;
	if (!matrix.singularValueDecomposition(u, w, v))
	{
		return false;
	}

#ifdef OCEAN_DEBUG
	for (size_t n = 0; n < 22; n++)
	{
		ocean_assert(w(n, 0) >= w(n + 1, 0));
	}
#endif

	const Scalar v20 = v(20, 23);
	const Scalar v21 = v(21, 23);
	const Scalar v22 = v(22, 23);
	const Scalar v23 = v(23, 23);

	if (Numeric::isEqualEps(v20) || Numeric::isEqualEps(v21) || Numeric::isEqualEps(v22) || Numeric::isEqualEps(v23))
	{
		return false;
	}

	distances[0] = Numeric::sqrt(Numeric::abs(v(0, 23) / v20));
	distances[1] = Numeric::sqrt(Numeric::abs(v(1, 23) / v21));
	distances[2] = Numeric::sqrt(Numeric::abs(v(2, 23) / v22));
	distances[3] = Numeric::sqrt(Numeric::abs(v(3, 23) / v23));

	return true;
}

}

}
