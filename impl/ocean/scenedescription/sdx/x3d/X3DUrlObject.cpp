/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DUrlObject.h"

#include "ocean/io/File.h"
#include "ocean/io/FileResolver.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DUrlObject::X3DUrlObject(const SDXEnvironment* environment) :
	SDXNode(environment)
{
	// nothing to do here
}

void X3DUrlObject::registerFields(NodeSpecification& specification)
{
	registerField(specification, "url", url_);
}

IO::Files X3DUrlObject::resolveUrls(const std::string& filename, const MultiString::Values& urls)
{
	IO::Files files;
	files.reserve(urls.size());

	for (const std::string& url : urls)
	{
		files.push_back(IO::File(url));
	}

	IO::Files resolved = IO::FileResolver::get().resolve(files, IO::File(filename));
	resolved.insert(resolved.begin(), files.begin(), files.end());

	return resolved;
}

IO::Files X3DUrlObject::resolveUrls() const
{
	return resolveUrls(filename(), url_.values());
}

}

}

}

}
