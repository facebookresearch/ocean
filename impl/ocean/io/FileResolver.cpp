/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/FileResolver.h"
#include "ocean/io/File.h"

namespace Ocean
{

namespace IO
{

FileResolver::FileResolver()
{
	// nothing to do here
}

bool FileResolver::addReferencePath(const Directory& reference)
{
	if (!reference.isAbsolute())
		return false;

	referencePaths.push_back(reference);
	return true;
}

Files FileResolver::resolve(const File& file, const bool checkExistence) const
{
	if (file.isAbsolute())
	{
		if (!checkExistence || file.exists())
			return Files(1, file);
	}

	Files files;

	for (Directories::const_iterator i = referencePaths.begin(); i != referencePaths.end(); ++i)
	{
		const IO::File resolvedFile(*i + file);

		if (!checkExistence || resolvedFile.exists())
			files.push_back(*i + file);
	}

	return files;
}

Files FileResolver::resolve(const Files& files, const bool checkExistence) const
{
	Files results;

	for (Files::const_iterator i = files.begin(); i != files.end(); ++i)
	{
		const Files result(resolve(*i, checkExistence));
		results.insert(results.end(), result.begin(), result.end());
	}

	return results;
}

Files FileResolver::resolve(const File& file, const Path& reference, const bool checkExistence) const
{
	if (file.isAbsolute())
	{
		if (!checkExistence || file.exists())
			return Files(1, file);
	}

	Files result;

	Directory refDirectory(reference);

	if (refDirectory.isAbsolute())
	{
		const IO::File refFile(refDirectory + file);

		if (!checkExistence || refFile.exists())
			result.push_back(refFile);
	}

	for (Directories::const_iterator i = referencePaths.begin(); i != referencePaths.end(); ++i)
	{
		const IO::File resolvedFile(*i + file);

		if (!checkExistence || resolvedFile.exists())
			result.push_back(*i + file);
	}

	if (result.empty())
	{
		const IO::File defaultFile(file);

		if (!checkExistence || defaultFile.exists())
			result.push_back(defaultFile);
	}

	return result;
}

Files FileResolver::resolve(const Files& files, const Path& reference, const bool checkExistence) const
{
	Files results;

	for (Files::const_iterator i = files.begin(); i != files.end(); ++i)
	{
		const Files result(resolve(*i, reference, checkExistence));
		results.insert(results.end(), result.begin(), result.end());
	}

	return results;
}

}

}
