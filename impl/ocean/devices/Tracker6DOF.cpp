/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/Tracker6DOF.h"

#include "ocean/math/Interpolation.h"

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

Measurement::SampleRef Tracker6DOF::interpolateSamples(const SampleRef& lowerSample, const SampleRef& upperSample, const double interpolationFactor, const Timestamp& interpolatedTimestamp) const
{
	ocean_assert(lowerSample && upperSample);
	ocean_assert(interpolationFactor >= 0.0 && interpolationFactor <= 1.0);

	const Tracker6DOFSampleRef lower6DOFSample = lowerSample;
	const Tracker6DOFSampleRef upper6DOFSample = upperSample;

	ocean_assert(lower6DOFSample && upper6DOFSample);

	ocean_assert(lower6DOFSample->positions().size() == upper6DOFSample->positions().size());
	ocean_assert(lower6DOFSample->orientations().size() == upper6DOFSample->orientations().size());
	ocean_assert(lower6DOFSample->referenceSystem() == upper6DOFSample->referenceSystem());

	Vectors3 interpolatedPositions(lower6DOFSample->positions().size());
	Quaternions interpolatedOrientations(lower6DOFSample->positions().size());

	for (size_t n = 0; n < lower6DOFSample->positions().size(); ++n)
	{
		interpolatedPositions[n] = Interpolation::linear(lower6DOFSample->positions()[n], upper6DOFSample->positions()[n], Scalar(interpolationFactor));
		interpolatedOrientations[n] = Interpolation::linear(lower6DOFSample->orientations()[n], upper6DOFSample->orientations()[n], Scalar(interpolationFactor));
	}

	return Tracker6DOFSampleRef(new Tracker6DOFSample(interpolatedTimestamp, lower6DOFSample->referenceSystem(), lower6DOFSample->objectIds(), interpolatedOrientations, interpolatedPositions));
}

}

}
