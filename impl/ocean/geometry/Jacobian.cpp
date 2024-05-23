/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/Jacobian.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/StaticMatrix.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Geometry
{

template <typename T>
void Jacobian::calculateRotationRodriguesDerivative(const ExponentialMapT<T>& rotation, SquareMatrixT3<T>& dwx, SquareMatrixT3<T>& dwy, SquareMatrixT3<T>& dwz)
{
	/*
	 * Rodriguez formula:
	 *
	 * Exponential map w := (wx, wy, wz)
	 * Angle axis rotation: r := (nx, ny, nz, a)
	 *                         = (nx, ny, nz, ||w||)
	 *                         = (nx, ny, nz, ||w||)
	 *                         = (wx / ||w||, wy / ||w||, wz / ||w||, ||w||)
	 *
	 * Angle a := ||w|| = sqrt(wx^2 + wy^2 + wz^2)
	 *
	 * R := cos(a) * I + (1 - cos(a)) * nnT + sin(a) * [n]x
	 *    = cos(a) * I + ((1 - cos(a)) / a^2) * wwT + (sin(a) / a) * [w]x
	 * with [n]x the skew symmetric matrix of n.
	 *
	 *              | 1 0 0 |                          | wx^2    wx*wy   wx*wz |                  |  0  -wz   wy |
	 * R = cos(a) * | 0 1 0 | + ((1 - cos(a)) / a^2) * | wx*wy   wy^2    wy*wz | + (sin(a) / a) * |  wz   0  -wx |
	 *              | 0 0 1 |                          | wx*wz   wy*wz   wz^2  |                  | -wy  wx   0  |
	 *
	 */

	// from "A compact formula for the derivative of a 3-D rotation in exponential coordinates":

	// (v_i * [v]x + [v cross (I - R)e_i]x)/||v||^2 * R

	const VectorT3<T> v(rotation.data());
	const T vSqr = v.sqr();

	if (NumericT<T>::isEqualEps(vSqr))
	{
		/*
		 * |  0  -wz   wy |
		 * |  wz   0  -wx |
		 * | -wy  wx   0  |
		 */

		dwx = SquareMatrixT3<T>(0, 0, 0, 0, 0, 1, 0, -1, 0);
		dwy = SquareMatrixT3<T>(0, 0, -1, 0, 0, 0, 1, 0, 0);
		dwz = SquareMatrixT3<T>(0, 1, 0, -1, 0, 0, 0, 0, 0);
	}
	else
	{
		const SquareMatrixT3<T> R(rotation.quaternion());

		// v cross (I - R) * e_i

		const VectorT3<T> xCross(VectorT3<T>(rotation.data()).cross(VectorT3<T>(T(1) - R[0], -R[1], -R[2])));
		const VectorT3<T> yCross(VectorT3<T>(rotation.data()).cross(VectorT3<T>(-R[3], T(1) - R[4], -R[5])));
		const VectorT3<T> zCross(VectorT3<T>(rotation.data()).cross(VectorT3<T>(-R[6], -R[7], T(1) - R[8])));

		const T invSqr = T(1) / vSqr;

		const T& vx = v.x();
		dwx = SquareMatrixT3<T>(0, (v.z() * vx + xCross.z()) * invSqr, (-v.y() * vx - xCross.y()) * invSqr, (-v.z() * vx - xCross.z()) * invSqr, 0, (v.x() * vx + xCross.x()) * invSqr, (v.y() * vx + xCross.y()) * invSqr, (-v.x() * vx - xCross.x()) * invSqr, 0) * R;
		ocean_assert(dwx == (SquareMatrixT3<T>::skewSymmetricMatrix(v) * v.x() + SquareMatrixT3<T>::skewSymmetricMatrix(v.cross((SquareMatrixT3<T>(true) - R) * VectorT3<T>(1, 0, 0)))) * invSqr * R);

		const T& vy = v.y();
		dwy = SquareMatrixT3<T>(0, (v.z() * vy + yCross.z()) * invSqr, (-v.y() * vy - yCross.y()) * invSqr, (-v.z() * vy - yCross.z()) * invSqr, 0, (v.x() * vy + yCross.x()) * invSqr, (v.y() * vy + yCross.y()) * invSqr, (-v.x() * vy - yCross.x()) * invSqr, 0) * R;
		ocean_assert(dwy == (SquareMatrixT3<T>::skewSymmetricMatrix(v) * v.y() + SquareMatrixT3<T>::skewSymmetricMatrix(v.cross((SquareMatrixT3<T>(true) - R) * VectorT3<T>(0, 1, 0)))) * invSqr * R);

		const T& vz = v.z();
		dwz = SquareMatrixT3<T>(0, (v.z() * vz + zCross.z()) * invSqr, (-v.y() * vz - zCross.y()) * invSqr, (-v.z() * vz - zCross.z()) * invSqr, 0, (v.x() * vz + zCross.x()) * invSqr, (v.y() * vz + zCross.y()) * invSqr, (-v.x() * vz - zCross.x()) * invSqr, 0) * R;
		ocean_assert(dwz == (SquareMatrixT3<T>::skewSymmetricMatrix(v) * v.z() + SquareMatrixT3<T>::skewSymmetricMatrix(v.cross((SquareMatrixT3<T>(true) - R) * VectorT3<T>(0, 0, 1)))) * invSqr * R);
	}

#ifdef ALTERNATIVE_IMPLEMENTATION

	/*
	 *
	 *
	 *     | (wx^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+Cos[Sqrt[wx^2+wy^2+wz^2]]                                (wx wy (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)-(wz Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]      (wx wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wy Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2] |
	 * R = | (wx wy (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wz Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]     (wy^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+Cos[Sqrt[wx^2+wy^2+wz^2]]                                 (wy wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)-(wx Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2] |
	 *     | (wx wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)-(wy Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]     (wy wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wx Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]      (wz^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+Cos[Sqrt[wx^2+wy^2+wz^2]]                            |
	 *
	 */

	/**
	 *          | -((2 wx^3 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(2 wx (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wx^3 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wx Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]                                                                -((2 wx^2 wy (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wy (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)-(wx wz Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx^2 wy Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)     -((2 wx^2 wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wx wy Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)-(wx wy Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx^2 wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2) |
	 * dR/dwx = | -((2 wx^2 wy (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wy (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wx wz Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx^2 wy Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wx wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)    -((2 wx wy^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wx wy^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wx Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]                                                                                                                 -((2 wx wy wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)-(wx^2 Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-Sin[Sqrt[wx^2+wy^2+wz^2]]/Sqrt[wx^2+wy^2+wz^2]      |
	 *          | -((2 wx^2 wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)-(wx wy Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx wy Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx^2 wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)    -((2 wx wy wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wx^2 Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)-(wx^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+Sin[Sqrt[wx^2+wy^2+wz^2]]/Sqrt[wx^2+wy^2+wz^2]          -((2 wx wz^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wx wz^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wx Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]                                                                                                             |
	 */

	/**
	 *          | -((2 wx^2 wy (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wx^2 wy Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wy Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]                                                                                                                -((2 wx wy^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wx (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)-(wy wz Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx wy^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)        -((2 wx wy wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wy^2 Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)-(wy^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+Sin[Sqrt[wx^2+wy^2+wz^2]]/Sqrt[wx^2+wy^2+wz^2]      |
	 * dR/dwy = | -((2 wx wy^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wx (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wy wz Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx wy^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)    -((2 wy^3 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(2 wy (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wy^3 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wy Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]                                                                    -((2 wy^2 wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)-(wx wy Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx wy Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wy^2 wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2) |
	 *          | -((2 wx wy wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)-(wy^2 Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wy^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-Sin[Sqrt[wx^2+wy^2+wz^2]]/Sqrt[wx^2+wy^2+wz^2]         -((2 wy^2 wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wx wy Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)-(wx wy Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wy^2 wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)        -((2 wy wz^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wy wz^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wy Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]                                                                                                             |
	 */

	/**
	 *
	 *          | -((2 wx^2 wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wx^2 wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wz Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]                                                                                                                 -((2 wx wy wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)-(wz^2 Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wz^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-Sin[Sqrt[wx^2+wy^2+wz^2]]/Sqrt[wx^2+wy^2+wz^2]              -((2 wx wz^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wx (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wy wz Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)-(wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx wz^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2) |
	 * dR/dwz = | -((2 wx wy wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wz^2 Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wz^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+Sin[Sqrt[wx^2+wy^2+wz^2]]/Sqrt[wx^2+wy^2+wz^2]          -((2 wy^2 wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wy^2 wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wz Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]                                                                                                                     -((2 wy wz^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wy (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)-(wx wz Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wy wz^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2) |
	 *          | -((2 wx wz^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wx (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)-(wy wz Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx wz^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)     -((2 wy wz^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wy (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wx wz Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)-(wx wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wy wz^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)         -((2 wz^3 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(2 wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wz^3 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wz Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]                                                             |
	 */

	const T angle = rotation.angle();
	const VectorT3<T>& axis = rotation.axis();

	if (NumericT<T>::isEqualEps(angle))
	{
		/*
		 * |  0  -wz   wy |
		 * |  wz   0  -wx |
		 * | -wy  wx   0  |
		 */

		dwx = SquareMatrixT3<T>(0, 0, 0, 0, 0, 1, 0, -1, 0);
		dwy = SquareMatrixT3<T>(0, 0, -1, 0, 0, 0, 1, 0, 0);
		dwz = SquareMatrixT3<T>(0, 1, 0, -1, 0, 0, 0, 0, 0);
	}
	else
	{
		ocean_assert(NumericT<T>::isNotEqualEps(angle));

		const T iAngle = T(1) / angle;
		ocean_assert((std::is_same<T, float>::value) || NumericT<T>::isEqual(angle, NumericT<T>::sqrt(axis.sqr())));
		const T iAngle2 = T(1) / axis.sqr();
		const T iAngle3 = iAngle2 * iAngle;
		const T iAngle4 = iAngle2 * iAngle2;

		const T wx = axis.x();
		const T wy = axis.y();
		const T wz = axis.z();

		const T s = NumericT<T>::sin(angle);
		const T c = NumericT<T>::cos(angle);

		const T v = -s * iAngle;
		const T n = s * iAngle3 - T(2) * (T(1) - c) * iAngle4;
		const T n1 = T(2) * (1 - c) * iAngle2;
		const T nx = wx * wx * s * iAngle3 - (wx * wx - wy * wy - wz * wz) * (T(1) - c) * iAngle4;
		const T ny = wy * wy * s * iAngle3 - (wy * wy - wx * wx - wz * wz) * (T(1) - c) * iAngle4;
		const T nz = wz * wz * s * iAngle3 - (wz * wz - wx * wx - wy * wy) * (T(1) - c) * iAngle4;

		const T t = c * iAngle2 - s * iAngle3;
		const T tx = wx * wx * c * iAngle2 + (wy * wy + wz * wz) * s * iAngle3;
		const T ty = wy * wy * c * iAngle2 + (wx * wx + wz * wz) * s * iAngle3;
		const T tz = wz * wz * c * iAngle2 + (wx * wx + wy * wy) * s * iAngle3;

		dwx[0] = wx * v + wx * wx * wx * n + wx * n1;
		dwx[1] = wy * nx + wz * wx * t;
		dwx[2] = wz * nx - wy * wx * t;
		dwx[3] = wy * nx - wz * wx * t;
		dwx[4] = wx * v + wx * wy * wy * n;
		dwx[5] = wx * wy * wz * n + tx;
		dwx[6] = wz * nx + wy * wx * t;
		dwx[7] = wx * wy * wz * n - tx;
		dwx[8] = wx * v + wx * wz * wz * n;

		dwy[0] = wy * v + wx * wx * wy * n;
		dwy[1] = wx * ny + wz * wy * t;
		dwy[2] = wx * wy * wz * n - ty;
		dwy[3] = wx * ny - wz * wy * t;
		dwy[4] = wy * v + wy * wy * wy * n + wy * n1;
		dwy[5] = wz * ny + wx * wy * t;
		dwy[6] = wx * wy * wz * n + ty;
		dwy[7] = wz * ny - wx * wy * t;
		dwy[8] = wy * v + wy * wz * wz * n;

		dwz[0] = wz * v + wx * wx * wz * n;
		dwz[1] = wx * wy * wz * n + tz;
		dwz[2] = wx * nz - wy * wz * t;
		dwz[3] = wx * wy * wz * n - tz;
		dwz[4] = wz * v + wy * wy * wz * n;
		dwz[5] = wy * nz + wx * wz * t;
		dwz[6] = wx * nz + wy * wz * t;
		dwz[7] = wy * nz - wx * wz * t;
		dwz[8] = wz * v + wz * wz * wz * n + wz * n1;
	}

#endif // ALTERNATIVE_IMPLEMENTATION
}

template void OCEAN_GEOMETRY_EXPORT Jacobian::calculateRotationRodriguesDerivative(const ExponentialMapT<float>& rotation, SquareMatrixT3<float>& dwx, SquareMatrixT3<float>& dwy, SquareMatrixT3<float>& dwz);
template void OCEAN_GEOMETRY_EXPORT Jacobian::calculateRotationRodriguesDerivative(const ExponentialMapT<double>& rotation, SquareMatrixT3<double>& dwx, SquareMatrixT3<double>& dwy, SquareMatrixT3<double>& dwz);

template <typename T>
void Jacobian::calculateSphericalObjectPointJacobian3x3(T* jx, T* jy, T* jz, const ExponentialMapT<T>& sphericalObjectPoint, const T objectPointDistance)
{
	ocean_assert(jx != nullptr && jy != nullptr && jz != nullptr);
	ocean_assert(objectPointDistance > NumericT<T>::eps());

	/**
	 * Rodriguez formula:
	 *
	 * Exponential map w := (wx, wy, wz)
	 * Angle axis rotation: r := (nx, ny, nz, a)
	 *                         = (nx, ny, nz, ||w||)
	 *                         = (nx, ny, nz, ||w||)
	 *                         = (wx / ||w||, wy / ||w||, wz / ||w||, ||w||)
	 *
	 * Angle a := ||w|| = sqrt(wx^2 + wy^2 + wz^2)
	 *
	 * R := cos(a) * I + (1 - cos(a)) * nnT + sin(a) * [n]x
	 *    = cos(a) * I + ((1 - cos(a)) / a^2) * wwT + (sin(a) / a) * [w]x
	 * with [n]x the skew symmetric matrix of n.
	 *
	 *              | 1 0 0 |                          | wx^2    wx*wy   wx*wz |                  |  0  -wz   wy |
	 * R = cos(a) * | 0 1 0 | + ((1 - cos(a)) / a^2) * | wx*wy   wy^2    wy*wz | + (sin(a) / a) * |  wz   0  -wx |
	 *              | 0 0 1 |                          | wx*wz   wy*wz   wz^2  |                  | -wy  wx   0  |
	 *
	 * With [ox, oy, oz] = R * [0, 0, -radius]
	 *                   = -radius * Rz
	 *
	 *     |  (wx wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wy Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]  |
	 * R = |  (wy wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)-(wx Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2]  |
	 *     |                 (wz^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+Cos[Sqrt[wx^2+wy^2+wz^2]]              |
	 */

	/**
	 *          | -radius ( -((2 wx^2 wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wx wy Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)-(wx wy Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx^2 wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2) ) |
	 * df/dwx = | -radius ( -((2 wx wy wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)-(wx^2 Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-Sin[Sqrt[wx^2+wy^2+wz^2]]/Sqrt[wx^2+wy^2+wz^2] )      |
	 *          | -radius ( -((2 wx wz^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wx wz^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wx Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2] )                                                                                                             |
	 */

	/**
	 *          | -radius ( -((2 wx wy wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wy^2 Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)-(wy^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+Sin[Sqrt[wx^2+wy^2+wz^2]]/Sqrt[wx^2+wy^2+wz^2] )      |
	 * df/dwy = | -radius ( -((2 wy^2 wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)-(wx wy Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx wy Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wy^2 wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2) ) |
	 *          | -radius ( -((2 wy wz^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wy wz^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wy Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2] )                                                                                                             |
	 */

	/**
	 *          | -radius ( -((2 wx wz^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wx (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wy wz Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)-(wy wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wx wz^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2) ) |
	 * df/dwz = | -radius ( -((2 wy wz^2 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(wy (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)-(wx wz Cos[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)+(wx wz Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)+(wy wz^2 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2) ) |
	 *          | -radius ( -((2 wz^3 (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)^2)+(2 wz (1-Cos[Sqrt[wx^2+wy^2+wz^2]]))/(wx^2+wy^2+wz^2)+(wz^3 Sin[Sqrt[wx^2+wy^2+wz^2]])/(wx^2+wy^2+wz^2)^(3/2)-(wz Sin[Sqrt[wx^2+wy^2+wz^2]])/Sqrt[wx^2+wy^2+wz^2] )                                                             |
	 */

	const T angle = sphericalObjectPoint.angle();
	const VectorT3<T>& axis = sphericalObjectPoint.axis();

	if (NumericT<T>::isEqualEps(angle))
	{
		// in the case the angle is zero we have R as the unit matrix and thus, the result is [0, 0, -radius]

		jx[0] = 0;
		jy[0] = 0;
		jz[0] = 0;

		jx[1] = 0;
		jy[1] = 0;
		jz[1] = 0;
	}
	else
	{
		ocean_assert(NumericT<T>::isNotEqualEps(angle));

		const T iAngle = T(1) / angle;
		const T iAngle2 = iAngle * iAngle;
		const T iAngle3 = iAngle2 * iAngle;
		const T iAngle4 = iAngle3 * iAngle;

		const T wx = axis.x();
		const T wy = axis.y();
		const T wz = axis.z();

		const T s = NumericT<T>::sin(angle);
		const T c = NumericT<T>::cos(angle);
		const T c1 = 1 - c;

		const T wx2 = wx * wx;
		const T wy2 = wy * wy;
		const T wz2 = wz * wz;

		jx[0] = -objectPointDistance * (-((2 * wx2 * wz * c1) * iAngle4) + (wz * c1) * iAngle2 + (wx * wy * c) * iAngle2 - (wx * wy * s) * iAngle3 + (wx2 * wz * s) * iAngle3);
		jy[0] =	-objectPointDistance * (-((2 * wx * wy * wz * c1) * iAngle4) - (wx2 * c) * iAngle2 + (wx2 * s) * iAngle3 + (wx * wy * wz * s) * iAngle3 - s * iAngle);
		jz[0] =	-objectPointDistance * (-((2 * wx * wz2 * c1) * iAngle4) + (wx * wz2 * s) * iAngle3 - (wx * s) * iAngle);

		jx[1] = -objectPointDistance * (-((2 * wx * wy * wz * c1) * iAngle4) + (wy2 * c) * iAngle2 - (wy2 * s) * iAngle3 + (wx * wy * wz * s) * iAngle3 + s * iAngle);
		jy[1] =	-objectPointDistance * (-((2 * wy2 * wz * c1) * iAngle4) + (wz * c1) * iAngle2 - (wx * wy * c) * iAngle2 + (wx * wy * s) * iAngle3 + (wy2 * wz * s) * iAngle3);
		jz[1] =	-objectPointDistance * (-((2 * wy * wz2 * c1) * iAngle4) + (wy * wz2 * s) * iAngle3 - (wy * s) * iAngle);

		jx[2] = -objectPointDistance * (-((2 * wx * wz2 * c1) * iAngle4) + (wx * c1) * iAngle2 + (wy * wz * c) * iAngle2 - (wy * wz * s) * iAngle3 + (wx * wz2 * s) * iAngle3);
		jy[2] = -objectPointDistance * (-((2 * wy * wz2 * c1) * iAngle4) + (wy * c1) * iAngle2 - (wx * wz * c) * iAngle2 + (wx * wz * s) * iAngle3 + (wy * wz2 * s) * iAngle3);
		jz[2] = -objectPointDistance * (-((2 * wz2 * wz * c1) * iAngle4) + (2 * wz * c1) * iAngle2 + (wz2 * wz * s) * iAngle3 - (wz * s) * iAngle);
	}
}

template void OCEAN_GEOMETRY_EXPORT Jacobian::calculateSphericalObjectPointJacobian3x3(float* jx, float* jy, float* jz, const ExponentialMapT<float>& sphericalObjectPoint, const float objectPointDistance);
template void OCEAN_GEOMETRY_EXPORT Jacobian::calculateSphericalObjectPointJacobian3x3(double* jx, double* jy, double* jz, const ExponentialMapT<double>& sphericalObjectPoint, const double objectPointDistance);

template <typename T>
void Jacobian::calculateSphericalObjectPointOrientationJacobian2x3IF(T* jx, T* jy, const AnyCameraT<T>& camera, const SquareMatrixT3<T>& flippedCamera_R_world, const ExponentialMapT<T>& sphericalObjectPoint, const T objectPointDistance)
{
	ocean_assert(jx != nullptr && jy != nullptr);
	ocean_assert(camera.isValid());
	ocean_assert(!flippedCamera_R_world.isSingular());
	ocean_assert(objectPointDistance > NumericT<T>::eps());

	/**
		* This functions uses four concatenated jacobian matrix to receive one final jacobian matrix with size 2x3.
		* The final jacobian is defined by: j = jFocal_Dist * jDeh * jTrans * jO;
		*
		* jFocal_Dist(u, v) = [Fx * (u * rad_factor + tan_factor_u) + mx, Fy * (v * rad_factor + tan_factor_v) + my]
		*
		* j (2x2) the final jacobian:
		* | dfx / dwx, dfx / dwz |
		* | dfy / dwx, dfy / dwz |
		*
		* jFocal_Dist (2x2) the jacobian for the focal projection and distortion correction:
		* | du' / du, du' / dv |   | ...  ... |
		* | dv' / du, dv' / dv | = | ...  ... |
		*
		* jDeh (2x3) the jacobian for the de-homogenization:                                        | U |
		* | du / dU, du / dV, du / dW |   | 1 / W       0         -U / W^2 |   | u |   | U / W |   | V |
		* | dv / dU, dv / dV, dv / dW | = |   0       1 / W       -V / W^2 |,  | v | = | V / W | = | W |
		*
		* jTrans (3x3) the jacobian for the transformation:
		* | dfx / dox, dfx / doy, dfx / doz |   | Rxx Ryx Rzx |
		* | dfy / dox, dfy / doy, dfy / doz | = | Rxy Ryy Rzy |
		* | dfz / dox, dfz / doy, dfz / doz |   | Rxz Ryz Rzz |
		*
		* jO (3x3) the jacobian for the object point:
		* | dfx / dwx, dfx / dwy, dfx / dwz |
		* | dfy / dwx, dfy / dwy, dfy / dwz |
		* | dfz / dwx, dfy / dwy, dfz / dwz |
		*/

	const VectorT3<T> objectPoint(sphericalObjectPoint.quaternion() * VectorT3<T>(0, 0, -objectPointDistance));

	ocean_assert(AnyCameraT<T>::isObjectPointInFrontIF(flippedCamera_R_world, objectPoint));

	const VectorT3<T> flippedCameraObjectPoint(flippedCamera_R_world * objectPoint);

	StaticMatrix<T, 3, 3> jacobianObjectPoint;
	calculateSphericalObjectPointJacobian3x3(jacobianObjectPoint.row(0), jacobianObjectPoint.row(1), jacobianObjectPoint.row(2), sphericalObjectPoint, objectPointDistance);

	const StaticMatrix<T, 3, 3> jacobianTransformation(flippedCamera_R_world.data(), false);

	StaticMatrix<T, 2, 3> jacobianProjection; // jFocal_Dist * jDeh

	camera.pointJacobian2x3IF(flippedCameraObjectPoint, jacobianProjection.row(0), jacobianProjection.row(1));

	const StaticMatrix<T, 2, 3> finalJacobian((jacobianProjection * jacobianTransformation) * jacobianObjectPoint);

	jx[0] = finalJacobian[0];
	jx[1] = finalJacobian[1];
	jx[2] = finalJacobian[2];

	jy[0] = finalJacobian[3];
	jy[1] = finalJacobian[4];
	jy[2] = finalJacobian[5];
}

template void OCEAN_GEOMETRY_EXPORT Jacobian::calculateSphericalObjectPointOrientationJacobian2x3IF(float* jx, float* jy, const AnyCameraT<float>& camera, const SquareMatrixT3<float>& flippedCamera_R_world, const ExponentialMapT<float>& sphericalObjectPoint, const float objectPointDistance);
template void OCEAN_GEOMETRY_EXPORT Jacobian::calculateSphericalObjectPointOrientationJacobian2x3IF(double* jx, double* jy, const AnyCameraT<double>& camera, const SquareMatrixT3<double>& flippedCamera_R_world, const ExponentialMapT<double>& sphericalObjectPoint, const double objectPointDistance);

template <typename T>
void Jacobian::calculateOrientationJacobianRodrigues2nx3IF(T* jacobian, const AnyCameraT<T>& camera, const ExponentialMapT<T>& flippedCamera_R_world, const ConstIndexedAccessor<VectorT3<T>>& objectPoints)
{
	SquareMatrixT3<T> Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(flippedCamera_R_world, Rwx, Rwy, Rwz);

	/**
		* fFocal(fDist(fDeh(fTrans(fFlippedPose))))
		*
		* This functions uses three concatenated jacobian matrix for each object point to receive one final jacobian matrix with size 2x3 for each object point.
		* The final jacobian is defined by: j = jFocal_Dist * jDeh * jTrans;
		*
		* j (2x6) the final jacobian:
		* | dfx / dwx, dfx / dwy, dfx / dwz |
		* | dfy / dwx, dfy / dwy, dfy / dwz |
		*
		* jFocal_Dist (2x2) the jacobian for the focal projection and distortion correction:
		* | Fx * fdistx / du, Fx * fdistx / dv |   | ...  ... |
		* | Fy * fdisty / du, Fy * fdisty / dv | = | ...  ... |
		*
		* jDeh (2x3) the jacobian for the de-homogenization:                                        | U |
		* | du / dU, du / dV, du / dW |   | 1 / W       0         -U / W^2 |   | u |   | U / W |   | V |
		* | dv / dU, dv / dV, dv / dW | = |   0       1 / W       -V / W^2 |,  | v | = | V / W | = | W |
		*
		* jTrans (3x3) the jacobian for the transformation:
		* | dfx / dwx, dfx / dwy, dfx / dwz |
		* | dfy / dwx, dfy / dwy, dfy / dwz |
		* | dfz / dwx, dfz / dwy, dfz / dwz |
		*
		* Final Jacobian:
		* | Fx / W * fdistx / du, Fx / W * fdistx / dv, -Fx / W^2 * (U * fdistx / du + V * fdistx / dv) |
		* | Fy / W * fdisty / du, Fy / W * fdisty / dv, -Fy / W^2 * (U * fdisty / du + V * fdisty / dv) | * | dR(w) / dw |
		*/

	const HomogenousMatrixT4<T> flippedCamera_T_world(flippedCamera_R_world.quaternion());

	std::array<T, 3> jacobianCameraX;
	std::array<T, 3> jacobianCameraY;

	for (size_t n = 0; n < objectPoints.size(); ++n)
	{
		const VectorT3<T>& worldObjectPoint = objectPoints[n];

		ocean_assert(AnyCameraT<T>::isObjectPointInFrontIF(flippedCamera_T_world, worldObjectPoint));

		const VectorT3<T> flippedCameraObjectPoint(flippedCamera_T_world * worldObjectPoint);

		camera.pointJacobian2x3IF(flippedCameraObjectPoint, jacobianCameraX.data(), jacobianCameraY.data());

		const VectorT3<T> dwx(Rwx * worldObjectPoint);
		const VectorT3<T> dwy(Rwy * worldObjectPoint);
		const VectorT3<T> dwz(Rwz * worldObjectPoint);

		T* const jx = jacobian + 0;
		T* const jy = jacobian + 3;

		// now, we apply the chain rule to determine the left 2x3 sub-matrix
		jx[0] = jacobianCameraX[0] * dwx[0] + jacobianCameraX[1] * dwx[1] + jacobianCameraX[2] * dwx[2];
		jx[1] = jacobianCameraX[0] * dwy[0] + jacobianCameraX[1] * dwy[1] + jacobianCameraX[2] * dwy[2];
		jx[2] = jacobianCameraX[0] * dwz[0] + jacobianCameraX[1] * dwz[1] + jacobianCameraX[2] * dwz[2];

		jy[0] = jacobianCameraY[0] * dwx[0] + jacobianCameraY[1] * dwx[1] + jacobianCameraY[2] * dwx[2];
		jy[1] = jacobianCameraY[0] * dwy[0] + jacobianCameraY[1] * dwy[1] + jacobianCameraY[2] * dwy[2];
		jy[2] = jacobianCameraY[0] * dwz[0] + jacobianCameraY[1] * dwz[1] + jacobianCameraY[2] * dwz[2];

		jacobian += 6;
	}
}

template void OCEAN_GEOMETRY_EXPORT Jacobian::calculateOrientationJacobianRodrigues2nx3IF(float* jacobian, const AnyCameraT<float>& camera, const ExponentialMapT<float>& flippedCamera_P_world, const ConstIndexedAccessor<VectorT3<float>>& objectPoints);
template void OCEAN_GEOMETRY_EXPORT Jacobian::calculateOrientationJacobianRodrigues2nx3IF(double* jacobian, const AnyCameraT<double>& camera, const ExponentialMapT<double>& flippedCamera_P_world, const ConstIndexedAccessor<VectorT3<double>>& objectPoints);

void Jacobian::calculateOrientationJacobianRodrigues2nx3(Scalar* jacobian, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const ConstIndexedAccessor<Vector3>& objectPoints, const bool distortImagePoints)
{
	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

	if (!distortImagePoints)
	{
		/**
		 * This functions uses three concatenated jacobian matrix for each object point to receive one final jacobian matrix with size 2x3 for each object point.
		 * The final jacobian is defined by: j = jDeh * jProj * jTrans;
		 *
		 * j (2x3) the final jacobian:
		 * | dfx / dwx, dfx / dwy, dfx / dwz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz |
		 *
		 * jDeh (2x3) the jacobian for the dehomogenization:                                       | U |
		 * | du / dU, du / dV, du / dW |   | 1 / W       0         -U / W^2 |   | u |   | U / W |  | V |
		 * | dv / dU, dv / dV, dv / dW | = |   0       1 / W       -V / W^2 |,  | v | = | V / W |, | W |
		 *
		 * jProj (3x3) the jacobian for the projection:
		 * | Fx   0  mx |
		 * | 0   Fy  my |
		 * | 0    0   1 |
		 *
		 * jTrans (3x3) the jacobian for the transformation:
		 * | dfx / dwx, dfx / dwy, dfx / dwz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz |
		 * | dfz / dwx, dfz / dwy, dfz / dwz |
		 *
		 * Final Jacobian:
		 * | Fx / W * dfx / dwx + (mx / W - U / W^2) * dfz / dwx, ..., ... |   | Fx / Z' * dfx / dwx - Fx * X' / Z'^2 * dfz / dwx, ..., ... |
		 * | Fy / W * dfy / dwx + (my / W - V / W^2) * dfz / dwx, ..., ... | = | Fy / Z' * dfy / dwx - Fy * Y' / Z'^2 * dfz / dwx, ..., ... |
		 */

		const HomogenousMatrix4 flippedCamera_T_world(flippedCamera_P_world.transformation());

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const ObjectPoint& objectPoint = objectPoints[n];

			const Vector3 transformedObjectPoint(flippedCamera_T_world * objectPoint);

			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
			const Scalar scaleFactor = Scalar(1) / transformedObjectPoint.z();

			const Scalar fx_z = pinholeCamera.focalLengthX() * scaleFactor;
			const Scalar fy_z = pinholeCamera.focalLengthY() * scaleFactor;

			const Scalar x_z = transformedObjectPoint.x() * scaleFactor;
			const Scalar y_z = transformedObjectPoint.y() * scaleFactor;

			const Vector3 dwx(Rwx * objectPoint);
			const Vector3 dwy(Rwy * objectPoint);
			const Vector3 dwz(Rwz * objectPoint);

			*jacobian++ = fx_z * (dwx[0] - x_z * dwx[2]);
			*jacobian++ = fx_z * (dwy[0] - x_z * dwy[2]);
			*jacobian++ = fx_z * (dwz[0] - x_z * dwz[2]);

			*jacobian++ = fy_z * (dwx[1] - y_z * dwx[2]);
			*jacobian++ = fy_z * (dwy[1] - y_z * dwy[2]);
			*jacobian++ = fy_z * (dwz[1] - y_z * dwz[2]);
		}
	}
	else
	{
		/**
		 * fFocal(fDist(fDeh(fTrans(fFlippedPose))))
		 *
		 * This functions uses three concatenated jacobian matrix for each object point to receive one final jacobian matrix with size 2x3 for each object point.
		 * The final jacobian is defined by: j = jFocal_Dist * jDeh * jTrans;
		 *
		 * j (2x6) the final jacobian:
		 * | dfx / dwx, dfx / dwy, dfx / dwz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz |
		 *
		 * jFocal_Dist (2x2) the jacobian for the focal projection and distortion correction:
		 * | Fx * fdistx / du, Fx * fdistx / dv |   | ...  ... |
		 * | Fy * fdisty / du, Fy * fdisty / dv | = | ...  ... |
		 *
		 * jDeh (2x3) the jacobian for the dehomogenization:                                        | U |
		 * | du / dU, du / dV, du / dW |   | 1 / W       0         -U / W^2 |   | u |   | U / W |   | V |
		 * | dv / dU, dv / dV, dv / dW | = |   0       1 / W       -V / W^2 |,  | v | = | V / W | = | W |
		 *
		 * jTrans (3x3) the jacobian for the transformation:
		 * | dfx / dwx, dfx / dwy, dfx / dwz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz |
		 * | dfz / dwx, dfz / dwy, dfz / dwz |
		 *
		 * Final Jacobian:
		 * | Fx / W * fdistx / du, Fx / W * fdistx / dv, -Fx / W^2 * (U * fdistx / du + V * fdistx / dv) |
		 * | Fy / W * fdisty / du, Fy / W * fdisty / dv, -Fy / W^2 * (U * fdisty / du + V * fdisty / dv) | * | dR(w) / dw |
		 */

		const Scalar k1 = pinholeCamera.radialDistortion().first;
		const Scalar k2 = pinholeCamera.radialDistortion().second;

		const Scalar p1 = pinholeCamera.tangentialDistortion().first;
		const Scalar p2 = pinholeCamera.tangentialDistortion().second;

		const HomogenousMatrix4 flippedCamera_T_world(flippedCamera_P_world.transformation());

		for (size_t n = 0; n < objectPoints.size(); ++n)
		{
			const ObjectPoint& objectPoint = objectPoints[n];

			const Vector3 transformedObjectPoint(flippedCamera_T_world * objectPoint);

			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
			const Scalar factor = Scalar(1) / transformedObjectPoint.z();

			const Scalar u = transformedObjectPoint.x() * factor;
			const Scalar v = transformedObjectPoint.y() * factor;

			const Scalar dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
			const Scalar dist2_u_1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v))));
			const Scalar dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);

			const Scalar Fx_w_dist1_u = pinholeCamera.focalLengthX() * factor * dist1_u;
			const Scalar Fy_w_dist2_u = pinholeCamera.focalLengthY() * factor * dist2_u_1_v;

			const Scalar Fx_w_dist1_v = pinholeCamera.focalLengthX() * factor * dist2_u_1_v;
			const Scalar Fy_w_dist2_v = pinholeCamera.focalLengthY() * factor * dist2_v;

			const Scalar Fx_w2__ = -pinholeCamera.focalLengthX() * factor * factor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist2_u_1_v);
			const Scalar Fy_w2__ = -pinholeCamera.focalLengthY() * factor * factor * (transformedObjectPoint.x() * dist2_u_1_v + transformedObjectPoint.y() * dist2_v);

			const Vector3 dwx(Rwx * objectPoint);
			const Vector3 dwy(Rwy * objectPoint);
			const Vector3 dwz(Rwz * objectPoint);

			*jacobian++ = Fx_w_dist1_u * dwx[0] + Fx_w_dist1_v * dwx[1] + Fx_w2__ * dwx[2];
			*jacobian++ = Fx_w_dist1_u * dwy[0] + Fx_w_dist1_v * dwy[1] + Fx_w2__ * dwy[2];
			*jacobian++ = Fx_w_dist1_u * dwz[0] + Fx_w_dist1_v * dwz[1] + Fx_w2__ * dwz[2];

			*jacobian++ = Fy_w_dist2_u * dwx[0] + Fy_w_dist2_v * dwx[1] + Fy_w2__ * dwx[2];
			*jacobian++ = Fy_w_dist2_u * dwy[0] + Fy_w_dist2_v * dwy[1] + Fy_w2__ * dwy[2];
			*jacobian++ = Fy_w_dist2_u * dwz[0] + Fy_w_dist2_v * dwz[1] + Fy_w2__ * dwz[2];
		}
	}
}

