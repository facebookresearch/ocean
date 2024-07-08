/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_EXPERIENCES_EXPERIENCES_LIBRARY_H
#define META_OCEAN_INTERACTION_EXPERIENCES_EXPERIENCES_LIBRARY_H

#include "ocean/interaction/experiences/Experiences.h"
#include "ocean/interaction/experiences/Experience.h"

#include "ocean/base/Singleton.h"

#include "ocean/interaction/Library.h"

#include <functional>

namespace Ocean
{

namespace Interaction
{

namespace Experiences
{

/**
 * This class implements the experiences interaction library object.
 * @ingroup interactionexperiences
 */
class OCEAN_INTERACTION_EXPERIENCES_EXPORT ExperiencesLibrary : public Library
{
	public:

		/**
		 * Definition of a unique pointer to an Experience object.
		 */
		typedef std::unique_ptr<Experience> UniqueExperience;

		/**
		 * Definition of a callback function allowing to create a new experience.
		 */
		typedef std::function<UniqueExperience()> CreateExperienceFunction;

	protected:

		/**
		 * Internal helper class holding the instance of this library.
		 */
		class Instance : public Singleton<Instance>
		{
			friend class Singleton<Instance>;

			public:

				/**
				 * Returns the pointer to the instance of the experience library.
				 * @return The experience library, nullptr if not yet set
				 */
				ExperiencesLibrary*& experiencesLibrary();

				/**
				 * Returns the lock.
				 * @return The lock
				 */
				Lock& lock();

			protected:

				/**
				 * Creates a new object with invalid instance.
				 */
				Instance();

			protected:

				/// The pointer to the instance of the experience library.
				ExperiencesLibrary* experiencesLibrary_ = nullptr;

				/// Lock to make this class thread safe
				Lock lock_;
		};

		/**
		 * Definition of a vector holding experiences.
		 */
		typedef std::unordered_map<std::string, UniqueExperience> NameToExperienceMap;

		/**
		 * Definition of an unordered map mapping experience names to experience create functions.
		 */
		typedef std::unordered_map<std::string, CreateExperienceFunction> NameToFunctionMap;

	public:

		/**
		 * Creates this library and registers it at the global interaction manager.
		 * Do not register this library if using it as plugin, because it's done by the plugin itself.<br>
		 * However, if you are not using the plugin mechanism you have to initialize this library once at program initialization.<br>
		 * If the library is not used anymore unregister it using the unregister function.<br>
		 * Beware: This registration must not be done more than once!<br>
		 * @see Manager, unregisterLibrary()
		 */
		static void registerLibrary();

		/**
		 * Unregisters this library at the global interaction manager.
		 * Do not unregister this library if using it as plugin, because it's done by the plugin itself when the plugin is unloaded.<br>
		 * Beware: This registration must not be done more than once and must not be done without previous registration!<br>
		 * @return True, if succeeded
		 * @see Manager, registerLibrary()
		 */
		static bool unregisterLibrary();

		/**
		 * Registers a new experience without actually creating the experience.
		 * @param experienceName The name of the experience to register, must be valid
		 * @param createExperienceFunction The create function of the new experience, must be valid
		 * @return True, if succeeded
		 */
		static bool registerNewExperience(std::string experienceName, CreateExperienceFunction createExperienceFunction);

	protected:

		/**
		 * Creates a new experiences library object.
		 */
		ExperiencesLibrary();

		/**
		 * Destructs a experiences library object.
		 */
		~ExperiencesLibrary() override;

		/**
		 * Loads a new interaction file.
		 * @see Library::load().
		 */
		bool load(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename) override;

		/**
		 * Unloads one specific loaded interaction files.
		 * @see Library::unload().
		 */
		bool unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename) override;

		/**
		 * Unloads all loaded scripts.
		 * @see Library::unload
		 */
		void unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Pre file load interaction function.
		 * @see Library::preFileLoad().
		 */
		void preFileLoad(const UserInterface& userInterface, const std::string& filename) override;

		/**
		 * Post file load interaction function.
		 * @see Library::postFileLoad().
		 */
		void postFileLoad(const UserInterface& userInterface, const std::string& filename, const bool succeeded) override;

		/**
		 * Pre update interaction function.
		 * @see Library::preUpdate().
		 */
		Timestamp preUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Post update interaction function.
		 * @see Library::postUpdate().
		 */
		void postUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Mouse press event function.
		 * @see Library::onMousePress().
		 */
		void onMousePress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp) override;

		/**
		 * Mouse move event function.
		 * @see Library::onMouseMove().
		 */
		void onMouseMove(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp) override;

		/**
		 * Mouse release event function.
		 * @see Library::onMouseRelease().
		 */
		void onMouseRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp) override;

		/**
		 * Key press function.
		 * @see Library::onKeyPress().
		 */
		void onKeyPress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

		/**
		 * Key release function.
		 * @see Library::onKeyRelease().
		 */
		void onKeyRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

		/**
		 * Releases this library.
		 */
		void release() override;

	protected:

		/// The map mapping names of experiences to create functions.
		NameToFunctionMap nameToFunctionMap_;

		/// The map mapping names of experiences to actual instances of experiences.
		NameToExperienceMap nameToExperienceMap_;

		/// The library's lock.
		Lock lock_;
};

}

}

}

#endif // META_OCEAN_INTERACTION_EXPERIENCES_EXPERIENCES_LIBRARY_H
