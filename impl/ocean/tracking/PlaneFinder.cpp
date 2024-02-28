// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/PlaneFinder.h"

#include "ocean/geometry/NonLinearOptimization.h"
#include "ocean/geometry/Utilities.h"

namespace Ocean
{

namespace Tracking
{

bool PlaneFinder::addImagePoint(const Vectors2& imagePoints)
{
	return imagePointCorrespondences.addElements(imagePoints);
}

bool PlaneFinder::addImagePoint(Vectors2&& imagePoints)
{
	return imagePointCorrespondences.addElements(imagePoints);
}

bool PlaneFinder::addImagePoint(const Vectors2& imagePoints, const Indices32& validIndices)
{
	return imagePointCorrespondences.addElements(imagePoints, validIndices);
}

}

}
