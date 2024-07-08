/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/orca/ContentManager.h"

#include "ocean/base/String.h"

#include "ocean/interaction/Manager.h"

#include "ocean/io/File.h"
#include "ocean/io/ProjectFile.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"

#include "ocean/scenedescription/Manager.h"
#include "ocean/scenedescription/SDLScene.h"

namespace Ocean
{

namespace Orca
{

bool ContentManager::Content::load(const Timestamp timestamp, bool& cancel)
{
	FilenameSet filenameSet;
	return load(timestamp, filenameSet, cancel);
}

bool ContentManager::Content::unload(const Timestamp timestamp)
{
	switch (contentType_)
	{
		case CT_UNKNOWN:
			return true;

		case CT_PROJECT:
			return unloadProject(timestamp);

		case CT_INTERACTION:
			return unloadInteraction(timestamp);

		case CT_SCENE_DESCRIPTION:
			return unloadSceneDescription(timestamp);
	}

	ocean_assert(false && "Missing implementation");
	return false;
}

bool ContentManager::Content::load(const Timestamp timestamp, FilenameSet& filenameSet, bool& cancel)
{
	const IO::File file(filename_);

	if (file.exists() == false)
	{
		Log::error() << "The file \"" << filename_ << " could not be found.";
		return false;
	}

	const std::string extension(String::toLower(file.extension()));
	if (extension == "oproj")
	{
		return loadProject(timestamp, filenameSet, cancel);
	}

	if (Interaction::Manager::get().isSupported(extension))
	{
		return loadInteraction(timestamp, cancel);
	}

	if (SceneDescription::Manager::get().isSupported(extension))
	{
		return loadSceneDescription(timestamp, cancel);
	}

	Log::error() << "The specified file \"" << filename_ << "\" is neither a project file (*.oproj), nor an interaction file, nor a scene description file.";
	return false;
}

bool ContentManager::Content::loadProject(const Timestamp timestamp, FilenameSet& filenameSet, bool& cancel)
{
	ocean_assert(contentType_ == CT_UNKNOWN);
	contentType_ = CT_PROJECT;

	// add the project file to ensure that any project file will not be loaded recursively
	filenameSet.insert(filename_);

	const IO::ProjectFile projectFile(filename_);

	if (projectFile.filenames().empty())
	{
		Log::warning() << "The project file \"" << filename_ << "\" contains no files.";
		return true;
	}

	const IO::ProjectFile::Filenames filenames(projectFile.filenames());

	for (IO::ProjectFile::Filenames::const_iterator i = filenames.begin(); i != filenames.end(); ++i)
	{
		if (filenameSet.find(*i) == filenameSet.end())
		{
			childContents_.push_back(Content(*i, renderingEngine_, descriptionType_, contentId_));
		}
		else
		{
			Log::warning() << "The file \"" << *i << "\" has already been loaded during this load progress, thus it will be discarded.";
		}
	}

	unsigned int successfully = 0;

	for (Contents::iterator i = childContents_.begin(); i != childContents_.end(); ++i)
	{
		if (i->load(timestamp, filenameSet, cancel))
		{
			++successfully;
		}
	}

	Log::info() << "Finished project file loading and " << successfully << " of " << filenames.size() << " file(s) succeeded.";

	return successfully != 0;
}

bool ContentManager::Content::loadInteraction(const Timestamp timestamp, bool& /*cancel*/)
{
	ocean_assert(contentType_ == CT_UNKNOWN);
	contentType_ = CT_INTERACTION;

	const Interaction::UserInterface userInterface;

	if (Interaction::Manager::get().load(userInterface, renderingEngine_, timestamp, filename_))
		return true;

	Log::error() << "Failed to load application interaction file: \"" << filename_ << "\".";
	return false;
}

bool ContentManager::Content::loadSceneDescription(const Timestamp timestamp, bool& cancel)
{
	ocean_assert(contentType_ == CT_UNKNOWN);
	contentType_ = CT_SCENE_DESCRIPTION;

	ContentManager::get().progress_ = 0;

	const SceneDescription::SceneRef scene = SceneDescription::Manager::get().load(filename_, renderingEngine_, timestamp, descriptionType_, &ContentManager::get().progress_, &cancel);

	ContentManager::get().progress_ = 0;

	if (scene.isNull())
	{
		Log::error() << "Failed to load \"" << filename_ << "\".";
		return false;
	}

	Log::info() << "Successfully loaded \"" << filename_ << "\".";

	const Rendering::Engine::Framebuffers framebuffers(renderingEngine_->framebuffers());

	if (scene->descriptionType() == SceneDescription::TYPE_TRANSIENT)
	{
		const SceneDescription::SDLSceneRef sdlScene(scene);
		ocean_assert(sdlScene);

		renderingScene_ = sdlScene->apply(renderingEngine_);

		if (renderingScene_ && !framebuffers.empty())
		{
			// **TODO** should be done explicitly
			framebuffers.front()->addScene(renderingScene_);
		}
	}
	else
	{
		const SceneDescription::SDXSceneRef sdxScene(scene);
		ocean_assert(sdxScene);

		sceneId_ = sdxScene->sceneId();

		renderingScene_ = sdxScene->renderingObject();

		if (renderingScene_ && !framebuffers.empty())
		{
			// **TODO** should be done explicitly
			framebuffers.front()->addScene(renderingScene_);
		}
	}

	return true;
}

bool ContentManager::Content::unloadProject(const Timestamp timestamp)
{
	ocean_assert(contentType_ == CT_PROJECT);

	bool result = true;

	for (Contents::reverse_iterator i = childContents_.rbegin(); i != childContents_.rend(); ++i)
	{
		result = i->unload(timestamp) && result;
	}

	return result;
}

bool ContentManager::Content::unloadInteraction(const Timestamp timestamp)
{
	ocean_assert(contentType_ == CT_INTERACTION);

	const Interaction::UserInterface userInterface;

	return Interaction::Manager::get().unload(userInterface, renderingEngine_, timestamp, filename_);
}

bool ContentManager::Content::unloadSceneDescription(const Timestamp /*timestamp*/)
{
	ocean_assert(contentType_ == CT_SCENE_DESCRIPTION);

	if (renderingScene_)
	{
		// **TODO** should be done explicitly
		if (!renderingEngine_->framebuffers().empty())
		{
			renderingEngine_->framebuffers().front()->removeScene(renderingScene_);
		}
	}

	renderingScene_.release();

	if (sceneId_ != SceneDescription::invalidSceneId)
	{
		SceneDescription::Manager::get().unload(sceneId_);
	}

	return true;
}

ContentManager::ContentId ContentManager::Content::uniqueContentId()
{
	static Lock lock;
	static ContentId contentIdCounter = 0;

	const ScopedLock scopedLock(lock);

	++contentIdCounter;

	return contentIdCounter;
}

ContentManager::ContentManager() :
	Thread("ContentManager Thread")
{
	// nothing to do here
}

ContentManager::~ContentManager()
{
	release();
}

bool ContentManager::setSynchronizationType(const SynchronizationType& type)
{
	const ScopedLock scopedLock(lock_);

	if (synchronizationType_ == type)
	{
		return true;
	}

	if (!taskQueue_.empty())
	{
		return false;
	}

	synchronizationType_ = type;

	return true;
}

ContentManager::ContentId ContentManager::addContent(const std::string& filename, const Rendering::EngineRef& engine, const SceneDescription::DescriptionType preferredDescriptionType)
{
	ocean_assert(engine);
	if (engine.isNull())
	{
		return invalidContentId_;
	}

	if (engine->engineName() == "GLESceneGraph")
	{
		// **TODO** workaround as currently GLESceneGraph is not supporting multi-threading

		if (!setSynchronizationType(ST_SYNCHRONOUSLY))
		{
			return invalidContentId_;
		}
	}

	const ScopedLock scopedLock(lock_);

	// content can be added only if the manager is not in a cancel mode
	ocean_assert(cancel_ == false);

	const Content newContent(filename, engine, preferredDescriptionType);

	taskQueue_.push(Task(newContent));

	if (synchronizationType_ == ST_SYNCHRONOUSLY)
	{
		executeContentTask();
	}
	else
	{
		if (!isThreadActive())
		{
			startThread();
		}
	}

	return newContent.id();
}

ContentManager::ContentIds ContentManager::addContent(const Filenames& filenames, const Rendering::EngineRef& engine, const SceneDescription::DescriptionType preferredDescriptionType)
{
	ocean_assert(engine);

	if (engine.isNull())
	{
		return ContentIds();
	}

	if (engine->engineName() == "GLESceneGraph")
	{
		// **TODO** workaround as currently GLESceneGraph is not supporting multi-threading

		if (!setSynchronizationType(ST_SYNCHRONOUSLY))
		{
			return ContentIds();
		}
	}

	const ScopedLock scopedLock(lock_);

	// content can be added only if the manager is not in a cancel mode
	ocean_assert(cancel_ == false);

	ContentIds contentIds;

	for (Filenames::const_iterator i = filenames.begin(); i != filenames.end(); ++i)
	{
		const Content newContent(*i, engine, preferredDescriptionType);

		taskQueue_.push(Task(newContent));
		contentIds.push_back(newContent.id());
	}

	if (synchronizationType_ == ST_SYNCHRONOUSLY)
	{
		executeContentTask();
	}
	else
	{
		if (!isThreadActive())
		{
			startThread();
		}
	}

	return contentIds;
}

bool ContentManager::removeContent()
{
	const ScopedLock scopedLock(lock_);

	// clear the task queue
	taskQueue_ = TaskQueue();

	// if currently a load task is active
	if (activeTask_.isLoadTask())
	{
		taskQueue_.push(Task(activeTask_.loadContent().id()));
	}

	// add unload tasks for the loaded content
	for (Contents::reverse_iterator i = contents_.rbegin(); i != contents_.rend(); ++i)
	{
		taskQueue_.push(Task(i->id()));
	}

	// nothing to unload
	if (taskQueue_.empty())
	{
		return true;
	}

	if (synchronizationType_ == ST_SYNCHRONOUSLY)
	{
		executeContentTask();
	}
	else
	{
		if (!isThreadActive())
		{
			startThread();
		}
	}

	return true;
}

bool ContentManager::removeContent(const ContentId id)
{
	const ScopedLock scopedLock(lock_);

	bool found = false;
	for (Contents::iterator i = contents_.begin(); i != contents_.end(); ++i)
	{
		if (i->id() == id)
		{
			found = true;
			break;
		}
	}

	if (found == false)
	{
		if (!activeTask_.isLoadTask() || activeTask_.loadContent().id() != id)
		{
			return false;
		}
	}

	taskQueue_.push(Task(id));

	if (synchronizationType_ == ST_SYNCHRONOUSLY)
	{
		executeContentTask();
	}
	else
	{
		if (!isThreadActive())
		{
			startThread();
		}
	}

	return true;
}

bool ContentManager::removeContent(const ContentIds ids)
{
	bool result = true;

	const ScopedLock scopedLock(lock_);

	for (ContentIds::const_iterator i = ids.begin(); i != ids.end(); ++i)
	{
		result = removeContent(*i) && result;
	}

	return result;
}

ContentManager::ContentId ContentManager::removeContent(const std::string& filename)
{
	const ScopedLock scopedLock(lock_);

	for (Contents::iterator i = contents_.begin(); i != contents_.end(); ++i)
	{
		if (i->filename() == filename)
		{
			removeContent(i->id());

			return i->id();
		}
	}

	return invalidContentId_;
}

ContentManager::ContentIds ContentManager::removeContent(const Filenames& filenames)
{
	bool result = true;

	const ScopedLock scopedLock(lock_);

	ContentIds contentIds;

	for (Filenames::const_iterator iF = filenames.begin(); iF != filenames.end(); ++iF)
	{
		bool found = false;

		for (Contents::const_iterator i = contents_.begin(); i != contents_.end(); ++i)
		{
			if (i->filename() == *iF)
			{
				contentIds.push_back(i->id());
				result = removeContent(i->id()) && result;

				found = true;
				break;
			}
		}

		if (!found)
		{
			contentIds.push_back(invalidContentId_);
		}
	}

	return contentIds;
}

void ContentManager::setContentLoadedCallbackFunction(const ContentCallback& callback)
{
	const ScopedLock scopedLock(lock_);

	contentLoadedCallback_ = callback;
}

void ContentManager::setContentUnloadedCallbackFunction(const ContentCallback& callback)
{
	const ScopedLock scopedLock(lock_);

	contentUnloadedCallback_ = callback;
}

void ContentManager::cancel()
{
	cancel_ = true;

	{
		const ScopedLock scopedLock(lock_);

		taskQueue_ = TaskQueue();
	}

	stopThread();

	const Timestamp cancelTimestamp(true);
	while (isThreadActive() && Timestamp(true) < cancelTimestamp + timeout_)
	{
		sleep(1);
	}

	ocean_assert(!isThreadActive());

	cancel_ = false;
}

void ContentManager::release()
{
	removeContent();
	stopIfIdle_ = true;

	const Timestamp cancelTimestamp0(true);
	while (isThreadActive() && Timestamp(true) < cancelTimestamp0 + timeout_)
	{
		sleep(1);
	}

	cancel();

	const Timestamp cancelTimestamp1(true);
	while (isThreadActive() && Timestamp(true) < cancelTimestamp1 + timeout_)
	{
		sleep(1);
	}

	ocean_assert(!isThreadActive());
	stopIfIdle_ = false;
}

void ContentManager::threadRun()
{
	try
	{
		while (!shouldThreadStop())
		{
			if (executeContentTask() && stopIfIdle_)
			{
				return;
			}

			sleep(1);
		}
	}
	catch(...)
	{
		Log::error() << "Error during content loading or unloading!";
		activeTask_ = Task();
	}
}

bool ContentManager::executeContentTask()
{
	ocean_assert(activeTask_.isNull());

	{
		const ScopedLock scopedLock(lock_);

		if (taskQueue_.empty())
		{
			return true;
		}

		activeTask_ = taskQueue_.front();
		taskQueue_.pop();
	}

	if (activeTask_.isLoadTask())
	{
		loadContent(activeTask_);
	}
	else
	{
		unloadContent(activeTask_);
	}

	return false;
}

void ContentManager::loadContent(Task& task)
{
	ocean_assert(task.isLoadTask());

	Content content(task.loadContent());

	const bool result = content.load(Timestamp(true), cancel_);

	if (result)
	{
		const ScopedLock scopedLock(lock_);

		contents_.push_back(content);
		task = Task();
	}

	task = Task();

	ContentCallback contentCallback;

	{
		const ScopedLock scopedLock(lock_);
		contentCallback = contentLoadedCallback_;
	}

	if (contentCallback)
	{
		contentCallback(content.id(), result);
	}
}

void ContentManager::unloadContent(Task& task)
{
	ocean_assert(task.isUnloadTask());

	const ContentId contentId = task.unloadContent();

	bool result = false;

	{
		const ScopedLock scopedLock(lock_);

		for (Contents::iterator i = contents_.begin(); i != contents_.end(); ++i)
		{
			if (i->id() == contentId)
			{
				result = i->unload(Timestamp(true));

				contents_.erase(i);
				task = Task();
				break;
			}
		}
	}

	ContentCallback contentCallback;

	{
		const ScopedLock scopedLock(lock_);
		contentCallback = contentUnloadedCallback_;
	}

	if (contentCallback)
	{
		contentCallback(contentId, result);
	}
}

}

}
