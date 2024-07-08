/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/NonLinearOptimizationLine.h"
#include "ocean/geometry/NonLinearUniversalOptimizationDense.h"

namespace Ocean
{

namespace Geometry
{

/**
 * This class implements a data object necessary to optimize a 2D line.
 * The internal model for the optimization is composed of one angular value defining the orientation of the line's normal and one scalar value defining the distance of the line to the origin.<br>
 * The internal model has the following two scalar values: normal-angle, distance.<br>
 * The external model has the following four scalar values: point-x, point-y, direction-x, direction-y (with a point lying on the line and the direction of the line).
 */
class NonLinearOptimizationLine::LineData
{
	public:

		/**
		 * Creates a new line data object.
		 * @param imagePoints The accessor for the image points
		 */
		explicit LineData(const ConstIndexedAccessor<ImagePoint>& imagePoints) :
			imagePoints_(imagePoints)
		{
			// nothing to do here
		}

		/**
		 * Determines the value for a given model and measurement.
		 * @param externalModel The external model for which a specific projected image point will be determined
		 * @param index The index of the image point for which the projected line point will be determined
		 * @param result The resulting projected 2D image point lying on the line
		 */
		void value(const StaticBuffer<Scalar, 4>& externalModel, const size_t index, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2 projectedImagePoint(projectOnLine(externalModel, index));

			result[0] = projectedImagePoint[0];
			result[1] = projectedImagePoint[1];
		}

		/**
		 * Determines the error for a given model and measurement.
		 * @param externalModel The external model for which a specific error will be determined
		 * @param index The index of the image point for which the error will be determined
		 * @param result The resulting error for each axis
		 */
		bool error(const StaticBuffer<Scalar, 4>& externalModel, const size_t index, StaticBuffer<Scalar, 2>& result)
		{
			const Vector2 projectedImagePoint(projectOnLine(externalModel, index));

			const Vector2& measurementImagePoint = imagePoints_[index];
			const Vector2 error(projectedImagePoint - measurementImagePoint);

			result[0] = error[0];
			result[1] = error[1];
			return true;
		}

		/**
		 * Transforms the internal model to a corresponding external model.
		 * @param internalModel The internal model which will be converted
		 * @param externalModel The resulting external model
		 */
		void transformModel(StaticBuffer<Scalar, 2>& internalModel, StaticBuffer<Scalar, 4>& externalModel)
		{
			const Line2 line(internalModel[0], internalModel[1]);

			externalModel[0] = line.point().x();
			externalModel[1] = line.point().y();
			externalModel[2] = line.direction().x();
			externalModel[3] = line.direction().y();
		}

	protected:

		/**
		 * Projects a 2D image point onto the 2D line defined by the external model.
		 * @param externalModel The external model defining the plane
		 * @param index The index of the 2D image point which will be projected
		 * @return The resulting projected 2D image point
		 */
		Vector2 projectOnLine(const StaticBuffer<Scalar, 4>& externalModel, const size_t index)
		{
			const Line2 line(Vector2(externalModel[0], externalModel[1]), Vector2(externalModel[2], externalModel[3]));
			ocean_assert(line.isValid());

			return line.nearestPoint(imagePoints_[index]);
		}

	protected:

		/// The 2D image points defining the 2D line.
		const ConstIndexedAccessor<ImagePoint>& imagePoints_;
};

bool NonLinearOptimizationLine::optimizeLine(const Line2& line, const ConstIndexedAccessor<ImagePoint>& pointAccessor, Line2& optimizedLine, const unsigned int iterations, const Estimator::EstimatorType estimator, Scalar lambda, const Scalar lambdaFactor, Scalar* initialError, Scalar* finalError)
{
	ocean_assert(line.isValid() && pointAccessor.size() >= 2);

	typedef NonLinearUniversalOptimizationDense<2, 2, 4> UniversalOptimization;

	LineData lineData(pointAccessor);

	UniversalOptimization::Model model;
	UniversalOptimization::Model optimizedModel;

	line.decomposeAngleDistance(model[0], model[1]);

	if (!UniversalOptimization::optimizeUniversalModel(model, pointAccessor.size(),
			UniversalOptimization::ValueCallback::create(lineData, &LineData::value),
			UniversalOptimization::ErrorCallback::create(lineData, &LineData::error),
			UniversalOptimization::ModelTransformationCallback::create(lineData, &LineData::transformModel),
			UniversalOptimization::ModelAdjustmentCallback(),
			optimizedModel, iterations, estimator, lambda, lambdaFactor, initialError, finalError))
	{
		return false;
	}

	optimizedLine = Line2(optimizedModel[0], optimizedModel[1]);
	return true;
}

}

}
