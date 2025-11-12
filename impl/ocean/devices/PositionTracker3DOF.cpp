/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/PositionTracker3DOF.h"

#include "ocean/math/Interpolation.h"

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

Measurement::SampleRef PositionTracker3DOF::interpolateSamples(const SampleRef& lowerSample, const SampleRef& upperSample, const double interpolationFactor, const Timestamp& interpolatedTimestamp) const
{
	ocean_assert(lowerSample && upperSample);
	ocean_assert(interpolationFactor >= 0.0 && interpolationFactor <= 1.0);

	const PositionTracker3DOFSampleRef lowerPositionSample = lowerSample;
	const PositionTracker3DOFSampleRef upperPositionSample = upperSample;

	ocean_assert(lowerPositionSample && upperPositionSample);

	ocean_assert(lowerPositionSample->positions().size() == upperPositionSample->positions().size());
	ocean_assert(lowerPositionSample->referenceSystem() == upperPositionSample->referenceSystem());

	Vectors3 interpolatedPositions(lowerPositionSample->positions().size());

	for (size_t n = 0; n < lowerPositionSample->positions().size(); ++n)
	{
		interpolatedPositions[n] = Interpolation::linear(lowerPositionSample->positions()[n], upperPositionSample->positions()[n], Scalar(interpolationFactor));
	}

	return PositionTracker3DOFSampleRef(new PositionTracker3DOFSample(interpolatedTimestamp, lowerPositionSample->referenceSystem(), lowerPositionSample->objectIds(), interpolatedPositions));
}

}

}
