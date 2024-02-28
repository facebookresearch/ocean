// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/devices/MagneticTracker.h"

namespace Ocean
{

namespace Devices
{

MagneticTracker::MagneticTracker(const std::string& name, const DeviceType type) :
	Device(name, type),
	Measurement(name, type),
	Tracker(name, type)
{
	// nothing to do here
}

MagneticTracker::~MagneticTracker()
{
	// nothing to do here
}

bool MagneticTracker::resetMagneticField()
{
	throw NotSupportedException("Magnetic field reset is not supported.");
}

}

}
