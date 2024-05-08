/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/Manager.h"
#include "ocean/scenedescription/SDXEventNode.h"
#include "ocean/scenedescription/SDXUpdateNode.h"

#include "ocean/io/File.h"
#include "ocean/io/FileResolver.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace SceneDescription
{

Manager::Manager()
{
	// nothing to do here
}

Manager::~Manager()
{
	const ScopedLock scopedLock(managerLock_);

	ocean_assert(permanentSceneMap_.empty());
	ocean_assert(libraries_.empty() && "Manager::release() has to be called once before the termination of this object.");

	release();
}

SceneRef Manager::load(const std::string& filename, const Rendering::EngineRef& engine, const Timestamp& timestamp, const DescriptionType preferredDescriptionType, float* progress, bool* cancel)
{
	ocean_assert(timestamp.isValid());
	ocean_assert((preferredDescriptionType & TYPE_PERMANENT) || (preferredDescriptionType & TYPE_TRANSIENT));

	const IO::File file(filename);
	IO::Files files(IO::FileResolver::get().resolve(file));

	if (file.exists())
	{
		files.push_back(file);
	}

	const ScopedLock lock(libraryLock_);

	bool invalidFilename = true;

	for (IO::Files::const_iterator iF = files.begin(); iF != files.end(); ++iF)
	{
		if (iF->exists())
		{
			invalidFilename = false;

			if (preferredDescriptionType & TYPE_PERMANENT)
			{
				for (const LibraryCounterPair& libraryCounterPair : libraries_)
				{
					if (libraryCounterPair.first->descriptionType() & TYPE_PERMANENT)
					{
						const SDXSceneRef scene(libraryCounterPair.first->load((*iF)(), file.extension(), engine, timestamp, TYPE_PERMANENT, progress, cancel));

						if (scene)
						{
							const ScopedLock scopedLock(managerLock_);

							permanentSceneMap_[scene->sceneId()] = scene;
							return scene;
						}
					}

				}
			}

			for (const LibraryCounterPair& libraryCounterPair : libraries_)
			{
				if (libraryCounterPair.first->descriptionType() & TYPE_TRANSIENT)
				{
					const SceneRef scene(libraryCounterPair.first->load((*iF)(), file.extension(), engine, timestamp, TYPE_TRANSIENT, progress, cancel));

					if (scene)
					{
						return scene;
					}
				}
			}
		}
	}

	if (invalidFilename)
	{
		Log::error() << "Failed to load file: \"" << filename << "\".";
	}

	return SceneRef();
}

bool Manager::unload(const SceneId sceneId)
{
	const ScopedLock scopedLock(managerLock_);

	const PermanentSceneMap::iterator i = permanentSceneMap_.find(sceneId);

	if (i == permanentSceneMap_.end())
	{
		return false;
	}

	permanentSceneMap_.erase(i);
	return true;
}

void Manager::unloadScenes()
{
	const ScopedLock scopedLock(managerLock_);

	// releasing the scenes in reverse order

	while (!permanentSceneMap_.empty())
	{
		permanentSceneMap_.erase(permanentSceneMap_.rbegin()->first);
	}
}

Manager::LibraryNames Manager::libraries()
{
	const ScopedLock scopedLock(libraryLock_);

	LibraryNames names;
	names.reserve(libraries_.size());

	for (Libraries::const_iterator i = libraries_.begin(); i != libraries_.end(); ++i)
	{
		names.emplace_back(i->first->name());
	}

	return names;
}

Manager::FileExtensions Manager::supportedExtensions()
{
	const ScopedLock scopedLock(libraryLock_);

	FileExtensions extensions;

	for (Libraries::const_iterator i = libraries_.begin(); i != libraries_.end(); ++i)
	{
		const Library::FileExtensions libraryExtensions(i->first->registeredFileExtensions());

		for (Library::FileExtensions::const_iterator iL = libraryExtensions.begin(); iL != libraryExtensions.end(); ++iL)
		{
			extensions.emplace(iL->first, iL->second);
		}
	}

	return extensions;
}

void Manager::mouseEvent(const ButtonType button, const ButtonEvent buttonEvent, const Vector2& screenPosition, const Vector3& objectPosition, const Rendering::ObjectId objectId, const Timestamp timestamp)
{
	const ScopedLock scopedLock(managerLock_);

	for (EventNodes::const_iterator i = eventNodes_.begin(); i != eventNodes_.end(); ++i)
	{
		ocean_assert(i->second);

		i->second->onMouse(button, buttonEvent, screenPosition, objectPosition, objectId, timestamp);
	}
}

void Manager::keyEvent(const int key, const ButtonEvent buttonEvent, const Rendering::ObjectId objectId, const Timestamp timestamp)
{
	const ScopedLock scopedLock(managerLock_);

	for (EventNodes::const_iterator i = eventNodes_.begin(); i != eventNodes_.end(); ++i)
	{
		ocean_assert(i->second);

		i->second->onKey(key, buttonEvent, objectId, timestamp);
	}
}

Timestamp Manager::preUpdate(const Rendering::ViewRef& view, const Timestamp timestamp)
{
	const ScopedLock scopedLock(managerLock_);

	Timestamp realTimestamp(timestamp);

	for (UpdateNodes::const_iterator i = updateNodes_.begin(); i != updateNodes_.end(); ++i)
	{
		ocean_assert(i->second);
		realTimestamp = i->second->onPreUpdate(view, realTimestamp);
	}

	ocean_assert(realTimestamp.isValid());
	return realTimestamp;
}

Timestamp Manager::preUpdate(const std::string& library, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	const ScopedLock scopedLock(managerLock_);

	Timestamp realTimestamp(timestamp);

	for (UpdateNodes::const_iterator i = updateNodes_.begin(); i != updateNodes_.end(); ++i)
	{
		ocean_assert(i->second);

		if (i->second->library() == library)
		{
			realTimestamp = i->second->onPreUpdate(view, realTimestamp);
		}
	}

	ocean_assert(realTimestamp.isValid());
	return realTimestamp;
}

void Manager::update(const Rendering::ViewRef& view, const Timestamp timestamp)
{
	const ScopedLock scopedLock(managerLock_);

	for (UpdateNodes::const_iterator i = updateNodes_.begin(); i != updateNodes_.end(); ++i)
	{
		ocean_assert(i->second);
		i->second->onUpdate(view, timestamp);
	}
}

void Manager::update(const std::string& library, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	const ScopedLock scopedLock(managerLock_);

	for (UpdateNodes::const_iterator i = updateNodes_.begin(); i != updateNodes_.end(); ++i)
	{
		ocean_assert(i->second);

		if (i->second->library() == library)
		{
			i->second->onUpdate(view, timestamp);
		}
	}
}

NodeRef Manager::node(const std::string& name) const
{
	const ScopedLock scopedLock(managerLock_);

	for (Libraries::const_iterator i = libraries_.cbegin(); i != libraries_.cend(); ++i)
	{
		const NodeRef node(i->first->node(name));

		if (node)
		{
			return node;
		}
	}

	return NodeRef();
}

NodeRef Manager::node(const std::string& library, const std::string& name) const
{
	const ScopedLock scopedLock(managerLock_);

	for (Libraries::const_iterator i = libraries_.cbegin(); i != libraries_.cend(); ++i)
	{
		if (i->first->name() == library)
		{
			const NodeRef node(i->first->node(name));

			if (node)
			{
				return node;
			}

			break;
		}
	}

	return NodeRef();
}

NodeRefs Manager::nodes(const std::string& name) const
{
	const ScopedLock scopedLock(managerLock_);

	NodeRefs nodes;

	for (Libraries::const_iterator i = libraries_.cbegin(); i != libraries_.cend(); ++i)
	{
		const NodeRefs subNodes(i->first->nodes(name));
		nodes.insert(nodes.end(), subNodes.begin(), subNodes.end());
	}

	return nodes;
}

NodeRefs Manager::nodes(const std::string& library, const std::string& name) const
{
	const ScopedLock scopedLock(managerLock_);

	for (Libraries::const_iterator i = libraries_.cbegin(); i != libraries_.cend(); ++i)
	{
		if (i->first->name() == library)
		{
			return i->first->nodes(name);
		}
	}

	return NodeRefs();
}

void Manager::release()
{
	const ScopedLock scopedLock(managerLock_);
	const ScopedLock lock(libraryLock_);

	Log::debug() << "Unregistering all " << libraries_.size() << " scene description libraries via Manager::release()";

	permanentSceneMap_.clear();

	for (Libraries::iterator i = libraries_.begin(); i != libraries_.end(); ++i)
	{
		i->first->release();
	}

	libraries_.clear();
}

bool Manager::unregisterLibrary(const std::string& name)
{
	Log::debug() << "Unregistering scene description library '" << name << "'";

	const ScopedLock mLock(managerLock_);
	const ScopedLock lLock(libraryLock_);

	for (Libraries::iterator i = libraries_.begin(); i != libraries_.end(); ++i)
	{
		ocean_assert(i->first);

		if (i->first->name() == name)
		{
			ocean_assert(i->second >= 1u);
			i->second--;

			if (i->second == 0u)
			{
				libraries_.erase(i);

				Log::debug() << "Successfully unregistered scene description library '" << name << "'";

				return true;
			}
			else
			{
				return false;
			}
		}
	}

	ocean_assert(false && "Library unknown!");

	return false;
}

void Manager::registerEventNode(SDXEventNode& node)
{
	const ScopedLock scopedLock(managerLock_);

	eventNodes_[node.id()] = &node;
}

void Manager::unregisterEventNode(SDXEventNode& node)
{
	const ScopedLock scopedLock(managerLock_);

	eventNodes_.erase(node.id());
}

void Manager::registerUpdateNode(SDXUpdateNode& node)
{
	const ScopedLock scopedLock(managerLock_);

	updateNodes_[node.id()] = &node;
}

void Manager::unregisterUpdateNode(SDXUpdateNode& node)
{
	const ScopedLock scopedLock(managerLock_);

	updateNodes_.erase(node.id());
}

}

}
