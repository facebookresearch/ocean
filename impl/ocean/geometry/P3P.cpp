/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/P3P.h"

#include "ocean/math/Equation.h"
#include "ocean/math/Line2.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Geometry
{

unsigned int P3P::poses(const PinholeCamera& pinholeCamera, const Vector3* objectPoints, const Vector2* imagePoints, HomogenousMatrix4* world_T_cameras, const Scalar minimalCollinearSqrDistance)
{
	/// p3p algorithm from original RANSAC paper

	ocean_assert(objectPoints != nullptr && imagePoints != nullptr && world_T_cameras != nullptr);

	if (imagePoints[0] == imagePoints[1] || imagePoints[0] == imagePoints[2] || imagePoints[1] == imagePoints[2])
	{
		return 0u;
	}

	// we ensure that the points are not co-linear
	if (Line2(imagePoints[0], (imagePoints[1] - imagePoints[0]).normalized()).sqrDistance(imagePoints[2]) <= minimalCollinearSqrDistance
		|| Line2(imagePoints[0], (imagePoints[2] - imagePoints[0]).normalized()).sqrDistance(imagePoints[1]) <= minimalCollinearSqrDistance
		|| Line2(imagePoints[1], (imagePoints[2] - imagePoints[1]).normalized()).sqrDistance(imagePoints[0]) <= minimalCollinearSqrDistance)
	{
		return 0u;
	}

	const Vector3 imageRays[3] =
	{
		pinholeCamera.vector(imagePoints[0]),
		pinholeCamera.vector(imagePoints[1]),
		pinholeCamera.vector(imagePoints[2])
	};

	const unsigned int resultNumberPoses = poses(objectPoints, imageRays, world_T_cameras);

#ifdef OCEAN_INTENSIVE_DEBUG
	if (std::is_same<Scalar, double>::value)
	{
		const Scalar debugEpsilon = Scalar(5 * 5);

		const Scalar cos_ab = pinholeCamera.calculateCosBetween(imagePoints[0], imagePoints[1]);
		const Scalar cos_ac = pinholeCamera.calculateCosBetween(imagePoints[0], imagePoints[2]);
		const Scalar cos_bc = pinholeCamera.calculateCosBetween(imagePoints[1], imagePoints[2]);

		const Scalar debugAngle01 = Numeric::rad2deg(Numeric::acos(cos_ab));
		const Scalar debugAngle02 = Numeric::rad2deg(Numeric::acos(cos_ac));
		const Scalar debugAngle12 = Numeric::rad2deg(Numeric::acos(cos_bc));

		for (unsigned int n = 0u; n < resultNumberPoses; ++n)
		{
			const HomogenousMatrix4& world_T_camera = world_T_cameras[n];

			const Scalar sqrDistance0 = pinholeCamera.projectToImage<true>(world_T_camera, objectPoints[0], false).sqrDistance(imagePoints[0]);
			const Scalar sqrDistance1 = pinholeCamera.projectToImage<true>(world_T_camera, objectPoints[1], false).sqrDistance(imagePoints[1]);
			const Scalar sqrDistance2 = pinholeCamera.projectToImage<true>(world_T_camera, objectPoints[2], false).sqrDistance(imagePoints[2]);

			if (debugAngle01 > Scalar(5) && debugAngle12 > Scalar(5) && debugAngle02 > Scalar(5))
			{
				ocean_assert(sqrDistance0 <= debugEpsilon && sqrDistance1 <= debugEpsilon && sqrDistance2 <= debugEpsilon);
			}
		}
	}
#endif

	return resultNumberPoses;
}

template <typename TCamera, typename TPoint>
unsigned int P3P::poses(const AnyCameraT<TCamera>& anyCamera, const VectorT3<TPoint>* objectPoints, const VectorT2<TPoint>* imagePoints, HomogenousMatrixT4<TPoint>* world_T_cameras)
{
	/// p3p algorithm from original RANSAC paper

	ocean_assert(objectPoints != nullptr && imagePoints != nullptr && world_T_cameras != nullptr);

	const TPoint minimalSqrDistance = TPoint(2 * 2);

	if (imagePoints[0].sqrDistance(imagePoints[1]) < minimalSqrDistance || imagePoints[0].sqrDistance(imagePoints[2]) < minimalSqrDistance || imagePoints[1].sqrDistance(imagePoints[2]) < minimalSqrDistance)
	{
		return 0u;
	}

	constexpr bool makeUnitVector = false; // we normalize the image ray on our own, as the camera profile (TCamera) may have less precision than TPoint

	const VectorT3<TPoint> imageRays[3] =
	{
		VectorT3<TPoint>(anyCamera.vector(VectorT2<TCamera>(imagePoints[0]), makeUnitVector)).normalized(),
		VectorT3<TPoint>(anyCamera.vector(VectorT2<TCamera>(imagePoints[1]), makeUnitVector)).normalized(),
		VectorT3<TPoint>(anyCamera.vector(VectorT2<TCamera>(imagePoints[2]), makeUnitVector)).normalized()
	};

	const unsigned int resultNumberPoses = poses(objectPoints, imageRays, world_T_cameras);

#ifdef OCEAN_INTENSIVE_DEBUG
	if (std::is_same<TPoint, double>::value)
	{
		const TPoint debugEpsilon = TPoint(5 * 5);

		const TPoint cos_ab = TPoint(anyCamera.vector(VectorT2<TCamera>(imagePoints[0])) * anyCamera.vector(VectorT2<TCamera>(imagePoints[1])));
		const TPoint cos_ac = TPoint(anyCamera.vector(VectorT2<TCamera>(imagePoints[0])) * anyCamera.vector(VectorT2<TCamera>(imagePoints[2])));
		const TPoint cos_bc = TPoint(anyCamera.vector(VectorT2<TCamera>(imagePoints[1])) * anyCamera.vector(VectorT2<TCamera>(imagePoints[2])));

		const TPoint debugAngle01 = NumericT<TPoint>::rad2deg(NumericT<TPoint>::acos(cos_ab));
		const TPoint debugAngle02 = NumericT<TPoint>::rad2deg(NumericT<TPoint>::acos(cos_ac));
		const TPoint debugAngle12 = NumericT<TPoint>::rad2deg(NumericT<TPoint>::acos(cos_bc));

		for (unsigned int n = 0u; n < resultNumberPoses; ++n)
		{
			const HomogenousMatrixT4<TPoint>& world_T_camera = world_T_cameras[n];

			const TPoint sqrDistance0 = TPoint(anyCamera.projectToImage(HomogenousMatrixT4<TCamera>(world_T_camera), VectorT3<TCamera>(objectPoints[0])).sqrDistance(VectorT2<TCamera>(imagePoints[0])));
			const TPoint sqrDistance1 = TPoint(anyCamera.projectToImage(HomogenousMatrixT4<TCamera>(world_T_camera), VectorT3<TCamera>(objectPoints[1])).sqrDistance(VectorT2<TCamera>(imagePoints[1])));
			const TPoint sqrDistance2 = TPoint(anyCamera.projectToImage(HomogenousMatrixT4<TCamera>(world_T_camera), VectorT3<TCamera>(objectPoints[2])).sqrDistance(VectorT2<TCamera>(imagePoints[2])));

			if (debugAngle01 > TPoint(5) && debugAngle12 > TPoint(5) && debugAngle02 > TPoint(5))
			{
				ocean_assert(sqrDistance0 <= debugEpsilon && sqrDistance1 <= debugEpsilon && sqrDistance2 <= debugEpsilon);
			}
		}
	}
#endif

	return resultNumberPoses;
}

template unsigned int OCEAN_GEOMETRY_EXPORT P3P::poses<float, float>(const AnyCameraT<float>&, const VectorT3<float>*, const VectorT2<float>*, HomogenousMatrixT4<float>*);
template unsigned int OCEAN_GEOMETRY_EXPORT P3P::poses<float, double>(const AnyCameraT<float>&, const VectorT3<double>*, const VectorT2<double>*, HomogenousMatrixT4<double>*);
template unsigned int OCEAN_GEOMETRY_EXPORT P3P::poses<double, float>(const AnyCameraT<double>&, const VectorT3<float>*, const VectorT2<float>*, HomogenousMatrixT4<float>*);
template unsigned int OCEAN_GEOMETRY_EXPORT P3P::poses<double, double>(const AnyCameraT<double>&, const VectorT3<double>*, const VectorT2<double>*, HomogenousMatrixT4<double>*);

template <typename T>
unsigned int P3P::poses(const VectorT3<T>* objectPoints, const VectorT3<T>* imageRays, HomogenousMatrixT4<T>* world_T_cameras)
{
	/// p3p algorithm from original RANSAC paper

	ocean_assert(objectPoints != nullptr && imageRays != nullptr && world_T_cameras != nullptr);
	ocean_assert(imageRays[0].isUnit() && imageRays[1].isUnit() && imageRays[2].isUnit());

	// as we use an inverted but standard coordinate system (not a flipped coordinate system)
	// we expect that the z-component of each image ray is negative
	ocean_assert(imageRays[0].z() < -NumericT<T>::eps() && imageRays[1].z() < -NumericT<T>::eps() && imageRays[2].z() < -NumericT<T>::eps());

	if (objectPoints[0] == objectPoints[1] || objectPoints[0] == objectPoints[2] || objectPoints[1] == objectPoints[2]
		|| imageRays[0] == imageRays[1] || imageRays[0] == imageRays[2] || imageRays[1] == imageRays[2])
	{
		return 0u;
	}

	// now we apply the Law of cosines:
	// for a the distance between the first object point and the center of project (CP)
	// for b the distance between the second object point and CP
	// for c the distance between the third object point and CP

	// ab^2 = a^2 + b^2 - 2 a b cos_ab
	// ac^2 = a^2 + c^2 - 2 a c cos_ac
	// bc^2 = b^2 + c^2 - 2 b c cos_bc

	const T ab = (objectPoints[0] - objectPoints[1]).length();
	const T ac = (objectPoints[0] - objectPoints[2]).length();
	const T bc = (objectPoints[1] - objectPoints[2]).length();

	// if two object points are identical
	if (NumericT<T>::isEqualEps(ab) || NumericT<T>::isEqualEps(ac) || NumericT<T>::isEqualEps(bc))
	{
		return 0u;
	}

	// we define b = x a and c = y a and receive:

	// ab^2 = a^2 + x^2 a^2 - 2 a^2 x cos_ab
	// ac^2 = a^2 + y^2 a^2 - 2 a^2 y cos_ac
	// bc^2 = x^2 a^2 + y^2 a^2 - 2 a^2 x y cos_bc

	// we define k1 = bc^2 / ac^2 and k2 = bc^2 / ab^2

	const T k1 = (bc / ac) * (bc / ac);
	const T k2 = (bc / ab) * (bc / ab);

	const T cos_ab = imageRays[0] * imageRays[1]; // all rays have a unit length
	const T cos_ac = imageRays[0] * imageRays[2];
	const T cos_bc = imageRays[1] * imageRays[2];

#ifdef OCEAN_INTENSIVE_DEBUG
	const T debugAngle01 = NumericT<T>::rad2deg(NumericT<T>::acos(cos_ab));
	const T debugAngle02 = NumericT<T>::rad2deg(NumericT<T>::acos(cos_ac));
	const T debugAngle12 = NumericT<T>::rad2deg(NumericT<T>::acos(cos_bc));
#endif

	// g0 = (k1 * k2 + k1 - k2)^2 - 4.0 * k1 * k1 * k2 * cos13 * cos13;
	const T g0 = sqr(k1 * k2 + k1 - k2) - 4 * sqr(k1) * k2 * sqr(cos_ac);

	// g1 = 4.0 * (k1 * k2 + k1 - k2) * k2 * (1 - k1) * cos12 + 4.0 * k1 * (( k1 * k2 - k1 + k2) * cos13 * cos23 + 2 * k1 * k2 * cos12 * cos13 * cos13);
	const T g1 = 4 * (k1 * k2 + k1 - k2) * k2 * (1 - k1) * cos_ab + 4 * k1 * ((k1 * k2 - k1 + k2) * cos_ac * cos_bc + 2 * k1 * k2 * cos_ab * sqr(cos_ac));

	// g2 = (2.0 * k2 * (1.0 - k1) * cos12)^2 + 2.0 * (k1 * k2 + k1 - k2) * (k1 * k2 - k1 - k2) + 4.0 * k1 * ((k1 - k2) * cos23 * cos23 + (1 - k2) * k1 * cos13 * cos13 - 2 * k2 * (1.0 + k1) * cos12 * cos13  * cos23);
	const T g2 = sqr(2 * k2 * (1 - k1) * cos_ab) + 2 * (k1 * k2 + k1 - k2) * (k1 * k2 - k1 - k2) + 4 * k1 * ((k1 - k2) * sqr(cos_bc) + (1 - k2) * k1 * sqr(cos_ac) - 2 * k2 * (1 + k1) * cos_ab * cos_ac * cos_bc);

	// g3 = 4.0 * (k1 * k2 - k1 - k2) * k2 * (1.0 - k1) * cos12 + 4.0 * k1 * cos23 * ((k1 * k2 + k2 - k1) * cos13 + 2.0 * k2 * cos12 * cos23);
	const T g3 = 4 * (k1 * k2 - k1 - k2) * k2 * (1 - k1) * cos_ab + 4 * k1 * cos_bc * ((k1 * k2 + k2 - k1) * cos_ac + 2 * k2 * cos_ab * cos_bc);

	// g4 = (k1 * k2 - k1 - k2) * (k1 * k2 - k1 - k2) - 4 * k1 * k2 * cos23 * cos23;
	const T g4 = sqr(k1 * k2 - k1 - k2) - 4 * k1 * k2 * sqr(cos_bc);

	if (NumericT<T>::isEqualEps(g4))
	{
		return 0u;
	}

	T xSolutions[4];
	const unsigned int solutions = EquationT<T>::solveQuartic(g4, g3, g2, g1, g0, xSolutions);

	if (solutions == 0u)
	{
		return 0u;
	}

	unsigned int numberPoses = 0u;
	for (unsigned int n = 0; n < solutions; ++n)
	{
		// we first need to determine the distances (a, b and c) between the camera's center of projection and the 3 object points

		const T x = xSolutions[n];

		// we can stop if x is negative as a, b and c cannot be negative (b = a * x)
		if (x < 0)
		{
			continue;
		}

		const T denominator = NumericT<T>::sqrt(x * x - 2 * x * cos_ab + 1);

		if (NumericT<T>::isEqualEps(denominator))
		{
			continue;
		}

		const T a = ab / denominator;
		const T b = a * x;

		if (a < NumericT<T>::eps() || b < NumericT<T>::eps())
		{
			continue;
		}

		const T sqrValue = sqr(cos_ac) + sqr(ac / a) - 1;

		if (sqrValue < 0)
		{
			continue;
		}

		const T sqrtValue = NumericT<T>::sqrt(sqrValue);

		const T y1 = cos_ac + sqrtValue;
		const T y2 = cos_ac - sqrtValue;

		const T bc2_1 = sqr(b) + sqr(y1 * a) - 2 * b * y1 * a * cos_bc;
		const T bc2_2 = sqr(b) + sqr(y2 * a) - 2 * b * y2 * a * cos_bc;

		const T c = NumericT<T>::abs(bc * bc - bc2_1) < NumericT<T>::abs(bc * bc - bc2_2) ? y1 * a : y2 * a;

		if (c < NumericT<T>::eps())
		{
			continue;
		}

		// now we have the distances between the CP and the individual object points: a, b, and c
		// so, we determine the nearest point between CP and the line 0-1 and the point between CP and the line 0-2

		const VectorT3<T> closestPointToCP01(constructClosestPointToCP(objectPoints[0], objectPoints[1], ab, a, b));
		const VectorT3<T> closestPointToCP02(constructClosestPointToCP(objectPoints[0], objectPoints[2], ac, a, c));

		VectorT3<T> closestPointToCPOnObjectPlane;
		if (!constructClosestPointToCPOnObjectPlane(objectPoints[0], objectPoints[1], objectPoints[2], closestPointToCP01, closestPointToCP02, closestPointToCPOnObjectPlane))
		{
			continue;
		}

		const T distanceToCPOnObjectPlaneSqr = objectPoints[0].sqrDistance(closestPointToCPOnObjectPlane);

		const T v = a * a - distanceToCPOnObjectPlaneSqr;
		if (v < NumericT<T>::eps())
		{
			continue;
		}

		const T distanceOfCPToPlane = NumericT<T>::sqrt(v);

		// we do not know the direction of the plane's normal (as we do not know whether the points correspondences are counter clockwise or clockwise
		// we can identify the direction by checking whether the (normalized) image points are ccw or cw

		const VectorT2<T> normalizedImagePoint0 = imageRays[0].xy() / imageRays[0].z();
		const VectorT2<T> normalizedImagePoint1 = imageRays[1].xy() / imageRays[1].z();
		const VectorT2<T> normalizedImagePoint2 = imageRays[2].xy() / imageRays[2].z();
		const T normalSign = NumericT<T>::copySign(T(1), (normalizedImagePoint1 - normalizedImagePoint0).cross(normalizedImagePoint2 - normalizedImagePoint0));

		const VectorT3<T> objectPlaneNormal = (objectPoints[1] - objectPoints[0]).cross(objectPoints[2] - objectPoints[0]).normalizedOrZero() * normalSign;
		ocean_assert(!objectPlaneNormal.isNull());

		const VectorT3<T> CP = closestPointToCPOnObjectPlane + objectPlaneNormal * distanceOfCPToPlane;

#ifdef OCEAN_INTENSIVE_DEBUG
		{
			// we check whether we the correct CP:

			const LineT3<T> line01(objectPoints[0], (objectPoints[1] - objectPoints[0]).normalizedOrZero());
			const VectorT3<T> testClosestPointToCP01 = line01.nearestPoint(CP);
			ocean_assert(testClosestPointToCP01.isEqual(closestPointToCP01, NumericT<T>::weakEps()));

			const LineT3<T> line02(objectPoints[0], (objectPoints[2] - objectPoints[0]).normalizedOrZero());
			const VectorT3<T> testClosestPointToCP02 = line02.nearestPoint(CP);
			ocean_assert(testClosestPointToCP02.isEqual(closestPointToCP02, NumericT<T>::weakEps()));

			const PlaneT3<T> plane012(objectPoints[0], objectPoints[1], objectPoints[2]);
			const VectorT3<T> testClosestPointOnPlane = plane012.projectOnPlane(CP);
			ocean_assert(testClosestPointOnPlane.isEqual(closestPointToCPOnObjectPlane, NumericT<T>::weakEps()));

			// we check whether the CP is on the correct side of the 3D plane

			ocean_assert((CP - objectPoints[0]) * objectPlaneNormal >= 0);
			ocean_assert((CP - objectPoints[1]) * objectPlaneNormal >= 0);
			ocean_assert((CP - objectPoints[2]) * objectPlaneNormal >= 0);

			// if we have the correct CP the distance between the object points and the CP
			// must match with our calculated foot lengths: a, b, and c

			const T distance0 = CP.distance(objectPoints[0]);
			const T distance1 = CP.distance(objectPoints[1]);
			const T distance2 = CP.distance(objectPoints[2]);

			ocean_assert(NumericT<T>::isWeakEqual(distance0, a));
			ocean_assert(NumericT<T>::isWeakEqual(distance1, b));
			ocean_assert(NumericT<T>::isWeakEqual(distance2, c));
		}

		if (std::is_same<T, double>::value)
		{
			// now we check whether the angle between our rays match with the angles
			// between the rays defined by object points and CP

			const T angleVector01 = NumericT<T>::rad2deg(imageRays[0].angle(imageRays[1]));
			const T angleVector12 = NumericT<T>::rad2deg(imageRays[1].angle(imageRays[2]));
			const T angleVector20 = NumericT<T>::rad2deg(imageRays[2].angle(imageRays[0]));

			ocean_assert(NumericT<T>::isEqual(debugAngle01, angleVector01, T(0.01)));
			ocean_assert(NumericT<T>::isEqual(debugAngle12, angleVector12, T(0.01)));
			ocean_assert(NumericT<T>::isEqual(debugAngle02, angleVector20, T(0.01)));

			const VectorT3<T> direction0 = objectPoints[0] - CP;
			const VectorT3<T> direction1 = objectPoints[1] - CP;
			const VectorT3<T> direction2 = objectPoints[2] - CP;

			const T testAngle01 = NumericT<T>::rad2deg(direction0.angle(direction1));
			const T testAngle02 = NumericT<T>::rad2deg(direction0.angle(direction2));
			const T testAngle12 = NumericT<T>::rad2deg(direction1.angle(direction2));

			// we expect almost identical angles, however if the angle is smaller than 5 degree the error can be a little bit larger
			ocean_assert(NumericT<T>::isEqual(debugAngle01, testAngle01, debugAngle01 > T(5) ? T(0.1) : T(0.5)));
			ocean_assert(NumericT<T>::isEqual(debugAngle02, testAngle02, debugAngle02 > T(5) ? T(0.1) : T(0.5)));
			ocean_assert(NumericT<T>::isEqual(debugAngle12, testAngle12, debugAngle12 > T(5) ? T(0.1) : T(0.5)));
		}
#endif

		VectorT3<T> foot0 = objectPoints[0] - CP;
		VectorT3<T> foot1 = objectPoints[1] - CP;
		VectorT3<T> foot2 = objectPoints[2] - CP;

#ifdef OCEAN_INTENSIVE_DEBUG
		if (std::is_same<T, double>::value)
		{
			// we check the foot lengths again
			ocean_assert(NumericT<T>::isWeakEqual(foot0.length(), NumericT<T>::abs(a)));
			ocean_assert(NumericT<T>::isWeakEqual(foot1.length(), NumericT<T>::abs(b)));
			ocean_assert(NumericT<T>::isWeakEqual(foot2.length(), NumericT<T>::abs(c)));
		}
#endif

		ocean_assert(NumericT<T>::isNotEqualEps(a));
		ocean_assert(NumericT<T>::isNotEqualEps(b));
		ocean_assert(NumericT<T>::isNotEqualEps(c));
		ocean_assert(!foot0.isNull() && !foot1.isNull() && !foot2.isNull());

		// we avoid to normalize the feet, we use the lengths we have already calculated

		foot0 /= a;
		foot1 /= b;
		foot2 /= c;

#ifdef OCEAN_INTENSIVE_DEBUG
		if (std::is_same<T, double>::value)
		{
			ocean_assert(NumericT<T>::isWeakEqual(foot0.length(), 1));
			ocean_assert(NumericT<T>::isWeakEqual(foot1.length(), 1));
			ocean_assert(NumericT<T>::isWeakEqual(foot2.length(), 1));

			const T foot01 = NumericT<T>::rad2deg(foot0.angle(foot1));
			const T foot12 = NumericT<T>::rad2deg(foot1.angle(foot2));
			const T foot20 = NumericT<T>::rad2deg(foot2.angle(foot0));

			const T vector01 = NumericT<T>::rad2deg(imageRays[0].angle(imageRays[1]));
			const T vector12 = NumericT<T>::rad2deg(imageRays[1].angle(imageRays[2]));
			const T vector20 = NumericT<T>::rad2deg(imageRays[2].angle(imageRays[0]));

			// we expect almost identical angles, however if the angle is smaller than 5 degree the error can be a little bit larger
			ocean_assert(NumericT<T>::isEqual(foot01, vector01, debugAngle01 > T(5) ? T(0.1) : T(0.5)));
			ocean_assert(NumericT<T>::isEqual(foot12, vector12, debugAngle12 > T(5) ? T(0.1) : T(0.5)));
			ocean_assert(NumericT<T>::isEqual(foot20, vector20, debugAngle02 > T(5) ? T(0.1) : T(0.5)));
		}
#endif

		// now we have the CP and we have three feet with unit length
		// we finally have to rotate the feet so that they match with the rays defined as function parameters

		// thus, we seek for an orthonormal transformation T that holds for:
		// feet0 = T * imageRays0
		// foot1 = T * imageRays1
		// foot2 = T * imageRays2

		// so that we get
		// [foot0 | foot1 | foot2] = T * [imageRays0 | imageRays1 | imageRays2]

		// therefore, we simply need to solve the equation for T:

		SquareMatrixT3<T> invImageRayMatrix;
		if (!SquareMatrixT3<T>(imageRays[0], imageRays[1], imageRays[2]).invert(invImageRayMatrix))
		{
			continue;
		}

		const SquareMatrixT3<T> overallRotation((SquareMatrixT3<T>(foot0, foot1, foot2) * invImageRayMatrix).orthonormalMatrix());

#ifdef OCEAN_INTENSIVE_DEBUG
		if (std::is_same<T, double>::value)
		{
			const T angle0 = NumericT<T>::rad2deg((overallRotation * imageRays[0]).angle(foot0));
			const T angle1 = NumericT<T>::rad2deg((overallRotation * imageRays[1]).angle(foot1));
			const T angle2 = NumericT<T>::rad2deg((overallRotation * imageRays[2]).angle(foot2));

			ocean_assert(angle0 < T(0.1) && angle1 < T(0.1));
			ocean_assert(angle2 < ((debugAngle01 > T(5) && debugAngle12 > T(5) && debugAngle02 > T(5)) ? T(0.1) : T(0.5)));
		}
#endif

		const HomogenousMatrixT4<T> world_T_camera(CP, overallRotation);

		world_T_cameras[numberPoses++] = world_T_camera;
	}

	return numberPoses;
}

template unsigned int OCEAN_GEOMETRY_EXPORT P3P::poses<float>(const VectorT3<float>*, const VectorT3<float>*, HomogenousMatrixT4<float>*);
template unsigned int OCEAN_GEOMETRY_EXPORT P3P::poses<double>(const VectorT3<double>*, const VectorT3<double>*, HomogenousMatrixT4<double>*);

}

}
