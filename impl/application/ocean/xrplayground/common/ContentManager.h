// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_CONTENT_MANAGER_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_CONTENT_MANAGER_H

#include "application/ocean/xrplayground/XRPlayground.h"

#include "application/ocean/xrplayground/common/DownloadManager.h"

#include "ocean/base/Singleton.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/interaction/UserInterface.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/Scene.h"

#include "ocean/scenedescription/SceneDescription.h"

#include <functional>

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class implements the manager for the content loaded in XRPlayground.
 * @ingroup xrplayground
 */
class ContentManager : public Singleton<ContentManager>
{
	friend class Singleton<ContentManager>;

	public:

		/**
		 * Definition of individual load modes for content.
		 */
		enum LoadMode : uint32_t
		{
			/// Invalid load mode.
			LM_INVALID = 0u,
			/// The content will be loaded and rendered directly, existing content will be replaced.
			LM_LOAD_REPLACE,
			/// The content will be loaded and rendered directly, existing content will be untouched.
			LM_LOAD_ADD,
			/// The content will be loaded but not rendered, a callback function needs to be provided to apply custom handling.
			LM_LOAD_ONLY
		};

		/**
		 * Definition of a callback function which will be called once the content has been handled.
		 * @param content The content which has been handled
		 * @param succeeded True, if the content has been handled successfully; False, otherwise
		 * @param scenes The Rendering::Scene objects which have been created to handled the content
		 */
		typedef std::function<void(const std::string& content, const bool succeeded, const std::vector<Rendering::SceneRef>& scenes)> HandledCallback;

	protected:

		/**
		 * Definition of different types to handle content.
		 */
		enum ContentHandling : unsigned int
		{
			/// Invalid handling.
			CH_INVALID = 0u,
			/// Only loads the content so that it is ready for custom usage.
			CH_LOAD_ONLY,
			/// Loads new content and replaces any existing content.
			CH_LOAD_REPLACE,
			/// Loads new content and adds it to the existing content.
			CH_LOAD_ADD,
			/// Unloads previously loaded content.
			CH_UNLOAD
		};

		/**
		 * This class holding content to be handled.
		 */
		class ContentHandlingData
		{
			public:

				/**
				 * Default move constructor.
				 * @param contentHandlingData The object to be moved
				 */
				ContentHandlingData(ContentHandlingData&& contentHandlingData) = default;

				/**
				 * Creates a new content handling object.
				 * @param content The content to be handled, must be valid
				 * @param contentHandling The handling of the content, must be valid
				 * @param handledCallback The load callback function to be invoked once the content has been handled, invalid otherwise
				 */
				inline ContentHandlingData(std::string content, ContentHandling contentHandling, HandledCallback handledCallback);

			public:

				/// The content to be handled.
				std::string content_;

				/// The handling of the content.
				ContentHandling contentHandling_ = CH_INVALID;

				/// The load callback function to be invoked once the content has been handled, invalid otherwise.
				HandledCallback handledCallback_;
		};

		/**
		 * Definition of a queue holding content handling objects.
		 */
		typedef std::queue<ContentHandlingData> ContentHandlingQueue;

		/**
		 * This class defines a container for a single individual content element.
		 * A single content element can be a 3D asset or an interaction content.
		 */
		class ContentElement
		{
			public:

				/**
				 * Default move constructor.
				 * @param contentElement Content element to be moved
				 */
				inline ContentElement(ContentElement&& contentElement);

				/**
				 * Creates a new content element for a permanent scene.
				 * @param content The filename of the scene, must be valid
				 * @param permanentSceneId The id of the permanent scene, must be valid
				 * @param scene The permanent rendering scene, must be valid
				 */
				inline ContentElement(const std::string& content, const SceneDescription::SceneId permanentSceneId, const Rendering::SceneRef& scene);

				/**
				 * Creates a new content element for a transient scene.
				 * @param content The filename of the scene, must be valid
				 * @param scene The transient rendering scene, must be valid
				 */
				inline ContentElement(const std::string& content, const Rendering::SceneRef& scene);

				/**
				 * Creates a new content element for an interaction content.
				 * @param content The content description, must be valid
				 * @param interactionContent The interaction content, must be valid
				 */
				inline ContentElement(const std::string& content, const std::string& interactionContent);

				/**
				 * Destructs the content element.
				 * The actual content must have been unloaded before.
				 * @see unload().
				 */
				~ContentElement();

				/**
				 * Returns the rendering scene node of this content.
				 * @return The scene object associated with this content element, invalid if no scene object is associated
				 */
				inline Rendering::SceneRef scene() const;

				/**
				 * Unloads this content element.
				 * @param userInterface The application's user interface
				 * @param engine The rendering engine to be used
				 * @param framebuffer The framebuffer to which the content was associated
				 * @param timestamp The timestamp at which the content is unloaded
				 */
				void unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, Rendering::Framebuffer& framebuffer, const Timestamp& timestamp);

				/**
				 * Releases the content element and frees all assocated resource.
				 */
				void release();

				/**
				 * Move operator.
				 * @param contentElement Content element to be moved
				 * @return Reference to this object
				 */
				inline ContentElement& operator=(ContentElement&& contentElement);

