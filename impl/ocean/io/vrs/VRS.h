// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_IO_VRS_VRS_H
#define META_OCEAN_IO_VRS_VRS_H

#include "ocean/io/IO.h"

namespace Ocean
{

namespace IO
{

namespace VRS
{

/**
 * @ingroup io
 * @defgroup iovrs Ocean IO VRS Library
 * @{
 * The Ocean IO VRS Library provides VRS functionalities.
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::IO::VRS Namespace of the Ocean IO VRS library<p>
 * The Namespace Ocean::IO::VRS is used in the entire Ocean IO VRS library.
 */

// Defines OCEAN_IO_VRS_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_IO_VRS_EXPORT
		#define OCEAN_IO_VRS_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_IO_VRS_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_IO_VRS_EXPORT
#endif

}

}

}

#endif // META_OCEAN_IO_VRS_VRS_H
