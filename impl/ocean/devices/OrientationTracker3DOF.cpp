/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/OrientationTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

OrientationTracker3DOF::OrientationTracker3DOFSample::OrientationTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Orientations& orientations, const Metadata& metadata) :
	Sample(timestamp, objectIds, metadata),
	TrackerSample(timestamp, referenceSystem, objectIds, metadata),
	orientations_(orientations)
{
	// nothing to do here
}

OrientationTracker3DOF::OrientationTracker3DOFSample::OrientationTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Orientations&& orientations, Metadata&& metadata) :
	Sample(timestamp, std::move(objectIds), std::move(metadata)),
	TrackerSample(timestamp, referenceSystem, std::move(objectIds), std::move(metadata)),
	orientations_(std::move(orientations))
{
	// nothing to do here
}

OrientationTracker3DOF::OrientationTracker3DOF(const std::string& name) :
	Device(name, deviceTypeOrientationTracker3DOF()),
	Measurement(name, deviceTypeOrientationTracker3DOF()),
	Tracker(name, deviceTypeOrientationTracker3DOF())
{
	// nothing to do here
}

OrientationTracker3DOF::~OrientationTracker3DOF()
{
	// nothing to do here
}

}

}
