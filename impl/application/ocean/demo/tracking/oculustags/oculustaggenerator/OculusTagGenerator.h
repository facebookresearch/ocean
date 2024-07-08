/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_OCULUSTAG_OCULUSTAGGENERATOR_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_OCULUSTAG_OCULUSTAGGENERATOR_H

#include <cstdint>
#include <string>
#include <vector>

/**
 * Display a help text for this application
 * @param applicationName The name that will be used for this application, usually this is set to the value of `argv[0]`
 */
void displayUsage(const std::string& applicationName);

/**
 * Parse the command-line arguments and extract the necessary parameters for this application
 * @param argc The number of command-line arguments
 * @param argv Pointer to the command-line arguments
 * @param directory The name of the directory where the images of the Oculus tags will be stored
 * @param tagSize The size of the generated tags in pixels
 * @param extraBorder Extra space that is added around the tags before drawing
 * @param tagIDs A list of tag IDs that should be generated
 * @return True if all necessary parameters have been extracted from the command-line arguments and have valid values, otherwise false
 */
bool parseArguments(const int argc, char** argv, std::string& directory, uint32_t& tagSize, uint32_t& extraBorder, std::vector<uint16_t>& tagIDs);

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_OCULUSTAG_OCULUSTAGGENERATOR_H
