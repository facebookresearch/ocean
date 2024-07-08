/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_EXPERIENCES_EXPERIENCE_H
#define META_OCEAN_INTERACTION_EXPERIENCES_EXPERIENCE_H

#include "ocean/interaction/experiences/Experiences.h"

#include "ocean/interaction/UserInterface.h"

#include "ocean/rendering/Engine.h"

namespace Ocean
{

namespace Interaction
{

namespace Experiences
{

/**
 * This class is the base class for all experiences defined via the interaction plugin mechanism.
 * An experience can be anything which combines rendering with user input and sensor information like e.g., 6-DOF trackers.<br>
 * This base class mainly defines several event functions which can be used to realize an experience.
 * @ingroup interactionexperiences
 */
class OCEAN_INTERACTION_EXPERIENCES_EXPORT Experience
{
	friend class ExperiencesLibrary;

	public:

		/**
		 * Destructs the experience.
		 */
		virtual ~Experience() = default;

		/**
		 * Loads this experience.
		 * @param userInterface The application's UI elements
		 * @param engine Current engine
		 * @param timestamp Recent timestamp
		 * @param properties Optional properties for the experience
		 * @return True, if succeeded
		 */
		virtual bool load(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties = std::string());

		/**
		 * Unloads this experience.
		 * @see Library::unload().
		 */
		virtual bool unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp);

		/**
		 * Pre update interaction function.
		 * @see Library::preUpdate().
		 */
		virtual Timestamp preUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp);

		/**
		 * Post update interaction function.
		 * @see Library::postUpdate().
		 */
		virtual void postUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp);

		/**
		 * Mouse press event function.
		 * @see Library::onMousePress().
		 */
		virtual void onMousePress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp);

		/**
		 * Mouse move event function.
		 * @see Library::onMouseMove().
		 */
		virtual void onMouseMove(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp);

		/**
		 * Mouse release event function.
		 * @see Library::onMouseRelease().
		 */
		virtual void onMouseRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp);

		/**
		 * Key press function.
		 * @see Library::onKeyPress().
		 */
		virtual void onKeyPress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp);

		/**
		 * Key release function.
		 * @see Library::onKeyRelease().
		 */
		virtual void onKeyRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp);

	protected:

		/**
		 * Creates a new experience object.
		 */
		Experience() = default;

		/**
		 * Event function before the experience is loaded.
		 * @param userInterface The application's UI elements
		 * @param engine Current engine
		 * @param timestamp Recent timestmap
		 * @param properties Optional properties for the experience
		 * @return True, if succeeded
		 */
		virtual bool preLoad(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties);

		/**
		 * Event function after the experience is unloaded.
		 * @see Library::unload().
		 */
		virtual bool postUnload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp);

		/**
		 * Explicitly releases this experience.
		 */
		virtual void release();
};

} // namespace Experiences

} // namespace Interaction

} // namespace Ocean

#endif // META_OCEAN_INTERACTION_EXPERIENCES_EXPERIENCE_H
