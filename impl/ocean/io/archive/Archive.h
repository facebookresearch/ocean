// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_IO_ARCHIVE_ARCHIVE_H
#define META_OCEAN_IO_ARCHIVE_ARCHIVE_H

#include "ocean/io/IO.h"

namespace Ocean
{

namespace IO
{

namespace Archive
{

/**
 * @ingroup io
 * @defgroup ioarchive Ocean Archive Image Library
 * @{
 * The Ocean IO Archive Library provides functionalities to read and write archives (e.g., zip archives).
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::IO::Archive Namespace of the Ocean IO Archive library<p>
 * The Namespace Ocean::IO::Archive is used in the entire Ocean IO Archive library.
 */

// Defines OCEAN_IO_ARCHIVE_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_IO_ARCHIVE_EXPORT
		#define OCEAN_IO_ARCHIVE_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_IO_ARCHIVE_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_IO_ARCHIVE_EXPORT
#endif

}

}

}

#endif // META_OCEAN_IO_ARCHIVE_ARCHIVE_H
