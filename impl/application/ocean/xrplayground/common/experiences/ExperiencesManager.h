// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_EXPERIENCES_MANAGER_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_EXPERIENCES_MANAGER_H

#include "application/ocean/xrplayground/XRPlayground.h"

#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/devices/GPSTracker.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements a manager for all experiences.
 * @ingroup xrplayground
 */
class ExperiencesManager
{
	public:

		/**
		 * Definition of individual platform types.
		 */
		enum PlatformType : uint32_t
		{
			/// Unknown platform.
			PT_UNKNOWN = 0u,
			/// Phone platform, iPhone
			PT_PHONE_IOS = 1u << 0u,
			/// Phone platform, Android
			PT_PHONE_ANDROID = 1u << 1u,
			/// Head-mounted-display platform, Quest, Indoor e.g., without GPS
			PT_HMD_QUEST_INDOOR = 1u << 2u,
			/// Desktop platform, macOS
			PT_DESKTOP_MACOS = 1u << 3u,
			/// Desktop platform, Windows
			PT_DESKTOP_WINDOWS = 1u << 4u,
			/// Any phone platform.
			PT_PHONE = PT_PHONE_IOS | PT_PHONE_ANDROID,
			/// Any desktop platform.
			PT_DESKTOP = PT_DESKTOP_MACOS | PT_DESKTOP_WINDOWS,
			/// Any phone or desktop platform, e.g., because on desktop a VRS replay can be used
			PT_PHONE_OR_DESKTOP = PT_PHONE | PT_DESKTOP,
			/// Any portable platform.
			PT_PORTABLE = PT_PHONE | PT_HMD_QUEST_INDOOR,
			/// Any platform.
			PT_ANY = PT_PORTABLE | PT_DESKTOP
		};

		/**
		 * Definition of a class storing relevant information of a selectable experience.
		 */
		class SelectableExperience
		{
			public:

				/**
				 * Creates a new experience object.
				 * @param name The readable name of the experience.
				 * @param url The url from which the experience can be loaded
				 * @param platformType The type of the platform on which this experience can be executed
				 * @param location Optional GPS location to which the experience is restricted, an invalid location to define an experience without specific area, the accuracy can be used to specify an explicit engagement radius
				 */
				inline SelectableExperience(std::string name, std::string url, const PlatformType platformType, const Devices::GPSTracker::Location& location = Devices::GPSTracker::Location());

				/**
				 * Returns the readable name of the experience.
				 * @return The experience's readable name
				 */
				inline const std::string& name() const;

				/**
				 * Returns the url from which the experience can be loaded.
				 * @return The experience's url
				 */
				inline const std::string& url() const;

				/**
				 * Returns the type of the platform on which this experience can be executed.
				 * @return The platform type
				 */
				inline PlatformType platformType() const;

				/**
				 * Returns the GPS location of this experience.
				 * @return The experience's GPS location, if any
				 */
				inline const Devices::GPSTracker::Location& location() const;

				/**
				 * Returns whether this experience is available at a given GPS location.
				 * This experience is available if this experience is available everywhere, or if the experience's location is close to the given location.<br>
				 * In case the experience's location holds a valid accuracy value, the accuracy value is used as maximal engagement radius; otherwise a default engagement radius of 500 meters is used.
				 * @param queryLocation The GPS location for which the exerience's location will be checked, must be valid
				 * @return True, whether this experience is available at the specified GPS location
				 */
				bool isAvailableAtLocation(const Devices::GPSTracker::Location& queryLocation) const;

			protected:

				/// The readable name of the experience.
				std::string name_;

				/// The url from which the experience can be loaded.
				std::string url_;

				/// he type of the platform on which this experience can be executed..
				PlatformType platformType_ = PT_UNKNOWN;

				/// Optional GPS location to which the experience is restricted, an invalid location to define an experience without specific area.
				Devices::GPSTracker::Location location_;
		};

		/**
		 * Definition of a vector holding selectable experiences.
		 */
		typedef std::vector<SelectableExperience> SelectableExperiences;

		/**
		 * Definition of a pair combining a group name and several selectable experiences.
		 */
		typedef std::pair<std::string, SelectableExperiences> SelectableExperienceGroup;

		/**
		 * Definition of a vector holding selectable experience groups.
		 */
		typedef std::vector<SelectableExperienceGroup> SelectableExperienceGroups;

	protected:

		/**
		 * Definition of a unique pointer to an Experience object.
		 */
		typedef std::unique_ptr<XRPlaygroundExperience> UniqueXRPlaygroundExperience;

		/**
		 * Definition of a callback function allowing to create a new experience.
		 */
		typedef std::function<UniqueXRPlaygroundExperience()> CreateXRPlaygroundExperienceFunction;

	public:

		/**
		 * Registers all experiences based on C++.
		 * Must be called once when the application starts.
		 */
		static void registerAllCodeBasedExperiences();

		/**
		 * Returns the groups of selectable experiences.
		 * @param platformType The current platform type on which the experiences will be executed
		 * @return The available selectable experiences
		 */
		static SelectableExperienceGroups selectableExperienceGroups(const PlatformType platformType);

	protected:

		/**
		 * Defines all selectable experiences.
		 * @return All selectable experience not yet filtered due to GPS or platform.
		 */
		static SelectableExperienceGroups defineExperienceGroups();

		/**
		 * Registers a new experience wihtout actually creating the experience.
		 * @param experienceName The name of the experience to register, must be valid
		 * @param createXRPlaygroundExperienceFunction The create function of the new experience, must be valid
		 * @return True, if succeeded
		 */
		static bool registerNewExperience(std::string experienceName, CreateXRPlaygroundExperienceFunction createXRPlaygroundExperienceFunction);
};

inline ExperiencesManager::SelectableExperience::SelectableExperience(std::string name, std::string url, const PlatformType platformType, const Devices::GPSTracker::Location& location) :
	name_(std::move(name)),
	url_(std::move(url)),
	platformType_(platformType),
	location_(location)
{
	// nothing to do here
}

inline const std::string& ExperiencesManager::SelectableExperience::name() const
{
	return name_;
}

inline const std::string& ExperiencesManager::SelectableExperience::url() const
{
	return url_;
}

inline ExperiencesManager::PlatformType ExperiencesManager::SelectableExperience::platformType() const
{
	return platformType_;
}

inline const Devices::GPSTracker::Location& ExperiencesManager::SelectableExperience::location() const
{
	return location_;
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_EXPERIENCES_MANAGER_H
