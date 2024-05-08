/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_APPLICATION_H
#define META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_APPLICATION_H

#include "ocean/platform/meta/Meta.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

namespace Application
{

/**
 * @ingroup platformmeta
 * @defgroup platformmetaquestopenxrapplication Ocean Platform Meta Quest OpenXR Application Library
 * @{
 * The Ocean Meta Quest OpenXR Application Library provides specific functionalities for Quest applications based on OpenXR.
 * The library is available on Quest platforms only.
 * @}
 */

/**
 * @namespace Ocean::Platform::Meta::Quest::OpenXR::Application Namespace of the Platform Meta Quest OpenXR Application library.<p>
 * The Namespace Ocean::Platform::Meta::Quest::OpenXR::Application is used in the entire Ocean Platform Meta Quest OpenXR Application Library.
 */

// Defines OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_EXPORT
		#define OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_EXPORT
#endif

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_OPENXR_APPLICATION_APPLICATION_H
