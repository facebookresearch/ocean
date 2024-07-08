/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/openxr/Instance.h"
#include "ocean/platform/openxr/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace OpenXR
{

Instance::~Instance()
{
	release();
}

bool Instance::initialize(const StringSet& necessaryExtensions, const std::string& applicationName)
{
	const ScopedLock scopedLock(lock_);

	if (xrInstance_ != XR_NULL_HANDLE)
	{
		ocean_assert(false && "This instance has already been initialized");
		return true;
	}

	ocean_assert(applicationName.size() < XR_MAX_APPLICATION_NAME_SIZE);
	static_assert(XR_MAX_APPLICATION_NAME_SIZE >= 2, "Invalid size");
	static_assert(sizeof(XrApplicationInfo::applicationName) == XR_MAX_APPLICATION_NAME_SIZE * sizeof(char), "Invalid parameter");

	XrApplicationInfo xrApplicationInfo = {};

	const size_t applicationNameLength = min(applicationName.size(), size_t(XR_MAX_APPLICATION_NAME_SIZE - 1));
	memcpy(xrApplicationInfo.applicationName, applicationName.c_str(), applicationNameLength * sizeof(char));
	xrApplicationInfo.applicationName[applicationNameLength] = '\0';
	xrApplicationInfo.applicationVersion = 0;

	strcpy(xrApplicationInfo.engineName, "Ocean");
	xrApplicationInfo.engineVersion = 0;
	xrApplicationInfo.apiVersion = XR_API_VERSION_1_0;

	const XrExtensionPropertyGroups xrExtensionPropertyGroups = determineExtensionProperties();
	const std::vector<const char*> enableExtensionNames = determineExistingExtensionNames(xrExtensionPropertyGroups, necessaryExtensions);

	XrInstanceCreateInfo xrInstanceCreateInfo = {XR_TYPE_INSTANCE_CREATE_INFO};
	xrInstanceCreateInfo.createFlags = 0;
	xrInstanceCreateInfo.applicationInfo = xrApplicationInfo;
	xrInstanceCreateInfo.enabledApiLayerCount = 0;
	xrInstanceCreateInfo.enabledApiLayerNames = nullptr;
	xrInstanceCreateInfo.enabledExtensionCount = uint32_t(enableExtensionNames.size());
	xrInstanceCreateInfo.enabledExtensionNames = enableExtensionNames.empty() ? nullptr : enableExtensionNames.data();

	XrResult xrResult = xrCreateInstance(&xrInstanceCreateInfo, &xrInstance_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to create instance: " << int(xrResult);

		return false;
	}

	enabledExtensions_.reserve(enableExtensionNames.size());

	for (const char* enabledExtensionName : enableExtensionNames)
	{
		enabledExtensions_.emplace(enabledExtensionName);
	}

	ocean_assert(enabledExtensions_.size() == enableExtensionNames.size());

	XrInstanceProperties xrInstanceProperties = {XR_TYPE_INSTANCE_PROPERTIES};
	xrResult = xrGetInstanceProperties(xrInstance_, &xrInstanceProperties);

	if (xrResult == XR_SUCCESS)
	{
		Log::debug() << "OpenXR: Instance '" << xrInstanceProperties.runtimeName << "' initialized, version " << XR_VERSION_MAJOR(xrInstanceProperties.runtimeVersion) << "." << XR_VERSION_MINOR(xrInstanceProperties.runtimeVersion) << "." << XR_VERSION_PATCH(xrInstanceProperties.runtimeVersion);
	}
	else
	{
		Log::error() << "OpenXR: Failed to determine instance properties: " << translateResult(xrResult);
	}

	XrSystemGetInfo xrSystemGetInfo = {XR_TYPE_SYSTEM_GET_INFO};
	xrSystemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

	ocean_assert(xrSystemId_ == XR_NULL_SYSTEM_ID);
	xrResult = xrGetSystem(xrInstance_, &xrSystemGetInfo, &xrSystemId_);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to determine the system id: " << translateResult(xrResult);
	}

	XrSystemProperties xrSystemProperties = {XR_TYPE_SYSTEM_PROPERTIES};

	XrSystemColorSpacePropertiesFB xrSystemColorSpacePropertiesFB = {XR_TYPE_SYSTEM_COLOR_SPACE_PROPERTIES_FB};
	xrSystemProperties.next = &xrSystemColorSpacePropertiesFB;

	xrResult = xrGetSystemProperties(xrInstance_, xrSystemId_, &xrSystemProperties);

	if (xrResult == XR_SUCCESS)
	{
		Log::debug() << "OpenXR: System name '" << xrSystemProperties.systemName << "', vendor id '" << xrSystemProperties.vendorId << "'";
		Log::debug() << "OpenXR: Resulution: " << xrSystemProperties.graphicsProperties.maxSwapchainImageWidth << "x" << xrSystemProperties.graphicsProperties.maxSwapchainImageHeight << ", " << xrSystemProperties.graphicsProperties.maxLayerCount << " layers";
		Log::debug() << "OpenXR: Orientation tracking: " << (xrSystemProperties.trackingProperties.orientationTracking ?  "True" : "False");
		Log::debug() << "OpenXR: Position tracking: " << (xrSystemProperties.trackingProperties.positionTracking ?  "True" : "False");
		Log::debug() << "OpenXR: Color space: " << Utilities::translateColorSpace(xrSystemColorSpacePropertiesFB.colorSpace);
	}
	else
	{
		Log::error() << "OpenXR: Failed to determine the system properties: " << translateResult(xrResult);
	}

	return true;
}

