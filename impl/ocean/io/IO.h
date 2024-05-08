/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_IO_IO_H
#define META_OCEAN_IO_IO_H

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"

namespace Ocean
{

namespace IO
{

/**
 * @defgroup io Ocean IO Library
 * @{
 * The Ocean IO Library provides all base functionalities needed for input and output operations.
 * The library provides a binary bitstream, a scanner, and file and directory handling functionalities.<p>
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::IO Namespace of the Ocean IO library<p>
 * The Namespace Ocean::IO is used in the entire Ocean IO library.
 */

// Defines OCEAN_IO_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_IO_EXPORT
		#define OCEAN_IO_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_IO_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_IO_EXPORT
#endif

}

}

#endif // META_OCEAN_IO_IO_H
