/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/tracking/PoseProjection.h"

#include "ocean/geometry/Error.h"

#include <algorithm>

namespace Ocean
{

namespace Tracking
{

PoseProjection::PoseProjection(const HomogenousMatrix4& world_T_camera, const AnyCamera& camera, const Vector3* objectPoints, const size_t number) :
	world_T_camera_(world_T_camera),
	imagePoints_(number)
{
	ocean_assert(objectPoints != nullptr);

	if (number != 0)
	{
		camera.projectToImage(world_T_camera, objectPoints, number, imagePoints_.data());
	}
}

PoseProjectionSet::~PoseProjectionSet()
{
	// nothing to do here
}

void PoseProjectionSet::clear()
{
	poseProjections_.clear();
}

}

}