void Instance::release()
{
	const ScopedLock scopedLock(lock_);

	if (xrInstance_ != XR_NULL_HANDLE)
	{
		xrDestroyInstance(xrInstance_);
		xrInstance_ = XR_NULL_HANDLE;
	}

	enabledExtensions_.clear();
}

std::string Instance::translateResult(const XrResult xrResult) const
{
	const ScopedLock scopedLock(lock_);

	if (xrInstance_ != XR_NULL_HANDLE)
	{
		return Utilities::translateResult(xrInstance_, xrResult);
	}

	ocean_assert(false && "Failed to translate XrResult");
	return "Unknown: OpenXR Instance not initialized";
}

bool Instance::isValid() const
{
	const ScopedLock scopedLock(lock_);

	return xrInstance_ != XR_NULL_HANDLE;
}

XrSystemId Instance::xrSystemId() const
{
	const ScopedLock scopedLock(lock_);

	return xrSystemId_;
}

Instance& Instance::operator=(Instance&& instance)
{
	if (this != &instance)
	{
		release();

		xrInstance_ = instance.xrInstance_;
		instance.xrInstance_ = XR_NULL_HANDLE;

		xrSystemId_ = instance.xrSystemId_;
		instance.xrSystemId_ = XR_NULL_SYSTEM_ID;

		enabledExtensions_ = std::move(instance.enabledExtensions_);
	}

	return *this;
}

Instance::operator XrInstance() const
{
	const ScopedLock scopedLock(lock_);

	return xrInstance_;
}

bool Instance::determineApiLayers(std::vector<std::string>* apiLayers)
{
	if (apiLayers != nullptr)
	{
		apiLayers->clear();
	}

	PFN_xrEnumerateApiLayerProperties xrEnumerateApiLayerProperties;
	XrResult xrResult = xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrEnumerateApiLayerProperties", (PFN_xrVoidFunction*)&xrEnumerateApiLayerProperties);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "Failed to get xrEnumerateApiLayerProperties function";
		return false;
	}

	uint32_t numberLayers = 0u;
	xrResult = xrEnumerateApiLayerProperties(0u, &numberLayers, nullptr);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "Failed to determine number of layers with xrEnumerateApiLayerProperties: " << int(xrResult);
		return false;
	}

	Log::debug() << "Found " << numberLayers << " OpenXR API layers:";

	if (numberLayers != 0u)
	{
		std::vector<XrApiLayerProperties> xrApiLayerPropertyGroups;

		xrResult = xrEnumerateApiLayerProperties(uint32_t(xrApiLayerPropertyGroups.size()), &numberLayers, xrApiLayerPropertyGroups.data());

		if (xrResult != XR_SUCCESS)
		{
			Log::error() << "Failed to determine layer properties with xrEnumerateApiLayerProperties: " << int(xrResult);
			return false;
		}

		if (apiLayers != nullptr)
		{
			apiLayers->reserve(xrApiLayerPropertyGroups.size());
		}

		for (const XrApiLayerProperties& xrApiLayerProperties : xrApiLayerPropertyGroups)
		{
			if (apiLayers != nullptr)
			{
				apiLayers->emplace_back(xrApiLayerProperties.layerName);
			}

			Log::debug() << xrApiLayerProperties.layerName << ", " << xrApiLayerProperties.description;
		}
	}

	return true;
}

