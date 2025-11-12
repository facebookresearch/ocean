/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/GravityTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

GravityTracker3DOF::GravityTracker3DOFSample::GravityTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Gravities& gravities, const Metadata& metadata) :
	Sample(timestamp, objectIds, metadata),
	TrackerSample(timestamp, referenceSystem, objectIds, metadata),
	gravities_(gravities)
{
	// nothing to do here
}

GravityTracker3DOF::GravityTracker3DOFSample::GravityTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Gravities&& gravities, Metadata&& metadata) :
	Sample(timestamp, std::move(objectIds), std::move(metadata)),
	TrackerSample(timestamp, referenceSystem, std::move(objectIds), std::move(metadata)),
	gravities_(std::move(gravities))
{
	// nothing to do here
}

GravityTracker3DOF::GravityTracker3DOF(const std::string& name) :
	Device(name, deviceTypeGravityTracker3DOF()),
	Measurement(name, deviceTypeGravityTracker3DOF()),
	Tracker(name, deviceTypeGravityTracker3DOF())
{
	// nothing to do here
}

GravityTracker3DOF::~GravityTracker3DOF()
{
	// nothing to do here
}

}

}
