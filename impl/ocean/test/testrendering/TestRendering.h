/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTRENDERING_TESTRENDERING_H
#define META_OCEAN_TEST_TESTRENDERING_TESTRENDERING_H

#include "ocean/test/Test.h"

#include "ocean/rendering/Rendering.h"

#include "ocean/base/Worker.h"

namespace Ocean
{

namespace Test
{

namespace TestRendering
{

/**
 * @ingroup test
 * @defgroup testrendering Ocean Test Rendering Library
 * @{
 * The Ocean Test Rendering Library provides several function to test the performance and validation of rendering functions.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Test::TestRendering Namespace of the Rendering Test library.<p>
 * The Namespace Ocean::Test::TestRendering is used in the entire Ocean Rendering Test Library.
 */

// Defines OCEAN_TEST_RENDERING_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_TEST_RENDERING_EXPORT
		#define OCEAN_TEST_RENDERING_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_TEST_RENDERING_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_TEST_RENDERING_EXPORT
#endif

/**
 * Tests the entire rendering library.
 * @param testDuration Number of seconds for each test
 * @param worker The worker object to distribute some computation on as many CPU cores as defined in the worker object.
 * @return True, if the entire test succeeded
 */
OCEAN_TEST_RENDERING_EXPORT bool testRendering(const double testDuration, Worker& worker);

}

}

}

#endif // META_OCEAN_TEST_TESTRENDERING_TESTRENDERING_H
