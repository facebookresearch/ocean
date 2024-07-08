/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/MotionModel.h"

namespace Ocean
{

namespace Tracking
{

MotionModel::MotionModel() :
	modelHasPose(false)
{
	// nothing to do here
}

MotionModel::MotionModel(const HomogenousMatrix4& transformation) :
	modelPose(transformation),
	modelPredictedPose(modelPose),
	modelHasPose(true)
{
	ocean_assert(modelPose == modelPredictedPose);
}

MotionModel::MotionModel(const Vector3& translation, const Quaternion& orientation) :
	modelPose(translation, orientation),
	modelPredictedPose(modelPose),
	modelHasPose(true)
{
	ocean_assert(modelPose == modelPredictedPose);
}

MotionModel::MotionModel(const Vector3& translation, const Rotation& orientation) :
	modelPose(translation, orientation),
	modelPredictedPose(modelPose),
	modelHasPose(true)
{
	ocean_assert(modelPose == modelPredictedPose);
}

void MotionModel::update(const HomogenousMatrix4& pose)
{
	if (modelHasPose)
	{
		const HomogenousMatrix4& worldTold = modelPose;
		const HomogenousMatrix4& worldTnew = pose;

		const HomogenousMatrix4 oldTworld(worldTold.inverted());

		// calculate the real recent motion (offset from the real previous pose to the real current pose)
		const HomogenousMatrix4 oldTnew(oldTworld * worldTnew);

		const Pose currentMotion(oldTnew);

		const Pose predictedMotion((modelVelocity * Scalar(0.55) + currentMotion * Scalar(0.45)) * Scalar(0.9));
		const HomogenousMatrix4 predictedMotionTransformation(predictedMotion.transformation());

		modelPose = pose;
		modelPredictedPose = pose * predictedMotionTransformation;

		modelVelocity = currentMotion;
		modelPredictedVelocity = predictedMotion;
	}
	else
	{
		modelPose = pose;
		modelPredictedPose = pose;

		modelHasPose = true;
	}
}

void MotionModel::reset()
{
	modelHasPose = false;
	modelVelocity = Pose();
	modelPredictedVelocity = Pose();
}

HomogenousMatrix4 MotionModel::predictPose(const HomogenousMatrices4& previousPoses, const Scalar factor)
{
	ocean_assert(previousPoses.size() >= 2);

	if (previousPoses.size() == 1)
		return previousPoses.front();

	HomogenousMatrices4 predictedPoses;
	predictedPoses.reserve(previousPoses.size() - 1);

	for (size_t n = previousPoses.size() - 1; n != 0; n--)
	{
		ocean_assert(previousPoses[n].isValid());
		ocean_assert(previousPoses[0].isValid());

		const HomogenousMatrix4 offsetTransformation(previousPoses[n].inverted() * previousPoses[0]);
		const HomogenousMatrix4 predictedPose(previousPoses[n - 1] * offsetTransformation);

		predictedPoses.push_back(predictedPose);
	}

	ocean_assert(predictedPoses.size() >= 1);

	if (predictedPoses.size() == 1)
		return predictedPoses.front();

	HomogenousMatrix4 predictedPose(interpolate(predictedPoses[0], predictedPoses[1], factor));

	for (size_t n = 2; n < predictedPoses.size(); ++n)
		predictedPose = interpolate(predictedPose, predictedPoses[n], factor);

	return predictedPose;
}

}

}
