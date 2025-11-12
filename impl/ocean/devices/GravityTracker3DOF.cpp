/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/GravityTracker3DOF.h"

#include "ocean/math/Interpolation.h"

namespace Ocean
{

namespace Devices
{

GravityTracker3DOF::GravityTracker3DOFSample::GravityTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, const ObjectIds& objectIds, const Gravities& gravities, const Metadata& metadata) :
	Sample(timestamp, objectIds, metadata),
	TrackerSample(timestamp, referenceSystem, objectIds, metadata),
	gravities_(gravities)
{
	// nothing to do here
}

GravityTracker3DOF::GravityTracker3DOFSample::GravityTracker3DOFSample(const Timestamp& timestamp, const ReferenceSystem referenceSystem, ObjectIds&& objectIds, Gravities&& gravities, Metadata&& metadata) :
	Sample(timestamp, std::move(objectIds), std::move(metadata)),
	TrackerSample(timestamp, referenceSystem, std::move(objectIds), std::move(metadata)),
	gravities_(std::move(gravities))
{
	// nothing to do here
}

GravityTracker3DOF::GravityTracker3DOF(const std::string& name) :
	Device(name, deviceTypeGravityTracker3DOF()),
	Measurement(name, deviceTypeGravityTracker3DOF()),
	Tracker(name, deviceTypeGravityTracker3DOF())
{
	// nothing to do here
}

GravityTracker3DOF::~GravityTracker3DOF()
{
	// nothing to do here
}

Measurement::SampleRef GravityTracker3DOF::interpolateSamples(const SampleRef& lowerSample, const SampleRef& upperSample, const double interpolationFactor, const Timestamp& interpolatedTimestamp) const
{
	ocean_assert(lowerSample && upperSample);
	ocean_assert(interpolationFactor >= 0.0 && interpolationFactor <= 1.0);

	const GravityTracker3DOFSampleRef lowerGravitySample = lowerSample;
	const GravityTracker3DOFSampleRef upperGravitySample = upperSample;

	ocean_assert(lowerGravitySample && upperGravitySample);

	ocean_assert(lowerGravitySample->gravities().size() == upperGravitySample->gravities().size());
	ocean_assert(lowerGravitySample->referenceSystem() == upperGravitySample->referenceSystem());

	GravityTracker3DOFSample::Gravities interpolatedGravities(lowerGravitySample->gravities().size());

	for (size_t n = 0; n < lowerGravitySample->gravities().size(); ++n)
	{
		const Vector3& lowerGravity = lowerGravitySample->gravities()[n];
		const Vector3& upperGravity = upperGravitySample->gravities()[n];

		interpolatedGravities[n] = Interpolation::spherical(lowerGravity, upperGravity, Scalar(interpolationFactor));

		if (!interpolatedGravities[n].normalize())
		{
			ocean_assert(false && "This should never happen!");
			interpolatedGravities[n] = lowerGravity;
		}
	}

	return GravityTracker3DOFSampleRef(new GravityTracker3DOFSample(interpolatedTimestamp, lowerGravitySample->referenceSystem(), lowerGravitySample->objectIds(), interpolatedGravities));
}

}

}