Instance::XrExtensionPropertyGroups Instance::determineExtensionProperties(std::vector<std::string>* extensionNames)
{
	if (extensionNames != nullptr)
	{
		extensionNames->clear();
	}

	PFN_xrEnumerateInstanceExtensionProperties xrEnumerateInstanceExtensionProperties = nullptr;

	XrResult xrResult = xrGetInstanceProcAddr(XR_NULL_HANDLE, "xrEnumerateInstanceExtensionProperties", (PFN_xrVoidFunction*)&xrEnumerateInstanceExtensionProperties);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "Failed to determine extension properties function: " << int(xrResult);
		return XrExtensionPropertyGroups();
	}

	uint32_t propertyCountOutput = 0u;
	xrResult = xrEnumerateInstanceExtensionProperties(nullptr, 0u, &propertyCountOutput, nullptr);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "Failed to determine number of extension properties: " << int(xrResult);
		return XrExtensionPropertyGroups();
	}

	Log::debug() << "Found " << propertyCountOutput << " OpenXR extensions:";

	if (propertyCountOutput != 0u)
	{
		if (extensionNames != nullptr)
		{
			extensionNames->reserve(propertyCountOutput);
		}

		XrExtensionPropertyGroups xrExtensionPropertyGroups(propertyCountOutput, {XR_TYPE_EXTENSION_PROPERTIES});

		xrResult = xrEnumerateInstanceExtensionProperties(nullptr, uint32_t(xrExtensionPropertyGroups.size()), &propertyCountOutput, xrExtensionPropertyGroups.data());
		ocean_assert(xrExtensionPropertyGroups.size() == size_t(propertyCountOutput));

		if (xrResult != XR_SUCCESS)
		{
			Log::error() << "Failed to determine extension properties: " << int(xrResult);
			return XrExtensionPropertyGroups();
		}

		for (const XrExtensionProperties& xrExtensionProperties : xrExtensionPropertyGroups)
		{
			if (extensionNames != nullptr)
			{
				extensionNames->emplace_back(xrExtensionProperties.extensionName);
			}

			Log::debug() << xrExtensionProperties.extensionName;
		}

		return xrExtensionPropertyGroups;
	}

	return XrExtensionPropertyGroups();
}

std::vector<const char*> Instance::determineExistingExtensionNames(const XrExtensionPropertyGroups& xrExtensionPropertyGroups, const StringSet& necessaryExtensions)
{
	if (necessaryExtensions.empty())
	{
		return std::vector<const char*>();
	}

	std::vector<const char*> extensionNames;
	extensionNames.reserve(necessaryExtensions.size());

	for (const XrExtensionProperties& xrExtensionProperties : xrExtensionPropertyGroups)
	{
		if (necessaryExtensions.find(xrExtensionProperties.extensionName) != necessaryExtensions.cend())
		{
			extensionNames.emplace_back(xrExtensionProperties.extensionName);
		}
	}

	if (extensionNames.size() == necessaryExtensions.size())
	{
#ifdef OCEAN_DEBUG
		Log::debug() << "Found all " << necessaryExtensions.size() << " necessary extensions:";

		for (const std::string& necessaryExtension : necessaryExtensions)
		{
			Log::debug() << necessaryExtension;
		}
#endif // OCEAN_DEBUG
	}
	else
	{
		StringSet copyNecessaryExtensions(necessaryExtensions);
		for (const char* extensionName : extensionNames)
		{
			copyNecessaryExtensions.erase(extensionName);
		}

		std::string missingExtensions;

		for (const std::string& necessaryExtension : copyNecessaryExtensions)
		{
			if (!missingExtensions.empty())
			{
				missingExtensions += ", ";
			}

			missingExtensions += necessaryExtension;
		}

		Log::warning() << "Found only " << extensionNames.size() << " of " << necessaryExtensions.size() << " necessary OpenXR extensions, missing: " << missingExtensions;
	}

	return extensionNames;
}