void Jacobian::calculatePoseJacobianRodrigues2x6(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_P_world, const Vector3& objectPoint, const bool distortImagePoint, const SquareMatrix3& Rwx, const SquareMatrix3& Rwy, const SquareMatrix3& Rwz)
{
	if (!distortImagePoint)
	{
		const Vector3 transformedObjectPoint(flippedCamera_P_world * objectPoint);

		ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
		const Scalar scaleFactor = 1 / transformedObjectPoint.z();

		const Scalar fx_z = pinholeCamera.focalLengthX() * scaleFactor;
		const Scalar fy_z = pinholeCamera.focalLengthY() * scaleFactor;

		const Scalar fx_x_z2 = -fx_z * transformedObjectPoint.x() * scaleFactor;
		const Scalar fy_y_z2 = -fy_z * transformedObjectPoint.y() * scaleFactor;

		const Vector3 dwx(Rwx * objectPoint);
		const Vector3 dwy(Rwy * objectPoint);
		const Vector3 dwz(Rwz * objectPoint);

		jx[0] = fx_z * dwx[0] + fx_x_z2 * dwx[2];
		jx[1] = fx_z * dwy[0] + fx_x_z2 * dwy[2];
		jx[2] = fx_z * dwz[0] + fx_x_z2 * dwz[2];
		jx[3] = fx_z;
		jx[4] = 0;
		jx[5] = fx_x_z2;

		jy[0] = fy_z * dwx[1] + fy_y_z2 * dwx[2];
		jy[1] = fy_z * dwy[1] + fy_y_z2 * dwy[2];
		jy[2] = fy_z * dwz[1] + fy_y_z2 * dwz[2];
		jy[3] = 0;
		jy[4] = fy_z;
		jy[5] = fy_y_z2;
	}
	else
	{
		const Scalar k1 = pinholeCamera.radialDistortion().first;
		const Scalar k2 = pinholeCamera.radialDistortion().second;

		const Scalar p1 = pinholeCamera.tangentialDistortion().first;
		const Scalar p2 = pinholeCamera.tangentialDistortion().second;

		const Vector3 transformedObjectPoint(flippedCamera_P_world * objectPoint);

		ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
		const Scalar factor = Scalar(1) / transformedObjectPoint.z();

		const Scalar u = transformedObjectPoint.x() * factor;
		const Scalar v = transformedObjectPoint.y() * factor;

		const Scalar dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
		const Scalar dist2_u_1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v))));
		const Scalar dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);

		const Scalar Fx_w_dist1_u = pinholeCamera.focalLengthX() * factor * dist1_u;
		const Scalar Fy_w_dist2_u = pinholeCamera.focalLengthY() * factor * dist2_u_1_v;

		const Scalar Fx_w_dist1_v = pinholeCamera.focalLengthX() * factor * dist2_u_1_v;
		const Scalar Fy_w_dist2_v = pinholeCamera.focalLengthY() * factor * dist2_v;

		const Scalar Fx_w2__ = -pinholeCamera.focalLengthX() * factor * factor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist2_u_1_v);
		const Scalar Fy_w2__ = -pinholeCamera.focalLengthY() * factor * factor * (transformedObjectPoint.x() * dist2_u_1_v + transformedObjectPoint.y() * dist2_v);

		const Vector3 dwx(Rwx * objectPoint);
		const Vector3 dwy(Rwy * objectPoint);
		const Vector3 dwz(Rwz * objectPoint);

		jx[0] = Fx_w_dist1_u * dwx[0] + Fx_w_dist1_v * dwx[1] + Fx_w2__ * dwx[2];
		jx[1] = Fx_w_dist1_u * dwy[0] + Fx_w_dist1_v * dwy[1] + Fx_w2__ * dwy[2];
		jx[2] = Fx_w_dist1_u * dwz[0] + Fx_w_dist1_v * dwz[1] + Fx_w2__ * dwz[2];
		jx[3] = Fx_w_dist1_u;
		jx[4] = Fx_w_dist1_v;
		jx[5] = Fx_w2__;

		jy[0] = Fy_w_dist2_u * dwx[0] + Fy_w_dist2_v * dwx[1] + Fy_w2__ * dwx[2];
		jy[1] = Fy_w_dist2_u * dwy[0] + Fy_w_dist2_v * dwy[1] + Fy_w2__ * dwy[2];
		jy[2] = Fy_w_dist2_u * dwz[0] + Fy_w_dist2_v * dwz[1] + Fy_w2__ * dwz[2];
		jy[3] = Fy_w_dist2_u;
		jy[4] = Fy_w_dist2_v;
		jy[5] = Fy_w2__;
	}
}

