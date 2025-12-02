/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/SerializationGravityTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

SerializationGravityTracker3DOF::SerializationGravityTracker3DOF(const std::string& name) :
	Device(name, deviceTypeSerializationGravityTracker3DOF()),
	Measurement(name, deviceTypeSerializationGravityTracker3DOF()),
	Tracker(name, deviceTypeSerializationGravityTracker3DOF()),
	GravityTracker3DOF(name),
	SerializationTracker(name, deviceTypeSerializationGravityTracker3DOF())
{
	// nothing to do here
}

SerializationGravityTracker3DOF::~SerializationGravityTracker3DOF()
{
	// nothing to do here
}

void SerializationGravityTracker3DOF::forwardSampleEvent(GravityTracker3DOFSample::Gravities&& gravities, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata)
{
	ocean_assert(isStarted_);

	if (!isStarted_)
	{
		return;
	}

	ObjectIds objectIds;
	objectIds.reserve(gravities.size());

	for (size_t n = 0; n < gravities.size(); ++n)
	{
		objectIds.emplaceBack(ObjectId(n));
	}

	postNewSample(SampleRef(new GravityTracker3DOFSample(timestamp, referenceSystem, std::move(objectIds), std::move(gravities), std::move(metadata))));
}

}

}

}
