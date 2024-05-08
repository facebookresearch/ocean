/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_ORCA_CONTENT_MANAGER_H
#define FACEBOOK_APPLICATION_OCEAN_ORCA_CONTENT_MANAGER_H

#include "application/ocean/orca/Orca.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

#include "ocean/rendering/Engine.h"

#include "ocean/scenedescription/SDXScene.h"

#include <queue>
#include <vector>

namespace Ocean
{

namespace Orca
{

/**
 * This class implements a content manager implemented as singleton using an own thread.
 * @ingroup orca
 */
class ContentManager :
	public Singleton<ContentManager>,
	protected Thread
{
	friend class Singleton<ContentManager>;
	friend class Content;

	public:

		/**
		 * Defines individual synchronization types.
		 */
		enum SynchronizationType
		{
			/// Synchronously content management.
			ST_SYNCHRONOUSLY,
			/// Asynchronously content management.
			ST_ASYNCHRONOUSLY
		};

		/**
		 * Definition of a content id.
		 */
		typedef unsigned int ContentId;

		/**
		 * Definition of an invalid content id.
		 */
		static constexpr ContentId invalidContentId_ = ContentId(-1);

		/**
		 * Definition of a vector holding content ids.
		 */
		typedef std::vector<ContentId> ContentIds;

		/**
		 * Definition of a callback function for loaded content events.
		 * The first parameter holds the unique content id
		 * The second parameter states whether the content could be loaded successfully
		 */
		typedef Callback<void, const ContentId, const bool> ContentCallback;

		/**
		 * Definition of a vector holding filenames.
		 */
		typedef std::vector<std::string> Filenames;

	private:

		// Forward declaration.
		class Content;

		/**
		 * Defines a vector holding content objects.
		 */
		typedef std::vector<Content> Contents;

		/**
		 * Definition of a content object holding all necessary information.
		 */
		class Content
		{
			private:

				/**
				 * Definition of a set holding filenames.
				 */
				typedef std::set<std::string> FilenameSet;

				/**
				 * Definition of different content types.
				 */
				enum ContentType
				{
					/// Unknown content type.
					CT_UNKNOWN,
					/// Project content.
					CT_PROJECT,
					/// Interaction content.
					CT_INTERACTION,
					/// Scene description content.
					CT_SCENE_DESCRIPTION
				};

			public:

				/**
				 * Creates an empty content object.
				 */
				Content() = default;

				/**
				 * Creates a new content object.
				 * @param filename The filename of the content to be loaded
				 * @param engine Rendering engine to be used for the content, if any
				 * @param type Preferred description type
				 * @param parentContentId Id of the parent content object
				 */
				inline Content(const std::string& filename, const Rendering::EngineRef& engine, const SceneDescription::DescriptionType type, const ContentId parentContentId = invalidContentId_);

				/**
				 * Returns the scene filename of this object.
				 * @return Scene filename
				 */
				inline const std::string& filename() const;

				/**
				 * Returns the rendering engine of this object, if any.
				 * @return Rendering engine
				 */
				inline const Rendering::SceneRef& renderingScene() const;

				/**
				 * Returns the scene reference of this object, if any.
				 * @return Scene reference
				 */
				inline SceneDescription::SceneId sceneId() const;

				/**
				 * Returns the scene description type of this object.
				 * @return Scene description type
				 */
				inline SceneDescription::DescriptionType type() const;

				/**
				 * Returns the content id of this object.
				 * @return Content id
				 */
				inline ContentId id() const;

				/**
				 * Loads the content.
				 * @param timestamp Load timestamp
				 * @param cancel State to cancel the load progress
				 * @return True, if succeeded
				 */
				bool load(const Timestamp timestamp, bool& cancel);

				/**
				 * Unloads the content.
				 * @param timestamp Unload timestamp
				 * @return True, if succeeded
				 */
				bool unload(const Timestamp timestamp);

				/**
				 * Returns whether the content is empty.
				 * @return True, if so
				 */
				inline bool isNull() const;

			private:

				/**
				 * Loads the content.
				 * @param timestamp Load timestamp
				 * @param cancel State to cancel the load progress
				 * @param filenameSet Set of filenames already loaded within this progress
				 * @return True, if succeeded
				 */
				bool load(const Timestamp timestamp, FilenameSet& filenameSet, bool& cancel);

				/**
				 * Loads a project file.
				 * @param timestamp Load timestamp
				 * @param filenameSet Set of filenames already loaded within this progress
				 * @param cancel State to cancel the load progress
				 * @return True, if succeeded
				 */
				bool loadProject(const Timestamp timestamp, FilenameSet& filenameSet, bool& cancel);

				/**
				 * Loads an interaction file.
				 * @param timestamp Load timestamp
				 * @param cancel State to cancel the load progress
				 * @return True, if succeeded
				 */
				bool loadInteraction(const Timestamp timestamp, bool& cancel);

				/**
				 * Loads a scene description file.
				 * @param timestamp Load timestamp
				 * @param cancel State to cancel the load progress
				 * @return True, if succeeded
				 */
				bool loadSceneDescription(const Timestamp timestamp, bool& cancel);

				/**
				 * Unloads a project.
				 * @param timestamp Unload timestamp
				 * @return True, if succeeded
				 */
				bool unloadProject(const Timestamp timestamp);

				/**
				 * Unloads an interaction.
				 * @param timestamp Unload timestamp
				 * @return True, if succeeded
				 */
				bool unloadInteraction(const Timestamp timestamp);

				/**
				 * Unloads a scene description.
				 * @param timestamp Unload timestamp
				 * @return True, if succeeded
				 */
				bool unloadSceneDescription(const Timestamp timestamp);

				/**
				 * Returns a unique content id.
				 * Unique content id.
				 */
				static ContentId uniqueContentId();

			private:

				/// Scene filename.
				std::string filename_;

				/// Rendering engine.
				Rendering::EngineRef renderingEngine_;

				/// Rendering scene object.
				Rendering::SceneRef renderingScene_;

				/// Scene description object.
				SceneDescription::SceneId sceneId_ = SceneDescription::invalidSceneId;

				/// Description type.
				SceneDescription::DescriptionType descriptionType_ = SceneDescription::TYPE_PERMANENT;

				/// Content id.
				ContentId contentId_ = invalidContentId_;

				/// Parent content id.
				ContentId parentContentId_ = invalidContentId_;

				/// Content type.
				ContentType contentType_ = CT_UNKNOWN;

				/// Child
				Contents childContents_;
		};

		/**
		 * Defines a task object.
		 */
		class Task
		{
			public:

				/**
				 * Creates an empty task.
				 */
				inline Task();

				/**
				 * Creates a task object loading content.
				 * @param content The content to be loaded
				 */
				inline explicit Task(const Content& content);

				/**
				 * Creates a task object unloaded content.
				 * @param contentId Id of the content to be unloaded
				 */
				inline explicit Task(const ContentId contentId);

				/**
				 * Returns the content to be loaded.
				 * @return Content to be loaded
				 */
				inline Content& loadContent();

				/**
				 * Returns the content id of the content to be unloaded.
				 * @return Id of the content to be unloaded
				 */
				inline ContentId unloadContent() const;

				/**
				 * Returns whether this task is a load task.
				 * @return True, if so
				 */
				inline bool isLoadTask() const;

				/**
				 * Returns whether this task is an unload task.
				 * @return True, if so
				 */
				inline bool isUnloadTask() const;

				/**
				 * Returns whether the task is empty.
				 * @return True, if so
				 */
				inline bool isNull() const;

			private:

				/// Content to be loaded.
				Content content_;

				/// Content to be unloaded.
				ContentId contentId_;
		};

		/**
		 * Definition of a queue holding content objects.
		 */
		typedef std::queue<Task> TaskQueue;

	public:

		/**
		 * Returns the synchronization type of the content manager.
		 * @return Current synchronization type, ST_ASYNCHRONOUSLY is default
		 */
		SynchronizationType synchronizationType() const;

		/**
		 * Sets the synchronization type of the content manager.
		 * @param type The type to be set
		 * @return True, if succeeded
		 */
		bool setSynchronizationType(const SynchronizationType& type);

		/**
		 * Adds a new file to the content queue.
		 * @param filename Name of the file to be added
		 * @param engine Rendering engine used for rendering
		 * @param preferredDescriptionType Preferred scene description type
		 * @return Unique id connected with the content allowing a unique identification in the event callback function
		 * @see SceneDescription::load() for more details about the parameters.
		 */
		ContentId addContent(const std::string& filename, const Rendering::EngineRef& engine, const SceneDescription::DescriptionType preferredDescriptionType);

		/**
		 * Adds a new file to the content queue.
		 * @param filenames Names of the files to be added
		 * @param engine Rendering engine used for rendering
		 * @param preferredDescriptionType Preferred scene description type
		 * @return Unique id connected with the content allowing a unique identification in the event callback function
		 * @see SceneDescription::load() for more details about the parameters.
		 */
		ContentIds addContent(const Filenames& filenames, const Rendering::EngineRef& engine, const SceneDescription::DescriptionType preferredDescriptionType);

		/**
		 * Removes all loaded content.
		 * Content pending to be loaded or currently in loading process is untouched.
		 * @return True, if succeeded
		 */
		bool removeContent();

		/**
		 * Removes a specified content by it's unique id.
		 * @param id Unique id of the content to be removed
		 * @return True, if the content existed and has been removed
		 */
		bool removeContent(const ContentId id);

		/**
		 * Removes a specified content by their unique ids.
		 * @param ids Unique ids of the content to be removed
		 * @return True, if the content existed and has been removed
		 */
		bool removeContent(const ContentIds ids);

		/**
		 * Removes a specified content by the name of main content file.
		 * @param filename The filename of the content to be removed
		 * @return Corresponding content id
		 */
		ContentId removeContent(const std::string& filename);

		/**
		 * Removes a specified content by their name of the main content files.
		 * @param filenames The filenames of the content to be removed
		 * @return Corresponding content ids
		 */
		ContentIds removeContent(const Filenames& filenames);

		/**
		 * Sets / changes or removes the callback function for content loaded events.
		 * @param callback The callback function to be set
		 */
		void setContentLoadedCallbackFunction(const ContentCallback& callback);

		/**
		 * Sets / changes or removes the callback function for content unloaded events.
		 * @param callback The callback function to be set
		 */
		void setContentUnloadedCallbackFunction(const ContentCallback& callback);

		/**
		 * Cancels the current load process and removes the queued content.
		 * The function does not return before the queued content is canceled.<br>
		 * Already loaded content is left untouched.
		 */
		void cancel();

		/**
		 * Releases the content manager.
		 * This call will cancel all load operations immediately and will release all loaded content afterwards.
		 * The function does not return before all content is released.
		 */
		void release();

		/**
		 * Returns the progress of the file manager.
		 * Use the progress state to get the progress of e.g. load operations.
		 * @return Progress state with range [0, 1]
		 */
		inline float progress() const;

	private:

		/**
		 * Creates a new content manager object.
		 */
		ContentManager();

		/**
		 * Destructs a content manager object.
		 */
		~ContentManager() override;

		/**
		 * Thread function.
		 * @see Thread::threadRun().
		 */
		void threadRun();

		/**
		 * Executes a content task.
		 * @return True, if the last task has been executed
		 */
		bool executeContentTask();

		/**
		 * Loads a given content object.
		 * @param task Load task
		 */
		void loadContent(Task& task);

		/**
		 * Unloads a given content object.
		 * @param task Unload task
		 */
		void unloadContent(Task& task);

	private:

		/// Queue holding all pending scenes to be loaded.
		TaskQueue taskQueue_;

		/// Active task.
		Task activeTask_;

		/// Content loaded callback function.
		ContentCallback contentLoadedCallback_;

		/// Content unloaded callback function.
		ContentCallback contentUnloadedCallback_;

		/// Successfully loaded content objects.
		Contents contents_;

		/// Synchronization type of this manager.
		SynchronizationType synchronizationType_ = ST_ASYNCHRONOUSLY;

		/// Progress state of load operations.
		float progress_ = 0.0f;

		/// Queue lock.
		Lock lock_;

		/// Cancel state.
		bool cancel_ = false;

		/// State to stop the manager thread if in an idle state.
		bool stopIfIdle_ = false;

		/// Timeout of the manager in seconds.
		double timeout_ = 5.0;
};

inline ContentManager::Content::Content(const std::string& filename, const Rendering::EngineRef& engine, const SceneDescription::DescriptionType type, const ContentId parentContentId) :
	filename_(filename),
	renderingEngine_(engine),
	descriptionType_(type),
	contentId_(uniqueContentId()),
	parentContentId_(parentContentId)
{
	// nothing to do here
}

inline const std::string& ContentManager::Content::filename() const
{
	return filename_;
}

inline const Rendering::SceneRef& ContentManager::Content::renderingScene() const
{
	return renderingScene_;
}

inline SceneDescription::SceneId ContentManager::Content::sceneId() const
{
	return sceneId_;
}

inline SceneDescription::DescriptionType ContentManager::Content::type() const
{
	return descriptionType_;
}

inline ContentManager::ContentId ContentManager::Content::id() const
{
	return contentId_;
}

inline bool ContentManager::Content::isNull() const
{
	return filename_.empty();
}

inline float ContentManager::progress() const
{
	return progress_;
}

inline ContentManager::Task::Task() :
	content_(Content()),
	contentId_(invalidContentId_)
{
	// nothing to do here
}

inline ContentManager::Task::Task(const Content& content) :
	content_(content),
	contentId_(invalidContentId_)
{
	// nothing to do here
}

inline ContentManager::Task::Task(const ContentId contentId) :
	content_(Content()),
	contentId_(contentId)
{
	// nothing to do here
}

inline ContentManager::Content& ContentManager::Task::loadContent()
{
	return content_;
}

inline ContentManager::ContentId ContentManager::Task::unloadContent() const
{
	return contentId_;
}

inline bool ContentManager::Task::isLoadTask() const
{
	return !content_.isNull();
}

inline bool ContentManager::Task::isUnloadTask() const
{
	return contentId_ != invalidContentId_;
}

inline bool ContentManager::Task::isNull() const
{
	return content_.isNull() && contentId_ == invalidContentId_;
}

inline ContentManager::SynchronizationType ContentManager::synchronizationType() const
{
	return synchronizationType_;
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_ORCA_CONTENT_MANAGER_H
