/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_MAPS_MAPS_H
#define META_OCEAN_IO_MAPS_MAPS_H

#include "ocean/io/IO.h"

namespace Ocean
{

namespace IO
{

namespace Maps
{

/**
 * @ingroup io
 * @defgroup iomaps Ocean IO Maps Library
 * @{
 * The Ocean IO Maps Library provides functions to read maps.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::IO::Maps Namespace of the Ocean IO Maps library<p>
 * The Namespace Ocean::IO::Maps is used in the entire IO Ocean Maps library.
 */

// Defines OCEAN_IO_MAPS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_IO_MAPS_EXPORT
		#define OCEAN_IO_MAPS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_IO_MAPS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_IO_MAPS_EXPORT
#endif

}

}

}

#endif // META_OCEAN_IO_MAPS_MAPS_H
