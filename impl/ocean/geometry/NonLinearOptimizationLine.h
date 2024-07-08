/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_LINE_H
#define META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_LINE_H

#include "ocean/geometry/Geometry.h"
#include "ocean/geometry/NonLinearOptimization.h"

#include "ocean/math/Line2.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements least square or robust optimization algorithms optimizing lines.
 * @ingroup geometry
 */
class OCEAN_GEOMETRY_EXPORT NonLinearOptimizationLine : protected NonLinearOptimization
{
	protected:

		/**
		 * Forward declaration of a data object allowing to optimize a 2D plane.
		 */
		class LineData;

	public:

		/**
		 * Optimizes a 2D line by reducing the distance between image points and their projected line points.
		 * @param line Line that has to be optimized
		 * @param pointAccessor The accessor providing the 2D points that define the 2D line, at least two points
		 * @param optimizedLine Resulting optimized line with reduced (optimized) distance between image points and projected line points
		 * @param iterations Number of iterations to be applied at most, if no convergence can be reached in the meantime, with range [1, infinity)
		 * @param estimator Robust error estimator to be used
		 * @param lambda Initial Levenberg-Marquardt damping value which may be changed after each iteration using the damping factor, with range [0, infinity)
		 * @param lambdaFactor Levenberg-Marquardt damping factor to be applied to the damping value, with range [1, infinity)
		 * @param initialError Optional resulting averaged pixel error for the given initial parameters, w.r.t. the defined estimator
		 * @param finalError Optional resulting averaged pixel error for the final optimized parameters, w.r.t. the defined estimator
		 * @return Result of the optimization
		 */
		static bool optimizeLine(const Line2& line, const ConstIndexedAccessor<ImagePoint>& pointAccessor, Line2& optimizedLine, const unsigned int iterations = 20u, const Estimator::EstimatorType estimator = Estimator::ET_SQUARE, Scalar lambda = Scalar(0.001), const Scalar lambdaFactor = Scalar(5), Scalar* initialError = nullptr, Scalar* finalError = nullptr);
};

}

}

#endif // META_OCEAN_GEOMETRY_NON_LINEAR_OPTIMIZATION_LINE_H