void Jacobian::calculatePoseJacobianRodrigues2x6(Scalar* jx, Scalar* jy, const FisheyeCamera& fisheyeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& worldObjectPoint, const SquareMatrix3& Rwx, const SquareMatrix3& Rwy, const SquareMatrix3& Rwz)
{
	ocean_assert(jx != nullptr && jy != nullptr);
	ocean_assert(fisheyeCamera.isValid());
	ocean_assert(flippedCamera_T_world.isValid());

	Scalar jDistX[2];
	Scalar jDistY[2];

	const Scalar& fx = fisheyeCamera.focalLengthX();
	const Scalar& fy = fisheyeCamera.focalLengthY();

	const Vector3 flippedCamera_objectPoint = flippedCamera_T_world * worldObjectPoint;
	const Scalar& u = flippedCamera_objectPoint.x();
	const Scalar& v = flippedCamera_objectPoint.y();
	const Scalar& w = flippedCamera_objectPoint.z();

	ocean_assert(Numeric::isNotEqualEps(w));
	const Scalar invW = Scalar(1) / w;

	const Scalar u_invW = u * invW;
	const Scalar v_invW = v * invW;

	calculateFisheyeDistortNormalized2x2(jDistX, jDistY, u_invW, v_invW, fisheyeCamera.radialDistortion(), fisheyeCamera.tangentialDistortion());

	const Scalar fx_jDistXx_invW = fx * jDistX[0] * invW;
	const Scalar fy_jDistYx_invW = fy * jDistY[0] * invW;

	const Scalar fx_jDistXy_invW = fx * jDistX[1] * invW;
	const Scalar fy_jDistYy_invW = fy * jDistY[1] * invW;

	const Scalar u_fx_jDistXx__ = u_invW * fx_jDistXx_invW + v_invW * fx_jDistXy_invW;
	const Scalar u_fy_jDistYx__ = u_invW * fy_jDistYx_invW + v_invW * fy_jDistYy_invW;

	const Vector3 dwx(Rwx * worldObjectPoint);
	const Vector3 dwy(Rwy * worldObjectPoint);
	const Vector3 dwz(Rwz * worldObjectPoint);

	jx[0] = fx_jDistXx_invW * dwx.x() + fx_jDistXy_invW * dwx.y() - u_fx_jDistXx__ * dwx.z();
	jx[1] = fx_jDistXx_invW * dwy.x() + fx_jDistXy_invW * dwy.y() - u_fx_jDistXx__ * dwy.z();
	jx[2] = fx_jDistXx_invW * dwz.x() + fx_jDistXy_invW * dwz.y() - u_fx_jDistXx__ * dwz.z();
	jx[3] = fx_jDistXx_invW;
	jx[4] = fx_jDistXy_invW;
	jx[5] = -u_fx_jDistXx__;

	jy[0] = fy_jDistYx_invW * dwx.x() + fy_jDistYy_invW * dwx.y() - u_fy_jDistYx__ * dwx.z();
	jy[1] = fy_jDistYx_invW * dwy.x() + fy_jDistYy_invW * dwy.y() - u_fy_jDistYx__ * dwy.z();
	jy[2] = fy_jDistYx_invW * dwz.x() + fy_jDistYy_invW * dwz.y() - u_fy_jDistYx__ * dwz.z();
	jy[3] = fy_jDistYx_invW;
	jy[4] = fy_jDistYy_invW;
	jy[5] = -u_fy_jDistYx__;

#ifdef SLOWER_IMPLEMENTATION

	const Vector3 dwx(Rwx * worldObjectPoint);
	const Vector3 dwy(Rwy * worldObjectPoint);
	const Vector3 dwz(Rwz * worldObjectPoint);

	StaticMatrix<Scalar, 3, 6> jacobianPose;
	jacobianPose(0, 0) = dwx[0];
	jacobianPose(1, 0) = dwx[1];
	jacobianPose(2, 0) = dwx[2];

	jacobianPose(0, 1) = dwy[0];
	jacobianPose(1, 1) = dwy[1];
	jacobianPose(2, 1) = dwy[2];

	jacobianPose(0, 2) = dwz[0];
	jacobianPose(1, 2) = dwz[1];
	jacobianPose(2, 2) = dwz[2];

	jacobianPose(0, 3) = 1;
	jacobianPose(1, 3) = 0;
	jacobianPose(2, 3) = 0;

	jacobianPose(0, 4) = 0;
	jacobianPose(1, 4) = 1;
	jacobianPose(2, 4) = 0;

	jacobianPose(0, 5) = 0;
	jacobianPose(1, 5) = 0;
	jacobianPose(2, 5) = 1;

	const Vector3 flippedCamera_objectPoint = flippedCamera_T_world * worldObjectPoint;

	ocean_assert(Numeric::isNotEqualEps(flippedCamera_objectPoint.z()));
	const Scalar factor = Scalar(1) / flippedCamera_objectPoint.z();

	StaticMatrix<Scalar, 2, 3> jacobianDehomogenous;
	jacobianDehomogenous(0, 0) = factor;
	jacobianDehomogenous(1, 0) = 0;
	jacobianDehomogenous(0, 1) = 0;
	jacobianDehomogenous(1, 1) = factor;
	jacobianDehomogenous(0, 2) = -flippedCamera_objectPoint.x() * factor * factor;
	jacobianDehomogenous(1, 2) = -flippedCamera_objectPoint.y() * factor * factor;

	StaticMatrix<Scalar, 2, 2> jacobianDistortion;
	calculateFisheyeDistortNormalized2x2(jacobianDistortion.row(0), jacobianDistortion.row(1), flippedCamera_objectPoint.x() * factor, flippedCamera_objectPoint.y() * factor, fisheyeCamera.radialDistortion(), fisheyeCamera.tangentialDistortion());

	StaticMatrix<Scalar, 2, 2> jacobianFocal(false);
	jacobianFocal(0, 0) = fisheyeCamera.focalLengthX();
	jacobianFocal(1, 1) = fisheyeCamera.focalLengthY();

	StaticMatrix<Scalar, 2, 6> finalJacobian(((jacobianFocal * jacobianDistortion) * jacobianDehomogenous) * jacobianPose);

	jx[0] = finalJacobian(0, 0);
	jx[1] = finalJacobian(0, 1);
	jx[2] = finalJacobian(0, 2);
	jx[3] = finalJacobian(0, 3);
	jx[4] = finalJacobian(0, 4);
	jx[5] = finalJacobian(0, 5);

	jy[0] = finalJacobian(1, 0);
	jy[1] = finalJacobian(1, 1);
	jy[2] = finalJacobian(1, 2);
	jy[3] = finalJacobian(1, 3);
	jy[4] = finalJacobian(1, 4);
	jy[5] = finalJacobian(1, 5);

#endif // SLOWER_IMPLEMENTATION
}

template <typename T>
void Jacobian::calculatePoseJacobianRodrigues2nx6IF(T* jacobian, const AnyCameraT<T>& camera, const PoseT<T>& flippedCamera_P_world, const VectorT3<T>* objectPoints, const size_t numberObjectPoints)
{
	ocean_assert(jacobian != nullptr);
	ocean_assert(camera.isValid());
	ocean_assert(objectPoints != nullptr);
	ocean_assert(numberObjectPoints >= 1);

	SquareMatrixT3<T> Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative<T>(ExponentialMapT<T>(VectorT3<T>(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

	const HomogenousMatrixT4<T> flippedCamera_T_world(flippedCamera_P_world.transformation());

	for (size_t n = 0; n < numberObjectPoints; ++n)
	{
		T* const jx = jacobian;
		T* const jy = jacobian + 6;

		const VectorT3<T>& objectPoint = objectPoints[n];

		camera.pointJacobian2x3IF(flippedCamera_T_world * objectPoint, jx + 3, jy + 3);

		const VectorT3<T> dwx(Rwx * objectPoint);
		const VectorT3<T> dwy(Rwy * objectPoint);
		const VectorT3<T> dwz(Rwz * objectPoint);

		// now, we apply the chain rule to determine the left 2x3 sub-matrix
		jx[0] = jx[3] * dwx[0] + jx[4] * dwx[1] + jx[5] * dwx[2];
		jx[1] = jx[3] * dwy[0] + jx[4] * dwy[1] + jx[5] * dwy[2];
		jx[2] = jx[3] * dwz[0] + jx[4] * dwz[1] + jx[5] * dwz[2];

		jy[0] = jy[3] * dwx[0] + jy[4] * dwx[1] + jy[5] * dwx[2];
		jy[1] = jy[3] * dwy[0] + jy[4] * dwy[1] + jy[5] * dwy[2];
		jy[2] = jy[3] * dwz[0] + jy[4] * dwz[1] + jy[5] * dwz[2];

		jacobian += 12;
	}
}

template void OCEAN_GEOMETRY_EXPORT Jacobian::calculatePoseJacobianRodrigues2nx6IF(float* jacobian, const AnyCameraT<float>& camera, const PoseT<float>& flippedCamera_P_world, const VectorT3<float>* objectPoints, const size_t numberObjectPoints);
template void OCEAN_GEOMETRY_EXPORT Jacobian::calculatePoseJacobianRodrigues2nx6IF(double* jacobian, const AnyCameraT<double>& camera, const PoseT<double>& flippedCamera_P_world, const VectorT3<double>* objectPoints, const size_t numberObjectPoints);

void Jacobian::calculatePoseJacobianRodrigues2nx6(Scalar* jacobian, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const Vector3* objectPoints, const size_t numberObjectPoints, const bool distortImagePoints)
{
	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

	if (!distortImagePoints)
	{
		/**
		 * This functions uses three concatenated jacobian matrix for each object point to receive one final jacobian matrix with size 2x6 for each object point.
		 * The final jacobian is defined by: j = jDeh * jProj * jTrans;
		 *
		 * j (2x6) the final jacobian:
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 *
		 * jDeh (2x3) the jacobian for the dehomogenization:                                       | U |
		 * | du / dU, du / dV, du / dW |   | 1 / W       0         -U / W^2 |   | u |   | U / W |  | V |
		 * | dv / dU, dv / dV, dv / dW | = |   0       1 / W       -V / W^2 |,  | v | = | V / W |, | W |
		 *
		 * jProj (3x3) the jacobian for the projection:
		 * | Fx   0  mx |
		 * | 0   Fy  my |
		 * | 0    0   1 |
		 *
		 * jTrans (3x6) the jacobian for the transformation:
		 * | dfx / dwx, dfx / dwy, dfx / dwz,   1,   0,   0 |
		 * | dfy / dwx, dfy / dwy, dfy / dwz,   0,   1,   0 |
		 * | dfz / dwx, dfz / dwy, dfz / dwz,   0,   0,   1 |
		 *
		 * Final Jacobian:
		 * | Fx / W,    0  , mx / W - U / W^2 |                              | Fx / W * dfx / dwx + (mx / W - U / W^2) * dfz / dwx, ..., ... |, | Fx / W,    0  , mx / W - U / W^2 |
		 * |    0  , Fy / W, my / W - V / W^2 | * | dR(w) / dw, dft / dt | = | Fy / W * dfy / dwx + (my / W - V / W^2) * dfz / dwx, ..., ... |, |    0  , Fy / W, my / W - V / W^2 |
		 *
		 * = | Fx / Z',    0   , Fx X' / Z'^2 |                              | ..., ..., ..., ..., ..., ... |
		 *   |    0   , Fy / Z', Fy Y' / Z'^2 | * | dR(w) / dw, dft / dt | = | ..., ..., ..., ..., ..., ... |
		 */

		const HomogenousMatrix4 flippedCamera_T_world(flippedCamera_P_world.transformation());

		for (size_t n = 0; n < numberObjectPoints; ++n)
		{
			const ObjectPoint& objectPoint = objectPoints[n];

			const Vector3 transformedObjectPoint(flippedCamera_T_world * objectPoint);

			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
			const Scalar scaleFactor = 1 / transformedObjectPoint.z();

			const Scalar fx_z = pinholeCamera.focalLengthX() * scaleFactor;
			const Scalar fy_z = pinholeCamera.focalLengthY() * scaleFactor;

			const Scalar fx_x_z2 = -fx_z * transformedObjectPoint.x() * scaleFactor;
			const Scalar fy_y_z2 = -fy_z * transformedObjectPoint.y() * scaleFactor;

			const Vector3 dwx(Rwx * objectPoint);
			const Vector3 dwy(Rwy * objectPoint);
			const Vector3 dwz(Rwz * objectPoint);

			*jacobian++ = fx_z * dwx[0] + fx_x_z2 * dwx[2];
			*jacobian++ = fx_z * dwy[0] + fx_x_z2 * dwy[2];
			*jacobian++ = fx_z * dwz[0] + fx_x_z2 * dwz[2];
			*jacobian++ = fx_z;
			*jacobian++ = 0;
			*jacobian++ = fx_x_z2;

			*jacobian++ = fy_z * dwx[1] + fy_y_z2 * dwx[2];
			*jacobian++ = fy_z * dwy[1] + fy_y_z2 * dwy[2];
			*jacobian++ = fy_z * dwz[1] + fy_y_z2 * dwz[2];
			*jacobian++ = 0;
			*jacobian++ = fy_z;
			*jacobian++ = fy_y_z2;
		}
	}
	else
	{
		/**
		 * fFocal(fDist(fDeh(fTrans(fFlippedPose))))
		 *
		 * This functions uses three concatenated jacobian matrix for each object point to receive one final jacobian matrix with size 2x6 for each object point.
		 * The final jacobian is defined by: j = jFocal_Dist * jDeh * jTrans;
		 *
		 * j (2x6) the final jacobian:
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz |
		 *
		 * jFocal_Dist (2x2) the jacobian for the focal projection and distortion correction:
		 * | Fx * fdistx / du, Fx * fdistx / dv |   | ...  ... |
		 * | Fy * fdisty / du, Fy * fdisty / dv | = | ...  ... |
		 *
		 * jDeh (2x3) the jacobian for the dehomogenization:                                        | U |
		 * | du / dU, du / dV, du / dW |   | 1 / W       0         -U / W^2 |   | u |   | U / W |   | V |
		 * | dv / dU, dv / dV, dv / dW | = |   0       1 / W       -V / W^2 |,  | v | = | V / W | = | W |
		 *
		 * jTrans (3x6) the jacobian for the transformation:
		 * | dfx / dwx, dfx / dwy, dfx / dwz,   1,   0,   0 |
		 * | dfy / dwx, dfy / dwy, dfy / dwz,   0,   1,   0 |
		 * | dfz / dwx, dfz / dwy, dfz / dwz,   0,   0,   1 |
		 *
		 * Final Jacobian:
		 * | Fx / W * fdistx / du, Fx / W * fdistx / dv, -Fx / W^2 * (U * fdistx / du + V * fdistx / dv) |
		 * | Fy / W * fdisty / du, Fy / W * fdisty / dv, -Fy / W^2 * (U * fdisty / du + V * fdisty / dv) | * | dR(w) / dw, dft / dt |
		 */

		const Scalar k1 = pinholeCamera.radialDistortion().first;
		const Scalar k2 = pinholeCamera.radialDistortion().second;

		const Scalar p1 = pinholeCamera.tangentialDistortion().first;
		const Scalar p2 = pinholeCamera.tangentialDistortion().second;

		const HomogenousMatrix4 flippedCamera_T_world(flippedCamera_P_world.transformation());

		for (size_t n = 0; n < numberObjectPoints; ++n)
		{
			const ObjectPoint& objectPoint = objectPoints[n];

			const Vector3 transformedObjectPoint(flippedCamera_T_world * objectPoint);

			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
			const Scalar factor = Scalar(1) / transformedObjectPoint.z();

			const Scalar u = transformedObjectPoint.x() * factor;
			const Scalar v = transformedObjectPoint.y() * factor;

			const Scalar dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
			const Scalar dist2_u_1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v))));
			const Scalar dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);

			const Scalar Fx_w_dist1_u = pinholeCamera.focalLengthX() * factor * dist1_u;
			const Scalar Fy_w_dist2_u = pinholeCamera.focalLengthY() * factor * dist2_u_1_v;

			const Scalar Fx_w_dist1_v = pinholeCamera.focalLengthX() * factor * dist2_u_1_v;
			const Scalar Fy_w_dist2_v = pinholeCamera.focalLengthY() * factor * dist2_v;

			const Scalar Fx_w2__ = -pinholeCamera.focalLengthX() * factor * factor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist2_u_1_v);
			const Scalar Fy_w2__ = -pinholeCamera.focalLengthY() * factor * factor * (transformedObjectPoint.x() * dist2_u_1_v + transformedObjectPoint.y() * dist2_v);

			const Vector3 dwx(Rwx * objectPoint);
			const Vector3 dwy(Rwy * objectPoint);
			const Vector3 dwz(Rwz * objectPoint);

			*jacobian++ = Fx_w_dist1_u * dwx[0] + Fx_w_dist1_v * dwx[1] + Fx_w2__ * dwx[2];
			*jacobian++ = Fx_w_dist1_u * dwy[0] + Fx_w_dist1_v * dwy[1] + Fx_w2__ * dwy[2];
			*jacobian++ = Fx_w_dist1_u * dwz[0] + Fx_w_dist1_v * dwz[1] + Fx_w2__ * dwz[2];
			*jacobian++ = Fx_w_dist1_u;
			*jacobian++ = Fx_w_dist1_v;
			*jacobian++ = Fx_w2__;

			*jacobian++ = Fy_w_dist2_u * dwx[0] + Fy_w_dist2_v * dwx[1] + Fy_w2__ * dwx[2];
			*jacobian++ = Fy_w_dist2_u * dwy[0] + Fy_w_dist2_v * dwy[1] + Fy_w2__ * dwy[2];
			*jacobian++ = Fy_w_dist2_u * dwz[0] + Fy_w_dist2_v * dwz[1] + Fy_w2__ * dwz[2];
			*jacobian++ = Fy_w_dist2_u;
			*jacobian++ = Fy_w_dist2_v;
			*jacobian++ = Fy_w2__;
		}
	}
}

