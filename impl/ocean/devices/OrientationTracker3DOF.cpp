/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/OrientationTracker3DOF.h"

#include "ocean/math/Interpolation.h"

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
	TrackerSample(timestamp, referenceSystem, ObjectIds(), Metadata()),
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

Measurement::SampleRef OrientationTracker3DOF::interpolateSamples(const SampleRef& lowerSample, const SampleRef& upperSample, const double interpolationFactor, const Timestamp& interpolatedTimestamp) const
{
	ocean_assert(lowerSample && upperSample);
	ocean_assert(interpolationFactor >= 0.0 && interpolationFactor <= 1.0);

	const OrientationTracker3DOFSampleRef lowerOrientationSample = lowerSample;
	const OrientationTracker3DOFSampleRef upperOrientationSample = upperSample;

	ocean_assert(lowerOrientationSample && upperOrientationSample);

	ocean_assert(lowerOrientationSample->orientations().size() == upperOrientationSample->orientations().size());
	ocean_assert(lowerOrientationSample->referenceSystem() == upperOrientationSample->referenceSystem());

	Quaternions interpolatedOrientations(lowerOrientationSample->orientations().size());

	for (size_t n = 0; n < lowerOrientationSample->orientations().size(); ++n)
	{
		interpolatedOrientations[n] = Interpolation::linear(lowerOrientationSample->orientations()[n], upperOrientationSample->orientations()[n], Scalar(interpolationFactor));
	}

	return OrientationTracker3DOFSampleRef(new OrientationTracker3DOFSample(interpolatedTimestamp, lowerOrientationSample->referenceSystem(), lowerOrientationSample->objectIds(), interpolatedOrientations));
}

}

}
