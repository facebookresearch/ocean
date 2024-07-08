/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_MAPS_RENDERING_RENDERING_H
#define META_OCEAN_IO_MAPS_RENDERING_RENDERING_H

#include "ocean/io/maps/Maps.h"

namespace Ocean
{

namespace IO
{

namespace Maps
{

namespace Rendering
{

/**
 * @ingroup iomaps
 * @defgroup iomapsrendering Ocean IO Maps Rendering Library
 * @{
 * The Ocean IO Maps Rendering Library provides functions to render maps.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::IO::Maps::Rendering Namespace of the Ocean IO Maps Rendering library<p>
 * The Namespace Ocean::IO::Maps::Rendering is used in the entire IO Ocean Maps Rendering library.
 */

// Defines OCEAN_IO_MAPS_RENDERING_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_IO_MAPS_RENDERING_EXPORT
		#define OCEAN_IO_MAPS_RENDERING_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_IO_MAPS_RENDERING_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_IO_MAPS_RENDERING_EXPORT
#endif

}

}

}

}

#endif // META_OCEAN_IO_MAPS_RENDERING_RENDERING_H
