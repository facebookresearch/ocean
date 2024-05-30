/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_MAPTEXTURING_TEXTUREGENERATOR_TEXTURE_GENERATOR_MAIN_H
#define META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_MAPTEXTURING_TEXTUREGENERATOR_TEXTURE_GENERATOR_MAIN_H

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

#include "ocean/devices/DevicePlayer.h"

/**
 * @ingroup applicationdemotracking
 * @defgroup applicationdemotrackingmaptexturingtexturegenerator Texture Generator
 * @{
 * The demo application demonstrates how to create a textured mesh based on a mono view camera.
 * This application is platform independent.
 * @}
 */

using namespace Ocean;

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER

/**
 * Creates a device player.
 * @return The resulting device player, nullptr if the player could not be created
 */
Devices::SharedDevicePlayer TextureGeneratorMain_createExternalDevicePlayer();

#endif // OCEAN_USE_EXTERNAL_DEVICE_PLAYER

#endif // META_OCEAN_APPLICATION_OCEAN_DEMO_TRACKING_MAPTEXTURING_TEXTUREGENERATOR_TEXTURE_GENERATOR_MAIN_H
