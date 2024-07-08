/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_OPENXR_INSTANCE_H
#define META_OCEAN_PLATFORM_OPENXR_INSTANCE_H

#include "ocean/platform/openxr/OpenXR.h"

#include "ocean/base/Lock.h"

namespace Ocean
{

namespace Platform
{

namespace OpenXR
{

/**
 * This class wraps an OpenXR instance.
 * An OpenXR instance is an object that allows an OpenXR application to communicate with an OpenXR runtime.
 * @ingroup platformopenxr
 */
class OCEAN_PLATFORM_OPENXR_EXPORT Instance final
{
	public:

		/**
		 * Definition of a vector holding extension properties.
		 */
		typedef std::vector<XrExtensionProperties> XrExtensionPropertyGroups;

		/**
		 * Definition of an unordered set holding strings.
		 */
		typedef std::unordered_set<std::string> StringSet;

		/**
		 * Definition of a vector holding XrViewConfigurationView objects.
		 */
		typedef std::vector<XrViewConfigurationView> XrViewConfigurationViews;

	public:

		/**
		 * Default constructor creating an invalid instance.
		 */
		Instance() = default;

		/**
		 * Move constructor.
		 * @param instance The instance to be moved
		 */
		inline Instance(Instance&& instance);

		/**
		 * Destructs the instance and releases all associated resources.
		 */
		~Instance();

		/**
		 * Initializes the instance.
		 * @param necessaryExtensions The names of the necessary extensions for the instance
		 * @param applicationName The name of the application
		 * @return True, if succeeded
		 */
		bool initialize(const StringSet& necessaryExtensions, const std::string& applicationName = "OceanOpenXR");

		/**
		 * Explicitly releases the instance and all associated resources.
		 * @see initialize().
		 */
		void release();

		/**
		 * Determines the view configurations for a specified view type available for this instance.
		 * @param xrViewConfigurationType The view type for which the configuration will be determined, must be valid
		 * @param xrViewConfigurationViews The resulting view configurations, e.g., one for each eye/display
		 * @return True, if succeeded
		 */
		inline bool determineViewConfigurations(const XrViewConfigurationType xrViewConfigurationType, XrViewConfigurationViews& xrViewConfigurationViews) const;

		/**
		 * Translates an OpenXR result associated with this instance into a readable string.
		 * @param xrResult The OpenXR result to translate
		 * @return The translated result
		 */
		std::string translateResult(const XrResult xrResult) const;

		/**
		 * Returns whether this object holds a valid OpenXR instance.
		 * @return True, if so
		 * @see initialize().
		 */
		bool isValid() const;

		/**
		 * Returns the identifier for the runtime.
		 * @return The runtime's identifier
		 */
		XrSystemId xrSystemId() const;

		/**
		 * Returns the enabled extensions of this instance.
		 * @return The instance's enabled extensions
		 */
		inline const StringSet& enabledExtensions() const;

		/**
		 * Move operator.
		 * @param instance The instance to be moved
		 * @return Reference to this object
		 */
		Instance& operator=(Instance&& instance);

		/**
		 * Returns the wrapped OpenXR instance.
		 * @return The actual instance, nullptr if not initialized
		 * @see isValid().
		 */
		operator XrInstance() const;

		/**
		 * Returns whether this object holds a valid OpenXR instance.
		 * @return True, if so
		 * @see isValid().
		 */
		explicit inline operator bool() const;

		/**
		 * Determines the available OpenXR API layers.
		 * @param apiLayers Optional resulting names of the API layers, nullptr if not of interest
		 * @return True, if succeeded
		 */
		static bool determineApiLayers(std::vector<std::string>* apiLayers = nullptr);

		/**
		 * Determines the properties of the available OpenXR extension.
		 * @param extensionNames Optional resulting names of the extensions, nullptr if not of interest
		 * @return True, if succeeded
		 */
		static XrExtensionPropertyGroups determineExtensionProperties(std::vector<std::string>* extensionNames = nullptr);

		/**
		 * Determines the names of existing OpenXR extensions which match the set of given necessary extensions.
		 * @param xrExtensionPropertyGroups The groups of available OpenXR extension properties
		 * @param necessaryExtensions The OpenXR extension which are necessary for the application
		 * @return The resulting names of all existing extensions which are also necessary
		 */
		static std::vector<const char*> determineExistingExtensionNames(const XrExtensionPropertyGroups& xrExtensionPropertyGroups, const StringSet& necessaryExtensions);

		/**
		 * Determines the view configurations for a specified view type.
		 * @param xrInstance The OpenXR instance for which the view configurations will be determined, must be valid
		 * @param xrSystemId The OpenXR runtime's identifier associated with the instance, must be valid
		 * @param xrViewConfigurationType The view type for which the configuration will be determined, must be valid
		 * @param xrViewConfigurationViews The resulting view configurations, e.g., one for each eye/display
		 * @return True, if succeeded
		 */
		static bool determineViewConfigurations(const XrInstance& xrInstance, const XrSystemId& xrSystemId, const XrViewConfigurationType xrViewConfigurationType, XrViewConfigurationViews& xrViewConfigurationViews);

	protected:

		/**
		 * Disabled copy constructor.
		 */
		Instance(const Instance&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		Instance& operator=(const Instance&) = delete;

	protected:

		/// The actual OpenXR instance.
		XrInstance xrInstance_ = XR_NULL_HANDLE;

		/// The identifier for the runtime.
		XrSystemId xrSystemId_ = XR_NULL_SYSTEM_ID;

		/// The instance's enabled extensions.
		StringSet enabledExtensions_;

		/// The instance's lock.
		mutable Lock lock_;
};

inline Instance::Instance(Instance&& instance)
{
	*this = std::move(instance);
}

inline bool Instance::determineViewConfigurations(const XrViewConfigurationType xrViewConfigurationType, XrViewConfigurationViews& xrViewConfigurationViews) const
{
	ocean_assert(isValid());

	return determineViewConfigurations(xrInstance_, xrSystemId_, xrViewConfigurationType, xrViewConfigurationViews);
}

inline const Instance::StringSet& Instance::enabledExtensions() const
{
	return enabledExtensions_;
}

inline Instance::operator bool() const
{
	return isValid();
}

}

}

}

#endif // META_OCEAN_PLATFORM_OPENXR_INSTANCE_H
