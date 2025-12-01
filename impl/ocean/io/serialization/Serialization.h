/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_SERIALIZATION_SERIALIZATION_H
#define META_OCEAN_IO_SERIALIZATION_SERIALIZATION_H

#include "ocean/io/IO.h"

namespace Ocean
{

namespace IO
{

namespace Serialization
{

/**
 * @ingroup io
 * @defgroup ioserialization Ocean IO Serialization Library
 * @{
 * The Ocean IO Serialization Library provides functionalities for data serialization and deserialization.
 * The library supports streaming binary data with timestamps and channels for recording and playback.
 * @}
 */

/**
 * @namespace Ocean::IO::Serialization
 * @brief Namespace of the Ocean IO Serialization library.
 *
 * The Namespace Ocean::IO::Serialization is used in the entire Ocean IO Serialization library.
 */

// Defines OCEAN_IO_SERIALIZATION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_IO_SERIALIZATION_EXPORT
		#define OCEAN_IO_SERIALIZATION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_IO_SERIALIZATION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_IO_SERIALIZATION_EXPORT
#endif

}

}

}

#endif // META_OCEAN_IO_SERIALIZATION_SERIALIZATION_H
