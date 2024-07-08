/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/usb/android/OceanUSBManager.h"
#include "ocean/system/usb/Utilities.h"

#include "ocean/platform/android/Utilities.h"

namespace Ocean
{

using namespace Platform::Android;

namespace System
{

namespace USB
{

namespace Android
{

using AndroidUtilities = Platform::Android::Utilities;
using USBUtilities = System::USB::Utilities;

std::string OceanUSBManager::DeviceDescriptor::toString() const
{
	if (!isValid())
	{
		return "Invalid";
	}

	std::string result;

	result += "Name: " + deviceName_;
	result += "\nProduct: " + productName_;
	result += "\nManufacturer: " + manufacturerName_;
	result += "\nVendorId: 0x" + String::toAStringHex(vendorId_, true);
	result += "\nProductId: 0x" + String::toAStringHex(productId_, true);
	result += "\nClass: 0x" + String::toAStringHex(deviceClass_, true);
	result += "\nSubclass: 0x" + String::toAStringHex(deviceSubclass_, true);
	result += "\nProtocol: 0x" + String::toAStringHex(deviceProtocol_, true);

	return result;
}

bool OceanUSBManager::initialize(JNIEnv* jniEnv)
{
	ocean_assert(jniEnv != nullptr);

	if (jniEnv == nullptr)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (javaClassOceanUSBManager_)
	{
		Log::info() << "OceanUSBManager already initialized.";
		return true;
	}

	javaClassOceanUSBManager_ = ScopedJClass(*jniEnv, jniEnv->FindClass("com/meta/ocean/system/usb/android/OceanUSBManager"));

	if (!javaClassOceanUSBManager_)
	{
		Log::error() << "Failed to initialize OceanUSBManager, ensure that Java class 'OceanUSBManager' exist, ensure to call initialize() from main thread.";
		return false;
	}

	javaClassOceanUSBManager_.makeGlobal();

	return true;
}

bool OceanUSBManager::isInitialized()
{
	const ScopedLock scopedLock(lock_);

	return javaClassOceanUSBManager_.isValid();
}

bool OceanUSBManager::enumerateDevices(JNIEnv* jniEnv, DeviceDescriptors& deviceDescriptors, const unsigned int deviceClass)
{
	ocean_assert(jniEnv != nullptr);
	if (jniEnv == nullptr)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (!javaClassOceanUSBManager_)
	{
		return false;
	}

	const jmethodID functionId = jniEnv->GetStaticMethodID(javaClassOceanUSBManager_, "enumerateDevices", "(I)Ljava/util/List;");

	if (functionId == nullptr)
	{
		return false;
	}

	int deviceClassInt = -1;

	if (deviceClass <= 0xFFFFu)
	{
		deviceClassInt = int(deviceClass);
	}

	const ScopedJObject resultList(*jniEnv, jniEnv->CallStaticObjectMethod(javaClassOceanUSBManager_, functionId, deviceClassInt));

	if (!resultList)
	{
		return false;
	}

	std::vector<std::string> deviceNames;
	if (!AndroidUtilities::toVector(jniEnv, resultList, deviceNames))
	{
		return false;
	}

	const jmethodID functionIdGetProductName = jniEnv->GetStaticMethodID(javaClassOceanUSBManager_, "getProductName", "(Ljava/lang/String;)Ljava/lang/String;");
	const jmethodID functionIdGetManufacturerName = jniEnv->GetStaticMethodID(javaClassOceanUSBManager_, "getManufacturerName", "(Ljava/lang/String;)Ljava/lang/String;");
	const jmethodID functionIdGetProductDetails = jniEnv->GetStaticMethodID(javaClassOceanUSBManager_, "getProductDetails","(Ljava/lang/String;)Ljava/util/List;");

	if (functionIdGetProductName == nullptr || functionIdGetManufacturerName == nullptr || functionIdGetProductDetails == nullptr)
	{
		return false;
	}

	deviceDescriptors.clear();

	for (const std::string& deviceName : deviceNames)
	{
		const ScopedJString jDeviceName(*jniEnv, AndroidUtilities::toJavaString(jniEnv, deviceName));
		ocean_assert(jDeviceName);

		const ScopedJString jManufacturerName(*jniEnv, jstring(jniEnv->CallStaticObjectMethod(javaClassOceanUSBManager_, functionIdGetManufacturerName, jobject(jDeviceName))));
		const ScopedJString jProductName(*jniEnv, jstring(jniEnv->CallStaticObjectMethod(javaClassOceanUSBManager_, functionIdGetProductName, jobject(jDeviceName))));
		const ScopedJString jProductDetails(*jniEnv, jstring(jniEnv->CallStaticObjectMethod(javaClassOceanUSBManager_, functionIdGetProductDetails, jobject(jDeviceName))));

		if (jProductDetails == nullptr)
		{
			return false;
		}

		DeviceDescriptor deviceDescriptor;

		deviceDescriptor.deviceName_ = deviceName;

		if (jProductName)
		{
			deviceDescriptor.productName_ = AndroidUtilities::toAString(jniEnv, jProductName);
		}

		if (jManufacturerName)
		{
			deviceDescriptor.manufacturerName_ = AndroidUtilities::toAString(jniEnv, jManufacturerName);
		}

		std::vector<int> productDetails;
		if (!AndroidUtilities::toVector(jniEnv, jProductDetails, productDetails))
		{
			return false;
		}

		if (productDetails.size() != 6)
		{
			return false;
		}

		if (productDetails[0] < 0 || productDetails[1] < 0 || productDetails[2] < 0 || productDetails[3] < 0 || productDetails[4] < 0 || productDetails[5] < 0)
		{
			return false;
		}

		deviceDescriptor.vendorId_ = (unsigned int)(productDetails[0]);
		deviceDescriptor.productId_ = (unsigned int)(productDetails[1]);
		deviceDescriptor.deviceClass_ = (unsigned int)(productDetails[2]);
		deviceDescriptor.deviceSubclass_ = (unsigned int)(productDetails[3]);
		deviceDescriptor.deviceProtocol_ = (unsigned int)(productDetails[4]);

		if (deviceDescriptor.manufacturerName_.empty())
		{
			// we try to lookup the vendor name from our own lookup table
			deviceDescriptor.manufacturerName_ = USBUtilities::vendorName(deviceDescriptor.vendorId_);
		}

		if (deviceDescriptor.productName_.empty())
		{
			// we try to lookup the vendor name from our own lookup table
			deviceDescriptor.productName_ = USBUtilities::productName(deviceDescriptor.vendorId_, deviceDescriptor.productId_);
		}

		ocean_assert(deviceDescriptor.isValid());

		if (!deviceDescriptor.isValid())
		{
			Log::error() << "Invalid device: " << deviceDescriptor.deviceName_;
			continue;
		}

		deviceDescriptors.emplace_back(std::move(deviceDescriptor));
  }

	return true;
}

bool OceanUSBManager::hasPermission(JNIEnv* jniEnv, const std::string& deviceName, bool& permissionGranted)
{
	ocean_assert(jniEnv != nullptr);
	if (jniEnv == nullptr)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (!javaClassOceanUSBManager_)
	{
		return false;
	}

	const jmethodID functionId = jniEnv->GetStaticMethodID(javaClassOceanUSBManager_, "hasPermission", "(Ljava/lang/String;)I");

	if (functionId == nullptr)
	{
		return false;
	}

	const ScopedJString jDeviceName(*jniEnv, AndroidUtilities::toJavaString(jniEnv, deviceName));
	ocean_assert(jDeviceName);

	const jint result = jniEnv->CallStaticIntMethod(javaClassOceanUSBManager_, functionId, jobject(jDeviceName));

	if (result == 1)
	{
		permissionGranted = true;
		return true;
	}

	if (result == 0)
	{
		permissionGranted = false;
		return true;
	}

	ocean_assert(result == -1);
	return false;
}

OceanUSBManager::ScopedPermissionSubscription OceanUSBManager::requestPermission(JNIEnv* jniEnv, const std::string& deviceName, PermissionCallback permissionCallback)
{
	ocean_assert(jniEnv != nullptr);
	if (jniEnv == nullptr)
	{
		return ScopedPermissionSubscription();
	}

	const ScopedLock scopedLock(lock_);

	if (!javaClassOceanUSBManager_)
	{
		return ScopedPermissionSubscription();
	}

	const jmethodID functionId = jniEnv->GetStaticMethodID(javaClassOceanUSBManager_, "requestPermission", "(Ljava/lang/String;)Z");

	if (functionId == nullptr)
	{
		return ScopedPermissionSubscription();
	}

	const ScopedJString jDeviceName(*jniEnv, AndroidUtilities::toJavaString(jniEnv, deviceName));
	ocean_assert(jDeviceName);

	const bool permissionCallbackWasValid = permissionCallback != nullptr;

	const uint32_t uniqueRequestId = ++uniqueRequestIdCounter_;

	if (permissionCallback)
	{
		PermissionCallbackMap::iterator iPermission = permissionCallbackMap_.find(deviceName);

		if (iPermission == permissionCallbackMap_.cend())
		{
			iPermission = permissionCallbackMap_.emplace(deviceName, PermissionCallbackPairs()).first;
		}

		ocean_assert(iPermission != permissionCallbackMap_.cend());
		iPermission->second.emplace_back(uniqueRequestId, std::move(permissionCallback));
	}

	if (!jniEnv->CallStaticBooleanMethod(javaClassOceanUSBManager_, functionId, jobject(jDeviceName)))
	{
		if (permissionCallbackWasValid)
		{
			// we remove the permission callback function again as it would never be called
			permissionCallbackMap_.erase(deviceName);
		}

		return ScopedPermissionSubscription();
	}

	return ScopedPermissionSubscription(std::make_pair(uniqueRequestId, deviceName), std::bind(&OceanUSBManager::releasePermissionRequest, this, std::placeholders::_1));;
}

bool OceanUSBManager::openDevice(JNIEnv* jniEnv, const std::string& deviceName, int64_t& fileDescriptor)
{
	ocean_assert(jniEnv != nullptr);
	if (jniEnv == nullptr)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (!javaClassOceanUSBManager_)
	{
		return false;
	}

	const jmethodID functionId = jniEnv->GetStaticMethodID(javaClassOceanUSBManager_, "openDevice", "(Ljava/lang/String;)I");

	if (functionId == nullptr)
	{
		return false;
	}

	const ScopedJString jDeviceName(*jniEnv, AndroidUtilities::toJavaString(jniEnv, deviceName));
	ocean_assert(jDeviceName);

	const jint result = jniEnv->CallStaticIntMethod(javaClassOceanUSBManager_, functionId, jobject(jDeviceName));

	if (result == -1)
	{
		return false;
	}

	ocean_assert(result > 0);
	fileDescriptor = int64_t(result);
	return true;
}

bool OceanUSBManager::closeDevice(JNIEnv* jniEnv, const std::string& deviceName)
{
	ocean_assert(jniEnv != nullptr);
	if (jniEnv == nullptr)
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	if (!javaClassOceanUSBManager_)
	{
		return false;
	}

	const jmethodID functionId = jniEnv->GetStaticMethodID(javaClassOceanUSBManager_, "closeDevice", "(Ljava/lang/String;)Z");

	if (functionId == nullptr)
	{
		return false;
	}

	const ScopedJString jDeviceName(*jniEnv, AndroidUtilities::toJavaString(jniEnv, deviceName));
	ocean_assert(jDeviceName);

	return jniEnv->CallStaticBooleanMethod(javaClassOceanUSBManager_, functionId, jobject(jDeviceName));
}

void OceanUSBManager::onDevicePermission(const std::string& deviceName, bool granted)
{
	Log::debug() << "OceanUSBManager: Device permission granted: '" << deviceName << "'" << " " << (granted ? "true" : "false");

	TemporaryScopedLock scopedLock(lock_);

	PermissionCallbackMap::iterator iCallback = permissionCallbackMap_.find(deviceName);

	if (iCallback != permissionCallbackMap_.cend())
	{
		const PermissionCallbackPairs permissionCallbackPairs = std::move(iCallback->second);
		permissionCallbackMap_.erase(iCallback);

		scopedLock.release();

		for (const PermissionCallbackPair& permissionCallbackPair : permissionCallbackPairs)
		{
			const PermissionCallback& permissionCallback = permissionCallbackPair.second;

			ocean_assert(permissionCallback);
			permissionCallback(deviceName, granted);
		}
	}
	else
	{
		Log::debug() << "OceanUSBManager: Device permission granted, but no callback defined '" << deviceName << "' (anymore)";
	}
}

void OceanUSBManager::releasePermissionRequest(const DevicePair& devicePair)
{
	const uint32_t requestId = devicePair.first;
	const std::string& deviceName = devicePair.second;

	ocean_assert(!deviceName.empty());

	const ScopedLock scopedLock(lock_);

	PermissionCallbackMap::iterator iCallback = permissionCallbackMap_.find(deviceName);

	if (iCallback != permissionCallbackMap_.cend())
	{
		// the permission request is still pending (e.g., because the user has not yet granted or denied the permission)
		Log::debug() << "OceanUSBManager: Pending device permission request released for '" << deviceName << "'";

		PermissionCallbackPairs& permissionCallbackPairs = iCallback->second;

		for (size_t n = 0; n < permissionCallbackPairs.size(); ++n)
		{
			if (permissionCallbackPairs[n].first == requestId)
			{
				permissionCallbackPairs[n] = permissionCallbackPairs.back();
				permissionCallbackPairs.pop_back();

				break;
			}
		}

		if (permissionCallbackPairs.empty())
		{
			permissionCallbackMap_.erase(iCallback);
		}
	}
}

}

}

}

}

bool Java_com_meta_ocean_system_usb_android_OceanUSBManager_initialize(JNIEnv* env, jobject javaThis)
{
	return Ocean::System::USB::Android::OceanUSBManager::get().initialize(env);
}

void Java_com_meta_ocean_system_usb_android_OceanUSBManager_onDevicePermission(JNIEnv* env, jobject javaThis, jstring deviceName, jboolean granted)
{
	const std::string cDeviceName = Ocean::Platform::Android::Utilities::toAString(env, deviceName);

	Ocean::System::USB::Android::OceanUSBManager::get().onDevicePermission(cDeviceName, granted);
}
