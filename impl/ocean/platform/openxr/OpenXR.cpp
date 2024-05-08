/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/openxr/OpenXR.h"

namespace Ocean
{

namespace Platform
{

namespace OpenXR
{

XrResult xrDestroySpaceOcean(XrSpace xrSpace)
{
	return xrDestroySpace(xrSpace);
}

}

}

}
