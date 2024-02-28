// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/ContentManager.h"
#include "application/ocean/xrplayground/common/DownloadManager.h"

#include "ocean/base/RandomI.h"

#include "ocean/io/FileResolver.h"
#include "ocean/io/JSONConfig.h"

#include "ocean/io/archive/ZipArchive.h"

#include "ocean/interaction/Manager.h"

#if defined(XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID) || defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
	#include "ocean/platform/android/Resource.h"
#endif

#include "ocean/scenedescription/Manager.h"
#include "ocean/scenedescription/SceneDescription.h"

namespace Ocean
{

namespace XRPlayground
{

ContentManager::ContentElement::~ContentElement()
{
	ocean_assert(content_.empty());
	ocean_assert(permanentSceneId_ == SceneDescription::invalidSceneId);
	ocean_assert(scene_.isNull());
	ocean_assert(interactionContent_.empty());
}

void ContentManager::ContentElement::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, Rendering::Framebuffer& framebuffer, const Timestamp& timestamp)
{
	if (permanentSceneId_ != SceneDescription::invalidSceneId)
	{
		SceneDescription::Manager::get().unload(permanentSceneId_);
		permanentSceneId_ = SceneDescription::invalidSceneId;
	}

	if (scene_)
	{
		framebuffer.removeScene(scene_);
		scene_.release();
	}

	if (!interactionContent_.empty())
	{
		Interaction::Manager::get().unload(userInterface, engine, timestamp, interactionContent_);
		interactionContent_.clear();
	}

	content_.clear();
}

void ContentManager::ContentElement::release()
{
	if (permanentSceneId_ != SceneDescription::invalidSceneId)
	{
		SceneDescription::Manager::get().unload(permanentSceneId_);
		permanentSceneId_ = SceneDescription::invalidSceneId;
	}

	scene_.release();

	interactionContent_.clear();

	content_.clear();
}

ContentManager::ContentGroup::~ContentGroup()
{
	ocean_assert(contentElements_.empty());
}

ContentManager::ContentGroup::ContentGroup(const std::string& content, HandledCallback handledCallback, const bool loadOnly) :
	groupContent_(content),
	handledCallback_(std::move(handledCallback)),
	loadOnly_(loadOnly)
{
	groupState_ = GS_INVALID;

	ocean_assert(!loadOnly_ || handledCallback);

	if (groupContent_.find("assetId:") == 0)
	{
		const std::string assetId = groupContent_.substr(8);

		if (!assetId.empty())
		{
			fileFuture_ = DownloadManager::get().downloadAsset(assetId);

			if (fileFuture_.valid())
			{
				groupState_ = GS_DOWNLOADING;
			}
			else
			{
				Log::error() << "Failed to download asset with id: " << assetId;
			}
		}
	}
	else
	{
		groupState_ = GS_NEEDS_PROCESSING;
	}
}

ContentManager::ContentGroup::GroupState ContentManager::ContentGroup::process(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, Rendering::Framebuffer& framebuffer, const Timestamp& timestamp)
{
	std::string localContent = groupContent_;

	if (groupState_ == GS_DOWNLOADING)
	{
		ocean_assert(fileFuture_.valid());

		if (fileFuture_.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
		{
			return groupState_;
		}

		const IO::File downloadedFile = fileFuture_.get();

		if (downloadedFile.isValid())
		{
			Log::info() << "Successful downloaded asset file " << downloadedFile.name();

			localContent = downloadedFile();
			groupState_ = GS_NEEDS_PROCESSING;
		}
		else
		{
			Log::error() << "Failed to download asset " << groupContent_;

			groupState_ = GS_INVALID;
			return groupState_;
		}
	}

	ocean_assert(groupState_ == GS_NEEDS_PROCESSING);

	groupState_ = GS_INVALID;

	std::vector<std::string> projectContent;

	const IO::Files resolvedFiles = IO::FileResolver::get().resolve(IO::File(localContent), true);

	if (!resolvedFiles.empty())
	{
		const IO::File& file = resolvedFiles.front();
		ocean_assert(file.exists());

		if (String::toLower(file.extension()) == "xrp")
		{
			if (!parseProjectFile(file(), projectContent))
			{
				Log::error() << "The project file '" << file.name() << "' could not be parsed";
			}
		}
		else if (String::toLower(file.extension()) == "zip")
		{
#if defined(XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID) || defined(XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST)
			ocean_assert(Platform::Android::ResourceManager::get().isValid());

			const IO::Directory externalDirectory(Platform::Android::ResourceManager::get().externalFilesDirectory());

			IO::Directory temporaryDirectory;

			for (unsigned int n = 0u; n < 1000u; ++n)
			{
				// trying to create a new unique temporary directory
				temporaryDirectory = externalDirectory + IO::Directory("contentmanager/zip/" + String::toAString(RandomI::random32()));

				if (!temporaryDirectory.exists())
				{
					break;
				}
			}

			if (!temporaryDirectory.exists() && temporaryDirectory.create())
#else
			const IO::Directory temporaryDirectory(IO::Directory::createTemporaryDirectory());

			if (temporaryDirectory.exists())
#endif
			{
				temporaryDirectory_ = IO::ScopedDirectory(temporaryDirectory);

				if (IO::Archive::ZipArchive::extractZipArchive(file(), temporaryDirectory(), false))
				{
					// now we need to parse the archive's project file

					const IO::File projectFile(temporaryDirectory + IO::File("experience.xrp"));

					if (projectFile.exists())
					{
						if (!parseProjectFile(projectFile(), projectContent))
						{
							Log::error() << "The archive's content files could not be parsed";
						}
					}
					else
					{
						Log::error() << "The zip archive does not contain a project file 'experience.xrp'";
					}
				}
				else
				{
					Log::error() << "Could not extract the zip archive file, '" << file() << "' to directory '" << temporaryDirectory() << "'";
				}
			}
			else
			{
				Log::error() << "Failed to create a temporary directory, '" << temporaryDirectory() << "', for the content" ;
			}
		}
		else
		{
			projectContent = {file()};
		}
	}
	else
	{
		projectContent = {localContent};
	}

	if (!projectContent.empty())
	{
		if (loadContentElements(projectContent, userInterface, engine, framebuffer, timestamp))
		{
			if (handledCallback_)
			{
				std::vector<Rendering::SceneRef> scenes;
				scenes.reserve(contentElements_.size());

				for (const ContentElement& contentElement : contentElements_)
				{
					Rendering::SceneRef scene = contentElement.scene();

					if (scene)
					{
						scenes.emplace_back(std::move(scene));
					}
				}

				handledCallback_(groupContent_, true, scenes);
			}

			groupState_ = GS_SUCCEEDED;
		}
	}

	return groupState_;
}

bool ContentManager::ContentGroup::loadContentElements(const std::vector<std::string>& contents, const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, Rendering::Framebuffer& framebuffer, const Timestamp& timestamp)
{
	ocean_assert(!contents.empty());

	bool allSucceeded = true;

	for (const std::string& content : contents)
	{
		IO::Files resolvedFiles;

		if (temporaryDirectory_.isValid())
		{
			resolvedFiles = IO::FileResolver::get().resolve(IO::File(content), temporaryDirectory_, true);
		}
		else
		{
			resolvedFiles = IO::FileResolver::get().resolve(IO::File(content), true);
		}

		try
		{
			if (!resolvedFiles.empty())
			{
				const IO::File& resolvedFile = resolvedFiles.front();
				ocean_assert(resolvedFile.exists());

				const SceneDescription::SceneRef scene = SceneDescription::Manager::get().load(resolvedFile(), engine, timestamp);

				if (scene)
				{
					if (scene->descriptionType() == SceneDescription::TYPE_PERMANENT)
					{
						const SceneDescription::SDXSceneRef sdxScene(scene);
						ocean_assert(sdxScene);

						const Rendering::SceneRef renderingScene(sdxScene->renderingScene());

						contentElements_.emplace_back(content, sdxScene->sceneId(), renderingScene);

						if (!loadOnly_)
						{
							framebuffer.addScene(renderingScene);
						}
					}
					else
					{
						const SceneDescription::SDLSceneRef sdlScene(scene);
						ocean_assert(sdlScene);

						const Rendering::SceneRef renderingScene(sdlScene->apply(engine));

						if (renderingScene)
						{
							contentElements_.emplace_back(content, renderingScene);

							if (!loadOnly_)
							{
								framebuffer.addScene(renderingScene);
							}
						}
						else
						{
							Log::error() << "Failed to apply the scene description to the rendering engine";
							allSucceeded = false;
						}
					}

					continue;
				}

				if (Interaction::Manager::get().load(userInterface, engine, timestamp, resolvedFile()))
				{
					contentElements_.emplace_back(content, resolvedFile());
					continue;
				}
			}

			if (Interaction::Manager::get().load(userInterface, engine, timestamp, content))
			{
				contentElements_.emplace_back(content, content);
			}
			else
			{
				if (resolvedFiles.empty())
				{
					Log::error() << "Could not load content element '" << content << "' which is not a file";
				}
				else
				{
					Log::error() << "Could not load content element '" << content << "' which is a file";
				}

				allSucceeded = false;
			}
		}
		catch (const std::exception& exception)
		{
			Log::error() << exception.what();

			allSucceeded = false;
		}
		catch (...)
		{
			Log::error() << "Uncaught exception occured!";

			allSucceeded = false;
		}
	}

	return allSucceeded;
}

void ContentManager::ContentGroup::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, Rendering::Framebuffer& framebuffer, const Timestamp& timestamp)
{
	// we unload the content in reverse order

	for (ContentElements::reverse_iterator iContent = contentElements_.rbegin(); iContent != contentElements_.rend(); ++iContent)
	{
		iContent->unload(userInterface, engine, framebuffer, timestamp);
	}

	contentElements_.clear();

	release();
}

void ContentManager::ContentGroup::release()
{
	// we unload the content in reverse order

	for (ContentElements::reverse_iterator iContent = contentElements_.rbegin(); iContent != contentElements_.rend(); ++iContent)
	{
		iContent->release();
	}

	contentElements_.clear();

	groupContent_.clear();
}

ContentManager::ContentManager()
{
	// nothing to do here
}

ContentManager::~ContentManager()
{
	ocean_assert(contentGroupMap_.empty());
	ocean_assert(pendingContentQueue_.empty());
}

bool ContentManager::loadContent(const std::string& content, const LoadMode loadMode, HandledCallback handledCallback)
{
	if (content.empty())
	{
		ocean_assert(false && "Invalid content!");
		return false;
	}

	ContentHandling contentHandling = CH_INVALID;

	switch (loadMode)
	{
		case LM_INVALID:
			ocean_assert(false && "Invalid load mode!");
			return false;

		case LM_LOAD_ADD:
			contentHandling = CH_LOAD_ADD;
			break;

		case LM_LOAD_REPLACE:
			contentHandling = CH_LOAD_REPLACE;
			break;

		case LM_LOAD_ONLY:
			if (!handledCallback)
			{
				Log::error() << "Content manager is supposed to load content '" << content << "' without adding it to framebuffer, but no callback function was provided";
				return false;
			}

			contentHandling = CH_LOAD_ONLY;
			break;
	};

	if (contentHandling == CH_INVALID)
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	pendingContentQueue_.emplace(std::string(content), contentHandling, std::move(handledCallback));

	return true;
}

bool ContentManager::unloadContent(const std::string& content)
{
	Log::debug() << "ContentManager::unloadContent(): " << content;

	if (content.empty())
	{
		ocean_assert(false && "Invalid content!");
		return false;
	}

	const ScopedLock scopedLock(lock_);

	pendingContentQueue_.emplace(content, CH_UNLOAD, nullptr);

	return true;
}

void ContentManager::unloadContent()
{
	Log::debug() << "ContentManager::unloadContent()";

	const ScopedLock scopedLock(lock_);

	pendingContentQueue_ = ContentHandlingQueue();

	pendingContentQueue_.emplace(std::string(), CH_UNLOAD, nullptr);
}

bool ContentManager::processContent(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, Rendering::Framebuffer& framebuffer, const Timestamp& timestamp)
{
	ocean_assert(engine && timestamp.isValid());

	bool allSucceeded = true;

	while (true)
	{
		TemporaryScopedLock scopedLock(lock_);

			if (pendingContentGroup_.groupState() != ContentGroup::GS_INVALID)
			{
				pendingContentGroup_.process(userInterface, engine, framebuffer, timestamp);

				if (pendingContentGroup_.groupState() == ContentGroup::GS_SUCCEEDED)
				{
					std::string content = pendingContentGroup_.content();

					contentGroupMap_.emplace(std::move(content), std::move(pendingContentGroup_));
					pendingContentGroup_ = ContentGroup();
				}
				else if (pendingContentGroup_.groupState() == ContentGroup::GS_INVALID)
				{
					if (pendingContentGroup_.handledCallback())
					{
						pendingContentGroup_.handledCallback()(pendingContentGroup_.content(), false, std::vector<Rendering::SceneRef>());
					}

					allSucceeded = false;
					pendingContentGroup_ = ContentGroup();
				}

				return allSucceeded;
			}

			if (pendingContentQueue_.empty())
			{
				break;
			}

			ContentHandlingData contentHandlingData(std::move(pendingContentQueue_.front()));
			pendingContentQueue_.pop();

		scopedLock.release();

		if (contentHandlingData.contentHandling_ == CH_UNLOAD)
		{
			if (contentHandlingData.content_.empty())
			{
				// we unload every content

				for (ContentGroupMap::iterator iContentGroup = contentGroupMap_.begin(); iContentGroup != contentGroupMap_.cend(); ++iContentGroup)
				{
					iContentGroup->second.unload(userInterface, engine, framebuffer, timestamp);
				}

				contentGroupMap_.clear();
			}
			else
			{
				const ContentGroupMap::iterator iContentGroup = contentGroupMap_.find(contentHandlingData.content_);
				if (iContentGroup != contentGroupMap_.cend())
				{
					iContentGroup->second.unload(userInterface, engine, framebuffer, timestamp);
					contentGroupMap_.erase(iContentGroup);
				}
			}

			continue;
		}

		if (contentHandlingData.contentHandling_ == CH_LOAD_REPLACE)
		{
			for (ContentGroupMap::iterator iContentGroup = contentGroupMap_.begin(); iContentGroup != contentGroupMap_.cend(); ++iContentGroup)
			{
				iContentGroup->second.unload(userInterface, engine, framebuffer, timestamp);
			}

			contentGroupMap_.clear();
		}
		else
		{
			ocean_assert(contentHandlingData.contentHandling_ == CH_LOAD_ADD);
		}

		ocean_assert(!contentHandlingData.content_.empty());

		if (contentGroupMap_.find(contentHandlingData.content_) != contentGroupMap_.cend())
		{
			Log::info() << "The content '" << contentHandlingData.content_ << "' is already loaded";
			continue;
		}

		ocean_assert(pendingContentGroup_.groupState() == ContentGroup::GS_INVALID);
		pendingContentGroup_ = ContentGroup(contentHandlingData.content_, std::move(contentHandlingData.handledCallback_), contentHandlingData.contentHandling_ == CH_LOAD_ONLY ? true : false);

		if (pendingContentGroup_.groupState() == ContentGroup::GS_INVALID)
		{
			if (pendingContentGroup_.handledCallback())
			{
				pendingContentGroup_.handledCallback()(pendingContentGroup_.content(), false, std::vector<Rendering::SceneRef>());
			}

			allSucceeded = false;
		}
	}

	return allSucceeded;
}

void ContentManager::release()
{
	const ScopedLock scopedLock(lock_);

	for (ContentGroupMap::iterator iContentGroup = contentGroupMap_.begin(); iContentGroup != contentGroupMap_.cend(); ++iContentGroup)
	{
		iContentGroup->second.release();
	}

	contentGroupMap_.clear();

	pendingContentQueue_ = ContentHandlingQueue();
}

bool ContentManager::parseProjectFile(const std::string& projectFilename, std::vector<std::string>& contentFiles)
{
	/**
	 * parsing a project file with the follwing pattern
	 *
	 * {
	 *     "project" : [
	 *         {
	 *             "content" : "first_content_file_to_load"
	 *         },
	 *         {
	 *             "content" : "second_content_file_to_load"
	 *         }
	 *     ]
	 * }
	 */

	IO::JSONConfig config(projectFilename, true);

	const unsigned int numberContentFiles = config.values("project");

	if (numberContentFiles == 0u)
	{
		return false;
	}

	std::vector<std::string> internalContentFiles;
	internalContentFiles.reserve(numberContentFiles);

	const std::string invalidContent("NOT_A_STRING");

	for (unsigned int n = 0u; n < numberContentFiles; ++n)
	{
		std::string contentFile = config.value("project", n)["content"](invalidContent);

		if (contentFile == invalidContent)
		{
			return false;
		}

		internalContentFiles.emplace_back(std::move(contentFile));
	}

	contentFiles = std::move(internalContentFiles);

	return true;
}

}

}