void Jacobian::calculatePoseJacobianRodriguesDampedDistortion2nx6(Scalar* jacobian, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const Scalar dampingFactor, const Vector3* objectPoints, const size_t numberObjectPoints, const bool distortImagePoints)
{
	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

	if (!distortImagePoints)
	{
		// we have here the simple Jacobian without any distortion

		const HomogenousMatrix4 flippedCamera_T_world(flippedCamera_P_world.transformation());

		for (size_t n = 0; n < numberObjectPoints; ++n)
		{
			const ObjectPoint& objectPoint = objectPoints[n];

			const Vector3 transformedObjectPoint(flippedCamera_T_world * objectPoint);

			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
			const Scalar scaleFactor = 1 / transformedObjectPoint.z();

			const Scalar fx_z = pinholeCamera.focalLengthX() * scaleFactor;
			const Scalar fy_z = pinholeCamera.focalLengthY() * scaleFactor;

			const Scalar fx_x_z2 = -fx_z * transformedObjectPoint.x() * scaleFactor;
			const Scalar fy_y_z2 = -fy_z * transformedObjectPoint.y() * scaleFactor;

			const Vector3 dwx(Rwx * objectPoint);
			const Vector3 dwy(Rwy * objectPoint);
			const Vector3 dwz(Rwz * objectPoint);

			*jacobian++ = fx_z * dwx[0] + fx_x_z2 * dwx[2];
			*jacobian++ = fx_z * dwy[0] + fx_x_z2 * dwy[2];
			*jacobian++ = fx_z * dwz[0] + fx_x_z2 * dwz[2];
			*jacobian++ = fx_z;
			*jacobian++ = 0;
			*jacobian++ = fx_x_z2;

			*jacobian++ = fy_z * dwx[1] + fy_y_z2 * dwx[2];
			*jacobian++ = fy_z * dwy[1] + fy_y_z2 * dwy[2];
			*jacobian++ = fy_z * dwz[1] + fy_y_z2 * dwz[2];
			*jacobian++ = 0;
			*jacobian++ = fy_z;
			*jacobian++ = fy_y_z2;
		}
	}
	else
	{
		const Scalar k1 = pinholeCamera.radialDistortion().first;
		const Scalar k2 = pinholeCamera.radialDistortion().second;

		const Scalar p1 = pinholeCamera.tangentialDistortion().first;
		const Scalar p2 = pinholeCamera.tangentialDistortion().second;

		const Scalar u0 = -pinholeCamera.principalPointX() * pinholeCamera.inverseFocalLengthX();
		const Scalar u1 = (Scalar(pinholeCamera.width()) - Scalar(1) - pinholeCamera.principalPointX()) * pinholeCamera.inverseFocalLengthX();

		const Scalar v0 = -pinholeCamera.principalPointY() * pinholeCamera.inverseFocalLengthY();
		const Scalar v1 = (Scalar(pinholeCamera.height()) - Scalar(1) - pinholeCamera.principalPointY()) * pinholeCamera.inverseFocalLengthY();

		const HomogenousMatrix4 flippedCamera_T_world(flippedCamera_P_world.transformation());

		for (size_t n = 0; n < numberObjectPoints; ++n)
		{
			const ObjectPoint& objectPoint = objectPoints[n];

			const Vector3 transformedObjectPoint(flippedCamera_T_world * objectPoint);

			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
			const Scalar factor = Scalar(1) / transformedObjectPoint.z();

			const Scalar u = transformedObjectPoint.x() * factor;
			const Scalar v = transformedObjectPoint.y() * factor;

			Scalar dist1_u, dist1_v, dist2_u, dist2_v;

#ifdef OCEAN_DEBUG
			dist1_u = Numeric::minValue();
			dist1_v = Numeric::minValue();
			dist2_u = Numeric::minValue();
			dist2_v = Numeric::minValue();
#endif

			if (u >= u0 && u <= u1)
			{
				if (v >= v0 && v <= v1)
				{
					ocean_assert(u >= u0 && u <= u1 && v >= v0 && v <= v1);

					dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
					dist1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v))));
					dist2_u = dist1_v;
					dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);
				}
				else
				{
					ocean_assert(u >= u0 && u <= u1 && (v < v0 || v > v1));

					const Scalar v_ = (v < v0) ? v0 : v1;

					const Scalar a = v_ * (1 + dampingFactor);
					const Scalar b = dampingFactor * dampingFactor * v_ * v_;
					const Scalar c = v_ * (dampingFactor - 1);

					ocean_assert(Numeric::isNotEqualEps(v + c));
					const Scalar invVC = Scalar(1) / (v + c);

					const Scalar dampedV = a - b * invVC;

					dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * dampedV + k1 * (Scalar(3) * u * u + dampedV * dampedV) + k2 * (u * u + dampedV * dampedV) * (Scalar(5) * u * u + dampedV * dampedV);

					dist1_v = invVC * invVC * (u * (4 * b * k2 * dampedV * (dampedV * dampedV + u * u) + 2 * b * k1 * dampedV) + 2 * b * p2 * dampedV + 2 * b * p1 * u);

					dist2_u = Scalar(2) * (p1 * u + dampedV * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + dampedV * dampedV))));

					const Scalar sqr = dampedV * dampedV + u * u;

					dist2_v = invVC * invVC * (b * (k2 * sqr * sqr + k1 * sqr) + dampedV * (4 * b * k2 * dampedV * sqr + 2 * b * k1 * dampedV) + 6 * b * p1 * dampedV + 2 * b * p2 * u) + 1;
				}
			}
			else
			{
				ocean_assert(u < u0 || u > u1);

				if (v >= v0 && v <= v1)
				{
					ocean_assert((u < u0 || u > u1) && v >= v0 && v <= v1);

					const Scalar u_ = (u < u0) ? u0 : u1;

					const Scalar a = u_ * (1 + dampingFactor);
					const Scalar b = dampingFactor * dampingFactor * u_ * u_;
					const Scalar c = u_ * (dampingFactor - 1);

					ocean_assert(Numeric::isNotEqualEps(u + c));
					const Scalar invUC = Scalar(1) / (u + c);

					const Scalar dampedU = a - b * invUC;

					dist1_v = Scalar(2) * (p1 * dampedU + v * (p2 + dampedU * (k1 + Scalar(2) * k2 * (dampedU * dampedU + v * v))));

					const Scalar sqr = dampedU * dampedU + v * v;

					dist1_u = invUC * invUC * (b * (k2 * sqr * sqr + k1 * sqr) + dampedU * (4 * b * k2 * dampedU * sqr + 2 * b * k1 * dampedU) + 6 * b * p2 * dampedU + 2 * b * p1 * v) + 1;

					dist2_v = Scalar(1) + Scalar(2) * p2 * dampedU + Scalar(6) * p1 * v + k1 * (dampedU * dampedU + Scalar(3) * v * v) + k2 * (dampedU * dampedU + v * v) * (dampedU * dampedU + Scalar(5) * v * v);

					dist2_u = invUC * invUC * (v * (4 * b * k2 * dampedU * sqr + 2 * b * k1 * dampedU) + 2 * b * p1 * dampedU + 2 * b * p2 * v);
				}
				else
				{
					ocean_assert((u < u0 || u > u1) && (v < v0 || v > v1));

					const Scalar u_ = (u < u0) ? u0 : u1;

					const Scalar a_u = u_ * (1 + dampingFactor);
					const Scalar b_u = dampingFactor * dampingFactor * u_ * u_;
					const Scalar c_u = u_ * (dampingFactor - 1);

					const Scalar v_ = (v < v0) ? v0 : v1;

					const Scalar a_v = v_ * (1 + dampingFactor);
					const Scalar b_v = dampingFactor * dampingFactor * v_ * v_;
					const Scalar c_v = v_ * (dampingFactor - 1);

					ocean_assert(Numeric::isNotEqualEps(u + c_u));
					const Scalar invUC = Scalar(1) / (u + c_u);

					const Scalar dampedU = a_u - b_u * invUC;

					ocean_assert(Numeric::isNotEqualEps(v + c_v));
					const Scalar invVC = Scalar(1) / (v + c_v);

					const Scalar dampedV = a_v - b_v * invVC;

					const Scalar sqr = dampedU * dampedU + dampedV * dampedV;

					dist1_u = invUC * invUC * (b_u * (k2 * sqr * sqr + k1 * sqr) + dampedU * (4 * b_u * k2 * dampedU * sqr + 2 * b_u * k1 * dampedU) + 6 * b_u * p2 * dampedU + 2 * b_u * p1 * dampedV) + 1;

					dist1_v = invVC * invVC * (dampedU * (4 * b_v * k2 * dampedV * (dampedV * dampedV + dampedU * dampedU) + 2 * b_v * k1 * dampedV) + 2 * b_v * p2 * dampedV + 2 * b_v * p1 * dampedU);

					dist2_u = invUC * invUC * (dampedV * (4 * b_u * k2 * dampedU * sqr + 2 * b_u * k1 * dampedU) + 2 * b_u * p1 * dampedU + 2 * b_u * p2 * dampedV);

					dist2_v = invVC * invVC * (b_v * (k2 * sqr * sqr + k1 * sqr) + dampedV * (4 * b_v * k2 * dampedV * sqr + 2 * b_v * k1 * dampedV) + 6 * b_v * p1 * dampedV + 2 * b_v * p2 * dampedU) + 1;
				}
			}

			ocean_assert(dist1_u != Numeric::minValue() && dist1_v != Numeric::minValue() && dist2_u != Numeric::minValue() && dist2_v != Numeric::minValue());

			const Scalar Fx_w_dist1_u = pinholeCamera.focalLengthX() * factor * dist1_u;
			const Scalar Fy_w_dist2_u = pinholeCamera.focalLengthY() * factor * dist2_u;

			const Scalar Fx_w_dist1_v = pinholeCamera.focalLengthX() * factor * dist1_v;
			const Scalar Fy_w_dist2_v = pinholeCamera.focalLengthY() * factor * dist2_v;

			const Scalar Fx_w2__ = -pinholeCamera.focalLengthX() * factor * factor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist1_v);
			const Scalar Fy_w2__ = -pinholeCamera.focalLengthY() * factor * factor * (transformedObjectPoint.x() * dist2_u + transformedObjectPoint.y() * dist2_v);

			const Vector3 dwx(Rwx * objectPoint);
			const Vector3 dwy(Rwy * objectPoint);
			const Vector3 dwz(Rwz * objectPoint);

			*jacobian++ = Fx_w_dist1_u * dwx[0] + Fx_w_dist1_v * dwx[1] + Fx_w2__ * dwx[2];
			*jacobian++ = Fx_w_dist1_u * dwy[0] + Fx_w_dist1_v * dwy[1] + Fx_w2__ * dwy[2];
			*jacobian++ = Fx_w_dist1_u * dwz[0] + Fx_w_dist1_v * dwz[1] + Fx_w2__ * dwz[2];
			*jacobian++ = Fx_w_dist1_u;
			*jacobian++ = Fx_w_dist1_v;
			*jacobian++ = Fx_w2__;

			*jacobian++ = Fy_w_dist2_u * dwx[0] + Fy_w_dist2_v * dwx[1] + Fy_w2__ * dwx[2];
			*jacobian++ = Fy_w_dist2_u * dwy[0] + Fy_w_dist2_v * dwy[1] + Fy_w2__ * dwy[2];
			*jacobian++ = Fy_w_dist2_u * dwz[0] + Fy_w_dist2_v * dwz[1] + Fy_w2__ * dwz[2];
			*jacobian++ = Fy_w_dist2_u;
			*jacobian++ = Fy_w_dist2_v;
			*jacobian++ = Fy_w2__;
		}
	}
}

void Jacobian::calculatePoseZoomJacobianRodrigues2x7(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const Scalar zoom, const Vector3& objectPoint, const bool distortImagePoints)
{
	ocean_assert(zoom > Numeric::eps());

	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

	if (!distortImagePoints)
	{
		const Vector3 transformedObjectPoint(flippedCamera_P_world.transformation() * objectPoint);

		ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
		const Scalar scaleFactor = 1 / transformedObjectPoint.z();

		const Scalar sfx_z = zoom * pinholeCamera.focalLengthX() * scaleFactor;
		const Scalar sfxx_z2 = -sfx_z * transformedObjectPoint.x() * scaleFactor;

		const Vector3 dwx(Rwx * objectPoint);
		const Vector3 dwy(Rwy * objectPoint);
		const Vector3 dwz(Rwz * objectPoint);

		jx[0] = sfx_z * dwx[0] + sfxx_z2 * dwx[2];
		jx[1] = sfx_z * dwy[0] + sfxx_z2 * dwy[2];
		jx[2] = sfx_z * dwz[0] + sfxx_z2 * dwz[2];

		jx[3] = sfx_z;
		jx[4] = 0;
		jx[5] = sfxx_z2;
		jx[6] = pinholeCamera.focalLengthX() * transformedObjectPoint.x() * scaleFactor;

		const Scalar sfy_z = zoom * pinholeCamera.focalLengthY() * scaleFactor;
		const Scalar sfyy_z2 = -sfy_z * transformedObjectPoint.y() * scaleFactor;

		jy[0] = sfy_z * dwx[1] + sfyy_z2 * dwx[2];
		jy[1] = sfy_z * dwy[1] + sfyy_z2 * dwy[2];
		jy[2] = sfy_z * dwz[1] + sfyy_z2 * dwz[2];

		jy[3] = 0;
		jy[4] = sfy_z;
		jy[5] = sfyy_z2;
		jy[6] = pinholeCamera.focalLengthY() * transformedObjectPoint.y() * scaleFactor;
	}
	else
	{
		const Scalar k1 = pinholeCamera.radialDistortion().first;
		const Scalar k2 = pinholeCamera.radialDistortion().second;

		const Scalar p1 = pinholeCamera.tangentialDistortion().first;
		const Scalar p2 = pinholeCamera.tangentialDistortion().second;

		const HomogenousMatrix4 flippedCamera_T_world(flippedCamera_P_world.transformation());

		const Vector3 transformedObjectPoint(flippedCamera_T_world * objectPoint);

		ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
		const Scalar scaleFactor = Scalar(1) / transformedObjectPoint.z();

		const Scalar u = transformedObjectPoint.x() * scaleFactor;
		const Scalar v = transformedObjectPoint.y() * scaleFactor;

		const Scalar dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
		const Scalar dist2_u_1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v))));
		const Scalar dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);

		const Scalar s_fx_z_dist1_u = zoom * pinholeCamera.focalLengthX() * scaleFactor * dist1_u;
		const Scalar s_fx_z_dist1_v = zoom * pinholeCamera.focalLengthX() * scaleFactor * dist2_u_1_v;
		const Scalar s_fx_z2__ = -zoom * pinholeCamera.focalLengthX() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist2_u_1_v);

		const Scalar sqrRadius = u * u + v * v;
		const Scalar radialDistortionFactor = 1 + k1 * sqrRadius + k2 * sqrRadius * sqrRadius;
		const Scalar distortedU = u * radialDistortionFactor + p1 * 2 * u * v + p2 * (sqrRadius + 2 * u * u);
		const Scalar distortedV = v * radialDistortionFactor + p2 * 2 * u * v + p1 * (sqrRadius + 2 * v * v);

		const Vector3 dwx(Rwx * objectPoint);
		const Vector3 dwy(Rwy * objectPoint);
		const Vector3 dwz(Rwz * objectPoint);

		jx[0] = s_fx_z_dist1_u * dwx[0] + s_fx_z_dist1_v * dwx[1] + s_fx_z2__ * dwx[2];
		jx[1] = s_fx_z_dist1_u * dwy[0] + s_fx_z_dist1_v * dwy[1] + s_fx_z2__ * dwy[2];
		jx[2] = s_fx_z_dist1_u * dwz[0] + s_fx_z_dist1_v * dwz[1] + s_fx_z2__ * dwz[2];
		jx[3] = s_fx_z_dist1_u;
		jx[4] = s_fx_z_dist1_v;
		jx[5] = s_fx_z2__;
		jx[6] = distortedU * pinholeCamera.focalLengthX();

		const Scalar s_fy_z_dist2_u = zoom * pinholeCamera.focalLengthY() * scaleFactor * dist2_u_1_v;
		const Scalar s_fy_z_dist2_v = zoom * pinholeCamera.focalLengthY() * scaleFactor * dist2_v;
		const Scalar s_fy_z2__ = -zoom * pinholeCamera.focalLengthY() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist2_u_1_v + transformedObjectPoint.y() * dist2_v);

		jy[0] = s_fy_z_dist2_u * dwx[0] + s_fy_z_dist2_v * dwx[1] + s_fy_z2__ * dwx[2];
		jy[1] = s_fy_z_dist2_u * dwy[0] + s_fy_z_dist2_v * dwy[1] + s_fy_z2__ * dwy[2];
		jy[2] = s_fy_z_dist2_u * dwz[0] + s_fy_z_dist2_v * dwz[1] + s_fy_z2__ * dwz[2];
		jy[3] = s_fy_z_dist2_u;
		jy[4] = s_fy_z_dist2_v;
		jy[5] = s_fy_z2__;
		jy[6] = distortedV * pinholeCamera.focalLengthY();
	}
}

void Jacobian::calculatePoseZoomJacobianRodrigues2nx7(Scalar* jacobian, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const Scalar zoom, const Vector3* objectPoints, const size_t numberObjectPoints, const bool distortImagePoints)
{
	ocean_assert(zoom > Numeric::eps());

	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

	if (!distortImagePoints)
	{
		/**
		 * This functions uses three concatenated jacobian matrix for each object point to receive one final jacobian matrix with size 2x7 for each object point.
		 * The final jacobian is defined by: j = jDeh * jProj * jTrans;
		 *
		 * j (2x7) the final jacobian:
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz, dfx / ds |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz, dfy / ds |
		 *
		 * jDeh (2x3) the jacobian for the dehomogenization:                                       | U |
		 * | du / dU, du / dV, du / dW |   | 1 / W       0         -U / W^2 |   | u |   | U / W |  | V |
		 * | dv / dU, dv / dV, dv / dW | = |   0       1 / W       -V / W^2 |,  | v | = | V / W |, | W |
		 *
		 * jProj (3x4) the jacobian for the projection:
		 * | s Fx   0    mx   Fx X' |
		 * |   0   s Fy  my   Fy Y' |
		 * |   0    0     1    0    |      (X', Y', Z') = trans(X, Y, Z)
		 *
		 * jTrans (4x7) the jacobian for the transformation:
		 * | dfx / dwx, dfx / dwy, dfx / dwz,   1,   0,   0,   0 |
		 * | dfy / dwx, dfy / dwy, dfy / dwz,   0,   1,   0,   0 |
		 * | dfz / dwx, dfz / dwy, dfz / dwz,   0,   0,   1,   0 |
		 * |     0    ,     0    ,     0    ,   0,   0,   0,   1 |
		 *
		 * Final Jacobian:
		 * | s * Fx / W,        0  , mx / W - U / W^2, Fx * X' / W |   | dR(w) / dw, dft / dt, 0 |   | s * Fx / Z',       0    , -s * Fx * X' / Z'^2, Fx * X' / Z' |   | dR(w) / dw, dft / dt, 0 |
		 * |        0  , S * Fy / W, my / W - V / W^2, Fy * Y' / W | * |       0   ,     0   , 1 | = |      0     , S * Fy / Z', -s * Fy * Y' / Z'^2, Fy * Y' / Z' | * |       0   ,     0   , 1 |
		 *
		 */

		const HomogenousMatrix4 iFlippedTransformation(flippedCamera_P_world.transformation());

		for (size_t n = 0; n < numberObjectPoints; ++n)
		{
			const ObjectPoint& objectPoint = objectPoints[n];

			const Vector3 transformedObjectPoint(iFlippedTransformation * objectPoint);

			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
			const Scalar scaleFactor = 1 / transformedObjectPoint.z();

			const Scalar sfx_z = zoom * pinholeCamera.focalLengthX() * scaleFactor;
			const Scalar sfxx_z2 = -sfx_z * transformedObjectPoint.x() * scaleFactor;

			const Vector3 dwx(Rwx * objectPoint);
			const Vector3 dwy(Rwy * objectPoint);
			const Vector3 dwz(Rwz * objectPoint);

			*jacobian++ = sfx_z * dwx[0] + sfxx_z2 * dwx[2];
			*jacobian++ = sfx_z * dwy[0] + sfxx_z2 * dwy[2];
			*jacobian++ = sfx_z * dwz[0] + sfxx_z2 * dwz[2];

			*jacobian++ = sfx_z;
			*jacobian++ = 0;
			*jacobian++ = sfxx_z2;
			*jacobian++ = pinholeCamera.focalLengthX() * transformedObjectPoint.x() * scaleFactor;

			const Scalar sfy_z = zoom * pinholeCamera.focalLengthY() * scaleFactor;
			const Scalar sfyy_z2 = -sfy_z * transformedObjectPoint.y() * scaleFactor;

			*jacobian++ = sfy_z * dwx[1] + sfyy_z2 * dwx[2];
			*jacobian++ = sfy_z * dwy[1] + sfyy_z2 * dwy[2];
			*jacobian++ = sfy_z * dwz[1] + sfyy_z2 * dwz[2];

			*jacobian++ = 0;
			*jacobian++ = sfy_z;
			*jacobian++ = sfyy_z2;
			*jacobian++ = pinholeCamera.focalLengthY() * transformedObjectPoint.y() * scaleFactor;
		}
	}
	else
	{
		/**
		 * fFocal(fDist(fDeh(fTrans(fFlippedPose))))
		 *
		 * This functions uses three concatenated jacobian matrix for each object point to receive one final jacobian matrix with size 2x6 for each object point.
		 * The final jacobian is defined by: j = jFocal_Dist * jDeh * jTrans;
		 *
		 * j (2x7) the final jacobian:
		 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtx, dfx / dty, dfx / dtz, dfx / ds |
		 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtx, dfy / dty, dfy / dtz, dfy / ds |
		 *
		 * jFocal_Dist (2x3) the jacobian for the focal projection and distortion correction:
		 * | s * Fx * dfdistx / du, s * Fx * dfdistx / dv, u' * Fx |
		 * | s * Fy * dfdisty / du, s * Fy * dfdisty / dv, v' * Fy |, with fdist(u, v) = (u', v')
		 *
		 * jDeh (3x4) the jacobian for the dehomogenization:                                        | U |
		 * | 1 / W       0         -U / W^2,  0 |   | u |   | U / W |   | V |
		 * |   0       1 / W       -V / W^2,  0 |,  | v | = | V / W | = | W |
		 * |   0         0            0    ,  1 |
		 *
		 * jTrans (3x6) the jacobian for the transformation:
		 * | dfx / dwx, dfx / dwy, dfx / dwz,   1,   0,   0 |
		 * | dfy / dwx, dfy / dwy, dfy / dwz,   0,   1,   0 |
		 * | dfz / dwx, dfz / dwy, dfz / dwz,   0,   0,   1 |
		 */

		const Scalar k1 = pinholeCamera.radialDistortion().first;
		const Scalar k2 = pinholeCamera.radialDistortion().second;

		const Scalar p1 = pinholeCamera.tangentialDistortion().first;
		const Scalar p2 = pinholeCamera.tangentialDistortion().second;

		const HomogenousMatrix4 flippedCamera_T_world(flippedCamera_P_world.transformation());

		for (size_t n = 0; n < numberObjectPoints; ++n)
		{
			const ObjectPoint& objectPoint = objectPoints[n];

			const Vector3 transformedObjectPoint(flippedCamera_T_world * objectPoint);

			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
			const Scalar scaleFactor = Scalar(1) / transformedObjectPoint.z();

			const Scalar u = transformedObjectPoint.x() * scaleFactor;
			const Scalar v = transformedObjectPoint.y() * scaleFactor;

			const Scalar dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
			const Scalar dist2_u_1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v))));
			const Scalar dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);

			const Scalar s_fx_z_dist1_u = zoom * pinholeCamera.focalLengthX() * scaleFactor * dist1_u;
			const Scalar s_fx_z_dist1_v = zoom * pinholeCamera.focalLengthX() * scaleFactor * dist2_u_1_v;
			const Scalar s_fx_z2__ = -zoom * pinholeCamera.focalLengthX() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist2_u_1_v);

			const Scalar sqrRadius = u * u + v * v;
			const Scalar radialDistortionFactor = 1 + k1 * sqrRadius + k2 * sqrRadius * sqrRadius;
			const Scalar distortedU = u * radialDistortionFactor + p1 * 2 * u * v + p2 * (sqrRadius + 2 * u * u);
			const Scalar distortedV = v * radialDistortionFactor + p2 * 2 * u * v + p1 * (sqrRadius + 2 * v * v);

			const Vector3 dwx(Rwx * objectPoint);
			const Vector3 dwy(Rwy * objectPoint);
			const Vector3 dwz(Rwz * objectPoint);

			*jacobian++ = s_fx_z_dist1_u * dwx[0] + s_fx_z_dist1_v * dwx[1] + s_fx_z2__ * dwx[2];
			*jacobian++ = s_fx_z_dist1_u * dwy[0] + s_fx_z_dist1_v * dwy[1] + s_fx_z2__ * dwy[2];
			*jacobian++ = s_fx_z_dist1_u * dwz[0] + s_fx_z_dist1_v * dwz[1] + s_fx_z2__ * dwz[2];
			*jacobian++ = s_fx_z_dist1_u;
			*jacobian++ = s_fx_z_dist1_v;
			*jacobian++ = s_fx_z2__;
			*jacobian++ = distortedU * pinholeCamera.focalLengthX();

			const Scalar s_fy_z_dist2_u = zoom * pinholeCamera.focalLengthY() * scaleFactor * dist2_u_1_v;
			const Scalar s_fy_z_dist2_v = zoom * pinholeCamera.focalLengthY() * scaleFactor * dist2_v;
			const Scalar s_fy_z2__ = -zoom * pinholeCamera.focalLengthY() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist2_u_1_v + transformedObjectPoint.y() * dist2_v);

			*jacobian++ = s_fy_z_dist2_u * dwx[0] + s_fy_z_dist2_v * dwx[1] + s_fy_z2__ * dwx[2];
			*jacobian++ = s_fy_z_dist2_u * dwy[0] + s_fy_z_dist2_v * dwy[1] + s_fy_z2__ * dwy[2];
			*jacobian++ = s_fy_z_dist2_u * dwz[0] + s_fy_z_dist2_v * dwz[1] + s_fy_z2__ * dwz[2];
			*jacobian++ = s_fy_z_dist2_u;
			*jacobian++ = s_fy_z_dist2_v;
			*jacobian++ = s_fy_z2__;
			*jacobian++ = distortedV * pinholeCamera.focalLengthY();
		}
	}
}

