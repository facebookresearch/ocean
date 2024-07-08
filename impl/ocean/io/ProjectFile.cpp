/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/ProjectFile.h"
#include "ocean/io/FileResolver.h"

namespace Ocean
{

namespace IO
{

ProjectFile::ProjectFile(const std::string& filename) :
	FileConfig(filename),
	fileExists_(false)
{
	File projectFile(filename);

	for (unsigned int n = 0; n < values("Unit"); ++n)
	{
		Value& unit = value("Unit", n);

		for (unsigned int i = 0; i < unit.values("File"); ++i)
		{
			Value& sceneFile = unit.value("File", i);

			if (sceneFile.type() == TYPE_STRING)
			{
				const File file(sceneFile(""));

				if (file().empty() == false)
				{
					const Files resolvedFiles(FileResolver::get().resolve(file, projectFile));

					if (resolvedFiles.empty() == false)
					{
						filenames_.emplace_back(resolvedFiles.front()());
					}
				}
			}
		}
	}

	fileExists_ = projectFile.exists();
}

}

}
