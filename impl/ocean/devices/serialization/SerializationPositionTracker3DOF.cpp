/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/SerializationPositionTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

SerializationPositionTracker3DOF::SerializationPositionTracker3DOF(const std::string& name) :
	Device(name, deviceTypeSerializationPositionTracker3DOF()),
	Measurement(name, deviceTypeSerializationPositionTracker3DOF()),
	Tracker(name, deviceTypeSerializationPositionTracker3DOF()),
	PositionTracker3DOF(name),
	SerializationTracker(name, deviceTypeSerializationPositionTracker3DOF())
{
	// nothing to do here
}

SerializationPositionTracker3DOF::~SerializationPositionTracker3DOF()
{
	// nothing to do here
}

void SerializationPositionTracker3DOF::forwardSampleEvent(PositionTracker3DOFSample::Positions&& positions, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata)
{
	ocean_assert(isStarted_);

	if (!isStarted_)
	{
		return;
	}

	ObjectIds objectIds;
	objectIds.reserve(positions.size());

	for (size_t n = 0; n < positions.size(); ++n)
	{
		objectIds.emplaceBack(ObjectId(n));
	}

	postNewSample(SampleRef(new PositionTracker3DOFSample(timestamp, referenceSystem, std::move(objectIds), std::move(positions), std::move(metadata))));
}

}

}

}
