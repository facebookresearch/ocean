// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/tracking/point/PosePointPair.h"

#include "ocean/geometry/NonLinearOptimizationObjectPoint.h"

namespace Ocean
{

namespace Tracking
{

namespace Point
{

bool PosePointPairsObject::optimizeObjectPoint(Database& database, const PinholeCamera& pinholeCamera, const bool useCameraDistortionParameters)
{
	HomogenousMatrices4 posesIF;
	Vectors2 imagePoints;
	Vector3 objectPoint;

	extractObjectIF(database, posesIF, imagePoints, objectPoint);

	const AnyCameraPinhole anyCameraPinhole(PinholeCamera(pinholeCamera, useCameraDistortionParameters && pinholeCamera.hasDistortionParameters()));

	Vector3 optimizedObjectPoint;
	if (!Geometry::NonLinearOptimizationObjectPoint::optimizeObjectPointForFixedPosesIF(anyCameraPinhole, ConstArrayAccessor<HomogenousMatrix4>(posesIF), objectPoint, ConstArrayAccessor<ImagePoint>(imagePoints), optimizedObjectPoint, 10u))
	{
		return false;
	}

	database.setObjectPoint<false>(dataObjectId, optimizedObjectPoint);

	return true;
}

}

}

}
