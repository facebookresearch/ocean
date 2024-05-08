/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_INTERACTION_H
#define META_OCEAN_INTERACTION_INTERACTION_H

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"

namespace Ocean
{

namespace Interaction
{

/**
 * @defgroup interaction Ocean Interaction Abstraction Library
 * @{
 * The Ocean Interaction Library is the base library and organizer of all interaction libraries / plugins.<br>
 * In major, this library holds abstract objects only and therefore is used as an interface to provide access to different interaction plugins.<br>
 * @}
 */

/**
 * @namespace Ocean::Interaction Namespace of the Interaction library.<p>
 * The Namespace Ocean::Interaction is used in the entire Ocean Interaction Library.
 */

// Defines OCEAN_INTERACTION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_INTERACTION_EXPORT
		#define OCEAN_INTERACTION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_INTERACTION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_INTERACTION_EXPORT
#endif

} // namespace Interaction

} // namespace Ocean

#endif // META_OCEAN_INTERACTION_INTERACTION_H
