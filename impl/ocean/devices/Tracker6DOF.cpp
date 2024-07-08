/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/Tracker6DOF.h"

namespace Ocean
{

namespace Devices
{

Tracker6DOF::Tracker6DOFSample::Tracker6DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Orientations& orientations, const Positions& positions, const Metadata& metadata) :
	Sample(timestamp, objectIds, metadata),
	TrackerSample(timestamp, referenceSystem, objectIds, metadata),
	OrientationTracker3DOFSample(timestamp, referenceSystem, objectIds, orientations, metadata),
	PositionTracker3DOFSample(timestamp, referenceSystem, objectIds, positions, metadata)
{
	// nothing to do here
}

Tracker6DOF::Tracker6DOFSample::Tracker6DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Orientations&& orientations, Positions&& positions, Metadata&& metadata) :
	Sample(timestamp, std::move(objectIds), std::move(metadata)),
	TrackerSample(timestamp, referenceSystem, std::move(objectIds), std::move(metadata)),
	OrientationTracker3DOFSample(timestamp, referenceSystem, std::move(objectIds), std::move(orientations), std::move(metadata)),
	PositionTracker3DOFSample(timestamp, referenceSystem, std::move(objectIds), std::move(positions), std::move(metadata))
{
	// nothing to do here
}

Tracker6DOF::Tracker6DOF(const std::string& name) :
	Device(name, deviceTypeTracker6DOF()),
	Measurement(name, deviceTypeTracker6DOF()),
	Tracker(name, deviceTypeTracker6DOF()),
	OrientationTracker3DOF(name),
	PositionTracker3DOF(name)
{
	// nothing to do here
}

Tracker6DOF::~Tracker6DOF()
{
	// nothing to do here
}

}

}
