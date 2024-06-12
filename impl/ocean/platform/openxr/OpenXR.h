/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_OPENXR_OPEN_XR_H
#define META_OCEAN_PLATFORM_OPENXR_OPEN_XR_H

#include "ocean/platform/Platform.h"

#include "ocean/base/ScopedObject.h"

#include <openxr/openxr.h>

namespace Ocean
{

namespace Platform
{

namespace OpenXR
{

/**
 * @ingroup platform
 * @defgroup platformopenxr Ocean Platform OpenXR Library
 * @{
 * The Ocean OpenXR Library provides specific functionalities for OpenXR.
 * The library is available on platforms supporting OpenXR.
 * @}
 */

/**
 * @namespace Ocean::Platform::OpenXR Namespace of the Platform OpenXR library.<p>
 * The Namespace Ocean::Platform::OpenXR is used in the entire Ocean Platform OpenXR Library.
 */

// Defines OCEAN_PLATFORM_OPENXR_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_PLATFORM_OPENXR_EXPORT
		#define OCEAN_PLATFORM_OPENXR_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_PLATFORM_OPENXR_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_PLATFORM_OPENXR_EXPORT
#endif

/**
 * Wrapper function for xrDestroySpace.
 * @param xrSpace The object to destroy
 * @return The return value
 * @ingroup platformopenxr
 */
XrResult xrDestroySpaceOcean(XrSpace xrSpace);

/**
 * Definition of a scoped object holding a XrSpace object.
 * The wrapped XrSpace object will be released automatically once the scoped object does not exist anymore.
 * @ingroup platformopenxr
 */
using ScopedXrSpace = ScopedObjectCompileTimeT<XrSpace, XrSpace, XrResult, xrDestroySpaceOcean, XR_SUCCESS>;

/**
 * Definition of a vector holding OpenXR XrVector2f objects.
 * @see XrVector2f.
 * @ingroup platformopenxr
 */
using XrVectors2f = std::vector<XrVector2f>;

/**
 * Definition of a vector holding OpenXR XrVector3f objects.
 * @see XrVector3f.
 * @ingroup platformopenxr
 */
using XrVectors3f = std::vector<XrVector3f>;

/**
 * Definition of a vector holding OpenXR XrVector4f objects.
 * @see XrVector4f.
 * @ingroup platformopenxr
 */
using XrVectors4f = std::vector<XrVector4f>;

/**
 * Definition of a vector holding OpenXR XrVector4sFB objects.
 * @see XrVector4sFB.
 * @ingroup platformopenxr
 */
using XrVectors4sFB = std::vector<XrVector4sFB>;

/**
 * Definition of a vector holding OpenXR XrPosef objects.
 * @see XrPosef.
 * @ingroup platformopenxr
 */
using XrPosesf = std::vector<XrPosef>;

/**
 * Definition of a vector holding OpenXR XrHandJointEXT objects.
 * @see XrHandJointEXT.
 * @ingroup platformopenxr
 */
using XrHandJointsEXT = std::vector<XrHandJointEXT>;

/**
 * Definition of a vector holding OpenXR XrPath objects.
 * @see XrPath.
 * @ingroup platformopenxr
 */
using XrPaths = std::vector<XrPath>;

}

}

}

#endif // META_OCEAN_PLATFORM_OPENXR_OPEN_XR_H