			protected:

				/**
				 * Disabled copy constructor.
				 * @param contentElement Content element to be copied
				 */
				ContentElement(const ContentElement& contentElement) = delete;

				/**
				 * Disabled asign operator.
				 * @param contentElement Content element to be assigned
				 * @return Reference to this object
				 */
				ContentElement& operator=(const ContentElement& contentElement) = delete;

			protected:

				/// The actual content.
				std::string content_;

				/// The id of a permanent scene associated with the content; Invalid, if no permanent scene is associated.
				SceneDescription::SceneId permanentSceneId_ = SceneDescription::invalidSceneId;

				/// The rendering scene associated with the content; Invalid, if no scene is associated.
				Rendering::SceneRef scene_;

				/// The interaction content, empty if no interaction content is associated.
				std::string interactionContent_;
		};

		typedef std::vector<ContentElement> ContentElements;

		/**
		 * This class defines a container for contents.
		 * A content group can hold one or several content elements.
		 */
		class ContentGroup
		{
			public:

				/**
				 * Definition of individual content states.
				 */
				enum GroupState : uint32_t
				{
					/// The content group is invalid.
					GS_INVALID = 0u,
					/// The content group needs further processing.
					GS_NEEDS_PROCESSING,
					/// The content group is currently downloading the content.
					GS_DOWNLOADING,
					/// All content elements have been processed successfully.
					GS_SUCCEEDED
				};

			public:

				/**
				 * Default constructor.
				 */
				ContentGroup() = default;

				/**
				 * Default move constructor.
				 * @param contentGroup Content group to be moved
				 */
				ContentGroup(ContentGroup&& contentGroup) = default;

				/**
				 * Creates a new content group with the given content.
				 * @param content The content of the group, can be a local file, a C++ experience, or an asset id
				 * @param handledCallback The load callback function to be used, must be valid if 'loadOnly == true'
				 * @param loadOnly True, to only load the content so that it is ready for custom usage; False, to load and add the content to the framebuffer
				 */
				ContentGroup(const std::string& content, HandledCallback handledCallback, const bool loadOnly);

				/**
				 * Destructs a content group.
				 * The actual content must have been unloaded before.
				 * @see unload().
				 */
				~ContentGroup();

				/**
				 * Returns the current state of the group.
				 * @return The group's current state
				 */
				inline GroupState groupState() const;

				/**
				 * Returns teh content of the group.
				 * @return The group's content
				 */
				inline const std::string& content() const;

				/**
				 * Returns the handling callback function.
				 * @return The group's handling callback function, if any
				 */
				inline const HandledCallback& handledCallback() const;

				/**
				 * Processes the content of this content group.
				 * @param userInterface The application's user interface
				 * @param engine The rendering engine to be used
				 * @param framebuffer The framebuffer to which the content will be associated
				 * @param timestamp The timestamp at which the content is loaded
				 * @return The state of the group
				 */
				GroupState process(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, Rendering::Framebuffer& framebuffer, const Timestamp& timestamp);

				/**
				 * Unloads all content elements in this group.
				 * @param userInterface The application's user interface
				 * @param engine The rendering engine to be used
				 * @param framebuffer The framebuffer to which the content was associated
				 * @param timestamp The timestamp at which the content is unloaded
				 */
				void unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, Rendering::Framebuffer& framebuffer, const Timestamp& timestamp);

				/**
				 * Releases the content group and frees all assocated resource.
				 */
				void release();

				/**
				 * Returns whether this content group holds valid and active content.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Move operator.
				 * @param contentGroup Content group to be moved
				 * @return Reference to this object
				 */
				ContentGroup& operator=(ContentGroup&& contentGroup) = default;

				/**
				 * Disabled asign operator.
				 * @param contentGroup Content group to be assigned
				 * @return Reference to this object
				 */
				ContentGroup& operator=(const ContentGroup& contentGroup) = delete;

			protected:

				/**
				 * Disabled copy constructor.
				 * @param contentGroup Content group to be copied
				 */
				ContentGroup(const ContentGroup& contentGroup) = delete;

				/**
				 * Loads several content elements in the given order.
				 * @param contents The contents to load, at least one
				 * @param userInterface The application's user interface
				 * @param engine The rendering engine to be used
				 * @param framebuffer The framebuffer to which the content will be associated
				 * @param timestamp The timestamp at which the content is loaded
				 * @return True, if all contents could be loaded; False, if some content could not be loaded
				 */
				bool loadContentElements(const std::vector<std::string>& contents, const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, Rendering::Framebuffer& framebuffer, const Timestamp& timestamp);

			protected:

				/// The current state of the content group.
				GroupState groupState_ = GS_INVALID;

				/// The content's group major (e.g., a zip archive, a simple scene file, or a url).
				std::string groupContent_;

				/// The contents of the projects.
				ContentElements contentElements_;

				/// Optional temporary directory containing the content.
				IO::ScopedDirectory temporaryDirectory_;

				/// The future of the file in case this group content needs to be downloaded.
				std::future<IO::File> fileFuture_;

				/// Optional callback function for load events.
				HandledCallback handledCallback_;

				/// True, to only load the content so that it is ready for custom usage; False, to load and add the content to the framebuffer
				bool loadOnly_ = false;
		};

		/**
		 * Definition of an unordered map mapping major content to content groups.
		 */
		typedef std::unordered_map<std::string, ContentGroup> ContentGroupMap;

	public:

		/**
		 * Loads or adds new content e.g., a scene file, or interaction file, or a zip archive.
		 * The content is actually loaded the next time processContent() is invoked.
		 * @param content The new content to be loaded, must be valid
		 * @param loadMode The load mode for the specified content
		 * @param handledCallback Optional callback function which will be called once the content has been processed, nullptr otherwise
		 * @return True, if succeeded
		 */
		bool loadContent(const std::string& content, const LoadMode loadMode, HandledCallback handledCallback = nullptr);

		/**
		 * Unloads previously loaded content.
		 * The content is actually loaded the next time processContent() is invoked.
		 * @param content The content to be unloaded, must be valid
		 * @return True, if succeeded
		 */
		bool unloadContent(const std::string& content);

		/**
		 * Unloads all previously loaded content.
		 * The content is actually unloaded the next time processContent() is invoked.
		 */
		void unloadContent();

		/**
		 * Processes all pending content files.
		 * @param userInterface The application's user interface
		 * @param engine The renering engine to be used, must be valid
		 * @param framebuffer The framebuffer to which the content will be loaded or from which the content will be removed, must be valid
		 * @param timestamp The current timestamp, must be valid
		 * @return True, if all pending files could be loaded successfully
		 */
		bool processContent(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, Rendering::Framebuffer& framebuffer, const Timestamp& timestamp);

		/**
		 * Releases the content manager and frees all assocated resource.
		 * The content is not unloaded gracefully, whenever possible use unloadContent.
		 */
		void release();

	protected:

		/**
		 * Creates a new content manager object.
		 */
		ContentManager();

		/**
		 * Destructs the content manager object.
		 */
		~ContentManager();

		/**
		 * Parses a project file.
		 * @param projectFilename The filename of the project to be parsed, must be valid
		 * @param contentFiles The resulting content files of the project
		 * @return True, if succeeded
		 */
		static bool parseProjectFile(const std::string& projectFilename, std::vector<std::string>& contentFiles);

	protected:

		/// The map mapping major content to content groups.
		ContentGroupMap contentGroupMap_;

		/// The pending content that will be handled the next time content is processed.
		ContentHandlingQueue pendingContentQueue_;

		/// The pending content group which needs to be processed until any other content will be processed.
		ContentGroup pendingContentGroup_;

		/// The application's lock.
		mutable Lock lock_;
};

