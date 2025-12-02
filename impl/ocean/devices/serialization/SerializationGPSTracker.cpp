/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/SerializationGPSTracker.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

SerializationGPSTracker::SerializationGPSTracker(const std::string& name) :
	Device(name, deviceTypeSerializationGPSTracker()),
	Measurement(name, deviceTypeSerializationGPSTracker()),
	Tracker(name, deviceTypeSerializationGPSTracker()),
	GPSTracker(name),
	SerializationTracker(name, deviceTypeSerializationGPSTracker())
{
	// nothing to do here
}

SerializationGPSTracker::~SerializationGPSTracker()
{
	// nothing to do here
}

void SerializationGPSTracker::forwardSampleEvent(GPSTracker::Locations&& locations, const ReferenceSystem referenceSystem, const Timestamp& timestamp, Metadata&& metadata)
{
	ocean_assert(isStarted_);

	if (!isStarted_)
	{
		return;
	}

	ObjectIds objectIds;
	objectIds.reserve(locations.size());

	for (size_t n = 0; n < locations.size(); ++n)
	{
		objectIds.emplaceBack(ObjectId(n));
	}

	postNewSample(SampleRef(new GPSTrackerSample(timestamp, referenceSystem, std::move(objectIds), std::move(locations), std::move(metadata))));
}

}

}

}
