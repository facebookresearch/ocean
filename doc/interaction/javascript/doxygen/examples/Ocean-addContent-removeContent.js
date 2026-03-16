/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * Author: Jan Herling, janherling@meta.com
 * This JavaScript file showing the usage of the addContent() and removeContent() function.
 * This example shows how to load and unload content files (e.g. scene descriptions, interaction files, or entire project files) with the scripting interaction compontent.
 * Two further files are neccessary to run this examples ("projectA.oproj" and "projectB.oproj").
 * The content of these project files can be arbitrary.
 */

/**
 * Initialization function which is called once to initialize this script.
 * @param engine Object providing access to the currently used rendering engine
 * @param timestmap Initialization timestamp, which is a standard unix timestamp
 */
function onOceanInitialize(engine, timestamp)
{
	ocean.info("JavaScript Ocean test for Ocean::addContent() and Ocean::removeContent().");
}

/**
 * Event function of key press events.
 * @param engine Object providing access to the currenlty used rendering engine
 * @param key Key which has been pressed
 * @param timestamp Event timestamp
 */
function onOceanKeyPress(engine, key, timestamp)
{
	// check whether the pressed key is an 'A'
	if (key == "A")
	{
		// adds a new project (with name "projectA.oproj") to the environment
		ocean.addContent("projectA.oproj");
	}

	// check whether the pressed key is a 'B'
	if (key == "B")
	{
		// adds a new project (with name "projectB.oproj") to the environment
		ocean.addContent("projectB.oproj");
	}

	// check whether the pressed key is a 'C'
	if (key == "C")
	{
		// removes an already added project (with name "projectA.oproj") from the environment
		ocean.removeContent("projectA.oproj");
	}

	// check whether the pressed key is a 'D'
	if (key == "D")
	{
		// removes an already added project (with name "projectB.oproj") from the environment
		ocean.removeContent("projectB.oproj");
	}

	// check whether the pressed key is an 'E'
	if (key == "E")
	{
		// removes both already added projects (with name "projectA.oproj" and "projectB.oproj") from the environment
		ocean.removeContent("projectA.oproj", "projectB.oproj");
	}

	// check whether the pressed key is an 'F'
	if (key == "F")
	{
		// removes all aready loaded content form the environment
		// Beware: also this calling script will be unloaded and thus no further operations can be done with this script
		ocean.removeContent();
	}
}
