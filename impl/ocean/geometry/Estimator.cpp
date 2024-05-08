/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/geometry/Estimator.h"

namespace Ocean
{

namespace Geometry
{

std::string Estimator::translateEstimatorType(const EstimatorType estimatorType)
{
	switch (estimatorType)
	{
		case ET_INVALID:
			return std::string("Invalid");

		case ET_SQUARE:
			return std::string("Square");

		case ET_LINEAR:
			return std::string("Linear");

		case ET_HUBER:
			return std::string("Huber");

		case ET_TUKEY:
			return std::string("Tukey");

		case ET_CAUCHY:
			return std::string("Cauchy");
	}

	ocean_assert(false && "Unknown estimator type!");
	return std::string("Invalid");
}

Estimator::EstimatorType Estimator::translateEstimatorType(const std::string& estimatorType)
{
	const std::string& lowerEstimatorType = String::toLower(estimatorType);

	if (lowerEstimatorType == "invalid")
	{
		return ET_INVALID;
	}

	if (lowerEstimatorType == "square")
	{
		return ET_SQUARE;
	}

	if (lowerEstimatorType == "linear")
	{
		return ET_LINEAR;
	}

	if (lowerEstimatorType == "huber")
	{
		return ET_HUBER;
	}

	if (lowerEstimatorType == "tukey")
	{
		return ET_TUKEY;
	}

	if (lowerEstimatorType == "cauchy")
	{
		return ET_CAUCHY;
	}

	ocean_assert(false && "Invalid estimator type!");
	return ET_INVALID;
}

const Estimator::EstimatorTypes& Estimator::estimatorTypes()
{
	static EstimatorTypes estimatortypes =
	{
		ET_SQUARE,
		ET_LINEAR,
		ET_HUBER,
		ET_TUKEY,
		ET_CAUCHY
	};

	return estimatortypes;
}

}

}
