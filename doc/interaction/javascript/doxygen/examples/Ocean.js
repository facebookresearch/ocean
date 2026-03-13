/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * Author: Jan Herling, janherling@meta.com
 * This JavaScript file test the ocean scripting support for the ocean base object.
 */

/**
 * Initialization function which is called once to initialize this script.
 */
function onOceanInitialize()
{
	ocean.info("JavaScript Ocean test: ");
	ocean.info(" ");

	ocean.info("Platform type: ", ocean.platformType());
	ocean.info("Architecture type: ", ocean.architectureType());
	ocean.info("Release type: ", ocean.releaseType());
	ocean.info("Compiler version: ", ocean.compilerVersion());
	ocean.info(" ");

	var theValue = "the";
	var twoValue = 2;

	ocean.info("This a information message showing ", theValue, " usage of ", twoValue, " variables (not of ", 3, ")");

	ocean.warning("This could be a warning message...");
	ocean.error("This could be an error message...");
}