void Jacobian::calculateObjectTransformation2x6(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& extrinsicIF, const Pose& objectPose, const Vector3& objectPoint, const SquareMatrix3& Rwx, const SquareMatrix3& Rwy, const SquareMatrix3& Rwz)
{
	ocean_assert(jx && jy && pinholeCamera.isValid() && extrinsicIF.isValid());

	// create one transformation matrix covering the entire pipeline (transformation and then projection)
	SquareMatrix4 cameraMatrix(pinholeCamera.intrinsic());
	cameraMatrix[15] = 1;

	const HomogenousMatrix4 transformationMatrix(cameraMatrix * SquareMatrix4(extrinsicIF) * SquareMatrix4(objectPose.transformation()));

	const StaticMatrix<Scalar, 3, 3> jacobianProjection(pinholeCamera.intrinsic().data(), false);

	StaticMatrix<Scalar, 3, 6> jacobianObject;
	const Vector3 dwx(Rwx * objectPoint);
	const Vector3 dwy(Rwy * objectPoint);
	const Vector3 dwz(Rwz * objectPoint);

	jacobianObject(0, 0) = dwx[0];
	jacobianObject(1, 0) = dwx[1];
	jacobianObject(2, 0) = dwx[2];

	jacobianObject(0, 1) = dwy[0];
	jacobianObject(1, 1) = dwy[1];
	jacobianObject(2, 1) = dwy[2];

	jacobianObject(0, 2) = dwz[0];
	jacobianObject(1, 2) = dwz[1];
	jacobianObject(2, 2) = dwz[2];

	jacobianObject(0, 3) = 1;
	jacobianObject(1, 3) = 0;
	jacobianObject(2, 3) = 0;

	jacobianObject(0, 4) = 0;
	jacobianObject(1, 4) = 1;
	jacobianObject(2, 4) = 0;

	jacobianObject(0, 5) = 0;
	jacobianObject(1, 5) = 0;
	jacobianObject(2, 5) = 1;

	StaticMatrix<Scalar, 3, 3> jacobianPose;
	jacobianPose(0, 0) = extrinsicIF(0, 0);
	jacobianPose(1, 0) = extrinsicIF(1, 0);
	jacobianPose(2, 0) = extrinsicIF(2, 0);

	jacobianPose(0, 1) = extrinsicIF(0, 1);
	jacobianPose(1, 1) = extrinsicIF(1, 1);
	jacobianPose(2, 1) = extrinsicIF(2, 1);

	jacobianPose(0, 2) = extrinsicIF(0, 2);
	jacobianPose(1, 2) = extrinsicIF(1, 2);
	jacobianPose(2, 2) = extrinsicIF(2, 2);

	const Vector3 projectedObjectPoint(transformationMatrix * objectPoint);
	const Scalar factor = Scalar(1) / projectedObjectPoint.z();

	StaticMatrix<Scalar, 2, 3> jacobianDehomogenous;
	jacobianDehomogenous(0, 0) = factor;
	jacobianDehomogenous(1, 0) = 0;
	jacobianDehomogenous(0, 1) = 0;
	jacobianDehomogenous(1, 1) = factor;
	jacobianDehomogenous(0, 2) = -projectedObjectPoint.x() * factor * factor;
	jacobianDehomogenous(1, 2) = -projectedObjectPoint.y() * factor * factor;

	StaticMatrix<Scalar, 2, 6> finalJacobian(((jacobianDehomogenous * jacobianProjection) * jacobianPose) * jacobianObject);

	jx[0] = finalJacobian(0, 0);
	jx[1] = finalJacobian(0, 1);
	jx[2] = finalJacobian(0, 2);
	jx[3] = finalJacobian(0, 3);
	jx[4] = finalJacobian(0, 4);
	jx[5] = finalJacobian(0, 5);

	jy[0] = finalJacobian(1, 0);
	jy[1] = finalJacobian(1, 1);
	jy[2] = finalJacobian(1, 2);
	jy[3] = finalJacobian(1, 3);
	jy[4] = finalJacobian(1, 4);
	jy[5] = finalJacobian(1, 5);
}

void Jacobian::calculateObjectTransformation2nx6(Scalar* jacobian, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& extrinsicIF, const Pose& objectPose, const Vector3* objectPoints, const size_t numberObjectPoints)
{
	ocean_assert(jacobian && pinholeCamera.isValid() && extrinsicIF.isValid());
	ocean_assert(!pinholeCamera.hasDistortionParameters());

	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(objectPose.rx(), objectPose.ry(), objectPose.rz())), Rwx, Rwy, Rwz);

	// create one transformation matrix covering the entire pipeline (transformation and then projection)
	SquareMatrix4 cameraMatrix(pinholeCamera.intrinsic());
	cameraMatrix[15] = 1;

	const HomogenousMatrix4 transformationMatrix(cameraMatrix * SquareMatrix4(extrinsicIF) * SquareMatrix4(objectPose.transformation()));

	StaticMatrix<Scalar, 3, 3> jacobianPose;
	jacobianPose(0, 0) = extrinsicIF(0, 0);
	jacobianPose(1, 0) = extrinsicIF(1, 0);
	jacobianPose(2, 0) = extrinsicIF(2, 0);

	jacobianPose(0, 1) = extrinsicIF(0, 1);
	jacobianPose(1, 1) = extrinsicIF(1, 1);
	jacobianPose(2, 1) = extrinsicIF(2, 1);

	jacobianPose(0, 2) = extrinsicIF(0, 2);
	jacobianPose(1, 2) = extrinsicIF(1, 2);
	jacobianPose(2, 2) = extrinsicIF(2, 2);

	const StaticMatrix<Scalar, 3, 3> jacobianProjection(pinholeCamera.intrinsic().data(), false);

	for (size_t n = 0; n < numberObjectPoints; ++n)
	{
		const Vector3& objectPoint = objectPoints[n];

		StaticMatrix<Scalar, 3, 6> jacobianObject;
		const Vector3 dwx(Rwx * objectPoint);
		const Vector3 dwy(Rwy * objectPoint);
		const Vector3 dwz(Rwz * objectPoint);

		jacobianObject(0, 0) = dwx[0];
		jacobianObject(1, 0) = dwx[1];
		jacobianObject(2, 0) = dwx[2];

		jacobianObject(0, 1) = dwy[0];
		jacobianObject(1, 1) = dwy[1];
		jacobianObject(2, 1) = dwy[2];

		jacobianObject(0, 2) = dwz[0];
		jacobianObject(1, 2) = dwz[1];
		jacobianObject(2, 2) = dwz[2];

		jacobianObject(0, 3) = 1;
		jacobianObject(1, 3) = 0;
		jacobianObject(2, 3) = 0;

		jacobianObject(0, 4) = 0;
		jacobianObject(1, 4) = 1;
		jacobianObject(2, 4) = 0;

		jacobianObject(0, 5) = 0;
		jacobianObject(1, 5) = 0;
		jacobianObject(2, 5) = 1;

		const Vector3 projectedObjectPoint(transformationMatrix * objectPoint);
		const Scalar factor = Scalar(1) / projectedObjectPoint.z();

		StaticMatrix<Scalar, 2, 3> jacobianDehomogenous;
		jacobianDehomogenous(0, 0) = factor;
		jacobianDehomogenous(1, 0) = 0;
		jacobianDehomogenous(0, 1) = 0;
		jacobianDehomogenous(1, 1) = factor;
		jacobianDehomogenous(0, 2) = -projectedObjectPoint.x() * factor * factor;
		jacobianDehomogenous(1, 2) = -projectedObjectPoint.y() * factor * factor;

		StaticMatrix<Scalar, 2, 6> finalJacobian(((jacobianDehomogenous * jacobianProjection) * jacobianPose) * jacobianObject);

		*jacobian++ = finalJacobian(0, 0);
		*jacobian++ = finalJacobian(0, 1);
		*jacobian++ = finalJacobian(0, 2);
		*jacobian++ = finalJacobian(0, 3);
		*jacobian++ = finalJacobian(0, 4);
		*jacobian++ = finalJacobian(0, 5);

		*jacobian++ = finalJacobian(1, 0);
		*jacobian++ = finalJacobian(1, 1);
		*jacobian++ = finalJacobian(1, 2);
		*jacobian++ = finalJacobian(1, 3);
		*jacobian++ = finalJacobian(1, 4);
		*jacobian++ = finalJacobian(1, 5);
	}
}

void Jacobian::calculateObjectTransformation2nx6(Scalar* jacobian, const FisheyeCamera& fisheyeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Pose& world_T_object, const Vector3* objectPoints, const size_t numberObjectPoints)
{
	ocean_assert(jacobian && fisheyeCamera.isValid() && flippedCamera_T_world.isValid());

	const HomogenousMatrix4 flippedCamera_T_object = flippedCamera_T_world * world_T_object.transformation();

	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(world_T_object.rx(), world_T_object.ry(), world_T_object.rz())), Rwx, Rwy, Rwz);

	Scalar jDistX[2];
	Scalar jDistY[2];

	const Scalar& fx = fisheyeCamera.focalLengthX();
	const Scalar& fy = fisheyeCamera.focalLengthY();

	for (size_t n = 0; n < numberObjectPoints; ++n)
	{
		const Vector3& objectPoint = objectPoints[n];

		const Vector3 flippedCamera_objectPoint = flippedCamera_T_object * objectPoint;
		const Scalar& u = flippedCamera_objectPoint.x();
		const Scalar& v = flippedCamera_objectPoint.y();
		const Scalar& w = flippedCamera_objectPoint.z();

		ocean_assert(Numeric::isNotEqualEps(w));
		const Scalar invW = Scalar(1) / w;

		const Scalar u_invW = u * invW;
		const Scalar v_invW = v * invW;

		calculateFisheyeDistortNormalized2x2(jDistX, jDistY, u_invW, v_invW, fisheyeCamera.radialDistortion(), fisheyeCamera.tangentialDistortion());

		const Scalar fx_jDistXx_invW = fx * jDistX[0] * invW;
		const Scalar fy_jDistYx_invW = fy * jDistY[0] * invW;

		const Scalar fx_jDistXy_invW = fx * jDistX[1] * invW;
		const Scalar fy_jDistYy_invW = fy * jDistY[1] * invW;

		const Scalar u_fx_jDistXx__ = u_invW * fx_jDistXx_invW + v_invW * fx_jDistXy_invW;
		const Scalar u_fy_jDistYx__ = u_invW * fy_jDistYx_invW + v_invW * fy_jDistYy_invW;

		const Scalar jFocalPoseXx = fx_jDistXx_invW * flippedCamera_T_world[0] + fx_jDistXy_invW * flippedCamera_T_world[1] - u_fx_jDistXx__ * flippedCamera_T_world[2];
		const Scalar jFocalPoseXy = fx_jDistXx_invW * flippedCamera_T_world[4] + fx_jDistXy_invW * flippedCamera_T_world[5] - u_fx_jDistXx__ * flippedCamera_T_world[6];
		const Scalar jFocalPoseXz = fx_jDistXx_invW * flippedCamera_T_world[8] + fx_jDistXy_invW * flippedCamera_T_world[9] - u_fx_jDistXx__ * flippedCamera_T_world[10];

		const Scalar jFocalPoseYx = fy_jDistYx_invW * flippedCamera_T_world[0] + fy_jDistYy_invW * flippedCamera_T_world[1] - u_fy_jDistYx__ * flippedCamera_T_world[2];
		const Scalar jFocalPoseYy = fy_jDistYx_invW * flippedCamera_T_world[4] + fy_jDistYy_invW * flippedCamera_T_world[5] - u_fy_jDistYx__ * flippedCamera_T_world[6];
		const Scalar jFocalPoseYz = fy_jDistYx_invW * flippedCamera_T_world[8] + fy_jDistYy_invW * flippedCamera_T_world[9] - u_fy_jDistYx__ * flippedCamera_T_world[10];

		const Vector3 dwx(Rwx * objectPoint);
		const Vector3 dwy(Rwy * objectPoint);
		const Vector3 dwz(Rwz * objectPoint);

		*jacobian++ = jFocalPoseXx * dwx.x() + jFocalPoseXy * dwx.y() + jFocalPoseXz * dwx.z();
		*jacobian++ = jFocalPoseXx * dwy.x() + jFocalPoseXy * dwy.y() + jFocalPoseXz * dwy.z();
		*jacobian++ = jFocalPoseXx * dwz.x() + jFocalPoseXy * dwz.y() + jFocalPoseXz * dwz.z();
		*jacobian++ = jFocalPoseXx;
		*jacobian++ = jFocalPoseXy;
		*jacobian++ = jFocalPoseXz;

		*jacobian++ = jFocalPoseYx * dwx.x() + jFocalPoseYy * dwx.y() + jFocalPoseYz * dwx.z();
		*jacobian++ = jFocalPoseYx * dwy.x() + jFocalPoseYy * dwy.y() + jFocalPoseYz * dwy.z();
		*jacobian++ = jFocalPoseYx * dwz.x() + jFocalPoseYy * dwz.y() + jFocalPoseYz * dwz.z();
		*jacobian++ = jFocalPoseYx;
		*jacobian++ = jFocalPoseYy;
		*jacobian++ = jFocalPoseYz;
	}

#ifdef SLOWER_IMPLEMENTATION

	StaticMatrix<Scalar, 3, 3> jacobianCameraPose;
	jacobianCameraPose(0, 0) = flippedCamera_T_world(0, 0);
	jacobianCameraPose(1, 0) = flippedCamera_T_world(1, 0);
	jacobianCameraPose(2, 0) = flippedCamera_T_world(2, 0);

	jacobianCameraPose(0, 1) = flippedCamera_T_world(0, 1);
	jacobianCameraPose(1, 1) = flippedCamera_T_world(1, 1);
	jacobianCameraPose(2, 1) = flippedCamera_T_world(2, 1);

	jacobianCameraPose(0, 2) = flippedCamera_T_world(0, 2);
	jacobianCameraPose(1, 2) = flippedCamera_T_world(1, 2);
	jacobianCameraPose(2, 2) = flippedCamera_T_world(2, 2);

	StaticMatrix<Scalar, 2, 2> jacobianFocal(false);
	jacobianFocal(0, 0) = fisheyeCamera.focalLengthX();
	jacobianFocal(1, 1) = fisheyeCamera.focalLengthY();

	for (size_t n = 0; n < numberObjectPoints; ++n)
	{
		const Vector3& objectPoint = objectPoints[n];

		StaticMatrix<Scalar, 3, 6> jacobianObject;
		const Vector3 dwx(Rwx * objectPoint);
		const Vector3 dwy(Rwy * objectPoint);
		const Vector3 dwz(Rwz * objectPoint);

		jacobianObject(0, 0) = dwx[0];
		jacobianObject(1, 0) = dwx[1];
		jacobianObject(2, 0) = dwx[2];

		jacobianObject(0, 1) = dwy[0];
		jacobianObject(1, 1) = dwy[1];
		jacobianObject(2, 1) = dwy[2];

		jacobianObject(0, 2) = dwz[0];
		jacobianObject(1, 2) = dwz[1];
		jacobianObject(2, 2) = dwz[2];

		jacobianObject(0, 3) = 1;
		jacobianObject(1, 3) = 0;
		jacobianObject(2, 3) = 0;

		jacobianObject(0, 4) = 0;
		jacobianObject(1, 4) = 1;
		jacobianObject(2, 4) = 0;

		jacobianObject(0, 5) = 0;
		jacobianObject(1, 5) = 0;
		jacobianObject(2, 5) = 1;

		const Vector3 flippedCamera_objectPoint = flippedCamera_T_object * objectPoint;

		ocean_assert(Numeric::isNotEqualEps(flippedCamera_objectPoint.z()));
		const Scalar factor = Scalar(1) / flippedCamera_objectPoint.z();

		StaticMatrix<Scalar, 2, 3> jacobianDehomogenous;
		jacobianDehomogenous(0, 0) = factor;
		jacobianDehomogenous(1, 0) = 0;
		jacobianDehomogenous(0, 1) = 0;
		jacobianDehomogenous(1, 1) = factor;
		jacobianDehomogenous(0, 2) = -flippedCamera_objectPoint.x() * factor * factor;
		jacobianDehomogenous(1, 2) = -flippedCamera_objectPoint.y() * factor * factor;

		StaticMatrix<Scalar, 2, 2> jacobianDistortion;
		calculateFisheyeDistortNormalized2x2(jacobianDistortion.row(0), jacobianDistortion.row(1), flippedCamera_objectPoint.x() * factor, flippedCamera_objectPoint.y() * factor, fisheyeCamera.radialDistortion(), fisheyeCamera.tangentialDistortion());

		StaticMatrix<Scalar, 2, 6> finalJacobian((((jacobianFocal * jacobianDistortion) * jacobianDehomogenous) * jacobianCameraPose) * jacobianObject);

		*jacobian++ = finalJacobian(0, 0);
		*jacobian++ = finalJacobian(0, 1);
		*jacobian++ = finalJacobian(0, 2);
		*jacobian++ = finalJacobian(0, 3);
		*jacobian++ = finalJacobian(0, 4);
		*jacobian++ = finalJacobian(0, 5);

		*jacobian++ = finalJacobian(1, 0);
		*jacobian++ = finalJacobian(1, 1);
		*jacobian++ = finalJacobian(1, 2);
		*jacobian++ = finalJacobian(1, 3);
		*jacobian++ = finalJacobian(1, 4);
		*jacobian++ = finalJacobian(1, 5);
	}
#endif // SLOWER_IMPLEMENTATION
}

