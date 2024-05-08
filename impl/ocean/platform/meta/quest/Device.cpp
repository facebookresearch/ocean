/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/Device.h"

#include "ocean/platform/android/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

Device::DeviceType Device::deviceType()
{
	static_assert(DT_QUEST_END < invalidQuestDeviceValue_, "Invalid device type!");

	std::string productModel;
	if (!Platform::Android::Utilities::systemPropertyValue("ro.product.model", productModel))
	{
		Log::error() << "Failed to read the 'ro.product.model' system property";

		return DT_UNKNOWN;
	}

	productModel = String::toLower(productModel);

	if (productModel == "quest")
	{
		return DT_QUEST;
	}

	if (productModel == "quest 2")
	{
		return DT_QUEST_2;
	}

	if (productModel == "quest 3")
	{
		return DT_QUEST_3;
	}

	if (productModel == "quest pro")
	{
		return DT_QUEST_PRO;
	}

#ifdef OCEAN_PLATFORM_META_QUEST_USE_EXTERNAL_DEVICE_NAME

	std::string productName;
	if (!Platform::Android::Utilities::systemPropertyValue("ro.product.name", productName))
	{
		Log::error() << "Failed to read the 'ro.product.name' system property";

		return DT_UNKNOWN;
	}

	const uint32_t externalDeviceType = PlatformMetaDevice_externalDeviceType(productModel, productName);

	if (externalDeviceType == invalidQuestDeviceValue_)
	{
		return DT_UNKNOWN;
	}

	return Device::DeviceType(externalDeviceType);

#else

	Log::error() << "The type of the Meta device could not be determined, unknown model name '" << productModel << "'";

	ocean_assert(false && "This should never happen!");

	return DT_UNKNOWN;

#endif // OCEAN_PLATFORM_META_QUEST_USE_EXTERNAL_DEVICE_NAME
}

std::string Device::deviceName(const DeviceType deviceType)
{
	static_assert(DT_QUEST_END < invalidQuestDeviceValue_, "Invalid device type!");

	switch (deviceType)
	{
		case DT_QUEST:
			return std::string("Quest");

		case DT_QUEST_2:
			return std::string("Quest 2");

		case DT_QUEST_3:
			return std::string("Quest 3");

		case DT_QUEST_PRO:
			return std::string("Quest Pro");

		case DT_UNKNOWN:
			return std::string("Unknown");

		case DT_QUEST_END:
			break;

#ifdef OCEAN_PLATFORM_META_QUEST_USE_EXTERNAL_DEVICE_NAME
		default:
			return PlatformMetaDevice_externalDeviceName(deviceType);
#endif
	}

	ocean_assert(false && "Invalid device type!");
	return std::string("Unknown");
}

} // namespace Quest

} // namespace Meta

} // namespace Platform

} // namespace Ocean
