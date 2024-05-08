/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/PositionTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

PositionTracker3DOF::PositionTracker3DOFSample::PositionTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Positions& positions, const Metadata& metadata) :
	Sample(timestamp, objectIds, metadata),
	TrackerSample(timestamp, referenceSystem, objectIds, metadata),
	positions_(positions)
{
	// nothing to do here
}

PositionTracker3DOF::PositionTracker3DOFSample::PositionTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Positions&& positions, Metadata&& metadata) :
	Sample(timestamp, std::move(objectIds), std::move(metadata)),
	TrackerSample(timestamp, referenceSystem, std::move(objectIds), std::move(metadata)),
	positions_(std::move(positions))
{
	// nothing to do here
}

PositionTracker3DOF::PositionTracker3DOF(const std::string& name) :
	Device(name, deviceTypePositionTracker3DOF()),
	Measurement(name, deviceTypePositionTracker3DOF()),
	Tracker(name, deviceTypePositionTracker3DOF())
{
	// nothing to do here
}

PositionTracker3DOF::~PositionTracker3DOF()
{
	// nothing to do here
}

}

}