bool Instance::determineViewConfigurations(const XrInstance& xrInstance, const XrSystemId& xrSystemId, const XrViewConfigurationType xrViewConfigurationType, XrViewConfigurationViews& xrViewConfigurationViews)
{
	ocean_assert(xrInstance != XR_NULL_HANDLE);
	ocean_assert(xrSystemId != XR_NULL_SYSTEM_ID);

	uint32_t viewConfigurationTypeCountOutput = 0u;
	XrResult xrResult = xrEnumerateViewConfigurations(xrInstance, xrSystemId, 0u, &viewConfigurationTypeCountOutput, nullptr);

	if (xrResult != XR_SUCCESS)
	{
		Log::error() << "OpenXR: Failed to enumerate view configuration: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	std::vector<XrViewConfigurationType> xrEnumeratedViewConfigurationTypes(viewConfigurationTypeCountOutput);

	viewConfigurationTypeCountOutput = 0u;
	xrResult = xrEnumerateViewConfigurations(xrInstance, xrSystemId, uint32_t(xrEnumeratedViewConfigurationTypes.size()), &viewConfigurationTypeCountOutput, xrEnumeratedViewConfigurationTypes.data());

	if (xrResult != XR_SUCCESS || viewConfigurationTypeCountOutput != uint32_t(xrEnumeratedViewConfigurationTypes.size()))
	{
		Log::error() << "OpenXR: Failed to enumerate view configuration: " << Utilities::translateResult(xrInstance, xrResult);
		return false;
	}

	XrViewConfigurationViews xrEnumeratedViewConfigurationViews;

	bool foundMatchingConfiguration = false;

	Log::debug() << "OpenXR: Found " << xrEnumeratedViewConfigurationTypes.size() << " view configurations:";

	for (const XrViewConfigurationType& xrEnumeratedViewConfigurationType : xrEnumeratedViewConfigurationTypes)
	{
		XrViewConfigurationProperties xrViewConfigurationProperties = {};
		xrResult = xrGetViewConfigurationProperties(xrInstance, xrSystemId, xrEnumeratedViewConfigurationType, &xrViewConfigurationProperties);

		if (xrResult != XR_SUCCESS)
		{
			Log::error() << "OpenXR: Failed to determine view configuration properties: " << Utilities::translateResult(xrInstance, xrResult);
			return false;
		}

		Log::debug() << Utilities::translateViewConfigurationType(xrEnumeratedViewConfigurationType) << ": " << (xrViewConfigurationProperties.fovMutable ? "mutable fov" : "fixed fov");

		uint32_t viewCountOutput = 0u;
		xrResult = xrEnumerateViewConfigurationViews(xrInstance, xrSystemId, xrEnumeratedViewConfigurationType, 0u, &viewCountOutput, nullptr);

		if (xrResult != XR_SUCCESS)
		{
			Log::error() << "OpenXR: Failed to enumerate view configuration views: " << Utilities::translateResult(xrInstance, xrResult);
			return false;
		}

		xrEnumeratedViewConfigurationViews.resize(viewCountOutput, {XR_TYPE_VIEW_CONFIGURATION_VIEW});

		xrResult = xrEnumerateViewConfigurationViews(xrInstance, xrSystemId, xrEnumeratedViewConfigurationType, uint32_t(xrEnumeratedViewConfigurationViews.size()), &viewCountOutput, xrEnumeratedViewConfigurationViews.data());

		if (xrResult != XR_SUCCESS || viewCountOutput != uint32_t(xrEnumeratedViewConfigurationViews.size()))
		{
			Log::error() << "OpenXR: Failed to enumerate view configuration views: " << Utilities::translateResult(xrInstance, xrResult);
			return false;
		}

#ifdef OCEAN_DEBUG

		Log::debug() << "With views:";

		for (const XrViewConfigurationView& xrEnumeratedViewConfigurationView : xrEnumeratedViewConfigurationViews)
		{
			Log::debug() << "Recommended image dimension: " << xrEnumeratedViewConfigurationView.recommendedImageRectWidth << "x" << xrEnumeratedViewConfigurationView.recommendedImageRectHeight;
			Log::debug() << "Maximal image dimension: " << xrEnumeratedViewConfigurationView.maxImageRectWidth << "x" << xrEnumeratedViewConfigurationView.maxImageRectHeight;
			Log::debug() << "Recommended swapchain sample count: " << xrEnumeratedViewConfigurationView.recommendedSwapchainSampleCount;
			Log::debug() << "Maximal swapchain sample count: " << xrEnumeratedViewConfigurationView.maxSwapchainSampleCount;
		}

#endif // OCEAN_DEBUG

		if (!foundMatchingConfiguration && xrEnumeratedViewConfigurationType == xrViewConfigurationType)
		{
			std::swap(xrViewConfigurationViews, xrEnumeratedViewConfigurationViews);

			foundMatchingConfiguration = true;

#ifndef OCEAN_DEBUG
			break;
#endif
		}
	}

	return foundMatchingConfiguration;
}

}

}

}