void Jacobian::calculateOrientationCameraJacobianRodrigues2x11(Scalar* jacobianX, Scalar* jacobianY, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const Vector3& objectPoint)
{
	ocean_assert(jacobianX && jacobianY && pinholeCamera.isValid());

	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

	const Scalar k1 = pinholeCamera.radialDistortion().first;
	const Scalar k2 = pinholeCamera.radialDistortion().second;
	const Scalar p1 = pinholeCamera.tangentialDistortion().first;
	const Scalar p2 = pinholeCamera.tangentialDistortion().second;

	const Vector3 transformedObjectPoint = flippedCamera_P_world.transformation() * objectPoint;

	ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
	const Scalar scaleFactor = Scalar(1) / transformedObjectPoint.z();

	const Scalar u = transformedObjectPoint.x() * scaleFactor;
	const Scalar v = transformedObjectPoint.y() * scaleFactor;

	const Scalar dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
	const Scalar dist2_u_1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v))));
	const Scalar dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);

	const Scalar uv2 = u * u + v * v;

	const Scalar dist1_k1 = u * uv2;
	const Scalar dist1_k2 = u * uv2 * uv2;
	const Scalar dist1_p1_2_p2 = 2 * u * v;
	const Scalar dist1_p2 = 3 * u * u + v * v;

	const Scalar dist2_k1 = v * uv2;
	const Scalar dist2_k2 = v * uv2 * uv2;
	const Scalar dist2_p1 = u * u + 3 * v * v;

	const Scalar intermediate1_1 = pinholeCamera.focalLengthX() * scaleFactor * dist1_u;
	const Scalar intermediate1_2 = pinholeCamera.focalLengthX() * scaleFactor * dist2_u_1_v;
	const Scalar intermediate1_3 = -pinholeCamera.focalLengthX() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist2_u_1_v);

	const Scalar intermediate2_1 = pinholeCamera.focalLengthY() * scaleFactor * dist2_u_1_v;
	const Scalar intermediate2_2 = pinholeCamera.focalLengthY() * scaleFactor * dist2_v;
	const Scalar intermediate2_3 = -pinholeCamera.focalLengthY() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist2_u_1_v + transformedObjectPoint.y() * dist2_v);

	const Vector3 dwx(Rwx * objectPoint);
	const Vector3 dwy(Rwy * objectPoint);
	const Vector3 dwz(Rwz * objectPoint);

	*jacobianX++ = intermediate1_1 * dwx[0] + intermediate1_2 * dwx[1] + intermediate1_3 * dwx[2];
	*jacobianX++ = intermediate1_1 * dwy[0] + intermediate1_2 * dwy[1] + intermediate1_3 * dwy[2];
	*jacobianX++ = intermediate1_1 * dwz[0] + intermediate1_2 * dwz[1] + intermediate1_3 * dwz[2];

	*jacobianY++ = intermediate2_1 * dwx[0] + intermediate2_2 * dwx[1] + intermediate2_3 * dwx[2];
	*jacobianY++ = intermediate2_1 * dwy[0] + intermediate2_2 * dwy[1] + intermediate2_3 * dwy[2];
	*jacobianY++ = intermediate2_1 * dwz[0] + intermediate2_2 * dwz[1] + intermediate2_3 * dwz[2];

	*jacobianX++ = pinholeCamera.focalLengthX() * dist1_k1;
	*jacobianX++ = pinholeCamera.focalLengthX() * dist1_k2;
	*jacobianX++ = pinholeCamera.focalLengthX() * dist1_p1_2_p2;
	*jacobianX++ = pinholeCamera.focalLengthX() * dist1_p2;
	*jacobianX++ = u + u * (k1 * uv2 + k2 * uv2 * uv2) + p1 * 2 * u * v + p2 * (uv2 + 2 * u * u);
	*jacobianX++ = 0;
	*jacobianX++ = 1;
	*jacobianX++ = 0;

	*jacobianY++ = pinholeCamera.focalLengthY() * dist2_k1;
	*jacobianY++ = pinholeCamera.focalLengthY() * dist2_k2;
	*jacobianY++ = pinholeCamera.focalLengthY() * dist2_p1;
	*jacobianY++ = pinholeCamera.focalLengthY() * dist1_p1_2_p2;
	*jacobianY++ = 0;
	*jacobianY++ = v + v * (k1 * uv2 + k2 * uv2 * uv2) + 2 * p2 * u * v + p1 * (uv2 + 2 * v * v);
	*jacobianY++ = 0;
	*jacobianY++ = 1;
}

void Jacobian::calculateOrientationCameraJacobianRodrigues2nx11(Scalar* jacobian, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const ConstIndexedAccessor<Vector3>& objectPoints)
{
	ocean_assert(jacobian && pinholeCamera.isValid());

	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

	const HomogenousMatrix4 iTransformation(flippedCamera_P_world.transformation());

	const Scalar k1 = pinholeCamera.radialDistortion().first;
	const Scalar k2 = pinholeCamera.radialDistortion().second;
	const Scalar p1 = pinholeCamera.tangentialDistortion().first;
	const Scalar p2 = pinholeCamera.tangentialDistortion().second;

	for (size_t n = 0u; n < objectPoints.size(); ++n)
	{
		const Vector3& objectPoint = objectPoints[n];

		const Vector3 transformedObjectPoint = iTransformation * objectPoint;

		ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
		const Scalar scaleFactor = Scalar(1) / transformedObjectPoint.z();

		const Scalar u = transformedObjectPoint.x() * scaleFactor;
		const Scalar v = transformedObjectPoint.y() * scaleFactor;

		const Scalar dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
		const Scalar dist2_u_1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v))));
		const Scalar dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);

		const Scalar uv2 = u * u + v * v;

		const Scalar dist1_k1 = u * uv2;
		const Scalar dist1_k2 = u * uv2 * uv2;
		const Scalar dist1_p1_2_p2 = 2 * u * v;
		const Scalar dist1_p2 = 3 * u * u + v * v;

		const Scalar dist2_k1 = v * uv2;
		const Scalar dist2_k2 = v * uv2 * uv2;
		const Scalar dist2_p1 = u * u + 3 * v * v;

		const Scalar intermediate1_1 = pinholeCamera.focalLengthX() * scaleFactor * dist1_u;
		const Scalar intermediate1_2 = pinholeCamera.focalLengthX() * scaleFactor * dist2_u_1_v;
		const Scalar intermediate1_3 = -pinholeCamera.focalLengthX() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist2_u_1_v);

		const Scalar intermediate2_1 = pinholeCamera.focalLengthY() * scaleFactor * dist2_u_1_v;
		const Scalar intermediate2_2 = pinholeCamera.focalLengthY() * scaleFactor * dist2_v;
		const Scalar intermediate2_3 = -pinholeCamera.focalLengthY() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist2_u_1_v + transformedObjectPoint.y() * dist2_v);

		const Vector3 dwx(Rwx * objectPoint);
		const Vector3 dwy(Rwy * objectPoint);
		const Vector3 dwz(Rwz * objectPoint);

		// Scalar* jacobianX = jacobian;
		Scalar* jacobianY = jacobian + 11;

		*jacobian++ = intermediate1_1 * dwx[0] + intermediate1_2 * dwx[1] + intermediate1_3 * dwx[2];
		*jacobian++ = intermediate1_1 * dwy[0] + intermediate1_2 * dwy[1] + intermediate1_3 * dwy[2];
		*jacobian++ = intermediate1_1 * dwz[0] + intermediate1_2 * dwz[1] + intermediate1_3 * dwz[2];

		*jacobianY++ = intermediate2_1 * dwx[0] + intermediate2_2 * dwx[1] + intermediate2_3 * dwx[2];
		*jacobianY++ = intermediate2_1 * dwy[0] + intermediate2_2 * dwy[1] + intermediate2_3 * dwy[2];
		*jacobianY++ = intermediate2_1 * dwz[0] + intermediate2_2 * dwz[1] + intermediate2_3 * dwz[2];

		*jacobian++ = pinholeCamera.focalLengthX() * dist1_k1;
		*jacobian++ = pinholeCamera.focalLengthX() * dist1_k2;
		*jacobian++ = pinholeCamera.focalLengthX() * dist1_p1_2_p2;
		*jacobian++ = pinholeCamera.focalLengthX() * dist1_p2;
		*jacobian++ = u + u * (k1 * uv2 + k2 * uv2 * uv2) + p1 * 2 * u * v + p2 * (uv2 + 2 * u * u);
		*jacobian++ = 0;
		*jacobian++ = 1;
		*jacobian++ = 0;

		*jacobianY++ = pinholeCamera.focalLengthY() * dist2_k1;
		*jacobianY++ = pinholeCamera.focalLengthY() * dist2_k2;
		*jacobianY++ = pinholeCamera.focalLengthY() * dist2_p1;
		*jacobianY++ = pinholeCamera.focalLengthY() * dist1_p1_2_p2;
		*jacobianY++ = 0;
		*jacobianY++ = v + v * (k1 * uv2 + k2 * uv2 * uv2) + 2 * p2 * u * v + p1 * (uv2 + 2 * v * v);
		*jacobianY++ = 0;
		*jacobianY++ = 1;

		jacobian += 11;
	}
}

void Jacobian::calculatePoseJacobianRodrigues2x5(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const ExponentialMap& rotation, const Vector2& translation, const Vector3& objectPoint)
{
	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(rotation, Rwx, Rwy, Rwz);

	/**
	 * This functions uses three concatenated jacobian matrix for each object point to receive one final jacobian matrix with size 2x5 for each object point.
	 * The final jacobian is defined by: j = jDeh * jProj * jTrans;
	 *
	 * j (2x5) the final jacobian:
	 * | dfx / dwx, dfx / dwy, dfx / dwz, dfx / dtu, dfx / dtv |
	 * | dfy / dwx, dfy / dwy, dfy / dwz, dfy / dtu, dfy / dtv |
	 *
	 * jDeh (2x3) the jacobian for the dehomogenization:                                        | U |
	 * | du / dU, du / dV, du / dW |   | 1 / W       0         -U / W^2 |   | u |   | U / W |   | V |
	 * | dv / dU, dv / dV, dv / dW | = |   0       1 / W       -V / W^2 |,  | v | = | V / W | = | W |
	 *
	 * jProj (3x3) the jacobian for the projection:
	 * | Fx   0  mx |
	 * | 0   Fy  my |
	 * | 0    0   1 |
	 *
	 */

	/**
	 * | cos(alpha) * sin(beta) |
	 * | sin(alpha) * sin(beta) |
	 * |        cos(beta)       |
	 */

	 /*
	 * jTrans (3x6) the jacobian for the transformation:
	 * | dfx / dwx, dfx / dwy, dfx / dwz,      -sin(alpha) * sin(beta) ,      cos(alpha) * cos(beta)     |
	 * | dfy / dwx, dfy / dwy, dfy / dwz,                  0           ,            -sin(beta)           |
	 * | dfz / dwx, dfz / dwy, dfz / dwz,       cos(alpha) * sin(beta) ,      sin(alpha) * cos(beta)     |
	 */

	// create one transformation matrix covering the entire pipeline (transformation and then projection)
	SquareMatrix4 cameraMatrix(pinholeCamera.intrinsic());
	cameraMatrix[15] = 1;

	const Scalar alpha = translation[0];
	const Scalar beta = translation[1];

	const Vector3 translationVector(Numeric::cos(alpha) * Numeric::sin(beta), Numeric::cos(beta), Numeric::sin(alpha) * Numeric::sin(beta));
	Numeric::isEqual(translationVector.length(), 1);

	const HomogenousMatrix4 transformationMatrix(cameraMatrix * SquareMatrix4(HomogenousMatrix4(translationVector, rotation.quaternion())));

	const StaticMatrix<Scalar, 3, 3> jacobianProjection(pinholeCamera.intrinsic().data(), false);

	StaticMatrix<Scalar, 3, 5> jacobianTransformation;
	const Vector3 dwx(Rwx * objectPoint);
	const Vector3 dwy(Rwy * objectPoint);
	const Vector3 dwz(Rwz * objectPoint);

	jacobianTransformation(0, 0) = dwx[0];
	jacobianTransformation(1, 0) = dwx[1];
	jacobianTransformation(2, 0) = dwx[2];

	jacobianTransformation(0, 1) = dwy[0];
	jacobianTransformation(1, 1) = dwy[1];
	jacobianTransformation(2, 1) = dwy[2];

	jacobianTransformation(0, 2) = dwz[0];
	jacobianTransformation(1, 2) = dwz[1];
	jacobianTransformation(2, 2) = dwz[2];

	jacobianTransformation(0, 3) = -Numeric::sin(alpha) * Numeric::sin(beta);
	jacobianTransformation(1, 3) = 0;
	jacobianTransformation(2, 3) = Numeric::cos(alpha) * Numeric::sin(beta);

	jacobianTransformation(0, 4) = Numeric::cos(alpha) * Numeric::cos(beta);
	jacobianTransformation(1, 4) = -Numeric::sin(beta);
	jacobianTransformation(2, 4) = Numeric::sin(alpha) * Numeric::cos(beta);

	const Vector3 projectedObjectPoint(transformationMatrix * objectPoint);
	const Scalar factor = 1 / projectedObjectPoint.z();

	StaticMatrix<Scalar, 2, 3> jacobianDehomogenous;
	jacobianDehomogenous(0, 0) = factor;
	jacobianDehomogenous(1, 0) = 0;
	jacobianDehomogenous(0, 1) = 0;
	jacobianDehomogenous(1, 1) = factor;
	jacobianDehomogenous(0, 2) = -projectedObjectPoint.x() * factor * factor;
	jacobianDehomogenous(1, 2) = -projectedObjectPoint.y() * factor * factor;

	StaticMatrix<Scalar, 2, 5> finalJacobian((jacobianDehomogenous * jacobianProjection) * jacobianTransformation);

	jx[0] = finalJacobian(0, 0);
	jx[1] = finalJacobian(0, 1);
	jx[2] = finalJacobian(0, 2);
	jx[3] = finalJacobian(0, 3);
	jx[4] = finalJacobian(0, 4);

	jy[0] = finalJacobian(1, 0);
	jy[1] = finalJacobian(1, 1);
	jy[2] = finalJacobian(1, 2);
	jy[3] = finalJacobian(1, 3);
	jy[4] = finalJacobian(1, 4);
}

void Jacobian::calculatePoseJacobianRodrigues2nx5(Scalar* jacobian, const PinholeCamera& pinholeCamera, const ExponentialMap& rotation, const Vector2& translation, const Vector3* objectPoints, const size_t numberObjectPoints)
{
	for (size_t n = 0; n < numberObjectPoints; ++n)
		calculatePoseJacobianRodrigues2x5(jacobian + (2 * n + 0) * 5, jacobian + (2 * n + 1) * 5, pinholeCamera, rotation, translation, objectPoints[n]);
}

void Jacobian::calculatePointJacobian2x3(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_P_world, const Vector3& objectPoint, const bool distortImagePoint)
{
	ocean_assert(jx && jy);
	ocean_assert(pinholeCamera && flippedCamera_P_world.isValid());

	if (!distortImagePoint)
	{
		const Vector3 transformedObjectPoint(flippedCamera_P_world * objectPoint);

		ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
		const Scalar factor = Scalar(1) / transformedObjectPoint.z();

		const Scalar m00 = factor * pinholeCamera.focalLengthX();
		const Scalar m11 = factor * pinholeCamera.focalLengthY();
		const Scalar m02 = -pinholeCamera.focalLengthX() * transformedObjectPoint.x() * factor * factor;
		const Scalar m12 = -pinholeCamera.focalLengthY() * transformedObjectPoint.y() * factor * factor;

		jx[0] = m00 * flippedCamera_P_world[0] + m02 * flippedCamera_P_world[2];
		jx[1] = m00 * flippedCamera_P_world[4] + m02 * flippedCamera_P_world[6];
		jx[2] = m00 * flippedCamera_P_world[8] + m02 * flippedCamera_P_world[10];

		jy[0] = m11 * flippedCamera_P_world[1] + m12 * flippedCamera_P_world[2];
		jy[1] = m11 * flippedCamera_P_world[5] + m12 * flippedCamera_P_world[6];
		jy[2] = m11 * flippedCamera_P_world[9] + m12 * flippedCamera_P_world[10];
	}
	else
	{
		const Scalar k1 = pinholeCamera.radialDistortion().first;
		const Scalar k2 = pinholeCamera.radialDistortion().second;

		const Scalar p1 = pinholeCamera.tangentialDistortion().first;
		const Scalar p2 = pinholeCamera.tangentialDistortion().second;

		const Scalar Fx = pinholeCamera.focalLengthX();
		const Scalar Fy = pinholeCamera.focalLengthY();

		const Vector3 transformedObjectPoint(flippedCamera_P_world * objectPoint);

		ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
		const Scalar scaleFactor = 1 / transformedObjectPoint.z();

		const Scalar u = transformedObjectPoint.x() * scaleFactor;
		const Scalar v = transformedObjectPoint.y() * scaleFactor;

		const Scalar dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
		const Scalar dist2_u_1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v)))); // dist2_u == dist1_v
		const Scalar dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);

		const Scalar fx_w_dist1_u = Fx * scaleFactor * dist1_u;
		const Scalar fx_w_dist1_v = Fx * scaleFactor * dist2_u_1_v;
		const Scalar fx_w2__ = -Fx * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist2_u_1_v);

		jx[0] = fx_w_dist1_u * flippedCamera_P_world[0] + fx_w_dist1_v * flippedCamera_P_world[1] + fx_w2__ * flippedCamera_P_world[2];
		jx[1] = fx_w_dist1_u * flippedCamera_P_world[4] + fx_w_dist1_v * flippedCamera_P_world[5] + fx_w2__ * flippedCamera_P_world[6];
		jx[2] = fx_w_dist1_u * flippedCamera_P_world[8] + fx_w_dist1_v * flippedCamera_P_world[9] + fx_w2__ * flippedCamera_P_world[10];

		const Scalar fy_w_dist2_u = Fy * scaleFactor * dist2_u_1_v;
		const Scalar fy_w_dist2_v = Fy * scaleFactor * dist2_v;
		const Scalar fy_w2__ = -Fy * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist2_u_1_v + transformedObjectPoint.y() * dist2_v);

		jy[0] = fy_w_dist2_u * flippedCamera_P_world[0] + fy_w_dist2_v * flippedCamera_P_world[1] + fy_w2__ * flippedCamera_P_world[2];
		jy[1] = fy_w_dist2_u * flippedCamera_P_world[4] + fy_w_dist2_v * flippedCamera_P_world[5] + fy_w2__ * flippedCamera_P_world[6];
		jy[2] = fy_w_dist2_u * flippedCamera_P_world[8] + fy_w_dist2_v * flippedCamera_P_world[9] + fy_w2__ * flippedCamera_P_world[10];
	}
}

void Jacobian::calculatePointJacobian2x3(Scalar* jx, Scalar* jy, const FisheyeCamera& fisheyeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& worldObjectPoint)
{
	ocean_assert(jx != nullptr && jy != nullptr);
	ocean_assert(fisheyeCamera.isValid());
	ocean_assert(flippedCamera_T_world.isValid());

	// | Fx   0  |   | df_distx_u   df_distx_v |   | 1/W    0   -U/W^2 |
	// | 0    Fy | * | df_disty_u   df_disty_v | * |  0    1/W  -V/W^2 |  * R

	Scalar jDistX[2];
	Scalar jDistY[2];

	const Scalar& fx = fisheyeCamera.focalLengthX();
	const Scalar& fy = fisheyeCamera.focalLengthY();

	const Vector3 flippedCamera_objectPoint = flippedCamera_T_world * worldObjectPoint;
	const Scalar& u = flippedCamera_objectPoint.x();
	const Scalar& v = flippedCamera_objectPoint.y();
	const Scalar& w = flippedCamera_objectPoint.z();

	ocean_assert(Numeric::isNotEqualEps(w));
	const Scalar invW = Scalar(1) / w;

	const Scalar u_invW = u * invW; // normalized image coordinates
	const Scalar v_invW = v * invW;

	calculateFisheyeDistortNormalized2x2(jDistX, jDistY, u_invW, v_invW, fisheyeCamera.radialDistortion(), fisheyeCamera.tangentialDistortion());

	const Scalar fx_jDistXx_invW = fx * jDistX[0] * invW;
	const Scalar fy_jDistYx_invW = fy * jDistY[0] * invW;

	const Scalar fx_jDistXy_invW = fx * jDistX[1] * invW;
	const Scalar fy_jDistYy_invW = fy * jDistY[1] * invW;

	const Scalar u_fx_jDistXx__ = u_invW * fx_jDistXx_invW + v_invW * fx_jDistXy_invW;
	const Scalar u_fy_jDistYx__ = u_invW * fy_jDistYx_invW + v_invW * fy_jDistYy_invW;

	jx[0] = fx_jDistXx_invW * flippedCamera_T_world[0] + fx_jDistXy_invW * flippedCamera_T_world[1] - u_fx_jDistXx__ * flippedCamera_T_world[2];
	jx[1] = fx_jDistXx_invW * flippedCamera_T_world[4] + fx_jDistXy_invW * flippedCamera_T_world[5] - u_fx_jDistXx__ * flippedCamera_T_world[6];
	jx[2] = fx_jDistXx_invW * flippedCamera_T_world[8] + fx_jDistXy_invW * flippedCamera_T_world[9] - u_fx_jDistXx__ * flippedCamera_T_world[10];

	jy[0] = fy_jDistYx_invW * flippedCamera_T_world[0] + fy_jDistYy_invW * flippedCamera_T_world[1] - u_fy_jDistYx__ * flippedCamera_T_world[2];
	jy[1] = fy_jDistYx_invW * flippedCamera_T_world[4] + fy_jDistYy_invW * flippedCamera_T_world[5] - u_fy_jDistYx__ * flippedCamera_T_world[6];
	jy[2] = fy_jDistYx_invW * flippedCamera_T_world[8] + fy_jDistYy_invW * flippedCamera_T_world[9] - u_fy_jDistYx__ * flippedCamera_T_world[10];
}