inline ContentManager::ContentHandlingData::ContentHandlingData(std::string content, ContentHandling contentHandling, HandledCallback handledCallback) :
	content_(std::move(content)),
	contentHandling_(contentHandling),
	handledCallback_(std::move(handledCallback))
{
	// nothing to do here
}

inline ContentManager::ContentElement::ContentElement(ContentElement&& contentElement)
{
	*this = std::move(contentElement);
}

inline ContentManager::ContentElement::ContentElement(const std::string& content, const SceneDescription::SceneId permanentSceneId, const Rendering::SceneRef& scene) :
	content_(content),
	permanentSceneId_(permanentSceneId),
	scene_(scene)
{
	ocean_assert(!content_.empty());
	ocean_assert(permanentSceneId_ != SceneDescription::invalidSceneId);
	ocean_assert(scene_);
}

inline ContentManager::ContentElement::ContentElement(const std::string& content, const Rendering::SceneRef& scene) :
	content_(content),
	scene_(scene)
{
	ocean_assert(!content_.empty());
	ocean_assert(scene_);
}

inline ContentManager::ContentElement::ContentElement(const std::string& content, const std::string& interactionContent) :
	content_(content),
	interactionContent_(interactionContent)
{
	ocean_assert(!content_.empty());
	ocean_assert(!interactionContent_.empty());
}

inline Rendering::SceneRef ContentManager::ContentElement::scene() const
{
	return scene_;
}

inline ContentManager::ContentElement& ContentManager::ContentElement::operator=(ContentElement&& contentElement)
{
	if (this != &contentElement)
	{
		content_ = std::move(contentElement.content_);

		permanentSceneId_ = contentElement.permanentSceneId_;
		contentElement.permanentSceneId_ = SceneDescription::invalidSceneId;

		scene_ = std::move(contentElement.scene_);
		interactionContent_ = std::move(contentElement.interactionContent_);
	}

	return *this;
}

inline ContentManager::ContentGroup::GroupState ContentManager::ContentGroup::groupState() const
{
	return groupState_;
}

inline const std::string& ContentManager::ContentGroup::content() const
{
	return groupContent_;
}

inline const ContentManager::HandledCallback& ContentManager::ContentGroup::handledCallback() const
{
	return handledCallback_;
}

inline bool ContentManager::ContentGroup::isValid() const
{
	return !contentElements_.empty() || !temporaryDirectory_.isValid();
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_CONTENT_MANAGER_H
