/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_VANISHING_PROJECTION_H
#define META_OCEAN_GEOMETRY_VANISHING_PROJECTION_H

#include "ocean/geometry/Geometry.h"

#include "ocean/base/Accessor.h"

#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements functions for vanishing projections.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT VanishingProjection
{
	public:

		/**
		 * Calculates the normal of a plane that is determined by four given image points defining a parallelogram and all lying on the same 3D plane.
		 * The resulting normal is defined in the coordinate system of the camera which is located in the origin and locking along the negative z-axis, with y-axis as up vector.<br>
		 * @param pinholeCamera The pinhole camera profile to be applied
		 * @param parallelogramPoints The four corners of the parallelogram visible in the camera frame
		 * @param undistortImagePoints True, so that the given image points will be undistorted before the normal is calculated
		 * @param normal Resulting normal of the plane
		 * @return True, if succeeded
		 */
		static bool planeNormal(const PinholeCamera& pinholeCamera, const Vector2 parallelogramPoints[4], const bool undistortImagePoints, Vector3& normal);

		/**
		 * Determines the vanishing line for four given (undistorted) image points defining a parallelogram that lies in a 3D plane.
		 * @param parallelogramPoints Four (undistorted) image points defining a parallelogram
		 * @param line Resulting vanishing line
		 * @return True, if succeeded
		 */
		static bool vanishingLine(const Vector2 parallelogramPoints[4], Line2& line);

		/**
		 * Determines the largest subset of perspectively parallel lines from a set of given infinite lines.
		 * The lines may be absolute parallel or may have a common vanishing point.
		 * @param lines The lines from which the largest group of (perspectively) parallel lines will be determined
		 * @param projectiveParallelAngle The maximal angle between two projected lines (actually the given lines interpreted in the coordinate system of the frame) count as parallel, in radian with range [0, PI/2)
		 * @param orientationError The expected orientation error each provided line can have e.g., due to measurement/detection inaccuracies, in radian with range [0, PI/2)
		 * @param maximalAngle The maximal angle between two lines so that they still count as perspectively parallel, in radian with range (max(orientationError, projectiveParallelAngle), PI/2)
		 * @return The indices of the requested lines out of the given group of lines
		 */
		static Indices32 perspectiveParallelLines(const ConstIndexedAccessor<Line2>& lines, const Scalar projectiveParallelAngle = Numeric::deg2rad(5), const Scalar orientationError = Numeric::deg2rad(2), const Scalar maximalAngle = Numeric::deg2rad(35));
};

}

}

#endif // META_OCEAN_GEOMETRY_VANISHING_PROJECTION_H