void Jacobian::calculatePointJacobian2nx3(Scalar* jacobian, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_P_world, const Vector3* objectPoints, const size_t numberObjectPoints, const bool distortImagePoints)
{
	ocean_assert(jacobian);
	ocean_assert(pinholeCamera && flippedCamera_P_world.isValid());

	if (!distortImagePoints)
	{
		/**
		 * This functions uses three concatenated jacobian matrices to receive one final jacobian matrix with size 2x3.
		 * The final jacobian is defined by: j = jDeh * jProj * jTrans;
		 *
		 * j (2x3) the final jacobian:
		 * | dfx / dox, dfx / doy, dfx / doz |
		 * | dfy / dox, dfy / doy, dfy / doz |
		 *
		 * jDeh (2x3) the jacobian for the dehomogenization:                                        | U |
		 * | du / dU, du / dV, du / dW |   | 1 / W       0         -U / W^2 |   | u |   | U / W |   | V |
		 * | dv / dU, dv / dV, dv / dW | = |   0       1 / W       -V / W^2 |,  | v | = | V / W | = | W |
		 *
		 * jProj (3x3) the jacobian for the projection:
		 * | Fx   0  mx |
		 * | 0   Fy  my |
		 * | 0    0   1 |
		 *
		 * jTrans (3x3) the jacobian for the transformation:
		 * | dfx / dox, dfx / doy, dfx / doz |   | Rxx Ryx Rzx |
		 * | dfy / dox, dfy / doy, dfy / doz | = | Rxy Ryy Rzy |
		 * | dfz / dox, dfz / doy, dfz / doz |   | Rxz Ryz Rzz |
		 */

		for (size_t n = 0; n < numberObjectPoints; ++n)
		{
			const Vector3& objectPoint = objectPoints[n];

			const Vector3 transformedObjectPoint(flippedCamera_P_world * objectPoint);

			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
			const Scalar factor = Scalar(1) / transformedObjectPoint.z();

			const Scalar m00 = factor * pinholeCamera.focalLengthX();
			const Scalar m11 = factor * pinholeCamera.focalLengthY();
			const Scalar m02 = -pinholeCamera.focalLengthX() * transformedObjectPoint.x() * factor * factor;
			const Scalar m12 = -pinholeCamera.focalLengthY() * transformedObjectPoint.y() * factor * factor;

			*jacobian++ = m00 * flippedCamera_P_world[0] + m02 * flippedCamera_P_world[2];
			*jacobian++ = m00 * flippedCamera_P_world[4] + m02 * flippedCamera_P_world[6];
			*jacobian++ = m00 * flippedCamera_P_world[8] + m02 * flippedCamera_P_world[10];

			*jacobian++ = m11 * flippedCamera_P_world[1] + m12 * flippedCamera_P_world[2];
			*jacobian++ = m11 * flippedCamera_P_world[5] + m12 * flippedCamera_P_world[6];
			*jacobian++ = m11 * flippedCamera_P_world[9] + m12 * flippedCamera_P_world[10];
		}
	}
	else
	{
		/**
		 * fFocal(fDist(fDeh(fTrans(fFlippedPose))))
		 *
		 * This functions uses three concatenated jacobian matrix for each object point to receive one final jacobian matrix with size 2x6 for each object point.
		 * The final jacobian is defined by: j = jFocal_Dist * jDeh * jTrans;
		 *
		 * jFocal_Dist(x, y) = [Fx * (x * rad_factor + tan_factor_u) + mx, Fy * (y * rad_factor + tan_factor_v) + my]
		 *
		 * j (2x6) the final jacobian:
		 * | dfx / dox, dfx / doy, dfx / doz |
		 * | dfy / dox, dfy / doy, dfy / doz |
		 *
		 * jFocal_Dist (2x2) the jacobian for the focal projection and distortion correction:
		 * | du' / du, du' / dv |   | ...  ... |
		 * | dv' / du, dv' / dv | = | ...  ... |
		 *
		 * jDeh (2x3) the jacobian for the dehomogenization:                                        | U |
		 * | du / dU, du / dV, du / dW |   | 1 / W       0         -U / W^2 |   | u |   | U / W |   | V |
		 * | dv / dU, dv / dV, dv / dW | = |   0       1 / W       -V / W^2 |,  | v | = | V / W | = | W |
		 *
		 * jTrans (3x3) the jacobian for the transformation:
		 * | dfx / dox, dfx / doy, dfx / doz |   | Rxx Ryx Rzx |
		 * | dfy / dox, dfy / doy, dfy / doz | = | Rxy Ryy Rzy |
		 * | dfz / dox, dfz / doy, dfz / doz |   | Rxz Ryz Rzz |
		 */

		const Scalar k1 = pinholeCamera.radialDistortion().first;
		const Scalar k2 = pinholeCamera.radialDistortion().second;

		const Scalar p1 = pinholeCamera.tangentialDistortion().first;
		const Scalar p2 = pinholeCamera.tangentialDistortion().second;

		const Scalar Fx = pinholeCamera.focalLengthX();
		const Scalar Fy = pinholeCamera.focalLengthY();

		for (size_t n = 0; n < numberObjectPoints; ++n)
		{
			const Vector3& objectPoint = objectPoints[n];

			const Vector3 transformedObjectPoint(flippedCamera_P_world * objectPoint);

			ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
			const Scalar scaleFactor = 1 / transformedObjectPoint.z();

			const Scalar u = transformedObjectPoint.x() * scaleFactor;
			const Scalar v = transformedObjectPoint.y() * scaleFactor;

			const Scalar dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
			const Scalar dist2_u_1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v)))); // dist2_u == dist1_v
			const Scalar dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);

			const Scalar fx_w_dist1_u = Fx * scaleFactor * dist1_u;
			const Scalar fx_w_dist1_v = Fx * scaleFactor * dist2_u_1_v;
			const Scalar fx_w2__ = -Fx * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist2_u_1_v);

			*jacobian++ = fx_w_dist1_u * flippedCamera_P_world[0] + fx_w_dist1_v * flippedCamera_P_world[1] + fx_w2__ * flippedCamera_P_world[2];
			*jacobian++ = fx_w_dist1_u * flippedCamera_P_world[4] + fx_w_dist1_v * flippedCamera_P_world[5] + fx_w2__ * flippedCamera_P_world[6];
			*jacobian++ = fx_w_dist1_u * flippedCamera_P_world[8] + fx_w_dist1_v * flippedCamera_P_world[9] + fx_w2__ * flippedCamera_P_world[10];

			const Scalar fy_w_dist2_u = Fy * scaleFactor * dist2_u_1_v;
			const Scalar fy_w_dist2_v = Fy * scaleFactor * dist2_v;
			const Scalar fy_w2__ = -Fy * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist2_u_1_v + transformedObjectPoint.y() * dist2_v);

			*jacobian++ = fy_w_dist2_u * flippedCamera_P_world[0] + fy_w_dist2_v * flippedCamera_P_world[1] + fy_w2__ * flippedCamera_P_world[2];
			*jacobian++ = fy_w_dist2_u * flippedCamera_P_world[4] + fy_w_dist2_v * flippedCamera_P_world[5] + fy_w2__ * flippedCamera_P_world[6];
			*jacobian++ = fy_w_dist2_u * flippedCamera_P_world[8] + fy_w_dist2_v * flippedCamera_P_world[9] + fy_w2__ * flippedCamera_P_world[10];
		}
	}
}

void Jacobian::calculateCameraDistortionJacobian2x4(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Vector2& normalizedImagePoint)
{
	ocean_assert(jx && jy);

	const Scalar& x = normalizedImagePoint.x();
	const Scalar& y = normalizedImagePoint.y();

	const Scalar& Fx = pinholeCamera.focalLengthX();
	const Scalar& Fy = pinholeCamera.focalLengthY();

	/**
	 * fundistort_intrinsic_xy(k1, k2, p1, p2, Fx, Fy, mx, my) = (u, v)
	 * x' = x + x * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2] + p1 * 2 * x * y + p2 * ((x^2 + y^2) + 2 * x^2)
	 * y' = y + y * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2] + p2 * 2 * x * y + p1 * ((x^2 + y^2) + 2 * y^2)
	 * u = Fx * x' + mx
	 * v = Fy * y' + my
	 *
	 * fui_x = Fx * (x + x * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2] + p1 * 2 * x * y + p2 * ((x^2 + y^2) + 2 * x^2)) + mx
	 * fui_y = Fy * (y + y * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2] + p2 * 2 * x * y + p1 * ((x^2 + y^2) + 2 * y^2)) + my
	 *
	 */

	const Scalar xy2 = x * x + y * y;

	/**
	 * dfui_xy / dk1
	 *
	 * | Fx x (x^2 + y^2) |
	 * | Fy y (x^2 + y^2) |
	 *
	 */
	jx[0] = Fx * x * xy2;
	jy[0] = Fy * y * xy2;


	/**
	 * dfui_xy / dk2
	 *
	 * | Fx x (x^2 + y^2)^2 |
	 * | Fy y (x^2 + y^2)^2 |
	 *
	 */
	jx[1] = Fx * x * Numeric::sqr(xy2);
	jy[1] = Fy * y * Numeric::sqr(xy2);


	/**
	 * dfui_xy / dp1
	 *
	 * |    2 Fx x y    |
	 * | Fy (x^2+3 y^2) |
	 *
	 */
	jx[2] = 2 * Fx * x * y;
	jy[2] = Fy * (xy2 + 2 * y * y);


	/**
	 * dfui_xy / dp2
	 *
	 * | Fx (3 x^2+y^2) |
	 * |    2 Fy x y    |
	 *
	 */
	jx[3] = Fx * (xy2 + 2 * x * x);
	jy[3] = 2 * Fy * x * y;
}

void Jacobian::calculateCameraJacobian2x6(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Vector2& normalizedImagePoint)
{
	ocean_assert(jx && jy);

	const Scalar& x = normalizedImagePoint.x();
	const Scalar& y = normalizedImagePoint.y();

	const Scalar& Fx = pinholeCamera.focalLengthX();
	const Scalar& Fy = pinholeCamera.focalLengthY();

	const Scalar& k1 = pinholeCamera.radialDistortion().first;
	const Scalar& k2 = pinholeCamera.radialDistortion().second;

	/**
	 * fundistort_intrinsic_xy(k1, k2, Fx, Fy, mx, my) = (u, v)
	 * x' = x + x * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2]
	 * y' = y + y * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2]
	 * u = Fx * x' + mx
	 * v = Fy * y' + my
	 *
	 * fui_x = Fx * (x + x * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2]) + mx
	 * fui_y = Fy * (y + y * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2]) + my
	 *
	 */

	const Scalar xy = x * x + y * y;

	/**
	 * dfui_xy / dk1
	 *
	 * | Fx x (x^2 + y^2) |
	 * | Fy y (x^2 + y^2) |
	 *
	 */
	jx[0] = Fx * x * xy;
	jy[0] = Fy * y * xy;


	/**
	 * dfui_xy / dk2
	 *
	 * | Fx x (x^2 + y^2)^2 |
	 * | Fy y (x^2 + y^2)^2 |
	 *
	 */
	jx[1] = Fx * x * Numeric::sqr(xy);
	jy[1] = Fy * y * Numeric::sqr(xy);


	/**
	 * dfui_xy / dFx
	 *
	 * | x * (1 + (x^2 + y^2) * (k1 + k2 * (x^2 + y^2))) |
	 * |                         0                       |
	 *
	 */
	jx[2] = x * (1 + xy * (k1 + k2 * xy));
	jy[2] = 0;


	/**
	 * dfui_xy / dFy
	 *
	 * |                         0                       |
	 * | y * (1 + (x^2 + y^2) * (k1 + k2 * (x^2 + y^2))) |
	 *
	 */
	jx[3] = 0;
	jy[3] = y * (1 + xy * (k1 + k2 * xy));


	/**
	 * dfui_xy / dmx
	 *
	 * | 1 |
	 * | 0 |
	 *
	 */
	jx[4] = 1;
	jy[4] = 0;


	/**
	 * dfui_xy / dmy
	 *
	 * | 0 |
	 * | 1 |
	 *
	 */
	jx[5] = 0;
	jy[5] = 1;
}

void Jacobian::calculateCameraJacobian2x7(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Vector2& normalizedImagePoint)
{
	ocean_assert(jx && jy);

	const Scalar& x = normalizedImagePoint.x();
	const Scalar& y = normalizedImagePoint.y();

	const Scalar& _Fx = pinholeCamera.focalLengthX();
	const Scalar& _Fy = pinholeCamera.focalLengthY();

	ocean_assert(Numeric::isEqual(_Fx, _Fy, Scalar(0.01)));
	const Scalar F = (_Fx + _Fy) * Scalar(0.5);

	const Scalar& k1 = pinholeCamera.radialDistortion().first;
	const Scalar& k2 = pinholeCamera.radialDistortion().second;

	const Scalar& p1 = pinholeCamera.tangentialDistortion().first;
	const Scalar& p2 = pinholeCamera.tangentialDistortion().second;

	/**
	 * fundistort_intrinsic_xy(k1, k2, p1, p2, Fx, Fy, mx, my) = (u, v)
	 * x' = x + x * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2] + p1 * 2 * x * y + p2 * ((x^2 + y^2) + 2 * x^2)
	 * y' = y + y * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2] + p2 * 2 * x * y + p1 * ((x^2 + y^2) + 2 * y^2)
	 * u = F * x' + mx
	 * v = F * y' + my
	 *
	 * fui_x = F * (x + x * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2] + p1 * 2 * x * y + p2 * ((x^2 + y^2) + 2 * x^2)) + mx
	 * fui_y = F * (y + y * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2] + p2 * 2 * x * y + p1 * ((x^2 + y^2) + 2 * y^2)) + my
	 *
	 */

	const Scalar xy2 = x * x + y * y;

	/**
	 * dfui_xy / dk1
	 *
	 * | F x (x^2 + y^2) |
	 * | F y (x^2 + y^2) |
	 *
	 */
	jx[0] = F * x * xy2;
	jy[0] = F * y * xy2;


	/**
	 * dfui_xy / dk2
	 *
	 * | F x (x^2 + y^2)^2 |
	 * | F y (x^2 + y^2)^2 |
	 *
	 */
	jx[1] = F * x * Numeric::sqr(xy2);
	jy[1] = F * y * Numeric::sqr(xy2);


	/**
	 * dfui_xy / dp1
	 *
	 * |    2 F x y    |
	 * | F (x^2+3 y^2) |
	 *
	 */
	jx[2] = 2 * F * x * y;
	jy[2] = F * (xy2 + 2 * y * y);


	/**
	 * dfui_xy / dp2
	 *
	 * | F (3 x^2+y^2) |
	 * |    2 F x y    |
	 *
	 */
	jx[3] = F * (xy2 + 2 * x * x);
	jy[3] = 2 * F * x * y;


	/**
	 * dfui_xy / dF
	 *
	 * | 2 p1 x y + p2 (3 x^2 + y^2) + x (1 + (x^2 + y^2) (k1 + k2 (x^2 + y^2))) |
	 * |  y + 2 p2 x y + p1 (x^2 + 3 y^2) + y (x^2 + y^2) (k1 + k2 (x^2 + y^2))  |
	 *
	 */
	jx[4] = 2 * p1 * x * y + p2 * (2 * x * x + xy2) + x * (1 + xy2 * (k1 + k2 * xy2));
	jy[4] = y + 2 * p2 * x * y + p1 * (xy2 + 2 * y * y) + y * xy2* (k1 + k2 * xy2);


	/**
	 * dfui_xy / dmx
	 *
	 * | 1 |
	 * | 0 |
	 *
	 */
	jx[5] = 1;
	jy[5] = 0;


	/**
	 * dfui_xy / dmy
	 *
	 * | 0 |
	 * | 1 |
	 *
	 */
	jx[6] = 0;
	jy[6] = 1;
}

void Jacobian::calculateCameraJacobian2x8(Scalar* jx, Scalar* jy, const PinholeCamera& pinholeCamera, const Vector2& normalizedImagePoint)
{
	ocean_assert(jx && jy);

	const Scalar& x = normalizedImagePoint.x();
	const Scalar& y = normalizedImagePoint.y();

	const Scalar& Fx = pinholeCamera.focalLengthX();
	const Scalar& Fy = pinholeCamera.focalLengthY();

	const Scalar& k1 = pinholeCamera.radialDistortion().first;
	const Scalar& k2 = pinholeCamera.radialDistortion().second;

	const Scalar& p1 = pinholeCamera.tangentialDistortion().first;
	const Scalar& p2 = pinholeCamera.tangentialDistortion().second;

	/**
	 * fundistort_intrinsic_xy(k1, k2, p1, p2, Fx, Fy, mx, my) = (u, v)
	 * x' = x + x * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2] + p1 * 2 * x * y + p2 * ((x^2 + y^2) + 2 * x^2)
	 * y' = y + y * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2] + p2 * 2 * x * y + p1 * ((x^2 + y^2) + 2 * y^2)
	 * u = Fx * x' + mx
	 * v = Fy * y' + my
	 *
	 * fui_x = Fx * (x + x * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2] + p1 * 2 * x * y + p2 * ((x^2 + y^2) + 2 * x^2)) + mx
	 * fui_y = Fy * (y + y * [k1 * (x^2 + y^2) + k2 * (x^2 + y^2)^2] + p2 * 2 * x * y + p1 * ((x^2 + y^2) + 2 * y^2)) + my
	 *
	 */

	const Scalar xy2 = x * x + y * y;

	/**
	 * dfui_xy / dk1
	 *
	 * | Fx x (x^2 + y^2) |
	 * | Fy y (x^2 + y^2) |
	 *
	 */
	jx[0] = Fx * x * xy2;
	jy[0] = Fy * y * xy2;


	/**
	 * dfui_xy / dk2
	 *
	 * | Fx x (x^2 + y^2)^2 |
	 * | Fy y (x^2 + y^2)^2 |
	 *
	 */
	jx[1] = Fx * x * Numeric::sqr(xy2);
	jy[1] = Fy * y * Numeric::sqr(xy2);


	/**
	 * dfui_xy / dp1
	 *
	 * |    2 Fx x y    |
	 * | Fy (x^2+3 y^2) |
	 *
	 */
	jx[2] = 2 * Fx * x * y;
	jy[2] = Fy * (xy2 + 2 * y * y);


	/**
	 * dfui_xy / dp2
	 *
	 * | Fx (3 x^2+y^2) |
	 * |    2 Fy x y    |
	 *
	 */
	jx[3] = Fx * (xy2 + 2 * x * x);
	jy[3] = 2 * Fy * x * y;


	/**
	 * dfui_xy / dFx
	 *
	 * | 2 p1 x y + p2 (3 x^2 + y^2) + x (1 + (x^2 + y^2) (k1 + k2 (x^2 + y^2))) |
	 * |                                      0                                  |
	 *
	 */
	jx[4] = 2 * p1 * x * y + p2 * (2 * x * x + xy2) + x * (1 + xy2 * (k1 + k2 * xy2));
	jy[4] = 0;


	/**
	 * dfui_xy / dFy
	 *
	 * |                               0                                       |
	 * | y + 2 p2 x y + p1 (x^2 + 3 y^2) + y (x^2 + y^2) (k1 + k2 (x^2 + y^2)) |
	 *
	 */
	jx[5] = 0;
	jy[5] = y + 2 * p2 * x * y + p1 * (xy2 + 2 * y * y) + y * xy2* (k1 + k2 * xy2);


	/**
	 * dfui_xy / dmx
	 *
	 * | 1 |
	 * | 0 |
	 *
	 */
	jx[6] = 1;
	jy[6] = 0;


	/**
	 * dfui_xy / dmy
	 *
	 * | 0 |
	 * | 1 |
	 *
	 */
	jx[7] = 0;
	jy[7] = 1;
}

void Jacobian::calculateJacobianCameraPoseRodrigues2x12(Scalar* jacobianX, Scalar* jacobianY, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& objectPoint)
{
	ocean_assert(jacobianX && jacobianY && pinholeCamera.isValid());

	const Pose flippedCamera_P_world(flippedCamera_T_world);

	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

	calculateJacobianCameraPoseRodrigues2x12(jacobianX, jacobianY, pinholeCamera, flippedCamera_T_world, flippedCamera_P_world, objectPoint, Rwx, Rwy, Rwz);
}

