/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TEST_H
#define META_OCEAN_TEST_TEST_H

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"

#ifdef OCEAN_USE_GTEST
	#include <gtest/gtest.h>
#endif

namespace Ocean
{

namespace Test
{

/**
 * @defgroup test Ocean Test Library
 * @{
 * The Ocean Test Library provides several function to test the performance and validation of the entire ocean functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test Namespace of the Test library.<p>
 * The Namespace Ocean::Test is used in the entire Ocean Test Library.
 */

// The default test duration for gtest in seconds we select 0.5 seconds to keep a test as short as possible (while still receiving a meaningful result)
#define GTEST_TEST_DURATION 0.5

// The default image size for gtest in pixels. For mobile (Android and iOS), we use smaller values because Android instrumentation tests tend to time-out if the images are "too large".
#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY) || defined(OCEAN_PLATFORM_BUILD_ANDROID)
	#define GTEST_TEST_IMAGE_WIDTH 320u
	#define GTEST_TEST_IMAGE_HEIGHT 180u
#else
	#define GTEST_TEST_IMAGE_WIDTH 1920u
	#define GTEST_TEST_IMAGE_HEIGHT 250u
#endif

// Defines OCEAN_TEST_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_EXPORT
		#define OCEAN_TEST_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_EXPORT
#endif

}

}

#endif // META_OCEAN_TEST_TEST_H
