/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/SerializationOrientationTracker3DOF.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

SerializationOrientationTracker3DOF::SerializationOrientationTracker3DOF(const std::string& name) :
	Device(name, deviceTypeSerializationOrientationTracker3DOF()),
	Measurement(name, deviceTypeSerializationOrientationTracker3DOF()),
	Tracker(name, deviceTypeSerializationOrientationTracker3DOF()),
	OrientationTracker3DOF(name),
	SerializationTracker(name, deviceTypeSerializationOrientationTracker3DOF())
{
	// nothing to do here
}

SerializationOrientationTracker3DOF::~SerializationOrientationTracker3DOF()
{
	// nothing to do here
}

void SerializationOrientationTracker3DOF::forwardSampleEvent(OrientationTracker3DOFSample::Orientations&& orientations, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata)
{
	ocean_assert(isStarted_);

	if (!isStarted_)
	{
		return;
	}

	ObjectIds objectIds;
	objectIds.reserve(orientations.size());

	for (size_t n = 0; n < orientations.size(); ++n)
	{
		objectIds.emplaceBack(ObjectId(n));
	}

	postNewSample(SampleRef(new OrientationTracker3DOFSample(timestamp, referenceSystem, std::move(objectIds), std::move(orientations), std::move(metadata))));
}

}

}

}
