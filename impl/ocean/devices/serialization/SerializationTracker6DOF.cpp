/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/SerializationTracker6DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

SerializationTracker6DOF::SerializationTracker6DOF(const std::string& name) :
	Device(name, deviceTypeSerializationTracker6DOF()),
	Measurement(name, deviceTypeSerializationTracker6DOF()),
	Tracker(name, deviceTypeSerializationTracker6DOF()),
	OrientationTracker3DOF(name),
	PositionTracker3DOF(name),
	Tracker6DOF(name),
	SerializationTracker(name, deviceTypeSerializationTracker6DOF())
{
	// nothing to do here
}

SerializationTracker6DOF::~SerializationTracker6DOF()
{
	// nothing to do here
}

void SerializationTracker6DOF::forwardSampleEvent(Tracker6DOFSample::Orientations&& orientations, Tracker6DOFSample::Positions&& positions, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata)
{
	ocean_assert(isStarted_);

	if (!isStarted_)
	{
		return;
	}

	ocean_assert(orientations.size() == positions.size());

	ObjectIds objectIds;
	objectIds.reserve(orientations.size());

	for (size_t n = 0; n < orientations.size(); ++n)
	{
		objectIds.emplaceBack(ObjectId(n));
	}

	postNewSample(SampleRef(new Tracker6DOFSample(timestamp, referenceSystem, std::move(objectIds), std::move(orientations), std::move(positions), std::move(metadata))));
}

}

}

}
