/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/Library.h"
#include "ocean/scenedescription/Manager.h"
#include "ocean/scenedescription/Node.h"

#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace SceneDescription
{

Library::Library(const std::string& name, const DescriptionType descriptionType, const unsigned int priority) :
	name_(name),
	priority_(priority),
	descriptionType_(descriptionType)
{
	// nothing to do here
}

Library::~Library()
{
	// nothing to do here
}

SceneRef Library::load(const std::string& filename, const std::string& fileExtension, const Rendering::EngineRef& engine, const Timestamp& timestamp, const DescriptionType descriptionType, float* progress, bool* cancel)
{
	ocean_assert(descriptionType & descriptionType_);

	if (fileExtension.empty() == false && fileExtensions_.find(String::toLower(fileExtension)) == fileExtensions_.end())
	{
		return SceneRef();
	}

	SceneRef scene;
	Timestamp startTimestamp(true);

	if (descriptionType == TYPE_PERMANENT)
	{
		ocean_assert(engine);

		if (engine)
		{
			scene = loadPermanent(filename, fileExtension, engine, timestamp, progress, cancel);
		}
	}
	else
	{
		scene = loadTransient(filename, fileExtension, progress, cancel);
	}

	Timestamp stopTimestamp(true);

	if (scene.isNull())
	{
		Log::error() << "Failure during load process of \"" << filename << "\".";
	}
	else
	{
		Log::info() << "Successfully loaded \"" << filename << "\" in " << double(stopTimestamp - startTimestamp) << " seconds.";
	}

	return scene;
}

NodeRef Library::node(const std::string& name) const
{
	return nodeRefManager_.node(name);
}

NodeRefs Library::nodes(const std::string& name) const
{
	return nodeRefManager_.nodes(name);
}

void Library::release()
{
	// nothing to do here
}

void Library::registerFileExtension(const std::string& extension, const std::string& description)
{
	ocean_assert(String::toLower(extension) == extension);
	ocean_assert(fileExtensions_.find(extension) == fileExtensions_.end());

	fileExtensions_.emplace(extension, description);
}

SceneRef Library::loadPermanent(const std::string& /*filename*/, const std::string& /*fileExtension*/, const Rendering::EngineRef& /*engine*/, const Timestamp& /*timestamp*/, float* /*progress*/, bool* /*cancel*/)
{
	// this function an be overwritten in derived classes

	ocean_assert(false && "This function is not supported and should not be used!");
	return SceneRef();
}

SceneRef Library::loadTransient(const std::string& /*filename*/, const std::string& /*fileExtension*/, float* /*progress*/, bool* /*cancel*/)
{
	// this function an be overwritten in derived classes

	ocean_assert(false && "This function is not supported and should not be used!");
	return SceneRef();
}

bool Library::unregisterLibrary(const std::string& library)
{
	return Manager::get().unregisterLibrary(library);
}

}

}
