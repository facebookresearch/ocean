/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/system/Performance.h"

#include "ocean/base/Build.h"
#include "ocean/base/Processor.h"

namespace Ocean
{

namespace System
{

Performance::Performance()
{
	const unsigned int cores = Processor::get().cores();

	if (Build::architectureType() == "x64" || Build::architectureType() == "x86")
	{
		if (cores >= 16u)
		{
			performanceLevel_ = LEVEL_ULTRA;
		}
		else if (cores >= 8u)
		{
			performanceLevel_ = LEVEL_VERY_HIGH;
		}
		else if (cores >= 4u)
		{
			performanceLevel_ = LEVEL_HIGH;
		}
	}
	else
	{
		performanceLevel_ = LEVEL_LOW;

		if (cores >= 8u)
		{
			performanceLevel_ = LEVEL_VERY_HIGH;
		}
		else if (cores >= 4u)
		{
			performanceLevel_ = LEVEL_HIGH;
		}
		else if (cores >= 2u)
		{
			performanceLevel_ = LEVEL_MEDIUM;
		}
	}
}

Performance::PerformanceLevel Performance::performanceLevel()
{
	return performanceLevel_;
}

void Performance::setPerformanceLevel(const PerformanceLevel level)
{
	performanceLevel_ = level;
}

}

}
