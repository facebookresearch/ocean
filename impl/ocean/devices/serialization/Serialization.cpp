/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/serialization/Serialization.h"
#include "ocean/devices/serialization/SerializationFactory.h"

namespace Ocean
{

namespace Devices
{

namespace Serialization
{

std::string nameSerializationLibrary()
{
	return std::string("Serialization Replay Library");
}

#if defined(OCEAN_RUNTIME_STATIC)

void registerSerializationLibrary()
{
	Ocean::Devices::Serialization::SerializationFactory::registerFactory();
}

bool unregisterSerializationLibrary()
{
	return Ocean::Devices::Serialization::SerializationFactory::unregisterFactory();
}

#endif // defined(OCEAN_RUNTIME_STATIC)

}

}

}
