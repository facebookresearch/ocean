/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/Library.h"
#include "ocean/media/Manager.h"

namespace Ocean
{

namespace Media
{

Library::Library(const std::string& name, const unsigned int priority) :
	libraryName(name),
	libraryPriority(priority)
{
	// nothing to do here
}

Library::~Library()
{
	// nothing to do here
}

Library::Definitions Library::selectableMedia() const
{
	return Definitions();
}

Library::Definitions Library::selectableMedia(const Medium::Type /*type*/) const
{
	// must be implemented in a derived class

	return Definitions();
}

bool Library::supports(const Medium::Type type) const
{
	return (supportedTypes() & type) == type;
}

Medium::Type Library::supportedTypes() const
{
	return Medium::MEDIUM;
}

bool Library::notSupported(const std::string& extension) const
{
	return notSupportedExtensionSet.find(extension) != notSupportedExtensionSet.end();
}

std::string Library::convertDefinition(const Definition& definition)
{
	std::string result("/");

	if (definition.library().empty())
	{
		result += "Any/";
	}
	else
	{
		result += definition.library() + std::string("/");
	}

	result += Medium::convertType(definition.type()) + std::string("/");

	result += definition.url();
	return result;
}

Library::Definition Library::convertDefinition(const std::string& string)
{
	std::string url;
	std::string library;
	Medium::Type type = Medium::MEDIUM;

	if (string.find('/') == 0)
	{
		std::string::size_type start = 1;
		std::string::size_type pos = string.find('/', start);

		if (pos != std::string::npos)
		{
			library = string.substr(1, pos - start);

			start = pos + 1;
			pos = string.find('/', start);

			if (pos != std::string::npos)
			{
				const std::string typeString = string.substr(start, pos - start);

				type = Medium::convertType(typeString);

				start = pos + 1;

				url = string.substr(start);
			}
		}
	}

	return Definition(std::move(url), type, std::move(library));
}

void Library::registerNotSupportedExtension(const std::string& extension)
{
	ocean_assert(extension.empty() == false);
	notSupportedExtensionSet.insert(extension);
}

}

}
