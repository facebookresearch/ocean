/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SCENE_DESCRIPTION_H
#define META_OCEAN_SCENEDESCRIPTION_SCENE_DESCRIPTION_H

#include "ocean/base/Base.h"
#include "ocean/base/Messenger.h"

namespace Ocean
{

namespace SceneDescription
{

/**
 * @defgroup scenedescription Ocean SceneDescription Abstraction Library
 * @{
 * The Ocean SceneDescription Library is the base library and organizer of all scene description libraries.<br>
 * Several different scene description libraries can be registered and managed at the same time.<br>
 * Each library may support a different types of scene description files / specifications.<br>
 * The Manager object manages all registered libraries and hides the internal complexity.<br>
 * The library is platform independent.<br>
 * @see Manager, Scene
 * @}
 */

/**
 * @namespace Ocean::SceneDescription Namespace of the SceneDescription library.<p>
 * The Namespace Ocean::SceneDescription is used in the entire Ocean SceneDescription Library.
 */

/**
 * Definition of a object id.
 * @ingroup scenedescription
 */
typedef size_t NodeId;

/**
 * Definition of an invalid object id.
 * @ingroup scenedescription
 */
constexpr NodeId invalidNodeId = NodeId(-1);

/**
 * Definition of a unique scene id.
 */
typedef size_t SceneId;

/**
 * Definition of an invalid scene id.
 */
constexpr SceneId invalidSceneId = SceneId(-1);

/**
 * Definition of different scene description types.
 * @ingroup scenedescription
 */
enum DescriptionType : uint32_t
{
	/// A scene description holding a transient scene hierarchy only.
	TYPE_TRANSIENT = 1u,
	/// A scene description holding a permanent scene hierarchy allowing permanent access.
	TYPE_PERMANENT = 2u
};

/**
 * Definition of different button types.
 * @ingroup scenedescription
 */
enum ButtonType
{
	// Invalid button.
	BUTTON_NONE,
	/// Left button.
	BUTTON_LEFT,
	/// Middle button.
	BUTTON_MIDDLE,
	/// Right button.
	BUTTON_RIGHT
};

/**
 * Definition of different device events.
 * @ingroup scenedescription
 */
enum ButtonEvent
{
	/// Press event.
	EVENT_PRESS,
	/// Hold event.
	EVENT_HOLD,
	/// Release event.
	EVENT_RELEASE
};

// Defines OCEAN_SCENEDESCRIPTION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_SCENEDESCRIPTION_EXPORT
		#define OCEAN_SCENEDESCRIPTION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_SCENEDESCRIPTION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_SCENEDESCRIPTION_EXPORT
#endif

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SCENE_DESCRIPTION_H