void Jacobian::calculateJacobianCameraPoseRodrigues2x12(Scalar* jacobianX, Scalar* jacobianY, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Pose& flippedCamera_P_world, const Vector3& objectPoint, const SquareMatrix3& dwx, const SquareMatrix3& dwy, const SquareMatrix3& dwz)
{
	ocean_assert(jacobianX && jacobianY && pinholeCamera.isValid());

	ocean_assert_and_suppress_unused((std::is_same<Scalar, float>::value) || flippedCamera_P_world.transformation().isEqual(flippedCamera_T_world, Numeric::weakEps()), flippedCamera_P_world);

	const Scalar k1 = pinholeCamera.radialDistortion().first;
	const Scalar k2 = pinholeCamera.radialDistortion().second;
	const Scalar p1 = pinholeCamera.tangentialDistortion().first;
	const Scalar p2 = pinholeCamera.tangentialDistortion().second;

	const Vector3 transformedObjectPoint = flippedCamera_T_world * objectPoint;

	ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
	const Scalar scaleFactor = Scalar(1) / transformedObjectPoint.z();

	const Scalar u = transformedObjectPoint.x() * scaleFactor;
	const Scalar v = transformedObjectPoint.y() * scaleFactor;

	const Scalar dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
	const Scalar dist2_u_1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v))));
	const Scalar dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);

	const Scalar uv2 = u * u + v * v;

	const Scalar dist1_k1 = u * uv2;
	const Scalar dist1_k2 = u * uv2 * uv2;

	const Scalar dist2_k1 = v * uv2;
	const Scalar dist2_k2 = v * uv2 * uv2;

	const Vector3 dwxObject(dwx * objectPoint);
	const Vector3 dwyObject(dwy * objectPoint);
	const Vector3 dwzObject(dwz * objectPoint);

	*jacobianX++ = pinholeCamera.focalLengthX() * dist1_k1;
	*jacobianX++ = pinholeCamera.focalLengthX() * dist1_k2;
	*jacobianX++ = u + u * (k1 * uv2 + k2 * uv2 * uv2) + p1 * 2 * u * v + p2 * (uv2 + 2 * u * u);
	*jacobianX++ = 0;
	*jacobianX++ = 1;
	*jacobianX++ = 0;

	const Scalar intermediate1_1 = pinholeCamera.focalLengthX() * scaleFactor * dist1_u;
	const Scalar intermediate1_2 = pinholeCamera.focalLengthX() * scaleFactor * dist2_u_1_v;
	const Scalar intermediate1_3 = -pinholeCamera.focalLengthX() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist2_u_1_v);

	*jacobianX++ = intermediate1_1 * dwxObject[0] + intermediate1_2 * dwxObject[1] + intermediate1_3 * dwxObject[2];
	*jacobianX++ = intermediate1_1 * dwyObject[0] + intermediate1_2 * dwyObject[1] + intermediate1_3 * dwyObject[2];
	*jacobianX++ = intermediate1_1 * dwzObject[0] + intermediate1_2 * dwzObject[1] + intermediate1_3 * dwzObject[2];
	*jacobianX++ = intermediate1_1;
	*jacobianX++ = intermediate1_2;
	*jacobianX++ = intermediate1_3;

	*jacobianY++ = pinholeCamera.focalLengthY() * dist2_k1;
	*jacobianY++ = pinholeCamera.focalLengthY() * dist2_k2;
	*jacobianY++ = 0;
	*jacobianY++ = v + v * (k1 * uv2 + k2 * uv2 * uv2) + 2 * p2 * u * v + p1 * (uv2 + 2 * v * v);
	*jacobianY++ = 0;
	*jacobianY++ = 1;

	const Scalar intermediate2_1 = pinholeCamera.focalLengthY() * scaleFactor * dist2_u_1_v;
	const Scalar intermediate2_2 = pinholeCamera.focalLengthY() * scaleFactor * dist2_v;
	const Scalar intermediate2_3 = -pinholeCamera.focalLengthY() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist2_u_1_v + transformedObjectPoint.y() * dist2_v);

	*jacobianY++ = intermediate2_1 * dwxObject[0] + intermediate2_2 * dwxObject[1] + intermediate2_3 * dwxObject[2];
	*jacobianY++ = intermediate2_1 * dwyObject[0] + intermediate2_2 * dwyObject[1] + intermediate2_3 * dwyObject[2];
	*jacobianY++ = intermediate2_1 * dwzObject[0] + intermediate2_2 * dwzObject[1] + intermediate2_3 * dwzObject[2];
	*jacobianY++ = intermediate2_1;
	*jacobianY++ = intermediate2_2;
	*jacobianY++ = intermediate2_3;
}

void Jacobian::calculateJacobianCameraPoseRodrigues2x14(Scalar* jacobianX, Scalar* jacobianY, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& objectPoint)
{
	ocean_assert(jacobianX && jacobianY && pinholeCamera.isValid());

	const Pose flippedCamera_P_world(flippedCamera_T_world);

	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

	calculateJacobianCameraPoseRodrigues2x14(jacobianX, jacobianY, pinholeCamera, flippedCamera_T_world, flippedCamera_P_world, objectPoint, Rwx, Rwy, Rwz);
}

void Jacobian::calculateJacobianCameraPoseRodrigues2x14(Scalar* jacobianX, Scalar* jacobianY, const PinholeCamera& pinholeCamera, const HomogenousMatrix4& flippedCamera_T_world, const Pose& flippedCamera_P_world, const Vector3& objectPoint, const SquareMatrix3& dwx, const SquareMatrix3& dwy, const SquareMatrix3& dwz)
{
	ocean_assert(jacobianX && jacobianY && pinholeCamera.isValid());

	ocean_assert_and_suppress_unused((std::is_same<Scalar, float>::value) || flippedCamera_P_world.transformation() == flippedCamera_T_world, flippedCamera_P_world);

	const Scalar k1 = pinholeCamera.radialDistortion().first;
	const Scalar k2 = pinholeCamera.radialDistortion().second;
	const Scalar p1 = pinholeCamera.tangentialDistortion().first;
	const Scalar p2 = pinholeCamera.tangentialDistortion().second;

	const Vector3 transformedObjectPoint = flippedCamera_T_world * objectPoint;

	ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
	const Scalar scaleFactor = Scalar(1) / transformedObjectPoint.z();

	const Scalar u = transformedObjectPoint.x() * scaleFactor;
	const Scalar v = transformedObjectPoint.y() * scaleFactor;

	const Scalar dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
	const Scalar dist2_u_1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v))));
	const Scalar dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);

	const Scalar uv2 = u * u + v * v;

	const Scalar dist1_k1 = u * uv2;
	const Scalar dist1_k2 = u * uv2 * uv2;
	const Scalar dist1_p1_2_p2 = 2 * u * v;
	const Scalar dist1_p2 = 3 * u * u + v * v;

	const Scalar dist2_k1 = v * uv2;
	const Scalar dist2_k2 = v * uv2 * uv2;
	const Scalar dist2_p1 = u * u + 3 * v * v;

	const Vector3 dwxObject(dwx * objectPoint);
	const Vector3 dwyObject(dwy * objectPoint);
	const Vector3 dwzObject(dwz * objectPoint);

	*jacobianX++ = pinholeCamera.focalLengthX() * dist1_k1;
	*jacobianX++ = pinholeCamera.focalLengthX() * dist1_k2;
	*jacobianX++ = pinholeCamera.focalLengthX() * dist1_p1_2_p2;
	*jacobianX++ = pinholeCamera.focalLengthX() * dist1_p2;
	*jacobianX++ = u + u * (k1 * uv2 + k2 * uv2 * uv2) + p1 * 2 * u * v + p2 * (uv2 + 2 * u * u);
	*jacobianX++ = 0;
	*jacobianX++ = 1;
	*jacobianX++ = 0;

	const Scalar intermediate1_1 = pinholeCamera.focalLengthX() * scaleFactor * dist1_u;
	const Scalar intermediate1_2 = pinholeCamera.focalLengthX() * scaleFactor * dist2_u_1_v;
	const Scalar intermediate1_3 = -pinholeCamera.focalLengthX() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist2_u_1_v);

	*jacobianX++ = intermediate1_1 * dwxObject[0] + intermediate1_2 * dwxObject[1] + intermediate1_3 * dwxObject[2];
	*jacobianX++ = intermediate1_1 * dwyObject[0] + intermediate1_2 * dwyObject[1] + intermediate1_3 * dwyObject[2];
	*jacobianX++ = intermediate1_1 * dwzObject[0] + intermediate1_2 * dwzObject[1] + intermediate1_3 * dwzObject[2];
	*jacobianX++ = intermediate1_1;
	*jacobianX++ = intermediate1_2;
	*jacobianX++ = intermediate1_3;

	*jacobianY++ = pinholeCamera.focalLengthY() * dist2_k1;
	*jacobianY++ = pinholeCamera.focalLengthY() * dist2_k2;
	*jacobianY++ = pinholeCamera.focalLengthY() * dist2_p1;
	*jacobianY++ = pinholeCamera.focalLengthY() * dist1_p1_2_p2;
	*jacobianY++ = 0;
	*jacobianY++ = v + v * (k1 * uv2 + k2 * uv2 * uv2) + 2 * p2 * u * v + p1 * (uv2 + 2 * v * v);
	*jacobianY++ = 0;
	*jacobianY++ = 1;

	const Scalar intermediate2_1 = pinholeCamera.focalLengthY() * scaleFactor * dist2_u_1_v;
	const Scalar intermediate2_2 = pinholeCamera.focalLengthY() * scaleFactor * dist2_v;
	const Scalar intermediate2_3 = -pinholeCamera.focalLengthY() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist2_u_1_v + transformedObjectPoint.y() * dist2_v);

	*jacobianY++ = intermediate2_1 * dwxObject[0] + intermediate2_2 * dwxObject[1] + intermediate2_3 * dwxObject[2];
	*jacobianY++ = intermediate2_1 * dwyObject[0] + intermediate2_2 * dwyObject[1] + intermediate2_3 * dwyObject[2];
	*jacobianY++ = intermediate2_1 * dwzObject[0] + intermediate2_2 * dwzObject[1] + intermediate2_3 * dwzObject[2];
	*jacobianY++ = intermediate2_1;
	*jacobianY++ = intermediate2_2;
	*jacobianY++ = intermediate2_3;
}

void Jacobian::calculateJacobianCameraPoseRodrigues2nx14(Scalar* jacobian, const PinholeCamera& pinholeCamera, const Pose& flippedCamera_P_world, const ConstIndexedAccessor<Vector3>& objectPoints)
{
	ocean_assert(jacobian && pinholeCamera.isValid());

	SquareMatrix3 Rwx, Rwy, Rwz;
	calculateRotationRodriguesDerivative(ExponentialMap(Vector3(flippedCamera_P_world.rx(), flippedCamera_P_world.ry(), flippedCamera_P_world.rz())), Rwx, Rwy, Rwz);

	const HomogenousMatrix4 iTransformation(flippedCamera_P_world.transformation());

	const Scalar k1 = pinholeCamera.radialDistortion().first;
	const Scalar k2 = pinholeCamera.radialDistortion().second;
	const Scalar p1 = pinholeCamera.tangentialDistortion().first;
	const Scalar p2 = pinholeCamera.tangentialDistortion().second;

	for (size_t n = 0u; n < objectPoints.size(); ++n)
	{
		const Vector3& objectPoint = objectPoints[n];

		const Vector3 transformedObjectPoint = iTransformation * objectPoint;

		ocean_assert(Numeric::isNotEqualEps(transformedObjectPoint.z()));
		const Scalar scaleFactor = Scalar(1) / transformedObjectPoint.z();

		const Scalar u = transformedObjectPoint.x() * scaleFactor;
		const Scalar v = transformedObjectPoint.y() * scaleFactor;

		const Scalar dist1_u = Scalar(1) + Scalar(6) * p2 * u + Scalar(2) * p1 * v + k1 * (Scalar(3) * u * u + v * v) + k2 * (u * u + v * v) * (Scalar(5) * u * u + v * v);
		const Scalar dist2_u_1_v = Scalar(2) * (p1 * u + v * (p2 + u * (k1 + Scalar(2) * k2 * (u * u + v * v))));
		const Scalar dist2_v = Scalar(1) + Scalar(2) * p2 * u + Scalar(6) * p1 * v + k1 * (u * u + Scalar(3) * v * v) + k2 * (u * u + v * v) * (u * u + Scalar(5) * v * v);

		const Scalar uv2 = u * u + v * v;

		const Scalar dist1_k1 = u * uv2;
		const Scalar dist1_k2 = u * uv2 * uv2;
		const Scalar dist1_p1_2_p2 = 2 * u * v;
		const Scalar dist1_p2 = 3 * u * u + v * v;

		const Scalar dist2_k1 = v * uv2;
		const Scalar dist2_k2 = v * uv2 * uv2;
		const Scalar dist2_p1 = u * u + 3 * v * v;

		const Vector3 dwx(Rwx * objectPoint);
		const Vector3 dwy(Rwy * objectPoint);
		const Vector3 dwz(Rwz * objectPoint);

		// Scalar* jacobianX = jacobian;
		Scalar* jacobianY = jacobian + 14;

		*jacobian++ = pinholeCamera.focalLengthX() * dist1_k1;
		*jacobian++ = pinholeCamera.focalLengthX() * dist1_k2;
		*jacobian++ = pinholeCamera.focalLengthX() * dist1_p1_2_p2;
		*jacobian++ = pinholeCamera.focalLengthX() * dist1_p2;
		*jacobian++ = u + u * (k1 * uv2 + k2 * uv2 * uv2) + p1 * 2 * u * v + p2 * (uv2 + 2 * u * u);
		*jacobian++ = 0;
		*jacobian++ = 1;
		*jacobian++ = 0;

		const Scalar intermediate1_1 = pinholeCamera.focalLengthX() * scaleFactor * dist1_u;
		const Scalar intermediate1_2 = pinholeCamera.focalLengthX() * scaleFactor * dist2_u_1_v;
		const Scalar intermediate1_3 = -pinholeCamera.focalLengthX() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist1_u + transformedObjectPoint.y() * dist2_u_1_v);

		*jacobian++ = intermediate1_1 * dwx[0] + intermediate1_2 * dwx[1] + intermediate1_3 * dwx[2];
		*jacobian++ = intermediate1_1 * dwy[0] + intermediate1_2 * dwy[1] + intermediate1_3 * dwy[2];
		*jacobian++ = intermediate1_1 * dwz[0] + intermediate1_2 * dwz[1] + intermediate1_3 * dwz[2];
		*jacobian++ = intermediate1_1;
		*jacobian++ = intermediate1_2;
		*jacobian++ = intermediate1_3;

		*jacobianY++ = pinholeCamera.focalLengthY() * dist2_k1;
		*jacobianY++ = pinholeCamera.focalLengthY() * dist2_k2;
		*jacobianY++ = pinholeCamera.focalLengthY() * dist2_p1;
		*jacobianY++ = pinholeCamera.focalLengthY() * dist1_p1_2_p2;
		*jacobianY++ = 0;
		*jacobianY++ = v + v * (k1 * uv2 + k2 * uv2 * uv2) + 2 * p2 * u * v + p1 * (uv2 + 2 * v * v);
		*jacobianY++ = 0;
		*jacobianY++ = 1;

		const Scalar intermediate2_1 = pinholeCamera.focalLengthY() * scaleFactor * dist2_u_1_v;
		const Scalar intermediate2_2 = pinholeCamera.focalLengthY() * scaleFactor * dist2_v;
		const Scalar intermediate2_3 = -pinholeCamera.focalLengthY() * scaleFactor * scaleFactor * (transformedObjectPoint.x() * dist2_u_1_v + transformedObjectPoint.y() * dist2_v);

		*jacobianY++ = intermediate2_1 * dwx[0] + intermediate2_2 * dwx[1] + intermediate2_3 * dwx[2];
		*jacobianY++ = intermediate2_1 * dwy[0] + intermediate2_2 * dwy[1] + intermediate2_3 * dwy[2];
		*jacobianY++ = intermediate2_1 * dwz[0] + intermediate2_2 * dwz[1] + intermediate2_3 * dwz[2];
		*jacobianY++ = intermediate2_1;
		*jacobianY++ = intermediate2_2;
		*jacobianY++ = intermediate2_3;

		jacobian += 14;
	}
}

void Jacobian::calculateHomographyJacobian2x8(Scalar* jx, Scalar* jy, const Scalar x, const Scalar y, const SquareMatrix3& homography)
{
	ocean_assert(jx && jy);

	// we expect a normalized homography
	ocean_assert(Numeric::isEqual(homography(2, 2), 1));

	// Homography:
	// |   h0    h3     h6  |
	// |   h1    h4		h7  |
	// |   h2    h5     1   |

	// hx(x, y) = [ h0*x + h3*y + h6 ] / [h2*x + h5*y + 1]
	// hy(x, y) = [ h1*x + h4*y + h7 ] / [h2*x + h5*y + 1]

	// u = h0*x + h3*y + h6
	// v = h1*x + h4*y + h7
	// w = h2*x + h5*y + 1

	// Jacobian:  x/w   0    -xu/w^2   y/w    0    -yu/w^2    1/w    0
	//             0   x/w   -xv/w^2    0    y/w   -yv/w^2     0    1/w

	const Scalar u = homography[0] * x + homography[3] * y + homography[6];
	const Scalar v = homography[1] * x + homography[4] * y + homography[7];
	const Scalar w = homography[2] * x + homography[5] * y + Scalar(1);

	ocean_assert(Numeric::isNotEqualEps(w));
	const Scalar invW = Scalar(1) / w;
	const Scalar invW2 = invW * invW;

	jx[0] = x * invW;
	jx[1] = Scalar(0);
	jx[2] = -x * u * invW2;
	jx[3] = y * invW;
	jx[4] = Scalar(0);
	jx[5] = -y * u * invW2;
	jx[6] = invW;
	jx[7] = Scalar(0);

	jy[0] = Scalar(0);
	jy[1] = x * invW;
	jy[2] = -x * v * invW2;
	jy[3] = Scalar(0);
	jy[4] = y * invW;
	jy[5] = -y * v * invW2;
	jy[6] = Scalar(0);
	jy[7] = invW;
}

void Jacobian::calculateHomographyJacobian2x9(Scalar* jx, Scalar* jy, const Scalar x, const Scalar y, const SquareMatrix3& homography)
{
	ocean_assert(jx && jy);

	// we expect a normalized homography
	ocean_assert(Numeric::isEqual(homography(2, 2), 1));

	// Homography:
	// |   h0    h3    h6   |
	// |   h1    h4    h7   |
	// |   h2    h5    h8   |

	// hx(x, y) = [ h0*x + h3*y + h6 ] / [h2*x + h5*y + h8]
	// hy(x, y) = [ h1*x + h4*y + h7 ] / [h2*x + h5*y + h8]

	// u = h0*x + h3*y + h6
	// v = h1*x + h4*y + h7
	// w = h2*x + h5*y + h8

	// Jacobian:  x/w   0    -xu/w^2   y/w    0    -yu/w^2    1/w    0   -u/w^2
	//             0   x/w   -xv/w^2    0    y/w   -yv/w^2     0    1/w  -v/w^2

	const Scalar u = homography[0] * x + homography[3] * y + homography[6];
	const Scalar v = homography[1] * x + homography[4] * y + homography[7];
	const Scalar w = homography[2] * x + homography[5] * y + Scalar(1);

	ocean_assert(Numeric::isNotEqualEps(w));
	const Scalar invW = Scalar(1) / w;
	const Scalar invW2 = invW * invW;

	jx[0] = x * invW;
	jx[1] = Scalar(0);
	jx[2] = -x * u * invW2;
	jx[3] = y * invW;
	jx[4] = Scalar(0);
	jx[5] = -y * u * invW2;
	jx[6] = invW;
	jx[7] = Scalar(0);
	jx[8] = -u * invW2;

	jy[0] = Scalar(0);
	jy[1] = x * invW;
	jy[2] = -x * v * invW2;
	jy[3] = Scalar(0);
	jy[4] = y * invW;
	jy[5] = -y * v * invW2;
	jy[6] = Scalar(0);
	jy[7] = invW;
	jy[8] = -v * invW2;
}

void Jacobian::calculateIdentityHomographyJacobian2x8(Scalar* jx, Scalar* jy, const Scalar x, const Scalar y)
{
	ocean_assert(jx && jy);

	// Homography:
	// | 1 + h0    h3     h6 |
	// |   h1    1 + h4   h7 |
	// |   h2      h5     1  |

	// hx(x, y) = [ (1+h0)*x + h3*y + h6 ] / [h2*x + h5*y + 1]
	// hy(x, y) = [ h1*x + (1+h4)*y + h7 ] / [h2*x + h5*y + 1]

	// Jacobian hx | h=0:  x   0  -x^2   y   0   -xy    1    0
	//          hy | h=0:  0   x  -xy    0   y   -y^2   0    1

	jx[0] = x;
	jx[1] = Scalar(0);
	jx[2] = -x * x;
	jx[3] = y;
	jx[4] = Scalar(0);
	jx[5] = -x * y;
	jx[6] = Scalar(1);
	jx[7] = Scalar(0);

	jy[0] = Scalar(0);
	jy[1] = x;
	jy[2] = -x * y;
	jy[3] = Scalar(0);
	jy[4] = y;
	jy[5] = -y * y;
	jy[6] = Scalar(0);
	jy[7] = Scalar(1);
}

void Jacobian::calculateIdentityHomographyJacobian2x9(Scalar* jx, Scalar* jy, const Scalar x, const Scalar y)
{
	ocean_assert(jx && jy);

	// Homography:
	// | 1 + h0    h3       h6   |
	// |   h1    1 + h4     h7   |
	// |   h2      h5     1 + h8 |

	// hx(x, y) = [ (1+h0)*x + h3*y + h6 ] / [h2*x + h5*y + (1+h8)]
	// hy(x, y) = [ h1*x + (1+h4)*y + h7 ] / [h2*x + h5*y + (1+h8)]

	// Jacobian hx | h=0:  x   0  -x^2   y   0   -xy    1    0   -x
	//          hy | h=0:  0   x  -xy    0   y   -y^2   0    1   -y

	jx[0] = x;
	jx[1] = Scalar(0);
	jx[2] = -x * x;
	jx[3] = y;
	jx[4] = Scalar(0);
	jx[5] = -x * y;
	jx[6] = Scalar(1);
	jx[7] = Scalar(0);
	jx[8] = -x;

	jy[0] = Scalar(0);
	jy[1] = x;
	jy[2] = -x * y;
	jy[3] = Scalar(0);
	jy[4] = y;
	jy[5] = -y * y;
	jy[6] = Scalar(0);
	jy[7] = Scalar(1);
	jy[8] = -y;
}

}

}
